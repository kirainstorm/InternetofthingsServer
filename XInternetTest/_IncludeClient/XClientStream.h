#ifndef __x_classic_client_stream_header_4567ijnvcdswe46780okb
#define __x_classic_client_stream_header_4567ijnvcdswe46780okb


#include "PlatformDefine.h"
#include "XClientStructDefine.h"
#include "XStream.hpp"
#include "Pool.hpp"
//
#include "Mp4File.hpp"
#include "XPlayerDecDataCallback.hpp"
#include "XDes.h"
#include "XDefineMediaChannelStruct.h"

//
#ifdef _PLATFORM_WINDOW
//#include "waveOut.hpp"
#endif


#define ST_CLIENT_STREAM_BUFFER_LEN_LITTLE (LITTLE_BUFFER_SIZE_KB - sizeof(signaling_channel_head_t))
struct ST_STREAM_BUFFER
{
	ST_XMEDIA_HEAD head;
	uint8_t msg[ST_CLIENT_STREAM_BUFFER_LEN_LITTLE];
};
class CStreamBufferPool
{
public:
	CStreamBufferPool() {};
	~CStreamBufferPool() {};
	static CStreamBufferPool& Instance() {
		static CStreamBufferPool inc;
		return inc;
	};
	ST_STREAM_BUFFER * malloc() {
		ST_STREAM_BUFFER * pBuffer = (ST_STREAM_BUFFER *)CLittleBufferPool::Instance().malloc();
		return pBuffer;
	}
	void free(ST_STREAM_BUFFER * p) {
		CLittleBufferPool::Instance().free((char *)p);
	}
};



// #define MAX_MEDIA_PREVIEW_BUFFER_LEN (2*1024*1024)
// #define MAX_PLAY_MSG_LEN (2*1024)
// struct MyStruct4RMediaPlayInterface
// {
// 	ST_XMEDIA_HEAD head;
// 	char buffer[MAX_PLAY_MSG_LEN - sizeof(ST_XMEDIA_HEAD)];
// };
// 
// 
// class CMediaPlayInterfaceVoiceBuffer
// {
// public:
// 	CMediaPlayInterfaceVoiceBuffer(int len){
// 		m_bufferlen = len;
// 		m_buffer = new char[len + 1];
// 	};
// 	~CMediaPlayInterfaceVoiceBuffer(){
// 		delete[] m_buffer;
// 	};
// public:
// 	char * m_buffer;//未解码数据
// 	int m_bufferlen;
// };


class CXClientStream : public CXClientStreamInterface, public CXNetStreamData
{
public:
	CXClientStream(emXClientType type, const char * uuid, int channel, const char * ip, int port, const char * user, const char * pwd);
	~CXClientStream();
public:
	virtual void OnPacketCompleteNetStreamData(int32_t bytesTransferred, emXStreamTansferKey transferKey);

public:
	virtual void XDelete();
	virtual emClientNetStatus XClientStreamGetStatus();
	virtual int XClientStreamSetRawdataCallback(CXClientStreamRawdataCallback* pCallback, void *pUser);
	virtual int XClientStreamSetDecodeVideoCallBack(DecodeVideoCallBack* pCallback, void *pUser);
	//
#ifdef _PLATFORM_WINDOW
	virtual int XClientStreamPlay(HWND hWnd);//"hWnd" can be "NULL"
#else
	virtual int XClientStreamPlay();
#endif
	virtual int XClientStreamClose();
	virtual int XClientStreamSound(BOOL bOpenSound);
	virtual int XClientStreamCapturePictures(const char* szFilePath, const char* szFileName);
	virtual int XClientStreamRecord(const char* szFilePath, const char *szDevName, BOOL bStart);

private:
	emXClientType m_type;
	//
	volatile BOOL m_bLogin;// 是否成功登陆 
	unsigned int m_nSessionID;
	int m_nSeq;
	emClientNetStatus m_emClientNetStatus;
	//
	char m_szIP[64];
	int m_port;
	char m_szUser[64];
	char m_szPwd[64];
	int m_classicid;
	int m_channel;
	char m_szIotuuid[64];
	//
	CXClientStreamRawdataCallback *m_pRawdataCallback;
	void * m_pRawdataCallbackUser;
	DecodeVideoCallBack * m_pDecodeVideoCallBack;
	void * m_pDecodeVideoCallBackUser;
	CDecDataCallback *m_pVideoCallback;
	//tcp
	CXNetStream *m_pTcpStream;
	CROSS_DWORD64 m_dwLastTick;
	//recv
	emNetRecieveStep m_nRcvStepStream;
	ST_XMEDIA_HEAD m_head;
	ST_XMEDIA_AVHEAD m_pFrameHeadTemp;
	char *m_pBuffer;	//接收的数据缓存
	unsigned int m_dataLen;	//接收的数据长度
	//send
	CrossCriticalSection m_csMsglist;
	list<ST_STREAM_BUFFER *> m_msgList;
private:
#ifdef _PLATFORM_WINDOW
	HWND m_hwnd;
#endif
	//
	void * m_dec;
	void * m_pAVUser;
	//recorder
	CrossCriticalSection m_csRecord;
	void *m_pMp4File;
	BOOL m_bRecodeMp4File;
	char m_szMp4FilePath[256];
	//voice
	void *m_Voice;
	static int	VoiceWorkerThread(void*);
	void VoiceWorker();
	CROSS_THREAD_HANDLE	m_hVoiceWorkerThread;
	volatile BOOL		m_hEvtVoiceWorkerStop;
	CrossCriticalSection m_csVoice;
	list<ST_STREAM_BUFFER *> m_VoiceBufferList;
	//list< boost::shared_ptr<CMediaPlayInterfaceVoiceBuffer> > m_VoiceList;
#ifdef _PLATFORM_WINDOW
//	CWaveOut m_waveout;
#endif
#ifdef _PLATFORM_ANDROID
	JNIEnv *m_AudioThreadEnvLocal;
	jmethodID m_jmidAudioNotify;
#endif


private:
	void Tidy();
	BOOL Login(int waitsec = 4);
	void data_callback();
	void AddSendMessge(int type, char *buffer, int len);

private:
	static int WorkerThread(void*);
	void Worker();
	CROSS_THREAD_HANDLE	m_hWorkerThread;
	BOOL		m_bWorkerStop;
	void DoMsg();
};








#endif