#ifndef a_define_signaling_channel_struct_qewkjh34523kjeksuyvekr
#define a_define_signaling_channel_struct_qewkjh34523kjeksuyvekr

//----------------------------------------
// kirainstorm
// https://github.com/kirainstorm
//----------------------------------------

//---------------------------------------------------------------------------------------------------------------------------
//		这个头文件是 <设备消息通道 与 信令服务器>  <TCP客户端 与 信令服务器>通讯的数据定义
//      消息：signaling_channel_head_t + json_data
//---------------------------------------------------------------------------------------------------------------------------
enum signaling_channel_cmd_def
{
	SINGNALING_CHANNEL_CMD_REDIRECTION_DEVICE = 0,
	SINGNALING_CHANNEL_CMD_REDIRECTION_USER,
	SINGNALING_CHANNEL_CMD_REDIRECTION_CLASSIC_GET_DEVICES,
	SINGNALING_CHANNEL_CMD_LOGIN = 20,				//登陆
	SINGNALING_CHANNEL_CMD_LOGIN_NEED_SESSION,		//key s -> c
	SINGNALING_CHANNEL_CMD_HEARBEAT,				//心跳
	SINGNALING_CHANNEL_CMD_LOGOUT,					//注销
	//
	SINGNALING_CHANNEL_CMD_IOT_DEVICE,				//IOT的添加修改删除
	SINGNALING_CHANNEL_CMD_IOT_DEVICE_SENSORS,		//IOT的传感器的添加修改删除
	SINGNALING_CHANNEL_CMD_TRANS,					//透明传输
	SINGNALING_CHANNEL_CMD_GPS,
	SINGNALING_CHANNEL_CMD_ALARM,
	//


	SINGNALING_CHANNEL_CMD_CHAT = 9999,
};
enum signaling_channel_cmd_result
{
	SINGNALING_CHANNEL_RESULT_OK = 0,
	SINGNALING_CHANNEL_RESULT_ERROR = -1,
};

//结构体
struct signaling_channel_head_t	//为防止各种编译器的字节对齐问题,使用4个uint32_t组成16字节的头
{
	uint32_t cmd;		//signaling_channel_cmd_def
	uint32_t result;	//signaling_channel_cmd_result
	uint32_t session;
	uint32_t datalen;
};

//---------------------------------------------------------------------------------------------------------------------------
//设备/用户发送获取重定向命令的结构体//服务器返回signaling_channel_head_t+数据(字符串)
struct login_redirection_t
{
	char id[32];
};


//---------------------------------------------------------------------------------------------------------------------------
struct signaling_channel_device_login_t
{
	char userdes[128];
	char uuiddes[128];
};
struct signaling_channel_user_login_t
{
	char userdes[128];
	char pwddes[128];
};
//---------------------------------------------------------------------------------------------------------------------------

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
