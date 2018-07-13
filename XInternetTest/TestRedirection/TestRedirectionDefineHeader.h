#pragma once

#include "PlatformDefine.h"
#include "XStream.hpp"
#include "XDefineSignalingChannelStruct.h"
#include "XDefineMediaChannelStruct.h"
#include "Pool.hpp"
#include "XCounter.hpp"
#include "XDes.h"

#define SERVER_IP "127.0.0.1" //"47.96.249.142"
#define ST_DEVICE_SIGNALING_BUFFER_LEN (MIDDLE_BUFFER_SIZE_MB - sizeof(signaling_channel_head_t))
struct ST_REDIRECTION_SEND_BUFFER
{
	signaling_channel_head_t head;
	uint8_t msg[ST_DEVICE_SIGNALING_BUFFER_LEN];
};