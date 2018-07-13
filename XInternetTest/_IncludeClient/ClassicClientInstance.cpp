#include "stdafx.h"
#include "ClassicClientInstance.h"
#include "XDes.h"

#if XINTERNET_TEST_STATUS
extern HANDLE g_hStartDoing; //手动重置,初始无信号
#endif

CXClassicClientInstance::CXClassicClientInstance()
{

	//m_hThreadGetDevice = CROSS_THREAD_NULL;
}
CXClassicClientInstance::~CXClassicClientInstance()
{
}
void CXClassicClientInstance::XDelete()
{



	// 	if (CROSS_THREAD_NULL != m_bTheadExitGetDevice)
	// 	{
	// 		m_bTheadExitGetDevice = TRUE;
	// 		CrossWaitThread(m_hThreadGetDevice);
	// 		CrossCloseThread(m_hThreadGetDevice);
	// 		m_hThreadGetDevice = CROSS_THREAD_NULL;
	// 	}


	//
	// 		if (m_pSignaling)
	// 		{
	// 			m_pSignaling->Disconnect();
	// 			delete m_pSignaling;
	// 			m_pSignaling = NULL;
	// 		}

	UnintInstance();
	ClearDeivceInfoList();
	ClearSignalingServers();
	delete this;
}

void CXClassicClientInstance::InitInstance(const char *pszIp, const char* pszUser, const char* pszPwd, CXSignalingChannelMessageCallback * pCallback)
{
	m_pMessageCallback = pCallback;

		memset(m_szCenterIp, 0, sizeof(m_szCenterIp));
		memset(m_szUser, 0, sizeof(m_szUser));
		memset(m_szPwd, 0, sizeof(m_szPwd));
		//
		memcpy(m_szCenterIp, pszIp, strlen(pszIp));
		memcpy(m_szUser, pszUser, strlen(pszUser));
		memcpy(m_szPwd, pszPwd, strlen(pszPwd));
		s_nCenterPort = 6499;
		//
		// 		m_pSignaling = new CXClassicClientSignaling(m_pMessageCallback);
		// 		assert(m_pSignaling);

		//
		// 		m_bUserGetDevices = FALSE;
		// 		m_bTheadExitGetDevice = FALSE;
		// 		m_hThreadGetDevice = CrossCreateThread(ThreadWorkerGetDevice, this);
	
}
void CXClassicClientInstance::UnintInstance()
{

}
void CXClassicClientInstance::GetDevices(char * pdstBuffer, int &len)
{
	m_cs.Lock();
	len = 0;
	CXNetStream* m_pTcpStream = NULL;
	ST_SIGNALING_SEND_BUFFER *pSendBuffer = CSignalingBufferPool::Instance().malloc();;
	signaling_channel_head_t msg_key_recv;

	do
	{
		//--------------------------------------------------------------------------------------------------------------------------
		m_pTcpStream = XNetCreateStream4Connect(m_szCenterIp, 6499, 10);
		if (0 != XNetConnectStream(m_pTcpStream))
		{
			//CROSS_TRACE("CMediaDevInterface::Login -- conn svr error");
			break;
		}
		//--------------------------------------------------------------------------------------------------------------------------
		//NEED_SESSIONID
		memset(pSendBuffer, 0, sizeof(ST_SIGNALING_SEND_BUFFER));
		pSendBuffer->head.cmd = SINGNALING_CHANNEL_CMD_LOGIN_NEED_SESSION;
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
		uint32_t m_nSessionID = msg_key_recv.session;
		//--------------------------------------------------------------------------------------------------------------------------
		//使用key加密，登陆
		signaling_channel_user_login_t stLogin;
		memset(&stLogin, 0, sizeof(stLogin));
		XDESEncode(m_szUser, m_nSessionID, stLogin.userdes);
		XDESEncode(m_szPwd, m_nSessionID, stLogin.pwddes);
		//
		memset(pSendBuffer, 0, sizeof(ST_SIGNALING_SEND_BUFFER));
		pSendBuffer->head.cmd = SINGNALING_CHANNEL_CMD_LOGIN;
		pSendBuffer->head.session = m_nSessionID;
		pSendBuffer->head.datalen = sizeof(stLogin);
		//
		memcpy(pSendBuffer->msg, &stLogin, sizeof(stLogin));
		//
		if (0 != m_pTcpStream->SyncWriteAndRead(pSendBuffer, sizeof(signaling_channel_head_t) + sizeof(stLogin), &msg_key_recv, sizeof(signaling_channel_head_t)))
		{
			//CROSS_TRACE("CMediaDevInterface::Login -- login error -- 1");
			break;
		}
		if ((msg_key_recv.cmd != SINGNALING_CHANNEL_CMD_LOGIN) || (msg_key_recv.result != SINGNALING_CHANNEL_RESULT_OK))
		{
			//CROSS_TRACE("CMediaDevInterface::Login -- login error -- 2");
			break;
		}
		//--------------------------------------------------------------------------------------------------------------------------
		pSendBuffer->head.cmd = SINGNALING_CHANNEL_CMD_REDIRECTION_CLASSIC_GET_DEVICES;
		pSendBuffer->head.datalen = 0;
		if (0 != m_pTcpStream->SyncWriteAndRead(pSendBuffer, sizeof(signaling_channel_head_t), &msg_key_recv, sizeof(signaling_channel_head_t), 5))
		{
			//CROSS_TRACE("CMediaDevInterface::Login -- request key error -- 1");
			break;
		}
		if ((msg_key_recv.cmd != SINGNALING_CHANNEL_CMD_REDIRECTION_CLASSIC_GET_DEVICES) || (msg_key_recv.result != SINGNALING_CHANNEL_RESULT_OK))
		{
			//CROSS_TRACE("CMediaDevInterface::Login -- request key error -- 2");
			break;
		}
		//
		if (msg_key_recv.datalen > 0)
		{
			if (0 != m_pTcpStream->SyncWriteAndRead(NULL, 0, pdstBuffer, msg_key_recv.datalen, 5))
			{
				//CROSS_TRACE("CMediaDevInterface::Login -- request key error -- 1");
				break;
			}
		}
		//
		ClearDeivceInfoList();
		for (int i = 0; i < (int)(msg_key_recv.datalen / sizeof(ST_CLASSIC_DEVICE_INFO_BASE)); i++)
		{
			ST_CLASSIC_DEVICE_INFO_BASE * pDeviceInfo = CClassicDeviceInfoBufferPool::Instance().malloc();
			memcpy(pDeviceInfo, pdstBuffer + i*sizeof(ST_CLASSIC_DEVICE_INFO_BASE), sizeof(ST_CLASSIC_DEVICE_INFO_BASE));
			m_lisDevice[pDeviceInfo->dev_id] = pDeviceInfo;
		}
		UpdateSignalingServers();
		//
		len = msg_key_recv.datalen;
	} while (0);

	if (m_pTcpStream)
	{
		m_pTcpStream->Release();
		m_pTcpStream = NULL;
	}
	CSignalingBufferPool::Instance().free(pSendBuffer);

	m_cs.Unlock();
}

emClientNetStatus CXClassicClientInstance::GetSignalingStatus(int nDevid)
{
	return CLIENT_NET_STATUS_NONE;
}
void CXClassicClientInstance::SignalingCtrl(const  char * szDeviceID, const char * pszInBuffer, int nInLen, char *pszOutBuffer, int &nOutLen)
{
	nOutLen = -1;
	m_cs.Lock();
	do{
		//
		int devid = atoi(szDeviceID);
		map<int/*devid*/, ST_CLASSIC_DEVICE_INFO_BASE *>::iterator itdevice = m_lisDevice.find(devid);
		if (itdevice == m_lisDevice.end())
		{
			break;
		}
		//
		string strServerIp = itdevice->second->dev_media_ip;
		map<string, CXClientSignaling *>::iterator itServer = m_SignalingMap.find(strServerIp);
		if (itServer == m_SignalingMap.end())
		{
			break;
		}
		//
		itServer->second->SignalingCtrl(pszInBuffer, nInLen, pszOutBuffer, nOutLen);
		//
	} while (0);
	m_cs.Unlock();
}
CXClientStreamInterface * CXClassicClientInstance::CreateXClientStreamInterface(const char * uuid, int channel)
{

// 	char m_szCenterIp[64];
// 	int s_nCenterPort;
// 	char m_szUser[64];
// 	char m_szPwd[64];

	string strServerIp = "";
	m_cs.Lock();
	do{
		//
		int devid = atoi(uuid);
		map<int/*devid*/, ST_CLASSIC_DEVICE_INFO_BASE *>::iterator itdevice = m_lisDevice.find(devid);
		if (itdevice == m_lisDevice.end())
		{
			break;
		}
		//
		strServerIp = itdevice->second->dev_media_ip;
		//
	} while (0);
	m_cs.Unlock();


	if (strServerIp == "")
	{
		return NULL;
	}

	CXClientStream *p = new CXClientStream(X_CLIENT_XXX_TYPE_CLASSIC, uuid, channel, strServerIp.c_str(), 6503, m_szUser, m_szPwd);
	assert(p);
	return p;
}
CXClientDownloadInterface * CXClassicClientInstance::CreateXClientDownloadInterface(const char * uuid, int channel)
{
	string strServerIp = "";
	m_cs.Lock();
	do{
		//
		int devid = atoi(uuid);
		map<int/*devid*/, ST_CLASSIC_DEVICE_INFO_BASE *>::iterator itdevice = m_lisDevice.find(devid);
		if (itdevice == m_lisDevice.end())
		{
			break;
		}
		//
		strServerIp = itdevice->second->dev_media_ip;
		//
	} while (0);
	m_cs.Unlock();
	//
	if (strServerIp == "")
	{
		return NULL;
	}
	//
	CXClientHislog *p = new CXClientHislog(X_CLIENT_XXX_TYPE_CLASSIC, uuid, channel, strServerIp.c_str(), 6505, m_szUser, m_szPwd);
	assert(p);
	return p;
}



// 
// int CXClassicClientInstance::ThreadWorkerGetDevice(void * param)
// {
// 	CXClassicClientInstance * p = (CXClassicClientInstance *)param;
// 	p->WokerGetDevice();
// 	return 0;
// }
// void CXClassicClientInstance::WokerGetDevice()
// {
// 	while (FALSE == m_bTheadExit)
// 	{
// 
// 		if (FALSE == m_bUserGetDevices)
// 		{
// 			CrossSleep(30);
// 			continue;
// 		}
// 		//
// 		
// 		//
// 
// 
// 
// 
// 
// 
// 
// 		//
// 		m_bUserGetDevices = FALSE;//重置标志位
// 	}
// }
void CXClassicClientInstance::UpdateSignalingServers()
{

#if 1
	//如果服务器被删除，这里不处理，作为冗余数据存在
	for (map<int/*devid*/, ST_CLASSIC_DEVICE_INFO_BASE *>::iterator it = m_lisDevice.begin(); it != m_lisDevice.end(); it++)
	{
		string strServerIp = it->second->dev_media_ip;
		map<string, CXClientSignaling *>::iterator itServer = m_SignalingMap.find(strServerIp);
		if (itServer == m_SignalingMap.end())
		{
			//添加一个
			CXClientSignaling * pSignaling = new CXClientSignaling(m_pMessageCallback);
			pSignaling->Connect(it->second->dev_media_ip, 6500, m_szUser, m_szPwd);
			m_SignalingMap[strServerIp] = pSignaling;
		}
	}
#endif
}
void CXClassicClientInstance::ClearSignalingServers()
{
	for (map<string, CXClientSignaling *>::iterator it = m_SignalingMap.begin(); it != m_SignalingMap.end(); it++)
	{
		it->second->Delete();
	}
	m_SignalingMap.clear();
}
void CXClassicClientInstance::ClearDeivceInfoList()
{
	for (map<int/*devid*/, ST_CLASSIC_DEVICE_INFO_BASE *>::iterator it = m_lisDevice.begin(); it != m_lisDevice.end(); it++)
	{
		CClassicDeviceInfoBufferPool::Instance().free(it->second);
	}
	m_lisDevice.clear();
}


// 
// void CXClassicClientInstance::GetIotServerInfo()
// {
// 
// #if XINTERNET_TEST_STATUS
// 	memset(m_szIotIp, 0, sizeof(m_szIotIp));
// 	//CROSS_STRCPY(m_szIotIp, "127.0.0.1");
// 	CROSS_STRCPY(m_szIotIp, "47.96.249.142");
// 	//CROSS_STRCPY(m_szIotIp, "52.175.25.247");
// 
// 	return;
// #endif
// 	//
// 
// 	ST_CLASSIC_CLIENT_SIGNALING_SEND_BUFFER *pSendBuffer = CXClassicClientSignalingBufferPool::Instance().malloc();
// 	login_redirection_t rt;
// 	signaling_channel_head_t msg_recv;
// 	char szBuffer[256] = {};
// 	CXNetStream* pTcpStream = NULL;
// 
// 	//m_cs.Lock();
// 	do
// 	{
// 		pTcpStream = XNetCreateStream4Connect(m_szCenterIp, s_nCenterPort, 3);
// 		if (NULL == pTcpStream)
// 		{
// 			CROSS_TRACE(">>>GetIOTServerIP Error<<< 1");
// 			break;
// 		}
// 		if (0 != XNetConnectStream(pTcpStream))
// 		{
// 			CROSS_TRACE(">>>GetIOTServerIP Error<<< 2");
// 			break;
// 		}
// 		//
// 		memset(pSendBuffer, 0, sizeof(ST_CLASSIC_CLIENT_SIGNALING_SEND_BUFFER));
// 		memset(&rt, 0, sizeof(rt));
// 		pSendBuffer->head.cmd = SINGNALING_CHANNEL_CMD_REDIRECTION_USER;
// 		pSendBuffer->head.datalen = sizeof(rt);
// 		CROSS_STRCPY(rt.id, m_szUser);
// 		memcpy(pSendBuffer->msg, &rt, sizeof(rt));
// 		//
// 		if (0 != pTcpStream->SyncWriteAndRead(pSendBuffer, sizeof(signaling_channel_head_t) + pSendBuffer->head.datalen, &msg_recv, sizeof(signaling_channel_head_t)))
// 		{
// 			CROSS_TRACE(">>>GetIOTServerIP Error<<< 3");
// 			break;
// 		}
// 		if (msg_recv.cmd != SINGNALING_CHANNEL_CMD_REDIRECTION_USER)
// 		{
// 			CROSS_TRACE(">>>GetIOTServerIP Error<<< 4");
// 			break;
// 		}
// 		if (msg_recv.datalen == 0)
// 		{
// 			CROSS_TRACE(">>>GetIOTServerIP Error<<< 5");
// 			break;
// 		}
// 		if (0 != pTcpStream->SyncWriteAndRead(NULL, 0, szBuffer, msg_recv.datalen))
// 		{
// 			CROSS_TRACE(">>>GetIOTServerIP Error<<< 10");
// 			break;
// 		}
// 		//
// 		memset(m_szIotIp, 0, sizeof(m_szIotIp));
// 		if (strlen(szBuffer) <= sizeof(m_szIotIp))
// 		{
// 			memcpy(m_szIotIp, szBuffer, strlen(szBuffer));
// 			CROSS_TRACE(">>>GetIOTServerIP<<< okokok");
// 		}
// 		//
// 	} while (FALSE);
// 	//
// 	if (pTcpStream)
// 	{
// 		pTcpStream->Release();
// 		pTcpStream = NULL;
// 	}
// 	//
// 	//m_cs.Unlock();
// 	//return (0 == ret) ? m_szIOTServerIp : nullptr;
// 
// }