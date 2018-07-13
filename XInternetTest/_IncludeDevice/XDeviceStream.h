#ifndef __x_device_stream_header_7FD5DAF47A844BE5A4D3F30AFD6BAB5B
#define __x_device_stream_header_7FD5DAF47A844BE5A4D3F30AFD6BAB5B
//----------------------------------------
// kirainstorm
// https://github.com/kirainstorm
//----------------------------------------
#include "PlatformDefine.h"
#include "XDeviceStructDefine.h"
#include "XStream.hpp"
#include "Pool.hpp"
//
//
#define ST_DEVICE_STREAM_BUFFER_MB (MIDDLE_BUFFER_SIZE_MB - sizeof(ST_XMEDIA_HEAD))
struct ST_DEVICE_STREAM_SEND_BUFFER
{
	ST_XMEDIA_HEAD head;
	uint8_t msg[ST_DEVICE_STREAM_BUFFER_MB];
};
class CXDeviceStreamBufferPool
{
public:
	CXDeviceStreamBufferPool() {};
	~CXDeviceStreamBufferPool() {};
	static CXDeviceStreamBufferPool& Instance(){
		static CXDeviceStreamBufferPool inc;
		return inc;
	};
	ST_DEVICE_STREAM_SEND_BUFFER * malloc(){
		ST_DEVICE_STREAM_SEND_BUFFER * pBuffer = (ST_DEVICE_STREAM_SEND_BUFFER *)CMiddleBufferPool::Instance().malloc();
		return pBuffer;
	}
	void free(ST_DEVICE_STREAM_SEND_BUFFER * p){
		CMiddleBufferPool::Instance().free((char *)p);
	}
};
class CXDeviceStream :public  CXNetStreamData
{
public:
	CXDeviceStream();
	~CXDeviceStream();

public:
	virtual void OnPacketCompleteNetStreamData(int32_t bytesTransferred, emXStreamTansferKey transferKey);

public:
	void Connect(char * s_ip, char * uuid, char * user);
	void Disconnect();
	BOOL IsConnectError();
	void AddHeartbeat();
	int AddSendStream(emMEDIA_FRAME_TYPE_DEFINE nAVFrameType, const char *data, int len, uint64_t tick,
		uint16_t nVideoFrameRate, uint16_t nVideoWidth, uint16_t nVideoHeight,
		uint16_t nAudioChannels, uint16_t nAudioSamplesRate, uint16_t nAudioBitsPerSample);

private:
	volatile BOOL m_nCanSendStream;

private:
	volatile BOOL m_bIsError;//init : TRUE
	CXNetStream* m_pTcpStream;
	//
	uint32_t m_nSessionID;
	ST_XMEDIA_HEAD m_head;
	char *m_pBuffer;
	emNetRecieveStep m_nRecvStep;
	void DoMsg();
	CROSS_DWORD64 m_dwLastTick;
	//
	CrossCriticalSection m_csSendList;//for m_dataSendList,   list in multiple-thread not safe.
	list<ST_DEVICE_STREAM_SEND_BUFFER *> m_dataSendList;
	void ClearSendBufferList();
};


#endif