#ifndef __x_device_signaling_header_66BFC80F0AF34612B492883F753EEE51
#define __x_device_signaling_header_66BFC80F0AF34612B492883F753EEE51
//----------------------------------------
// kirainstorm
// https://github.com/kirainstorm
//----------------------------------------
#include "PlatformDefine.h"
#include "XDeviceStructDefine.h"
#include "XStream.hpp"
#include "Pool.hpp"
//
//--------------------------------------------------------------------------------------------------------------------
#define ST_DEVICE_SIGNALING_BUFFER_LEN_LITTLE (LITTLE_BUFFER_SIZE_KB - sizeof(signaling_channel_head_t))
struct ST_DEVICE_SIGNALING_SEND_BUFFER
{
	signaling_channel_head_t head;
	uint8_t msg[ST_DEVICE_SIGNALING_BUFFER_LEN_LITTLE];
};
class CXDeviceSignalingBufferPool
{
public:
	CXDeviceSignalingBufferPool() {};
	~CXDeviceSignalingBufferPool() {};
	static CXDeviceSignalingBufferPool& Instance(){
		static CXDeviceSignalingBufferPool inc;
		return inc;
	};
	ST_DEVICE_SIGNALING_SEND_BUFFER * malloc(){
		ST_DEVICE_SIGNALING_SEND_BUFFER * pBuffer = (ST_DEVICE_SIGNALING_SEND_BUFFER *)CLittleBufferPool::Instance().malloc();
		return pBuffer;
	}
	void free(ST_DEVICE_SIGNALING_SEND_BUFFER * p){
		CLittleBufferPool::Instance().free((char *)p);
	}
};
//--------------------------------------------------------------------------------------------------------------------
class CXDeviceSignaling :public  CXNetStreamData
{
public:
	CXDeviceSignaling(XDeviceMessageCallback *cb);
	~CXDeviceSignaling();

public:
	virtual void OnPacketCompleteNetStreamData(int32_t bytesTransferred, emXStreamTansferKey transferKey);

public:
	void Connect(char * s_ip, char * uuid, char * user);
	void Disconnect();
	BOOL IsConnectError();
	void AddSendMessage(int type,  char *buffer, int len);

private:
	volatile BOOL m_bIsError;//init:TRUE
	CXNetStream* m_pTcpStream;
	//
	XDeviceMessageCallback *m_pCallBack;
	unsigned int m_nSessionID;
	//
	signaling_channel_head_t m_head;
	char *m_pBuffer;
	emNetRecieveStep m_nRecvStep;
	void DoMsg();
	CROSS_DWORD64 m_dwLastTick;
	//
	CrossCriticalSection m_csSendList;//for m_msgSendList,   list in multiple-thread not safe.
	list<ST_DEVICE_SIGNALING_SEND_BUFFER *> m_msgSendList;
	void ClearSendBufferList();
	//
};

#endif