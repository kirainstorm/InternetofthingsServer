#include "stdafx.h"
#include "XIOTClientStream.h"
// 
// CXDeviceStream::CXDeviceStream()
// {
// 	m_pTcpStream = NULL;
// 	m_bIsError = TRUE;
// 	m_pBuffer = CLittleBufferPool::Instance().malloc();
// }
// 
// CXDeviceStream::~CXDeviceStream()
// {
// 	CLittleBufferPool::Instance().free(m_pBuffer);
// }
// void CXDeviceStream::OnPacketCompleteNetStreamData(int32_t bytesTransferred, emXStreamTansferKey transferKey)
// {
// 	if (m_bIsError)
// 	{
// 		return;
// 	}
// 	if (bytesTransferred < 0)
// 	{
// 		m_bIsError = TRUE;
// 		return;
// 	}
// 
// 
// 	if (ENUM_XTSTREAM_TRANSKEY_READ == transferKey)
// 	{
// 		if (0 == bytesTransferred)
// 		{
// 			m_nRecvStep = NET_RECIEVE_STEP_HEAD;
// 			m_pTcpStream->AsyncRead(&m_head, sizeof(m_head));
// 			return;
// 		}
// 		else
// 		{
// 			if (m_nRecvStep == NET_RECIEVE_STEP_HEAD)
// 			{
// 				if (m_head.datalen > MIDDLE_BUFFER_SIZE_MB)
// 				{
// 					return;
// 				}
// 				if (m_head.datalen == 0)
// 				{
// 					//StreamDataDecryption(m_pBuffer);
// 					DoMsg();
// 					m_nRecvStep = NET_RECIEVE_STEP_HEAD;
// 					m_pTcpStream->AsyncRead(&m_head, sizeof(m_head));
// 				}
// 				else
// 				{
// 					m_nRecvStep = NET_RECIEVE_STEP_BODY;
// 					m_pTcpStream->AsyncRead(m_pBuffer, m_head.datalen);
// 				}
// 				return;
// 			}
// 			if (m_nRecvStep == NET_RECIEVE_STEP_BODY)
// 			{
// 				//StreamDataDecryption(m_pBuffer);
// 				DoMsg();
// 				m_nRecvStep = NET_RECIEVE_STEP_HEAD;
// 				m_pTcpStream->AsyncRead(&m_head, sizeof(m_head));
// 				return;
// 			}
// 		}
// 	}
// 	if (ENUM_XTSTREAM_TRANSKEY_WRITE == transferKey)
// 	{
// // 		if (m_msgSendList.size() > 0)
// // 		{
// // 			ST_BUFFER_SEND_TO_CLIENT_SESSION_LITTLE * pMsg = (ST_BUFFER_SEND_TO_CLIENT_SESSION_LITTLE *)m_msgSendList.front();
// // 			if (pMsg->head.datalen > 0)
// // 			{
// // 				int iiii = 0;
// // 			}
// // 			m_pTcpStream->AsyncWrite(pMsg, pMsg->head.datalen + sizeof(pMsg->head));//发送
// // 			m_msgSendList.pop_front();
// // 			CMediaAvDataInstanceBufferPool::Instance().free(pMsg);
// // 		}
// // 		else
// // 		{
// // 			m_pTcpStream->PostDelayWriteStatus();
// // 		}
// 	}
// }
// 
// void CXDeviceStream::Connect(char * s_ip, int s_port, char * uuid, char * user)
// {
// 	if (NULL == m_pTcpStream)
// 	{
// 		m_bIsError = FALSE;
// 		m_dwLastTick = CrossGetTickCount64();
// 
// // 
// // 		//--------------------------------------------------------------------------------------------------------------------------
// // 		//关闭网络
// // 		if (m_pTcpStream)
// // 		{
// // 			m_pTcpStream->Release();
// // 			m_pTcpStream = NULL;
// // 		}
// // 		//--------------------------------------------------------------------------------------------------------------------------
// // 		//清理消息
// // 		while (m_msgSendList.size() > 0)
// // 		{
// // 			ST_BUFFER_SEND_TO_CLIENT_SESSION_LITTLE * p = m_msgSendList.front();
// // 			m_msgSendList.pop_front();
// // 			CMediaAvDataInstanceBufferPool::Instance().free(p);
// // 		}
// // 		//--------------------------------------------------------------------------------------------------------------------------
// // 		//链接服务器
// // 		m_pTcpStream = XNetCreateStream4Connect(svr_ip.c_str(), svr_port, 4);
// // 		if (0 != XNetConnectStream(m_pTcpStream))
// // 		{
// // 			//CROSS_TRACE("CMediaDevInterface::Login -- conn svr error");
// // 			break;
// // 		}
// // 		//--------------------------------------------------------------------------------------------------------------------------
// // 		//发送Z_CMD_OF_SERVER_LOGIN_NEED_SESSIONID
// // 		ST_SERVER_HEAD msg_key_send;
// // 		ST_SERVER_HEAD msg_key_recv;
// // 		memset(&msg_key_send, 0, sizeof(ST_SERVER_HEAD));
// // 		memset(&msg_key_recv, 0, sizeof(ST_SERVER_HEAD));
// // 		msg_key_send.cmd = Z_CMD_OF_SERVER_LOGIN_NEED_SESSIONID;
// // 		//
// // 		if (0 != m_pTcpStream->SyncWriteAndRead(&msg_key_send, sizeof(ST_SERVER_HEAD), &msg_key_recv, sizeof(ST_SERVER_HEAD), 10))
// // 		{
// // 			//CROSS_TRACE("CMediaDevInterface::Login -- request key error -- 1");
// // 			break;
// // 		}
// // 		//
// // 		if ((msg_key_recv.cmd != Z_CMD_OF_SERVER_LOGIN_NEED_SESSIONID) || (msg_key_recv.result != _CMD_OF_SERVER_RESULT_OK))
// // 		{
// // 			//CROSS_TRACE("CMediaDevInterface::Login -- request key error -- 2");
// // 			break;
// // 		}
// // 		//
// // 		m_nSessionID = msg_key_recv.session;
// // 
// // 		//--------------------------------------------------------------------------------------------------------------------------
// // 		//发送Z_CMD_OF_SERVER_LOGIN,  使用key加密，登陆
// // 		char enc_pwd[256] = { 0 };
// // 		XDESEncode(user_pwd.c_str(), m_nSessionID, enc_pwd);
// // 		//
// // 		char szSendBuffer[1024] = { 0 };
// // 		ST_SREVER_LOGIN_INFO msg_login_info;
// // 		ST_SERVER_HEAD msg_login_send;
// // 		ST_SERVER_HEAD msg_login_recv;
// // 		memset(&msg_login_info, 0, sizeof(ST_SREVER_LOGIN_INFO));
// // 		memset(&msg_login_send, 0, sizeof(ST_SERVER_HEAD));
// // 		memset(&msg_login_recv, 0, sizeof(ST_SERVER_HEAD));
// // 		//
// // 		msg_login_send.cmd = Z_CMD_OF_SERVER_LOGIN;
// // 		msg_login_send.seq = m_nSeq++;
// // 		msg_login_send.session = m_nSessionID;
// // 		msg_login_send.datalen = sizeof(ST_SREVER_LOGIN_INFO);
// // 		//
// // 		msg_login_info.dev_id = classic_dev_id;
// // 		msg_login_info.dev_channel = 0;
// // 		memcpy(msg_login_info.user, user_name.c_str(), user_name.length());
// // 		memcpy(msg_login_info.pwd, enc_pwd, strlen(enc_pwd));
// // 		//
// // 		memcpy(szSendBuffer, &msg_login_send, sizeof(ST_SERVER_HEAD));
// // 		memcpy(szSendBuffer + sizeof(ST_SERVER_HEAD), &msg_login_info, sizeof(ST_SREVER_LOGIN_INFO));
// // 		//
// // 		if (0 != m_pTcpStream->SyncWriteAndRead(szSendBuffer, sizeof(ST_SERVER_HEAD) + sizeof(ST_SREVER_LOGIN_INFO), &msg_login_recv, sizeof(ST_SERVER_HEAD)))
// // 		{
// // 			//CROSS_TRACE("CMediaDevInterface::Login -- login error -- 1");
// // 			break;
// // 		}
// // 		//
// // 		if ((msg_login_recv.cmd != Z_CMD_OF_SERVER_LOGIN) || (msg_login_recv.result != _CMD_OF_SERVER_RESULT_OK))
// // 		{
// // 			//CROSS_TRACE("CMediaDevInterface::Login -- login error -- 2");
// // 			break;
// // 		}
// // 		//CROSS_TRACE("CMediaDevInterface::Login ------------------------- 6");
// // 		m_pTcpStream->SetStreamData(this);
// // 		//		m_dwLastTick = CrossGetTickCount64();
// // 		bLoginOK = TRUE;
// // 		//CROSS_TRACE("CMediaDevInterface::Login ------------------------- 7");
// // 		//if (m_bUserOpenPreview)
// // 		{
// // 			AddSendMessge(Z_CMD_OF_SERVER_OPEN_STREAM, NULL, 0);
// // 		}
// // 
// // 	} while (0);
// // 
// // 	//CROSS_TRACE("CMediaDevInterface::Login ------------------------- 8");
// // 	if (!bLoginOK)
// // 	{
// // 		if (m_pTcpStream)
// // 		{
// // 			m_pTcpStream->Release();
// // 			m_pTcpStream = NULL;
// // 		}
// // 	}
// // 
// // 	return bLoginOK;
// 	}
// }
// void CXDeviceStream::Disconnect()
// {
// 	if (m_pTcpStream)
// 	{
// 		m_pTcpStream->Release();
// 		m_pTcpStream = NULL;
// 
// 
// 		m_bIsError = TRUE;
// 	}
// }
// BOOL CXDeviceStream::IsError()
// {
// 	if ((CrossGetTickCount64() - m_dwLastTick) > 15000)
// 	{
// 		m_bIsError = TRUE;
// 	}
// 
// 	return m_bIsError;
// }
// void CXDeviceStream::AddSendStream(char *data, int len)
// {
// 
// }
// 
// void CXDeviceStream::DoMsg()
// {
// 	m_dwLastTick = CrossGetTickCount64();
// };