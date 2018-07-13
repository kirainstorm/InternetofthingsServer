#ifndef __x_classic_client_his_download_header_ijbvdetyihcdswetyjk
#define __x_classic_client_his_download_header_ijbvdetyihcdswetyjk


#include "PlatformDefine.h"
//
#include "XClientStructDefine.h"
#include "XStream.hpp"
#include "XPlayer.hpp"
#include "XDefineMediaChannelStruct.h"
#include "XPlayerDecDataCallback.hpp"
#define MAX_BUFFER_SIZE MAX_FRAME_SIZE
//
#ifdef _PLATFORM_WINDOW
//#include "waveOut.hpp"
#endif
//
#ifdef _PLATFORM_ANDROID
#include <openssl/des.h>
#include "XDes.h"
#endif
//
#include "Mp4File.hpp"

#define MAX_FILE_BUFFER_LEN (128*1024*1024)
#define MAX_FILE_INDEX_COUNT (100*1024)

//����˼·��
//��ʼ���ŵ�ʱ����ȡ��һ֡I֡��ʾ��ͬʱȡ������һ����Ƶ֡Ҫ�ȴ��ļ��
//����ȡ��һ����Ƶ֡��������֮��϶���N�����Ƶ֡��һ��ȡ����������ͬʱȡ������һ����Ƶ֡Ҫ�ȴ��ļ��

//��Ƶ�ȴ�����ʱ������ʱ����Ƶ����������Ƶ����

#define MAX_ONE_VIDEO_AUDIO_COUNT 16
struct MyOnlinePlayAudioInfo
{
	char *pAudioBuffer;//��Ƶ����  //ST_XMEDIA_HEAD+ST_XMEDIA_AVHEAD+data
	//int nAudioLen;//��Ƶ���ݳ���//ST_XMEDIA_AVHEAD+data
};
struct MyOnlinePlayVideoInfo
{
	int nSleep;//����һ֡��ʱ���
	//
	char *pVideoBuffer;//��Ƶ����//ST_XMEDIA_HEAD+ST_XMEDIA_AVHEAD+data
	//int nVideoLen;//��Ƶ���ݳ��� //ST_XMEDIA_AVHEAD+data
	//
	//���MAX_ONE_VIDEO_AUDIO_COUNT֡��Ƶ�����2����Ƶ֮�䳬��MAX_ONE_VIDEO_AUDIO_COUNT֡����ȡ��ǰ��MAX_ONE_VIDEO_AUDIO_COUNT֡�����������û���ô�ÿ�����ͼ��
	char *pAudioBuffer[MAX_ONE_VIDEO_AUDIO_COUNT];
	int nAudioCount;
};
inline CROSS_DWORD64 __online_dec_cb_systime_to_millisecond(ST_XMEDIA_AVHEAD& st)//
{
	struct tm gm = { st.time.wSecond, st.time.wMinute, st.time.wHour, st.time.wDay, st.time.wMonth - 1, st.time.wYear - 1900, st.time.wDayOfWeek, 0, 0 };
	CROSS_DWORD64 k = mktime(&gm) * 1000 + st.time.wMilliseconds;
	return k;
}
inline CROSS_DWORD64 __online_dec_cb_systime_to_millisecond(ST_XMEDIA_TIME& st)//
{
	struct tm gm = { st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth - 1, st.wYear - 1900, st.wDayOfWeek, 0, 0 };
	CROSS_DWORD64 k = mktime(&gm) * 1000 + st.wMilliseconds;
	return k;
}

//
class CXClientHislog :public CXClientDownloadInterface, public CXNetStreamData
{
public:
	CXClientHislog(emXClientType type, const char * uuid, int channel, const char * ip, int port, const char * user, const char * pwd);
	~CXClientHislog();
public:
	//virtual void DeleteStreamData();
	virtual void OnPacketCompleteNetStreamData(int32_t bytesTransferred, emXStreamTansferKey transferKey);


public:
public:
	virtual int XClientDownloadInterfaceDelete();
public:
	//virtual int XClientDownloadInterfaceSetInfo(int id, int channel/*0*/);
	//��ȡ�б�
	virtual int XClientDownloadInterfaceGetHisLogs(ST_XMEDIA_TIME szBeginTime, ST_XMEDIA_TIME szEndTime, vector<ST_SREVER_HIS_LOG> &v);
	//��������																										//
	virtual int XClientDownloadInterfaceDownloadHislog(ST_XMEDIA_TIME szBeginTime, ST_XMEDIA_TIME szEndTime);

	virtual int XClientDownloadInterfaceDownloadClose();
	//

#ifdef _PLATFORM_WINDOW
	virtual int XClientDownloadInterfacePlayFrameWindows(HWND hwnd); //>0��ʾ����һ֡��ʱ����-1��ʾʧ�ܣ�0��ʾ������ 
#else

#endif
	//
	virtual int XClientDownloadInterfacePlaySeekSecond(int sec); //��ָ����ʱ�䲥��
	virtual int XClientDownloadInterfacePlayGetPlaySecond(); //��ȡ�Ѳ�������
	virtual int XClientDownloadInterfacePlayGetDownloadSecond();//��ȡ����������
	virtual int XClientDownloadInterfacePlayGetFileSecond();//��ȡ�ļ�����
	//
	virtual int XClientDownloadInterfacePlayWiteMp4File(char * pathfile);
	virtual int XClientDownloadInterfacePlayWiteMp4FileProgress();

private:
	char m_szIP[64];
	int m_port;
	char m_szUser[64];
	char m_szPwd[64];
	char m_id[64];
	int m_channel;


private:
	BOOL Login();
#ifdef _PLATFORM_WINDOW
	HWND m_hwnd;
	void * pDecoder;
//	CWaveOut *pWaveOut;
	void *pAudioDecIns;
	CDecDataCallback * pVideoCallback;
#endif

	//CDownloadCallback m_DownloadCallback;
private:
	CXNetStream* m_pTcpStream;
	//void * m_hDevInterface;
	int m_nSessionID;
	//int m_nSeq;

private:
	emNetRecieveStep m_nRcvStepStream;
	ST_XMEDIA_HEAD m_frameHead;
	char *m_pDataBuffer;	//���յ����ݻ���
	size_t m_dataLen;	//���յ����ݳ���

private:
	CXMediaDownloadCallback *m_pDownloadHisFileCallback;
	void *m_pUser;
	void data_callback();

	//CrossCriticalSection m_cs;

	//
	//
	
private:
	char * m_pDownFileBuffer;
	BOOL m_bNeedFirstIFrame;
	MyOnlinePlayVideoInfo *m_pFrame;
	int m_nWiteMp4FileProgress;
	CrossCriticalSection m_cs;
	ST_XMEDIA_TIME m_tmStartTime;
	ST_XMEDIA_TIME m_tmEndTime;
	int m_nDownloadFileLen;//�Ѿ����ص����ݳ���
	int m_nDownloadFileIndex;//�Ѿ����ص�����֡��
	int m_nCureFileIndex;//��ǰ�����ڼ�֡
	int *m_pFileIndexArray;//ÿһ֡�ڻ������ĵ�ַ
	int m_nIndexTmp;
	//
	MyOnlinePlayVideoInfo * GetNextVideoFrame();
};


#endif