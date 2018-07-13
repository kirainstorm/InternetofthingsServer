#include "stdafx.h"
#include "XClientHislog.h"
#include "XDes.h"



#define MAX_MEDIA_DOWNLAOD_BUFFER_LEN (2*1024*1024)
CXClientHislog::CXClientHislog(emXClientType type, const char * uuid, int channel, const char * ip, int port, const char * user, const char * pwd)
{
	memset(m_id, 0, sizeof(m_id));
	memset(m_szIP, 0, sizeof(m_szIP));
	memset(m_szUser, 0, sizeof(m_szUser));
	memset(m_szPwd, 0, sizeof(m_szPwd));
	CROSS_STRCPY(m_id, uuid);
	CROSS_STRCPY(m_szIP, ip);
	CROSS_STRCPY(m_szUser, user);
	CROSS_STRCPY(m_szPwd, pwd);
	m_port = port;
	m_channel = channel;


//	m_hDevInterface = dev;
	m_pTcpStream = NULL;
	m_nSessionID = 0;
//	m_nSeq = 0;
	m_pDataBuffer = new char[MAX_MEDIA_DOWNLAOD_BUFFER_LEN];
	m_dataLen = 0;


#ifdef _PLATFORM_WINDOW
	pDecoder = NULL;
	//pWaveOut = NULL;
	pAudioDecIns = NULL;
	pVideoCallback = NULL;
#endif


	m_pDownFileBuffer = NULL;
//	m_pDownInterface = NULL;
	m_pFileIndexArray = NULL;
	m_bNeedFirstIFrame = TRUE;
	m_pFrame = NULL;
	m_nWiteMp4FileProgress = 0;
	m_pFrame = new MyOnlinePlayVideoInfo;
	memset(m_pFrame, 0, sizeof(MyOnlinePlayVideoInfo));
	m_pFrame->pVideoBuffer = new char[2 * 1024 * 1024];

	for (int i = 0; i < MAX_ONE_VIDEO_AUDIO_COUNT; i++)
	{
		m_pFrame->pAudioBuffer[i] = new char[4096];
	}

	m_pFileIndexArray = new int[MAX_FILE_INDEX_COUNT];
	memset(m_pFileIndexArray, 0, MAX_FILE_INDEX_COUNT);
	m_pDownFileBuffer = new char[MAX_FILE_BUFFER_LEN];
}


CXClientHislog::~CXClientHislog()
{
	for (int i = 0; i < MAX_ONE_VIDEO_AUDIO_COUNT; i++)
	{
		delete[] m_pFrame->pAudioBuffer[i];
	}
	delete[] m_pFrame->pVideoBuffer;
	delete m_pFrame;
	if (m_pDownFileBuffer)
	{
		delete[] m_pDownFileBuffer;
		m_pDownFileBuffer = NULL;
	}

	if (m_pFileIndexArray)
	{
		delete[] m_pFileIndexArray;
		m_pFileIndexArray = NULL;
	}
	delete[] m_pDataBuffer;
}
int CXClientHislog::XClientDownloadInterfaceDelete()
{
	delete this;
	return 0;
}
// int CXClientHislog::XClientDownloadInterfaceSetInfo(const char * ip, int port, const char * user, const char * pwd, int id, int channel/*0*/)
// {
// 
// 	m_id = id;
// 	m_channel = channel;
// 	return 0;
// }

int CXClientHislog::XClientDownloadInterfaceGetHisLogs(ST_XMEDIA_TIME szBeginTime, ST_XMEDIA_TIME szEndTime, vector<ST_SREVER_HIS_LOG> &v)
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
		ST_XMEDIA_HEAD msg_send;
		ST_XMEDIA_HEAD msg_recv;
		ST_SREVER_GET_HIS_LOG msg_send_data;
		memset(&msg_send, 0, sizeof(ST_XMEDIA_HEAD));
		memset(&msg_recv, 0, sizeof(ST_XMEDIA_HEAD));
		memset(&msg_send_data, 0, sizeof(ST_SREVER_GET_HIS_LOG));
		//
		msg_send.cmd = XMEDIA_COMMAND_GET_STORE_HISTORY_LOG;
		msg_send.session = m_nSessionID;
		msg_send.datalen = sizeof(ST_SREVER_GET_HIS_LOG);
		//
		memcpy(msg_send_data.devid, m_id, strlen(m_id));
		memcpy(&msg_send_data.tmBeginTime, &szBeginTime, sizeof(ST_XMEDIA_TIME));
		memcpy(&msg_send_data.tmEndTime, &szEndTime, sizeof(ST_XMEDIA_TIME));
		//
		memcpy(szSendBuffer, &msg_send, sizeof(msg_send));
		memcpy(szSendBuffer + sizeof(msg_send), &msg_send_data, sizeof(msg_send_data));
		//
		if (0 != m_pTcpStream->SyncWriteAndRead(szSendBuffer, sizeof(ST_XMEDIA_HEAD) + sizeof(ST_SREVER_GET_HIS_LOG), &msg_recv, sizeof(ST_XMEDIA_HEAD)))
		{
			CROSS_TRACE("XClientHislogListFromMedia ---- 1");
			break;
		}

		//
		if ((msg_recv.cmd != XMEDIA_COMMAND_GET_STORE_HISTORY_LOG) || (msg_recv.result != XMEDIA_RESULT_OK))
		{
			CROSS_TRACE("XClientHislogListFromMedia ---- 2");
			break;
		}

		if (msg_recv.datalen > 0)
		{
			if (0 != m_pTcpStream->SyncWriteAndRead(NULL, 0, pszBuffer, msg_recv.datalen))
			{
				CROSS_TRACE("XClientHislogListFromMedia ---- 3");
				break;
			}

			for (unsigned int i = 0; i < (msg_recv.datalen / sizeof(ST_SREVER_HIS_LOG)); i++)
			{
				ST_SREVER_HIS_LOG * pLog = (ST_SREVER_HIS_LOG *)(pszBuffer + i * sizeof(ST_SREVER_HIS_LOG));

				ST_SREVER_HIS_LOG t;
				memset(&t, 0, sizeof(t));

				t.nHislogID = pLog->nHislogID;
				memcpy(t.szFile, pLog->szFile, strlen(pLog->szFile));
				t.begin_time_tm = pLog->begin_time_tm;
				t.end_time_tm = pLog->end_time_tm;


				//t.store_begin_time_tm = pLog->begin_time_tm;
				//t.store_end_time_tm = pLog->end_time_tm;

				v.push_back(t);
			}
			CROSS_TRACE("XClientHislogListFromMedia -- -- --  8\n");
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


int CXClientHislog::XClientDownloadInterfaceDownloadHislog(ST_XMEDIA_TIME szBeginTime, ST_XMEDIA_TIME szEndTime)
{
	//return m_DownloadCallback.Open((CMediaDevInterface *)m_hDevInterface, t, szBeginTime, szEndTime);
#if 1
	memcpy(&m_tmEndTime, &szEndTime, sizeof(ST_XMEDIA_TIME));
	memcpy(&m_tmStartTime, &szBeginTime, sizeof(ST_XMEDIA_TIME));

	m_cs.Lock();

// 	m_pDownloadHisFileCallback = pDownloadObj;
// 	m_pUser = pUser;

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
		ST_XMEDIA_HEAD msg_send;
		ST_XMEDIA_HEAD msg_recv;
		ST_SREVER_OPEN_HIS_STREAM msg_send_data;
		memset(&msg_send, 0, sizeof(ST_XMEDIA_HEAD));
		memset(&msg_recv, 0, sizeof(ST_XMEDIA_HEAD));
		memset(&msg_send_data, 0, sizeof(ST_SREVER_OPEN_HIS_STREAM));
		//
		msg_send.cmd = XMEDIA_COMMAND_OPEN_HIS_STREAM;
		msg_send.session = m_nSessionID;
		//msg_send.seq = m_nSeq++;
		msg_send.datalen = sizeof(ST_SREVER_OPEN_HIS_STREAM);
		//
		memcpy(msg_send_data.devid, m_id, strlen(m_id));
		memcpy(&msg_send_data.tmBeginTime, &szBeginTime, sizeof(ST_XMEDIA_TIME));
		memcpy(&msg_send_data.tmEndTime, &szEndTime, sizeof(ST_XMEDIA_TIME));
		//
		memcpy(szSendBuffer, &msg_send, sizeof(ST_XMEDIA_HEAD));
		memcpy(szSendBuffer + sizeof(ST_XMEDIA_HEAD), &msg_send_data, sizeof(ST_SREVER_OPEN_HIS_STREAM));


		int n = sizeof(ST_XMEDIA_HEAD);

		//
		if (0 != m_pTcpStream->SyncWriteAndRead(szSendBuffer, sizeof(ST_XMEDIA_HEAD) + sizeof(ST_SREVER_OPEN_HIS_STREAM), &msg_recv, sizeof(ST_XMEDIA_HEAD), 15))
		{
			CROSS_TRACE("XClientHislogFromMedia ---- 1");
			break;
		}
		//
		if (msg_recv.cmd != XMEDIA_COMMAND_OPEN_HIS_STREAM)
		{
			CROSS_TRACE("XClientHislogFromMedia ---- 2");
			break;
		}
// 		if (msg_recv.result != XMEDIA_RESULT_OK) //失败
// 		{
// 			CROSS_TRACE("XClientHislogFromMedia ---- 2");
// 			break;
// 		}

		if (msg_recv.result == XMEDIA_RESULT_OK)//XMEDIA_RESULT_DOWNLOAD_START)
		{
			m_pTcpStream->SetStreamData(this);//<<<<<<<<<<<

			ret = 0;//<<<<<<<<<<<<<<<
		}
		else
		{
			//失败

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



#endif
	return 0;
}
int CXClientHislog::XClientDownloadInterfaceDownloadClose()
{
	if (m_pTcpStream)
	{
		m_pTcpStream->Release();
		m_pTcpStream = NULL;
	}

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


// 	if (pWaveOut)
// 	{
// 		pWaveOut->Close();
// 		delete pWaveOut;
// 		pWaveOut = NULL;
// 	}
	if (pAudioDecIns)
	{
		DestoryAudioDecInstance(pAudioDecIns);
		pAudioDecIns = NULL;
	}
#endif

	return 0;
}


#ifdef _PLATFORM_WINDOW
//
int CXClientHislog::XClientDownloadInterfacePlayFrameWindows(HWND hwnd)
{



	//m_pDownloadHisFileCallback = pDownloadObj;
	//m_pUser = this;
	m_hwnd = hwnd;
	//return m_DownloadCallback.Open((CMediaDevInterface *)m_hDevInterface, t, szBeginTime, szEndTime);

#if 1
	MyOnlinePlayVideoInfo *pData = GetNextVideoFrame();
	if (NULL == pData)
	{
		//播放结束?/正在缓冲?
		Sleep(10);
		return 0;
	}



	//-----------------------------------------------------------------------------------------------------------------------------
	CROSS_DWORD64 dwAudioAndDecvideoStart = CrossGetTickCount64();

	if (pData->nAudioCount > 0)
	{
// 		if (NULL == pWaveOut)
// 		{
// 			pWaveOut = new CWaveOut();
// 			pWaveOut->Open(2048, 1, 8000, 16);
// 			pAudioDecIns = CreateAudioDecInstance(RSPLAYER_CODEC_AUDIO_TYPE_AAC);
// 		}
// 
// 
// 
// 		for (int i = 0; i < pData->nAudioCount; i++)
// 		{
// 			int len = 0;
// 			char pszBuf[4096] = { 0 };
// 			ST_XMEDIA_HEAD *pAudioHeadTemp = (ST_XMEDIA_HEAD *)pData->pAudioBuffer[i];
// 			AudioDecData(pAudioDecIns, pData->pAudioBuffer[i] + sizeof(ST_XMEDIA_HEAD) + sizeof(ST_XMEDIA_AVHEAD), 
// 				pAudioHeadTemp->datalen - sizeof(ST_XMEDIA_AVHEAD), pszBuf, &len);
// 			pszBuf[len] = '\0';
// 			//
// 			if (pWaveOut->inlineis_start())
// 			{
// 				pWaveOut->input((unsigned char *)pszBuf, len);
// 			}
// 
// 		}
	}


	//-----------------------------------------------------------------------------------------------------------------------------
	// 		
	// 
	ST_XMEDIA_AVHEAD *pframeHead = (ST_XMEDIA_AVHEAD *)(pData->pVideoBuffer + sizeof(ST_XMEDIA_HEAD));

	if ((NULL == pDecoder) && (NULL == pVideoCallback))
	{
		int nFrameTypeTemp = RSPLAYER_CODEC_VIDEO_TYPE_H264;
		//
		if (((pframeHead->nAVFrameType == XMEDIA_FREAM_TYPE_H264_IFRAME) || (pframeHead->nAVFrameType == XMEDIA_FREAM_TYPE_H264_PFRAME)) && (NULL == pDecoder))
		{
			nFrameTypeTemp = RSPLAYER_CODEC_VIDEO_TYPE_H264;
		}
		//
		if (((pframeHead->nAVFrameType == XMEDIA_FREAM_TYPE_H265_IFRAME) || (pframeHead->nAVFrameType == XMEDIA_FREAM_TYPE_H265_PFRAME)) && (NULL == pDecoder))
		{
			nFrameTypeTemp = RSPLAYER_CODEC_VIDEO_TYPE_H265;
		}
		//
		pVideoCallback = new CDecDataCallback();

		pDecoder = CreateFFmpegInstance(nFrameTypeTemp, pVideoCallback);
		FFmpegStart(pDecoder);
		pVideoCallback->SetPlayHwnd(m_hwnd);


	}

	if ((pframeHead->nAVFrameType == XMEDIA_FREAM_TYPE_H264_IFRAME) ||
		(pframeHead->nAVFrameType == XMEDIA_FREAM_TYPE_H264_PFRAME) ||
		(pframeHead->nAVFrameType == XMEDIA_FREAM_TYPE_H265_IFRAME) ||
		(pframeHead->nAVFrameType == XMEDIA_FREAM_TYPE_H265_PFRAME)
		)
	{
		//CROSS_TRACE("CMediaPreviewInterface::data_callback --- 666 \n");

		ST_XMEDIA_HEAD *pVideoHeadTemp = (ST_XMEDIA_HEAD *)pData->pVideoBuffer;
		FFmpegInputData(pDecoder, (char *)pData->pVideoBuffer + sizeof(ST_XMEDIA_HEAD) + sizeof(ST_XMEDIA_AVHEAD), 
			pVideoHeadTemp->datalen - sizeof(ST_XMEDIA_AVHEAD), pframeHead->nVideoFrameRate);
	}

	int needSleep = (pData->nSleep - (int)(CrossGetTickCount64() - dwAudioAndDecvideoStart)) / 15 * 15;

	return needSleep;
#endif
	
	return 0;
}
#endif
//
int CXClientHislog::XClientDownloadInterfacePlaySeekSecond(int sec)
{
#if 1
	m_cs.Lock();

	do
	{
		if (sec >= m_nDownloadFileIndex)
		{
			break;
		}

		for (int z = 0; z < m_nDownloadFileIndex; z++)
		{


			ST_XMEDIA_AVHEAD *pTmp1 = (ST_XMEDIA_AVHEAD *)(m_pDownFileBuffer + m_pFileIndexArray[z] + sizeof(ST_XMEDIA_HEAD));
			ST_XMEDIA_AVHEAD *pTmp2 = (ST_XMEDIA_AVHEAD *)(m_pDownFileBuffer + m_pFileIndexArray[0] + sizeof(ST_XMEDIA_HEAD));

			CROSS_DWORD64 lastTM = __online_dec_cb_systime_to_millisecond(*pTmp1);
			CROSS_DWORD64 FilrstTM = __online_dec_cb_systime_to_millisecond(*pTmp2);

			int a = (int)(lastTM - FilrstTM); //
			if (a < 0)
			{
				a = 0;
			}

			if ((a / 1000) > sec)
			{
				m_nCureFileIndex = z;
				m_bNeedFirstIFrame = TRUE;
				break;
			}
		}
	} while (0);


	m_cs.Unlock();
#endif

return 0;
}
//
int CXClientHislog::XClientDownloadInterfacePlayGetPlaySecond()
{
#if 1
	if (!m_pDownFileBuffer)
	{
		return 0;
	}
	if (m_nCureFileIndex < 2)
	{
		return 0;
	}

	int k = m_nCureFileIndex - 2;
	//CROSS_TRACE(" **************  %d ", k);
	ST_XMEDIA_AVHEAD *pTmp1 = (ST_XMEDIA_AVHEAD *)(m_pDownFileBuffer + m_pFileIndexArray[k] + sizeof(ST_XMEDIA_HEAD));
	ST_XMEDIA_AVHEAD *pTmp2 = (ST_XMEDIA_AVHEAD *)(m_pDownFileBuffer + m_pFileIndexArray[0] + sizeof(ST_XMEDIA_HEAD));

	CROSS_DWORD64 lastTM = __online_dec_cb_systime_to_millisecond(*pTmp1);
	CROSS_DWORD64 FilrstTM = __online_dec_cb_systime_to_millisecond(*pTmp2);


	int a = (int)(lastTM - FilrstTM);
	if (a < 0)
	{

		a = 0;
	}
	return int(a / 1000);
#endif
	return 0;
}
int CXClientHislog::XClientDownloadInterfacePlayGetDownloadSecond()
{
#if 1
	if (!m_pDownFileBuffer)
	{
		return 0;
	}

	if (m_nDownloadFileIndex < 2)
	{
		return 0;
	}

	int k = m_nDownloadFileIndex - 2;



	ST_XMEDIA_AVHEAD *pTmp1 = (ST_XMEDIA_AVHEAD *)(m_pDownFileBuffer + m_pFileIndexArray[k] + sizeof(ST_XMEDIA_HEAD));
	ST_XMEDIA_AVHEAD *pTmp2 = (ST_XMEDIA_AVHEAD *)(m_pDownFileBuffer + m_pFileIndexArray[0] + sizeof(ST_XMEDIA_HEAD));

	CROSS_DWORD64 lastTM = __online_dec_cb_systime_to_millisecond(*pTmp1);
	CROSS_DWORD64 FilrstTM = __online_dec_cb_systime_to_millisecond(*pTmp2);

	int a = (int)(lastTM - FilrstTM);
	if (a < 0)
	{

		a = 0;
	}
	return int(a / 1000);
#endif
	return 0;
}
int CXClientHislog::XClientDownloadInterfacePlayGetFileSecond()
{
#if 1

	//CROSS_TRACE("******** %d %d %d %d %d %d ---- %d %d %d %d %d %d ",
	//	m_tmEndTime.wYear, m_tmEndTime.wMonth, m_tmEndTime.wDay, m_tmEndTime.wHour, m_tmEndTime.wMinute, m_tmEndTime.wSecond,
	//	m_tmStartTime.wYear, m_tmStartTime.wMonth, m_tmStartTime.wDay, m_tmStartTime.wHour, m_tmStartTime.wMinute, m_tmStartTime.wSecond);

	CROSS_DWORD64 lastTM = __online_dec_cb_systime_to_millisecond(m_tmEndTime);
	CROSS_DWORD64 FilrstTM = __online_dec_cb_systime_to_millisecond(m_tmStartTime);

	//CROSS_TRACE(" **************  %lld    %lld ", lastTM, FilrstTM);

	int a = (int)(lastTM - FilrstTM);
	if (a < 0)
	{
		a = 0;
	}
	return int(a / 1000);
#endif
	return 0;
}
int CXClientHislog::XClientDownloadInterfacePlayWiteMp4File(char * pathfile)
{
#if 1
	int ret = -1;
	m_cs.Lock();

	do
	{
		m_nWiteMp4FileProgress = 0;
		ST_XMEDIA_HEAD *pTmp2 = (ST_XMEDIA_HEAD *)(m_pDownFileBuffer + m_pFileIndexArray[0]);
		ST_XMEDIA_AVHEAD *pAvHeader = (ST_XMEDIA_AVHEAD *)(m_pDownFileBuffer + m_pFileIndexArray[0] + sizeof(ST_XMEDIA_HEAD));
		void *pMp4File = NULL;

		if ((pAvHeader->nAudioChannels <= 0) || (pAvHeader->nAudioSamplesRate <= 0) || (pAvHeader->nAudioBitsPerSample <= 0) || (pAvHeader->nAudioBitRate <= 0))//
		{
			pMp4File = Mp4File_CreateInstance4WriteH264(pathfile,
				pAvHeader->nVideoWidth, pAvHeader->nVideoHeight, pAvHeader->nVideoFrameRate,
				1, 8000, 16, 16000, 2048);

		}
		else
		{
// 			pMp4File = Mp4File_CreateInstance4WriteH264(pathfile,
// 				pAvHeader->nVideoWidth, pAvHeader->nVideoHeight, pAvHeader->nVideoFrameRate,
// 				pAvHeader->nAudioChannels, pAvHeader->nAudioSamplesRate, pAvHeader->nAudioBitsPerSample, pTmp2->nAudioBitRate, 2048);
		}



		int m = m_nDownloadFileIndex;
		for (int i = 0; i < m; i++)
		{
			pTmp2 = (ST_XMEDIA_HEAD *)(m_pDownFileBuffer + m_pFileIndexArray[i]);
			pAvHeader = (ST_XMEDIA_AVHEAD *)(m_pDownFileBuffer + m_pFileIndexArray[i] + sizeof(ST_XMEDIA_HEAD));

			BOOL bVideo = FALSE;
			BOOL bVideoI = FALSE;
			if ((pAvHeader->nAVFrameType == XMEDIA_FREAM_TYPE_H264_IFRAME) ||
				(pAvHeader->nAVFrameType == XMEDIA_FREAM_TYPE_H264_PFRAME) ||
				(pAvHeader->nAVFrameType == XMEDIA_FREAM_TYPE_H265_IFRAME) ||
				(pAvHeader->nAVFrameType == XMEDIA_FREAM_TYPE_H265_PFRAME)

				)
			{
				bVideo = TRUE;
			}
			if ((pAvHeader->nAVFrameType == XMEDIA_FREAM_TYPE_H264_IFRAME) ||
				(pAvHeader->nAVFrameType == XMEDIA_FREAM_TYPE_H265_IFRAME)
				)
			{
				bVideoI = TRUE;
			}

			Mp4File_WriteData(pMp4File, bVideo, bVideoI, (char *)pTmp2 + sizeof(ST_XMEDIA_AVHEAD), pTmp2->datalen - sizeof(ST_XMEDIA_AVHEAD));

			m_nWiteMp4FileProgress = i * 100 / m;
		}
		Mp4File_DestoryInstance4Write(pMp4File);
		m_nWiteMp4FileProgress = 100;

	} while (0);

	m_cs.Unlock();

	return ret;
#endif
	return 0;
}
int CXClientHislog::XClientDownloadInterfacePlayWiteMp4FileProgress()
{
	return m_nWiteMp4FileProgress;
}



//--------------------------------------------------------------------------------------------------------------------------------------------------
//从流媒体取数据
//int m_kkkkkkkkkkkkkkkkk = 0;
void CXClientHislog::OnPacketCompleteNetStreamData(int32_t bytesTransferred, emXStreamTansferKey transferKey)
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

void CXClientHislog::data_callback()
{
// 	if (m_pDownloadHisFileCallback)
// 	{
// 		m_pDownloadHisFileCallback->OnDownloadHisFileCallback(m_frameHead, m_pDataBuffer, m_dataLen, m_pUser);
// 	}
	
	CROSS_TRACE("=======>>>>> %d   %d \n", m_frameHead.cmd, m_frameHead.result);
#if 1
	if (m_frameHead.cmd == XMEDIA_COMMAND_AV_STREAM)
	{

		if (XMEDIA_RESULT_DOWNLOAD_END == m_frameHead.result)
		{

		}

		if (XMEDIA_RESULT_DOWNLOAD_ERROR == m_frameHead.result)
		{

		}

		if (XMEDIA_RESULT_DOWNLOAD_DATA == m_frameHead.result)
		{
			//TRACE("__online_dec_cb_systime_to_millisecond    %lld\n", __online_dec_cb_systime_to_millisecond(head));

			m_pFileIndexArray[m_nDownloadFileIndex] = m_nDownloadFileLen;
			//
			memcpy(m_pDownFileBuffer + m_nDownloadFileLen, &m_frameHead, sizeof(m_frameHead));
			m_nDownloadFileLen += sizeof(m_frameHead);
			memcpy(m_pDownFileBuffer + m_nDownloadFileLen, m_pDataBuffer, m_dataLen);
			m_nDownloadFileLen += m_dataLen;
			m_nDownloadFileIndex++;
		}

	}
#endif

}




//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
BOOL CXClientHislog::Login()
{

	BOOL bLoginOK = FALSE;
	do
	{
		//CROSS_TRACE("CMediaDevInterface::Login ------------------------- 1");
		//--------------------------------------------------------------------------------------------------------------------------
		//关闭网络
		if (m_pTcpStream)
		{
			m_pTcpStream->Release();
			m_pTcpStream = NULL;
		}
		//CROSS_TRACE("CMediaDevInterface::Login ------------------------- 2");
		//--------------------------------------------------------------------------------------------------------------------------
		//清理消息
// 		m_csMsglist.Lock();
// 		while (m_msgList.size() > 0)
// 		{
// 			ST_STREAM_BUFFER * p = m_msgList.front();
// 			m_msgList.pop_front();
// 			CStreamBufferPool::Instance().free(p);
// 		}
// 		m_csMsglist.Unlock();
// 		//CROSS_TRACE("CMediaDevInterface::Login ------------------------- 3");
// 		//关闭解码
// 		if (m_dec)
// 		{
// 			FFmpegStop(m_dec);
// 			DestoryFFmpegInstance(m_dec);
// 			m_dec = NULL;
// 		}

		//CROSS_TRACE("CMediaDevInterface::Login ------------------------- 4");
		//--------------------------------------------------------------------------------------------------------------------------
		//链接服务器
		m_pTcpStream = XNetCreateStream4Connect(m_szIP, m_port, 3);
		//CROSS_TRACE("CMediaDevInterface::Login ------------------------- 4-1");
		if (0 != XNetConnectStream(m_pTcpStream))
		{
			CROSS_TRACE("CMediaDevInterface::Login -- conn svr error");
			break;
		}
		//CROSS_TRACE("CMediaDevInterface::Login ------------------------- 5");
		//--------------------------------------------------------------------------------------------------------------------------
		//发送Z_CMD_OF_SERVER_LOGIN_NEED_SESSIONID
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
		//发送Z_CMD_OF_SERVER_LOGIN,  使用key加密，登陆
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

//		memcpy(msg_login_info.devid, m_id, strlen(m_id));
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
		//m_pTcpStream->SetStreamData(this);
		//m_dwLastTick = CrossGetTickCount64();
		bLoginOK = TRUE;
		//m_emClientNetStatus = CLIENT_NET_STATUS_CONNECTED;
		CROSS_TRACE("CMediaDevInterface::Login ------------------------- okokokokok");
		//if (m_bUserOpenPreview)
		{
			//AddSendMessge(XMEDIA_COMMAND_OPEN_STREAM, NULL, 0);
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







MyOnlinePlayVideoInfo * CXClientHislog::GetNextVideoFrame()//请注意，非线程安全
{

#if 1
	m_cs.Lock();

	BOOL bFind = FALSE;
	do
	{
		//-------------------------------------------------
		//ResetFrame();
		//-------------------------------------------------

		if (m_nCureFileIndex >= m_nDownloadFileIndex)
		{
			//CROSS_TRACE("______ %d  -- %d $$$$$$$$$$\n", m_nCureFileIndex, m_nDownloadFileIndex);
			break;
		}


		int i = 0;
		BOOL bFindVideoFrame = FALSE;


		//------------------------------------------------------------------------------------------------------
		// ?????????????????
		//------------------------------------------------------------------------------------------------------
		for (i = m_nCureFileIndex; i < m_nDownloadFileIndex; i++)
		{
			ST_XMEDIA_HEAD *pTmp = (ST_XMEDIA_HEAD *)(m_pDownFileBuffer + m_pFileIndexArray[i]);
			ST_XMEDIA_AVHEAD *pAvHeader = (ST_XMEDIA_AVHEAD *)(m_pDownFileBuffer + m_pFileIndexArray[i] + sizeof(ST_XMEDIA_HEAD));

			if (m_bNeedFirstIFrame)
			{
				if ((pTmp->cmd == XMEDIA_COMMAND_AV_STREAM) &&
					((pAvHeader->nAVFrameType == XMEDIA_FREAM_TYPE_H264_IFRAME) || (pAvHeader->nAVFrameType == XMEDIA_FREAM_TYPE_H265_IFRAME))
					)
				{
					m_bNeedFirstIFrame = FALSE;
					bFindVideoFrame = TRUE;
					break;
				}
			}
			else
			{
				if ((pTmp->cmd == XMEDIA_COMMAND_AV_STREAM) &&
					((pAvHeader->nAVFrameType == XMEDIA_FREAM_TYPE_H264_IFRAME) ||
					(pAvHeader->nAVFrameType == XMEDIA_FREAM_TYPE_H264_PFRAME) ||
					(pAvHeader->nAVFrameType == XMEDIA_FREAM_TYPE_H265_IFRAME) ||
					(pAvHeader->nAVFrameType == XMEDIA_FREAM_TYPE_H265_PFRAME))
					)
				{
					bFindVideoFrame = TRUE;
					break;
				}
			}
		}

		if (!bFindVideoFrame)
		{
			//
			break;
		}


		//------------------------------------------------------------------------------------------------------
		// ???????????????????????????????
		//------------------------------------------------------------------------------------------------------
		int nAudioConut = 0;
		bFindVideoFrame = FALSE;
		int k = 0;
		for (k = i + 1; k < m_nDownloadFileIndex; k++)
		{
			ST_XMEDIA_HEAD *pTmp = (ST_XMEDIA_HEAD *)(m_pDownFileBuffer + m_pFileIndexArray[k]);
			ST_XMEDIA_AVHEAD *pAvHeader = (ST_XMEDIA_AVHEAD *)(m_pDownFileBuffer + m_pFileIndexArray[k] + sizeof(ST_XMEDIA_HEAD));


			if ((pTmp->cmd == XMEDIA_COMMAND_AV_STREAM) &&
				(pAvHeader->nAVFrameType == XMEDIA_FREAM_TYPE_AAC)
				)
			{
				if (nAudioConut < MAX_ONE_VIDEO_AUDIO_COUNT)
				{
					memcpy(m_pFrame->pAudioBuffer[nAudioConut], pTmp, pTmp->datalen + sizeof(ST_XMEDIA_HEAD));
					//m_pFrame->stAudio[nAudioConut].nAudioLen = pTmp->datalen;
					nAudioConut++;
				}
			}

			if ((pTmp->cmd == XMEDIA_COMMAND_AV_STREAM) &&
				((pAvHeader->nAVFrameType == XMEDIA_FREAM_TYPE_H264_IFRAME) ||
				(pAvHeader->nAVFrameType == XMEDIA_FREAM_TYPE_H264_PFRAME) ||
				(pAvHeader->nAVFrameType == XMEDIA_FREAM_TYPE_H265_IFRAME) ||
				(pAvHeader->nAVFrameType == XMEDIA_FREAM_TYPE_H265_PFRAME))
				)
			{
				bFindVideoFrame = TRUE;
				break;
			}
		}

		if (!bFindVideoFrame)
		{
			//??????????????????????null
			break;
		}
		//------------------------------------------------------------------------------------------------------
		// ?????????????????
		//------------------------------------------------------------------------------------------------------
		m_nCureFileIndex = k;//



		//???????????
		ST_XMEDIA_HEAD *pTmp = (ST_XMEDIA_HEAD *)(m_pDownFileBuffer + m_pFileIndexArray[i]);
		//m_pFrame->nVideoLen = pTmp->datalen;
		memcpy(m_pFrame->pVideoBuffer, pTmp, pTmp->datalen + sizeof(ST_XMEDIA_HEAD));
		//
		m_pFrame->nAudioCount = nAudioConut;
		//sleep
		ST_XMEDIA_AVHEAD *pTmpLast = (ST_XMEDIA_AVHEAD *)(m_pDownFileBuffer + m_pFileIndexArray[k] + sizeof(ST_XMEDIA_HEAD));
		ST_XMEDIA_AVHEAD *pFilrstTM = (ST_XMEDIA_AVHEAD *)(m_pDownFileBuffer + m_pFileIndexArray[i] + sizeof(ST_XMEDIA_HEAD));

		CROSS_DWORD64 lastTM = __online_dec_cb_systime_to_millisecond(*pTmpLast);
		CROSS_DWORD64 FilrstTM = __online_dec_cb_systime_to_millisecond(*pFilrstTM);
		m_pFrame->nSleep = int(lastTM - FilrstTM);
		if (m_pFrame->nSleep < 0)
		{
			m_pFrame->nSleep = 33;
		}
		if (m_pFrame->nSleep > 1000)
		{
			m_pFrame->nSleep = 33;
		}
		//CROSS_TRACE("______ %d  -- %d\n", m_nCureFileIndex, m_nDownloadFileIndex);
		bFind = TRUE;
	} while (0);

	m_cs.Unlock();
	return bFind ? m_pFrame : NULL;
#endif
	
	return NULL;
}
