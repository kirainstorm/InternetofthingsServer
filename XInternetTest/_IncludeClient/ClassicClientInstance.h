#ifndef __x_classic_client_inatance_header_12346765rdgh8i8fx2kh
#define __x_classic_client_inatance_header_12346765rdgh8i8fx2kh

#include "PlatformDefine.h"
#include "XClientSignaling.h"
#include "XClientStream.h"
#include "XClientHislog.h"


class CClassicDeviceInfoBufferPool
{
public:
	CClassicDeviceInfoBufferPool() {};
	~CClassicDeviceInfoBufferPool() {};
	static CClassicDeviceInfoBufferPool& Instance() {
		static CClassicDeviceInfoBufferPool inc;
		return inc;
	};
	ST_CLASSIC_DEVICE_INFO_BASE * malloc() {
		ST_CLASSIC_DEVICE_INFO_BASE * pBuffer = (ST_CLASSIC_DEVICE_INFO_BASE *)CLittleBufferPool::Instance().malloc();
		return pBuffer;
	}
	void free(ST_CLASSIC_DEVICE_INFO_BASE * p) {
		CLittleBufferPool::Instance().free((char *)p);
	}
};

class CXClassicClientInstance :public CXClientInferface
{
public:
	CXClassicClientInstance();
	~CXClassicClientInstance();
public:
	virtual void XDelete();
	virtual void InitInstance(const char *pszIp, const char* pszUser, const char* pszPwd, CXSignalingChannelMessageCallback * pCallback);
	virtual void UnintInstance();
	//
	virtual void GetDevices(char * pdstBuffer, int &len);
	//
	virtual emClientNetStatus GetSignalingStatus(int nDevid);
	virtual void SignalingCtrl(const  char * szDeviceID, const char * pszInBuffer, int nInLen, char *pszOutBuffer, int &nOutLen);
	//
	virtual CXClientStreamInterface * CreateXClientStreamInterface(const char * uuid, int channel);
	virtual CXClientDownloadInterface * CreateXClientDownloadInterface(const char * uuid, int channel);
private:
	CXSignalingChannelMessageCallback * m_pMessageCallback;
private:
	char m_szCenterIp[64];
	int s_nCenterPort;
	char m_szUser[64];
	char m_szPwd[64];
	//
	CrossCriticalSection m_cs;
// private:
// 	volatile BOOL m_bTheadExit;
// 	CROSS_THREAD_HANDLE m_hThread;
// 	static int ThreadWorker(void * param);
// 	void Woker();

private:
	map<string, CXClientSignaling *> m_SignalingMap;//因为是多个服务器，所以要多个信令通道
	void UpdateSignalingServers();//更新信令服务器map
	void ClearSignalingServers();
private:
	map<int/*devid*/, ST_CLASSIC_DEVICE_INFO_BASE *> m_lisDevice;
	void ClearDeivceInfoList();


// private:
// 	CrossCriticalSection m_csGetDevice;
// 	volatile BOOL m_bUserGetDevices;//用户请求获取设备信息
// 	volatile BOOL m_bTheadExitGetDevice;
// 	CROSS_THREAD_HANDLE m_hThreadGetDevice;
// 	static int ThreadWorkerGetDevice(void * param);
// 	void WokerGetDevice();

};

#endif