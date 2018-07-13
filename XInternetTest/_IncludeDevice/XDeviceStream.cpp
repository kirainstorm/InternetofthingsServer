#include "stdafx.h"
#include "XDeviceStream.h"
#include "XDes.h"
//----------------------------------------
// kirainstorm
// https://github.com/kirainstorm
//----------------------------------------
CXDeviceStream::CXDeviceStream()
{
	m_nCanSendStream = FALSE;
	m_pTcpStream = NULL;
	m_bIsError = TRUE;
	m_pBuffer = CLittleBufferPool::Instance().malloc();
	m_dwLastTick = CrossGetTickCount64() - 100000;
}
CXDeviceStream::~CXDeviceStream()
{
	CLittleBufferPool::Instance().free(m_pBuffer);
}
void CXDeviceStream::OnPacketCompleteNetStreamData(int32_t bytesTransferred, emXStreamTansferKey transferKey)
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
		if (m_dataSendList.size() > 0)
		{
			ST_DEVICE_STREAM_SEND_BUFFER * pMsg = (ST_DEVICE_STREAM_SEND_BUFFER *)m_dataSendList.front();
			if (pMsg->head.datalen > 0)
			{
				int iiii = 0;
			}
			m_pTcpStream->AsyncWrite(pMsg, pMsg->head.datalen + sizeof(pMsg->head));//发送
			m_dataSendList.pop_front();
			CXDeviceStreamBufferPool::Instance().free(pMsg);
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

void CXDeviceStream::Connect(char * s_ip, char * uuid, char * user)
{


	if (NULL == m_pTcpStream)
	{
		m_nCanSendStream = FALSE;
		BOOL bLoginOK = FALSE;
		ST_DEVICE_STREAM_SEND_BUFFER *pSendBuffer = CXDeviceStreamBufferPool::Instance().malloc();;
		ST_XMEDIA_HEAD msg_key_recv;
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
				m_pTcpStream = XNetCreateStream4Connect(s_ip, 6504, 10);
			}
			else
			{
				m_pTcpStream = XNetCreateStream4Connect(s_ip, 6604, 10);
			}


			if (0 != XNetConnectStream(m_pTcpStream))
			{
				CROSS_TRACE("CXDeviceStream::Connect -- error --- 1");
				break;
			}
			//--------------------------------------------------------------------------------------------------------------------------
			//NEED_SESSIONID
			memset(pSendBuffer, 0, sizeof(ST_DEVICE_STREAM_SEND_BUFFER));
			pSendBuffer->head.cmd = XMEDIA_COMMAND_LOGIN_NEED_SESSIONID;
			//
			if (0 != m_pTcpStream->SyncWriteAndRead(pSendBuffer, sizeof(ST_XMEDIA_HEAD), &msg_key_recv, sizeof(ST_XMEDIA_HEAD), 5))
			{
				CROSS_TRACE("CXDeviceStream::Connect -- error --- 2");
				break;
			}
			//
			if ((msg_key_recv.cmd != XMEDIA_COMMAND_LOGIN_NEED_SESSIONID) || (msg_key_recv.result != XMEDIA_RESULT_OK))
			{
				CROSS_TRACE("CXDeviceStream::Connect -- error --- 3");
				break;
			}
			//
			m_nSessionID = msg_key_recv.session;

			//--------------------------------------------------------------------------------------------------------------------------
			//使用key加密，登陆
			ST_XMEDIA_LOGIN_INFO stLogin;
			memset(&stLogin, 0, sizeof(ST_XMEDIA_LOGIN_INFO));
			CROSS_STRCPY(stLogin.user, user);
			XDESEncode(uuid, m_nSessionID, stLogin.despwd);
			//
			memset(pSendBuffer, 0, sizeof(ST_DEVICE_STREAM_SEND_BUFFER));
			pSendBuffer->head.cmd = XMEDIA_COMMAND_LOGIN;
			pSendBuffer->head.session = m_nSessionID;
			pSendBuffer->head.datalen = sizeof(ST_XMEDIA_LOGIN_INFO);
			//
			memcpy(pSendBuffer->msg, &stLogin, sizeof(ST_XMEDIA_LOGIN_INFO));
			//
			if (0 != m_pTcpStream->SyncWriteAndRead(pSendBuffer, sizeof(ST_XMEDIA_HEAD) + sizeof(ST_XMEDIA_LOGIN_INFO), &msg_key_recv, sizeof(ST_XMEDIA_HEAD)))
			{
				CROSS_TRACE("CXDeviceStream::Connect -- error --- 4");
				break;
			}
			//
			if ((msg_key_recv.cmd != XMEDIA_COMMAND_LOGIN) || (msg_key_recv.result != XMEDIA_RESULT_OK))
			{
				CROSS_TRACE("CXDeviceStream::Connect -- error --- 5");
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
void CXDeviceStream::Disconnect()
{
	if (m_pTcpStream)
	{
		m_pTcpStream->Release();
		m_pTcpStream = NULL;

		ClearSendBufferList();
		m_bIsError = TRUE;
	}
}
BOOL CXDeviceStream::IsConnectError()
{
	if ((CrossGetTickCount64() - m_dwLastTick) > 10000)
	{
		CROSS_TRACE("CXDeviceStream----->>>>>> IsConnectError");
		m_bIsError = TRUE;
		m_nCanSendStream = FALSE;
	}

	return m_bIsError;
}
int CXDeviceStream::AddSendStream(emMEDIA_FRAME_TYPE_DEFINE nAVFrameType, const char *data, int len, uint64_t tick,
	uint16_t nVideoFrameRate, uint16_t nVideoWidth, uint16_t nVideoHeight,
	uint16_t nAudioChannels, uint16_t nAudioSamplesRate, uint16_t nAudioBitsPerSample)
{
	if (!m_nCanSendStream)
	{
		return -3;
	}

	if (m_bIsError)
	{
		return -2;//not connect ! so donot send data
	}

	if (m_dataSendList.size() > 30)
	{
		return -1;//network error ! so donot send data
	}

	//
	ST_DEVICE_STREAM_SEND_BUFFER * pMsg = CXDeviceStreamBufferPool::Instance().malloc();
	//
	pMsg->head.cmd = XMEDIA_COMMAND_AV_STREAM;
	pMsg->head.datalen = sizeof(ST_XMEDIA_AVHEAD) + len;
	pMsg->head.session = m_nSessionID;
	//
	ST_XMEDIA_AVHEAD  *pAvDataHead = (ST_XMEDIA_AVHEAD *)pMsg->msg;
	pAvDataHead->nAVFrameType = nAVFrameType;
	pAvDataHead->nVideoFrameRate = nVideoFrameRate;
	pAvDataHead->nVideoWidth = nVideoWidth;
	pAvDataHead->nVideoHeight = nVideoHeight;
	pAvDataHead->nAudioChannels = nAudioChannels;
	pAvDataHead->nAudioSamplesRate = nAudioSamplesRate;
	pAvDataHead->nAudioBitsPerSample = nAudioBitsPerSample;
	//
	if (len > 0)
	{
		memcpy(pMsg->msg + sizeof(ST_XMEDIA_AVHEAD), data, len);
	}
	//
	m_csSendList.Lock();
	m_dataSendList.push_back(pMsg);
	m_csSendList.Unlock();

	return 0;
}
void CXDeviceStream::AddHeartbeat()
{
	//
	ST_DEVICE_STREAM_SEND_BUFFER * pMsg = CXDeviceStreamBufferPool::Instance().malloc();
	//
	pMsg->head.cmd = XMEDIA_COMMAND_HEARTBEAT;
	pMsg->head.datalen = 0;
	pMsg->head.session = m_nSessionID;
	//
	m_csSendList.Lock();
	m_dataSendList.push_back(pMsg);
	m_csSendList.Unlock();
}
void CXDeviceStream::ClearSendBufferList()
{
	m_csSendList.Lock();
	while (m_dataSendList.size() > 0)
	{
		ST_DEVICE_STREAM_SEND_BUFFER * p = m_dataSendList.front();
		m_dataSendList.pop_front();
		CXDeviceStreamBufferPool::Instance().free(p);
	}
	m_csSendList.Unlock();
}
void CXDeviceStream::DoMsg()
{
	m_dwLastTick = CrossGetTickCount64();


	if (m_head.cmd == XMEDIA_COMMAND_HEARTBEAT)
	{
#ifdef _WIN32
		CROSS_TRACE("CXDeviceStream::DoMsg() -> XMEDIA_COMMAND_HEARTBEAT");
#endif
	}

	if (m_head.cmd == XMEDIA_COMMAND_OPEN_MAIN_STREAM || m_head.cmd == XMEDIA_COMMAND_OPEN_SUB_STREAM)
	{
		m_nCanSendStream = TRUE;
	}


	if (m_head.cmd == XMEDIA_COMMAND_CLOSE_MAIN_STREAM || m_head.cmd == XMEDIA_COMMAND_CLOSE_SUB_STREAM)
	{
		m_nCanSendStream = FALSE;
	}

};