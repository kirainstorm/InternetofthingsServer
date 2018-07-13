#include "stdafx.h"
#include "NetToServer.h"
#include "XDes.h"

CNetToServer::CNetToServer(CXIOTMessageCallback * pCallback, const char *pRedirectUrl, const char* szUser, const char* szPwd)
{
	m_pCallback = pCallback;
	m_pTcpStream = nullptr;
	//
	memset(m_szRedirectUrl, 0, sizeof(m_szRedirectUrl));
	memset(m_szUser, 0, sizeof(m_szUser));
	memset(m_szPwd, 0, sizeof(m_szPwd));
	memset(m_szIOTServerIp, 0, sizeof(m_szIOTServerIp));
	CROSS_STRCPY(m_szRedirectUrl, pRedirectUrl);
	CROSS_STRCPY(m_szUser, szUser);
	CROSS_STRCPY(m_szPwd, szPwd);
	//
	m_pBuffer = new char[LITTLE_BUFFER_SIZE_KB];
	//
	Clear();

}
CNetToServer::~CNetToServer()
{
	if (m_pBuffer)
	{
		delete[] m_pBuffer;
		m_pBuffer = nullptr;
	}
	Logout();
}

void CNetToServer::OnPacketCompleteNetStreamData(int32_t bytesTransferred, emXStreamTansferKey transferKey)
{
	//---------------------------------------------------------------------------
	if (FALSE == m_bLoginOK)
	{
		return;//直接返回
	}
	if (bytesTransferred < 0)
	{
		m_bLoginOK = FALSE;
		return;
	}
	//---------------------------------------------------------------------------
	if (ENUM_XTSTREAM_TRANSKEY_READ == transferKey)
	{
		if (0 == bytesTransferred)
		{
			m_nRcvStep = NET_RECIEVE_STEP_HEAD;
			m_pTcpStream->AsyncRead(&m_head, sizeof(m_head));
			return;
		}
		else
		{
			if (m_nRcvStep == NET_RECIEVE_STEP_HEAD)
			{
				m_dataLen = m_head.datalen;
				if (m_dataLen > LITTLE_BUFFER_SIZE_KB)
				{
					m_bLoginOK = FALSE;//出错，断开链接
					return;
				}

				if (m_dataLen == 0)
				{
					DoMsg();
					m_nRcvStep = NET_RECIEVE_STEP_HEAD;
					m_pTcpStream->AsyncRead(&m_head, sizeof(m_head));
				}
				else
				{
					m_nRcvStep = NET_RECIEVE_STEP_BODY;
					memset(m_pBuffer, 0, LITTLE_BUFFER_SIZE_KB);
					m_pTcpStream->AsyncRead(m_pBuffer, m_dataLen);
				}
				return;
			}
			if (m_nRcvStep == NET_RECIEVE_STEP_BODY)
			{
				DoMsg();
				m_nRcvStep = NET_RECIEVE_STEP_HEAD;
				m_pTcpStream->AsyncRead(&m_head, sizeof(m_head));
				return;
			}
		}
	}
	//---------------------------------------------------------------------------
	if (ENUM_XTSTREAM_TRANSKEY_WRITE == transferKey)
	{
		if (m_msgList.size() > 0)
		{
			MyStruct4NetToServer *p = m_msgList.front();
			m_pTcpStream->AsyncWrite(p, p->head.datalen + sizeof(ST_SERVER_HEAD));
			m_msgList.pop_front();
		}
		else
		{
			m_pTcpStream->PostDelayWriteStatus();
		}
	}
}


void CNetToServer::Clear()
{
	//关闭网络
	if (m_pTcpStream)
	{
		m_pTcpStream->Release();
		m_pTcpStream = NULL;
	}
	//清理内存
	while (m_msgList.size() > 0)
	{
		MyStruct4NetToServer * p = m_msgList.front();
		m_msgList.pop_front();
		delete p;
	}
	//
	m_bWorkerStop = FALSE;
	m_hWorkerThread = CROSS_THREAD_NULL;
	//
	m_nSeq = 0;
	m_nSessionID = 0;
	m_bLoginOK = FALSE;

}

int CNetToServer::Login()
{
	Logout();


	if (_Login(4))
	{
		if (CROSS_THREAD_NULL == m_hWorkerThread)
		{
			m_dwLastTick = CrossGetTickCount64();
			m_bWorkerStop = FALSE;
			m_hWorkerThread = CrossCreateThread(WorkerThread, this);
		}
	}
	else
	{
		return -1;
	}



	return 0;
}

int CNetToServer::Logout()
{
	if (CROSS_THREAD_NULL != m_hWorkerThread)
	{
		m_bWorkerStop = TRUE;
		CrossWaitThread(m_hWorkerThread);
		CrossCloseThread(m_hWorkerThread);
		m_hWorkerThread = NULL;
	}
	//
	Clear();
	return 0;
};

int CNetToServer::TansData(const char* szData, int nLen)
{
	//AddSendMessge(Z_CMD_OF_SERVER_TRANS_TRANSMISSION, (char *)&szData, nLen);
	return 0;
}
int CNetToServer::GetUserDevice()
{
	//AddSendMessge(Z_CMD_OF_SERVER_IOT_GET_DEVICES, NULL, 0);
	return 0;
};
int CNetToServer::OpenHLS(const char * devuuid)
{
// 	ST_SREVER_IOT msg;
// 	memset(&msg, 0, sizeof(msg));
// 	CROSS_STRCPY(msg.devuuid, devuuid);

	//AddSendMessge(Z_CMD_OF_SERVER_IOT_OPENHIS, (char *)&msg, sizeof(msg));
	return 0;
};
int CNetToServer::AddDevice(const char * devuuid)
{
// 	ST_SREVER_IOT msg;
// 	memset(&msg, 0, sizeof(msg));
// 	CROSS_STRCPY(msg.devuuid, devuuid);

	//AddSendMessge(Z_CMD_OF_SERVER_IOT_ADD_DEVICE, (char *)&msg, sizeof(msg));
	return 0;
};
int CNetToServer::DeleteDevice(const char * devuuid)
{
// 	ST_SREVER_IOT msg;
// 	memset(&msg, 0, sizeof(msg));
// 	CROSS_STRCPY(msg.devuuid, devuuid);

	//AddSendMessge(Z_CMD_OF_SERVER_IOT_DELETE_DEVICES, (char *)&msg, sizeof(msg));
	return 0;
};
int CNetToServer::SetIOTServerIP(const char *iotip)
{
	memset(m_szIOTServerIp, 0, sizeof(m_szIOTServerIp));
	CROSS_STRCPY(m_szIOTServerIp, iotip);
	return 0;
}
char * CNetToServer::GetIOTServerIP()
{

	int nSession;
	int ret = -1;
	ST_SERVER_HEAD stHeadRecv;
	CXNetStream* pTcpStream = NULL;
// 
// 	//m_cs.Lock();
// 	do
// 	{
// 		pTcpStream = XNetCreateStream4Connect(m_szRedirectUrl, 6410, 3);
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
// 		//recv key
// 		ST_SERVER_HEAD t;
// 		memset(&t, 0, sizeof(t));
// 		t.cmd = Z_CMD_OF_SERVER_LOGIN;
// 		t.datalen = 0;
// 		//send "need session"
// 		if (0 != pTcpStream->SyncWriteAndRead(&t, sizeof(ST_SERVER_HEAD), &stHeadRecv, sizeof(ST_SERVER_HEAD)))
// 		{
// 			CROSS_TRACE(">>>GetIOTServerIP Error<<< 3");
// 			break;
// 		}
// 		if (stHeadRecv.cmd != Z_CMD_OF_SERVER_LOGIN_NEED_SESSIONID)
// 		{
// 			CROSS_TRACE(">>>GetIOTServerIP Error<<< 4");
// 			break;
// 		}
// 		if (stHeadRecv.datalen != 0)
// 		{
// 			CROSS_TRACE(">>>GetIOTServerIP Error<<< 5");
// 			break;
// 		}
// 		//Encode "key"
// 		char szTempSend[1024] = { 0 };
// 		ST_SREVER_IOT _iot;
// 		memset(&_iot, 0, sizeof(_iot));
// 
// #if 1
// 		CROSS_STRCPY(_iot.useremail, m_szUser);
// 		char dest[256] = { 0 };
// 		XDESEncode("wjm", stHeadRecv.session, dest);
// 		nSession = stHeadRecv.session;
// 		CROSS_STRCPY(_iot.dessession, dest);//<<-------------------------
// 		t.cmd = Z_CMD_OF_SERVER_IOT_USER_ROTER;
// #else
// 		CROSS_STRCPY(_iot.devuuid, "WJM0A00000001SAXZSA");
// 		char dest[256] = { 0 };
// 		XDESEncode("wjm", stHeadRecv.session, dest);
// 		nSession = stHeadRecv.session;
// 		CROSS_STRCPY(_iot.dessession, dest);//<<-------------------------
// 		t.cmd = Z_CMD_OF_SERVER_IOT_DEVICE_ROTER;
// #endif
// 
// 
// 
// 
// 		//char mkdest[256] = { 0 };
// 		//unsigned int mkmk = 0;
// 		//XDESDecode(_login.pwd, mkdest, mkmk);
// 
// 		t.datalen = sizeof(ST_SREVER_IOT);
// 		t.session = nSession;
// 		memcpy(szTempSend, &t, sizeof(ST_SERVER_HEAD));
// 		memcpy(szTempSend + sizeof(ST_SERVER_HEAD), &_iot, sizeof(ST_SREVER_IOT));
// 		if (0 != pTcpStream->SyncWriteAndRead(szTempSend, sizeof(ST_SERVER_HEAD) + sizeof(ST_SREVER_IOT), &stHeadRecv, sizeof(ST_SERVER_HEAD)))
// 		{
// 			CROSS_TRACE(">>>GetIOTServerIP Error<<< 6");
// 			break;
// 		}
// #if 1
// 		if (stHeadRecv.cmd != Z_CMD_OF_SERVER_IOT_USER_ROTER)
// 		{
// 			CROSS_TRACE(">>>GetIOTServerIP Error<<< 7");
// 			break;
// 		}
// #else
// 		if (stHeadRecv.cmd != Z_CMD_OF_SERVER_IOT_DEVICE_ROTER)
// 		{
// 			CROSS_TRACE(">>>GetIOTServerIP Error<<< 7");
// 			break;
// 		}
// #endif
// 
// 		if (stHeadRecv.result != _CMD_OF_SERVER_RESULT_OK)
// 		{
// 			CROSS_TRACE(">>>GetIOTServerIP Error<<< 8");
// 			break;
// 		}
// 		if (stHeadRecv.datalen != sizeof(ST_SREVER_IOT_RESULT))
// 		{
// 			CROSS_TRACE(">>>GetIOTServerIP Error<<< 9");
// 			break;
// 		}
// 		if (stHeadRecv.datalen == sizeof(ST_SREVER_IOT_RESULT))
// 		{
// 			ST_SREVER_IOT_RESULT stResult;
// 			memset(&stResult, 0, sizeof(ST_SREVER_IOT_RESULT));
// 
// 			if (0 != pTcpStream->SyncWriteAndRead(NULL, 0, &stResult, stHeadRecv.datalen))
// 			{
// 				CROSS_TRACE(">>>GetIOTServerIP Error<<< 10");
// 				break;
// 			}
// 
// 			SetIOTServerIP(stResult.result);
// 		}
// 
// 		ret = 0;
// 		CROSS_TRACE(">>>GetIOTServerIP<<< okokok");
// 	} while (FALSE);
// 	//
// 	if (pTcpStream)
// 	{
// 		pTcpStream->Release();
// 		pTcpStream = NULL;
// 	}
// 	//
// 	//m_cs.Unlock();
	return (0 == ret) ? m_szIOTServerIp:nullptr;
}


int CNetToServer::WorkerThread(void* param)
{
	CNetToServer * p = (CNetToServer *)param;
	p->Worker();
	return 0;
}
void CNetToServer::Worker()
{
	CROSS_DWORD64 dwHeartTick = CrossGetTickCount64();//心跳
	//BOOL bLogin = FALSE;

	while (!m_bWorkerStop)
	{
		//-------------------------------------------------------
		//登陆服务器
		if (!m_bLoginOK)
		{
			//m_pCallback->OnIOTMessageCallback(EM_IOT_CALLBACK_TYPE_CONNECTING, NULL, 0);
			_Login(4);
			if (!m_bLoginOK)
			{
				//m_pCallback->OnIOTMessageCallback(EM_IOT_CALLBACK_TYPE_CONNECTFAILED, NULL, 0);
			}
			else
			{
				//m_pCallback->OnIOTMessageCallback(EM_IOT_CALLBACK_TYPE_CONNECTED, NULL, 0);
				dwHeartTick = CrossGetTickCount64();
			}
			CrossSleep(100);
			continue;
		}
		//---------------------------------------------------------
		//检测链接的存活
		if ((int)(CrossGetTickCount64() - m_dwLastTick) > (1000 * 1000))
		{
			//m_pCallback->OnIOTMessageCallback(EM_IOT_CALLBACK_TYPE_DISCONNECTED, NULL, 0);
			m_bLoginOK = FALSE;
			continue;
		}
		//---------------------------------------------------------
		//心跳
		if ((CrossGetTickCount64() - dwHeartTick) > 5 * 1000)
		{
			dwHeartTick = CrossGetTickCount64();
			AddSendMessge(Z_CMD_OF_SERVER_HEARBEAT, NULL, 0);
		}
		//---------------------------------------------------------
		CrossSleep(100);
	}
}

BOOL CNetToServer::_Login(int waitsec)
{
// 
// 	m_bLoginOK = FALSE;
// 	do
// 	{
// 		CROSS_TRACE("CMediaDevInterface::Login ------------------------- 1");
// 		//--------------------------------------------------------------------------------------------------------------------------
// 		//关闭网络
// 		if (m_pTcpStream)
// 		{
// 			m_pTcpStream->Release();
// 			m_pTcpStream = NULL;
// 		}
// 		CROSS_TRACE("CMediaDevInterface::Login ------------------------- 2");
// 		//--------------------------------------------------------------------------------------------------------------------------
// 		//清理消息
// 		while (m_msgList.size() > 0)
// 		{
// 			MyStruct4NetToServer * p = m_msgList.front();
// 			m_msgList.pop_front();
// 			delete p;
// 		}
// 		CROSS_TRACE("CMediaDevInterface::Login ------------------------- 4");
// 		//--------------------------------------------------------------------------------------------------------------------------
// 		//链接服务器
// 		m_pTcpStream = XNetCreateStream4Connect(m_szIOTServerIp, 6500, waitsec);
// 		CROSS_TRACE("CMediaDevInterface::Login ------------------------- 4-1");
// 		if (0 != XNetConnectStream(m_pTcpStream))
// 		{
// 			CROSS_TRACE("CMediaDevInterface::Login -- conn svr error");
// 			break;
// 		}
// 		CROSS_TRACE("CMediaDevInterface::Login ------------------------- 5");
// 		//--------------------------------------------------------------------------------------------------------------------------
// 		//发送Z_CMD_OF_SERVER_LOGIN_NEED_SESSIONID
// 		ST_SERVER_HEAD msg_key_send;
// 		ST_SERVER_HEAD msg_key_recv;
// 		memset(&msg_key_send, 0, sizeof(ST_SERVER_HEAD));
// 		memset(&msg_key_recv, 0, sizeof(ST_SERVER_HEAD));
// 		msg_key_send.cmd = SINGNALING_CHANNEL_CMD_LOGIN_NEED_SESSION;
// 		//
// 		if (0 != m_pTcpStream->SyncWriteAndRead(&msg_key_send, sizeof(ST_SERVER_HEAD), &msg_key_recv, sizeof(ST_SERVER_HEAD), 10))
// 		{
// 			CROSS_TRACE("CMediaDevInterface::Login -- request key error -- 1");
// 			break;
// 		}
// 		//
// 		if ((msg_key_recv.cmd != SINGNALING_CHANNEL_CMD_LOGIN_NEED_SESSION) || (msg_key_recv.result != _CMD_OF_SERVER_RESULT_OK))
// 		{
// 			CROSS_TRACE("CMediaDevInterface::Login -- request key error -- 2");
// 			break;
// 		}
// 
// 		CROSS_TRACE("CMediaDevInterface::Login ------------------------- 6");
// 		//
// 		m_nSessionID = msg_key_recv.session;
// 
// 		//--------------------------------------------------------------------------------------------------------------------------
// 		//发送Z_CMD_OF_SERVER_LOGIN,  使用key加密，登陆
// 		char enc_pwd[256] = { 0 };
// 		XDESEncode(m_szPwd, m_nSessionID, enc_pwd);
// 		//
// 		char szSendBuffer[1024] = { 0 };
// 		ST_SREVER_IOT msg_login_info;
// 		ST_SERVER_HEAD msg_login_send;
// 		ST_SERVER_HEAD msg_login_recv;
// 		memset(&msg_login_info, 0, sizeof(ST_SREVER_IOT));
// 		memset(&msg_login_send, 0, sizeof(ST_SERVER_HEAD));
// 		memset(&msg_login_recv, 0, sizeof(ST_SERVER_HEAD));
// 		//
// 		msg_login_send.cmd = SINGNALING_CHANNEL_CMD_LOGIN;
// 		msg_login_send.seq = m_nSeq++;
// 		msg_login_send.session = m_nSessionID;
// 		msg_login_send.datalen = sizeof(ST_SREVER_IOT);
// 		//
// 		//msg_login_info.dev_id = m_stDevice.dev_id;
// 		//msg_login_info.dev_channel = 0;
// 		memcpy(msg_login_info.useremail, m_szUser, strlen(m_szUser));
// 		memcpy(msg_login_info.userdespwd, enc_pwd, strlen(enc_pwd));
// 		//
// 		memcpy(szSendBuffer, &msg_login_send, sizeof(ST_SERVER_HEAD));
// 		memcpy(szSendBuffer + sizeof(ST_SERVER_HEAD), &msg_login_info, sizeof(ST_SREVER_IOT));
// 		//
// 		if (0 != m_pTcpStream->SyncWriteAndRead(szSendBuffer, sizeof(ST_SERVER_HEAD) + sizeof(ST_SREVER_IOT), &msg_login_recv, sizeof(ST_SERVER_HEAD)))
// 		{
// 			CROSS_TRACE("CMediaDevInterface::Login -- login error -- 1");
// 			break;
// 		}
// 		//
// 		if ((msg_login_recv.cmd != Z_CMD_OF_SERVER_IOT_LOGIN_AS_CMD) || (msg_login_recv.result != _CMD_OF_SERVER_RESULT_OK))
// 		{
// 			CROSS_TRACE("CMediaDevInterface::Login -- login error -- 2");
// 			break;
// 		}
// 		CROSS_TRACE("CMediaDevInterface::Login ------------------------- 7");
// 		
// 		m_dwLastTick = CrossGetTickCount64();
// 		m_bLoginOK = TRUE;
// 		m_pTcpStream->SetStreamData(this);
// 		CROSS_TRACE("CMediaDevInterface::Login ------------------------- 8");
// 
// 	} while (0);
// 
// 	CROSS_TRACE("CMediaDevInterface::Login ------------------------- 9");
// 	if (!m_bLoginOK)
// 	{
// 		if (m_pTcpStream)
// 		{
// 			m_pTcpStream->Release();
// 			m_pTcpStream = NULL;
// 		}
// 	}
// 
// 	CROSS_TRACE("CMediaDevInterface::Login ------------------------- 10");

	return m_bLoginOK;
}


void CNetToServer::AddSendMessge(int type, char *buffer, int len)
{
// 	if (!m_bLoginOK)
// 	{
// 		return;//没有正式登陆，不允许其他线程发送数据
// 	}
// 
// 	//
// 	MyStruct4NetToServer * pMsg = new MyStruct4NetToServer;
// 	memset(pMsg, 0, sizeof(MyStruct4NetToServer));
// 
// 	pMsg->head.cmd = type;
// 	pMsg->head.datalen = len;
// 	pMsg->head.seq = m_nSeq++;
// 	pMsg->head.session = m_nSessionID;
// 
// 	if (len > 0)
// 	{
// 		memcpy(pMsg->buffer, buffer, len);
// 	}
// 	pMsg->nSendLen = sizeof(ST_SERVER_HEAD) + len;
// 
// 	//m_csMsglist.Lock();
// 	m_msgList.push_back(pMsg);
// 	//m_csMsglist.Unlock();
}
void CNetToServer::DoMsg()
{
// 	m_dwLastTick = CrossGetTickCount64();
// 	if (Z_CMD_OF_SERVER_HEARBEAT == m_head.cmd)
// 	{
// 		CROSS_TRACE("Z_CMD_OF_SERVER_HEARBEAT == m_head.cmd");
// 	}
// 
// 	if (Z_CMD_OF_SERVER_TRANS_TRANSMISSION == m_head.cmd)
// 	{
// 		if (m_pCallback)
// 		{
// 			//m_pCallback->OnIOTMessageCallback(EM_IOT_CALLBACK_TYPE_TRANS_TRANSMISSION,)
// 		}
// 	}
// 	if (Z_CMD_OF_SERVER_IOT_GET_DEVICES == m_head.cmd)
// 	{
// 		if (m_pCallback)
// 		{
// 			m_pCallback->OnIOTMessageCallback(EM_IOT_CALLBACK_TYPE_GET_DEVICES, m_pBuffer, m_dataLen);
// 		}
// 	}
// 	if (Z_CMD_OF_SERVER_IOT_ADD_DEVICE == m_head.cmd)
// 	{
// 		if (m_pCallback)
// 		{
// 			//m_pCallback->OnIOTMessageCallback(EM_IOT_CALLBACK_TYPE_ADD_DEVICE,)
// 		}
// 	}
// 	if (Z_CMD_OF_SERVER_IOT_DELETE_DEVICES == m_head.cmd)
// 	{
// 		if (m_pCallback)
// 		{
// 			//m_pCallback->OnIOTMessageCallback(EM_IOT_CALLBACK_TYPE_DELETE_DEVICE,)
// 		}
// 	}
// 	if (Z_CMD_OF_SERVER_IOT_OPENHIS == m_head.cmd)
// 	{
// 		if (m_pCallback)
// 		{
// 			//m_pCallback->OnIOTMessageCallback(EM_IOT_CALLBACK_TYPE_OPENHLS,)
// 		}
// 	}
// 	if (Z_CMD_OF_SERVER_IOT_PICTURE_DATA == m_head.cmd)
// 	{
// 		if (m_pCallback)
// 		{
// 			//m_pCallback->OnIOTMessageCallback(EM_IOT_CALLBACK_TYPE_PICTURE_DATA,)
// 		}
// 	}
// 	if (Z_CMD_OF_SERVER_IOT_SENSOR_DATA == m_head.cmd)
// 	{
// 		if (m_pCallback)
// 		{
// 			//m_pCallback->OnIOTMessageCallback(EM_IOT_CALLBACK_TYPE_SENSOR_DATA,)
// 		}
// 	}
// 	if (Z_CMD_OF_SERVER_IOT_ALARM_DATA == m_head.cmd)
// 	{
// 		if (m_pCallback)
// 		{
// 			//m_pCallback->OnIOTMessageCallback(EM_IOT_CALLBACK_TYPE_ALARM_DATA,)
// 		}
// 	}
// 	if (Z_CMD_OF_SERVER_IOT_GPS_DATA == m_head.cmd)
// 	{
// 		if (m_pCallback)
// 		{
// 			//m_pCallback->OnIOTMessageCallback(EM_IOT_CALLBACK_TYPE_GPS_DATA,)
// 		}
// 	}
	


}

