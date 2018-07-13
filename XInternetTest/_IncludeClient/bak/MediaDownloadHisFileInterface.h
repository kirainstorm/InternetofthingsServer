#pragma once

#include "PlatformDefine.h"
//
#include "XPlayer.hpp"
#include "MediaDevInterface.h"

#define MAX_BUFFER_SIZE MAX_FRAME_SIZE

#include "XPlayerDecdatacallback.hpp"
#include "DownloadCallback.hpp"
//
#ifdef _PLATFORM_WINDOW

#include "waveOut.hpp"
//#include "cpp_windows/XCheckDiskFreeSpace.h"
#endif
//
#ifdef _PLATFORM_ANDROID
#include <openssl/des.h>
#include "XDes.h"
#endif

class CMediaDownloadHisFileInterface :public CXMediaDownloadInterface, public CXNetStreamData
{
public:
	CMediaDownloadHisFileInterface(void *dev);
	~CMediaDownloadHisFileInterface();
public:
	//virtual void DeleteStreamData();
	virtual void OnPacketCompleteNetStreamData(int32_t bytesTransferred, emXStreamTansferKey transferKey);

public:
	virtual int XDownloadHisFileDelete();
	virtual int XDownloadHisFileListFromMedia(_XSERVER_TIME szBeginTime, _XSERVER_TIME szEndTime, vector<HIS_LOG_T> &v);
	virtual int XDownloadHislogFromMedia(_XSERVER_TIME szBeginTime, _XSERVER_TIME szEndTime, CXMediaDownloadCallback* pDownloadObj = NULL, void *pUser = NULL);
	virtual int XDownloadHisFileClose();

#ifdef _PLATFORM_WINDOW
	//
	virtual int XDownloadHisFileFromMediaForWindows(_XSERVER_TIME szBeginTime, _XSERVER_TIME szEndTime, DeviceInfo_T t, HWND hwnd);//按时间下载文件
	//
	virtual int XDownloadHisFilePlayFrame(); //>0表示与下一帧的时间间隔，-1表示失败，0表示无数据 
	virtual int XDownloadHisFileSeekSecond(int sec); //从指定的时间播放
	//
	virtual int XDownloadHisFileGetPlaySecond(); //获取已播放秒数
	virtual int XDownloadHisFileGetDownloadSecond();//获取已下载秒数
	virtual int XDownloadHisFileGetSecond();//获取文件长度

	virtual int XDownloadHisFileWiteMp4File(char * pathfile);
	virtual int XDownloadHisFileGetWiteMp4FileProgress();
#endif

private:
	BOOL Login();
#ifdef _PLATFORM_WINDOW
	HWND m_hwnd;
	void * pDecoder;
	CWaveOut *pWaveOut;
	void *pAudioDecIns;
	CDecDataCallback * pVideoCallback;
#endif


	CDownloadCallback m_DownloadCallback;
private:
	CXNetStream* m_pTcpStream;
	void * m_hDevInterface;
	int m_nSessionID;
	int m_nSeq;

private:
	emNetRecieveStep m_nRcvStepStream;
	ST_SERVER_HEAD m_frameHead;
	char *m_pDataBuffer;	//接收的数据缓存
	size_t m_dataLen;	//接收的数据长度

private:
	CXMediaDownloadCallback *m_pDownloadHisFileCallback;
	void *m_pUser;
	void data_callback();

	CrossCriticalSection m_cs;
};

