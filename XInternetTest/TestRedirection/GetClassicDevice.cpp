#include "stdafx.h"
#include "GetClassicDevice.h"


CGetClassicDevice::CGetClassicDevice()
{
}
CGetClassicDevice::~CGetClassicDevice()
{
}
void CGetClassicDevice::Get(char * user , char * pwd)
{
	printf("----------------------------------------------------------------------\n");
	CXNetStream* m_pTcpStream = NULL;
	ST_REDIRECTION_SEND_BUFFER *pSendBuffer = (ST_REDIRECTION_SEND_BUFFER *)CMiddleBufferPool::Instance().malloc();
	char * pdstBuffer = CMiddleBufferPool::Instance().malloc();
	signaling_channel_head_t msg_key_recv;

	do
	{
		//--------------------------------------------------------------------------------------------------------------------------
		m_pTcpStream = XNetCreateStream4Connect(SERVER_IP, 6499, 3);
		if (0 != XNetConnectStream(m_pTcpStream))
		{
			printf(">>>CGetClassicDevice::Get Error<<< 1  \n");
			break;
		}
		//--------------------------------------------------------------------------------------------------------------------------
		//NEED_SESSIONID
		memset(pSendBuffer, 0, sizeof(ST_REDIRECTION_SEND_BUFFER));
		pSendBuffer->head.cmd = SINGNALING_CHANNEL_CMD_LOGIN_NEED_SESSION;
		if (0 != m_pTcpStream->SyncWriteAndRead(pSendBuffer, sizeof(signaling_channel_head_t), &msg_key_recv, sizeof(signaling_channel_head_t), 5))
		{
			printf(">>>CGetClassicDevice::Get Error<<< 2  \n");
			break;
		}
		//
		if ((msg_key_recv.cmd != SINGNALING_CHANNEL_CMD_LOGIN_NEED_SESSION) || (msg_key_recv.result != SINGNALING_CHANNEL_RESULT_OK))
		{
			printf(">>>CGetClassicDevice::Get Error<<< 3  \n");
			break;
		}
		//
		uint32_t m_nSessionID = msg_key_recv.session;
		//--------------------------------------------------------------------------------------------------------------------------
		//Ê¹ÓÃkey¼ÓÃÜ£¬µÇÂ½
		signaling_channel_user_login_t stLogin;
		memset(&stLogin, 0, sizeof(stLogin));
		XDESEncode(user, m_nSessionID, stLogin.userdes);
		XDESEncode(pwd, m_nSessionID, stLogin.pwddes);
		//
		memset(pSendBuffer, 0, sizeof(ST_REDIRECTION_SEND_BUFFER));
		pSendBuffer->head.cmd = SINGNALING_CHANNEL_CMD_LOGIN;
		pSendBuffer->head.session = m_nSessionID;
		pSendBuffer->head.datalen = sizeof(stLogin);
		//
		memcpy(pSendBuffer->msg, &stLogin, sizeof(stLogin));
		//
		if (0 != m_pTcpStream->SyncWriteAndRead(pSendBuffer, sizeof(signaling_channel_head_t) + sizeof(stLogin), &msg_key_recv, sizeof(signaling_channel_head_t)))
		{
			printf(">>>CGetClassicDevice::Get Error<<< 4  \n");
			break;
		}
		if ((msg_key_recv.cmd != SINGNALING_CHANNEL_CMD_LOGIN) || (msg_key_recv.result != SINGNALING_CHANNEL_RESULT_OK))
		{
			printf(">>>CGetClassicDevice::Get Error<<< 5  \n");
			break;
		}
		//--------------------------------------------------------------------------------------------------------------------------
		pSendBuffer->head.cmd = SINGNALING_CHANNEL_CMD_REDIRECTION_CLASSIC_GET_DEVICES;
		pSendBuffer->head.datalen = 0;
		if (0 != m_pTcpStream->SyncWriteAndRead(pSendBuffer, sizeof(signaling_channel_head_t), &msg_key_recv, sizeof(signaling_channel_head_t), 5))
		{
			printf(">>>CGetClassicDevice::Get Error<<< 6  \n");
			break;
		}
		if ((msg_key_recv.cmd != SINGNALING_CHANNEL_CMD_REDIRECTION_CLASSIC_GET_DEVICES) || (msg_key_recv.result != SINGNALING_CHANNEL_RESULT_OK))
		{
			//CROSS_TRACE("CMediaDevInterface::Login -- request key error -- 2");
			break;
		}
		//
		if (msg_key_recv.datalen > 0)
		{
			if (0 != m_pTcpStream->SyncWriteAndRead(NULL, 0, pdstBuffer, msg_key_recv.datalen, 5))
			{
				printf(">>>CGetClassicDevice::Get Error<<< 7  \n");
				break;
			}
		}
		//
		for (int i = 0; i < (int)(msg_key_recv.datalen / sizeof(ST_CLASSIC_DEVICE_INFO_BASE)); i++)
		{
			ST_CLASSIC_DEVICE_INFO_BASE st;
			memcpy(&st, pdstBuffer + i * sizeof(ST_CLASSIC_DEVICE_INFO_BASE), sizeof(ST_CLASSIC_DEVICE_INFO_BASE));
			printf(st.dev_name);
			printf("\n");
		}
		printf(">>>CGetClassicDevice::Get okokok!  \n");
		//
	} while (0);

	if (m_pTcpStream)
	{
		m_pTcpStream->Release();
		m_pTcpStream = NULL;
	}
	CMiddleBufferPool::Instance().free((char *)pSendBuffer);
	CMiddleBufferPool::Instance().free((char *)pdstBuffer);
	printf("----------------------------------------------------------------------\n");
}