#pragma once

#include "PlatformDefine.h"
#include "XStream.hpp"
#include "XDefineSignalingChannelStruct.h"
#include "Pool.hpp"
#include "XCounter.hpp"

#define SERVER_IP "47.96.249.142"

#define ST_DEVICE_SIGNALING_BUFFER_LEN_LITTLE (MIDDLE_BUFFER_SIZE_MB - sizeof(signaling_channel_head_t))
struct ST_REDIRECTION_SEND_BUFFER
{
	signaling_channel_head_t head;
	uint8_t msg[ST_DEVICE_SIGNALING_BUFFER_LEN_LITTLE];
};
class CXRedirectionGetRoutes
{
public:
	CXRedirectionGetRoutes(string _id);
	~CXRedirectionGetRoutes();

	void Start();
	void Stop();

private:
	static void WINAPI WorkerThread(void*);
	void Worker();
	HANDLE	m_hThread;
	string m_strID;
	//
};

