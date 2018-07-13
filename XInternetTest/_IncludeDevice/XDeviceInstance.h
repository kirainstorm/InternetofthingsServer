#ifndef __x_device_inatance_header_9EC028B5357C450C8DF1A77769029F87
#define __x_device_inatance_header_9EC028B5357C450C8DF1A77769029F87
//----------------------------------------
// kirainstorm
// https://github.com/kirainstorm
//----------------------------------------
#include "PlatformDefine.h"
#include "XDeviceSignaling.h"
#include "XDeviceStream.h"
//
class CXDeviceInstance :public XDeviceInterface
{
public:
	CXDeviceInstance();
	~CXDeviceInstance();
public:
	virtual void XDelete();
	//
	virtual int XConnect(XDeviceMessageCallback *callback, const char * ip, const char * uuid);
	virtual int XDisconnect();
	virtual int XGetStatus();
	//
	virtual int XSendMessage(const char * jsonbuffer, int len);
	virtual int XSendStream(emMEDIA_FRAME_TYPE_DEFINE nAVFrameType, const char *data, int len, uint64_t tick,
		uint16_t nVideoFrameRate, uint16_t nVideoWidth, uint16_t nVideoHeight,
		uint16_t nAudioChannels = 1, uint16_t nAudioSamplesRate = 8000, uint16_t nAudioBitsPerSample = 16);
private:
	CXDeviceStream *m_pStream;
	CXDeviceSignaling *m_pSignaling;
	XDeviceMessageCallback * m_pMessageCallback;
private:
	char m_szSvrIp[64];
	char m_szUuid[64];
private:
	volatile BOOL m_bTheadExit;
	CROSS_THREAD_HANDLE m_hThread;
	static int ThreadWorker(void * param);
	void Woker();
	//
	char m_szRealIp[64];
	char m_szIotUsername[64];//iot need bind username
	void GetIotServerInfo();
};

#endif