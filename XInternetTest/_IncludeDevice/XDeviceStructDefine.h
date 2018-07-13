#ifndef __msg_define_for_device_channel_6F60A5AC053F410890197A1C99654689
#define __msg_define_for_device_channel_6F60A5AC053F410890197A1C99654689
//----------------------------------------
// kirainstorm
// https://github.com/kirainstorm
//----------------------------------------
#include "PlatformDefine.h"
#include "XDefineSignalingChannelStruct.h"
#include "XDefineMediaChannelStruct.h"
class XDeviceMessageCallback
{
public:
	virtual void OnDeviceMessageCallback(const char *jsonbuffer, int len) = 0;
};
class XDeviceInterface
{
public:
	virtual void XDelete() = 0;
	//
	virtual int XConnect(XDeviceMessageCallback *callback, const char * ip, const char * uuid) = 0;
	virtual int XDisconnect() = 0;
	virtual int XGetStatus() = 0;
	//
	virtual int XSendMessage(const char * jsonbuffer, int len) = 0;
	virtual int XSendStream(emMEDIA_FRAME_TYPE_DEFINE nAVFrameType, const char *data, int len, uint64_t tick,
		uint16_t nVideoFrameRate, uint16_t nVideoWidth, uint16_t nVideoHeight,
		uint16_t nAudioChannels = 1, uint16_t nAudioSamplesRate = 8000, uint16_t nAudioBitsPerSample = 16) = 0;
	//
};
inline BOOL ____XDeviceInterfaceIsClassicID(string str) //classic id is number
{
	for (int i = 0; i < (int)(str.size()); i++)
	{
		if ((str.at(i) > '9') || (str.at(i) < '0'))
		{
			return FALSE;
		}
	}
	return TRUE;
}
#endif