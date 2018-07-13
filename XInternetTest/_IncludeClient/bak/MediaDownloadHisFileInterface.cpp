#include "stdafx.h"
#include "MediaDownloadHisFileInterface.h"
#include "MediaDevInterface.h"
//#include "StreamDataEncryption.h"

#define MAX_MEDIA_DOWNLAOD_BUFFER_LEN (2*1024*1024)
CMediaDownloadHisFileInterface::CMediaDownloadHisFileInterface(void *dev)
{
	m_hDevInterface = dev;
	m_pTcpStream = NULL;
	m_nSessionID = 0;
	m_nSeq = 0;
	m_pDataBuffer = new char[MAX_MEDIA_DOWNLAOD_BUFFER_LEN];
	m_dataLen = 0;


#ifdef _PLATFORM_WINDOW
	pDecoder = NULL;
	pWaveOut = NULL;
	pAudioDecIns = NULL;
	pVideoCallback = NULL;
#endif
}


CMediaDownloadHisFileInterface::~CMediaDownloadHisFileInterface()
{
	delete[] m_pDataBuffer;
}
int CMediaDownloadHisFileInterface::XDownloadHisFileDelete()
{
	delete this;
	return 0;
}


int CMediaDownloadHisFileInterface::XDownloadHisFileListFromMedia(_XSERVER_TIME szBeginTime, _XSERVER_TIME szEndTime, vector<HIS_LOG_T> &v)
{

	m_cs.Lock();


	char * pszBuffer = new char[sizeof(ST_SREVER_HIS_LOG_LIST)];

	int ret = -1;
	do
	{
		//登陆
		if (FALSE == Login())
		{
			break;
		}

		//请求文件列表
		char szSendBuffer[1024] = { 0 };
		ST_SERVER_HEAD msg_send;
		ST_SERVER_HEAD msg_recv;
		ST_SREVER_GET_HIS_LOG msg_send_data;
		memset(&msg_send, 0, sizeof(ST_SERVER_HEAD));
		memset(&msg_recv, 0, sizeof(ST_SERVER_HEAD));
		memset(&msg_send_data, 0, sizeof(ST_SREVER_GET_HIS_LOG));
		//
		msg_send.cmd = Z_CMD_OF_SERVER_GET_STORE_HISTORY_LOG;
		msg_send.session = m_nSessionID;
		msg_send.seq = m_nSeq++;
		msg_send.datalen = sizeof(ST_SREVER_GET_HIS_LOG);
		//
		memcpy(&msg_send_data.tmBeginTime, &szBeginTime, sizeof(_XSERVER_TIME));
		memcpy(&msg_send_data.tmEndTime, &szEndTime, sizeof(_XSERVER_TIME));
		//
		memcpy(szSendBuffer, &msg_send, sizeof(ST_SERVER_HEAD));
		memcpy(szSendBuffer + sizeof(ST_SERVER_HEAD), &msg_send_data, sizeof(ST_SREVER_GET_HIS_LOG));
		//
		if (0 != m_pTcpStream->SyncWriteAndRead(szSendBuffer, sizeof(ST_SERVER_HEAD) + sizeof(ST_SREVER_GET_HIS_LOG), &msg_recv, sizeof(ST_SERVER_HEAD)))
		{
			CROSS_TRACE("XDownloadHislogListFromMedia ---- 1");
			break;
		}

		//
		if ((msg_recv.cmd != Z_CMD_OF_SERVER_GET_STORE_HISTORY_LOG) || (msg_recv.result != _CMD_OF_SERVER_RESULT_OK))
		{
			CROSS_TRACE("XDownloadHislogListFromMedia ---- 2");
			break;
		}

		if (msg_recv.datalen > 0)
		{
			if (0 != m_pTcpStream->SyncWriteAndRead(NULL, 0, pszBuffer, msg_recv.datalen))
			{
				CROSS_TRACE("XDownloadHislogListFromMedia ---- 3");
				break;
			}

			for (unsigned int i = 0; i < (msg_recv.datalen / sizeof(ST_SREVER_HIS_LOG)); i++)
			{
				ST_SREVER_HIS_LOG * pLog = (ST_SREVER_HIS_LOG *)(pszBuffer + i * sizeof(ST_SREVER_HIS_LOG));

				HIS_LOG_T t;
				memset(&t, 0, sizeof(HIS_LOG_T));

				t.log_id = pLog->nHislogID;
				memcpy(t.store_file, pLog->szFile, strlen(pLog->szFile));
				t.store_begin_time_tm = pLog->begin_time_tm;
				t.store_end_time_tm = pLog->end_time_tm;


				//t.store_begin_time_tm = pLog->begin_time_tm;
				//t.store_end_time_tm = pLog->end_time_tm;

				v.push_back(t);
			}
			CROSS_TRACE("XDownloadHislogListFromMedia -- -- --  8\n");
			ret = 0;
		}


	} while (0);

	if (m_pTcpStream)
	{
		m_pTcpStream->Release();
		m_pTcpStream = NULL;
	}

	delete[] pszBuffer;


	m_cs.Unlock();

	return ret;
}
#ifdef _PLATFORM_WINDOW
//
int CMediaDownloadHisFileInterface::XDownloadHisFileFromMediaForWindows(_XSERVER_TIME szBeginTime, _XSERVER_TIME szEndTime, DeviceInfo_T t, HWND hwnd)
{
	//m_pDownloadHisFileCallback = pDownloadObj;
	//m_pUser = this;
	m_hwnd = hwnd;
	return m_DownloadCallback.Open((CMediaDevInterface *)m_hDevInterface, t, szBeginTime, szEndTime);

}
//
int CMediaDownloadHisFileInterface::XDownloadHisFilePlayFrame() //>0表示与下一帧的时间间隔，-1表示失败，0表示无数据 
{
	MyOnlinePlayVideoInfo *pData = m_DownloadCallback.GetNextVideoFrame();
	if (NULL == pData)
	{
		//播放结束?/正在缓冲?
		Sleep(10);
		return 0;
	}



	//-----------------------------------------------------------------------------------------------------------------------------
	DWORD64 dwAudioAndDecvideoStart = GetTickCount64();

	if (pData->nAudioCount > 0)
	{
		if (NULL == pWaveOut)
		{
			pWaveOut = new CWaveOut();
			pWaveOut->Open(2048, 1, 8000, 16);
			pAudioDecIns = CreateAudioDecInstance(RSPLAYER_CODEC_AUDIO_TYPE_AAC);
		}



		for (int i = 0; i < pData->nAudioCount; i++)
		{
			int len = 0;
			char pszBuf[4096] = { 0 };
			AudioDecData(pAudioDecIns, pData->stAudio[i].pAudioBuffer + sizeof(ST_SERVER_HEAD), pData->stAudio[i].nAudioLen, pszBuf, &len);
			pszBuf[len] = '\0';
			//
			if (pWaveOut->inlineis_start())
			{
				pWaveOut->input((unsigned char *)pszBuf, len);
			}

		}
	}

	
	//-----------------------------------------------------------------------------------------------------------------------------
	// 		
	// 
	ST_SERVER_HEAD *pframeHead = (ST_SERVER_HEAD *)pData->pVideoBuffer;

	if ((NULL == pDecoder) && (NULL == pVideoCallback))
	{
		int nFrameTypeTemp = RSPLAYER_CODEC_VIDEO_TYPE_H264;
		//
		if (((pframeHead->nAVFrameType == XSERVER_FREAM_TYPE_H264_IFRAME) || (pframeHead->nAVFrameType == XSERVER_FREAM_TYPE_H264_PFRAME)) && (NULL == pDecoder))
		{
			nFrameTypeTemp = RSPLAYER_CODEC_VIDEO_TYPE_H264;
		}
		//
		if (((pframeHead->nAVFrameType == XSERVER_FREAM_TYPE_H265_IFRAME) || (pframeHead->nAVFrameType == XSERVER_FREAM_TYPE_H265_PFRAME)) && (NULL == pDecoder))
		{
			nFrameTypeTemp = RSPLAYER_CODEC_VIDEO_TYPE_H265;
		}
		//
		pVideoCallback = new CDecDataCallback();

		pDecoder = CreateFFmpegInstance(nFrameTypeTemp, pVideoCallback);
		FFmpegStart(pDecoder);
		pVideoCallback->SetPlayHwnd(m_hwnd);

		
	}

	if ((pframeHead->nAVFrameType == XSERVER_FREAM_TYPE_H264_IFRAME) ||
		(pframeHead->nAVFrameType == XSERVER_FREAM_TYPE_H264_PFRAME) ||
		(pframeHead->nAVFrameType == XSERVER_FREAM_TYPE_H265_IFRAME) ||
		(pframeHead->nAVFrameType == XSERVER_FREAM_TYPE_H265_PFRAME)
		)
	{
		//CROSS_TRACE("CMediaPreviewInterface::data_callback --- 666 \n");
		FFmpegInputData(pDecoder, (char *)pData->pVideoBuffer + sizeof(ST_SERVER_HEAD), pData->nVideoLen, pframeHead->nVideoFrameRate);
	}

	int needSleep = (pData->nSleep - (int)(CrossGetTickCount64() - dwAudioAndDecvideoStart)) / 15 * 15;

	return needSleep;
}
int CMediaDownloadHisFileInterface::XDownloadHisFileSeekSecond(int sec)
{
	return m_DownloadCallback.SeekSecond(sec);
}
//
int CMediaDownloadHisFileInterface::XDownloadHisFileGetPlaySecond()
{
	return m_DownloadCallback.GetPlayFileSecond();
}
int CMediaDownloadHisFileInterface::XDownloadHisFileGetDownloadSecond()
{
	return m_DownloadCallback.GetDownloadFileSecond();
}
int CMediaDownloadHisFileInterface::XDownloadHisFileGetSecond()
{
	return m_DownloadCallback.GetFileSecond();
}
int CMediaDownloadHisFileInterface::XDownloadHisFileWiteMp4File(char * pathfile)
{
	return m_DownloadCallback.WiteMp4File(pathfile);
}
int CMediaDownloadHisFileInterface::XDownloadHisFileGetWiteMp4FileProgress()
{
	return m_DownloadCallback.GetWiteMp4FileProgress();
}
#endif

int CMediaDownloadHisFileInterface::XDownloadHislogFromMedia(_XSERVER_TIME szBeginTime, _XSERVER_TIME szEndTime, CXMediaDownloadCallback* pDownloadObj, void *pUser)
{
	m_cs.Lock();

	m_pDownloadHisFileCallback = pDownloadObj;
	m_pUser = pUser;

	int ret = -1;
	do
	{
		//登陆
		if (FALSE == Login())
		{
			break;
		}


		//请求下载
		char szSendBuffer[1024] = { 0 };
		ST_SERVER_HEAD msg_send;
		ST_SERVER_HEAD msg_recv;
		ST_SREVER_OPEN_HIS_STREAM msg_send_data;
		memset(&msg_send, 0, sizeof(ST_SERVER_HEAD));
		memset(&msg_recv, 0, sizeof(ST_SERVER_HEAD));
		memset(&msg_send_data, 0, sizeof(ST_SREVER_OPEN_HIS_STREAM));
		//
		msg_send.cmd = Z_CMD_OF_SERVER_OPEN_HIS_STREAM;
		msg_send.session = m_nSessionID;
		msg_send.seq = m_nSeq++;
		msg_send.datalen = sizeof(ST_SREVER_OPEN_HIS_STREAM);
		//
		memcpy(&msg_send_data.tmBeginTime, &szBeginTime, sizeof(_XSERVER_TIME));
		memcpy(&msg_send_data.tmEndTime, &szEndTime, sizeof(_XSERVER_TIME));
		//
		memcpy(szSendBuffer, &msg_send, sizeof(ST_SERVER_HEAD));
		memcpy(szSendBuffer + sizeof(ST_SERVER_HEAD), &msg_send_data, sizeof(ST_SREVER_OPEN_HIS_STREAM));


		int n = sizeof(ST_SERVER_HEAD);

		//
		if (0 != m_pTcpStream->SyncWriteAndRead(szSendBuffer, sizeof(ST_SERVER_HEAD) + sizeof(ST_SREVER_OPEN_HIS_STREAM), &msg_recv, sizeof(ST_SERVER_HEAD), 15))
		{
			CROSS_TRACE("XDownloadHislogFromMedia ---- 1");
			break;
		}
		//
		if (msg_recv.cmd != Z_CMD_OF_SERVER_AV_STREAM)
		{
			CROSS_TRACE("XDownloadHislogFromMedia ---- 2");
			break;
		}
		if (msg_recv.result == _CMD_OF_SERVER_RESULT_DOWNLOAD_ERROR) //失败
		{
			CROSS_TRACE("XDownloadHislogFromMedia ---- 2");
			break;
		}

		if (msg_recv.result == _CMD_OF_SERVER_RESULT_DOWNLOAD_START)
		{
			m_pTcpStream->SetStreamData(this);//<<<<<<<<<<<

			ret = 0;//<<<<<<<<<<<<<<<
		}


	} while (0);

	if (0 != ret)
	{
		if (m_pTcpStream)
		{
			m_pTcpStream->Release();
			m_pTcpStream = NULL;
		}
	}



	m_cs.Unlock();


	return ret;

}




int CMediaDownloadHisFileInterface::XDownloadHisFileClose()
{

	m_cs.Unlock();

	if (m_pTcpStream)
	{
		m_pTcpStream->Release();
		m_pTcpStream = NULL;
	}

	m_DownloadCallback.Close();

#ifdef _PLATFORM_WINDOW




	if (pDecoder)
	{
		FFmpegStop(pDecoder);
		DestoryFFmpegInstance(pDecoder);
		pDecoder = NULL;
	}
	if (pVideoCallback)
	{
		delete pVideoCallback;
		pVideoCallback = NULL;
	}
	

	if (pWaveOut)
	{
		pWaveOut->Close();
		delete pWaveOut;
		pWaveOut = NULL;
	}
	if (pAudioDecIns)
	{
		DestoryAudioDecInstance(pAudioDecIns);
		pAudioDecIns = NULL;
	}
#endif


	

	m_cs.Unlock();


	return 0;
}



//--------------------------------------------------------------------------------------------------------------------------------------------------
//从流媒体取数据
//int m_kkkkkkkkkkkkkkkkk = 0;
void CMediaDownloadHisFileInterface::OnPacketCompleteNetStreamData(int32_t bytesTransferred, emXStreamTansferKey transferKey)
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
			m_pTcpStream->AsyncRead(&m_frameHead, sizeof(m_frameHead));
			return;
		}
		else
		{
			if (m_nRcvStepStream == NET_RECIEVE_STEP_HEAD)
			{
				m_dataLen = m_frameHead.datalen;
				if (m_dataLen > MAX_MEDIA_DOWNLAOD_BUFFER_LEN)
				{
					return;
				}
				if (m_dataLen == 0)
				{
					//StreamDataDecryption(m_pDataBuffer + sizeof(m_frameHead));
					data_callback();
					m_nRcvStepStream = NET_RECIEVE_STEP_HEAD;
					m_pTcpStream->AsyncRead(&m_frameHead, sizeof(m_frameHead));
				}
				else
				{
					m_nRcvStepStream = NET_RECIEVE_STEP_BODY;
					m_pTcpStream->AsyncRead(m_pDataBuffer, m_dataLen);
				}
				return;
			}
			if (m_nRcvStepStream == NET_RECIEVE_STEP_BODY)
			{
				//CROSS_TRACE("----------%d------- %d\n", m_kkkkkkkkkkkkkkkkk++, m_dataLen);
				//StreamDataDecryption(m_pDataBuffer + sizeof(m_frameHead));
				data_callback();
				m_nRcvStepStream = NET_RECIEVE_STEP_HEAD;
				m_pTcpStream->AsyncRead(&m_frameHead, sizeof(m_frameHead));
				return;
			}
		}
	}
}

void CMediaDownloadHisFileInterface::data_callback()
{
	if (m_pDownloadHisFileCallback)
	{
		m_pDownloadHisFileCallback->OnDownloadHisFileCallback(m_frameHead, m_pDataBuffer, m_dataLen, m_pUser);
	}
}




//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
BOOL CMediaDownloadHisFileInterface::Login()
{
	BOOL bLoginOK = FALSE;
// 	do
// 	{
// 		//--------------------------------------------------------------------------------------------------------------------------
// 		//关闭网络
// 		if (m_pTcpStream)
// 		{
// 			m_pTcpStream->Release();
// 			m_pTcpStream = NULL;
// 		}
// 
// 		//--------------------------------------------------------------------------------------------------------------------------
// 		//		//清理消息
// 		// 		while (m_msgList.size() > 0)
// 		// 		{
// 		// 			MyStruct4RMediaPlayInterface * p = m_msgList.front();
// 		// 			m_msgList.pop_front();
// 		// 			delete p;
// 		// 		}
// 
// 		//--------------------------------------------------------------------------------------------------------------------------
// 		//链接服务器
// 		m_pTcpStream = XNetCreateStream4Connect(((CMediaDevInterface *)m_hDevInterface)->m_stDevice.core_svr_ip, ((CMediaDevInterface *)m_hDevInterface)->m_stDevice.his_svr_port, 4);
// 		if (0 != XNetConnectStream(m_pTcpStream))
// 		{
// 			CROSS_TRACE("CMediaDevInterface::Login -- conn svr error");
// 			break;
// 		}
// 
// 		//--------------------------------------------------------------------------------------------------------------------------
// 		//发送Z_CMD_OF_SERVER_LOGIN_NEED_SESSIONID
// 		ST_SERVER_HEAD msg_key_send;
// 		ST_SERVER_HEAD msg_key_recv;
// 		memset(&msg_key_send, 0, sizeof(ST_SERVER_HEAD));
// 		memset(&msg_key_recv, 0, sizeof(ST_SERVER_HEAD));
// 		msg_key_send.cmd = Z_CMD_OF_SERVER_LOGIN_NEED_SESSIONID;
// 		//
// 		if (0 != m_pTcpStream->SyncWriteAndRead(&msg_key_send, sizeof(ST_SERVER_HEAD), &msg_key_recv, sizeof(ST_SERVER_HEAD)))
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
// 		//
// 		m_nSessionID = msg_key_recv.session;
// 
// 		//--------------------------------------------------------------------------------------------------------------------------
// 		//发送Z_CMD_OF_SERVER_LOGIN,  使用key加密，登陆
// 		char enc_pwd[256] = { 0 };
// 		XDESEncode(((CMediaDevInterface *)m_hDevInterface)->m_stDevice.user_pwd, m_nSessionID, enc_pwd);
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
// 		memcpy(msg_login_info.useremail, ((CMediaDevInterface *)m_hDevInterface)->m_stDevice.user_name, strlen(((CMediaDevInterface *)m_hDevInterface)->m_stDevice.user_name));
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
// 
// 		bLoginOK = TRUE;
// 
// 
// 	} while (0);
// 
// 
// 	if (!bLoginOK)
// 	{
// 		if (m_pTcpStream)
// 		{
// 			m_pTcpStream->Release();
// 			m_pTcpStream = NULL;
// 		}
// 	}

	return bLoginOK;
}