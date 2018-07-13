#pragma once
#include "PlatformDefine.h"

//#define  


class CXCounterConnectAndLoginUser
{
public:
	CXCounterConnectAndLoginUser()
	{
		m_nConnectError = 0;
		m_nConnect = 0;
		m_nLoginStepOne = 0;
		m_nLoginStepOne_1 = 0;	
		m_nLoginStepTwo = 0;
		m_nServerStartTick = GetTickCount64();
	};
	~CXCounterConnectAndLoginUser() {};
	//
	static CXCounterConnectAndLoginUser& Instance()
	{
		static CXCounterConnectAndLoginUser inc;
		return inc;
	};
public:

	volatile uint64_t m_nServerStartTick;//服务器启动时间
	//=========================================
	uint64_t GetOnlyConnectErrorCount()
	{
		return m_nConnectError;
	}
	void AddOnlyErrorConnect()
	{
		m_csConnectError.Lock();
		m_nConnectError++;
		m_csConnectError.Unlock();
	}
	//=========================================
	uint64_t GetOnlyConnectCount()
	{
		return m_nConnect;
	}
	void AddOnlyConnect()
	{
		m_csConnect.Lock();
		m_nConnect++;
		m_csConnect.Unlock();
	}
	//=========================================
	uint64_t GetOnlyLoginStepOne()
	{
		return m_nLoginStepOne;
	}
	void AddOnlyLoginStepOne()
	{
		m_csLoginStepOne.Lock();
		m_nLoginStepOne++;
		m_csLoginStepOne.Unlock();
	}
	//=========================================
	uint64_t GetOnlyLoginStepOne_1()
	{
		return m_nLoginStepOne_1;
	}
	void AddOnlyLoginStepOne_1()
	{
		m_csLoginStepOne_1.Lock();
		m_nLoginStepOne_1++;
		m_csLoginStepOne_1.Unlock();
	}
	//=========================================	
	uint64_t GetLoginStepTwo()
	{
		return m_nLoginStepTwo;
	}
	void AddLoginStepTwo()
	{
		m_csLoginStepTwo.Lock();
		m_nLoginStepTwo++;
		m_csLoginStepTwo.Unlock();
	}
	//=========================================
private:
	//
	volatile uint64_t m_nConnectError;
	CrossCriticalSection m_csConnectError;
	//
	volatile uint64_t m_nConnect;
	CrossCriticalSection m_csConnect;
	//
	volatile uint64_t m_nLoginStepOne;
	CrossCriticalSection m_csLoginStepOne;
	//
	volatile uint64_t m_nLoginStepOne_1;
	CrossCriticalSection m_csLoginStepOne_1;
	//
	volatile uint64_t m_nLoginStepTwo;
	CrossCriticalSection m_csLoginStepTwo;

};



class CConnectAndLoginUser
{
public:
	CConnectAndLoginUser(string user , string pwd);
	~CConnectAndLoginUser();

private:
	static void WINAPI WorkerThread(void*);
	void Worker();
	HANDLE	m_hThread;
	BOOL m_bExit;

};

