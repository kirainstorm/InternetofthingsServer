#include "stdafx.h"
#include "MediaDevInterface.h"
#include "MediaDownloadHisFileInterface.h"


#ifdef _PLATFORM_ANDROID
extern JavaVM* g_JavaVM;
extern JNIEnv	*m_pEnvForAudio;
extern jobject g_AudioObj;
#endif



CMediaDevInterface::CMediaDevInterface()
{

	m_pBuffer = new char[MAX_MEDIA_PREVIEW_BUFFER_LEN];
	m_pVideoCallback = new CDecDataCallback();
	memset(&m_pFrameHeadTemp, 0, sizeof(m_pFrameHeadTemp));
	Tidy();
}
CMediaDevInterface::~CMediaDevInterface()
{
	delete[] m_pBuffer;
	delete m_pVideoCallback;
}

void CMediaDevInterface::Tidy()
{
	m_nSeq = 0;
	m_nSessionID = 0;
	//
	m_cbMessage = NULL;
	m_pUser = NULL;
	//
	m_hWorkerThread = CROSS_THREAD_NULL;
	m_bWorkerStop = FALSE;
	//
	m_bLogin = FALSE;
	m_bUserOpenPreview = FALSE;
	//
	m_pTcpStream = NULL;
	//
	m_cbTran = NULL;
	m_pDecVideoCallback = NULL;
	m_pAVUser = NULL;
	//
	m_dec = NULL;
	m_dataLen = 0;
	//
	m_pMp4File = NULL;
	m_bRecodeMp4File = FALSE;
	m_hVoiceWorkerThread = CROSS_THREAD_NULL;
	m_pStreamCallback = NULL;
	m_Voice = NULL;
}


int CMediaDevInterface::XDeviceDelete()
{
	delete this;
	return 0;
}

int CMediaDevInterface::XDeviceSetInfo(DeviceInfo_T t, CXMediaTransparentTransmissionCallback *tran_cb, CXMediaMessageCallback *cb, void * pUser)
{



	m_csLogin.Lock();
	Tidy();
	memcpy(&m_stDevice, &t, sizeof(DeviceInfo_T));
	m_cbTran = tran_cb;
	m_cbMessage = cb;
	m_pUser = pUser;
	m_dwLastTick = CrossGetTickCount64() - 12 * 1000;
	m_csLogin.Unlock();
	return 0;
}
int CMediaDevInterface::XDeviceLogin()
{
	CROSS_TRACE("CMediaDevInterface::XDeviceLogin ------------------------- 1");
	m_csLogin.Lock();
	if (m_hWorkerThread == CROSS_THREAD_NULL)
	{
#ifdef _PLATFORM_WINDOW
		m_bLogin = Login(4);
#endif
		CROSS_TRACE("CMediaDevInterface::XDeviceLogin ------------------------- 2");

		m_bWorkerStop = FALSE;
		m_hWorkerThread = CrossCreateThread(WorkerThread, this);//启动消息收发线程
		CROSS_TRACE("CMediaDevInterface::XDeviceLogin ------------------------- 3");
	}
	m_csLogin.Unlock();
	CROSS_TRACE("CMediaDevInterface::XDeviceLogin ------------------------- 4");
	return 0;
}
int CMediaDevInterface::XDeviceLogout()
{
	m_csLogin.Lock();

	if (m_hWorkerThread)
	{
		//CROSS_TRACE("CMediaDevInterface::XDeviceLogout ------------------------- 1");
		//
		m_bWorkerStop = TRUE;

		//关闭消息收发线程
		CrossWaitThread(m_hWorkerThread);
		CrossCloseThread(m_hWorkerThread);
		m_hWorkerThread = CROSS_THREAD_NULL;
		//CROSS_TRACE("CMediaDevInterface::XDeviceLogout ------------------------- 2");


		//关闭网络
		if (m_pTcpStream)
		{
			//CROSS_TRACE("CMediaDevInterface::XDeviceLogout ------------------------- 2-1");
			//m_pTcpStream->Shutdown();//由于可能处于收发的阻塞状态，所以使用Shutdown使Login()函数退出
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
		//关闭解码
		if (m_dec)
		{
			FFmpegStop(m_dec);
			DestoryFFmpegInstance(m_dec);
			m_dec = NULL;
		}
		//CROSS_TRACE("CMediaDevInterface::XDeviceLogout ------------------------- 4");
		m_csMsglist.Lock();
		//清理内存
		while (m_msgList.size() > 0)
		{
			MyStruct4RMediaPlayInterface * p = m_msgList.front();
			m_msgList.pop_front();
			delete p;
		}
		m_csMsglist.Unlock();

		//CROSS_TRACE("CMediaDevInterface::XDeviceLogout ------------------------- 5");
	}

	Tidy();
	m_csLogin.Unlock();

	return 0;
}
int CMediaDevInterface::XDeviceGetStatus(XMEDIA_DEVICE_STATUS_DEFINE type)
{
	int s = type;
	switch (s)
	{
	case XMEDIA_DEVICE_STATUS_PREVIEW:
	{
		int _live = (int)(CrossGetTickCount64() - m_dwLastTick);
		if (_live > (10 * 1000))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	break;
	default:
		break;
	}

	return 0;
}


#ifdef _PLATFORM_WINDOW
int CMediaDevInterface::XPreviewOpenForWindows(HWND hWnd, CXMediaStreamData* pDataObj, DecodeVideoCallBack* pDecDataObj, void *pUser)
{
	m_bUserOpenPreview = TRUE;
	m_hwnd = hWnd;
	m_pStreamCallback = pDataObj;
	m_pDecVideoCallback = pDecDataObj;
	m_pAVUser = pUser;
	//
	AddSendMessge(Z_CMD_OF_SERVER_OPEN_STREAM, NULL, 0);

	return 0;
}
#endif
#if (defined _PLATFORM_ANDROID) || (defined _PLATFORM_IOS) || (defined _PLATFORM_WINDOW_WIN10_UWP)
int CMediaDevInterface::XPreviewOpenForApp(DecodeVideoCallBack* pDecDataObj, CXMediaStreamData* pVoiceObj , void *pUser)
{
	m_bUserOpenPreview = TRUE;
	m_pStreamCallback = pVoiceObj;
	m_pDecVideoCallback = pDecDataObj;
	m_pAVUser = pUser;
	//
	AddSendMessge(Z_CMD_OF_SERVER_OPEN_STREAM, NULL, 0);
	return 0;
}
#endif
int CMediaDevInterface::XPreviewClose()
{
	m_bUserOpenPreview = FALSE;
	m_pDecVideoCallback = NULL;
	m_pAVUser = NULL;
	//
	AddSendMessge(Z_CMD_OF_SERVER_CLOSE_STREAM, NULL, 0);

	return 0;
}


int CMediaDevInterface::XPreviewSound(int nOpenSound)
{
	if (!nOpenSound)
	{
		if (NULL != m_Voice)
		{
			if (m_hVoiceWorkerThread)
			{
				m_hEvtVoiceWorkerStop = TRUE;
#ifdef _PLATFORM_WINDOW
				m_waveout.Close();
#endif
				//关闭重连线程
				CrossWaitThread(m_hVoiceWorkerThread);
				CrossCloseThread(m_hVoiceWorkerThread);
				m_hVoiceWorkerThread = CROSS_THREAD_NULL;

				while (m_VoiceList.size() > 0)
				{
					m_VoiceList.pop_front();
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
		m_waveout.Open(2048, 1, 8000, 16);
#endif
		if (m_hVoiceWorkerThread == CROSS_THREAD_NULL)
		{
			m_hEvtVoiceWorkerStop = FALSE;
			m_hVoiceWorkerThread = CrossCreateThread(VoiceWorkerThread, this);
		}
	}
	return 0;
}

int CMediaDevInterface::XPreviewCapturePictures(const char* szFilePath, const char* szFileName)
{
	if (m_dec)
	{
		FFmpegCapPic(m_dec, (char *)szFilePath, (char *)szFileName);
	}

	return 0;
}

int CMediaDevInterface::XPreviewRecord(const char* szFilePath, const char *szDevName, BOOL bStart)
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

int CMediaDevInterface::TransparentTransmission(const  char *  szChanParam, int nlen)
{
	AddSendMessge(Z_CMD_OF_SERVER_TRANS_TRANSMISSION, (char *)szChanParam, nlen);
	return 0;
}

CXMediaDownloadInterface* CMediaDevInterface::CreateDownloadHisFileInstance()
{
	CMediaDownloadHisFileInterface * p = new CMediaDownloadHisFileInterface(this);
	return p;
}

void CMediaDevInterface::AddSendMessge(int type, char *buffer, int len)
{
	m_csMsglist.Lock();
	//
	MyStruct4RMediaPlayInterface * pMsg = new MyStruct4RMediaPlayInterface;
	memset(pMsg, 0, sizeof(MyStruct4RMediaPlayInterface));

	pMsg->head.cmd = type;
	pMsg->head.datalen = len;
	pMsg->head.seq = m_nSeq++;
	pMsg->head.session = m_nSessionID;

	if (len > 0)
	{
		memcpy(pMsg->buffer, buffer, len);
	}
	pMsg->nSendLen = sizeof(ST_SERVER_HEAD) + len;

	//
	m_msgList.push_back(pMsg);
	//
	m_csMsglist.Unlock();
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int CMediaDevInterface::WorkerThread(void* param)
{
	CMediaDevInterface * p = (CMediaDevInterface *)param;
	p->Worker();
	return 0;
}
void CMediaDevInterface::Worker()
{

	CROSS_DWORD64 dwHeartTick = CrossGetTickCount64();//心跳

	while (!m_bWorkerStop)
	{

		//-------------------------------------------------------
		//登陆服务器
		if (!m_bLogin)
		{
			m_bLogin = Login();


			//登陆失败，等待一段时间重新登陆
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
		//检测链接的存活

		int _live = (int)(CrossGetTickCount64() - m_dwLastTick);

		if (_live > 10 * 1000)
		{
			CROSS_TRACE(" == %lld  ---  %lld\n", _live, m_dwLastTick);

			//超时
			m_bLogin = FALSE;
			continue;
		}


		//---------------------------------------------------------
		//心跳
		if ((CrossGetTickCount64() - dwHeartTick) > 4 * 1000)
		{
			dwHeartTick = CrossGetTickCount64();
			AddSendMessge(Z_CMD_OF_SERVER_HEARBEAT, NULL, 0);
		}


		//------------------------------------------
		CrossSleep(200);
	}

}


BOOL CMediaDevInterface::Login(int waitsec)
{
	m_csLogin.Lock();

	BOOL bLoginOK = FALSE;
// 	do
// 	{
// 		CROSS_TRACE("CMediaDevInterface::Login ------------------------- 1");
// 		//--------------------------------------------------------------------------------------------------------------------------
// 		//关闭网络
// 		if (m_pTcpStream)
// 		{
// 			m_pTcpStream->Release();
// 			m_pTcpStream = NULL;
// 		}
// 		CROSS_TRACE("CMediaDevInterface::Login ------------------------- 2");
// 		//--------------------------------------------------------------------------------------------------------------------------
// 		//清理消息
// 		m_csMsglist.Lock();
// 		while (m_msgList.size() > 0)
// 		{
// 			MyStruct4RMediaPlayInterface * p = m_msgList.front();
// 			m_msgList.pop_front();
// 			delete p;
// 		}
// 		m_csMsglist.Unlock();
// 		CROSS_TRACE("CMediaDevInterface::Login ------------------------- 3");
// 		//关闭解码
// 		if (m_dec)
// 		{
// 			FFmpegStop(m_dec);
// 			DestoryFFmpegInstance(m_dec);
// 			m_dec = NULL;
// 		}
// 
// 		CROSS_TRACE("CMediaDevInterface::Login ------------------------- 4");
// 		//--------------------------------------------------------------------------------------------------------------------------
// 		//链接服务器
// 		m_pTcpStream = XNetCreateStream4Connect(m_stDevice.core_svr_ip, m_stDevice.core_svr_port, waitsec);
// 		CROSS_TRACE("CMediaDevInterface::Login ------------------------- 4-1");
// 		if (0 != XNetConnectStream(m_pTcpStream))
// 		{
// 			CROSS_TRACE("CMediaDevInterface::Login -- conn svr error");
// 			break;
// 		}
// 		CROSS_TRACE("CMediaDevInterface::Login ------------------------- 5");
// 		//--------------------------------------------------------------------------------------------------------------------------
// 		//发送Z_CMD_OF_SERVER_LOGIN_NEED_SESSIONID
// 		ST_SERVER_HEAD msg_key_send;
// 		ST_SERVER_HEAD msg_key_recv;
// 		memset(&msg_key_send, 0, sizeof(ST_SERVER_HEAD));
// 		memset(&msg_key_recv, 0, sizeof(ST_SERVER_HEAD));
// 		msg_key_send.cmd = Z_CMD_OF_SERVER_LOGIN_NEED_SESSIONID;
// 		//
// 		if (0 != m_pTcpStream->SyncWriteAndRead(&msg_key_send, sizeof(ST_SERVER_HEAD), &msg_key_recv, sizeof(ST_SERVER_HEAD), 10))
// 		{
// 			CROSS_TRACE("CMediaDevInterface::Login -- request key error -- 1");
// 			break;
// 		}
// 		//
// 		if ((msg_key_recv.cmd != Z_CMD_OF_SERVER_LOGIN_NEED_SESSIONID) || (msg_key_recv.result != _CMD_OF_SERVER_RESULT_OK))
// 		{
// 			CROSS_TRACE("CMediaDevInterface::Login -- request key error -- 2");
// 			break;
// 		}
// 
// 		CROSS_TRACE("CMediaDevInterface::Login ------------------------- 6");
// 		//
// 		m_nSessionID = msg_key_recv.session;
// 
// 		//--------------------------------------------------------------------------------------------------------------------------
// 		//发送Z_CMD_OF_SERVER_LOGIN,  使用key加密，登陆
// 		char enc_pwd[256] = { 0 };
// 		XDESEncode(m_stDevice.user_pwd, m_nSessionID, enc_pwd);
// 		//
// 		char szSendBuffer[1024] = { 0 };
// 		ST_SREVER_IOT msg_login_info;
// 		ST_SERVER_HEAD msg_login_send;
// 		ST_SERVER_HEAD msg_login_recv;
// 		memset(&msg_login_info, 0, sizeof(ST_SREVER_IOT));
// 		memset(&msg_login_send, 0, sizeof(ST_SERVER_HEAD));
// 		memset(&msg_login_recv, 0, sizeof(ST_SERVER_HEAD));
// 		//
// 		msg_login_send.cmd = Z_CMD_OF_SERVER_LOGIN;
// 		msg_login_send.seq = m_nSeq++;
// 		msg_login_send.session = m_nSessionID;
// 		msg_login_send.datalen = sizeof(ST_SREVER_IOT);
// 		//
// 		memcpy(msg_login_info.useremail, m_stDevice.user_name, strlen(m_stDevice.user_name));
// 		memcpy(msg_login_info.userdespwd, enc_pwd, strlen(enc_pwd));
// 		//
// 		memcpy(szSendBuffer, &msg_login_send, sizeof(ST_SERVER_HEAD));
// 		memcpy(szSendBuffer + sizeof(ST_SERVER_HEAD), &msg_login_info, sizeof(ST_SREVER_IOT));
// 		//
// 		if (0 != m_pTcpStream->SyncWriteAndRead(szSendBuffer, sizeof(ST_SERVER_HEAD) + sizeof(ST_SREVER_IOT), &msg_login_recv, sizeof(ST_SERVER_HEAD)))
// 		{
// 			CROSS_TRACE("CMediaDevInterface::Login -- login error -- 1");
// 			break;
// 		}
// 		//
// 		if ((msg_login_recv.cmd != Z_CMD_OF_SERVER_LOGIN) || (msg_login_recv.result != _CMD_OF_SERVER_RESULT_OK))
// 		{
// 			CROSS_TRACE("CMediaDevInterface::Login -- login error -- 2");
// 			break;
// 		}
// 		CROSS_TRACE("CMediaDevInterface::Login ------------------------- 7");
// 		m_pTcpStream->SetStreamData(this);
// 		m_dwLastTick = CrossGetTickCount64();
// 		bLoginOK = TRUE;
// 		CROSS_TRACE("CMediaDevInterface::Login ------------------------- 8");
// 		if (m_bUserOpenPreview)
// 		{
// 			AddSendMessge(Z_CMD_OF_SERVER_OPEN_STREAM, NULL, 0);
// 		}
// 
// 	} while (0);
// 
// 	CROSS_TRACE("CMediaDevInterface::Login ------------------------- 9");
// 	if (!bLoginOK)
// 	{
// 		if (m_pTcpStream)
// 		{
// 			m_pTcpStream->Release();
// 			m_pTcpStream = NULL;
// 		}
// 	}
// 
// 	CROSS_TRACE("CMediaDevInterface::Login ------------------------- 10");
	m_csLogin.Unlock();
	//
	return bLoginOK;
}



void CMediaDevInterface::DoMsg()
{

	//CROSS_TRACE("CMediaDevInterface::DoMsg ------------------------- 1");
	//
	m_dwLastTick = CrossGetTickCount64();

	//CROSS_TRACE("m_head.cmd == %d\n", m_head.cmd);
	//
	//-----------------------------------------------------------------


	// 		CROSS_TRACE("-----------\t%04d-%02d-%02d  %02d:%02d:%02d   %03d",
	// 		m_head.wYear, m_head.wMonth, m_head.wDay,
	// 		m_head.wHour, m_head.wMinute, m_head.wSecond,
	// 		m_head.wMilliseconds);



	//CROSS_TRACE("CMediaDevInterface::DoMsg ------------------------- 2");
	if (m_head.cmd == Z_CMD_OF_SERVER_TRANS_TRANSMISSION)
	{
		if (m_cbTran)
		{
			m_cbTran->OnTransparentTransmission(m_head, m_pBuffer, m_dataLen);
		}
		return;
	}
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

		if (m_head.cmd == Z_CMD_OF_SERVER_AV_STREAM)
		{

			if (
				(m_pFrameHeadTemp.nVideoWidth != m_head.nVideoWidth) ||
				(m_pFrameHeadTemp.nVideoHeight != m_head.nVideoHeight) ||
				(m_pFrameHeadTemp.nVideoFrameRate != m_head.nVideoFrameRate) ||
				(m_pFrameHeadTemp.nAudioChannels != m_head.nAudioChannels) ||
				(m_pFrameHeadTemp.nAudioSamplesRate != m_head.nAudioSamplesRate) ||
				(m_pFrameHeadTemp.nAudioBitsPerSample != m_head.nAudioBitsPerSample)
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

				if (((m_head.nAVFrameType == XSERVER_FREAM_TYPE_H264_IFRAME) || (m_head.nAVFrameType == XSERVER_FREAM_TYPE_H264_PFRAME)))
				{
					memcpy(&m_pFrameHeadTemp, &m_head, sizeof(ST_SERVER_HEAD));
					//CROSS_TRACE("CMediaPreviewInterface::data_callback --- rec --- 333 \n");
					// 					m_pMp4File = Mp4File_CreateInstance4WriteH264((const char *)m_szMp4FilePath,
					// 						m_head.nVideoWidth, m_head.nVideoHeight, m_head.nVideoFrameRate,
					// 						m_head.nAudioChannels, m_head.nAudioSamplesRate, m_head.nAudioBitsPerSample, m_head.nAudioBitRate,2048);

					if ((m_head.nAudioChannels <= 0) || (m_head.nAudioSamplesRate <= 0) || (m_head.nAudioBitsPerSample <= 0) || (m_head.nAudioBitRate <= 0))//第三方的数据不全,无音频数据
					{
						m_pMp4File = Mp4File_CreateInstance4WriteH264((const char *)m_szMp4FilePath,
							m_head.nVideoWidth, m_head.nVideoHeight, m_head.nVideoFrameRate,
							1, 8000, 16, 16000, 2048);

					}
					else
					{
						m_pMp4File = Mp4File_CreateInstance4WriteH264((const char *)m_szMp4FilePath,
							m_head.nVideoWidth, m_head.nVideoHeight, m_head.nVideoFrameRate,
							m_head.nAudioChannels, m_head.nAudioSamplesRate, m_head.nAudioBitsPerSample, m_head.nAudioBitRate, 2048);
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
				// 					if ((m_head.nAudioChannels <= 0) || (m_head.nAudioSamplesRate <= 0) || (m_head.nAudioBitsPerSample <= 0) || (m_head.nAudioBitRate <= 0))//第三方的数据不全,无音频数据
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
				if (m_head.nAVFrameType == XSERVER_FREAM_TYPE_AAC)
				{
					//CROSS_TRACE("CMediaPreviewInterface::data_callback --- rec --- aac \n");
					Mp4File_WriteData(m_pMp4File, FALSE, FALSE, m_pBuffer, m_dataLen);
				}

				if (((m_head.nAVFrameType == XSERVER_FREAM_TYPE_H264_IFRAME) || (m_head.nAVFrameType == XSERVER_FREAM_TYPE_H265_IFRAME)))
				{
					//CROSS_TRACE("CMediaPreviewInterface::data_callback --- rec --- i \n");
					Mp4File_WriteData(m_pMp4File, TRUE, TRUE, m_pBuffer, m_dataLen);
				}

				if (((m_head.nAVFrameType == XSERVER_FREAM_TYPE_H264_PFRAME) || (m_head.nAVFrameType == XSERVER_FREAM_TYPE_H265_PFRAME)))
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
	if ((m_pStreamCallback) && (m_head.cmd == Z_CMD_OF_SERVER_AV_STREAM))
	{
		m_pStreamCallback->OnStreamData(m_head, m_pBuffer, m_dataLen);
		return;
	}
#endif

	//-----------------------------------------------------------------



	//CROSS_TRACE("CMediaDevInterface::DoMsg ------------------------- 5");

	//XSERVER_FREAM_TYPE_AAC
	if ((m_head.nAVFrameType == XSERVER_FREAM_TYPE_AAC) && (m_hVoiceWorkerThread))
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
		boost::shared_ptr<CMediaPlayInterfaceVoiceBuffer> sFrame(new CMediaPlayInterfaceVoiceBuffer(m_dataLen));
		CMediaPlayInterfaceVoiceBuffer *p = sFrame.get();
		memcpy(p->m_buffer, m_pBuffer, m_dataLen);
		m_csVoice.Lock();
		m_VoiceList.push_back(sFrame);
		m_csVoice.Unlock();

#endif

		return;
	}
	//CROSS_TRACE("CMediaDevInterface::DoMsg -------------------------6");
	//RSPLAYER_CODEC_VIDEO_TYPE_H264/RSPLAYER_CODEC_VIDEO_TYPE_H265
	int nFrameTypeTemp = RSPLAYER_CODEC_VIDEO_TYPE_H264;
	//
	if (((m_head.nAVFrameType == XSERVER_FREAM_TYPE_H264_IFRAME) || (m_head.nAVFrameType == XSERVER_FREAM_TYPE_H264_PFRAME)) && (NULL == m_dec))
	{
		nFrameTypeTemp = RSPLAYER_CODEC_VIDEO_TYPE_H264;
	}
	//
	if (((m_head.nAVFrameType == XSERVER_FREAM_TYPE_H265_IFRAME) || (m_head.nAVFrameType == XSERVER_FREAM_TYPE_H265_PFRAME)) && (NULL == m_dec))
	{
		nFrameTypeTemp = RSPLAYER_CODEC_VIDEO_TYPE_H265;
	}
	//
	//CROSS_TRACE("CMediaDevInterface::DoMsg ------------------------- 7");
	if ((NULL == m_dec) && (m_head.cmd == Z_CMD_OF_SERVER_AV_STREAM))
	{
		//CROSS_TRACE("CMediaDevInterface::DoMsg ------------------------- 7--1");
		m_dec = CreateFFmpegInstance(nFrameTypeTemp, m_pVideoCallback);
		//CROSS_TRACE("CMediaDevInterface::DoMsg ------------------------- 7--2");
		FFmpegStart(m_dec);
		//CROSS_TRACE("CMediaDevInterface::DoMsg ------------------------- 7--3");
#ifdef _PLATFORM_WINDOW
		m_pVideoCallback->SetPlayHwnd(m_hwnd);
		if (m_pDecVideoCallback)
		{
			m_pVideoCallback->SetUserDecVideoDataCallback(m_pDecVideoCallback, m_pUser);
		}
#endif
#if (defined _PLATFORM_ANDROID) || (defined _PLATFORM_IOS) || (defined _PLATFORM_WINDOW_WIN10_UWP)
		m_pVideoCallback->SetUserDecVideoDataCallback(m_pDecVideoCallback, m_pUser);
#endif
		//CROSS_TRACE("CMediaDevInterface::DoMsg ------------------------- 7--4");
	}
	//-----------------------------------------------------------------
	//CROSS_TRACE("CMediaDevInterface::DoMsg ------------------------- 8");
	//-----------------------------------------------------------------
	if (((m_head.nAVFrameType == XSERVER_FREAM_TYPE_H264_IFRAME) ||
		(m_head.nAVFrameType == XSERVER_FREAM_TYPE_H264_PFRAME) ||
		(m_head.nAVFrameType == XSERVER_FREAM_TYPE_H265_IFRAME) ||
		(m_head.nAVFrameType == XSERVER_FREAM_TYPE_H265_PFRAME)
		) && (m_head.cmd == Z_CMD_OF_SERVER_AV_STREAM))
	{
		//CROSS_TRACE("CMediaPreviewInterface::data_callback --- m_dataLen=%d \n", m_dataLen);
		FFmpegInputData(m_dec, m_pBuffer, m_dataLen, m_head.nVideoFrameRate);
	}
}



// void CMediaDevInterface::DeleteStreamData()
// {
// 	m_pTcpStream = NULL;
// }
void CMediaDevInterface::OnPacketCompleteNetStreamData(int32_t bytesTransferred, emXStreamTansferKey transferKey)
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
				if (m_dataLen > MAX_MEDIA_PREVIEW_BUFFER_LEN)
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
			m_pTcpStream->AsyncWrite((MyStruct4RMediaPlayInterface *)m_msgList.front(), ((MyStruct4RMediaPlayInterface *)m_msgList.front())->nSendLen);//发送
			m_msgList.pop_front();
		}
		else
		{
			m_pTcpStream->PostDelayWriteStatus();
		}
		m_csMsglist.Unlock();
	}
}





int CMediaDevInterface::VoiceWorkerThread(void* param)
{
	CMediaDevInterface *pService = (CMediaDevInterface*)param;

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
void CMediaDevInterface::VoiceWorker()
{

	char * pszBuf = new char[4096];
	int len;

	while (!m_hEvtVoiceWorkerStop)
	{
		if (m_VoiceList.size() <= 0)
		{
			CrossSleep(1);
			continue;
		}
		//
		boost::shared_ptr<CMediaPlayInterfaceVoiceBuffer> sFrame = m_VoiceList.front();
		CMediaPlayInterfaceVoiceBuffer *p = sFrame.get();
		//
		memset(pszBuf, 0, 4096);
		AudioDecData(m_Voice, p->m_buffer, p->m_bufferlen, pszBuf, &len);
		pszBuf[len] = '\0';
		//
#ifdef _PLATFORM_WINDOW
		//播放
		//TRACE("DecodeInputVoiceData - %d\n", len);
		if (m_waveout.inlineis_start())
		{
			m_waveout.input((unsigned char *)pszBuf, len);
		}
#endif

		//CROSS_TRACE("CMediaPreviewInterface::VoiceWorker #### - %d\n", len);

		//
#ifdef _PLATFORM_ANDROID
		//回调出去
		//notify pcm data
		int PCMLEN = 2048;
		jbyteArray jBuff;
		jBuff = m_AudioThreadEnvLocal->NewByteArray(PCMLEN);
		m_AudioThreadEnvLocal->SetByteArrayRegion(jBuff, 0, PCMLEN,
			(jbyte*)pszBuf);
		//回调java，通知视频数据
		m_AudioThreadEnvLocal->CallVoidMethod(g_AudioObj, m_jmidAudioNotify,
			jBuff, PCMLEN);
		m_AudioThreadEnvLocal->DeleteLocalRef(jBuff);

#endif

		m_csVoice.Lock();
		m_VoiceList.pop_front();
		m_csVoice.Unlock();

	}

	delete[] pszBuf;
}