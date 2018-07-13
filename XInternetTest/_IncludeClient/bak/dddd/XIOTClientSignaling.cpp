#include "stdafx.h"
#include "XIOTClientSignaling.h"
#include "XDes.h"

CXIotClientSignaling::CXIotClientSignaling(CXSignalingChannelMessageCallback *cb)
{
	m_pCallBack = cb;
	m_pTcpStream = NULL;
	m_bIsError = TRUE;
	m_pBuffer = CLittleBufferPool::Instance().malloc();
#if XINTERNET_TEST_STATUS
	m_bCanReleaseSuccessCounter = FALSE;
#endif
}

CXIotClientSignaling::~CXIotClientSignaling()
{
	CLittleBufferPool::Instance().free(m_pBuffer);
}
void CXIotClientSignaling::OnPacketCompleteNetStreamData(int32_t bytesTransferred, emXStreamTansferKey transferKey)
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
			ST_IOT_CLIENT_SIGNALING_SEND_BUFFER * pMsg = (ST_IOT_CLIENT_SIGNALING_SEND_BUFFER *)m_msgSendList.front();
			if (pMsg->head.datalen > 0)
			{
				int iiii = 0;
			}
			m_pTcpStream->AsyncWrite(pMsg, pMsg->head.datalen + sizeof(pMsg->head));//发送
			m_msgSendList.pop_front();
			CXIotClientSignalingBufferPool::Instance().free(pMsg);
			m_csSendList.Unlock();
		}
		else
		{
			m_pTcpStream->PostDelayWriteStatus();
		}
	}
}

void CXIotClientSignaling::Connect(char * s_ip, char * user, char * pwd)
{
	if (NULL == m_pTcpStream)
	{
#if XINTERNET_TEST_STATUS
		if (m_bCanReleaseSuccessCounter)
		{
			CXTestCounter::Instance().ReleaseSuccess();
			CXTestCounter::Instance().AddFailed();
			m_bCanReleaseSuccessCounter = FALSE;
		}
#endif


		m_bIsError = FALSE;
		m_dwLastTick = CrossGetTickCount64();
		BOOL bLoginOK = FALSE;
		ST_IOT_CLIENT_SIGNALING_SEND_BUFFER *pSendBuffer = CXIotClientSignalingBufferPool::Instance().malloc();;
		signaling_channel_head_t msg_key_recv;
		do
		{
			//--------------------------------------------------------------------------------------------------------------------------
			//清理消息
			ClearSendBufferList();
			//--------------------------------------------------------------------------------------------------------------------------
			//链接服务器
			m_pTcpStream = XNetCreateStream4Connect(s_ip, 6600, 10);
			if (0 != XNetConnectStream(m_pTcpStream))
			{
				//CROSS_TRACE("CMediaDevInterface::Login -- conn svr error");
				break;
			}
			//--------------------------------------------------------------------------------------------------------------------------
			//NEED_SESSIONID
			memset(pSendBuffer, 0, sizeof(ST_IOT_CLIENT_SIGNALING_SEND_BUFFER));
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
			//使用key加密，登陆
			signaling_channel_user_login_t stLogin;
			memset(&stLogin, 0, sizeof(stLogin));
			XDESEncode(user, m_nSessionID, stLogin.userdes);
			XDESEncode(pwd, m_nSessionID, stLogin.pwddes);
			//
			// 			char szSendBuffer[1024] = { 0 };
			// 			ST_SREVER_LOGIN_INFO msg_login_info;
			// 			ST_SERVER_HEAD msg_login_send;
			// 			ST_SERVER_HEAD msg_login_recv;
			memset(pSendBuffer, 0, sizeof(ST_IOT_CLIENT_SIGNALING_SEND_BUFFER));
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
}
void CXIotClientSignaling::Disconnect()
{
	if (m_pTcpStream)
	{
		m_pTcpStream->Release();
		m_pTcpStream = NULL;

		ClearSendBufferList();
		m_bIsError = TRUE;
	}
}
BOOL CXIotClientSignaling::IsError()
{
	if ((CrossGetTickCount64() - m_dwLastTick) > 15000)
	{
		m_bIsError = TRUE;
	}

	return m_bIsError;
}
void CXIotClientSignaling::AddSendMessage(int type,char *buffer, int len)
{
	//
	ST_IOT_CLIENT_SIGNALING_SEND_BUFFER * pMsg = CXIotClientSignalingBufferPool::Instance().malloc();
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
void CXIotClientSignaling::ClearSendBufferList()
{
	m_csSendList.Lock();
	while (m_msgSendList.size() > 0)
	{
		ST_IOT_CLIENT_SIGNALING_SEND_BUFFER * p = m_msgSendList.front();
		m_msgSendList.pop_front();
		CXIotClientSignalingBufferPool::Instance().free(p);
	}
	m_csSendList.Unlock();
}

void CXIotClientSignaling::DoMsg()
{
	m_dwLastTick = CrossGetTickCount64();

	if (m_head.cmd == SINGNALING_CHANNEL_CMD_HEARBEAT)
	{
		CROSS_TRACE("CXDeviceSignaling::DoMsg() -> SINGNALING_CHANNEL_CMD_HEARBEAT");
	}

	if (m_head.cmd == SINGNALING_CHANNEL_CMD_TRANS)
	{
		AddSendMessage(m_head.cmd, m_pBuffer, m_head.datalen - 5);
	}

	if (m_head.cmd > SINGNALING_CHANNEL_CMD_LOGOUT)
	{
		m_pCallBack->OnSignalingChannelMessageCallback(m_pBuffer, m_head.datalen);
	}
};