#ifndef PHONE_DEVICE_X_456W34324
#define PHONE_DEVICE_X_456W34324
#include <stdio.h>
#include <stdlib.h>
#include "PlatformDefine.h"
#include "XStream.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/pool/pool.hpp>
#include "XDefineMediaChannelStruct.h"



#define MAX_MSG_4_PHONE_DEVICE_SESSION (1*1024*1024)
struct MyStruct4PhoneDeviceSession
{
	ST_XMEDIA_HEAD head;
	char buffer[MAX_MSG_4_PHONE_DEVICE_SESSION - sizeof(ST_XMEDIA_HEAD)];
};
enum emPhoneDeviceNetSendStep
{
	NET_SEND_STEP_SLEEP = 0,
	NET_SEND_STEP_SEND
};

class CPhoneDevice :public CXNetStreamData
{
public:
	CPhoneDevice();
	~CPhoneDevice();
	//////////////////////////////////////////////////////////////////////////
public:
	void Start();
	void Stop();
	void InputData(int avtype, int isKey, char * buffer, int len);


public:
	virtual void OnPacketCompleteNetStreamData(int32_t bytesTransferred, emXStreamTansferKey transferKey);
private:
	int LoginPlat();
	void LogoutPlat();
	void DoMsg();
	//
	BOOL m_bLogin;
	BOOL m_bStreamIsError;
	uint32_t m_nSession;
	//
	boost::pool<> m_plMsg;
	//
	emNetRecieveStep m_nRcvStep;
	ST_XMEDIA_HEAD m_head;
	int m_dataLen;
	unsigned char * m_szBuffer;
	//
	CXNetStream* m_pTcpStream;
	CrossCriticalSection m_csTcp;
	list<MyStruct4PhoneDeviceSession *> m_msgList;
	CrossCriticalSection m_csMsg;
	//
	//emPhoneDeviceNetSendStep m_nSendStep;
	static int WorkerThread(void*);
	void Worker();
	CROSS_THREAD_HANDLE	m_hWorkerThread;
	BOOL		m_hEvtWorkerStop;
	CROSS_DWORD64 m_dTickHearbeat;


};


#endif //PHONE_DEVICE_X_456W34324
