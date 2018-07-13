#pragma once

#include "PlatformDefine.h"
#include "XStream.hpp"
#include "MediaInterfaceDefine.h"

#ifdef _PLATFORM_ANDROID
#include "XDes.h"
#endif

// #ifndef MAX_XML_BUFFER_SIZE
// #define MAX_XML_BUFFER_SIZE		(2*1024*1024)
// #endif

// Short connection
class NetToSmartLifeServer
{
public:
	NetToSmartLifeServer();
	~NetToSmartLifeServer();
public:
	static NetToSmartLifeServer& Instance();

public:
	int UserRegister(const char *ip, int port, const char* user, const char * pwd);
	int UserChangePwd(const char *ip, int port, const char* user, const char * pwd, const char * newpwd);
	int UserAddDevice(const char *ip, int port, const char* user, const char * pwd, const char * uuid);
	int UserDeleteDevice(const char *ip, int port, const char* user, const char * pwd, const char * uuid);

private:
	CrossCriticalSection m_cs;
	 
};


NetToSmartLifeServer::NetToSmartLifeServer()
{
}


NetToSmartLifeServer::~NetToSmartLifeServer()
{
}
NetToSmartLifeServer& NetToSmartLifeServer::Instance()
{
	static NetToSmartLifeServer inc;
	return inc;
}
int NetToSmartLifeServer::UserRegister(const char *ip, int port, const char* user, const char * pwd)
{
	int m_Session;

	int ret = -1;
	ST_SERVER_HEAD stHeadRecv;

// 	m_cs.Lock();
// 	CXNetStream* m_pTcpStream = NULL;
// 
// 	do
// 	{
// 		CROSS_TRACE(">>>Login server<<< cc");
// 		m_pTcpStream = XNetCreateStream4Connect(ip, port, 3);
// 		if (NULL == m_pTcpStream)
// 		{
// 			break;
// 		}
// 		CROSS_TRACE(">>>Login server<<< cc1");
// 		if (0 != XNetConnectStream(m_pTcpStream))
// 		{
// 			break;
// 		}
// 		//recv key
// 		ST_SERVER_HEAD t;
// 		memset(&t, 0, sizeof(t));
// 		t.cmd = Z_CMD_OF_SERVER_LOGIN;
// 		t.datalen = 0;
// 		//send "need session"
// 		if (0 != m_pTcpStream->SyncWriteAndRead(&t, sizeof(ST_SERVER_HEAD), &stHeadRecv, sizeof(ST_SERVER_HEAD)))
// 		{
// 			CROSS_TRACE(">>>Login server<<< 111");
// 			break;
// 		}
// 		if (stHeadRecv.cmd != Z_CMD_OF_SERVER_LOGIN_NEED_SESSIONID)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 222");
// 			break;
// 		}
// 
// 		if (stHeadRecv.datalen != 0)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 333");
// 			break;
// 		}
// 		//Encode "key"
// 		char szTempSend[1024] = { 0 };
// 		ST_SREVER_LOGIN_INFO _login;
// 		memset(&_login, 0, sizeof(_login));
// 
// 		CROSS_STRCPY(_login.user, user);
// 
// 		char dest[256] = { 0 };
// 		XDESEncode(pwd, stHeadRecv.session, dest);
// 		m_Session = stHeadRecv.session;
// 
// 		CROSS_STRCPY(_login.pwd, dest);//<<-------------------------
// 
// 		//char mkdest[256] = { 0 };
// 		//unsigned int mkmk = 0;
// 		//XDESDecode(_login.pwd, mkdest, mkmk);
// 
// 		t.cmd = Z_CMD_OF_SERVER_LOGIN;
// 		t.reserve = Z_CMD_OF_SERVER_SMARTLIFE_ACCOUNT_REGISTER;
// 		t.datalen = sizeof(ST_SREVER_LOGIN_INFO);
// 		t.session = m_Session;
// 
// 		memcpy(szTempSend, &t, sizeof(ST_SERVER_HEAD));
// 		memcpy(szTempSend + sizeof(ST_SERVER_HEAD), &_login, sizeof(ST_SREVER_LOGIN_INFO));
// 		if (0 != m_pTcpStream->SyncWriteAndRead(szTempSend, sizeof(ST_SERVER_HEAD) + sizeof(ST_SREVER_LOGIN_INFO), &stHeadRecv, sizeof(ST_SERVER_HEAD)))
// 		{
// 			CROSS_TRACE(">>>Login server<<< 444");
// 			break;
// 		}
// 		if (stHeadRecv.cmd != Z_CMD_OF_SERVER_SMARTLIFE_ACCOUNT_REGISTER)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 555");
// 			break;
// 		}
// 
// 		if (stHeadRecv.result == _CMD_OF_SERVER_RESULT_SMARTLIFE_REGISTER_ERROR)
// 		{
// 			CROSS_TRACE(">>>_CMD_OF_SERVER_RESULT_SMARTLIFE_REGISTER_ERROR  _CMD_OF_SERVER_RESULT_SMARTLIFE_REGISTER_ERROR<<<");
// 			ret = 333;
// 			break;
// 		}
// 
// 		if (stHeadRecv.result != _CMD_OF_SERVER_RESULT_OK)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 666");
// 			break;
// 		}
// 
// 		ret = 0;
// 		CROSS_TRACE(">>>Login server<<< okokok");
// 	} while (FALSE);
// 
// 	if (m_pTcpStream)
// 	{
// 		m_pTcpStream->Release();
// 		m_pTcpStream = NULL;
// 	}

	m_cs.Unlock();
	return ret;
}
int NetToSmartLifeServer::UserChangePwd(const char *ip, int port, const char* user, const char * pwd, const char * newpwd)
{
	int m_Session;

	int ret = -1;
	ST_SERVER_HEAD stHeadRecv;

// 	m_cs.Lock();
// 	CXNetStream* m_pTcpStream = NULL;
// 
// 	do
// 	{
// 		CROSS_TRACE(">>>Login server<<< cc");
// 		m_pTcpStream = XNetCreateStream4Connect(ip, port, 3);
// 		if (NULL == m_pTcpStream)
// 		{
// 			break;
// 		}
// 		CROSS_TRACE(">>>Login server<<< cc1");
// 		if (0 != XNetConnectStream(m_pTcpStream))
// 		{
// 			break;
// 		}
// 		//recv key
// 		ST_SERVER_HEAD t;
// 		memset(&t, 0, sizeof(t));
// 		t.cmd = Z_CMD_OF_SERVER_LOGIN;
// 		t.datalen = 0;
// 		//send "need session"
// 		if (0 != m_pTcpStream->SyncWriteAndRead(&t, sizeof(ST_SERVER_HEAD), &stHeadRecv, sizeof(ST_SERVER_HEAD)))
// 		{
// 			CROSS_TRACE(">>>Login server<<< 111");
// 			break;
// 		}
// 		if (stHeadRecv.cmd != Z_CMD_OF_SERVER_LOGIN_NEED_SESSIONID)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 222");
// 			break;
// 		}
// 
// 		if (stHeadRecv.datalen != 0)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 333");
// 			break;
// 		}
// 		//Encode "key"
// 		char szTempSend[1024] = { 0 };
// 		ST_SREVER_LOGIN_INFO _login;
// 		memset(&_login, 0, sizeof(_login));
// 
// 		CROSS_STRCPY(_login.user, user);
// 
// 		char dest[256] = { 0 };
// 		XDESEncode(pwd, stHeadRecv.session, dest);
// 		m_Session = stHeadRecv.session;
// 
// 		CROSS_STRCPY(_login.pwd, dest);//<<-------------------------
// 
// 		//char mkdest[256] = { 0 };
// 		//unsigned int mkmk = 0;
// 		//XDESDecode(_login.pwd, mkdest, mkmk);
// 
// 		t.cmd = Z_CMD_OF_SERVER_LOGIN;
// 		t.reserve = 0;
// 		t.datalen = sizeof(ST_SREVER_LOGIN_INFO);
// 		t.session = m_Session;
// 
// 		memcpy(szTempSend, &t, sizeof(ST_SERVER_HEAD));
// 		memcpy(szTempSend + sizeof(ST_SERVER_HEAD), &_login, sizeof(ST_SREVER_LOGIN_INFO));
// 		if (0 != m_pTcpStream->SyncWriteAndRead(szTempSend, sizeof(ST_SERVER_HEAD) + sizeof(ST_SREVER_LOGIN_INFO), &stHeadRecv, sizeof(ST_SERVER_HEAD)))
// 		{
// 			CROSS_TRACE(">>>Login server<<< 444");
// 			break;
// 		}
// 		if (stHeadRecv.cmd != Z_CMD_OF_SERVER_LOGIN)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 555");
// 			break;
// 		}
// 		if (stHeadRecv.result != _CMD_OF_SERVER_RESULT_OK)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 666");
// 			break;
// 		}
// 		//-----------------------------------------------------------------------------------------------------------------
// 
// 
// 		t.cmd = Z_CMD_OF_SERVER_SMARTLIFE_ACCOUNT_CHANGE_PWD;
// 		t.session = m_Session;
// 		t.datalen = sizeof(ST_SREVER_SMARTLIFE_CHANGEPWD);
// 
// 		ST_SREVER_SMARTLIFE_CHANGEPWD stPwd;
// 		memset(&stPwd, 0, sizeof(ST_SREVER_SMARTLIFE_CHANGEPWD));
// 		memcpy(stPwd.newpwd, newpwd, strlen(newpwd));
// 
// 		char sTmp[1024] = { 0 };
// 		memcpy(sTmp, &t, sizeof(ST_SERVER_HEAD));
// 		memcpy(sTmp + sizeof(ST_SERVER_HEAD), &stPwd, sizeof(ST_SREVER_SMARTLIFE_CHANGEPWD));
// 
// 		if (0 != m_pTcpStream->SyncWriteAndRead(sTmp, sizeof(ST_SERVER_HEAD) + sizeof(ST_SREVER_SMARTLIFE_CHANGEPWD), &stHeadRecv, sizeof(ST_SERVER_HEAD)))
// 		{
// 			CROSS_TRACE(">>>Login server<<< 777");
// 			break;
// 		}
// 		if (stHeadRecv.cmd != Z_CMD_OF_SERVER_SMARTLIFE_ACCOUNT_CHANGE_PWD)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 888");
// 			break;
// 		}
// 		if (stHeadRecv.result != _CMD_OF_SERVER_RESULT_OK)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 999");
// 			break;
// 		}
// 
// 
// 
// 		ret = 0;
// 		CROSS_TRACE(">>>UserChangePwd server<<< okokok");
// 	} while (FALSE);
// 
// 	if (m_pTcpStream)
// 	{
// 		m_pTcpStream->Release();
// 		m_pTcpStream = NULL;
// 	}
// 
// 	m_cs.Unlock();
	return ret;
}
int NetToSmartLifeServer::UserAddDevice(const char *ip, int port, const char* user, const char * pwd, const char * uuid)
{
	int m_Session;

	int ret = -1;
	ST_SERVER_HEAD stHeadRecv;

// 	m_cs.Lock();
// 	CXNetStream* m_pTcpStream = NULL;
// 
// 	do
// 	{
// 		CROSS_TRACE(">>>Login server<<< cc");
// 		m_pTcpStream = XNetCreateStream4Connect(ip, port, 3);
// 		if (NULL == m_pTcpStream)
// 		{
// 			break;
// 		}
// 		CROSS_TRACE(">>>Login server<<< cc1");
// 		if (0 != XNetConnectStream(m_pTcpStream))
// 		{
// 			break;
// 		}
// 		//recv key
// 		ST_SERVER_HEAD t;
// 		memset(&t, 0, sizeof(t));
// 		t.cmd = Z_CMD_OF_SERVER_LOGIN;
// 		t.datalen = 0;
// 		//send "need session"
// 		if (0 != m_pTcpStream->SyncWriteAndRead(&t, sizeof(ST_SERVER_HEAD), &stHeadRecv, sizeof(ST_SERVER_HEAD)))
// 		{
// 			CROSS_TRACE(">>>Login server<<< 111");
// 			break;
// 		}
// 		if (stHeadRecv.cmd != Z_CMD_OF_SERVER_LOGIN_NEED_SESSIONID)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 222");
// 			break;
// 		}
// 
// 		if (stHeadRecv.datalen != 0)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 333");
// 			break;
// 		}
// 		//Encode "key"
// 		char szTempSend[1024] = { 0 };
// 		ST_SREVER_LOGIN_INFO _login;
// 		memset(&_login, 0, sizeof(_login));
// 
// 		CROSS_STRCPY(_login.user, user);
// 
// 		char dest[256] = { 0 };
// 		XDESEncode(pwd, stHeadRecv.session, dest);
// 		m_Session = stHeadRecv.session;
// 
// 		CROSS_STRCPY(_login.pwd, dest);//<<-------------------------
// 
// 		//char mkdest[256] = { 0 };
// 		//unsigned int mkmk = 0;
// 		//XDESDecode(_login.pwd, mkdest, mkmk);
// 
// 		t.cmd = Z_CMD_OF_SERVER_LOGIN;
// 		t.reserve = 0;
// 		t.datalen = sizeof(ST_SREVER_LOGIN_INFO);
// 		t.session = m_Session;
// 
// 		memcpy(szTempSend, &t, sizeof(ST_SERVER_HEAD));
// 		memcpy(szTempSend + sizeof(ST_SERVER_HEAD), &_login, sizeof(ST_SREVER_LOGIN_INFO));
// 		if (0 != m_pTcpStream->SyncWriteAndRead(szTempSend, sizeof(ST_SERVER_HEAD) + sizeof(ST_SREVER_LOGIN_INFO), &stHeadRecv, sizeof(ST_SERVER_HEAD)))
// 		{
// 			CROSS_TRACE(">>>Login server<<< 444");
// 			break;
// 		}
// 		if (stHeadRecv.cmd != Z_CMD_OF_SERVER_LOGIN)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 555");
// 			break;
// 		}
// 		if (stHeadRecv.result != _CMD_OF_SERVER_RESULT_OK)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 666");
// 			break;
// 		}
// 		//-----------------------------------------------------------------------------------------------------------------
// 
// 
// 		t.cmd = Z_CMD_OF_SERVER_SMARTLIFE_ACCOUNT_ADD_DEVICE;
// 		t.session = m_Session;
// 		t.datalen = sizeof(ST_SREVER_SMARTLIFE_DEVICE);
// 
// 		ST_SREVER_SMARTLIFE_DEVICE stUUID;
// 		memset(&stUUID, 0, sizeof(ST_SREVER_SMARTLIFE_DEVICE));
// 		memcpy(stUUID.devuuid, uuid, strlen(uuid));
// 
// 		char sTmp[1024] = { 0 };
// 		memcpy(sTmp, &t, sizeof(ST_SERVER_HEAD));
// 		memcpy(sTmp + sizeof(ST_SERVER_HEAD), &stUUID, sizeof(ST_SREVER_SMARTLIFE_DEVICE));
// 
// 		if (0 != m_pTcpStream->SyncWriteAndRead(sTmp, sizeof(ST_SERVER_HEAD) + sizeof(ST_SREVER_SMARTLIFE_DEVICE), &stHeadRecv, sizeof(ST_SERVER_HEAD)))
// 		{
// 			CROSS_TRACE(">>>Login server<<< 777");
// 			break;
// 		}
// 		if (stHeadRecv.cmd != Z_CMD_OF_SERVER_SMARTLIFE_ACCOUNT_ADD_DEVICE)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 888");
// 			break;
// 		}
// 		if (stHeadRecv.result != _CMD_OF_SERVER_RESULT_OK)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 999");
// 			break;
// 		}
// 
// 
// 
// 		ret = 0;
// 		CROSS_TRACE(">>>UserChangePwd server<<< okokok");
// 	} while (FALSE);
// 
// 	if (m_pTcpStream)
// 	{
// 		m_pTcpStream->Release();
// 		m_pTcpStream = NULL;
// 	}
// 
// 	m_cs.Unlock();
	return ret;
}
int NetToSmartLifeServer::UserDeleteDevice(const char *ip, int port, const char* user, const char * pwd, const char * uuid)
{
	int m_Session;

	int ret = -1;
	ST_SERVER_HEAD stHeadRecv;

// 	m_cs.Lock();
// 	CXNetStream* m_pTcpStream = NULL;
// 
// 	do
// 	{
// 		CROSS_TRACE(">>>Login server<<< cc");
// 		m_pTcpStream = XNetCreateStream4Connect(ip, port, 3);
// 		if (NULL == m_pTcpStream)
// 		{
// 			break;
// 		}
// 		CROSS_TRACE(">>>Login server<<< cc1");
// 		if (0 != XNetConnectStream(m_pTcpStream))
// 		{
// 			break;
// 		}
// 		//recv key
// 		ST_SERVER_HEAD t;
// 		memset(&t, 0, sizeof(t));
// 		t.cmd = Z_CMD_OF_SERVER_LOGIN;
// 		t.datalen = 0;
// 		//send "need session"
// 		if (0 != m_pTcpStream->SyncWriteAndRead(&t, sizeof(ST_SERVER_HEAD), &stHeadRecv, sizeof(ST_SERVER_HEAD)))
// 		{
// 			CROSS_TRACE(">>>Login server<<< 111");
// 			break;
// 		}
// 		if (stHeadRecv.cmd != Z_CMD_OF_SERVER_LOGIN_NEED_SESSIONID)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 222");
// 			break;
// 		}
// 
// 		if (stHeadRecv.datalen != 0)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 333");
// 			break;
// 		}
// 		//Encode "key"
// 		char szTempSend[1024] = { 0 };
// 		ST_SREVER_LOGIN_INFO _login;
// 		memset(&_login, 0, sizeof(_login));
// 
// 		CROSS_STRCPY(_login.user, user);
// 
// 		char dest[256] = { 0 };
// 		XDESEncode(pwd, stHeadRecv.session, dest);
// 		m_Session = stHeadRecv.session;
// 
// 		CROSS_STRCPY(_login.pwd, dest);//<<-------------------------
// 
// 		//char mkdest[256] = { 0 };
// 		//unsigned int mkmk = 0;
// 		//XDESDecode(_login.pwd, mkdest, mkmk);
// 
// 		t.cmd = Z_CMD_OF_SERVER_LOGIN;
// 		t.reserve = 0;
// 		t.datalen = sizeof(ST_SREVER_LOGIN_INFO);
// 		t.session = m_Session;
// 
// 		memcpy(szTempSend, &t, sizeof(ST_SERVER_HEAD));
// 		memcpy(szTempSend + sizeof(ST_SERVER_HEAD), &_login, sizeof(ST_SREVER_LOGIN_INFO));
// 		if (0 != m_pTcpStream->SyncWriteAndRead(szTempSend, sizeof(ST_SERVER_HEAD) + sizeof(ST_SREVER_LOGIN_INFO), &stHeadRecv, sizeof(ST_SERVER_HEAD)))
// 		{
// 			CROSS_TRACE(">>>Login server<<< 444");
// 			break;
// 		}
// 		if (stHeadRecv.cmd != Z_CMD_OF_SERVER_LOGIN)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 555");
// 			break;
// 		}
// 		if (stHeadRecv.result != _CMD_OF_SERVER_RESULT_OK)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 666");
// 			break;
// 		}
// 		//-----------------------------------------------------------------------------------------------------------------
// 
// 
// 		t.cmd = Z_CMD_OF_SERVER_SMARTLIFE_ACCOUNT_DELETE_DEVICE;
// 		t.session = m_Session;
// 		t.datalen = sizeof(ST_SREVER_SMARTLIFE_DEVICE);
// 
// 		ST_SREVER_SMARTLIFE_DEVICE stUUID;
// 		memset(&stUUID, 0, sizeof(ST_SREVER_SMARTLIFE_DEVICE));
// 		memcpy(stUUID.devuuid, uuid, strlen(uuid));
// 
// 		char sTmp[1024] = { 0 };
// 		memcpy(sTmp, &t, sizeof(ST_SERVER_HEAD));
// 		memcpy(sTmp + sizeof(ST_SERVER_HEAD), &stUUID, sizeof(ST_SREVER_SMARTLIFE_DEVICE));
// 
// 		if (0 != m_pTcpStream->SyncWriteAndRead(sTmp, sizeof(ST_SERVER_HEAD) + sizeof(ST_SREVER_SMARTLIFE_DEVICE), &stHeadRecv, sizeof(ST_SERVER_HEAD)))
// 		{
// 			CROSS_TRACE(">>>Login server<<< 777");
// 			break;
// 		}
// 		if (stHeadRecv.cmd != Z_CMD_OF_SERVER_SMARTLIFE_ACCOUNT_DELETE_DEVICE)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 888");
// 			break;
// 		}
// 		if (stHeadRecv.result != _CMD_OF_SERVER_RESULT_OK)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 999");
// 			break;
// 		}
// 
// 
// 
// 		ret = 0;
// 		CROSS_TRACE(">>>UserChangePwd server<<< okokok");
// 	} while (FALSE);
// 
// 	if (m_pTcpStream)
// 	{
// 		m_pTcpStream->Release();
// 		m_pTcpStream = NULL;
// 	}
// 
// 	m_cs.Unlock();
	return ret;
}



// 
// int NetToSmartLifeServer::GetUserDevices(const char *ip, int port, const char* user, const char * pwd, vector<ST_SREVER_DEVICE_INFO_BASE> &rs)
// {
// 
// 	int m_Session;
// 
// 	int ret = -1;
// 	ST_SERVER_HEAD stHeadRecv;
// 
// 	m_cs.Lock();
// 	CXNetStream* m_pTcpStream = NULL;
// 
// 	do
// 	{
// 		CROSS_TRACE(">>>Login server<<< cc");
// 		m_pTcpStream = XNetCreateStream4Connect(ip, port, 3);
// 		if (NULL == m_pTcpStream)
// 		{
// 			break;
// 		}
// 		CROSS_TRACE(">>>Login server<<< cc1");
// 		if (0 != XNetConnectStream(m_pTcpStream))
// 		{
// 			break;
// 		}
// 		//recv key
// 		ST_SERVER_HEAD t;
//         memset(&t,0,sizeof(t));
// 		t.cmd = Z_CMD_OF_SERVER_LOGIN;
// 		t.datalen = 0;
// 		//send "need session"
// 		if (0 != m_pTcpStream->SyncWriteAndRead(&t, sizeof(ST_SERVER_HEAD), &stHeadRecv, sizeof(ST_SERVER_HEAD)))
// 		{
// 			CROSS_TRACE(">>>Login server<<< 111");
// 			break;
// 		}
// 		if (stHeadRecv.cmd != Z_CMD_OF_SERVER_LOGIN_NEED_SESSIONID)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 222");
// 			break;
// 		}
// 
// 		if (stHeadRecv.datalen != 0)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 333");
// 			break;
// 		}
// 
// 
// 		//Encode "key"
// 		char szTempSend[1024] = { 0 };
// 		ST_SREVER_LOGIN_INFO _login;
// 		memset(&_login, 0, sizeof(_login));
// 
// 		CROSS_STRCPY(_login.user, user);
// 
// 		char dest[256] = { 0 };
// 		XDESEncode(pwd, stHeadRecv.session, dest);
// 		m_Session = stHeadRecv.session;
// 
// 		CROSS_STRCPY(_login.pwd, dest);//<<-------------------------
// 
//         //char mkdest[256] = { 0 };
//         //unsigned int mkmk = 0;
//         //XDESDecode(_login.pwd, mkdest, mkmk);
//         
//         
// 		t.cmd = Z_CMD_OF_SERVER_LOGIN;
// 		t.datalen = sizeof(ST_SREVER_LOGIN_INFO);
// 		t.session = m_Session;
// 
// 		memcpy(szTempSend, &t, sizeof(ST_SERVER_HEAD));
// 		memcpy(szTempSend + sizeof(ST_SERVER_HEAD), &_login, sizeof(ST_SREVER_LOGIN_INFO));
// 		if (0 != m_pTcpStream->SyncWriteAndRead(szTempSend, sizeof(ST_SERVER_HEAD) + sizeof(ST_SREVER_LOGIN_INFO), &stHeadRecv, sizeof(ST_SERVER_HEAD)))
// 		{
// 			CROSS_TRACE(">>>Login server<<< 444");
// 			break;
// 		}
// 		if (stHeadRecv.cmd != Z_CMD_OF_SERVER_LOGIN)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 555");
// 			break;
// 		}
// 		if (stHeadRecv.result != _CMD_OF_SERVER_RESULT_OK)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 666");
// 			break;
// 		}
// 
// 
// 		t.cmd = Z_CMD_OF_SERVER_GET_DEVICE_LIST;
// 		t.session = m_Session;
// 		t.datalen = 0;
// 		if (0 != m_pTcpStream->SyncWriteAndRead(&t, sizeof(ST_SERVER_HEAD), &stHeadRecv, sizeof(ST_SERVER_HEAD)))
// 		{
// 			CROSS_TRACE(">>>Login server<<< 777");
// 			break;
// 		}
// 		if (stHeadRecv.cmd != Z_CMD_OF_SERVER_GET_DEVICE_LIST)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 888");
// 			break;
// 		}
// 		if (stHeadRecv.result != _CMD_OF_SERVER_RESULT_OK)
// 		{
// 			CROSS_TRACE(">>>Login server<<< 999");
// 			break;
// 		}
// 
// 
// 		if (stHeadRecv.datalen > 0)
// 		{
// 			char *pBuffer = new char[stHeadRecv.datalen + 1];
// 			pBuffer[stHeadRecv.datalen] = '\0';
// 
// 			if (0 != m_pTcpStream->SyncWriteAndRead(NULL, 0, pBuffer, stHeadRecv.datalen))
// 			{
// 				CROSS_TRACE(">>>Login server<<< 101010");
// 				break;
// 			}
// 
// 			for (size_t i = 0; i < (stHeadRecv.datalen / sizeof(ST_SREVER_DEVICE_INFO_BASE)); i++)
// 			{
// 				ST_SREVER_DEVICE_INFO_BASE * _dev = (ST_SREVER_DEVICE_INFO_BASE *)(pBuffer + i*sizeof(ST_SREVER_DEVICE_INFO_BASE));
// 
// 				ST_SREVER_DEVICE_INFO_BASE st;
// 				memcpy(&st, _dev, sizeof(ST_SREVER_DEVICE_INFO_BASE));
// 
// 				rs.push_back(st);
// 			}
// 
// 			delete[] pBuffer;
// 		}
// 		ret = 0;
//         CROSS_TRACE(">>>Login server<<< okokok");
// 	} while (FALSE);
// 
// 	if (m_pTcpStream)
// 	{
// 		m_pTcpStream->Release();
// 		m_pTcpStream = NULL;
// 	}
// 
// 	m_cs.Unlock();
// 	return ret;
// 
// 
// 	return 0;
// 
// }
