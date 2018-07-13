#include "stdafx.h"
#include "XClientSignaling.h"
#include "XDes.h"
#include "a_signaling_channel_json_reader_builder.h"

CXClientSignaling::CXClientSignaling(CXSignalingChannelMessageCallback *cb)
{
	m_hThread = CROSS_THREAD_NULL;
	m_pCallBack = cb;
	m_pTcpStream = NULL;
	m_bIsError = TRUE;
	m_pBuffer = CLittleBufferPool::Instance().malloc();
	m_pSignalingCtrlBuffer =  CLittleBufferPool::Instance().malloc();
#if XINTERNET_TEST_STATUS
	m_bCanReleaseSuccessCounter = FALSE;
#endif
}

CXClientSignaling::~CXClientSignaling()
{
	CLittleBufferPool::Instance().free(m_pBuffer);
	CLittleBufferPool::Instance().free(m_pSignalingCtrlBuffer);
}
void CXClientSignaling::OnPacketCompleteNetStreamData(int32_t bytesTransferred, emXStreamTansferKey transferKey)
{
	if (m_bIsError)
	{
		return;
	}
	if (bytesTransferred < 0)
	{
		m_bIsError = TRUE;
		return;
	}


	if (ENUM_XTSTREAM_TRANSKEY_READ == transferKey)
	{
		if (0 == bytesTransferred)
		{
			m_nRecvStep = NET_RECIEVE_STEP_HEAD;
			m_pTcpStream->AsyncRead(&m_head, sizeof(m_head));
			return;
		}
		else
		{
			if (m_nRecvStep == NET_RECIEVE_STEP_HEAD)
			{
				if (m_head.datalen > MIDDLE_BUFFER_SIZE_MB)
				{
					return;
				}
				if (m_head.datalen == 0)
				{
					//StreamDataDecryption(m_pBuffer);
					DoMsg();
					m_nRecvStep = NET_RECIEVE_STEP_HEAD;
					m_pTcpStream->AsyncRead(&m_head, sizeof(m_head));
				}
				else
				{
					m_nRecvStep = NET_RECIEVE_STEP_BODY;
					m_pTcpStream->AsyncRead(m_pBuffer, m_head.datalen);
				}
				return;
			}
			if (m_nRecvStep == NET_RECIEVE_STEP_BODY)
			{
				//StreamDataDecryption(m_pBuffer);
				m_pBuffer[m_head.datalen] = '\0';
				DoMsg();
				m_nRecvStep = NET_RECIEVE_STEP_HEAD;
				m_pTcpStream->AsyncRead(&m_head, sizeof(m_head));
				return;
			}
		}
	}
	if (ENUM_XTSTREAM_TRANSKEY_WRITE == transferKey)
	{
		if (m_msgSendList.size() > 0)
		{
			m_csSendList.Lock();
			ST_SIGNALING_SEND_BUFFER * pMsg = (ST_SIGNALING_SEND_BUFFER *)m_msgSendList.front();
			if (pMsg->head.datalen > 0)
			{
				int iiii = 0;
			}
			m_pTcpStream->AsyncWrite(pMsg, pMsg->head.datalen + sizeof(pMsg->head));//����
			m_msgSendList.pop_front();
			CSignalingBufferPool::Instance().free(pMsg);
			m_csSendList.Unlock();
		}
		else
		{
			m_pTcpStream->PostDelayWriteStatus();
		}
	}
}


void CXClientSignaling::Login()
{



#if XINTERNET_TEST_STATUS
		if (m_bCanReleaseSuccessCounter)
		{
			CXTestCounter::Instance().ReleaseSuccess();
			CXTestCounter::Instance().AddFailed();
			m_bCanReleaseSuccessCounter = FALSE;
		}
#endif


		if (m_pTcpStream)
		{
			m_pTcpStream->Release();
			m_pTcpStream = NULL;
		}

		m_bIsError = FALSE;
		m_dwLastTick = CrossGetTickCount64();
		BOOL bLoginOK = FALSE;
		ST_SIGNALING_SEND_BUFFER *pSendBuffer = CSignalingBufferPool::Instance().malloc();;
		signaling_channel_head_t msg_key_recv;
		do
		{
			//--------------------------------------------------------------------------------------------------------------------------
			//������Ϣ
			ClearSendBufferList();
			//--------------------------------------------------------------------------------------------------------------------------
			//���ӷ�����
			m_pTcpStream = XNetCreateStream4Connect(m_szCenterIp, s_nCenterPort, 10);
			if (0 != XNetConnectStream(m_pTcpStream))
			{
				//CROSS_TRACE("CMediaDevInterface::Login -- conn svr error");
				break;
			}
			//--------------------------------------------------------------------------------------------------------------------------
			//NEED_SESSIONID
			memset(pSendBuffer, 0, sizeof(ST_SIGNALING_SEND_BUFFER));
			pSendBuffer->head.cmd = SINGNALING_CHANNEL_CMD_LOGIN_NEED_SESSION;
			//
			if (0 != m_pTcpStream->SyncWriteAndRead(pSendBuffer, sizeof(signaling_channel_head_t), &msg_key_recv, sizeof(signaling_channel_head_t), 5))
			{
				//CROSS_TRACE("CMediaDevInterface::Login -- request key error -- 1");
				break;
			}
			//
			if ((msg_key_recv.cmd != SINGNALING_CHANNEL_CMD_LOGIN_NEED_SESSION) || (msg_key_recv.result != SINGNALING_CHANNEL_RESULT_OK))
			{
				//CROSS_TRACE("CMediaDevInterface::Login -- request key error -- 2");
				break;
			}
			//
			m_nSessionID = msg_key_recv.session;

			//--------------------------------------------------------------------------------------------------------------------------
			//ʹ��key���ܣ���½
			signaling_channel_user_login_t stLogin;
			memset(&stLogin, 0, sizeof(stLogin));
			XDESEncode(m_szUser, m_nSessionID, stLogin.userdes);
			XDESEncode(m_szPwd, m_nSessionID, stLogin.pwddes);
			//
			// 			char szSendBuffer[1024] = { 0 };
			// 			ST_SREVER_LOGIN_INFO msg_login_info;
			// 			ST_SERVER_HEAD msg_login_send;
			// 			ST_SERVER_HEAD msg_login_recv;
			memset(pSendBuffer, 0, sizeof(ST_SIGNALING_SEND_BUFFER));
			// 			memset(&msg_login_send, 0, sizeof(ST_SERVER_HEAD));
			// 			memset(&msg_login_recv, 0, sizeof(ST_SERVER_HEAD));
			// 			//
			pSendBuffer->head.cmd = SINGNALING_CHANNEL_CMD_LOGIN;
			pSendBuffer->head.session = m_nSessionID;
			pSendBuffer->head.datalen = sizeof(stLogin);
			// 			//
			// 			msg_login_info.dev_id = classic_dev_id;
			// 			msg_login_info.dev_channel = 0;
			// 			memcpy(msg_login_info.user, user_name.c_str(), user_name.length());
			// 			memcpy(msg_login_info.pwd, enc_pwd, strlen(enc_pwd));
			//
			memcpy(pSendBuffer->msg, &stLogin, sizeof(stLogin));
			//memcpy(szSendBuffer + sizeof(ST_SERVER_HEAD), &msg_login_info, sizeof(ST_SREVER_LOGIN_INFO));
			//
			if (0 != m_pTcpStream->SyncWriteAndRead(pSendBuffer, sizeof(signaling_channel_head_t) + sizeof(stLogin), &msg_key_recv, sizeof(signaling_channel_head_t)))
			{
				//CROSS_TRACE("CMediaDevInterface::Login -- login error -- 1");
				break;
			}
			//
			if ((msg_key_recv.cmd != SINGNALING_CHANNEL_CMD_LOGIN) || (msg_key_recv.result != SINGNALING_CHANNEL_RESULT_OK))
			{
				//CROSS_TRACE("CMediaDevInterface::Login -- login error -- 2");
				break;
			}
			//CROSS_TRACE("CMediaDevInterface::Login ------------------------- 6");
			m_pTcpStream->SetStreamData(this);
			//		m_dwLastTick = CrossGetTickCount64();
			bLoginOK = TRUE;

#if XINTERNET_TEST_STATUS
			CXTestCounter::Instance().ReleaseFailed();
			CXTestCounter::Instance().AddSuccess();
			m_bCanReleaseSuccessCounter = TRUE;
#endif
			//CROSS_TRACE("CMediaDevInterface::Login ------------------------- 7");
			//if (m_bUserOpenPreview)
			// 			{
			// 				AddSendMessge(Z_CMD_OF_SERVER_OPEN_STREAM, NULL, 0);
			// 			}
		} while (0);



		//CROSS_TRACE("CMediaDevInterface::Login ------------------------- 8");
		if (!bLoginOK)
		{
			if (m_pTcpStream)
			{
				m_pTcpStream->Release();
				m_pTcpStream = NULL;
			}
		}
}
void CXClientSignaling::Delete()
{
	Disconnect();
	if (CROSS_THREAD_NULL != m_hThread)
	{
		m_bTheadExit = TRUE;
		CrossWaitThread(m_hThread);
		CrossCloseThread(m_hThread);
		m_hThread = CROSS_THREAD_NULL;
	}

	delete this;
}
void CXClientSignaling::Connect(char * s_ip, int port, char * user, char * pwd)
{

	if (CROSS_THREAD_NULL == m_hThread)
	{
		memset(m_szCenterIp, 0, sizeof(m_szCenterIp));
		memset(m_szUser, 0, sizeof(m_szUser));
		memset(m_szPwd, 0, sizeof(m_szPwd));
		//
		memcpy(m_szCenterIp, s_ip, strlen(s_ip));
		memcpy(m_szUser, user, strlen(user));
		memcpy(m_szPwd, pwd, strlen(pwd));
		s_nCenterPort = port;
		//
		m_bTheadExit = FALSE;
		m_hThread = CrossCreateThread(ThreadWorker, this);

	}

}
void CXClientSignaling::Disconnect()
{
	if (m_pTcpStream)
	{
		m_pTcpStream->Release();
		m_pTcpStream = NULL;	
	}
	m_bIsError = TRUE;
	ClearSendBufferList();

}
BOOL CXClientSignaling::IsNetError()
{
	return m_bIsError;
}
// BOOL CXClientSignaling::IsNetError()
// {
// 	if ((CrossGetTickCount64() - m_dwLastTick) > 15000)
// 	{
// 		m_bIsError = TRUE;
// 	}
// 
// 	return m_bIsError;
// }
void CXClientSignaling::AddSendMessage(int type,char *buffer, int len)
{
	//
	ST_SIGNALING_SEND_BUFFER * pMsg = CSignalingBufferPool::Instance().malloc();
	//
	pMsg->head.cmd = type;
	pMsg->head.datalen = len;
	pMsg->head.session = m_nSessionID;

	if (len > 0)
	{
		memcpy(pMsg->msg, buffer, len);
	}
	//
	m_csSendList.Lock();
	m_msgSendList.push_back(pMsg);
	m_csSendList.Unlock();
	//CROSS_TRACE("pMsg->head.cmd = %d pMsg->head.datalen=%d", pMsg->head.cmd, pMsg->head.datalen);
};
void CXClientSignaling::ClearSendBufferList()
{
	m_csSendList.Lock();
	while (m_msgSendList.size() > 0)
	{
		ST_SIGNALING_SEND_BUFFER * p = m_msgSendList.front();
		m_msgSendList.pop_front();
		CSignalingBufferPool::Instance().free(p);
	}
	m_csSendList.Unlock();
}

void CXClientSignaling::DoMsg()
{
	m_dwLastTick = CrossGetTickCount64();

	if (m_head.cmd == SINGNALING_CHANNEL_CMD_HEARTBEAT)
	{
		//CROSS_TRACE("CXDeviceSignaling::DoMsg() -> SINGNALING_CHANNEL_CMD_HEARBEAT");
	}
	if (m_head.cmd == SINGNALING_CHANNEL_CMD_ALARM)
	{
		//CROSS_TRACE(m_pBuffer);
	}
// 	if (m_head.cmd == SINGNALING_CHANNEL_CMD_TRANS)
// 	{
// 		AddSendMessage(m_head.cmd, m_pBuffer, m_head.datalen - 5);
// 	}

	if (m_head.cmd == SINGNALING_CHANNEL_CMD_TRANS)
	{
		//CROSS_TRACE(m_pBuffer);


		string command;
		int result;
		string deviceuuid;
		string msguuid;
		//
		if (!JsonGetHeaderInfo((char *)m_pBuffer, command, result, deviceuuid, msguuid))
		{
			return;
		}
		//
		if (0 == msguuid.compare(m_strSignalingCtrlMsgUuid))
		{
			memcpy(m_pSignalingCtrlBuffer, m_pBuffer, m_head.datalen);
			m_nSignalingCtrlBufferLen = m_head.datalen;
			m_bSignalingCtrlFinished = TRUE;
			return;
		}

	}

	if (m_head.cmd > SINGNALING_CHANNEL_CMD_LOGOUT)
	{
		m_pCallBack->OnSignalingChannelMessageCallback(m_pBuffer, m_head.datalen);
	}
};


int CXClientSignaling::ThreadWorker(void * param)
{
	CXClientSignaling * p = (CXClientSignaling *)param;
	p->Woker();
	return 0;
}
void CXClientSignaling::Woker()
{
#if XINTERNET_TEST_STATUS
	WaitForSingleObject(g_hStartDoing, INFINITE);
#endif

	while (FALSE == m_bTheadExit)
	{


		if (IsNetError())
		{

			//connect
			Disconnect();
			Login();
		}
		else
		{
			//����
			AddSendMessage(SINGNALING_CHANNEL_CMD_HEARTBEAT, nullptr, 0);
		}


		// 
		// 
		// 		if (m_pStream->IsError())
		// 		{
		// 			//connect
		// 		}
		// 		else
		// 		{
		// 			//����
		// 		}

		for (int i = 0; i < 100; i++)
		{
			CrossSleep(10);
			if (m_bTheadExit)
			{
				break;
			}
		}
	}
}

void CXClientSignaling::SignalingCtrl(const char * pszInBuffer, int nInLen, char *pszOutBuffer, int &nOutLen)
{
	m_csSignalingCtrl.Lock();
	nOutLen = -1;
	m_bSignalingCtrlFinished = FALSE;
	//m_strSignalingCtrlMsgUuid = ;//

	string command;
	int result;
	string deviceuuid;
	string msguuid;




	do 
	{
		if (!JsonGetHeaderInfo((char *)pszInBuffer, command, result, deviceuuid, m_strSignalingCtrlMsgUuid))
		{
			break;
		}


		AddSendMessage(SINGNALING_CHANNEL_CMD_TRANS, (char *)pszInBuffer, nInLen);
		//
		for (int i = 0; i < 100; i++)
		{
			if (m_bSignalingCtrlFinished)
			{
				memcpy(pszOutBuffer, m_pSignalingCtrlBuffer, m_nSignalingCtrlBufferLen);
				nOutLen = m_nSignalingCtrlBufferLen;
				break;
			}
			CrossSleep(15);
		}
	} while (0);



	m_csSignalingCtrl.Unlock();
}