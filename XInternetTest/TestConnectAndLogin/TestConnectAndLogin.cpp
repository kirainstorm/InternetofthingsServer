// TestConnectAndLogin.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ConnectAndLoginUser.h"

//测试服务器请注意！请使用release版本的服务器程序，debug和release下的性能差十万八千里
//
//
// 
//---------------------------------------------------------------------------
//
// 用户登陆压力性能测试：
//
// 使用N个ConnectAndLoginUser线程并发访问用户登陆服务器进行压力性能测试
// 测试目标
// 1：服务器socket是否能正常回收，FIN_WAIT,FIN_TIMEOU之类
// 2：服务器类是否正常new/delete，且无内存泄漏
// 测试方式，随机进行以下测试
// 1：正常流程，connect - login1 - login2 - disconnect
// 2：非正常流程 conncet - disconnect
// 3：非正常流程 conncet - login1 - disconnect
//---------------------------------------------------------------------------
//
//
//---------------------------------------------------------------------------
// 20180629 《阿里云服务器》 测试发现
// 在本机开启3000个线程访问阿里云服务器的服务器端（电信上行10M）
// 超过3k并发时服务器的accept回调抖动，不稳定，可能5秒没有回调，可能1秒几百个回调
// accept回调抖动与服务器核心数无关
//---------------------------------------------------------------------------
// 20180702 《本机测试》
// 在本机开启3000个线程访问 本机 服务器端
// 3k,4k正常连接断开，没有抖动
//---------------------------------------------------------------------------

int main()
{
	XNetStartStreamManager();

	CXCounterConnectAndLoginUser::Instance().GetOnlyConnectErrorCount();

	for (int i = 0 ; i < 4000 ; i ++)
	{
		CConnectAndLoginUser * p = new CConnectAndLoginUser("13800138000", "13800138000");
	}

	while (true)
	{
		printf("Sec:[%I64d]\t\t\tConnError[%I64d]\t\t\tSessionError[%I64d]\t\t\tLoginError[%I64d]\t\t\tOK[%I64d]\n",
			(GetTickCount64() - CXCounterConnectAndLoginUser::Instance().m_nServerStartTick) /1000,
			CXCounterConnectAndLoginUser::Instance().GetOnlyConnectErrorCount(),
			CXCounterConnectAndLoginUser::Instance().GetOnlyLoginStepOne(),
			CXCounterConnectAndLoginUser::Instance().GetOnlyLoginStepOne_1(),
			CXCounterConnectAndLoginUser::Instance().GetLoginStepTwo());

		Sleep(1000);
	}



    return 0;
}

