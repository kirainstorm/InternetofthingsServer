#include "stdafx.h"
#include "XDevice.h"
#include "PlatformDefine.h"
#include "XDeviceInstance.h"
//----------------------------------------
// kirainstorm
// https://github.com/kirainstorm
//----------------------------------------
XDeviceInterface * CreateDeviceInstance()
{
	XDeviceInterface * p = new CXDeviceInstance();
	assert(p);
	return p;
};

