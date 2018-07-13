#ifndef __x_classic_client_signaling_header_4567hbvcde5689oknbgtyuio
#define __x_classic_client_signaling_header_4567hbvcde5689oknbgtyuio

#include "PlatformDefine.h"
#include "XClientStructDefine.h"
#include "XStream.hpp"
#include "Pool.hpp"
//--------------------------------------------------------------------------------------------------------------------
#define ST_CLIENT_SIGNALING_BUFFER_LEN_LITTLE (LITTLE_BUFFER_SIZE_KB - sizeof(signaling_channel_head_t))
struct ST_SIGNALING_SEND_BUFFER
{
	signaling_channel_head_t head;
	uint8_t msg[ST_CLIENT_SIGNALING_BUFFER_LEN_LITTLE];
};
class CSignalingBufferPool
{
public:
	CSignalingBufferPool() {};
	~CSignalingBufferPool() {};
	static CSignalingBufferPool& Instance() {
		static CSignalingBufferPool inc;
		return inc;
	};
	ST_SIGNALING_SEND_BUFFER * malloc() {
		ST_SIGNALING_SEND_BUFFER * pBuffer = (ST_SIGNALING_SEND_BUFFER *)CLittleBufferPool::Instance().malloc();
		return pBuffer;
	}
	void free(ST_SIGNALING_SEND_BUFFER * p) {
		CLittleBufferPool::Instance().free((char *)p);
	}
};
//--------------------------------------------------------------------------------------------------------------------
class CXClientSignaling :public  CXNetStreamData
{
public:
	CXClientSignaling(CXSignalingChannelMessageCallback *cb);
	~CXClientSignaling();

public:
	virtual void OnPacketCompleteNetStreamData(int32_t bytesTransferred, emXStreamTansferKey transferKey);

public:
	void Delete();
	void Connect(char * s_ip, int port, char * user, char * pwd);
	void AddSendMessage(int type, char *buffer, int len);


	//
	void SignalingCtrl(const char * pszInBuffer, int nInLen, char *pszOutBuffer, int &nOutLen);

private:
	void Disconnect();
	BOOL IsNetError();//当前连接是否出错

private:
	volatile BOOL m_bIsError;//初始化为TRUE
	CXNetStream* m_pTcpStream;
	//
	CXSignalingChannelMessageCallback *m_pCallBack;
	unsigned int m_nSessionID;
	//
	signaling_channel_head_t m_head;
	char *m_pBuffer;
	emNetRecieveStep m_nRecvStep;
	void DoMsg();
	CROSS_DWORD64 m_dwLastTick;
	//
	CrossCriticalSection m_csSendList;//list多线程不安全
	list<ST_SIGNALING_SEND_BUFFER *> m_msgSendList;
	void ClearSendBufferList();
	//
	char * m_pSignalingCtrlBuffer;
	int m_nSignalingCtrlBufferLen;
	string m_strSignalingCtrlMsgUuid;
	volatile BOOL m_bSignalingCtrlFinished;
	CrossCriticalSection m_csSignalingCtrl;
private:
	volatile BOOL m_bTheadExit;
	CROSS_THREAD_HANDLE m_hThread;
	static int ThreadWorker(void * param);
	void Woker();
	//
	char m_sziotip[64];
	char m_szusername[64];
	void GetIotServerInfo();

private:
	char m_szCenterIp[64];
	int s_nCenterPort;
	char m_szUser[64];
	char m_szPwd[64];
	void Login();

private:
#if XINTERNET_TEST_STATUS
	volatile BOOL m_bCanReleaseSuccessCounter;
#endif
};

#endif