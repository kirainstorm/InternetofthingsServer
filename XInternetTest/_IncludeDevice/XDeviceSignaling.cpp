#include "stdafx.h"
#include "XDeviceSignaling.h"
#include "XDes.h"
//----------------------------------------
// kirainstorm
// https://github.com/kirainstorm
//----------------------------------------
CXDeviceSignaling::CXDeviceSignaling(XDeviceMessageCallback *cb)
{
	m_pCallBack = cb;
	m_pTcpStream = NULL;
	m_bIsError = TRUE;
	m_pBuffer = CLittleBufferPool::Instance().malloc();
	m_dwLastTick = CrossGetTickCount64() - 100000;
}

CXDeviceSignaling::~CXDeviceSignaling()
{
	CLittleBufferPool::Instance().free(m_pBuffer);
}
void CXDeviceSignaling::OnPacketCompleteNetStreamData(int32_t bytesTransferred, emXStreamTansferKey transferKey)
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
				if (m_head.datalen > LITTLE_BUFFER_SIZE_KB)
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
		volatile BOOL bSendMessage = FALSE;
		//
		m_csSendList.Lock();
		if (m_msgSendList.size() > 0)
		{
			ST_DEVICE_SIGNALING_SEND_BUFFER * pMsg = (ST_DEVICE_SIGNALING_SEND_BUFFER *)m_msgSendList.front();
			if (pMsg->head.datalen > 0)
			{
				int iiii = 0;
			}
			m_pTcpStream->AsyncWrite(pMsg, pMsg->head.datalen + sizeof(pMsg->head));//发送
			m_msgSendList.pop_front();
			CXDeviceSignalingBufferPool::Instance().free(pMsg);
			//
			bSendMessage = TRUE;
		}
		m_csSendList.Unlock();
		//
		if (FALSE == bSendMessage)
		{
			m_pTcpStream->PostDelayWriteStatus();
		}
	}
}

void CXDeviceSignaling::Connect(char * s_ip, char * uuid, char * user)
{
	if (NULL == m_pTcpStream)
	{
		
		BOOL bLoginOK = FALSE;
		ST_DEVICE_SIGNALING_SEND_BUFFER *pSendBuffer = CXDeviceSignalingBufferPool::Instance().malloc();;
		signaling_channel_head_t msg_key_recv;
		do
		{
			//--------------------------------------------------------------------------------------------------------------------------
			//清理消息
			ClearSendBufferList();
			//--------------------------------------------------------------------------------------------------------------------------
			//链接服务器
			string s1 = uuid;
			if (____XDeviceInterfaceIsClassicID(s1))
			{
				m_pTcpStream = XNetCreateStream4Connect(s_ip, 6501, 4);
			}
			else
			{
				m_pTcpStream = XNetCreateStream4Connect(s_ip, 6601, 4);
			}
			
			
			if (0 != XNetConnectStream(m_pTcpStream))
			{
				CROSS_TRACE("CXDeviceSignaling::Connect -- error --- 1");
				break;
			}
			//--------------------------------------------------------------------------------------------------------------------------
			//NEED_SESSIONID
			memset(pSendBuffer, 0, sizeof(ST_DEVICE_SIGNALING_SEND_BUFFER));
			pSendBuffer->head.cmd = SINGNALING_CHANNEL_CMD_LOGIN_NEED_SESSION;
			//
			if (0 != m_pTcpStream->SyncWriteAndRead(pSendBuffer, sizeof(signaling_channel_head_t), &msg_key_recv, sizeof(signaling_channel_head_t), 5))
			{
				CROSS_TRACE("CXDeviceSignaling::Connect -- error --- 2");
				break;
			}
			//
			if ((msg_key_recv.cmd != SINGNALING_CHANNEL_CMD_LOGIN_NEED_SESSION) || (msg_key_recv.result != SINGNALING_CHANNEL_RESULT_OK))
			{
				CROSS_TRACE("CXDeviceSignaling::Connect -- error --- 3");
				break;
			}
			//
			m_nSessionID = msg_key_recv.session;

			//--------------------------------------------------------------------------------------------------------------------------
			//使用key加密，登陆
			signaling_channel_device_login_t stLogin;
			memset(&stLogin, 0, sizeof(signaling_channel_device_login_t));
			XDESEncode(user, m_nSessionID, stLogin.userdes);
			XDESEncode(uuid, m_nSessionID, stLogin.uuiddes);
			//
			memset(pSendBuffer, 0, sizeof(ST_DEVICE_SIGNALING_SEND_BUFFER));	
			pSendBuffer->head.cmd = SINGNALING_CHANNEL_CMD_LOGIN;
			pSendBuffer->head.session = m_nSessionID;
			pSendBuffer->head.datalen = sizeof(signaling_channel_device_login_t);
			//
			memcpy(pSendBuffer->msg, &stLogin, sizeof(signaling_channel_device_login_t));
			//
			if (0 != m_pTcpStream->SyncWriteAndRead(pSendBuffer, sizeof(signaling_channel_head_t) + sizeof(signaling_channel_device_login_t), &msg_key_recv, sizeof(signaling_channel_head_t)))
			{
				CROSS_TRACE("CXDeviceSignaling::Connect -- error --- 4");
				break;
			}
			//
			if ((msg_key_recv.cmd != SINGNALING_CHANNEL_CMD_LOGIN) || (msg_key_recv.result != SINGNALING_CHANNEL_RESULT_OK))
			{
				CROSS_TRACE("CXDeviceSignaling::Connect -- error --- 5");
				break;
			}
			//--------------------------------------------------------------------------------------------------------------------------
			m_bIsError = FALSE;
			m_pTcpStream->SetStreamData(this);
			m_dwLastTick = CrossGetTickCount64();
			bLoginOK = TRUE;
			//--------------------------------------------------------------------------------------------------------------------------
		} while (0);
		//
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
void CXDeviceSignaling::Disconnect()
{
	if (m_pTcpStream)
	{
		m_pTcpStream->Release();
		m_pTcpStream = NULL;

		ClearSendBufferList();
		m_bIsError = TRUE;
	}
}
BOOL CXDeviceSignaling::IsConnectError()
{
	if ((CrossGetTickCount64() - m_dwLastTick) > 10000)
	{
		m_bIsError = TRUE;
		CROSS_TRACE("CXDeviceSignaling----->>>>>> IsConnectError");
	}
	return m_bIsError;
}
void CXDeviceSignaling::AddSendMessage(int type,char *buffer, int len)
{
	//
	ST_DEVICE_SIGNALING_SEND_BUFFER * pMsg = CXDeviceSignalingBufferPool::Instance().malloc();
	//
	pMsg->head.cmd = type;
	pMsg->head.datalen = len;
	pMsg->head.session = m_nSessionID;
	//
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
void CXDeviceSignaling::ClearSendBufferList()
{
	m_csSendList.Lock();
	while (m_msgSendList.size() > 0)
	{
		ST_DEVICE_SIGNALING_SEND_BUFFER * p = m_msgSendList.front();
		m_msgSendList.pop_front();
		CXDeviceSignalingBufferPool::Instance().free(p);
	}
	m_csSendList.Unlock();
}

void CXDeviceSignaling::DoMsg()
{
	m_dwLastTick = CrossGetTickCount64();

	if (m_head.cmd == SINGNALING_CHANNEL_CMD_HEARTBEAT)
	{
#ifdef _WIN32
		CROSS_TRACE("CXDeviceSignaling::DoMsg() -> SINGNALING_CHANNEL_CMD_HEARBEAT");
#endif
	}

	if (m_head.cmd == SINGNALING_CHANNEL_CMD_TRANS)
	{
		//AddSendMessage(m_head.cmd, m_pBuffer, m_head.datalen);
	}

	if (m_head.cmd > SINGNALING_CHANNEL_CMD_LOGOUT)
	{
		m_pCallBack->OnDeviceMessageCallback(m_pBuffer, m_head.datalen);
	}
};