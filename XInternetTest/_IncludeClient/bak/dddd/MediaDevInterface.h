#pragma once


//

#include "PlatformDefine.h"
#include "MediaInterface.h"
#include "XStream.hpp"
//
#include "Mp4File.hpp"
#include "XPlayerDecDataCallback.hpp"
#include "XDes.h"

//
#ifdef _PLATFORM_WINDOW
#include "waveOut.hpp"
#endif

#define MAX_MEDIA_PREVIEW_BUFFER_LEN (2*1024*1024)
#define MAX_PLAY_MSG_LEN (2*1024)
struct MyStruct4RMediaPlayInterface
{
	ST_SERVER_HEAD head;
	char buffer[MAX_PLAY_MSG_LEN - sizeof(ST_SERVER_HEAD) - sizeof(int)];
	int nSendLen;
};
class CMediaPlayInterfaceVoiceBuffer
{
public:
	CMediaPlayInterfaceVoiceBuffer(int len){
		m_bufferlen = len;
		m_buffer = new char[len + 1];
	};
	~CMediaPlayInterfaceVoiceBuffer(){
		delete[] m_buffer;
	};
public:
	char * m_buffer;//未解码数据
	int m_bufferlen;
};


class CMediaDevInterface : public CXMediaDeviceInterface,  public CXNetStreamData
{
public:
	CMediaDevInterface();
	~CMediaDevInterface();

public:
	//virtual void DeleteStreamData();
	virtual void OnPacketCompleteNetStreamData(int32_t bytesTransferred, emXStreamTansferKey transferKey);

public:
	virtual int XDeviceDelete();
	virtual int XDeviceSetInfo(DeviceInfo_T t, CXMediaTransparentTransmissionCallback *tran_cb = NULL, CXMediaMessageCallback *cb = NULL, void * pUser = NULL);
	virtual int XDeviceLogin();
	virtual int XDeviceLogout();
	virtual int XDeviceGetStatus(XMEDIA_DEVICE_STATUS_DEFINE type);
#ifdef _PLATFORM_WINDOW
	virtual int XPreviewOpenForWindows(HWND hWnd, CXMediaStreamData* pDataObj = NULL, DecodeVideoCallBack* pDecDataObj = NULL, void *pUser = NULL);
#endif
#if (defined _PLATFORM_ANDROID) || (defined _PLATFORM_IOS) || (defined _PLATFORM_WINDOW_WIN10_UWP)
	virtual int XPreviewOpenForApp(DecodeVideoCallBack* pDecDataObj = NULL, CXMediaStreamData* pVoiceObj = NULL , void *pUser = NULL);
#endif
	virtual int XPreviewClose();
	virtual int XPreviewSound(int nOpenSound);
	virtual int XPreviewCapturePictures(const char* szFilePath, const char* szFileName);
	virtual int XPreviewRecord(const char* szFilePath, const char *szDevName, BOOL bStart);
	virtual int TransparentTransmission(const  char *  szChanParam, int nlen);
	virtual CXMediaDownloadInterface* CreateDownloadHisFileInstance();

public:
	DeviceInfo_T m_stDevice;
	CrossCriticalSection m_csLogin;//for android???
	void Tidy();
private:
	int m_nSessionID;
	int m_nSeq;
	//
	CXMediaTransparentTransmissionCallback *m_cbTran;
	CXMediaMessageCallback *m_cbMessage;
	void * m_pUser;
	//
	BOOL m_bUserOpenPreview;//用户是否需要取流，如果是，当断线重连后需要发送取流命令
	BOOL m_bLogin;// 是否成功登陆 
	//
	CXNetStream *m_pTcpStream;
	CROSS_DWORD64 m_dwLastTick;
	//
	emNetRecieveStep m_nRcvStepStream;
	ST_SERVER_HEAD m_head;
	ST_SERVER_HEAD m_pFrameHeadTemp;
	char *m_pBuffer;	//接收的数据缓存
	unsigned int m_dataLen;	//接收的数据长度
	//
//	REMOTESESSIONSTEP m_nSendStep;
	CrossCriticalSection m_csMsglist;
	list<MyStruct4RMediaPlayInterface *> m_msgList;
	//
	//音视频
#ifdef _PLATFORM_WINDOW
	HWND m_hwnd;
#endif
	void * m_dec;
	CXMediaStreamData *m_pStreamCallback;
	DecodeVideoCallBack *m_pDecVideoCallback;
	void * m_pAVUser;
	CDecDataCallback *m_pVideoCallback;


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
	BOOL		m_hEvtVoiceWorkerStop;
	CrossCriticalSection m_csVoice;
	list< boost::shared_ptr<CMediaPlayInterfaceVoiceBuffer> > m_VoiceList;
#ifdef _PLATFORM_WINDOW
	CWaveOut m_waveout;
#endif
#ifdef _PLATFORM_ANDROID
	JNIEnv *m_AudioThreadEnvLocal;
	jmethodID m_jmidAudioNotify;
#endif


private:
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

