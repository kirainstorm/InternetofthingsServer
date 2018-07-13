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
	virtual int XDownloadHisFileFromMediaForWindows(_XSERVER_TIME szBeginTime, _XSERVER_TIME szEndTime, DeviceInfo_T t, HWND hwnd);//��ʱ�������ļ�
	//
	virtual int XDownloadHisFilePlayFrame(); //>0��ʾ����һ֡��ʱ������-1��ʾʧ�ܣ�0��ʾ������ 
	virtual int XDownloadHisFileSeekSecond(int sec); //��ָ����ʱ�䲥��
	//
	virtual int XDownloadHisFileGetPlaySecond(); //��ȡ�Ѳ�������
	virtual int XDownloadHisFileGetDownloadSecond();//��ȡ����������
	virtual int XDownloadHisFileGetSecond();//��ȡ�ļ�����

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
	char *m_pDataBuffer;	//���յ����ݻ���
	size_t m_dataLen;	//���յ����ݳ���

private:
	CXMediaDownloadCallback *m_pDownloadHisFileCallback;
	void *m_pUser;
	void data_callback();

	CrossCriticalSection m_cs;
};

