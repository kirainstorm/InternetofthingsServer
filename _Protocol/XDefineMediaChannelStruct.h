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
//std::tm��android��windows���ǲ�ͬ��
// #include "PlatformDefine.h"


//---------------------------------------------------------------------------------------------------------------------------
//#########�豸/������/�ͻ��� ����
//---------------------------------------------------------------------------------------------------------------------------
#define MAGIC_NUM 0XAA55
enum emMEDIA_COMMAND_DEFINE
{
	//---------------------------------------------------------------------------------------------------------------
	//�û���½���� //1:NEED_SESSIONID //2:LOGIN
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
	//�����ǿͻ��������������
	XMEDIA_COMMAND_OPEN_STREAM = 20000,
	XMEDIA_COMMAND_CLOSE_STREAM,
	XMEDIA_COMMAND_GET_DEVICE_LIST,	//�����ķ�������ȡ�豸�б�,��ʹ�ö����ӵķ�ʽ��ȡ(����������) (ͬʱ����������������Ƶ) c->S
	XMEDIA_COMMAND_GET_STORE_HISTORY_LOG,//��ȡ¼���б�,��ʹ�ö����ӵķ�ʽ��ȡ  client->media
	XMEDIA_COMMAND_OPEN_HIS_STREAM,//����	//�����������ظ�cmd�����ص���XMEDIA_COMMAND_AV_STREAM����ע���ж�result����_CMD_OF_SERVER_RESULT_DOWNLOAD_XXXX
	XMEDIA_COMMAND_CLOSE_HIS_STREAM,//�ر�����
};
enum emMEDIA_RESULT_DEFINE
{
	XMEDIA_RESULT_OK = 0,
	XMEDIA_RESULT_ERROR,
	XMEDIA_RESULT_NOTREGISTER,//�豸δע�ᣬ��ע�⣬�����ż���ղ����ûظ�
	//
	XMEDIA_RESULT_DOWNLOAD_START, //���ؿ�ʼ��־
	XMEDIA_RESULT_DOWNLOAD_END,//���ؽ�����־
	XMEDIA_RESULT_DOWNLOAD_DATA,//���ݱ�־
	XMEDIA_RESULT_DOWNLOAD_ERROR,//�����־
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
	uint64_t tick;//���ʱ��-�豸��Ҫʹ��Ӳ��ʱ�䱣֤����ֵһֱ�ǵ�����
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
	//nAudioBitRate��nAudioBitsPerSample�ڲ�ͬ�ĵ��������п���ֻ��һ�֣������ǿ��Լ������һ����
	uint16_t nAudioChannels;//ͨ����: ������Ϊ 1, ������Ϊ 2
	uint16_t nAudioSamplesRate;	// nAudioSamplesPerSec //ÿ��Ĳ�����������������//8000,44100��
	uint16_t nAudioBitRate;	//���ݴ����ƽ������(ÿ����ֽ���)  �Զ����ֵ
	uint16_t nAudioBitsPerSample;//������С(�ֽ�)����16 
	//
	ST_XMEDIA_TIME time;//�豸��ʹ�ò������ֶ�
};
struct stFrameX_T
{
	ST_XMEDIA_HEAD header;
	ST_XMEDIA_AVHEAD avheader;
	unsigned int data[0];//�������ṹ
};

//ST_XMEDIA_LOGIN_INFO
//�豸��ֻ���pwd
//�ͻ��ˣ�devid//�豸�����ݿ��ID--devchannel//һ����0����ʹ��NVR��ÿ��ͨ������һ������ID
struct ST_XMEDIA_LOGIN_INFO
{
	int reserve[2];
	char user[32];
	char despwd[128];
};
//---------------------------------------------------------------------------------------------------------------------------










//---------------------------------------------------------------------------------------------------------------------------
//######### ����Ϊ�ͻ��������������ʹ��
//--------- �ò�����ƽ̨ͨѶʹ��
struct ST_CLASSIC_DEVICE_INFO_BASE
{
	int dev_is_publish;			//0:˽���豸  1��publish device
	int dev_online;				//�Ƿ�����
	int dev_browse_num;			//����û�����
								//
	int	dev_id;					//�豸�����ݿ��id
	int	dev_areaid;				//����id
	int dev_parent_areaid;		//����id�ĸ�id
								//
	char dev_name[64];			//�豸����
	char dev_area_name[64];		//��������
	char dev_media_ip[16];		//���ķ�����ip
	//char dev_publish_ip[64];	//�ַ�������ip
								//
	//int	dev_media_port;			//���ķ�����Ԥ���˿�
	//int dev_playback_port;		//���ķ������طŶ˿�
	//int dev_publish_port;		//�ַ�������Ԥ���˿�
								//


	char reserve[88];
};
struct ST_SREVER_OPEN_STREAM
{
	char devid[32];
};

//����ʷ��Ƶ
struct ST_SREVER_OPEN_HIS_STREAM
{
	char devid[32];
	ST_XMEDIA_TIME tmBeginTime;
	ST_XMEDIA_TIME tmEndTime;
};

//��ȡ��ĳһʱ��ε���ʷ��Ƶ
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
	ST_SREVER_HIS_LOG t[MAX_SREVER_HIS_LOG_LIST_COUNT];//��෵��MAX_SREVER_HIS_LOG_LIST_COUNT���ļ�
};

//---------------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------







/*
�������ṹ����ռ�ڴ��С��
Ϊ�ڴ��Ż���һ������ֽڶ��롣
�ֽڶ�����Ǳ����洢�ĵ�ַ�Ǳ�������Ч�ֽڶ���ֵ��������������
address%������Ч�ֽڶ���ֵ = 0;
�����ֽڶ���ֵ
1) �����������ֽڶ���ֵ
�����ͱ���
����char�����ݣ����������ֵΪ1������short��Ϊ2������int,float,double���ͣ����������ֵΪ4����λ�ֽڡ�
�ṹ���ͻ��������ͣ�
�����ֽڶ���ֵ�Ĵ�С�����Ա������������Ҫ�����Ч�ֽڶ���ֵ
2) ������Ҫ����ֽڶ���ֵ��
����������Ҫ����ֽڶ���ֵ
3) ������Ч���ֽڶ���ֵ��
�������ֽڶ���ֵ�ͱ�����Ҫ����ֽڶ���ֵ��ȡ��С�ġ�
��ʵ�ֽڶ����ϸ�ں;��������ʵ����أ���һ����ԣ���������׼��
1) �ṹ��������׵�ַ�ܹ��������������ͳ�Ա�Ĵ�С��������
2) �ṹ��ÿ����Ա����ڽṹ���׵�ַ��ƫ�������ǳ�Ա��С���߳�Ա���ӳ�Ա��С��ֻҪ�ó�Ա���ӳ�Ա������˵�����飬�ṹ��ȣ�����������������Ҫ���������ڳ�Ա֮���������ֽڣ���������ڶ����ṹ������ĵ�ַ�ռ䡣
3) �ṹ����ܴ�СΪ�ṹ�����������ͳ�Ա��С����������������Ҫ������������ĩһ����Ա֮���������ֽڡ�
ag.1
typedef struct bb
{
int id;             //[0]....[3]
double weight;      //[8].....[15]������������ԭ��
float height;      //[16]..[19],�ܳ�ҪΪ����������,����[20]...[23]����������ԭ��
}BB;

typedef struct aa
{
char name[2];     //[0],[1]
int  id;         //[4]...[7]��������������������ԭ��

double score;     //[8]....[15]��������
short grade;    //[16],[17]����������������
BB b;             //[24]......[47]��������������������ԭ��
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
printf("%d\n",sizeof(too));       //���Ϊ24

��ͬ�����г�Ա���� int�������ֽڶ���ֵΪ4����������Ĵ�С����Ϊdouble���ֽڶ���ֵΪ8��

*/

#endif
