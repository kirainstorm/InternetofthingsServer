#include "stdafx.h"
#include "GetUserRoutes.h"

CGetUserRoutes::CGetUserRoutes()
{
}
CGetUserRoutes::~CGetUserRoutes()
{
}
void CGetUserRoutes::Get(char * email)
{
	printf("----------------------------------------------------------------------\n");
	//srand(time(NULL));
	//int _index = rand() % 90000 + m_index;
	//
	//
	BOOL bSuccess = FALSE;
	ST_REDIRECTION_SEND_BUFFER *pSendBuffer = (ST_REDIRECTION_SEND_BUFFER *)CMiddleBufferPool::Instance().malloc();
	login_redirection_t _login;
	signaling_channel_head_t msg_recv;
	char szBuffer[256] = {};
	CXNetStream* pTcpStream = NULL;
	//
	//
	do
	{
		pTcpStream = XNetCreateStream4Connect(SERVER_IP, 6599, 5);
		if (NULL == pTcpStream)
		{
			printf(">>>CGetUserRoutes::Get Error<<< 1  \n");
			break;
		}
		if (0 != XNetConnectStream(pTcpStream))
		{
			printf(">>>CGetUserRoutes::Get Error<<< 2  \n");
			break;
		}
		//
		memset(pSendBuffer, 0, sizeof(ST_REDIRECTION_SEND_BUFFER));
		memset(&_login, 0, sizeof(_login));
		//
		pSendBuffer->head.cmd = SINGNALING_CHANNEL_CMD_REDIRECTION_USER;
		pSendBuffer->head.datalen = sizeof(_login);
		char szttmp[32] = { 0 };
		CROSS_STRCPY(_login.id, email);
		memcpy(pSendBuffer->msg, &_login, sizeof(_login));
		//
		if (0 != pTcpStream->SyncWriteAndRead(pSendBuffer, sizeof(signaling_channel_head_t) + pSendBuffer->head.datalen, &msg_recv, sizeof(signaling_channel_head_t),5))
		{
			printf(">>>CGetUserRoutes::Get Error<<< 3  \n");
			break;
		}
		if (msg_recv.cmd != SINGNALING_CHANNEL_CMD_REDIRECTION_USER)
		{
			printf(">>>CGetUserRoutes::Get Error<<< 4  \n");
			break;
		}
		if (msg_recv.datalen == 0)
		{
			printf(">>>CGetUserRoutes::Get Error<<< 5  \n");
			break;
		}
		if (0 != pTcpStream->SyncWriteAndRead(NULL, 0, szBuffer, msg_recv.datalen))
		{
			printf(">>>CGetUserRoutes::Get Error<<< 6  \n");
			break;
		}
		//
		printf(szBuffer);
		printf("\n");
		printf(">>>CGetUserRoutes::Get okokok!  \n");
		//
		bSuccess = TRUE;
		//
	} while (FALSE);
	//
	if (pTcpStream)
	{
		pTcpStream->Release();
		pTcpStream = NULL;
	}
	CMiddleBufferPool::Instance().free((char *)pSendBuffer);
	printf("----------------------------------------------------------------------\n");
}