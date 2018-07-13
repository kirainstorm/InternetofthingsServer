
#ifndef download_callback_jhdashbdyq23213jjebfdu232jbwedbfsnmk
#define download_callback_jhdashbdyq23213jjebfdu232jbwedbfsnmk



#include "XMediaInterface.h"
#include "PlatformDefine.h"
#include "XPlayerDecdatacallback.hpp"
#include "Mp4File.hpp"

#define MAX_FILE_BUFFER_LEN (128*1024*1024)
#define MAX_FILE_INDEX_COUNT (100*1024)

//播放思路是
//开始播放的时候，先取第一帧I帧显示，同时取到到下一个视频帧要等待的间隔
//继续取下一个视频帧，但是这之间肯定有N多的音频帧，一起取出来，并且同时取到到下一个视频帧要等待的间隔

//视频等待播放时间间隔，这时候音频数据送入音频解码

#define MAX_ONE_VIDEO_AUDIO_COUNT 16
struct MyOnlinePlayAudioInfo
{
	char *pAudioBuffer;//视频数据  //带ST_SREVER_STREAM_HEAD
	int nAudioLen;//视频数据长度//不带 sizeof(ST_SREVER_STREAM_HEAD)
};

struct MyOnlinePlayVideoInfo
{
	int nSleep;//与上一帧的时间差
	//
	char *pVideoBuffer;//视频数据//带ST_SREVER_STREAM_HEAD
	int nVideoLen;//视频数据长度 //不带 sizeof(ST_SREVER_STREAM_HEAD)
	//
	//最多MAX_ONE_VIDEO_AUDIO_COUNT帧音频，如果2个视频之间超过MAX_ONE_VIDEO_AUDIO_COUNT帧，则取最前的MAX_ONE_VIDEO_AUDIO_COUNT帧，不可能让用户那么久看不到图像
	MyOnlinePlayAudioInfo stAudio[MAX_ONE_VIDEO_AUDIO_COUNT];
	int nAudioCount;

};
inline CROSS_DWORD64 __online_dec_cb_systime_to_millisecond(ST_SERVER_HEAD& st)//
{
	struct tm gm = { st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth - 1, st.wYear - 1900, st.wDayOfWeek, 0, 0 };
	CROSS_DWORD64 k = mktime(&gm) * 1000 + st.wMilliseconds;
	return k;
}


inline CROSS_DWORD64 __online_dec_cb_systime_to_millisecond(_XSERVER_TIME& st)//
{
	struct tm gm = { st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth - 1, st.wYear - 1900, st.wDayOfWeek, 0, 0 };
	CROSS_DWORD64 k = mktime(&gm) * 1000 + st.wMilliseconds;
	return k;
}

class CDownloadCallback :public CXMediaDownloadCallback
{
public:
	CDownloadCallback()
	{
		m_pDownFileBuffer = NULL;
		m_pDownInterface = NULL;
		m_pFileIndexArray = NULL;
		m_bNeedFirstIFrame = TRUE;
		m_pFrame = NULL;
		m_nWiteMp4FileProgress = 0;
		m_pFrame = new MyOnlinePlayVideoInfo;
		memset(m_pFrame, 0, sizeof(MyOnlinePlayVideoInfo));
		m_pFrame->pVideoBuffer = new char[2 * 1024 * 1024];

		for (int i = 0; i < MAX_ONE_VIDEO_AUDIO_COUNT; i++)
		{
			m_pFrame->stAudio[i].pAudioBuffer = new char[4096];
		}
	};
	~CDownloadCallback()
	{
		for (int i = 0; i < MAX_ONE_VIDEO_AUDIO_COUNT; i++)
		{
			delete[] m_pFrame->stAudio[i].pAudioBuffer;
		}
		delete[] m_pFrame->pVideoBuffer;
		delete m_pFrame;
	};
public:
	virtual int OnDownloadHisFileCallback(ST_SERVER_HEAD head, const char *data, int len, const void * pUser)
	{
		//CROSS_TRACE("_________________________________ %d  -- %d\n", m_nIndexTmp++, len);
		if (head.cmd == Z_CMD_OF_SERVER_AV_STREAM)
		{

			if (_CMD_OF_SERVER_RESULT_DOWNLOAD_END == head.result)
			{

			}

			if (_CMD_OF_SERVER_RESULT_DOWNLOAD_ERROR == head.result)
			{

			}

			if (_CMD_OF_SERVER_RESULT_DOWNLOAD_DATA == head.result)
			{
				//TRACE("__online_dec_cb_systime_to_millisecond    %lld\n", __online_dec_cb_systime_to_millisecond(head));

				m_pFileIndexArray[m_nDownloadFileIndex] = m_nDownloadFileLen;


				memcpy(m_pDownFileBuffer + m_nDownloadFileLen, &head, sizeof(ST_SERVER_HEAD));
				m_nDownloadFileLen += sizeof(ST_SERVER_HEAD);
				memcpy(m_pDownFileBuffer + m_nDownloadFileLen, data, len);
				m_nDownloadFileLen += len;

				m_nDownloadFileIndex++;
			}

		}


		// 	if (0 != len)
		// 	{
		// 		m_pFileIndexArray[m_nDownloadFileIndex] = m_nDownloadFileLen;
		// 
		// 		memcpy(m_pDownFileBuffer + m_nDownloadFileLen, data, len);
		// 		m_nDownloadFileLen += len;
		// 
		// 		m_nDownloadFileIndex++;
		// 	}
		// 	else
		// 	{
		// 
		// 		//
		// 	}



		return 0;
	}
	;

public:
	int Open(CXMediaDeviceInterface *pDevInteface, DeviceInfo_T t, _XSERVER_TIME szBeginTime, _XSERVER_TIME szEndTime)
	{
		m_nWiteMp4FileProgress = 0;
		m_nCureFileIndex = 0;
		m_nDownloadFileIndex = 0;
		m_nDownloadFileLen = 0;
		m_pDownInterface = NULL;
		m_pFileIndexArray = NULL;
		m_bNeedFirstIFrame = TRUE;
		m_nIndexTmp = 0;


		int ret = -1;
		do
		{
			m_tmStartTime = szBeginTime;
			m_tmEndTime = szEndTime;
			//m_pDevInteface = pDevInteface;
			if (pDevInteface == NULL)
			{
				break;
			}


			pDevInteface->XDeviceSetInfo(t);


			m_pDownInterface = pDevInteface->CreateDownloadHisFileInstance();
			if (NULL == m_pDownInterface)
			{
				break;
			}

			m_pFileIndexArray = new int[MAX_FILE_INDEX_COUNT];
			memset(m_pFileIndexArray, 0, MAX_FILE_INDEX_COUNT);
			m_pDownFileBuffer = new char[MAX_FILE_BUFFER_LEN];

			if (0 != m_pDownInterface->XDownloadHislogFromMedia(m_tmStartTime, m_tmEndTime, this, this))
			{
				break;
			}



			ret = 0;
		} while (0);

		if (-1 == ret)
		{
			Close();
		}

		return ret;
	};
	int Close()
	{

		if (m_pDownInterface)
		{
			m_pDownInterface->XDownloadHisFileClose();
			m_pDownInterface->XDownloadHisFileDelete();
			m_pDownInterface = NULL;
		}

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


		return 0;
	};

	int GetFileSecond()
	{

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
	};
	int GetDownloadFileSecond()
	{
		if (!m_pDownFileBuffer)
		{
			return 0;
		}

		if (m_nDownloadFileIndex < 2)
		{
			return 0;
		}

		int k = m_nDownloadFileIndex - 2;



		ST_SERVER_HEAD *pTmp1 = (ST_SERVER_HEAD *)(m_pDownFileBuffer + m_pFileIndexArray[k]);
		ST_SERVER_HEAD *pTmp2 = (ST_SERVER_HEAD *)(m_pDownFileBuffer + m_pFileIndexArray[0]);

		CROSS_DWORD64 lastTM = __online_dec_cb_systime_to_millisecond(*pTmp1);
		CROSS_DWORD64 FilrstTM = __online_dec_cb_systime_to_millisecond(*pTmp2);

		int a = (int)(lastTM - FilrstTM);
		if (a < 0)
		{

			a = 0;
		}
		return int(a / 1000);
	};
	int GetPlayFileSecond()
	{
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
		ST_SERVER_HEAD *pTmp1 = (ST_SERVER_HEAD *)(m_pDownFileBuffer + m_pFileIndexArray[k]);
		ST_SERVER_HEAD *pTmp2 = (ST_SERVER_HEAD *)(m_pDownFileBuffer + m_pFileIndexArray[0]);

		CROSS_DWORD64 lastTM = __online_dec_cb_systime_to_millisecond(*pTmp1);
		CROSS_DWORD64 FilrstTM = __online_dec_cb_systime_to_millisecond(*pTmp2);


		int a = (int)(lastTM - FilrstTM);
		if (a < 0)
		{

			a = 0;
		}
		return int(a / 1000);
	};
	MyOnlinePlayVideoInfo * GetNextVideoFrame()//请注意，非线程安全
	{
		m_cs.Lock();

		BOOL bFind = FALSE;
		do
		{
			//-------------------------------------------------
			ResetFrame();
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
				ST_SERVER_HEAD *pTmp = (ST_SERVER_HEAD *)(m_pDownFileBuffer + m_pFileIndexArray[i]);
				if (m_bNeedFirstIFrame)
				{
					if ((pTmp->cmd == Z_CMD_OF_SERVER_AV_STREAM) &&
						((pTmp->nAVFrameType == XSERVER_FREAM_TYPE_H264_IFRAME) || (pTmp->nAVFrameType == XSERVER_FREAM_TYPE_H265_IFRAME))
						)
					{
						m_bNeedFirstIFrame = FALSE;
						bFindVideoFrame = TRUE;
						break;
					}
				}
				else
				{
					if ((pTmp->cmd == Z_CMD_OF_SERVER_AV_STREAM) &&
						((pTmp->nAVFrameType == XSERVER_FREAM_TYPE_H264_IFRAME) ||
						(pTmp->nAVFrameType == XSERVER_FREAM_TYPE_H264_PFRAME) ||
						(pTmp->nAVFrameType == XSERVER_FREAM_TYPE_H265_IFRAME) ||
						(pTmp->nAVFrameType == XSERVER_FREAM_TYPE_H265_PFRAME))
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
				ST_SERVER_HEAD *pTmp = (ST_SERVER_HEAD *)(m_pDownFileBuffer + m_pFileIndexArray[k]);

				if ((pTmp->cmd == Z_CMD_OF_SERVER_AV_STREAM) &&
					(pTmp->nAVFrameType == XSERVER_FREAM_TYPE_AAC)
					)
				{
					if (nAudioConut < MAX_ONE_VIDEO_AUDIO_COUNT)
					{
						memcpy(m_pFrame->stAudio[nAudioConut].pAudioBuffer, pTmp, pTmp->datalen + sizeof(ST_SERVER_HEAD));
						m_pFrame->stAudio[nAudioConut].nAudioLen = pTmp->datalen;
						nAudioConut++;
					}
				}

				if ((pTmp->cmd == Z_CMD_OF_SERVER_AV_STREAM) &&
					((pTmp->nAVFrameType == XSERVER_FREAM_TYPE_H264_IFRAME) ||
					(pTmp->nAVFrameType == XSERVER_FREAM_TYPE_H264_PFRAME) ||
					(pTmp->nAVFrameType == XSERVER_FREAM_TYPE_H265_IFRAME) ||
					(pTmp->nAVFrameType == XSERVER_FREAM_TYPE_H265_PFRAME))
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
			ST_SERVER_HEAD *pTmp = (ST_SERVER_HEAD *)(m_pDownFileBuffer + m_pFileIndexArray[i]);
			m_pFrame->nVideoLen = pTmp->datalen;
			memcpy(m_pFrame->pVideoBuffer, pTmp, m_pFrame->nVideoLen + sizeof(ST_SERVER_HEAD));
			//
			m_pFrame->nAudioCount = nAudioConut;
			//sleep
			ST_SERVER_HEAD *pTmpLast = (ST_SERVER_HEAD *)(m_pDownFileBuffer + m_pFileIndexArray[k]);
			CROSS_DWORD64 lastTM = __online_dec_cb_systime_to_millisecond(*pTmpLast);
			CROSS_DWORD64 FilrstTM = __online_dec_cb_systime_to_millisecond(*pTmp);
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
	}
	int SeekSecond(int i)
	{
		m_cs.Lock();

		do
		{
			if (i >= m_nDownloadFileIndex)
			{
				break;
			}

			for (int z = 0; z < m_nDownloadFileIndex; z++)
			{
				ST_SERVER_HEAD *pTmp1 = (ST_SERVER_HEAD *)(m_pDownFileBuffer + m_pFileIndexArray[z]);
				ST_SERVER_HEAD *pTmp2 = (ST_SERVER_HEAD *)(m_pDownFileBuffer + m_pFileIndexArray[0]);

				CROSS_DWORD64 lastTM = __online_dec_cb_systime_to_millisecond(*pTmp1);
				CROSS_DWORD64 FilrstTM = __online_dec_cb_systime_to_millisecond(*pTmp2);

				int a = (int)(lastTM - FilrstTM); //
				if (a < 0)
				{
					a = 0;
				}

				if ((a / 1000) > i)
				{
					m_nCureFileIndex = z;
					m_bNeedFirstIFrame = TRUE;
					break;
				}
			}
		} while (0);


		m_cs.Unlock();
		return 0;
	};


	int WiteMp4File(char * filepath)
	{
		int ret = -1;
		m_cs.Lock();

		do
		{
			m_nWiteMp4FileProgress = 0;
			ST_SERVER_HEAD *pTmp2 = (ST_SERVER_HEAD *)(m_pDownFileBuffer + m_pFileIndexArray[0]);
			void *pMp4File = NULL;

			if ((pTmp2->nAudioChannels <= 0) || (pTmp2->nAudioSamplesRate <= 0) || (pTmp2->nAudioBitsPerSample <= 0) || (pTmp2->nAudioBitRate <= 0))//
			{
				pMp4File = Mp4File_CreateInstance4WriteH264(filepath,
					pTmp2->nVideoWidth, pTmp2->nVideoHeight, pTmp2->nVideoFrameRate,
					1, 8000, 16, 16000, 2048);

			}
			else
			{
				pMp4File = Mp4File_CreateInstance4WriteH264(filepath,
					pTmp2->nVideoWidth, pTmp2->nVideoHeight, pTmp2->nVideoFrameRate,
					pTmp2->nAudioChannels, pTmp2->nAudioSamplesRate, pTmp2->nAudioBitsPerSample, pTmp2->nAudioBitRate, 2048);
			}



			int m = m_nDownloadFileIndex;
			for (int i = 0; i < m; i++)
			{
				pTmp2 = (ST_SERVER_HEAD *)(m_pDownFileBuffer + m_pFileIndexArray[i]);

				BOOL bVideo = FALSE;
				BOOL bVideoI = FALSE;
				if ((pTmp2->nAVFrameType == XSERVER_FREAM_TYPE_H264_IFRAME) ||
					(pTmp2->nAVFrameType == XSERVER_FREAM_TYPE_H264_PFRAME) ||
					(pTmp2->nAVFrameType == XSERVER_FREAM_TYPE_H265_IFRAME) ||
					(pTmp2->nAVFrameType == XSERVER_FREAM_TYPE_H265_PFRAME)

					)
				{
					bVideo = TRUE;
				}
				if ((pTmp2->nAVFrameType == XSERVER_FREAM_TYPE_H264_IFRAME) ||
					(pTmp2->nAVFrameType == XSERVER_FREAM_TYPE_H265_IFRAME)
					)
				{
					bVideoI = TRUE;
				}

				Mp4File_WriteData(pMp4File, bVideo, bVideoI, (char *)pTmp2 + sizeof(ST_SERVER_HEAD), pTmp2->datalen);

				m_nWiteMp4FileProgress = i * 100 / m;
			}
			Mp4File_DestoryInstance4Write(pMp4File);
			m_nWiteMp4FileProgress = 100;

		} while (0);

		m_cs.Unlock();

		return ret;
	};
	int GetWiteMp4FileProgress()
	{
		return m_nWiteMp4FileProgress;
	};//返回0-100;

private:
	char * m_pDownFileBuffer;
	BOOL m_bNeedFirstIFrame;
	MyOnlinePlayVideoInfo *m_pFrame;


	void ResetFrame(){

	};

	int m_nWiteMp4FileProgress;
private:
	//CXMediaDeviceInterface *m_pDevInteface;
	CXMediaDownloadInterface *m_pDownInterface;
	CrossCriticalSection m_cs;

	_XSERVER_TIME m_tmStartTime;
	_XSERVER_TIME m_tmEndTime;


	int m_nDownloadFileLen;//已经下载的数据长度
	int m_nDownloadFileIndex;//已经下载的数据帧数
	int m_nCureFileIndex;//当前读到第几帧
	int *m_pFileIndexArray;//每一帧在缓冲区的地址
	



	int m_nIndexTmp;
};

#endif
