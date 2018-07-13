#pragma once

#include "PlatformDefine.h"
#include "XStream.hpp"
#include "XDes.h"
#include "Pool.hpp"
#include "MediaInterface.h"


enum signaling_channel_cmd_def
{
	SINGNALING_CHANNEL_CMD_LOGIN = 0,				//��½
	SINGNALING_CHANNEL_CMD_LOGIN_NEED_SESSION,		//key s -> c
	SINGNALING_CHANNEL_CMD_HEARBEAT,				//����
	SINGNALING_CHANNEL_CMD_LOGOUT,					//ע��
	SINGNALING_CHANNEL_CMD_TRANS,					//͸������,��ʾ������Ϣ����
	//
	SINGNALING_CHANNEL_CMD_GPS,
};

struct signaling_channel_head_t
{
	//
	unsigned int cmd;//signaling_channel_cmd_def
	unsigned int session;
	unsigned int result;
	unsigned int datalen;
	//
	unsigned char msguuid[16];
	//
	unsigned int reserve[4];
};

#define NET_TO_SERVER_CMD_MSG_LEN (LITTLE_BUFFER_SIZE_KB - sizeof(signaling_channel_head_t))
struct MyStruct4NetToServer
{
	signaling_channel_head_t head;
	char msg[NET_TO_SERVER_CMD_MSG_LEN];
};

class CNetToServer :public CXNetStreamData
{
public:
	CNetToServer(CXIOTMessageCallback * pCallback, const char *pRedirectUrl, const char* szUser, const char* szPwd);
	~CNetToServer();

public:
	virtual void OnPacketCompleteNetStreamData(int32_t bytesTransferred, emXStreamTansferKey transferKey);

public:
	int Login();
	int Logout();
	int TansData(const char* szData, int nLen);;

public:
	int SetIOTServerIP(const char *iotip);
	char *  GetIOTServerIP();
	int GetUserDevice();
	int OpenHLS(const char * devuuid);
	int AddDevice(const char * devuuid);
	int DeleteDevice(const char * devuuid);
private:
	static int WorkerThread(void* param);
	void Worker();
	void AddSendMessge(int type, char *buffer, int len);
	void DoMsg();
	void Clear();
	BOOL _Login(int waitsec);
private:
	//CrossCriticalSection m_cs;
	//
	CXNetStream* m_pTcpStream;
	//
	char m_szRedirectUrl[128];
	char m_szUser[128];
	char m_szPwd[128];
	//
	char m_szIOTServerIp[128];
	CXIOTMessageCallback * m_pCallback;
	//
	CROSS_DWORD64 m_dwLastTick;
	uint32_t m_nSessionID;
	uint16_t m_nSeq;
	//
	volatile BOOL m_bLoginOK;
	//
	emNetRecieveStep m_nRcvStep;
	ST_SERVER_HEAD m_head;
	char *m_pBuffer;	//���յ����ݻ���
	unsigned int m_dataLen;	//���յ����ݳ���
private:

	CROSS_THREAD_HANDLE	m_hWorkerThread;
	volatile BOOL m_bWorkerStop;
	//CrossCriticalSection m_csMsglist;
	list<MyStruct4NetToServer *> m_msgList;

};

