#ifndef a_define_media_channel_structoyhwedc456u23v
#define a_define_media_channel_structoyhwedc456u23v

//----------------------------------------
// kirainstorm
// https://github.com/kirainstorm
//----------------------------------------
//enum
//in "struct" must not use "enum" ,"sizeof" return wrong size.
//
//std::tm
//std::tm在android和windows下是不同的
// #include "PlatformDefine.h"


//---------------------------------------------------------------------------------------------------------------------------
//#########设备/服务器/客户端 公用
//---------------------------------------------------------------------------------------------------------------------------
#define MAGIC_NUM 0XAA55
enum emMEDIA_COMMAND_DEFINE
{
	//---------------------------------------------------------------------------------------------------------------
	//用户登陆步骤 //1:NEED_SESSIONID //2:LOGIN
	XMEDIA_COMMAND_LOGIN = 0,
	XMEDIA_COMMAND_LOGIN_NEED_SESSIONID,
	XMEDIA_COMMAND_HEARTBEAT,
	XMEDIA_COMMAND_LOGOUT,
	//
	XMEDIA_COMMAND_OPEN_MAIN_STREAM,
	XMEDIA_COMMAND_CLOSE_MAIN_STREAM,
	XMEDIA_COMMAND_OPEN_SUB_STREAM,
	XMEDIA_COMMAND_CLOSE_SUB_STREAM,
	XMEDIA_COMMAND_AV_STREAM,
	//以下是客户端与服务器交互
	XMEDIA_COMMAND_OPEN_STREAM = 20000,
	XMEDIA_COMMAND_CLOSE_STREAM,
	XMEDIA_COMMAND_GET_DEVICE_LIST,	//从中心服务器获取设备列表,请使用短链接的方式获取(包括了区域) (同时包括公共发布的视频) c->S
	XMEDIA_COMMAND_GET_STORE_HISTORY_LOG,//获取录像列表,请使用短链接的方式获取  client->media
	XMEDIA_COMMAND_OPEN_HIS_STREAM,//下载	//服务器不返回该cmd，返回的是XMEDIA_COMMAND_AV_STREAM，请注意判断result区分_CMD_OF_SERVER_RESULT_DOWNLOAD_XXXX
	XMEDIA_COMMAND_CLOSE_HIS_STREAM,//关闭下载
};
enum emMEDIA_RESULT_DEFINE
{
	XMEDIA_RESULT_OK = 0,
	XMEDIA_RESULT_ERROR,
	XMEDIA_RESULT_NOTREGISTER,//设备未注册，请注意，你可能偶尔收不到该回复
	//
	XMEDIA_RESULT_DOWNLOAD_START, //下载开始标志
	XMEDIA_RESULT_DOWNLOAD_END,//下载结束标志
	XMEDIA_RESULT_DOWNLOAD_DATA,//数据标志
	XMEDIA_RESULT_DOWNLOAD_ERROR,//出错标志
};
enum emMEDIA_FRAME_TYPE_DEFINE
{
	//H264
	XMEDIA_FREAM_TYPE_H264_IFRAME = 0,
	XMEDIA_FREAM_TYPE_H264_PFRAME,
	XMEDIA_FREAM_TYPE_H264_OTHERS,
	//H265
	XMEDIA_FREAM_TYPE_H265_IFRAME,
	XMEDIA_FREAM_TYPE_H265_PFRAME,
	XMEDIA_FREAM_TYPE_H265_OTHERS,
	//MPEG4
	XMEDIA_FREAM_TYPE_MPEG4,
	//MJPEG
	XMEDIA_FREAM_TYPE_MJPEG,
	//Audio
	XMEDIA_FREAM_TYPE_PCM,
	XMEDIA_FREAM_TYPE_ADPCM,
	XMEDIA_FREAM_TYPE_G711A,
	XMEDIA_FREAM_TYPE_G711U,
	XMEDIA_FREAM_TYPE_AAC,
	//JPG
	XMEDIA_FREAM_TYPE_JPEG
};
struct ST_XMEDIA_TIME {
	uint16_t wYear;
	uint16_t wMonth;
	uint16_t wDayOfWeek;
	uint16_t wDay;
	uint16_t wHour;
	uint16_t wMinute;
	uint16_t wSecond;
	uint16_t wMilliseconds;
};
struct ST_XMEDIA_HEAD
{
	uint32_t cmd;//emMEDIA_COMMAND_DEFINE
	uint32_t result;//emMEDIA_RESULT_DEFINE
	uint32_t session;
	uint32_t datalen;
	uint64_t tick;//相对时间-设备需要使用硬件时间保证该数值一直是递增的
	uint32_t reserve[2];
};
struct ST_XMEDIA_AVHEAD
{
	//emMEDIA_FRAME_TYPE_DEFINE
	uint16_t nAVFrameType;
	//video
	uint16_t nVideoFrameRate;
	uint16_t nVideoWidth;
	uint16_t nVideoHeight;
	//audio
	//nAudioBitRate，nAudioBitsPerSample在不同的第三方库中可能只用一种，但是是可以计算出另一个的
	uint16_t nAudioChannels;//通道数: 单声道为 1, 立体声为 2
	uint16_t nAudioSamplesRate;	// nAudioSamplesPerSec //每秒的采样样本数，采样率//8000,44100等
	uint16_t nAudioBitRate;	//数据传输的平均速率(每秒的字节数)  自定义的值
	uint16_t nAudioBitsPerSample;//采样大小(字节)比如16 
	//
	ST_XMEDIA_TIME time;//设备不使用不填充该字段
};
struct stFrameX_T
{
	ST_XMEDIA_HEAD header;
	ST_XMEDIA_AVHEAD avheader;
	unsigned int data[0];//灵活数组结构
};

//ST_XMEDIA_LOGIN_INFO
//设备：只填充pwd
//客户端：devid//设备在数据库的ID--devchannel//一定是0，即使是NVR，每个通道都有一个独立ID
struct ST_XMEDIA_LOGIN_INFO
{
	int reserve[2];
	char user[32];
	char despwd[128];
};
//---------------------------------------------------------------------------------------------------------------------------










//---------------------------------------------------------------------------------------------------------------------------
//######### 以下为客户端与服务器交互使用
//--------- 该参数与平台通讯使用
struct ST_CLASSIC_DEVICE_INFO_BASE
{
	int dev_is_publish;			//0:私有设备  1：publish device
	int dev_online;				//是否在线
	int dev_browse_num;			//浏览用户数量
								//
	int	dev_id;					//设备在数据库的id
	int	dev_areaid;				//区域id
	int dev_parent_areaid;		//区域id的父id
								//
	char dev_name[64];			//设备名称
	char dev_area_name[64];		//区域名称
	char dev_media_ip[16];		//核心服务器ip
	//char dev_publish_ip[64];	//分发服务器ip
								//
	//int	dev_media_port;			//核心服务器预览端口
	//int dev_playback_port;		//核心服务器回放端口
	//int dev_publish_port;		//分发服务器预览端口
								//


	char reserve[88];
};
struct ST_SREVER_OPEN_STREAM
{
	char devid[32];
};

//打开历史视频
struct ST_SREVER_OPEN_HIS_STREAM
{
	char devid[32];
	ST_XMEDIA_TIME tmBeginTime;
	ST_XMEDIA_TIME tmEndTime;
};

//获取在某一时间段的历史视频
struct ST_SREVER_GET_HIS_LOG
{
	char devid[32];
	ST_XMEDIA_TIME tmBeginTime;
	ST_XMEDIA_TIME tmEndTime;
};
struct ST_SREVER_HIS_LOG
{
	int nHislogID;
	int reserve;
	char szFile[128];
	ST_XMEDIA_TIME begin_time_tm;
	ST_XMEDIA_TIME end_time_tm;
};
#define MAX_SREVER_HIS_LOG_LIST_COUNT 100
struct ST_SREVER_HIS_LOG_LIST
{
	ST_SREVER_HIS_LOG t[MAX_SREVER_HIS_LOG_LIST_COUNT];//最多返回MAX_SREVER_HIS_LOG_LIST_COUNT个文件
};

//---------------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------







/*
（三）结构体所占内存大小：
为内存优化，一般采用字节对齐。
字节对齐就是变量存储的地址是变量的有效字节对齐值的整数倍，即：
address%最终有效字节对齐值 = 0;
变量字节对齐值
1) 变量的自身字节对齐值
简单类型变量
对于char型数据，其自身对齐值为1，对于short型为2，对于int,float,double类型，其自身对齐值为4，单位字节。
结构类型或联合类型：
自身字节对齐值的大小是其成员中最大基本类型要求的有效字节对齐值
2) 编译器要求的字节对齐值：
编译器设置要求的字节对齐值
3) 最终有效的字节对齐值：
在自身字节对齐值和编译器要求的字节对齐值中取较小的。
其实字节对齐的细节和具体编译器实现相关，但一般而言，满足三个准则：
1) 结构体变量的首地址能够被其最宽基本类型成员的大小所整除。
2) 结构体每个成员相对于结构体首地址的偏移量都是成员大小或者成员的子成员大小（只要该成员有子成员，比如说是数组，结构体等）的整数倍，如有需要编译器会在成员之间加上填充字节；例如上面第二个结构体变量的地址空间。
3) 结构体的总大小为结构体最宽基本类型成员大小的整数倍，如有需要编译器会在最末一个成员之后加上填充字节。
ag.1
typedef struct bb
{
int id;             //[0]....[3]
double weight;      //[8].....[15]　　　　　　原则１
float height;      //[16]..[19],总长要为８的整数倍,补齐[20]...[23]　　　　　原则３
}BB;

typedef struct aa
{
char name[2];     //[0],[1]
int  id;         //[4]...[7]　　　　　　　　　　原则１

double score;     //[8]....[15]　　　　
short grade;    //[16],[17]　　　　　　　　
BB b;             //[24]......[47]　　　　　　　　　　原则２
}AA;

int main()
{
AA a;
cout<<sizeof(a)<<" "<<sizeof(BB)<<endl;
return 0;
}

ag.2
typedef union {char i; int k[2]; char c;} DATE;
struct data { int cat; DATE cow; char dog;} too;
DATE max;
printf("%d\n",sizeof(too));       //结果为24

共同体中有成员类型 int，所以字节对齐值为4，忽略数组的大小，若为double，字节对齐值为8。

*/

#endif
