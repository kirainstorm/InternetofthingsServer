// TestConnectAndLogin.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ConnectAndLoginUser.h"

//���Է�������ע�⣡��ʹ��release�汾�ķ���������debug��release�µ����ܲ�ʮ���ǧ��
//
//
// 
//---------------------------------------------------------------------------
//
// �û���½ѹ�����ܲ��ԣ�
//
// ʹ��N��ConnectAndLoginUser�̲߳��������û���½����������ѹ�����ܲ���
// ����Ŀ��
// 1��������socket�Ƿ����������գ�FIN_WAIT,FIN_TIMEOU֮��
// 2�����������Ƿ�����new/delete�������ڴ�й©
// ���Է�ʽ������������²���
// 1���������̣�connect - login1 - login2 - disconnect
// 2������������ conncet - disconnect
// 3������������ conncet - login1 - disconnect
//---------------------------------------------------------------------------
//
//
//---------------------------------------------------------------------------
// 20180629 �������Ʒ������� ���Է���
// �ڱ�������3000���̷߳��ʰ����Ʒ������ķ������ˣ���������10M��
// ����3k����ʱ��������accept�ص����������ȶ�������5��û�лص�������1�뼸�ٸ��ص�
// accept�ص�������������������޹�
//---------------------------------------------------------------------------
// 20180702 ���������ԡ�
// �ڱ�������3000���̷߳��� ���� ��������
// 3k,4k�������ӶϿ���û�ж���
//---------------------------------------------------------------------------

int main()
{
	XNetStartStreamManager();

	CXCounterConnectAndLoginUser::Instance().GetOnlyConnectErrorCount();

	for (int i = 0 ; i < 4000 ; i ++)
	{
		CConnectAndLoginUser * p = new CConnectAndLoginUser("13800138000", "13800138000");
	}

	while (true)
	{
		printf("Sec:[%I64d]\t\t\tConnError[%I64d]\t\t\tSessionError[%I64d]\t\t\tLoginError[%I64d]\t\t\tOK[%I64d]\n",
			(GetTickCount64() - CXCounterConnectAndLoginUser::Instance().m_nServerStartTick) /1000,
			CXCounterConnectAndLoginUser::Instance().GetOnlyConnectErrorCount(),
			CXCounterConnectAndLoginUser::Instance().GetOnlyLoginStepOne(),
			CXCounterConnectAndLoginUser::Instance().GetOnlyLoginStepOne_1(),
			CXCounterConnectAndLoginUser::Instance().GetLoginStepTwo());

		Sleep(1000);
	}



    return 0;
}

