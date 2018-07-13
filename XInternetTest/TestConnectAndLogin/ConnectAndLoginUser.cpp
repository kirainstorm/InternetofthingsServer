#include "stdafx.h"
#include "ConnectAndLoginUser.h"
#include "XDefineMediaChannelStruct.h"
#include "XDefineSignalingChannelStruct.h"
#include "XStream.hpp"
#include "XDes.h"
#include "Pool.hpp"

CConnectAndLoginUser::CConnectAndLoginUser(string user, string pwd)
{
	m_bExit = FALSE;
	m_hThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkerThread, this, 0, NULL);
}
CConnectAndLoginUser::~CConnectAndLoginUser()
{
	if (m_hThread)
	{
		m_bExit = TRUE;
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}

void CConnectAndLoginUser::WorkerThread(void* param)
{
	CConnectAndLoginUser *p = (CConnectAndLoginUser*)param;
	p->Worker();
}
struct ST_DEVICE_SIGNALING_SEND_BUFFER
{
	signaling_channel_head_t head;
	char msg[4096 - sizeof(signaling_channel_head_t)];
};
void CConnectAndLoginUser::Worker()
{
	uint32_t m_nSessionID;
	CXNetStream *m_pTcpStream = nullptr;
	//
	signaling_channel_head_t msg_key_recv;
	ST_DEVICE_SIGNALING_SEND_BUFFER *pSendBuffer = new ST_DEVICE_SIGNALING_SEND_BUFFER;
	//m_pTcpStream = XNetCreateStream4Connect(s_ip, 6501, 10);
	//m_pTcpStream = XNetCreateStream4Connect(s_ip, 6601, 10);

	while (!m_bExit)
	{
		do
		{
			int ret = 0;
			srand(time(NULL));
			//int _index = rand() % 3;
			//--------------------------------------------------------------------------------------------------------------------------
			//--------------------------------------------------------------------------------------------------------------------------
			//--------------------------------------------------------------------------------------------------------------------------
			if (m_pTcpStream)
			{
				m_pTcpStream->Release();
				m_pTcpStream = NULL;
			}
			m_pTcpStream = XNetCreateStream4Connect("47.96.249.142", 6500, 1);
			//m_pTcpStream = XNetCreateStream4Connect("47.106.200.197", 6500, 3);
			if (0 != XNetConnectStream(m_pTcpStream))
			{
				CXCounterConnectAndLoginUser::Instance().AddOnlyErrorConnect();//<<<-----
				break;
			}
			//@@@@
// 			if (0 == _index)
// 			{
// 				CXCounterConnectAndLoginUser::Instance().AddOnlyConnect();
// 				break;
// 			}
			//@@@@
			//--------------------------------------------------------------------------------------------------------------------------
			//--------------------------------------------------------------------------------------------------------------------------
			//--------------------------------------------------------------------------------------------------------------------------
			//NEED_SESSIONID
			memset(pSendBuffer, 0, sizeof(ST_DEVICE_SIGNALING_SEND_BUFFER));
			pSendBuffer->head.cmd = SINGNALING_CHANNEL_CMD_LOGIN_NEED_SESSION;
			ret = m_pTcpStream->SyncWriteAndRead(pSendBuffer, sizeof(signaling_channel_head_t), &msg_key_recv, sizeof(signaling_channel_head_t), 3);
			if (0 != ret)
			{
				CXCounterConnectAndLoginUser::Instance().AddOnlyLoginStepOne();//@@@@
				break;
			}
			//
			if ((msg_key_recv.cmd != SINGNALING_CHANNEL_CMD_LOGIN_NEED_SESSION) || (msg_key_recv.result != SINGNALING_CHANNEL_RESULT_OK))
			{
				CXCounterConnectAndLoginUser::Instance().AddOnlyLoginStepOne();//@@@@
				break;
			}
			//
			m_nSessionID = msg_key_recv.session;
			//--------------------------------------------------------------------------------------------------------------------------
			//--------------------------------------------------------------------------------------------------------------------------
			//--------------------------------------------------------------------------------------------------------------------------
			//Ê¹ÓÃkey¼ÓÃÜ£¬µÇÂ½
			signaling_channel_user_login_t stLogin;
			memset(&stLogin, 0, sizeof(signaling_channel_user_login_t));
			XDESEncode("13800", m_nSessionID, stLogin.userdes);
			XDESEncode("13800", m_nSessionID, stLogin.pwddes);
			//
			memset(pSendBuffer, 0, sizeof(ST_DEVICE_SIGNALING_SEND_BUFFER));
			// 			//
			pSendBuffer->head.cmd = SINGNALING_CHANNEL_CMD_LOGIN;
			pSendBuffer->head.session = m_nSessionID;
			pSendBuffer->head.datalen = sizeof(signaling_channel_user_login_t);
			//
			memcpy(pSendBuffer->msg, &stLogin, sizeof(signaling_channel_user_login_t));
			//
			ret = m_pTcpStream->SyncWriteAndRead(pSendBuffer, sizeof(signaling_channel_head_t) + sizeof(signaling_channel_user_login_t), &msg_key_recv, sizeof(signaling_channel_head_t),3);

// 			if (2 == _index)
// 			{
// 				CXCounterConnectAndLoginUser::Instance().AddLoginStepTwo();
// 				break;
// 			}

// 			if (0 == ret && !((msg_key_recv.cmd != SINGNALING_CHANNEL_CMD_LOGIN) || (msg_key_recv.result != SINGNALING_CHANNEL_RESULT_OK)))
// 			{
// 				CXCounterConnectAndLoginUser::Instance().AddLoginStepTwo();//@@@@
// 			}
			
			if (0 != ret)
			{
				CXCounterConnectAndLoginUser::Instance().AddOnlyLoginStepOne_1();//@@@@
				break;
			}
			//
			if ((msg_key_recv.cmd != SINGNALING_CHANNEL_CMD_LOGIN) || (msg_key_recv.result != SINGNALING_CHANNEL_RESULT_OK))
			{
				CXCounterConnectAndLoginUser::Instance().AddOnlyLoginStepOne_1();//@@@@
				break;
			}

			CXCounterConnectAndLoginUser::Instance().AddLoginStepTwo();//@@@@
			m_bExit = TRUE;

			//--------------------------------------------------------------------------------------------------------------------------
			//--------------------------------------------------------------------------------------------------------------------------
			//--------------------------------------------------------------------------------------------------------------------------
		} while (0);


		if (m_pTcpStream)
		{
			m_pTcpStream->Release();
			m_pTcpStream = NULL;
		}
		//
		Sleep(3000);
	}
}

