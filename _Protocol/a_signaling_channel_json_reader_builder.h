#ifndef a_signaling_channel_json_reader_builder_yhm43edxibvwedujmre
#define a_signaling_channel_json_reader_builder_yhm43edxibvwedujmre
//----------------------------------------
// kirainstorm
// https://github.com/kirainstorm
//----------------------------------------
//#include "PlatformDefine.h"
#include <stdlib.h>
#include <string.h>

#include "json/json.h"


//websocket 特有的
//login / heartbeat / openrtmp / closertmp / 

using namespace std;

// #ifndef BOOL
// #define BOOL bool
// #endif
// 
// #ifndef FALSE
// #define FALSE false
// #endif
// 
// #ifndef TRUE
// #define TRUE true
// #endif


//通用的
//getdevices / adddevice / modifydevice / deletedevice
//getsensors / addsensor / modifysensor / deldetesensor / ctrlsensor
//getparams / setparam
//ptz

//
/*
//发给服务器的命令格式是 header+info, "header"必须有，"info"可选，"info"内的项目可选
{
"header" :
{
"command" : "cmd",
"result" : 0,
"deviceuuid" : "WWDSDJ9982HH88",
"msguuid" : "777777"
},
"info": {
"pwd": "222",
"user": "111",
}
}



"msgid": "887dd66s56s987s"
{
"header": {
"command" : "cmd",
"result" : 0,
"deviceuuid" : "WWDSDJ9982HH88",
"msguuid" : "777777"
},
"info": {
"pwd": "222",
"user": "111"
}
}
{
"header": {
"command" : "cmd",
"result" : 0,
"deviceuuid" : "WWDSDJ9982HH88",
"msguuid" : "777777"
},
"info": {
"user1": {
"pwd": "222",
"user": "111"
},
"user2": {
"pwd": "444",
"user": "333"
},
"user3": {
"pwd": "666",
"user": "555"
}
}
}
*/

inline void JsonMsgUuid(char * pszUuid) //pszUuid缓冲区至少为64
{
#ifdef _PLATFORM_WINDOW
	memset(pszUuid, 0, 64);
	GUID guid;
	CoInitialize(NULL);
	if (S_OK == CoCreateGuid(&guid))
	{
		_snprintf(pszUuid, 32,
			"%08x%04x%04x%02x%02x%02x%02x%02x%02x%02x%02x",
			guid.Data1,
			guid.Data2,
			guid.Data3,
			guid.Data4[0], guid.Data4[1],
			guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5],
			guid.Data4[6], guid.Data4[7]);
	}
	CoUninitialize();
#endif

}

inline BOOL JsoncppGetKeyValueAsString(string key, string &val, Json::Value v)//只遍历当前级，不遍历子节点
{
	BOOL bFind = FALSE;
	try
	{
		Json::Value::Members mem = v.getMemberNames();
		for (Json::Value::Members::iterator iter = mem.begin(); iter != mem.end(); iter++)
		{
			if ((key == *iter) && (v[*iter].type() == Json::stringValue))
			{
				val = v[*iter].asString();
				bFind = TRUE;
				break;
			}
		}
	}
	catch (...)
	{
		return FALSE;
	}
	return bFind;
}
inline BOOL JsoncppGetKeyValueAsInt(string key, int &val, Json::Value v)//只遍历当前级，不遍历子节点
{
	BOOL bFind = FALSE;

	try
	{
		Json::Value::Members mem = v.getMemberNames();
		for (Json::Value::Members::iterator iter = mem.begin(); iter != mem.end(); iter++)
		{
			if ((key == *iter) && (v[*iter].type() == Json::intValue))
			{
				val = v[*iter].asInt();
				bFind = TRUE;
				break;
			}
		}
	}
	catch (...)
	{
		return FALSE;
	}


	return bFind;
}
inline BOOL JsoncppGetJsonValue(char * msg, string str, Json::Value &v) //根据节点名称str获取Json::Value节点
{
	BOOL bFind = FALSE;
	do
	{
		try
		{
			//
			Json::CharReaderBuilder reader_builder;
			//std::unique_ptr<Json::CharReader> reader(reader_builder.newCharReader());
			Json::CharReader *reader(reader_builder.newCharReader());
			JSONCPP_STRING jsonerr;
			Json::Value jsonRoot;
			if (!reader->parse(msg, msg + strlen(msg), &jsonRoot, &jsonerr))
			{
				if (reader)
					delete reader;
				break;//格式错误
			}
			//int a = jsonRoot.size();
			Json::Value::Members mem = jsonRoot.getMemberNames();
			for (Json::Value::Members::iterator iter = mem.begin(); iter != mem.end(); iter++)
			{
				if (*iter == str)
				{
					v = jsonRoot[*iter];
					bFind = TRUE;
					if (reader)
						delete reader;
					break;
				}
			}
		}
		catch (...)
		{
			return FALSE;
		}
	} while (0);

	return bFind;
}

//----------------------------------------------------------------------------------------------------------------------------------
inline std::string JsonBuildResult(string command, int result, string deviceuuid, string msguuid)
{
	//根节点
	Json::Value root;
	//子节点
	Json::Value json_header;
	//子节点属性
	json_header["command"] = Json::Value(command);
	json_header["result"] = Json::Value(result);
	json_header["deviceuuid"] = Json::Value(deviceuuid);
	json_header["msguuid"] = Json::Value(msguuid);
	//子节点挂到根节点上
	root["header"] = Json::Value(json_header);
	std::string json_file = root.toStyledString();
	//
	return json_file;
}


inline BOOL JsonGetHeaderInfo(char * msg, string & command, int & result, string & deviceuuid, string & msguuid)
{
	BOOL bFind = FALSE;
	do
	{
		Json::Value v;
		if (JsoncppGetJsonValue(msg, "header", v))
		{
			if (!JsoncppGetKeyValueAsString("command", command, v))
			{
				break;
			}
			if (!JsoncppGetKeyValueAsInt("result", result, v))
			{
				break;
			}
			if (!JsoncppGetKeyValueAsString("deviceuuid", deviceuuid, v))
			{
				break;
			}
			if (!JsoncppGetKeyValueAsString("msguuid", msguuid, v))
			{
				break;
			}

			bFind = TRUE;
		}
	} while (0);
	return bFind;
}
//----------------------------------------------------------------------------------------------------------------------------------
//聊天
inline std::string JsonBuildChatMessage(const char * pszUuid, const char * pszChatInfo)
{
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("chat");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value("");
	//info
	Json::Value json_info;
	json_info["info"] = Json::Value(pszChatInfo);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
};
//----------------------------------------------------------------------------------------------------------------------------------
inline std::string JsonBuildDevicesGet(char * pszMsgUuid)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("getdevices");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value("");
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//
	root["header"] = Json::Value(json_header);
	//
	return root.toStyledString();
}

inline BOOL JsonGetDevicesAddModifyInfo(char * msg, string & describe)
{
	BOOL bFind = FALSE;
	do
	{
		Json::Value v;
		if (JsoncppGetJsonValue(msg, "info", v))
		{
			if (!JsoncppGetKeyValueAsString("describe", describe, v))
			{
				break;
			}
			bFind = TRUE;
		}
	} while (0);
	return bFind;
}
inline std::string JsonBuildDeviceInfoForAdd(const char * pszUuid, const char * pszDescribe, char * pszMsgUuid)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("adddevice");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["describe"] = Json::Value(pszDescribe);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
};

inline std::string JsonBuildDeviceInfoForModify(const char * pszUuid, const char * pszDescribe, char * pszMsgUuid)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("modifydevice");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["describe"] = Json::Value(pszDescribe);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
};
inline std::string JsonBuildDeviceInfoForDelete(const char * pszUuid, char * pszMsgUuid)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("deletedevice");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//
	root["header"] = Json::Value(json_header);
	//
	return root.toStyledString();
};
inline BOOL JsonGetDeviceDescribe(char * msg, string & describe)
{
	BOOL bFind = FALSE;
	do
	{
		Json::Value v;
		if (JsoncppGetJsonValue(msg, "info", v))
		{
			if (!JsoncppGetKeyValueAsString("describe", describe, v))
			{
				break;
			}
			bFind = TRUE;
		}
	} while (0);
	return bFind;
}



//pszPtz====left/right/up/down/leftup/leftdown/rightup/rightdown/positionset/postioncall
inline std::string JsonBuildPtzInfo(const char * pszUuid, const char * pszPtz, const int nVal, char * pszMsgUuid)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("ptz");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["ptz"] = Json::Value(pszPtz);
	json_info["ptzvalue"] = Json::Value(nVal);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
};

//pszParam====setimagesize/setbirate/set...
inline std::string JsonBuildParamInfoSet(const char * pszUuid, const char * pszParam, const char * pszParamVal, char * pszMsgUuid)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("setparam");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["param"] = Json::Value(pszParam);
	json_info["paramvalue"] = Json::Value(pszParamVal);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
};

//返回所有可用参数
/*
"info": {
"imagesize": {
"value": "111"
},
"birate": {
"value": "444"
},
}
*/
inline std::string JsonBuildParamInfoGet(const char * pszUuid, char * pszMsgUuid)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("getparams");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//
	root["header"] = Json::Value(json_header);
	//
	return root.toStyledString();
}
//返回所有传感器
inline std::string JsonBuildSensorsGet(const char * pszUuid, char * pszMsgUuid)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("getsensors");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//
	root["header"] = Json::Value(json_header);
	//
	return root.toStyledString();
}

inline std::string JsonBuildSensorInfoForAdd(const char * pszUuid, const char * pszSensorType, const char * pszSensorID, const char * pszSensorName, char * pszMsgUuid)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("addsensor");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["sensortype"] = Json::Value(pszSensorType);
	json_info["sensorid"] = Json::Value(pszSensorID);
	json_info["sensorname"] = Json::Value(pszSensorName);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
};
inline std::string JsonBuildSensorInfoForModify(const char * pszUuid, const char * pszSensorType, const char * pszSensorID, const char * pszSensorName, char * pszMsgUuid)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("modifysensor");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["sensortype"] = Json::Value(pszSensorType);
	json_info["sensorid"] = Json::Value(pszSensorID);
	json_info["sensorname"] = Json::Value(pszSensorName);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
};
inline std::string JsonBuildSensorInfoForDelete(const char * pszUuid, const char * pszSensorID, char * pszMsgUuid)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("deletesensor");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["sensorid"] = Json::Value(pszSensorID);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
};

inline std::string JsonBuildSensorCtrl(const char * pszUuid, const char * pszParam, const char * pszParamVal, char * pszMsgUuid)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("ctrlsensor");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["ctrlid"] = Json::Value(pszParam);
	json_info["ctrlvalue"] = Json::Value(pszParamVal);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
}

inline void JsonBuildHeaderAsValue(Json::Value &v, string command, int result, string deviceuuid, string msguuid)
{
	v["command"] = Json::Value(command);
	v["result"] = Json::Value(result);
	v["deviceuuid"] = Json::Value(deviceuuid);
	v["msguuid"] = Json::Value(msguuid);
}

inline void JsonBuildValueAsString(Json::Value &v, string key, string val)
{
	v[key] = Json::Value(val);
}

inline void JsonBuildValueAsInt(Json::Value &v, string key, int val)
{
	v[key] = Json::Value(val);
}

inline void JsonBuildValueAsValue(Json::Value &v, string key, Json::Value val)
{
	v[key] = Json::Value(val);
}

inline std::string JsonBuildValueToString(Json::Value v)
{
	return v.toStyledString();
}

inline BOOL JsonGetAllParams(char *msg, string &command, int &result, string &deviceuuid, string &msguuid,
	int &mainbitrate, int &mainframerate, int &mainimagesize,
	int &subbitrate, int &subimagesize,
	int &staturtion, int &brightness, int &contrast, int &sharpness, int &hue, int &flip,
	int &dhcp, string &wiredip,
	string &ssid, string &wifipwd,
	int &timezone, string &ntpserver,
	string &osdtitle,
	string &platip, int &platport, string &platid
	)
{
	BOOL bFind = FALSE;
	do
	{
		Json::Value v;
		if (JsoncppGetJsonValue(msg, "header", v))
		{
			if (!JsoncppGetKeyValueAsString("command", command, v))
			{
				break;
			}

			if (!JsoncppGetKeyValueAsInt("result", result, v))
			{
				break;
			}

			if (!JsoncppGetKeyValueAsString("deviceuuid", deviceuuid, v))
			{
				break;
			}

			if (!JsoncppGetKeyValueAsString("msguuid", msguuid, v))
			{
				break;
			}

			if (JsoncppGetJsonValue(msg, "info", v))
			{
				//-----------------------------------------------------------------------------
				if (!JsoncppGetKeyValueAsInt("mainbitrate", mainbitrate, v))
				{
					break;
				}

				if (!JsoncppGetKeyValueAsInt("mainframerate", mainframerate, v))
				{
					break;
				}

				if (!JsoncppGetKeyValueAsInt("mainimagesize", mainimagesize, v))
				{
					break;
				}
				//-----------------------------------------------------------------------------
				if (!JsoncppGetKeyValueAsInt("subbitrate", subbitrate, v))
				{
					break;
				}

				if (!JsoncppGetKeyValueAsInt("subimagesize", subimagesize, v))
				{
					break;
				}
				//-----------------------------------------------------------------------------

				if (!JsoncppGetKeyValueAsInt("staturtion", staturtion, v))
				{
					break;
				}

				if (!JsoncppGetKeyValueAsInt("brightness", brightness, v))
				{
					break;
				}

				if (!JsoncppGetKeyValueAsInt("contrast", contrast, v))
				{
					break;
				}
				if (!JsoncppGetKeyValueAsInt("sharpness", sharpness, v))
				{
					break;
				}

				if (!JsoncppGetKeyValueAsInt("hue", hue, v))
				{
					break;
				}

				if (!JsoncppGetKeyValueAsInt("flip", flip, v))
				{
					break;
				}

				//-----------------------------------------------------------------------------
				if (!JsoncppGetKeyValueAsInt("dhcp", dhcp, v))
				{
					break;
				}

				if (!JsoncppGetKeyValueAsString("wiredip", wiredip, v))
				{
					break;
				}
				//-----------------------------------------------------------------------------
				if (!JsoncppGetKeyValueAsString("ssid", ssid, v))
				{
					break;
				}

				if (!JsoncppGetKeyValueAsString("wifipwd", wifipwd, v))
				{
					break;
				}
				//-----------------------------------------------------------------------------
				if (!JsoncppGetKeyValueAsInt("timezone", timezone, v))
				{
					break;
				}

				if (!JsoncppGetKeyValueAsString("ntpserver", ntpserver, v))
				{
					break;
				}
				//-----------------------------------------------------------------------------
				if (!JsoncppGetKeyValueAsString("osdtitle", osdtitle, v))
				{
					break;
				}
				//-----------------------------------------------------------------------------
				if (!JsoncppGetKeyValueAsString("platip", platip, v))
				{
					break;
				}

				if (!JsoncppGetKeyValueAsInt("platport", platport, v))
				{
					break;
				}

				if (!JsoncppGetKeyValueAsString("platid", platid, v))
				{
					break;
				}
			}
			else
			{
				break;
			}
			bFind = TRUE;
		}
	} while (0);
	return bFind;
}

inline std::string JsonBuildSetMainBitRate(const char * pszUuid, char *pszMsgUuid, int pszParamVal)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("setparam");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["command"] = Json::Value("mainbitrate");
	json_info["mainbitrate"] = Json::Value(pszParamVal);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
}

inline std::string JsonBuildSetMainFrameRate(const char * pszUuid, char *pszMsgUuid, int pszParamVal)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("setparam");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["command"] = Json::Value("mainframerate");
	json_info["mainframerate"] = Json::Value(pszParamVal);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
}

inline std::string JsonBuildSetMainImageSize(const char * pszUuid, char *pszMsgUuid, int pszParamVal)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("setparam");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["command"] = Json::Value("mainimagesize");
	json_info["mainimagesize"] = Json::Value(pszParamVal);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
}

inline std::string JsonBuildSetImageFlip(const char * pszUuid, char * pszMsgUuid, int pszParamVal)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("setparam");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["command"] = Json::Value("flip");
	json_info["flip"] = Json::Value(pszParamVal);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
}

inline std::string JsonBuildSetImageStaturtion(const char * pszUuid, char * pszMsgUuid, int pszParamVal)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("setparam");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["command"] = Json::Value("staturtion");
	json_info["staturtion"] = Json::Value(pszParamVal);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
}

inline std::string JsonBuildSetImageBrightness(const char * pszUuid, char * pszMsgUuid, int pszParamVal)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("setparam");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["command"] = Json::Value("brightness");
	json_info["brightness"] = Json::Value(pszParamVal);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
}

inline std::string JsonBuildSetImageContrast(const char * pszUuid, char * pszMsgUuid, int pszParamVal)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("setparam");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["command"] = Json::Value("contrast");
	json_info["contrast"] = Json::Value(pszParamVal);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
}

inline std::string JsonBuildSetImageSharpness(const char * pszUuid, char * pszMsgUuid, int pszParamVal)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("setparam");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["command"] = Json::Value("sharpness");
	json_info["sharpness"] = Json::Value(pszParamVal);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
}

inline std::string JsonBuildSetImageHue(const char * pszUuid, char * pszMsgUuid, int pszParamVal)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("setparam");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["command"] = Json::Value("hue");
	json_info["hue"] = Json::Value(pszParamVal);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
}

inline std::string JsonBuildSetWiredNetwork(const char * pszUuid, char * pszMsgUuid, int pszDhcpVal, char *pszIPVal)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("setparam");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["command"] = Json::Value("wirednet");
	json_info["dhcp"] = Json::Value(pszDhcpVal);
	json_info["wiredip"] = Json::Value(pszIPVal);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
}

inline std::string JsonBuildSetWifiNetwork(const char * pszUuid, char * pszMsgUuid, char *pszSsidVal, char *pszPwdVal)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("setparam");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["command"] = Json::Value("wifi");
	json_info["ssid"] = Json::Value(pszSsidVal);
	json_info["wifipwd"] = Json::Value(pszPwdVal);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
}

inline std::string JsonBuildSetTimeZone(const char * pszUuid, char * pszMsgUuid, int pszParamVal)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("setparam");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["command"] = Json::Value("timezone");
	json_info["timezone"] = Json::Value(pszParamVal);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
}

inline std::string JsonBuildSetNtpServer(const char * pszUuid, char * pszMsgUuid, char *pszParamVal)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("setparam");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["command"] = Json::Value("ntpserver");
	json_info["ntpserver"] = Json::Value(pszParamVal);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
}

inline std::string JsonBuildSetOsdTitle(const char * pszUuid, char * pszMsgUuid, char *pszParamVal)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("setparam");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["command"] = Json::Value("osdtitle");
	json_info["osdtitle"] = Json::Value(pszParamVal);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
}

inline std::string JsonBuildSetPlatInfo(const char * pszUuid, char * pszMsgUuid, char *pszIPVal, int pszPortVal, char *pszIDVal)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("setparam");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["command"] = Json::Value("platinfo");
	json_info["platip"] = Json::Value(pszIPVal);
	json_info["platport"] = Json::Value(pszPortVal);
	json_info["platid"] = Json::Value(pszIDVal);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
}

inline std::string JsonBuildSetSubBitRate(const char * pszUuid, char * pszMsgUuid, int pszParamVal)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("setparam");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["command"] = Json::Value("subbitrate");
	json_info["subbitrate"] = Json::Value(pszParamVal);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
}

inline std::string JsonBuildSetSubImageSize(const char * pszUuid, char * pszMsgUuid, int pszParamVal)
{
	JsonMsgUuid(pszMsgUuid);
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value("setparam");
	json_header["result"] = Json::Value(0);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	json_info["command"] = Json::Value("subimagesize");
	json_info["subimagesize"] = Json::Value(pszParamVal);
	//
	root["header"] = Json::Value(json_header);
	root["info"] = Json::Value(json_info);
	//
	return root.toStyledString();
}


struct JSON_DEVICE_INFO
{
	int dev_online;
	char dev_uuid[32];
	char dev_name[128];
	char rtmpip[64];
	char rtmpuuid[64];
};
inline std::string JsonBuildDevicesList(const char * command, int result,const char * pszUuid, const char * pszMsgUuid,const char *serverip, vector<JSON_DEVICE_INFO> r)
{
	//root
	Json::Value root;
	//heder
	Json::Value json_header;
	json_header["command"] = Json::Value(command);
	json_header["result"] = Json::Value(result);
	json_header["deviceuuid"] = Json::Value(pszUuid);
	json_header["msguuid"] = Json::Value(pszMsgUuid);
	//info
	Json::Value json_info;
	for (int i = 0 ; i < (int)r.size(); i++)
	{
		Json::Value json_device;
		json_device["online"] = Json::Value(r[i].dev_online);
		json_device["uuid"] = Json::Value(r[i].dev_uuid);
		json_device["name"] = Json::Value(r[i].dev_name);
		json_device["rtmpip"] = Json::Value(serverip);
		json_device["rtmpuuid"] = Json::Value(r[i].rtmpuuid);
		//
		json_info.append(json_device);
	}
	//
	root["header"] = Json::Value(json_header);
	if (r.size() > 0)
	{
		root["info"] = Json::Value(json_info);
	}
	
	//
	return root.toStyledString();
}


#endif

