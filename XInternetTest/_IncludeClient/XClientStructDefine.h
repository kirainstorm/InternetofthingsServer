#ifndef __x_client_struct_defin_4ghwedb9yf234bh23n
#define __x_client_struct_defin_4ghwedb9yf234bh23n



#include "XDefineSignalingChannelStruct.h"
#include "XDefineMediaChannelStruct.h"

#include "XPlayer.hpp"

enum emClientNetStatus
{
	CLIENT_NET_STATUS_NONE = 0, //用户没有调用login
	CLIENT_NET_STATUS_CONNECTING,
	CLIENT_NET_STATUS_CONNECTED,
	CLIENT_NET_STATUS_WRONG_NAMEPWD,
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
class CXClientStreamRawdataCallback
{
public:
	virtual int OnRawdata(ST_XMEDIA_HEAD head, const char* pData, int nLen) = 0;
};
class CXClientStreamInterface
{
public:
	virtual void XDelete() = 0;
public:
	virtual emClientNetStatus XClientStreamGetStatus() = 0;
	//
	virtual int XClientStreamSetRawdataCallback(CXClientStreamRawdataCallback* pCallback, void *pUser) = 0;
	virtual int XClientStreamSetDecodeVideoCallBack(DecodeVideoCallBack* pCallback, void *pUser) = 0;
	//
#ifdef _PLATFORM_WINDOW
	virtual int XClientStreamPlay(HWND hWnd) = 0;//"hWnd" can be "NULL"
#else
	virtual int XClientStreamPlay() = 0;
#endif
	virtual int XClientStreamClose() = 0;
	virtual int XClientStreamSound(BOOL bOpenSound) = 0;
	virtual int XClientStreamCapturePictures(const char* szFilePath, const char* szFileName) = 0;
	virtual int XClientStreamRecord(const char* szFilePath, const char *szDevName, BOOL bStart) = 0;
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
class CXMediaDownloadCallback
{
public:
	//cmd:Z_CMD_OF_SERVER_AV_STREAM
	//reslut:emCmdResultOfServer
	virtual int OnDownloadHisFileCallback(ST_XMEDIA_HEAD head, const char *data, int len, const void * pUser) = 0;
};

class CXClientDownloadInterface
{
public:
	virtual int XClientDownloadInterfaceDelete() = 0;
public:
	//获取列表
	virtual int XClientDownloadInterfaceGetHisLogs(ST_XMEDIA_TIME szBeginTime, ST_XMEDIA_TIME szEndTime, vector<ST_SREVER_HIS_LOG> &v) = 0;//获取文件列表
	//下载数据																										//
	virtual int XClientDownloadInterfaceDownloadHislog(ST_XMEDIA_TIME szBeginTime, ST_XMEDIA_TIME szEndTime) = 0;
	virtual int XClientDownloadInterfaceDownloadClose() = 0;
	//

#ifdef _PLATFORM_WINDOW
	virtual int XClientDownloadInterfacePlayFrameWindows(HWND hwnd) = 0; //>0表示与下一帧的时间间隔，-1表示失败，0表示无数据 
#else

#endif
	//
	virtual int XClientDownloadInterfacePlaySeekSecond(int sec) = 0; //从指定的时间播放
	virtual int XClientDownloadInterfacePlayGetPlaySecond() = 0; //获取已播放秒数
	virtual int XClientDownloadInterfacePlayGetDownloadSecond() = 0;//获取已下载秒数
	virtual int XClientDownloadInterfacePlayGetFileSecond() = 0;//获取文件长度
	//
	virtual int XClientDownloadInterfacePlayWiteMp4File(char * pathfile) = 0;
	virtual int XClientDownloadInterfacePlayWiteMp4FileProgress() = 0;
	//
};



//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
class CXSignalingChannelMessageCallback//Message
{
public:
	virtual void OnSignalingChannelMessageCallback(const char * jsondata, int datalen) = 0;
};
//classic 某个用户的设备<<不一定>>在同一台服务器上
//iot 某个用户的所有设备<<一定>>在同一台服务器上
enum emXClientType
{
	X_CLIENT_XXX_TYPE_CLASSIC = 0,
	X_CLIENT_XXX_TYPE_IOT
};
class CXClientInferface
{
public:
	virtual void XDelete() = 0;
public:
	virtual void InitInstance(const char *pszIp, const char* pszUser, const char* pszPwd, CXSignalingChannelMessageCallback * pCallback) = 0;
	virtual void UnintInstance() = 0;
	//
	virtual void GetDevices(char * pdstBuffer, int &len) = 0;
	//
	virtual emClientNetStatus GetSignalingStatus(int value) = 0;
	virtual void SignalingCtrl(const  char * szDeviceID, const char * pszInBuffer, int nInLen, char *pszOutBuffer, int &nOutLen) = 0;
	//
	virtual CXClientStreamInterface * CreateXClientStreamInterface(const char * uuid, int channel/*0*/) = 0;
	//
	virtual CXClientDownloadInterface * CreateXClientDownloadInterface(const char * uuid, int channel/*0*/) = 0;
};
// class CXIOTClientInferface
// {
// public:
// 	virtual void XDelete() = 0;
// public:
// 	virtual void InitInstance(const char *pszIp, const char* pszUser, const char* pszPwd, CXSignalingChannelMessageCallback * pCallback) = 0;
// 	virtual void UnintInstance() = 0;
// 	//
// 	virtual void GetDevices(char * pdstBuffer, int &len) = 0;
// 	//
// 	virtual emClientNetStatus GetSignalingStatus() = 0;
// 	virtual void SendSignaling(const char * pszBuffer, int nLen) = 0;
// 	//
// 	virtual CXClientStreamInterface * CreateXClientStreamInterface() = 0;
// 	virtual CXClientDownloadInterface * CreateXClientDownloadInterface() = 0;
// };

#if XINTERNET_TEST_STATUS
class CClinetMessageCb :public CXSignalingChannelMessageCallback
{
public:
	CClinetMessageCb() {};
	~CClinetMessageCb() {};
	virtual int OnSignalingChannelMessageCallback(const char * jsondata, int datalen)
	{
		printf(jsondata);
		printf("\n");
		return 0;
	};
private:

};
#endif




#endif