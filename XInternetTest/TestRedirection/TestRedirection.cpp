// TestRedirection.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//aliyun huadong 1core 1gb 访问hk azure database -base 5dtu，每秒处理11个selecte请求 ，dtu 10%
//
//
//
//azure hk B2s (20%cpu 2 vcpu，4 GB 内存) 2core 4gb 访问hk azure database -base 5dtu，
//每秒处理55个左右selecte请求，，dtu 100% ，cpu 7%
//
//
//
//azure hk B2s 低优先级 (20%cpu 2 vcpu，4 GB 内存) 2core 4gb 访问hk azure database - S0: 10 DTU，
//平均每秒处理200个selecte请求，峰值370 ，cpu没有可用分被压制在33%，dtu 峰值 40%
//

#define MAX_THREAD_COUNT 1
#include "GetUserRoutes.h"
#include "GetDeviceRoutes.h"
#include "GetClassicDevice.h"
HANDLE g_hStartDoing;
int main()
{
	//g_hStartDoing = CreateEvent(NULL, TRUE, FALSE, NULL);
	//
	XNetStartStreamManager();
	//----------------------------------------------------------
	//iot user rout
	//right
	CGetUserRoutes * pGetUserRoutes = new CGetUserRoutes();
	pGetUserRoutes->Get("1");
	delete pGetUserRoutes;
	//error
	pGetUserRoutes = new CGetUserRoutes();
	pGetUserRoutes->Get("XXSA");
	delete pGetUserRoutes;
	//----------------------------------------------------------
	//----------------------------------------------------------
	//iot dev rout
	//right
	CGetDeviceRoutes * pGetDeviceRoutes = new CGetDeviceRoutes();
	pGetDeviceRoutes->Get("XXSA");
	delete pGetDeviceRoutes;
	//error
	pGetDeviceRoutes = new CGetDeviceRoutes();
	pGetDeviceRoutes->Get("132312434234");
	delete pGetDeviceRoutes;
	//----------------------------------------------------------
	//classic get device
	CGetClassicDevice * pGetClassicDevice = new CGetClassicDevice();
	pGetClassicDevice->Get("13800", "13800");
	delete pGetClassicDevice;
	//error
	pGetClassicDevice = new CGetClassicDevice();
	pGetClassicDevice->Get("13800", "sssss");
	delete pGetClassicDevice;
	//----------------------------------------------------------



	XNetStopStreamManager();

	getchar();
    return 0;
}

