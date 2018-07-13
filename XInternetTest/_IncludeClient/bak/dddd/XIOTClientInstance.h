#ifndef __x_iot_client_inatance_header_sdobvrgh8i8fx2kh
#define __x_iot_client_inatance_header_sdobvrgh8i8fx2kh

#include "PlatformDefine.h"
#include "XIOTClientSignaling.h"
#include "XIOTClientStream.h"




class CXIotClientInstance :public CXIOTClientInferface
{
public:
	CXIotClientInstance();
	~CXIotClientInstance();
public:
	virtual void XDelete();
	virtual void InitInstance(const char *pszIp, const char* pszUser, const char* pszPwd, CXSignalingChannelMessageCallback * pCallback);
	virtual void UnintInstance();
	//
	virtual void GetDevices(char * pdstBuffer);
	//
	virtual emClientNetStatus GetSignalingStatus();
	virtual void SendSignaling(const char * pszBuffer, int nLen);
	//
	virtual CXClientStreamInterface * CreateXClientStreamInterface();
	virtual CXClientDownloadInterface * CreateXClientDownloadInterface();
private:
	CXIotClientSignaling *m_pSignaling;
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