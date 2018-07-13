#include "stdafx.h"
#include "PhoneDevice.h"
#include "XDes.h"

CPhoneDevice::CPhoneDevice() :m_plMsg(sizeof(MyStruct4PhoneDeviceSession))
{
	m_szBuffer = new unsigned char[MAX_MSG_4_PHONE_DEVICE_SESSION];
	m_hWorkerThread = CROSS_THREAD_NULL;
	m_bLogin = FALSE;
	m_pTcpStream = NULL;
}
CPhoneDevice::~CPhoneDevice(void)
{
	Stop();
	delete[] m_szBuffer;
}
void CPhoneDevice::Start()
{
	if (m_hWorkerThread == CROSS_THREAD_NULL)
	{
		m_bStreamIsError = FALSE;
		m_nSession = CrossGetTickCount64();
		m_bLogin = FALSE;
		m_hEvtWorkerStop = FALSE;
		m_hWorkerThread = CrossCreateThread(WorkerThread, this);
	}
}
void CPhoneDevice::Stop()
{
	if (m_hWorkerThread)
	{
		m_hEvtWorkerStop = TRUE;
		CROSS_TRACE("CPhoneDevice -- Stop -- 2\n");
		//
		CrossWaitThread(m_hWorkerThread);
		CrossCloseThread(m_hWorkerThread);
		m_hWorkerThread = CROSS_THREAD_NULL;
		m_bLogin = FALSE;
	}
	LogoutPlat();
}

void CPhoneDevice::LogoutPlat()
{
	if (m_pTcpStream)
	{
		m_pTcpStream->Release();
		m_pTcpStream = NULL;
	}
	m_csMsg.Lock();
	while (m_msgList.size() > 0)
	{
		MyStruct4PhoneDeviceSession * p = m_msgList.front();
		m_msgList.pop_front();
		m_plMsg.free(p);
	}
	m_csMsg.Unlock();
}

void CPhoneDevice::InputData(int avtype, int isKey, char * buffer, int len)
{
	if (!m_bLogin)
	{
		return;
	}

	if (m_hEvtWorkerStop)
	{
		return;
	}
	m_csMsg.Lock();
	MyStruct4PhoneDeviceSession *p = (MyStruct4PhoneDeviceSession *)m_plMsg.malloc();
	p->head.cmd = XMEDIA_COMMAND_AV_STREAM;
	p->head.result = XMEDIA_RESULT_OK;
	p->head.session = m_nSession;
	p->head.datalen = len + sizeof(ST_XMEDIA_AVHEAD);
	p->head.tick = CrossGetTickCount64();
	//
	ST_XMEDIA_AVHEAD m_stream;
	m_stream.nAVFrameType = (isKey) ? XMEDIA_FREAM_TYPE_H264_IFRAME : XMEDIA_FREAM_TYPE_H264_PFRAME;
	//
	m_stream.nVideoFrameRate = 15;	//帧率
	m_stream.nVideoWidth = 640;
	m_stream.nVideoHeight = 480;
	m_stream.nAudioChannels = 0;
	m_stream.nAudioSamplesRate = 0;
	m_stream.nAudioBitRate = 0;
	m_stream.nAudioBitsPerSample = 0;
	//
	memcpy(p->buffer, &m_stream, sizeof(ST_XMEDIA_AVHEAD));
	memcpy(p->buffer + sizeof(ST_XMEDIA_AVHEAD), buffer, len);
	//
	m_msgList.push_back(p);
	m_csMsg.Unlock();
}








void CPhoneDevice::OnPacketCompleteNetStreamData(int32_t bytesTransferred, emXStreamTansferKey transferKey)
{
	if (NULL == m_pTcpStream)
	{
		return;
	}
	if (m_bStreamIsError)
	{
		return;
	}
	//
	if (bytesTransferred < 0)
	{
		m_bStreamIsError = TRUE;
		return;
	}

	if (transferKey == ENUM_XTSTREAM_TRANSKEY_READ)
	{
		if (0 == bytesTransferred)
		{
			m_nRcvStep = NET_RECIEVE_STEP_HEAD;
			m_pTcpStream->AsyncRead(&m_head, sizeof(m_head));
			return;
		}
		else
		{
			if (m_nRcvStep == NET_RECIEVE_STEP_HEAD)
			{
				m_dataLen = m_head.datalen;
				if (m_dataLen > MAX_MSG_4_PHONE_DEVICE_SESSION)
				{
					m_bStreamIsError = TRUE;
					return;
				}
				if (m_dataLen == 0)
				{
					DoMsg();
					m_nRcvStep = NET_RECIEVE_STEP_HEAD;
					m_pTcpStream->AsyncRead(&m_head, sizeof(m_head));
				}
				else
				{
					m_nRcvStep = NET_RECIEVE_STEP_BODY;
					m_pTcpStream->AsyncRead(m_szBuffer, m_dataLen);
				}
				return;
			}
			if (m_nRcvStep == NET_RECIEVE_STEP_BODY)
			{
				DoMsg();
				m_nRcvStep = NET_RECIEVE_STEP_HEAD;
				m_pTcpStream->AsyncRead(&m_head, sizeof(m_head));
				return;
			}
		}
	}
	if (transferKey == ENUM_XTSTREAM_TRANSKEY_WRITE)
	{

		m_csMsg.Lock();
			if (m_msgList.size() > 0)
			{
			
				m_pTcpStream->AsyncWrite(m_msgList.front(), 
					((MyStruct4PhoneDeviceSession *)m_msgList.front())->head.datalen + sizeof(ST_XMEDIA_HEAD));


				
			}
			else
			{
				m_pTcpStream->PostDelayWriteStatus();//
			}
			m_csMsg.Unlock();
	}
}

void CPhoneDevice::DoMsg()
{
	m_dTickHearbeat = CrossGetTickCount64();
}



int CPhoneDevice::LoginPlat()
{
	CROSS_TRACE("CPhoneDevice -- Login \n");
	MyStruct4PhoneDeviceSession * pSendmsg = new MyStruct4PhoneDeviceSession;
	MyStruct4PhoneDeviceSession *pRecvmsg = new MyStruct4PhoneDeviceSession;
	do
	{
		CROSS_TRACE("CPhoneDevice -- Login 1\n");
		m_pTcpStream = XNetCreateStream4Connect("23.99.107.113", 8002, 3);//23.99.10
		if (0 != XNetConnectStream(m_pTcpStream))
		{
			CROSS_TRACE("CPhoneDevice -- connect error\n");
			break;
		}
		CROSS_TRACE("CPhoneDevice -- Login 2\n");
		memset(&pSendmsg->head, 0, sizeof(pSendmsg));
		pSendmsg->head.result = XMEDIA_RESULT_OK;
		pSendmsg->head.cmd = XMEDIA_COMMAND_LOGIN_NEED_SESSIONID;
		pSendmsg->head.datalen = 0;
		if (0 != m_pTcpStream->SyncWriteAndRead(pSendmsg, sizeof(ST_XMEDIA_HEAD), pRecvmsg, sizeof(ST_XMEDIA_HEAD)))
		{
			CROSS_TRACE("CPhoneDevice -- connect error 1\n");
			break;
		}
		if (XMEDIA_COMMAND_LOGIN_NEED_SESSIONID != pRecvmsg->head.cmd)
		{
			CROSS_TRACE("CPhoneDevice -- connect error 2\n");
			break;
		}
		m_nSession = pRecvmsg->head.session;
		//
		//
		pSendmsg->head.result = XMEDIA_RESULT_OK;
		pSendmsg->head.cmd = XMEDIA_COMMAND_LOGIN;
		pSendmsg->head.datalen = sizeof(ST_XMEDIA_LOGIN_INFO);
		//
		ST_XMEDIA_LOGIN_INFO login;
		memset(&login, 0, sizeof(login));
		//
		XDESEncode("19544", 19544, login.user);
		XDESEncode("19544", 19544, login.user);
		//
		//
		if (0 != m_pTcpStream->SyncWriteAndRead(pSendmsg, pSendmsg->head.datalen + sizeof(ST_XMEDIA_HEAD), pRecvmsg, sizeof(ST_XMEDIA_HEAD)))
		{
			CROSS_TRACE("CPhoneDevice -- connect error 1\n");
			break;
		}
		if (XMEDIA_COMMAND_LOGIN != pRecvmsg->head.cmd)
		{
			CROSS_TRACE("CPhoneDevice -- connect error 2\n");
			break;
		}
		CROSS_TRACE("CPhoneDevice -- connect okokokokokok\n");
		//////////////////////////////////////////////////////////////////////////
		
		m_dTickHearbeat = CrossGetTickCount64();
		m_pTcpStream->SetStreamData(this);
		delete pSendmsg;
		delete pRecvmsg;
		return 0;
		//////////////////////////////////////////////////////////////////////////


	} while (0);

	if (m_pTcpStream)
	{
		m_pTcpStream->Release();
		m_pTcpStream = NULL;
	}

	delete pSendmsg;
	delete pRecvmsg;
	return -1;
}


int CPhoneDevice::WorkerThread(void* param)
{
	CPhoneDevice *pService = (CPhoneDevice*)param;
	pService->Worker();
	return 0;
}
//---------------------------------------------------------------------------------------
void CPhoneDevice::Worker()
{
	//CROSS_TRACE("CPhoneDevice -- Worker 1\n");
	m_bLogin = FALSE;
	while (!m_hEvtWorkerStop)// (::WaitForSingleObject(m_hEvtWorkerStop, 3 * 1000) != WAIT_OBJECT_0)
	{

		//CROSS_TRACE("CPhoneDevice -- Worker 2\n");
		//////////////////////////////////////////////////////////////////////////
		if (!m_bLogin)
		{
			//CROSS_TRACE("CPhoneDevice -- Worker 3\n");
			
			if (0 != LoginPlat())
			{
				//CROSS_TRACE("CPhoneDevice -- Worker 4\n");
				m_bLogin = FALSE;
				if (!m_hEvtWorkerStop)
				{
					
					for (int i = 0; i < 30; i++)
					{
						CrossSleep(100);
						if (m_hEvtWorkerStop)
							break;
					}
					CROSS_TRACE("Login error!\n");
					continue;
				}
			}
			else
			{
				m_bLogin = TRUE;
			}
			//CROSS_TRACE("CPhoneDevice -- Worker 5\n");
		}
		//CROSS_TRACE("CPhoneDevice -- Worker 6\n");

		//////////////////////////////////////////////////////////////////////////
		if ((CrossGetTickCount64() - m_dTickHearbeat) > 20 * 1000)
		{
			m_bLogin = FALSE;
			LogoutPlat();
		}

		//////////////////////////////////////////////////////////////////////////
		CrossSleep(1);

	}


	// #ifdef _PLATFORM_WINDOW
	// 	TRACE("##@@@@@@  CYYJPlayInterface::Worker() Exit!\n");
	// #endif

}
