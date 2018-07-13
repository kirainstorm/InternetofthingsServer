#ifndef __x_iot_client_signaling_header_aisjieaaaa56f7sfd6d
#define __x_iot_client_signaling_header_aisjieaaaa56f7sfd6d

#include "PlatformDefine.h"
#include "XClientStructDefine.h"
#include "XStream.hpp"
#include "Pool.hpp"
//--------------------------------------------------------------------------------------------------------------------
#define ST_IOT_CLIENT_SIGNALING_BUFFER_LEN_LITTLE (LITTLE_BUFFER_SIZE_KB - sizeof(signaling_channel_head_t))
struct ST_IOT_CLIENT_SIGNALING_SEND_BUFFER
{
	signaling_channel_head_t head;
	uint8_t msg[ST_IOT_CLIENT_SIGNALING_BUFFER_LEN_LITTLE];
};
class CXIotClientSignalingBufferPool
{
public:
	CXIotClientSignalingBufferPool() {};
	~CXIotClientSignalingBufferPool() {};
	static CXIotClientSignalingBufferPool& Instance() {
		static CXIotClientSignalingBufferPool inc;
		return inc;
	};
	ST_IOT_CLIENT_SIGNALING_SEND_BUFFER * malloc() {
		ST_IOT_CLIENT_SIGNALING_SEND_BUFFER * pBuffer = (ST_IOT_CLIENT_SIGNALING_SEND_BUFFER *)CLittleBufferPool::Instance().malloc();
		return pBuffer;
	}
	void free(ST_IOT_CLIENT_SIGNALING_SEND_BUFFER * p) {
		CLittleBufferPool::Instance().free((char *)p);
	}
};
//--------------------------------------------------------------------------------------------------------------------
class CXIotClientSignaling :public  CXNetStreamData
{
public:
	CXIotClientSignaling(CXSignalingChannelMessageCallback *cb);
	~CXIotClientSignaling();

public:
	virtual void OnPacketCompleteNetStreamData(int32_t bytesTransferred, emXStreamTansferKey transferKey);

public:
	void Connect(char * s_ip, char * user, char * pwd);
	void Disconnect();
	BOOL IsError();//当前连接是否出错
	void AddSendMessage(int type,  char *buffer, int len);

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
	list<ST_IOT_CLIENT_SIGNALING_SEND_BUFFER *> m_msgSendList;
	void ClearSendBufferList();
	//
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
#if XINTERNET_TEST_STATUS
	volatile BOOL m_bCanReleaseSuccessCounter;
#endif
};

#endif