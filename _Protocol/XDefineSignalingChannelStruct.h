#ifndef a_define_signaling_channel_struct_qewkjh34523kjeksuyvekr
#define a_define_signaling_channel_struct_qewkjh34523kjeksuyvekr

//----------------------------------------
// kirainstorm
// https://github.com/kirainstorm
//----------------------------------------

//---------------------------------------------------------------------------------------------------------------------------
//		���ͷ�ļ��� <�豸��Ϣͨ�� �� ���������>  <TCP�ͻ��� �� ���������>ͨѶ�����ݶ���
//      ��Ϣ��signaling_channel_head_t + json_data
//---------------------------------------------------------------------------------------------------------------------------
enum signaling_channel_cmd_def
{
	SINGNALING_CHANNEL_CMD_REDIRECTION_DEVICE = 0,
	SINGNALING_CHANNEL_CMD_REDIRECTION_USER,
	SINGNALING_CHANNEL_CMD_REDIRECTION_CLASSIC_GET_DEVICES,
	SINGNALING_CHANNEL_CMD_LOGIN = 20,				//��½
	SINGNALING_CHANNEL_CMD_LOGIN_NEED_SESSION,		//key s -> c
	SINGNALING_CHANNEL_CMD_HEARBEAT,				//����
	SINGNALING_CHANNEL_CMD_LOGOUT,					//ע��
	//
	SINGNALING_CHANNEL_CMD_IOT_DEVICE,				//IOT������޸�ɾ��
	SINGNALING_CHANNEL_CMD_IOT_DEVICE_SENSORS,		//IOT�Ĵ�����������޸�ɾ��
	SINGNALING_CHANNEL_CMD_TRANS,					//͸������
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

//�ṹ��
struct signaling_channel_head_t	//Ϊ��ֹ���ֱ��������ֽڶ�������,ʹ��4��uint32_t���16�ֽڵ�ͷ
{
	uint32_t cmd;		//signaling_channel_cmd_def
	uint32_t result;	//signaling_channel_cmd_result
	uint32_t session;
	uint32_t datalen;
};

//---------------------------------------------------------------------------------------------------------------------------
//�豸/�û����ͻ�ȡ�ض�������Ľṹ��//����������signaling_channel_head_t+����(�ַ���)
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
