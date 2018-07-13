#include "stdafx.h"
#include "IOTClientInstance.h"

#if XINTERNET_TEST_STATUS
extern HANDLE g_hStartDoing; //手动重置,初始无信号
#endif

CXIotClientInstance::CXIotClientInstance()
{
	m_hThread = CROSS_THREAD_NULL;
	m_pSignaling = NULL;
}
CXIotClientInstance::~CXIotClientInstance()
{
}
void CXIotClientInstance::XDelete()
{
	if (CROSS_THREAD_NULL != m_hThread)
	{
		m_bTheadExit = TRUE;
		CrossWaitThread(m_hThread);
		CrossCloseThread(m_hThread);
		m_hThread = CROSS_THREAD_NULL;

	}
	//
	if (m_pSignaling)
	{
		m_pSignaling->Delete();
		m_pSignaling = NULL;
	}

}

void CXIotClientInstance::InitInstance(const char *pszIp, const char* pszUser, const char* pszPwd, CXSignalingChannelMessageCallback * pCallback)
{
	m_pMessageCallback = pCallback;

	if (CROSS_THREAD_NULL == m_hThread)
	{
		memset(m_szCenterIp, 0, sizeof(m_szCenterIp));
		memset(m_szUser, 0, sizeof(m_szUser));
		memset(m_szPwd, 0, sizeof(m_szPwd));
		//
		memcpy(m_szCenterIp, pszIp, strlen(pszIp));
		memcpy(m_szUser, pszUser, strlen(pszUser));
		memcpy(m_szPwd, pszPwd, strlen(pszPwd));
		s_nCenterPort = 6599;
		//
		m_pSignaling = new CXClientSignaling(m_pMessageCallback);
		assert(m_pSignaling);
		m_bTheadExit = FALSE;
		m_hThread = CrossCreateThread(ThreadWorker, this);
	}
}
void CXIotClientInstance::UnintInstance()
{

}
void CXIotClientInstance::GetDevices(char * pdstBuffer, int &len)
{

}
emClientNetStatus CXIotClientInstance::GetSignalingStatus(int value)
{
	return CLIENT_NET_STATUS_NONE;
}
void CXIotClientInstance::SignalingCtrl(const  char * szDeviceID, const char * pszInBuffer, int nInLen, char *pszOutBuffer, int &nOutLen)
{

}
CXClientStreamInterface * CXIotClientInstance::CreateXClientStreamInterface(const char * uuid, int channel/*0*/)
{
	return NULL;
}
CXClientDownloadInterface * CXIotClientInstance::CreateXClientDownloadInterface(const char * uuid, int channel/*0*/)
{
	return NULL;
}


int CXIotClientInstance::ThreadWorker(void * param)
{
	CXIotClientInstance * p = (CXIotClientInstance *)param;
	p->Woker();
	return 0;
}
void CXIotClientInstance::Woker()
{
#if XINTERNET_TEST_STATUS
	WaitForSingleObject(g_hStartDoing, INFINITE);
#endif

	while (FALSE == m_bTheadExit)
	{
		// 		if (m_pSignaling->IsError())
		// 		{
		// 			GetIotServerInfo();
		// 		}
		// 
		// 
		// 
		// 		if (m_pSignaling->IsError())
		// 		{
		// 
		// 			//connect
		// 			m_pSignaling->Disconnect();
		// 			m_pSignaling->Connect(m_szIotIp,6600, m_szUser, m_szPwd);
		// 		}
		// 		else
		// 		{
		// 			//心跳
		// 			m_pSignaling->AddSendMessage(SINGNALING_CHANNEL_CMD_HEARBEAT, nullptr, 0);
		// 		}


		// 
		// 
		// 		if (m_pStream->IsError())
		// 		{
		// 			//connect
		// 		}
		// 		else
		// 		{
		// 			//心跳
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


void CXIotClientInstance::GetIotServerInfo()
{

#if XINTERNET_TEST_STATUS
	memset(m_szIotIp, 0, sizeof(m_szIotIp));
	//CROSS_STRCPY(m_szIotIp, "127.0.0.1");
	CROSS_STRCPY(m_szIotIp, "47.96.249.142");
	//CROSS_STRCPY(m_szIotIp, "52.175.25.247");

	return;
#endif
	//

	ST_SIGNALING_SEND_BUFFER *pSendBuffer = CSignalingBufferPool::Instance().malloc();
	login_redirection_t rt;
	signaling_channel_head_t msg_recv;
	char szBuffer[256] = {};
	CXNetStream* pTcpStream = NULL;

	//m_cs.Lock();
	do
	{
		pTcpStream = XNetCreateStream4Connect(m_szCenterIp, s_nCenterPort, 3);
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
		memset(pSendBuffer, 0, sizeof(ST_SIGNALING_SEND_BUFFER));
		memset(&rt, 0, sizeof(rt));
		pSendBuffer->head.cmd = SINGNALING_CHANNEL_CMD_REDIRECTION_USER;
		pSendBuffer->head.datalen = sizeof(rt);
		CROSS_STRCPY(rt.id, m_szUser);
		memcpy(pSendBuffer->msg, &rt, sizeof(rt));
		//
		if (0 != pTcpStream->SyncWriteAndRead(pSendBuffer, sizeof(signaling_channel_head_t) + pSendBuffer->head.datalen, &msg_recv, sizeof(signaling_channel_head_t)))
		{
			CROSS_TRACE(">>>GetIOTServerIP Error<<< 3");
			break;
		}
		if (msg_recv.cmd != SINGNALING_CHANNEL_CMD_REDIRECTION_USER)
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
		memset(m_szIotIp, 0, sizeof(m_szIotIp));
		if (strlen(szBuffer) <= sizeof(m_szIotIp))
		{
			memcpy(m_szIotIp, szBuffer, strlen(szBuffer));
			CROSS_TRACE(">>>GetIOTServerIP<<< okokok");
		}
		//
	} while (FALSE);
	//
	if (pTcpStream)
	{
		pTcpStream->Release();
		pTcpStream = NULL;
	}
	//
	//m_cs.Unlock();
	//return (0 == ret) ? m_szIOTServerIp : nullptr;


	CSignalingBufferPool::Instance().free(pSendBuffer);
}