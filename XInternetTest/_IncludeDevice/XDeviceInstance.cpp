#include "stdafx.h"
#include "XDeviceInstance.h"
//----------------------------------------
// kirainstorm
// https://github.com/kirainstorm
//----------------------------------------
CXDeviceInstance::CXDeviceInstance()
{
	m_bTheadExit = FALSE;
	m_hThread = CROSS_THREAD_NULL;
	m_pStream = NULL;
	m_pSignaling = NULL;
}
CXDeviceInstance::~CXDeviceInstance()
{
}
void CXDeviceInstance::XDelete()
{
	XDisconnect();
	delete this;
}
int CXDeviceInstance::XConnect(XDeviceMessageCallback *callback, const char * ip, const char * uuid)
{
	m_pMessageCallback = callback;
	if (CROSS_THREAD_NULL == m_hThread)
	{
		memset(m_szSvrIp, 0, sizeof(m_szSvrIp));
		memset(m_szUuid, 0, sizeof(m_szUuid));
		memset(m_szIotUsername, 0, sizeof(m_szIotUsername));
		//
		memcpy(m_szSvrIp, ip, strlen(ip));
		memcpy(m_szUuid, uuid, strlen(uuid));
		//
		m_pSignaling = new CXDeviceSignaling(m_pMessageCallback);
		assert(m_pSignaling);
		m_pStream = new CXDeviceStream();
		assert(m_pStream);
		m_bTheadExit = FALSE;
		m_hThread = CrossCreateThread(ThreadWorker, this);
	}
	return 0;
}
int CXDeviceInstance::XDisconnect()
{
	if (CROSS_THREAD_NULL != m_hThread)
	{
		m_bTheadExit = TRUE;
		CrossWaitThread(m_hThread);
		CrossCloseThread(m_hThread);
		m_hThread = CROSS_THREAD_NULL;
		//
	}

	if (m_pSignaling)
	{
		m_pSignaling->Disconnect();
		delete m_pSignaling;
		m_pSignaling = NULL;
	}
	if (m_pStream)
	{
		m_pStream->Disconnect();
		delete m_pStream;
		m_pStream = NULL;
	}

	return 0;
}
int CXDeviceInstance::XGetStatus()
{
	if (m_pSignaling->IsConnectError() && m_pStream->IsConnectError())
	{
		return -3;
	}
	if (m_pSignaling->IsConnectError())
	{
		return -2;
	}
	if (m_pStream->IsConnectError())
	{
		return -1;
	}
	return 0;
}
int CXDeviceInstance::XSendMessage(const char * jsonbuffer, int len)
{
	if (m_pSignaling->IsConnectError())
	{
		return -2;
	}
	m_pSignaling->AddSendMessage(SINGNALING_CHANNEL_CMD_TRANS, (char *)jsonbuffer, len);
	return 0;
}
int CXDeviceInstance::XSendStream(emMEDIA_FRAME_TYPE_DEFINE nAVFrameType, const char *data, int len, uint64_t tick,
	uint16_t nVideoFrameRate, uint16_t nVideoWidth, uint16_t nVideoHeight,
	uint16_t nAudioChannels, uint16_t nAudioSamplesRate, uint16_t nAudioBitsPerSample)
{
	if (m_pStream->IsConnectError())
	{
		return -1;
	}
	return m_pStream->AddSendStream(nAVFrameType, data, len, tick,
		nVideoFrameRate, nVideoWidth, nVideoHeight,
		nAudioChannels, nAudioSamplesRate, nAudioBitsPerSample);
}
int CXDeviceInstance::ThreadWorker(void * param)
{
	CXDeviceInstance * p = (CXDeviceInstance *)param;
	p->Woker();
	return 0;
}
void CXDeviceInstance::Woker()
{

	while (FALSE == m_bTheadExit)
	{
		//-----------------------------------------------------------------------------
		if (m_pSignaling->IsConnectError() && m_pStream->IsConnectError())
		{
			//
			string s1 = m_szUuid;
			if (____XDeviceInterfaceIsClassicID(s1))
			{
				memset(m_szRealIp, 0, sizeof(m_szRealIp));
				CROSS_STRCPY(m_szRealIp, m_szSvrIp);
			}
			else
			{
				GetIotServerInfo();
			}

		}
		//-----------------------------------------------------------------------------
#if 1

		if (m_pSignaling->IsConnectError())
		{

			//connect
			m_pSignaling->Disconnect();
			m_pSignaling->Connect(m_szRealIp, m_szUuid, m_szIotUsername);
		}
		else
		{
			//
			m_pSignaling->AddSendMessage(SINGNALING_CHANNEL_CMD_HEARTBEAT, nullptr, 0);
// 			char szttmp[64] = { 0 };
// 			sprintf(szttmp, "%s send: hello world (alarm)!", m_szUuid);
// 			for (int i = 0; i < 3; i++)
// 			{
// 				m_pSignaling->AddSendMessage(SINGNALING_CHANNEL_CMD_ALARM, szttmp, strlen(szttmp));
// 			}
		}

#endif
		//-----------------------------------------------------------------------------
		if (m_pStream->IsConnectError())
		{
			//connect
			m_pStream->Disconnect();
			m_pStream->Connect(m_szRealIp, m_szUuid, m_szIotUsername);
		}
		else
		{
			//
			m_pStream->AddHeartbeat();
		}
		//-----------------------------------------------------------------------------
		for (int i = 0; i< 100; i ++)
		{
			CrossSleep(40);
			if (m_bTheadExit)
			{
				break;
			}
		}
		//-----------------------------------------------------------------------------
	}
}


void CXDeviceInstance::GetIotServerInfo()
{

	ST_DEVICE_SIGNALING_SEND_BUFFER *pSendBuffer = CXDeviceSignalingBufferPool::Instance().malloc();
	login_redirection_t rt;
	signaling_channel_head_t msg_recv;
	char szBuffer[256] = {};
	CXNetStream* pTcpStream = NULL;


	do
	{
		pTcpStream = XNetCreateStream4Connect(m_szSvrIp, 6599, 3);
		if (NULL == pTcpStream)
		{
			CROSS_TRACE(">>>GetIOTServerIP Error<<< 1");
			break;
		}
		if (0 != XNetConnectStream(pTcpStream))
		{
			CROSS_TRACE(">>>GetIOTServerIP Error<<< 2");
			break;
		}
		//
		memset(pSendBuffer, 0, sizeof(ST_DEVICE_SIGNALING_SEND_BUFFER));
		memset(&rt, 0, sizeof(rt));
		pSendBuffer->head.cmd = SINGNALING_CHANNEL_CMD_REDIRECTION_DEVICE;
		pSendBuffer->head.datalen = sizeof(rt);
		CROSS_STRCPY(rt.id, m_szUuid);
		memcpy(pSendBuffer->msg, &rt, sizeof(rt));
		//
		if (0 != pTcpStream->SyncWriteAndRead(pSendBuffer, sizeof(signaling_channel_head_t) + pSendBuffer->head.datalen, &msg_recv, sizeof(signaling_channel_head_t)))
		{
			CROSS_TRACE(">>>GetIOTServerIP Error<<< 3");
			break;
		}
		if (msg_recv.cmd != SINGNALING_CHANNEL_CMD_REDIRECTION_DEVICE)
		{
			CROSS_TRACE(">>>GetIOTServerIP Error<<< 4");
			break;
		}
		if (msg_recv.datalen == 0)
		{
			CROSS_TRACE(">>>GetIOTServerIP Error<<< 5");
			break;
		}
		if (0 != pTcpStream->SyncWriteAndRead(NULL, 0, szBuffer, msg_recv.datalen))
		{
			CROSS_TRACE(">>>GetIOTServerIP Error<<< 10");
			break;
		}
		//
		string s = szBuffer;
		vector<string> v;
		_SplitString(s, v, "|");
		//
		CROSS_STRCPY(m_szRealIp, v[0].c_str());
		CROSS_STRCPY(m_szIotUsername, v[1].c_str());
		//
		//CROSS_TRACE(">>>GetIOTServerIP<<< okokok");
	} while (FALSE);
	//
	if (pTcpStream)
	{
		pTcpStream->Release();
		pTcpStream = NULL;
	}


}