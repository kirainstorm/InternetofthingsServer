#include "stdafx.h"
#include "GetRoutes.h"

extern HANDLE g_hStartDoing; //手动重置,初始无信号

CXRedirectionGetRoutes::CXRedirectionGetRoutes(emXInternetType _type, string _id)
{
	m_hThread = NULL;
	m_type = _type;
	m_strID = _id;
}
CXRedirectionGetRoutes::~CXRedirectionGetRoutes()
{
}
void CXRedirectionGetRoutes::Start()
{
	m_hThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkerThread, this, 0, NULL);
}
void CXRedirectionGetRoutes::Stop()
{
	if (m_hThread)
	{
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}
void CXRedirectionGetRoutes::WorkerThread(void* param)
{
	CXRedirectionGetRoutes *p = (CXRedirectionGetRoutes*)param;
	p->Worker();
}
void CXRedirectionGetRoutes::Worker()
{
	//srand(time(NULL));
	//int _index = rand() % 90000 + m_index;
	//
	//
	BOOL bSuccess = FALSE;
	ST_REDIRECTION_SEND_BUFFER *pSendBuffer = (ST_REDIRECTION_SEND_BUFFER *)CMiddleBufferPool::Instance().malloc();
	login_redirection_t _login;
	signaling_channel_head_t msg_recv;
	char szBuffer[256] = {};
	CXNetStream* pTcpStream = NULL;
	//
	//
	WaitForSingleObject(g_hStartDoing, INFINITE);
	//
	//
	do
	{
		AAAA:
		pTcpStream = XNetCreateStream4Connect(SERVER_IP, 6599, 5);
		if (NULL == pTcpStream)
		{
			break;
		}
		if (0 != XNetConnectStream(pTcpStream))
		{
			break;
		}
		//
		memset(pSendBuffer, 0, sizeof(ST_REDIRECTION_SEND_BUFFER));
		memset(&_login, 0, sizeof(_login));
		//
		if (m_type == X_INTERNET_XXX_TYPE_CLASSIC)
		{

		}

		pSendBuffer->head.cmd = SINGNALING_CHANNEL_CMD_REDIRECTION_USER;



		pSendBuffer->head.datalen = sizeof(_login);
		char szttmp[32] = { 0 };
		sprintf(szttmp, "%d@qq.com", _index);
#if 1
		CROSS_STRCPY(_login.id, szttmp);
#else
		CROSS_STRCPY(_login.id, "6281@qq.com");
#endif

		memcpy(pSendBuffer->msg, &_login, sizeof(_login));
		//
		if (0 != pTcpStream->SyncWriteAndRead(pSendBuffer, sizeof(signaling_channel_head_t) + pSendBuffer->head.datalen, &msg_recv, sizeof(signaling_channel_head_t),120))
		{
			//CROSS_TRACE(">>>GetIOTServerIP Error<<< 3");
			break;
		}
		if (msg_recv.cmd != SINGNALING_CHANNEL_CMD_REDIRECTION_USER)
		{
			//CROSS_TRACE(">>>GetIOTServerIP Error<<< 4");
			break;
		}
		if (msg_recv.datalen == 0)
		{
			//CROSS_TRACE(">>>GetIOTServerIP Error<<< 5");
			break;
		}
		if (0 != pTcpStream->SyncWriteAndRead(NULL, 0, szBuffer, msg_recv.datalen))
		{
			//CROSS_TRACE(">>>GetIOTServerIP Error<<< 10");
			break;
		}
		//
		string s = szBuffer;
		vector<string> v;
		_SplitString(s, v, "|");
		//
		//CROSS_STRCPY(m_sziotip, v[0].c_str());
		//CROSS_STRCPY(m_szusername, v[1].c_str());
		//
		CROSS_TRACE(">>>GetIOTServerIP<<< okokok");
		//
		bSuccess = TRUE;
		//
	} while (FALSE);
	//
	if (pTcpStream)
	{
		pTcpStream->Release();
		pTcpStream = NULL;
	}
	//
	if (bSuccess)
	{
		CXCounter::Instance().AddOnlineTcpUsers();
	}
	else
	{
		CXCounter::Instance().AddOnlineWebSocketUser();
	}
	//
	//
	goto AAAA;
}