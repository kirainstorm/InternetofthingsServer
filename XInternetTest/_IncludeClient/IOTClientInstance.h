#ifndef __x_iot_client_inatance_header_sdobvrgh8i8fx2kh
#define __x_iot_client_inatance_header_sdobvrgh8i8fx2kh

#include "PlatformDefine.h"
#include "XClientSignaling.h"
#include "XClientStream.h"
#include "XClientHislog.h"




class CXIotClientInstance :public CXClientInferface
{
public:
	CXIotClientInstance();
	~CXIotClientInstance();
public:
	virtual void XDelete();
	virtual void InitInstance(const char *pszIp, const char* pszUser, const char* pszPwd, CXSignalingChannelMessageCallback * pCallback);
	virtual void UnintInstance();
	//
	virtual void GetDevices(char * pdstBuffer, int &len);
	//
	virtual emClientNetStatus GetSignalingStatus(int value);
	virtual void SignalingCtrl(const  char * szDeviceID, const char * pszInBuffer, int nInLen, char *pszOutBuffer, int &nOutLen);
	//
	virtual CXClientStreamInterface * CreateXClientStreamInterface(const char * uuid, int channel/*0*/);
	virtual CXClientDownloadInterface * CreateXClientDownloadInterface(const char * uuid, int channel/*0*/);
private:
	CXClientSignaling *m_pSignaling;
	CXSignalingChannelMessageCallback * m_pMessageCallback;
private:
	char m_szCenterIp[64];
	int s_nCenterPort;
	char m_szUser[64];
	char m_szPwd[64];
private:
	volatile BOOL m_bTheadExit;
	CROSS_THREAD_HANDLE m_hThread;
	static int ThreadWorker(void * param);
	void Woker();
	//
	char m_szIotIp[64];
	void GetIotServerInfo();
};

#endif