#include "stdafx.h"
#include "XClientStream.h"


#ifdef _PLATFORM_ANDROID
extern JavaVM* g_JavaVM;
extern JNIEnv	*m_pEnvForAudio;
extern jobject g_AudioObj;
#endif



CXClientStream::CXClientStream(emXClientType type, const char * uuid, int channel, const char * ip, int port, const char * user, const char * pwd)
{
	m_pBuffer = CMiddleBufferPool::Instance().malloc();
	m_pVideoCallback = new CDecDataCallback();
	//
	m_type = type;
	if (type == X_CLIENT_XXX_TYPE_CLASSIC)
	{
		m_classicid = atoi(uuid);
	}
	else
	{
		CROSS_STRCPY(m_szIotuuid, uuid);
	}
	//
	CROSS_STRCPY(m_szIP, ip);
	CROSS_STRCPY(m_szUser, user);
	CROSS_STRCPY(m_szPwd, pwd);
	m_channel = channel;
	m_port = port;
}
CXClientStream::~CXClientStream()
{
	CMiddleBufferPool::Instance().free(m_pBuffer);
	delete m_pVideoCallback;
}

void CXClientStream::Tidy()
{
	memset(&m_pFrameHeadTemp, 0, sizeof(m_pFrameHeadTemp));
	memset(m_szIP, 0, sizeof(m_szIP));
	memset(m_szUser, 0, sizeof(m_szUser));
	memset(m_szPwd, 0, sizeof(m_szPwd));
	memset(m_szIotuuid, 0, sizeof(m_szIotuuid));
	m_pRawdataCallback = NULL;
	m_pDecodeVideoCallBack = NULL;
	m_nSeq = 0;
	m_nSessionID = 0;
	m_hWorkerThread = CROSS_THREAD_NULL;
	m_bWorkerStop = FALSE;
	m_bLogin = FALSE;
	m_pTcpStream = NULL;
	m_pAVUser = NULL;
	m_dec = NULL;
	m_dataLen = 0;
	m_pMp4File = NULL;
	m_bRecodeMp4File = FALSE;
	m_hVoiceWorkerThread = CROSS_THREAD_NULL;
	m_Voice = NULL;
	m_emClientNetStatus = CLIENT_NET_STATUS_NONE;
}



void CXClientStream::XDelete()
{



	XClientStreamClose();
	delete this;
}
// int CXClientStream::XClientStreamSetInfo(const char * ip, int port, const char * user, const char * pwd, int classicid, int classicchannel/*0*/, const char * iotuuid)
// {
// 	Tidy();
// 	//
// 
// 	//
// 	return 0;
// }
emClientNetStatus CXClientStream::XClientStreamGetStatus()
{
	return m_emClientNetStatus;
}
int CXClientStream::XClientStreamSetRawdataCallback(CXClientStreamRawdataCallback* pCallback, void *pUser)
{
	m_pRawdataCallback = pCallback;
	m_pRawdataCallbackUser = pUser;
	return 0;
}
int CXClientStream::XClientStreamSetDecodeVideoCallBack(DecodeVideoCallBack* pCallback, void *pUser)
{
	m_pDecodeVideoCallBack = pCallback;
	m_pDecodeVideoCallBackUser = pUser;
	return 0;
}
//
#ifdef _PLATFORM_WINDOW
int CXClientStream::XClientStreamPlay(HWND hWnd)
{
	m_emClientNetStatus = CLIENT_NET_STATUS_CONNECTING;
	m_hwnd = hWnd;
	//CROSS_TRACE("CMediaDevInterface::XDeviceLogin ------------------------- 1");
	if (m_hWorkerThread == CROSS_THREAD_NULL)
	{

#ifdef _PLATFORM_WINDOW
		m_bLogin = Login(4);
#endif
		//CROSS_TRACE("CMediaDevInterface::XDeviceLogin ------------------------- 2");
		m_bWorkerStop = FALSE;
		m_hWorkerThread = CrossCreateThread(WorkerThread, this);//����Ϣ�շ��߳�
		//CROSS_TRACE("CMediaDevInterface::XDeviceLogin ------------------------- 3");
		//

	}
	//CROSS_TRACE("CMediaDevInterface::XDeviceLogin ------------------------- 4");

	return 0;
}
#else
int CXClientStream::XClientStreamPlay()
{
	if (m_hWorkerThread == CROSS_THREAD_NULL)
	{
		m_bWorkerStop = FALSE;
		m_hWorkerThread = CrossCreateThread(WorkerThread, this);//����Ϣ�շ��߳�
	}
	return 0;
}
#endif
int CXClientStream::XClientStreamClose()
{
	if (m_hWorkerThread)
	{
		//CROSS_TRACE("CMediaDevInterface::XDeviceLogout ------------------------- 1");
		//
		m_bWorkerStop = TRUE;

		//�ر���Ϣ�շ��߳�
		CrossWaitThread(m_hWorkerThread);
		CrossCloseThread(m_hWorkerThread);
		m_hWorkerThread = CROSS_THREAD_NULL;
		//CROSS_TRACE("CMediaDevInterface::XDeviceLogout ------------------------- 2");


		//�ر�����
		if (m_pTcpStream)
		{
			//CROSS_TRACE("CMediaDevInterface::XDeviceLogout ------------------------- 2-1");
			//m_pTcpStream->Shutdown();//���ڿ��ܴ����շ�������״̬������ʹ��ShutdownʹLogin()�����˳�
			m_pTcpStream->Release();
			m_pTcpStream = NULL;
			// 			while (m_pTcpStream)
			// 			{
			// 				CrossSleep(1);
			// 			}
			// 			do
			// 			{
			// 				CrossSleep(30);
			// 			} while (m_pTcpStream);
		}


		//CROSS_TRACE("CMediaDevInterface::XDeviceLogout ------------------------- 3");
		//�رս���
		if (m_dec)
		{
			FFmpegStop(m_dec);
			DestoryFFmpegInstance(m_dec);
			m_dec = NULL;
		}
		//CROSS_TRACE("CMediaDevInterface::XDeviceLogout ------------------------- 4");
		m_csMsglist.Lock();
		//�����ڴ�
		while (m_msgList.size() > 0)
		{
			ST_STREAM_BUFFER * p = m_msgList.front();
			m_msgList.pop_front();
			CStreamBufferPool::Instance().free(p);
		}
		m_csMsglist.Unlock();


		while (m_VoiceBufferList.size() > 0)
		{
			ST_STREAM_BUFFER * p = m_VoiceBufferList.front();
			m_VoiceBufferList.pop_front();
			CStreamBufferPool::Instance().free(p);
		}


		//CROSS_TRACE("CMediaDevInterface::XDeviceLogout ------------------------- 5");
	}

	Tidy();

	return 0;
}
int CXClientStream::XClientStreamSound(BOOL bOpenSound)
{
	if (!bOpenSound)
	{
		if (NULL != m_Voice)
		{
			if (m_hVoiceWorkerThread)
			{
				m_hEvtVoiceWorkerStop = TRUE;
#ifdef _PLATFORM_WINDOW
//				m_waveout.Close();
#endif
				//�ر������߳�
				CrossWaitThread(m_hVoiceWorkerThread);
				CrossCloseThread(m_hVoiceWorkerThread);
				m_hVoiceWorkerThread = CROSS_THREAD_NULL;

				while (m_VoiceBufferList.size() > 0)
				{
					ST_STREAM_BUFFER * p = m_VoiceBufferList.front();
					m_VoiceBufferList.pop_front();
					CStreamBufferPool::Instance().free(p);
				}
			}

			DestoryAudioDecInstance(m_Voice);
			m_Voice = NULL;
		}
	}
	else
	{
		m_Voice = CreateAudioDecInstance(RSPLAYER_CODEC_AUDIO_TYPE_AAC);
#ifdef _PLATFORM_WINDOW
		//m_waveout.Open(2048, 1, 8000, 16);
#endif
		if (m_hVoiceWorkerThread == CROSS_THREAD_NULL)
		{
			m_hEvtVoiceWorkerStop = FALSE;
			m_hVoiceWorkerThread = CrossCreateThread(VoiceWorkerThread, this);
		}
	}
	return 0;
}
int CXClientStream::XClientStreamCapturePictures(const char* szFilePath, const char* szFileName)
{
	if (m_dec)
	{
		FFmpegCapPic(m_dec, (char *)szFilePath, (char *)szFileName);
	}

	return 0;
}
int CXClientStream::XClientStreamRecord(const char* szFilePath, const char *szDevName, BOOL bStart)
{
	m_csRecord.Lock();

	m_bRecodeMp4File = bStart;

	if (!m_bRecodeMp4File)
	{
		if (m_pMp4File)
		{
			Mp4File_DestoryInstance4Write(m_pMp4File);
			m_pMp4File = NULL;
		}
	}
	else
	{
		memset(m_szMp4FilePath, 0, sizeof(m_szMp4FilePath));
		char szTmp[256] = { 0 };

#if (defined _PLATFORM_WINDOW) || (defined _PLATFORM_WINDOW_WIN10_UWP)
		CROSS_SPRINTF(szTmp, "%s\\Mp4Files\\%s", szFilePath, szDevName);
#endif
#if (defined _PLATFORM_ANDROID) || (defined _PLATFORM_IOS)
		CROSS_SPRINTF(szTmp, "%s/Mp4Files/%s", szFilePath, szDevName);

#endif


		memcpy(m_szMp4FilePath, szTmp, strlen(szTmp));
	}
	m_csRecord.Unlock();
	return 0;
}






void CXClientStream::AddSendMessge(int type, char *buffer, int len)
{
	m_csMsglist.Lock();
	//
	ST_STREAM_BUFFER * pMsg = CStreamBufferPool::Instance().malloc();


	pMsg->head.cmd = type;
	pMsg->head.datalen = len;
	//pMsg->head.seq = m_nSeq++;
	pMsg->head.session = m_nSessionID;

	if (len > 0)
	{
		memcpy(pMsg->msg, buffer, len);
	}
	//pMsg->nSendLen = sizeof(ST_XMEDIA_HEAD) + len;

	//
	m_msgList.push_back(pMsg);
	//
	m_csMsglist.Unlock();
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int CXClientStream::WorkerThread(void* param)
{
	CXClientStream * p = (CXClientStream *)param;
	p->Worker();
	return 0;
}
void CXClientStream::Worker()
{
	//m_emClientNetStatus = CLIENT_NET_STATUS_CONNECTING;
	CROSS_DWORD64 dwHeartTick = CrossGetTickCount64();//����

	while (!m_bWorkerStop)
	{

		//-------------------------------------------------------
		//��½������
		if (!m_bLogin)
		{
			m_bLogin = Login();


			//��½ʧ�ܣ��ȴ�һ��ʱ�����µ�½
			if (!m_bLogin)
			{
				for (int i = 0; i < 100; i++)
				{
					CrossSleep(50);
					if (m_bWorkerStop)
					{
						break;
					}
				}
			}
			else
			{
				dwHeartTick = CrossGetTickCount64();
			}
			//
			continue;
		}

		// 		CrossSleep(3000);
		// 		m_bLogin = FALSE;
		// 		continue;



		//---------------------------------------------------------
		//������ӵĴ��

		int _live = (int)(CrossGetTickCount64() - m_dwLastTick);

		if (_live > 15 * 1000)
		{
			//CROSS_TRACE(" == %lld  ---  %lld\n", _live, m_dwLastTick);

			//��ʱ
			m_bLogin = FALSE;
			m_emClientNetStatus = CLIENT_NET_STATUS_CONNECTING;
			continue;
		}


		//---------------------------------------------------------
		//����
		if ((CrossGetTickCount64() - dwHeartTick) > 4 * 1000)
		{
			dwHeartTick = CrossGetTickCount64();
			AddSendMessge(XMEDIA_COMMAND_HEARTBEAT, NULL, 0);
		}


		//------------------------------------------
		CrossSleep(200);
	}

}


BOOL CXClientStream::Login(int waitsec)
{
	//m_csLogin.Lock();

	BOOL bLoginOK = FALSE;
	do
	{
		//CROSS_TRACE("CMediaDevInterface::Login ------------------------- 1");
		//--------------------------------------------------------------------------------------------------------------------------
		//�ر�����
		if (m_pTcpStream)
		{
			m_pTcpStream->Release();
			m_pTcpStream = NULL;
		}
		//CROSS_TRACE("CMediaDevInterface::Login ------------------------- 2");
		//--------------------------------------------------------------------------------------------------------------------------
		//������Ϣ
		m_csMsglist.Lock();
		while (m_msgList.size() > 0)
		{
			ST_STREAM_BUFFER * p = m_msgList.front();
			m_msgList.pop_front();
			CStreamBufferPool::Instance().free(p);
		}
		m_csMsglist.Unlock();
		//CROSS_TRACE("CMediaDevInterface::Login ------------------------- 3");
		//�رս���
		if (m_dec)
		{
			FFmpegStop(m_dec);
			DestoryFFmpegInstance(m_dec);
			m_dec = NULL;
		}

		//CROSS_TRACE("CMediaDevInterface::Login ------------------------- 4");
		//--------------------------------------------------------------------------------------------------------------------------
		//���ӷ�����
		m_pTcpStream = XNetCreateStream4Connect(m_szIP, m_port, waitsec);
		//CROSS_TRACE("CMediaDevInterface::Login ------------------------- 4-1");
		if (0 != XNetConnectStream(m_pTcpStream))
		{
			CROSS_TRACE("CMediaDevInterface::Login -- conn svr error");
			break;
		}
		//CROSS_TRACE("CMediaDevInterface::Login ------------------------- 5");
		//--------------------------------------------------------------------------------------------------------------------------
		//����Z_CMD_OF_SERVER_LOGIN_NEED_SESSIONID
		ST_XMEDIA_HEAD msg_key_send;
		ST_XMEDIA_HEAD msg_key_recv;
		memset(&msg_key_send, 0, sizeof(ST_XMEDIA_HEAD));
		memset(&msg_key_recv, 0, sizeof(ST_XMEDIA_HEAD));
		msg_key_send.cmd = XMEDIA_COMMAND_LOGIN_NEED_SESSIONID;
		//
		if (0 != m_pTcpStream->SyncWriteAndRead(&msg_key_send, sizeof(ST_XMEDIA_HEAD), &msg_key_recv, sizeof(ST_XMEDIA_HEAD), 10))
		{
			CROSS_TRACE("CMediaDevInterface::Login -- request key error -- 1");
			break;
		}
		//
		if ((msg_key_recv.cmd != XMEDIA_COMMAND_LOGIN_NEED_SESSIONID) || (msg_key_recv.result != XMEDIA_RESULT_OK))
		{
			CROSS_TRACE("CMediaDevInterface::Login -- request key error -- 2");
			break;
		}

		//CROSS_TRACE("CMediaDevInterface::Login ------------------------- 6");
		//
		m_nSessionID = msg_key_recv.session;

		//--------------------------------------------------------------------------------------------------------------------------
		//����Z_CMD_OF_SERVER_LOGIN,  ʹ��key���ܣ���½
		char enc_pwd[256] = { 0 };
		XDESEncode(m_szPwd, m_nSessionID, enc_pwd);

		//
		char szSendBuffer[1024] = { 0 };
		ST_XMEDIA_LOGIN_INFO msg_login_info;
		ST_XMEDIA_HEAD msg_login_send;
		ST_XMEDIA_HEAD msg_login_recv;
		memset(&msg_login_info, 0, sizeof(ST_XMEDIA_LOGIN_INFO));
		memset(&msg_login_send, 0, sizeof(ST_XMEDIA_HEAD));
		memset(&msg_login_recv, 0, sizeof(ST_XMEDIA_HEAD));
		//
		msg_login_send.cmd = XMEDIA_COMMAND_LOGIN;
		//msg_login_send.seq = m_nSeq++;
		msg_login_send.session = m_nSessionID;
		msg_login_send.datalen = sizeof(ST_XMEDIA_LOGIN_INFO);
		//
		//msg_login_info.devid = m_classicid;
		
		memcpy(msg_login_info.user, m_szUser, strlen(m_szUser));
		memcpy(msg_login_info.despwd, enc_pwd, strlen(enc_pwd));
		//
		memcpy(szSendBuffer, &msg_login_send, sizeof(ST_XMEDIA_HEAD));
		memcpy(szSendBuffer + sizeof(ST_XMEDIA_HEAD), &msg_login_info, sizeof(ST_XMEDIA_LOGIN_INFO));
		//
		if (0 != m_pTcpStream->SyncWriteAndRead(szSendBuffer, sizeof(ST_XMEDIA_HEAD) + sizeof(ST_XMEDIA_LOGIN_INFO), &msg_login_recv, sizeof(ST_XMEDIA_HEAD)))
		{
			CROSS_TRACE("CMediaDevInterface::Login -- login error -- 1");
			break;
		}
		//
		if ((msg_login_recv.cmd != XMEDIA_COMMAND_LOGIN) || (msg_login_recv.result != XMEDIA_RESULT_OK))
		{
			CROSS_TRACE("CMediaDevInterface::Login -- login error -- 2");
			break;
		}
		//CROSS_TRACE("CMediaDevInterface::Login ------------------------- 7");
		m_pTcpStream->SetStreamData(this);
		m_dwLastTick = CrossGetTickCount64();
		bLoginOK = TRUE;
		m_emClientNetStatus = CLIENT_NET_STATUS_CONNECTED;
		CROSS_TRACE("CMediaDevInterface::Login ------------------------- okokokokok");
		//if (m_bUserOpenPreview)
		{

			ST_SREVER_OPEN_STREAM ssst;
			CROSS_SPRINTF(ssst.devid, "%d", m_classicid);
			AddSendMessge(XMEDIA_COMMAND_OPEN_STREAM, (char *)&ssst, sizeof(ssst));
		}

	} while (0);

	//CROSS_TRACE("CMediaDevInterface::Login ------------------------- 9");
	if (!bLoginOK)
	{
		if (m_pTcpStream)
		{
			m_pTcpStream->Release();
			m_pTcpStream = NULL;
		}
	}

	//CROSS_TRACE("CMediaDevInterface::Login ------------------------- 10");


	return bLoginOK;
}



void CXClientStream::DoMsg()
{
	m_dwLastTick = CrossGetTickCount64();
	if (m_head.cmd != XMEDIA_COMMAND_AV_STREAM)
	{
		return;
	}

	CROSS_TRACE("CXClientStream::DoMsg ------------------------- 1");
	//

	ST_XMEDIA_AVHEAD avheader;
	memcpy(&avheader, m_pBuffer, sizeof(avheader));

	//CROSS_TRACE("m_head.cmd == %d\n", m_head.cmd);
	//
	//-----------------------------------------------------------------


	// 		CROSS_TRACE("-----------\t%04d-%02d-%02d  %02d:%02d:%02d   %03d",
	// 		m_head.wYear, m_head.wMonth, m_head.wDay,
	// 		m_head.wHour, m_head.wMinute, m_head.wSecond,
	// 		m_head.wMilliseconds);



	//CROSS_TRACE("CMediaDevInterface::DoMsg ------------------------- 3");
	//---------------------------------------------------------------- -
	m_csRecord.Lock();
	if (m_bRecodeMp4File)
	{
		//CROSS_TRACE("CMediaPreviewInterface::data_callback --- rec --- 111 , # %d - %d # %d - %d # %d - %d # %d - %d # %d - %d # %d - %d  \n",
		//	m_pFrameHeadTemp->nVideoWidth, m_frameHead.nVideoWidth,
		//	m_pFrameHeadTemp->nVideoHeight, m_frameHead.nVideoHeight,
		//	m_pFrameHeadTemp->nVideoFrameRate,m_frameHead.nVideoFrameRate,
		//	m_pFrameHeadTemp->nAudioChannels,m_frameHead.nAudioChannels,
		//	m_pFrameHeadTemp->nAudioSamplesRate, m_frameHead.nAudioSamplesRate,
		//	m_pFrameHeadTemp->nAudioBitsPerSample, m_frameHead.nAudioBitsPerSample
		//	);

		if (m_head.cmd == XMEDIA_COMMAND_AV_STREAM)
		{


			if (
				(m_pFrameHeadTemp.nVideoWidth != avheader.nVideoWidth) ||
				(m_pFrameHeadTemp.nVideoHeight != avheader.nVideoHeight) ||
				(m_pFrameHeadTemp.nVideoFrameRate != avheader.nVideoFrameRate) ||
				(m_pFrameHeadTemp.nAudioChannels != avheader.nAudioChannels) ||
				(m_pFrameHeadTemp.nAudioSamplesRate != avheader.nAudioSamplesRate) ||
				(m_pFrameHeadTemp.nAudioBitsPerSample != avheader.nAudioBitsPerSample)
				)
			{
				if (NULL != m_pMp4File)
				{
					//CROSS_TRACE("CMediaPreviewInterface::data_callback --- rec --- 222 \n");
					Mp4File_DestoryInstance4Write(m_pMp4File);
					m_pMp4File = NULL;
				}
			}


			if (NULL == m_pMp4File)
			{

				if (((avheader.nAVFrameType == XMEDIA_FREAM_TYPE_H264_IFRAME) || (avheader.nAVFrameType == XMEDIA_FREAM_TYPE_H265_IFRAME)))
				{
					memcpy(&m_pFrameHeadTemp, m_pBuffer, sizeof(m_pFrameHeadTemp));


					//CROSS_TRACE("CMediaPreviewInterface::data_callback --- rec --- 333 \n");
					// 					m_pMp4File = Mp4File_CreateInstance4WriteH264((const char *)m_szMp4FilePath,
					// 						m_head.nVideoWidth, m_head.nVideoHeight, m_head.nVideoFrameRate,
					// 						m_head.nAudioChannels, m_head.nAudioSamplesRate, m_head.nAudioBitsPerSample, m_head.nAudioBitRate,2048);

					if ((avheader.nAudioChannels <= 0) || (avheader.nAudioSamplesRate <= 0) || (avheader.nAudioBitsPerSample <= 0) || (avheader.nAudioBitRate <= 0))//�����������ݲ�ȫ,����Ƶ����
					{
						m_pMp4File = Mp4File_CreateInstance4WriteH264((const char *)m_szMp4FilePath,
							avheader.nVideoWidth, avheader.nVideoHeight, avheader.nVideoFrameRate,
							1, 8000, 16, 16000, 2048);

					}
					else
					{
						m_pMp4File = Mp4File_CreateInstance4WriteH264((const char *)m_szMp4FilePath,
							avheader.nVideoWidth, avheader.nVideoHeight, avheader.nVideoFrameRate,
							avheader.nAudioChannels, avheader.nAudioSamplesRate, avheader.nAudioBitsPerSample, avheader.nAudioBitRate, 2048);
					}


					Mp4File_WriteData(m_pMp4File, TRUE, TRUE, m_pBuffer, m_dataLen);
				}
				//
				// 				if (((m_head.nAVFrameType == XSERVER_FREAM_TYPE_H265_IFRAME) || (m_head.nAVFrameType == XSERVER_FREAM_TYPE_H265_PFRAME)))
				// 				{
				// 					memcpy(&m_pFrameHeadTemp, &m_head, sizeof(ST_SERVER_HEAD));
				// 					//CROSS_TRACE("CMediaPreviewInterface::data_callback --- rec --- 444 \n");
				// // 					m_pMp4File = Mp4File_CreateInstance4WriteH265((const char *)m_szMp4FilePath,
				// // 						m_head.nVideoWidth, m_head.nVideoHeight, m_head.nVideoFrameRate,
				// // 						m_head.nAudioChannels, m_head.nAudioSamplesRate, m_head.nAudioBitsPerSample, m_head.nAudioBitRate, 2048);
				// 
				// 
				// 					if ((m_head.nAudioChannels <= 0) || (m_head.nAudioSamplesRate <= 0) || (m_head.nAudioBitsPerSample <= 0) || (m_head.nAudioBitRate <= 0))//�����������ݲ�ȫ,����Ƶ����
				// 					{
				// 						m_pMp4File = Mp4File_CreateInstance4WriteH265((const char *)m_szMp4FilePath,
				// 							m_head.nVideoWidth, m_head.nVideoHeight, m_head.nVideoFrameRate,
				// 							1, 8000, 16, 16000, 2048);
				// 
				// 					}
				// 					else
				// 					{
				// 						m_pMp4File = Mp4File_CreateInstance4WriteH265((const char *)m_szMp4FilePath,
				// 							m_head.nVideoWidth, m_head.nVideoHeight, m_head.nVideoFrameRate,
				// 							m_head.nAudioChannels, m_head.nAudioSamplesRate, m_head.nAudioBitsPerSample, m_head.nAudioBitRate, 2048);
				// 					}
				// 
				// 
				// 					Mp4File_WriteData(m_pMp4File, TRUE, TRUE, m_pBuffer, m_dataLen);
				// 				}
			}
			else
			{
				//CROSS_TRACE("CMediaPreviewInterface::data_callback --- rec --- 555 \n");
				if (avheader.nAVFrameType == XMEDIA_FREAM_TYPE_AAC)
				{
					//CROSS_TRACE("CMediaPreviewInterface::data_callback --- rec --- aac \n");
					Mp4File_WriteData(m_pMp4File, FALSE, FALSE, m_pBuffer, m_dataLen);
				}

				if (((avheader.nAVFrameType == XMEDIA_FREAM_TYPE_H264_IFRAME) || (avheader.nAVFrameType == XMEDIA_FREAM_TYPE_H265_IFRAME)))
				{
					//CROSS_TRACE("CMediaPreviewInterface::data_callback --- rec --- i \n");
					Mp4File_WriteData(m_pMp4File, TRUE, TRUE, m_pBuffer, m_dataLen);
				}

				if (((avheader.nAVFrameType == XMEDIA_FREAM_TYPE_H264_PFRAME) || (avheader.nAVFrameType == XMEDIA_FREAM_TYPE_H265_PFRAME)))
				{
					//CROSS_TRACE("CMediaPreviewInterface::data_callback --- rec --- p \n");
					Mp4File_WriteData(m_pMp4File, TRUE, FALSE, m_pBuffer, m_dataLen);
				}
			}
		}
	}
	m_csRecord.Unlock();
	//-----------------------------------------------------------------




	//CROSS_TRACE("CMediaDevInterface::DoMsg ------------------------- 4");

	//m_pStreamCallback
	//-----------------------------------------------------------------

#ifdef _PLATFORM_WINDOW
	//only for x86/x64
	if ((m_pRawdataCallback) && (m_head.cmd == XMEDIA_COMMAND_AV_STREAM))
	{
		m_pRawdataCallback->OnRawdata(m_head, m_pBuffer + sizeof(ST_XMEDIA_HEAD), m_dataLen - sizeof(ST_XMEDIA_HEAD));
	}
#endif

	//-----------------------------------------------------------------



	//CROSS_TRACE("CMediaDevInterface::DoMsg ------------------------- 5");

	//XSERVER_FREAM_TYPE_AAC
	if ((avheader.nAVFrameType == XMEDIA_FREAM_TYPE_AAC) && (m_hVoiceWorkerThread))
	{
		//CROSS_TRACE("CMediaDevInterface::DoMsg ------------------------- 5---1");
		//TRACE("pPacketData.FrameType == emFrameType_AFrame %d\n", m_dataLen);

#ifdef _PLATFORM_IOS
		//only for ios
		if (m_pStreamCallback)
		{
			m_pStreamCallback->OnStreamData(m_head, m_pBuffer, m_dataLen);
		}
#else
		//boost::shared_ptr<CMediaPlayInterfaceVoiceBuffer> sFrame(new CMediaPlayInterfaceVoiceBuffer(m_dataLen));
		//CMediaPlayInterfaceVoiceBuffer *p = sFrame.get();

		ST_STREAM_BUFFER * pMsg = CStreamBufferPool::Instance().malloc();

		pMsg->head.datalen = m_dataLen - sizeof(ST_XMEDIA_AVHEAD);
		memcpy(pMsg->msg, m_pBuffer + sizeof(ST_XMEDIA_AVHEAD), pMsg->head.datalen);
		m_csVoice.Lock();
		m_VoiceBufferList.push_back(pMsg);
		m_csVoice.Unlock();

#endif

		return;
	}
	//CROSS_TRACE("CMediaDevInterface::DoMsg -------------------------6");
	//RSPLAYER_CODEC_VIDEO_TYPE_H264/RSPLAYER_CODEC_VIDEO_TYPE_H265
	int nFrameTypeTemp = RSPLAYER_CODEC_VIDEO_TYPE_H264;
	//
	if (((avheader.nAVFrameType == XMEDIA_FREAM_TYPE_H264_IFRAME) || (avheader.nAVFrameType == XMEDIA_FREAM_TYPE_H264_PFRAME)) && (NULL == m_dec))
	{
		nFrameTypeTemp = RSPLAYER_CODEC_VIDEO_TYPE_H264;
	}
	//
	if (((avheader.nAVFrameType == XMEDIA_FREAM_TYPE_H265_IFRAME) || (avheader.nAVFrameType == XMEDIA_FREAM_TYPE_H265_PFRAME)) && (NULL == m_dec))
	{
		nFrameTypeTemp = RSPLAYER_CODEC_VIDEO_TYPE_H265;
	}
	//
	//CROSS_TRACE("CMediaDevInterface::DoMsg ------------------------- 7");
	if ((NULL == m_dec) && (m_head.cmd == XMEDIA_COMMAND_AV_STREAM))
	{
		//CROSS_TRACE("CMediaDevInterface::DoMsg ------------------------- 7--1");
		m_dec = CreateFFmpegInstance(nFrameTypeTemp, m_pVideoCallback);
		//CROSS_TRACE("CMediaDevInterface::DoMsg ------------------------- 7--2");
		FFmpegStart(m_dec);
		//CROSS_TRACE("CMediaDevInterface::DoMsg ------------------------- 7--3");
#ifdef _PLATFORM_WINDOW
		m_pVideoCallback->SetPlayHwnd(m_hwnd);
		if (m_pDecodeVideoCallBack)
		{
			m_pVideoCallback->SetUserDecVideoDataCallback(m_pDecodeVideoCallBack, m_pDecodeVideoCallBackUser);
		}
#endif
#if (defined _PLATFORM_ANDROID) || (defined _PLATFORM_IOS) || (defined _PLATFORM_WINDOW_WIN10_UWP)
if (m_pDecodeVideoCallBack)
		m_pVideoCallback->SetUserDecVideoDataCallback(m_pDecodeVideoCallBack, m_pDecodeVideoCallBackUser);
#endif
		//CROSS_TRACE("CMediaDevInterface::DoMsg ------------------------- 7--4");
	}
	//-----------------------------------------------------------------
	//CROSS_TRACE("CMediaDevInterface::DoMsg ------------------------- 8");
	//-----------------------------------------------------------------
	if (((avheader.nAVFrameType == XMEDIA_FREAM_TYPE_H264_IFRAME) ||
		(avheader.nAVFrameType == XMEDIA_FREAM_TYPE_H264_PFRAME) ||
		(avheader.nAVFrameType == XMEDIA_FREAM_TYPE_H265_IFRAME) ||
		(avheader.nAVFrameType == XMEDIA_FREAM_TYPE_H265_PFRAME)
		) && (m_head.cmd == XMEDIA_COMMAND_AV_STREAM))
	{
		//CROSS_TRACE("CMediaPreviewInterface::data_callback --- m_dataLen=%d \n", m_dataLen);
		FFmpegInputData(m_dec, m_pBuffer, m_dataLen, avheader.nVideoFrameRate);
	}
}



// void CMediaDevInterface::DeleteStreamData()
// {
// 	m_pTcpStream = NULL;
// }
void CXClientStream::OnPacketCompleteNetStreamData(int32_t bytesTransferred, emXStreamTansferKey transferKey)
{
	if (bytesTransferred < 0)
	{
		return;
	}

	if (ENUM_XTSTREAM_TRANSKEY_READ == transferKey)
	{
		if (0 == bytesTransferred)
		{
			m_nRcvStepStream = NET_RECIEVE_STEP_HEAD;
			m_pTcpStream->AsyncRead(&m_head, sizeof(m_head));
			return;
		}
		else
		{
			if (m_nRcvStepStream == NET_RECIEVE_STEP_HEAD)
			{
				m_dataLen = m_head.datalen;
				if (m_dataLen > MIDDLE_BUFFER_SIZE_MB)
				{
					return;
				}
				if (m_dataLen == 0)
				{
					//StreamDataDecryption(m_pBuffer);
					DoMsg();
					m_nRcvStepStream = NET_RECIEVE_STEP_HEAD;
					m_pTcpStream->AsyncRead(&m_head, sizeof(m_head));
				}
				else
				{
					m_nRcvStepStream = NET_RECIEVE_STEP_BODY;
					m_pTcpStream->AsyncRead(m_pBuffer, m_dataLen);
				}
				return;
			}
			if (m_nRcvStepStream == NET_RECIEVE_STEP_BODY)
			{
				//CROSS_TRACE("----------%d------- %d\n", m_kkkkkkkkkkkkkkkkk++, m_dataLen);
				//StreamDataDecryption(m_pBuffer);
				DoMsg();
				m_nRcvStepStream = NET_RECIEVE_STEP_HEAD;
				m_pTcpStream->AsyncRead(&m_head, sizeof(m_head));
				return;
			}
		}
	}
	if (ENUM_XTSTREAM_TRANSKEY_WRITE == transferKey)
	{
		m_csMsglist.Lock();
		if (m_msgList.size() > 0)
		{
			ST_STREAM_BUFFER * pMsg = m_msgList.front();
			m_pTcpStream->AsyncWrite(pMsg, pMsg->head.datalen + sizeof(ST_XMEDIA_HEAD));//����
			m_msgList.pop_front();
			CStreamBufferPool::Instance().free(pMsg);
		}
		else
		{
			m_pTcpStream->PostDelayWriteStatus();
		}
		m_csMsglist.Unlock();
	}
}





int CXClientStream::VoiceWorkerThread(void* param)
{
	CXClientStream *pService = (CXClientStream*)param;

#ifdef _PLATFORM_ANDROID
	JNIEnv *env;
	jclass jclsProcess;
	jmethodID jmidProcess;
	bool isAttached = false;
	int status;
	status = g_JavaVM->GetEnv((void **)&env, JNI_VERSION_1_4);
	if (status < 0) {
		status = g_JavaVM->AttachCurrentThread(&env, NULL);
		if (status < 0) {
			Log("CMediaDevInterface::WorkerThread: AttachCurrentThread Failed!!");
			return 0;
		}
		isAttached = true;
	}

	jclsProcess = env->GetObjectClass(g_AudioObj);
	if (jclsProcess == NULL) {
		Log("CMediaDevInterface::WorkerThread: m_envLocalCmd->GetObjectClass(g_AudioObj) error\n");
		return 0;
	}
	jmidProcess = env->GetMethodID(jclsProcess, "AudioData", "([BI)V");
	if (jmidProcess == NULL) {
		Log("m_envLocalCmd->GetMethodID(clazz, \"AudioData\", error\n");
		return 0;
	}
	pService->m_jmidAudioNotify = jmidProcess;
	pService->m_AudioThreadEnvLocal = env;
#endif

	pService->VoiceWorker();//<<<--------------------------------------

#ifdef _PLATFORM_ANDROID
	if (isAttached)
		g_JavaVM->DetachCurrentThread();
#endif

	return 0;
}
void CXClientStream::VoiceWorker()
{

	char * pszBuf = new char[4096];
	int len;

	while (!m_hEvtVoiceWorkerStop)
	{
		if (m_VoiceBufferList.size() <= 0)
		{
			CrossSleep(1);
			continue;
		}
		//
		//boost::shared_ptr<CMediaPlayInterfaceVoiceBuffer> sFrame = m_VoiceList.front();
		ST_STREAM_BUFFER *p = m_VoiceBufferList.front();
		//
		memset(pszBuf, 0, 4096);
		AudioDecData(m_Voice, (const char *)p->msg, p->head.datalen, pszBuf, &len);
		pszBuf[len] = '\0';
		//
#ifdef _PLATFORM_WINDOW
		//����
		//TRACE("DecodeInputVoiceData - %d\n", len);
// 		if (m_waveout.inlineis_start())
// 		{
// 			m_waveout.input((unsigned char *)pszBuf, len);
// 		}
#endif

		//CROSS_TRACE("CMediaPreviewInterface::VoiceWorker #### - %d\n", len);

		//
#ifdef _PLATFORM_ANDROID
		//�ص���ȥ
		//notify pcm data
		int PCMLEN = 2048;
		jbyteArray jBuff;
		jBuff = m_AudioThreadEnvLocal->NewByteArray(PCMLEN);
		m_AudioThreadEnvLocal->SetByteArrayRegion(jBuff, 0, PCMLEN,
			(jbyte*)pszBuf);
		//�ص�java��֪ͨ��Ƶ����
		m_AudioThreadEnvLocal->CallVoidMethod(g_AudioObj, m_jmidAudioNotify,
			jBuff, PCMLEN);
		m_AudioThreadEnvLocal->DeleteLocalRef(jBuff);

#endif

		m_csVoice.Lock();
		m_VoiceBufferList.pop_front();
		CStreamBufferPool::Instance().free(p);
		m_csVoice.Unlock();

	}

	delete[] pszBuf;
}