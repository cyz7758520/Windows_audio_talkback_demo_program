#include "VdoInpt.h"
#include "qedit.h"

void FreeMediaType( AM_MEDIA_TYPE& mt )
{
	if( mt.cbFormat != 0 )
	{
		CoTaskMemFree( ( PVOID )mt.pbFormat );
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if( mt.pUnk != NULL )
	{
		// pUnk should not be used.
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}

// Delete a media type structure that was allocated on the heap.
void DeleteMediaType( AM_MEDIA_TYPE * pmt )
{
	if( pmt != NULL )
	{
		FreeMediaType( *pmt );
		CoTaskMemFree( pmt );
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoInptThrd
 * 功能说明：视频输入线程主类。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：无。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

struct VdoInptThrd : public ISampleGrabberCB
{
public:
	VdoInpt * m_VdoInptPt;

	VdoInptThrd( VdoInpt * VdoInptPt )
	{
		m_VdoInptPt = VdoInptPt;
	}

	STDMETHODIMP_( ULONG ) AddRef()
	{
		return 1;
	}

	STDMETHODIMP_( ULONG ) Release()
	{
		return 1;
	}

	STDMETHODIMP QueryInterface( REFIID riid, void * * ppv )
	{
		if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown )
		{
			*ppv = ( void * ) static_cast< ISampleGrabberCB* > ( this );
			return NOERROR;
		}
		return E_NOINTERFACE;
	}

	STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample )
	{
		return 0;
	}

	STDMETHODIMP BufferCB( double dblSampleTime, BYTE * pBuffer, long lBufferSize )
	{
		if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输入线程：读取一个BGRA8888格式视频输入原始帧。" ) );

		//丢弃采样频率过快的BGRA8888格式视频输入原始帧。
		m_VdoInptPt->m_LastTickMsec = FuncGetTickAsMsec();
		if( m_VdoInptPt->m_LastVdoInptFrmTickMsec != 0 ) //如果已经设置过上一个视频输入帧的嘀嗒钟。
		{
			if( m_VdoInptPt->m_LastTickMsec - m_VdoInptPt->m_LastVdoInptFrmTickMsec >= m_VdoInptPt->m_VdoInptFrmTimeStepMsec )
			{
				m_VdoInptPt->m_LastVdoInptFrmTickMsec += m_VdoInptPt->m_VdoInptFrmTimeStepMsec;
			}
			else
			{
				if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输入线程：采样频率过快，本次视频输入帧丢弃。" ) );
				goto Out;
			}
		}
		else //如果没有设置过上一个视频输入帧的嘀嗒钟。
		{
			m_VdoInptPt->m_LastVdoInptFrmTickMsec = m_VdoInptPt->m_LastTickMsec;
		}
		
		//获取一个视频输入空闲帧。
		m_VdoInptPt->m_VdoInptIdleFrmLnkLst.GetTotal( &m_VdoInptPt->m_FrmLnkLstElmTotal, 1, NULL ); //获取视频输入空闲帧链表的元素总数。
        if( m_VdoInptPt->m_FrmLnkLstElmTotal > 0 ) //如果视频输入空闲帧链表中有帧。
        {
            //从视频输入空闲帧链表中取出第一个帧。
            {
				m_VdoInptPt->m_VdoInptIdleFrmLnkLst.Locked( NULL ); //视频输入空闲帧链表的互斥锁加锁。
                m_VdoInptPt->m_VdoInptIdleFrmLnkLst.GetHead( NULL, &m_VdoInptPt->m_VdoInptFrmElmPt, NULL, 0, NULL );
				m_VdoInptPt->m_VdoInptIdleFrmLnkLst.DelHead( 0, NULL );
				m_VdoInptPt->m_VdoInptIdleFrmLnkLst.Unlock( NULL ); //视频输入空闲帧链表的互斥锁解锁。
            }
            if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )  LOGFI( Cu8vstr( "视频输入线程：从视频输入空闲帧链表中取出第一个帧，视频输入空闲帧链表元素总数：%uzd。" ), m_VdoInptPt->m_FrmLnkLstElmTotal );
        }
        else //如果视频输入空闲帧链表中没有帧。
        {
			m_VdoInptPt->m_VdoInptFrmLnkLst.GetTotal( &m_VdoInptPt->m_FrmLnkLstElmTotal, 1, NULL ); //获取视频输入帧链表的元素总数。
			if( m_VdoInptPt->m_FrmLnkLstElmTotal <= 1 ) //视频输入帧链表最多只存储2帧，避免因为视频设备有变化导致卡顿并积累大量视频输入帧，从而导致不及时。
			{
				//创建一个视频输入空闲帧。
				{
					int p_CreateVdoInptIdleFrmRslt = -1; //存放本处理段执行结果，为0表示成功，为非0表示失败。

					m_VdoInptPt->m_VdoInptFrmElmPt = ( VdoInptFrmElm * )calloc( 1, sizeof( VdoInptFrmElm ) );
					if( m_VdoInptPt->m_VdoInptFrmElmPt == NULL )
					{
						goto OutCreateVdoInptIdleFrm;
					}
					m_VdoInptPt->m_VdoInptFrmElmPt->m_BgraVdoInptSrcFrmPt = ( uint8_t * )malloc( m_VdoInptPt->m_BgraVdoInptSrcFrmLenByt );
					if( m_VdoInptPt->m_VdoInptFrmElmPt->m_BgraVdoInptSrcFrmPt == NULL )
					{
						goto OutCreateVdoInptIdleFrm;
					}
					m_VdoInptPt->m_VdoInptFrmElmPt->m_YU12VdoInptRsltFrmPt = ( uint8_t * )malloc( m_VdoInptPt->m_YU12FrmLenByt );
					if( m_VdoInptPt->m_VdoInptFrmElmPt->m_YU12VdoInptRsltFrmPt == NULL )
					{
						goto OutCreateVdoInptIdleFrm;
					}
					if( m_VdoInptPt->m_UseWhatEncd != 0 )
					{
						m_VdoInptPt->m_VdoInptFrmElmPt->m_EncdVdoInptRsltFrmPt = ( uint8_t * )malloc( m_VdoInptPt->m_YU12FrmLenByt );
						if( m_VdoInptPt->m_VdoInptFrmElmPt->m_EncdVdoInptRsltFrmPt == NULL )
						{
							goto OutCreateVdoInptIdleFrm;
						}
					}

					p_CreateVdoInptIdleFrmRslt = 0; //设置本处理段执行成功。

					OutCreateVdoInptIdleFrm:
					if( p_CreateVdoInptIdleFrmRslt != 0 ) //如果本处理段执行失败。
					{
						if( m_VdoInptPt->m_VdoInptFrmElmPt != NULL )
						{
							if( m_VdoInptPt->m_VdoInptFrmElmPt->m_BgraVdoInptSrcFrmPt != NULL ) free( m_VdoInptPt->m_VdoInptFrmElmPt->m_BgraVdoInptSrcFrmPt );
							if( m_VdoInptPt->m_VdoInptFrmElmPt->m_YU12VdoInptRsltFrmPt != NULL ) free( m_VdoInptPt->m_VdoInptFrmElmPt->m_YU12VdoInptRsltFrmPt );
							if( m_VdoInptPt->m_VdoInptFrmElmPt->m_EncdVdoInptRsltFrmPt != NULL ) free( m_VdoInptPt->m_VdoInptFrmElmPt->m_EncdVdoInptRsltFrmPt );
							free( m_VdoInptPt->m_VdoInptFrmElmPt );
							m_VdoInptPt->m_VdoInptFrmElmPt = NULL;
						}
					}
				}
				
				if( m_VdoInptPt->m_VdoInptFrmElmPt != NULL )
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：视频输入空闲帧链表中没有帧，创建一个视频输入空闲帧成功。" ) );
				}
				else
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：视频输入空闲帧链表中没有帧，创建一个视频输入空闲帧失败。原因：内存不足。" ) );
				}
			}
			else
			{
				if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "视频输入线程：视频输入帧链表中帧数量为%uzd已经超过上限1，不再创建视频输入空闲帧，本次视频输入帧丢弃。" ), m_VdoInptPt->m_FrmLnkLstElmTotal );
			}
        }

		if( m_VdoInptPt->m_VdoInptFrmElmPt != NULL ) //如果获取了一个视频输入空闲帧。
		{
			m_VdoInptPt->m_VdoInptTmpFrmLenByt  = m_VdoInptPt->m_BgraVdoInptSrcFrmLenByt; //设置视频输入临时帧的长度。
			memcpy( m_VdoInptPt->m_VdoInptFrmElmPt->m_BgraVdoInptSrcFrmPt, pBuffer, m_VdoInptPt->m_VdoInptTmpFrmLenByt ); //复制BGRA8888格式视频输入原始帧。
			memcpy( m_VdoInptPt->m_VdoInptTmp1FrmPt, pBuffer, m_VdoInptPt->m_VdoInptTmpFrmLenByt ); //将BGRA8888格式视频输入原始帧复制到视频输入临时帧，方便处理。

			//裁剪BGRA8888格式视频输入原始帧。
			if( m_VdoInptPt->m_BgraVdoInptSrcFrmIsCrop != 0 )
			{
				if( LibYUVPictrCrop( m_VdoInptPt->m_VdoInptTmp1FrmPt, PICTR_FMT_SRGBF8_BGRA8888, m_VdoInptPt->m_BgraVdoInptSrcFrmWidth, m_VdoInptPt->m_BgraVdoInptSrcFrmHeight,
									 m_VdoInptPt->m_BgraVdoInptSrcFrmCropX, m_VdoInptPt->m_BgraVdoInptSrcFrmCropY, m_VdoInptPt->m_BgraVdoInptSrcFrmCropWidth, m_VdoInptPt->m_BgraVdoInptSrcFrmCropHeight,
									 m_VdoInptPt->m_VdoInptTmp2FrmPt, m_VdoInptPt->m_VdoInptTmpFrmSzByt, &m_VdoInptPt->m_VdoInptTmpFrmLenByt , NULL, NULL,
									 NULL ) == 0 )
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：裁剪BGRA8888格式视频输入原始帧成功。" ) );
					SWAPPT( m_VdoInptPt->m_VdoInptTmp1FrmPt, m_VdoInptPt->m_VdoInptTmp2FrmPt ); //交换视频输入临时帧的指针。
				}
				else
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：裁剪BGRA8888格式视频输入原始帧失败，本次视频输入帧丢弃。" ) );
					goto Out;
				}
			}

			//垂直翻转镜像BGRA8888格式视频输入原始帧。因为BGRA8888格式视频输入原始帧是自底向上的。
			{
				if( LibYUVPictrMirror( m_VdoInptPt->m_VdoInptTmp1FrmPt, PICTR_FMT_SRGBF8_BGRA8888, m_VdoInptPt->m_BgraVdoInptSrcFrmCropWidth, m_VdoInptPt->m_BgraVdoInptSrcFrmCropHeight,
									   1,
									   m_VdoInptPt->m_VdoInptTmp2FrmPt, m_VdoInptPt->m_VdoInptTmpFrmSzByt, &m_VdoInptPt->m_VdoInptTmpFrmLenByt , NULL, NULL,
									   NULL ) == 0 )
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：垂直翻转镜像BGRA8888格式视频输入原始帧成功。" ) );
					SWAPPT( m_VdoInptPt->m_VdoInptTmp1FrmPt, m_VdoInptPt->m_VdoInptTmp2FrmPt ); //交换视频输入临时帧的指针。
				}
				else
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：垂直翻转镜像BGRA8888格式视频输入原始帧失败，本次视频输入帧丢弃。" ) );
					goto Out;
				}
			}

			//缩放BGRA8888格式视频输入原始帧。
			if( m_VdoInptPt->m_BgraVdoInptSrcFrmIsScale != 0 )
			{
				if( LibYUVPictrScale( m_VdoInptPt->m_VdoInptTmp1FrmPt, PICTR_FMT_SRGBF8_BGRA8888, m_VdoInptPt->m_BgraVdoInptSrcFrmCropWidth, m_VdoInptPt->m_BgraVdoInptSrcFrmCropHeight,
									  3,
									  m_VdoInptPt->m_VdoInptTmp2FrmPt, m_VdoInptPt->m_VdoInptTmpFrmSzByt, &m_VdoInptPt->m_VdoInptTmpFrmLenByt , m_VdoInptPt->m_BgraVdoInptSrcFrmScaleWidth, m_VdoInptPt->m_BgraVdoInptSrcFrmScaleHeight,
									  NULL ) == 0 )
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：缩放BGRA8888格式视频输入原始帧成功。" ) );
					SWAPPT( m_VdoInptPt->m_VdoInptTmp1FrmPt, m_VdoInptPt->m_VdoInptTmp2FrmPt ); //交换视频输入临时帧的指针。
				}
				else
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：缩放BGRA8888格式视频输入原始帧失败，本次视频输入帧丢弃。" ) );
					goto Out;
				}
			}

			//预览BGRA8888格式视频输入原始帧。
			if( m_VdoInptPt->m_VdoInptPrvwWndHdl != NULL )
			{
				//水平翻转镜像BGRA8888格式视频输入原始帧。
				if( LibYUVPictrMirror( m_VdoInptPt->m_VdoInptTmp1FrmPt, PICTR_FMT_SRGBF8_BGRA8888, m_VdoInptPt->m_BgraVdoInptSrcFrmScaleWidth, m_VdoInptPt->m_BgraVdoInptSrcFrmScaleHeight,
									   0,
									   m_VdoInptPt->m_VdoInptTmp2FrmPt, m_VdoInptPt->m_VdoInptTmpFrmSzByt, &m_VdoInptPt->m_VdoInptTmpFrmLenByt , NULL, NULL,
									   NULL ) == 0 )
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：水平翻转镜像BGRA8888格式视频输入原始帧成功。" ) );
				}
				else
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：水平翻转镜像BGRA8888格式视频输入原始帧失败，本次视频输入帧丢弃。" ) );
					goto Out;
				}

				//绘制BGRA8888格式视频输入原始帧到视频输入预览窗口。
				if( LibYUVPictrDrawToWnd( m_VdoInptPt->m_VdoInptTmp2FrmPt, PICTR_FMT_SRGBF8_BGRA8888, m_VdoInptPt->m_BgraVdoInptSrcFrmScaleWidth, m_VdoInptPt->m_BgraVdoInptSrcFrmScaleHeight,
										  0,
										  m_VdoInptPt->m_VdoInptPrvwWndHdl,
										  NULL ) == 0 )
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：绘制BGRA8888格式视频输入原始帧到视频输入预览窗口成功。" ) );
				}
				else
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：绘制BGRA8888格式视频输入原始帧到视频输入预览窗口失败，本次视频输入帧丢弃。" ) );
					goto Out;
				}
			}

			//将BGRA8888格式视频输入原始帧转为YU12格式视频输入结果帧。
			if( LibYUVPictrFmtCnvrt( m_VdoInptPt->m_VdoInptTmp1FrmPt, PICTR_FMT_SRGBF8_BGRA8888, m_VdoInptPt->m_BgraVdoInptSrcFrmScaleWidth, m_VdoInptPt->m_BgraVdoInptSrcFrmScaleHeight,
									 m_VdoInptPt->m_VdoInptFrmElmPt->m_YU12VdoInptRsltFrmPt, m_VdoInptPt->m_YU12FrmLenByt, NULL, PICTR_FMT_BT601F8_YU12_I420,
									 NULL ) == 0 )
			{
				if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：将BGRA8888格式视频输入原始帧转为YU12格式视频输入结果帧成功。" ) );
			}
			else
			{
				if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：将BGRA8888格式视频输入原始帧转为YU12格式视频输入结果帧失败，本次视频输入帧丢弃。" ) );
				goto Out;
			}
			//FILE * p_File = fopen( "123.yuv", "wb+" ); fwrite( m_VdoInptPt->m_VdoInptFrmElmPt->m_YU12VdoInptRsltFrmPt, 1, m_VdoInptPt->m_YU12FrmLenByt, p_File ); fclose( p_File );
			//FILE * p_File = fopen( "123.yuv", "rb" ); size_t p_SzByt = fread( m_VdoInptPt->m_VdoInptFrmElmPt->m_YU12VdoInptRsltFrmPt, 1, m_VdoInptPt->m_YU12FrmLenByt, p_File ); fclose( p_File );
			
			//判断视频输入是否黑屏。在视频输入处理完后再设置黑屏，这样可以保证视频输入处理器的连续性。
			if( m_VdoInptPt->m_VdoInptIsBlack != 0 )
			{
				int p_TmpLen = m_VdoInptPt->m_FrmWidth * m_VdoInptPt->m_FrmHeight;
				memset( m_VdoInptPt->m_VdoInptFrmElmPt->m_YU12VdoInptRsltFrmPt, 0, p_TmpLen );
				memset( m_VdoInptPt->m_VdoInptFrmElmPt->m_YU12VdoInptRsltFrmPt + p_TmpLen, 128, p_TmpLen / 2 );
				if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：设置YU12格式视频输入结果帧为黑屏成功。" ) );
			}
			
			//使用编码器。
			switch( m_VdoInptPt->m_UseWhatEncd )
			{
				case 0: //如果要使用YU12原始数据。
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：使用YU12原始数据。" ) );
					break;
				}
				case 1: //如果要使用OpenH264编码器。
				{
					if( OpenH264EncdPocs( m_VdoInptPt->m_OpenH264EncdPt,
										  m_VdoInptPt->m_VdoInptFrmElmPt->m_YU12VdoInptRsltFrmPt, m_VdoInptPt->m_FrmWidth, m_VdoInptPt->m_FrmHeight, m_VdoInptPt->m_LastTickMsec,
										  m_VdoInptPt->m_VdoInptFrmElmPt->m_EncdVdoInptRsltFrmPt, m_VdoInptPt->m_YU12FrmLenByt, &m_VdoInptPt->m_VdoInptFrmElmPt->m_EncdVdoInptRsltFrmLenByt,
										  NULL ) == 0 )
					{
						if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输入线程：使用OpenH264编码器成功。H264格式视频输入结果帧的长度：%uzd，时间戳：%uz64d，类型：%hhd。" ), m_VdoInptPt->m_VdoInptFrmElmPt->m_EncdVdoInptRsltFrmLenByt, m_VdoInptPt->m_LastTickMsec, m_VdoInptPt->m_VdoInptFrmElmPt->m_EncdVdoInptRsltFrmPt[ 4 ] );
					}
					else
					{
						if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：使用OpenH264编码器失败，本次视频输入帧丢弃。" ) );
						goto Out;
					}
					break;
				}
			}
			
			m_VdoInptPt->m_VdoInptFrmElmPt->m_TimeStampMsec = m_VdoInptPt->m_LastTickMsec; //设置时间戳。

			//追加本次视频输入帧到视频输入帧链表。
			{
				m_VdoInptPt->m_VdoInptFrmLnkLst.PutTail( &m_VdoInptPt->m_VdoInptFrmElmPt, 1, NULL );
				m_VdoInptPt->m_VdoInptFrmElmPt = NULL;
			}
			
			if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
			{
				m_VdoInptPt->m_NowTickMsec = FuncGetTickAsMsec();
				LOGFI( Cu8vstr( "视频输入线程：本次视频输入帧处理完毕，耗时 %uz64d 毫秒。" ), m_VdoInptPt->m_NowTickMsec - m_VdoInptPt->m_LastTickMsec );
			}
		}

		Out:
		if( m_VdoInptPt->m_VdoInptFrmElmPt != NULL ) //如果获取的视频输入空闲帧没有追加到视频输入帧链表。
		{
			m_VdoInptPt->m_VdoInptIdleFrmLnkLst.PutTail( &m_VdoInptPt->m_VdoInptFrmElmPt, 1, NULL );
			m_VdoInptPt->m_VdoInptFrmElmPt = NULL;
		}
		return 0;
	}
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoInptInit
 * 功能说明：初始化视频输入。
 * 参数说明：参数1名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             参数2名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             ……
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int VdoInptInit( VdoInpt * VdoInptPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	uint64_t p_LastTickMsec = 0;
	uint64_t p_NowTickMsec = 0;
	
	if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) p_LastTickMsec = FuncGetTickAsMsec(); //记录初始化开始的嘀嗒钟。

	//初始化视频输入线程。必须在初始化视频输入设备前初始化视频输入线程，因为视频输入设备会使用视频输入线程的指针。
	{
		VdoInptPt->m_VdoInptThrdPt = new VdoInptThrd( VdoInptPt );
		if( VdoInptPt->m_VdoInptThrdPt != NULL )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化视频输入线程成功。" ) );
		}
		else
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化视频输入线程失败。" ) );
			goto Out;
		}
	}

	//初始化视频输入设备。
	{
		int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
		ICreateDevEnum * p_CreateDevEnumPt = NULL;
		IEnumMoniker * p_EnumMonikerPt = NULL;
		IMoniker * p_MonikerPt = NULL;
		IEnumPins * p_EnumPinsPt = NULL;
		IPin * p_PinPt = NULL;
		PIN_DIRECTION p_PinDirection;
		IEnumMediaTypes * p_EnumMediaTypesPt = NULL;
		AM_MEDIA_TYPE * p_AmMediaTypePt = NULL;

		IBaseFilter * p_VdoInptDvcFilterPt = NULL; //存放视频输入设备过滤器的指针。
		IPin * p_SelPinPt = NULL;
		AM_MEDIA_TYPE * p_SelAmMediaTypePt = NULL;

		IBaseFilter * p_DecFilterPt = NULL;
		IPin * p_DecFilterInptPinPt = NULL;
		IPin * p_DecFilterOtptPinPt = NULL;

		IBaseFilter * p_SmplGrabberFilterPt = NULL;
		ISampleGrabber * p_SmplGrabberInterfacePt = NULL;
		IPin * p_SmplGrabberFilterInptPinPt = NULL;
		IPin * p_SmplGrabberFilterOtptPinPt = NULL;
		AM_MEDIA_TYPE p_TmpAmMediaType = { 0 };

		int p_TmpInt;

		//创建视频输入过滤器图管理器。
		if( CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, ( void * * )&VdoInptPt->m_FilterGraphManagerPt ) != S_OK )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建视频输入过滤器图管理器失败。" ) );
			goto OutInitVdoInptDvc;
		}

		//获取视频输入过滤器图媒体事件器。
		if( VdoInptPt->m_FilterGraphManagerPt->QueryInterface( IID_IMediaEvent, ( LPVOID * )&VdoInptPt->m_FilterGraphMediaEventPt ) != S_OK )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：获取视频输入过滤器图媒体事件器失败。" ) );
			goto OutInitVdoInptDvc;
		}

		//获取视频输入过滤器图媒体控制器。
		if( VdoInptPt->m_FilterGraphManagerPt->QueryInterface( IID_IMediaControl, ( LPVOID * )&VdoInptPt->m_FilterGraphMediaCtrlPt ) != S_OK )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：获取视频输入过滤器图媒体控制器失败。" ) );
			goto OutInitVdoInptDvc;
		}

		//创建系统设备枚举器。
		if( CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, ( void * * )&p_CreateDevEnumPt ) != S_OK )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建系统设备枚举器失败。" ) );
			goto OutInitVdoInptDvc;
		}

		//查找并创建指定的视频输入设备过滤器。
		if( p_CreateDevEnumPt->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, &p_EnumMonikerPt, 0 ) == S_OK )
		{
			for( p_TmpInt = 0; p_EnumMonikerPt->Next( 1, &p_MonikerPt, NULL ) == S_OK; p_TmpInt++ )
			{
				if( p_TmpInt == VdoInptPt->m_VdoInptDvcID )
				{
					if( p_MonikerPt->BindToObject( NULL, NULL, IID_IBaseFilter, ( void * * )&p_VdoInptDvcFilterPt ) == S_OK )
					{
						p_MonikerPt->Release();
						p_MonikerPt = NULL;
						break;
					}
					else
					{
						p_MonikerPt->Release();
						p_MonikerPt = NULL;
						if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：根据Moniker创建视频输入设备过滤器失败。" ) );
						break;
					}
				}
				else
				{
					p_MonikerPt->Release();
					p_MonikerPt = NULL;
				}
			}
			p_EnumMonikerPt->Release();
			p_EnumMonikerPt = NULL;
		}
		p_CreateDevEnumPt->Release();
		p_CreateDevEnumPt = NULL;
		if( p_VdoInptDvcFilterPt == NULL ) //如果创建视频输入设备失败。
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建视频输入设备过滤器失败。原因：可能没有视频输入设备。" ) );
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsShowToast != 0 ) ToastFmt( VdoInptPt->m_MediaPocsThrdPt->m_ShowToastWndHdl, 3000, NULL, Cu8vstr( "媒体处理线程：创建视频输入设备过滤器失败。原因：可能没有视频输入设备。" ) );
			goto OutInitVdoInptDvc;
		}
		if( VdoInptPt->m_FilterGraphManagerPt->AddFilter( p_VdoInptDvcFilterPt, L"Video Capture" ) != S_OK ) //如果添加视频输入设备过滤器到视频输入过滤器图失败。
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：添加视频输入设备过滤器到视频输入过滤器图失败。" ) );
			goto OutInitVdoInptDvc;
		}

		//选择视频输入设备过滤器上合适的引脚和媒体类型。
		if( p_VdoInptDvcFilterPt->EnumPins( &p_EnumPinsPt ) == S_OK ) //创建引脚枚举器。
		{
			int32_t p_TgtVdoInptFrmWidth = VdoInptPt->m_FrmWidth; //存放目标视频输入帧的宽度，单位为像素。
			int32_t p_TgtVdoInptFrmHeight = VdoInptPt->m_FrmHeight; //存放目标视频输入帧的高度，单位为像素。
			double p_TgtFrmWidthToHeightRatio = ( double )VdoInptPt->m_FrmWidth / ( double )VdoInptPt->m_FrmHeight; //存放指定帧的宽高比。
			int32_t p_TgtAvgTimePerFrm = 1000.0 / ( VdoInptPt->m_MaxSmplRate / 10.0 / 1000.0 ); //存放目标视频输入帧的帧间隔时间，单位为100纳秒。
			double p_VdoInptDvcFrmAspectRatio; //存放本次视频输入设备帧的宽高比。
			int32_t p_VdoInptDvcFrmCropX; //存放本次视频输入设备帧裁剪区域左上角的横坐标，单位像素。
			int32_t p_VdoInptDvcFrmCropY; //存放本次视频输入设备帧裁剪区域左上角的纵坐标，单位像素。
			int32_t p_VdoInptDvcFrmCropWidth; //存放本次视频输入设备帧裁剪区域的宽度，单位像素。
			int32_t p_VdoInptDvcFrmCropHeight; //存放本次视频输入设备帧裁剪区域的高度，单位像素。
			int32_t p_IsSetSelCur; //存放是否设置选择的为本次的，为0表示不设置，为非0表示要设置。
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：目标视频输入的媒体格式：帧间隔时间：%z32d，%lf，宽度：%ld，高度：%ld。" ),
																		 p_TgtAvgTimePerFrm, 1000.0 / ( p_TgtAvgTimePerFrm / 10.0 / 1000.0 ),
																		 p_TgtVdoInptFrmWidth, p_TgtVdoInptFrmHeight );

			while( p_EnumPinsPt->Next( 1, &p_PinPt, NULL ) == S_OK ) //遍历引脚。
			{
				if( ( p_PinPt->QueryDirection( &p_PinDirection ) == S_OK ) && ( p_PinDirection == PINDIR_OUTPUT ) ) //查询引脚方向是否为输出。
				{
					if( p_PinPt->EnumMediaTypes( &p_EnumMediaTypesPt ) == S_OK ) //创建引脚的媒体类型枚举器。
					{
						//遍历媒体类型，并智能选择满足目标的媒体类型。
						while( p_EnumMediaTypesPt->Next( 1, &p_AmMediaTypePt, NULL ) == S_OK )
						{
							if( p_AmMediaTypePt->majortype == MEDIATYPE_Video && p_AmMediaTypePt->formattype == FORMAT_VideoInfo )
							{
								#define VIDEOINFOHEADER_AvgTimePerFrm( AmMediaTypePt ) ( ( ( VIDEOINFOHEADER * )AmMediaTypePt->pbFormat )->AvgTimePerFrame )
								#define VIDEOINFOHEADER_AvgTimePerFrmCom( AmMediaType1Pt, Com, AmMediaType2Pt ) ( VIDEOINFOHEADER_AvgTimePerFrm( AmMediaType1Pt ) Com VIDEOINFOHEADER_AvgTimePerFrm( AmMediaType2Pt ) )

								#define VIDEOINFOHEADER_Width( AmMediaTypePt ) ( ( ( VIDEOINFOHEADER * )AmMediaTypePt->pbFormat )->bmiHeader.biWidth )
								#define VIDEOINFOHEADER_Height( AmMediaTypePt ) ( ( ( VIDEOINFOHEADER * )AmMediaTypePt->pbFormat )->bmiHeader.biHeight )
								#define VIDEOINFOHEADER_WidthHeightCom( AmMediaType1Pt, Com, AmMediaType2Pt ) ( ( VIDEOINFOHEADER_Width( AmMediaType1Pt ) Com VIDEOINFOHEADER_Width( AmMediaType2Pt ) ) && ( VIDEOINFOHEADER_Height( AmMediaType1Pt ) Com VIDEOINFOHEADER_Height( AmMediaType2Pt ) ) )

								if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输入设备支持的媒体格式：子类型：%s，帧间隔时间：%lld，%.1f，宽度：%ld，高度：%ld。" ),
																							 ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_MJPG ) ? "MJPEG" : ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_YUY2 ) ? "YUY2" : ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_RGB24 ) ? "RGB24" : "unkown",
																							 VIDEOINFOHEADER_AvgTimePerFrm( p_AmMediaTypePt ), 1000.0 / ( VIDEOINFOHEADER_AvgTimePerFrm( p_AmMediaTypePt ) / 10.0 / 1000.0 ),
																							 VIDEOINFOHEADER_Width( p_AmMediaTypePt ), VIDEOINFOHEADER_Height( p_AmMediaTypePt ) );

								if( ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_MJPG ) || ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_YUY2 ) ) //如果媒体格式为MJPEG或YUY2才进行选择。
								{
									//设置本次视频输入设备帧的宽高比、裁剪宽度、裁剪高度、裁剪区域左上角的坐标。
									p_VdoInptDvcFrmAspectRatio = ( double )VIDEOINFOHEADER_Width( p_AmMediaTypePt ) / ( double )VIDEOINFOHEADER_Height( p_AmMediaTypePt );
									if( p_VdoInptDvcFrmAspectRatio >= p_TgtFrmWidthToHeightRatio ) //如果本次视频输入设备帧的宽高比目标的大，就表示需要裁剪宽度。
									{
										p_VdoInptDvcFrmCropWidth = ( int )( ( double )VIDEOINFOHEADER_Height( p_AmMediaTypePt ) * p_TgtFrmWidthToHeightRatio ); //设置本次视频输入设备帧裁剪区域左上角的宽度，使裁剪区域居中。
										p_VdoInptDvcFrmCropWidth -= p_VdoInptDvcFrmCropWidth % 2;
										p_VdoInptDvcFrmCropHeight = VIDEOINFOHEADER_Height( p_AmMediaTypePt ); //设置本次视频输入设备帧裁剪区域左上角的高度，使裁剪区域居中。

										p_VdoInptDvcFrmCropX = ( VIDEOINFOHEADER_Width( p_AmMediaTypePt ) - p_VdoInptDvcFrmCropWidth ) / 2; //设置本次视频输入设备帧裁剪区域左上角的横坐标，使裁剪区域居中。
										p_VdoInptDvcFrmCropX -= p_VdoInptDvcFrmCropX % 2;
										p_VdoInptDvcFrmCropY = 0; //设置本次视频输入设备帧裁剪区域左上角的纵坐标。
									}
									else //如果本次视频输入设备帧的宽高比目标的小，就表示需要裁剪高度。
									{
										p_VdoInptDvcFrmCropWidth = VIDEOINFOHEADER_Width( p_AmMediaTypePt ); //设置本次视频输入设备帧裁剪区域左上角的宽度，使裁剪区域居中。
										p_VdoInptDvcFrmCropHeight = ( int )( ( double )VIDEOINFOHEADER_Width( p_AmMediaTypePt ) / p_TgtFrmWidthToHeightRatio ); //设置本次视频输入设备帧裁剪区域左上角的高度，使裁剪区域居中。
										p_VdoInptDvcFrmCropHeight -= p_VdoInptDvcFrmCropHeight % 2;

										p_VdoInptDvcFrmCropX = 0; //设置本次视频输入设备帧裁剪区域左上角的横坐标。
										p_VdoInptDvcFrmCropY = ( VIDEOINFOHEADER_Height( p_AmMediaTypePt ) - p_VdoInptDvcFrmCropHeight ) / 2; //设置本次视频输入设备帧裁剪区域左上角的纵坐标，使裁剪区域居中。
										p_VdoInptDvcFrmCropY -= p_VdoInptDvcFrmCropY % 2;
									}

									//如果选择的为空，就设置选择的为本次的。
									//如果选择的帧裁剪区域不满足目标的，则只要本次的帧裁剪区域比选择的大，就设置选择的为本次的。
									//如果选择的帧裁剪区域满足目标的，但帧率不满足目标的，则只要本次的帧裁剪区域满足目标的，且本次的帧率比选择的高，就设置选择的为本次的。
									//如果选择的帧裁剪区域和帧率都满足目标的，就只要本次的帧裁剪区域和帧率都满足目标的，且本次的帧裁剪区域比选择的小、或本次的帧裁剪区域相同但裁剪量比选择的小、或本次的帧裁剪区域和裁剪量都相同但帧率比选择的低，就设置选择的为本次的。
									p_IsSetSelCur = 0;
									if( p_SelAmMediaTypePt == NULL )
									{
										p_IsSetSelCur = 1;
									}
									else if( ( VdoInptPt->m_BgraVdoInptSrcFrmCropWidth < p_TgtVdoInptFrmWidth ) && ( VdoInptPt->m_BgraVdoInptSrcFrmCropHeight < p_TgtVdoInptFrmHeight ) )
									{
										if( ( p_VdoInptDvcFrmCropWidth > VdoInptPt->m_BgraVdoInptSrcFrmCropWidth ) && ( p_VdoInptDvcFrmCropHeight > VdoInptPt->m_BgraVdoInptSrcFrmCropHeight ) )
										{
											p_IsSetSelCur = 1;
										}
									}
									else if( VIDEOINFOHEADER_AvgTimePerFrm( p_SelAmMediaTypePt ) > p_TgtAvgTimePerFrm )
									{
										if( ( p_VdoInptDvcFrmCropWidth >= p_TgtVdoInptFrmWidth ) && ( p_VdoInptDvcFrmCropHeight >= p_TgtVdoInptFrmHeight ) )
										{
											if( VIDEOINFOHEADER_AvgTimePerFrmCom( p_AmMediaTypePt, < , p_SelAmMediaTypePt ) )
											{
												p_IsSetSelCur = 1;
											}
										}
									}
									else
									{
										if( ( p_VdoInptDvcFrmCropWidth >= p_TgtVdoInptFrmWidth ) && ( p_VdoInptDvcFrmCropHeight >= p_TgtVdoInptFrmHeight ) )
										{
											if( VIDEOINFOHEADER_AvgTimePerFrm( p_AmMediaTypePt ) <= p_TgtAvgTimePerFrm )
											{
												if( ( p_VdoInptDvcFrmCropWidth < VdoInptPt->m_BgraVdoInptSrcFrmCropWidth ) && ( p_VdoInptDvcFrmCropHeight < VdoInptPt->m_BgraVdoInptSrcFrmCropHeight ) )
												{
													p_IsSetSelCur = 1;
												}
												else if( ( p_VdoInptDvcFrmCropWidth == VdoInptPt->m_BgraVdoInptSrcFrmCropWidth ) && ( p_VdoInptDvcFrmCropHeight == VdoInptPt->m_BgraVdoInptSrcFrmCropHeight ) )
												{
													if( p_VdoInptDvcFrmCropX + p_VdoInptDvcFrmCropY < VdoInptPt->m_BgraVdoInptSrcFrmCropX + VdoInptPt->m_BgraVdoInptSrcFrmCropY )
													{
														p_IsSetSelCur = 1;
													}
													else if( p_VdoInptDvcFrmCropX + p_VdoInptDvcFrmCropY == VdoInptPt->m_BgraVdoInptSrcFrmCropX + VdoInptPt->m_BgraVdoInptSrcFrmCropY )
													{
														if( VIDEOINFOHEADER_AvgTimePerFrmCom( p_AmMediaTypePt, > , p_SelAmMediaTypePt ) )
														{
															p_IsSetSelCur = 1;
														}
													}
												}
											}
										}
									}
									if( p_IsSetSelCur != 0 ) //如果要设置选择的为本次的。
									{
										if( p_PinPt != NULL )
										{
											if( p_SelPinPt != NULL ) p_SelPinPt->Release();
											p_SelPinPt = p_PinPt;
											p_PinPt = NULL;
										}
										if( p_SelAmMediaTypePt != NULL ) DeleteMediaType( p_SelAmMediaTypePt );
										p_SelAmMediaTypePt = p_AmMediaTypePt;
										p_AmMediaTypePt = NULL;

										VdoInptPt->m_BgraVdoInptSrcFrmWidth = VIDEOINFOHEADER_Width( p_SelAmMediaTypePt );
										VdoInptPt->m_BgraVdoInptSrcFrmHeight = VIDEOINFOHEADER_Height( p_SelAmMediaTypePt );
										VdoInptPt->m_BgraVdoInptSrcFrmLenByt = VdoInptPt->m_BgraVdoInptSrcFrmWidth * VdoInptPt->m_BgraVdoInptSrcFrmHeight * 4;

										VdoInptPt->m_BgraVdoInptSrcFrmCropX = p_VdoInptDvcFrmCropX;
										VdoInptPt->m_BgraVdoInptSrcFrmCropY = p_VdoInptDvcFrmCropY;
										VdoInptPt->m_BgraVdoInptSrcFrmCropWidth = p_VdoInptDvcFrmCropWidth;
										VdoInptPt->m_BgraVdoInptSrcFrmCropHeight = p_VdoInptDvcFrmCropHeight;
									}
								}
								else
								{
									if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：本次视频输入设备支持的媒体格式不是为MJPEG或YUY2，不能被选择。" ) );
								}
							}
							/*else if( p_AmMediaTypePt->majortype == MEDIATYPE_Video && p_AmMediaTypePt->formattype == FORMAT_VideoInfo2 ) //如果当前还没有FORMAT_VideoInfo格式的媒体类型。
							{
								#define VIDEOINFOHEADER2_AvgTimePerFrm( AmMediaTypePt ) ( ( ( VIDEOINFOHEADER2 * )AmMediaTypePt->pbFormat )->AvgTimePerFrame )
								#define VIDEOINFOHEADER2_AvgTimePerFrmCom( AmMediaType1Pt, Com, AmMediaType2Pt ) ( VIDEOINFOHEADER2_AvgTimePerFrm( AmMediaType1Pt ) Com VIDEOINFOHEADER2_AvgTimePerFrm( AmMediaType2Pt ) )

								#define VIDEOINFOHEADER2_Width( AmMediaTypePt ) ( ( ( VIDEOINFOHEADER2 * )AmMediaTypePt->pbFormat )->bmiHeader.biWidth )
								#define VIDEOINFOHEADER2_Height( AmMediaTypePt ) ( ( ( VIDEOINFOHEADER2 * )AmMediaTypePt->pbFormat )->bmiHeader.biHeight )
								#define VIDEOINFOHEADER2_WidthHeightCom( AmMediaType1Pt, Com, AmMediaType2Pt ) ( ( VIDEOINFOHEADER2_Width( AmMediaType1Pt ) Com VIDEOINFOHEADER2_Width( AmMediaType2Pt ) ) && ( VIDEOINFOHEADER2_Height( AmMediaType1Pt ) Com VIDEOINFOHEADER2_Height( AmMediaType2Pt ) ) )

								//如果选择的为空，就设置选择的为本次的。
								//如果选择的帧大小不满足指定的，但是本次的帧大小比选择的高，就设置选择的为本次的。
								//如果本次的帧大小满足指定的（选择的帧大小肯定也满足指定的，如果选择的帧大小不满足指定的，那么就会走上一条判断），或本次的帧大小和选择的相同，且：
									//如果选择的帧率不满足指定的，但是本次的帧率比选择的高，就设置选择的为本次的。
									//如果本次的帧率满足指定的（选择的帧率肯定也满足指定的，如果选择的帧率不满足指定的，那么就会走上一条判断），或本次的帧率和选择的相同，且：
										//如果本次的帧大小比选择的低，或本次的帧率比选择的低，就设置选择的为本次的。
								if( p_SelAmMediaTypePt == NULL )
								{
									p_TmpInt = 1;
								}
								else if( ( ( VIDEOINFOHEADER2_Width( p_SelAmMediaTypePt ) < VdoInptPt->m_FrmWidth ) || ( VIDEOINFOHEADER2_Height( p_SelAmMediaTypePt ) < VdoInptPt->m_FrmHeight ) ) && ( VIDEOINFOHEADER2_WidthHeightCom( p_AmMediaTypePt, > , p_SelAmMediaTypePt ) ) )
								{
									p_TmpInt = 1;
								}
								else if( ( ( VIDEOINFOHEADER2_Width( p_AmMediaTypePt ) >= VdoInptPt->m_FrmWidth ) && ( VIDEOINFOHEADER2_Height( p_AmMediaTypePt ) >= VdoInptPt->m_FrmHeight ) ) || ( VIDEOINFOHEADER2_WidthHeightCom( p_AmMediaTypePt, == , p_SelAmMediaTypePt ) ) )
								{
									if( ( VIDEOINFOHEADER2_AvgTimePerFrm( p_SelAmMediaTypePt ) > p_TgtAvgTimePerFrm ) && ( VIDEOINFOHEADER2_AvgTimePerFrmCom( p_AmMediaTypePt, < , p_SelAmMediaTypePt ) ) )
									{
										p_TmpInt = 1;
									}
									else if( ( VIDEOINFOHEADER2_AvgTimePerFrm( p_AmMediaTypePt ) <= p_TgtAvgTimePerFrm ) || ( VIDEOINFOHEADER2_AvgTimePerFrmCom( p_AmMediaTypePt, == , p_SelAmMediaTypePt ) ) )
									{
										if( ( VIDEOINFOHEADER2_WidthHeightCom( p_AmMediaTypePt, < , p_SelAmMediaTypePt ) ) || ( VIDEOINFOHEADER2_AvgTimePerFrmCom( p_AmMediaTypePt, > , p_SelAmMediaTypePt ) ) )
										{
											p_TmpInt = 1;
										}
									}
								}

								if( p_TmpInt != 0 )
								{
									if( p_SelPinPt != p_PinPt )
									{
										if( p_SelPinPt != NULL ) p_SelPinPt->Release();
										p_SelPinPt = p_PinPt;
									}
									DeleteMediaType( p_SelAmMediaTypePt );
									p_SelAmMediaTypePt = p_AmMediaTypePt;
									p_AmMediaTypePt = NULL;
									VdoInptPt->m_BgraVdoInptSrcFrmWidth = VIDEOINFOHEADER2_Width( p_SelAmMediaTypePt );
									VdoInptPt->m_BgraVdoInptSrcFrmHeight = VIDEOINFOHEADER2_Height( p_SelAmMediaTypePt );
								}
								else
								{
									DeleteMediaType( p_AmMediaTypePt );
									p_AmMediaTypePt = NULL;
								}
							}*/
							if( p_AmMediaTypePt != NULL ) //因为媒体类型可能被选择，所以要判断是否为空。
							{
								DeleteMediaType( p_AmMediaTypePt );
								p_AmMediaTypePt = NULL;
							}
						}
						p_EnumMediaTypesPt->Release();
						p_EnumMediaTypesPt = NULL;
					}
				}
				if( p_PinPt != NULL ) //因为引脚可能被选择，所以要判断是否为空。
				{
					p_PinPt->Release();
					p_PinPt = NULL;
				}
			}
			p_EnumPinsPt->Release();
			p_EnumPinsPt = NULL;
		}
		if( p_SelPinPt == NULL || p_SelAmMediaTypePt == NULL )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：选择视频输入设备过滤器上合适的引脚和媒体类型失败。" ) );
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsShowToast != 0 ) Toast( VdoInptPt->m_MediaPocsThrdPt->m_ShowToastWndHdl, 3000, NULL, Cu8vstr( "媒体处理线程：选择视频输入设备过滤器上合适的引脚和媒体类型失败。" ) );
			goto OutInitVdoInptDvc;
		}
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：选择视频输入设备的媒体格式：子类型：%s，帧间隔时间：%lld，%.1f，宽度：%ld，高度：%ld。" ),
																	 ( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_MJPG ) ? "MJPEG" : ( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_YUY2 ) ? "YUY2" : ( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_RGB24 ) ? "RGB24" : "unkown",
																	 VIDEOINFOHEADER_AvgTimePerFrm( p_SelAmMediaTypePt ), 1000.0 / ( VIDEOINFOHEADER_AvgTimePerFrm( p_SelAmMediaTypePt ) / 10.0 / 1000.0 ),
																	 VIDEOINFOHEADER_Width( p_SelAmMediaTypePt ), VIDEOINFOHEADER_Height( p_SelAmMediaTypePt ) );

		//判断视频输入设备帧是否裁剪。
		if( ( VdoInptPt->m_BgraVdoInptSrcFrmWidth > VdoInptPt->m_BgraVdoInptSrcFrmCropWidth ) || //如果BGRA8888格式视频输入原始帧的宽度比裁剪宽度大，就表示需要裁剪宽度。
			( VdoInptPt->m_BgraVdoInptSrcFrmHeight > VdoInptPt->m_BgraVdoInptSrcFrmCropHeight ) ) //如果BGRA8888格式视频输入原始帧的高度比裁剪高度大，就表示需要裁剪高度。
		{
			VdoInptPt->m_BgraVdoInptSrcFrmIsCrop = 1; //设置BGRA8888格式视频输入原始帧要裁剪。
		}
		else //如果视频输入设备帧的宽度和高度与裁剪宽度和高度一致，就表示不需要裁剪。
		{
			VdoInptPt->m_BgraVdoInptSrcFrmIsCrop = 0; //设置BGRA8888格式视频输入原始帧不裁剪。
		}
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：BGRA8888格式视频输入原始帧是否裁剪：%z32d，左上角的横坐标：%z32d，纵坐标：%z32d，裁剪区域的宽度：%z32d，高度：%z32d。" ), VdoInptPt->m_BgraVdoInptSrcFrmIsCrop, VdoInptPt->m_BgraVdoInptSrcFrmCropX, VdoInptPt->m_BgraVdoInptSrcFrmCropY, VdoInptPt->m_BgraVdoInptSrcFrmCropWidth, VdoInptPt->m_BgraVdoInptSrcFrmCropHeight );

		//判断视频输入设备帧是否缩放。
		if( ( VdoInptPt->m_BgraVdoInptSrcFrmCropWidth != VdoInptPt->m_FrmWidth ) || ( VdoInptPt->m_BgraVdoInptSrcFrmCropHeight != VdoInptPt->m_FrmHeight ) )
		{
			VdoInptPt->m_BgraVdoInptSrcFrmIsScale = 1; //设置BGRA8888格式视频输入原始帧要缩放。
		}
		else
		{
			VdoInptPt->m_BgraVdoInptSrcFrmIsScale = 0; //设置BGRA8888格式视频输入原始帧不缩放。
		}
		VdoInptPt->m_BgraVdoInptSrcFrmScaleWidth = VdoInptPt->m_FrmWidth; //设置BGRA8888格式视频输入原始帧缩放后的宽度。
		VdoInptPt->m_BgraVdoInptSrcFrmScaleHeight = VdoInptPt->m_FrmHeight; //设置视频输入设备帧缩放后的高度。
		VdoInptPt->m_BgraVdoInptSrcFrmScaleLenByt = VdoInptPt->m_BgraVdoInptSrcFrmScaleWidth * VdoInptPt->m_BgraVdoInptSrcFrmScaleHeight * 3 / 2; //设置BGRA8888格式视频输入原始帧缩放后的长度。
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：BGRA8888格式视频输入原始帧是否缩放：%z32d，缩放后的宽度：%z32d，缩放后的高度：%z32d，缩放后的长度：%z32d。" ), VdoInptPt->m_BgraVdoInptSrcFrmIsScale, VdoInptPt->m_BgraVdoInptSrcFrmScaleWidth, VdoInptPt->m_BgraVdoInptSrcFrmScaleHeight, VdoInptPt->m_BgraVdoInptSrcFrmScaleLenByt );

		//创建视频输入设备过滤器上选择的引脚和媒体类型对应的解码过滤器，并连接引脚。
		if( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_MJPG )
		{
			if( CoCreateInstance( CLSID_MjpegDec, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, ( void * * )&p_DecFilterPt ) != S_OK )
			{
				if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建MJPG解码过滤器失败。" ) );
				goto OutInitVdoInptDvc;
			}
		}
		else if( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_YUY2 )
		{
			if( CoCreateInstance( CLSID_AVIDec, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, ( void * * )&p_DecFilterPt ) != S_OK )
			{
				if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建AVI解码过滤器失败。" ) );
				goto OutInitVdoInptDvc;
			}
		}
		{
			p_DecFilterPt->EnumPins( &p_EnumPinsPt );
			while( p_EnumPinsPt->Next( 1, &p_PinPt, NULL ) == S_OK )
			{
				if( p_PinPt->QueryDirection( &p_PinDirection ) == S_OK )
				{
					if( p_PinDirection == PINDIR_INPUT )
					{
						p_DecFilterInptPinPt = p_PinPt;
					}
					else if( p_PinDirection == PINDIR_OUTPUT )
					{
						p_DecFilterOtptPinPt = p_PinPt;
					}
				}
				p_PinPt = NULL;
			}
			p_EnumPinsPt->Release();
			p_EnumPinsPt = NULL;
		}
		if( VdoInptPt->m_FilterGraphManagerPt->AddFilter( p_DecFilterPt, L"Decompressor" ) != S_OK ) //如果添加解码过滤器到视频输入过滤器图失败。
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：添加解码过滤器到视频输入过滤器图失败。" ) );
			goto OutInitVdoInptDvc;
		}
		if( VdoInptPt->m_FilterGraphManagerPt->ConnectDirect( p_SelPinPt, p_DecFilterInptPinPt, p_SelAmMediaTypePt ) != S_OK ) //如果连接引脚失败。
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：连接视频输入设备过滤器与解码过滤器的引脚失败。" ) );
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsShowToast != 0 ) ToastFmt( VdoInptPt->m_MediaPocsThrdPt->m_ShowToastWndHdl, 3000, NULL, Cu8vstr( "媒体处理线程：连接视频输入设备过滤器与解码过滤器的引脚失败。原因：可能是视频输入设备无法正常工作。" ) );
			goto OutInitVdoInptDvc;
		}

		//创建采样抓取过滤器，并连接引脚。
		if( CoCreateInstance( CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, ( void * * )&p_SmplGrabberFilterPt ) != S_OK )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建采样抓取过滤器失败。" ) );
			goto OutInitVdoInptDvc;
		}
		{
			p_SmplGrabberFilterPt->EnumPins( &p_EnumPinsPt );
			while( p_EnumPinsPt->Next( 1, &p_PinPt, NULL ) == S_OK )
			{
				if( p_PinPt->QueryDirection( &p_PinDirection ) == S_OK )
				{
					if( p_PinDirection == PINDIR_INPUT )
					{
						p_SmplGrabberFilterInptPinPt = p_PinPt;
					}
					else if( p_PinDirection == PINDIR_OUTPUT )
					{
						p_SmplGrabberFilterOtptPinPt = p_PinPt;
					}
				}
				p_PinPt = NULL;
			}
			p_EnumPinsPt->Release();
			p_EnumPinsPt = NULL;
		}
		if( VdoInptPt->m_FilterGraphManagerPt->AddFilter( p_SmplGrabberFilterPt, L"Sample Grabber" ) != S_OK ) //如果采样抓取过滤器到视频输入过滤器图失败。
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：添加采样抓取过滤器到视频输入过滤器图失败。" ) );
			goto OutInitVdoInptDvc;
		}
		p_TmpAmMediaType.majortype = MEDIATYPE_Video;
		//p_TmpAmMediaType.subtype = MEDIASUBTYPE_RGB555; //PICTR_FMT_SRGBF8_BGR555。
		//p_TmpAmMediaType.subtype = MEDIASUBTYPE_RGB565; //PICTR_FMT_SRGBF8_BGR565。
		//p_TmpAmMediaType.subtype = MEDIASUBTYPE_RGB24; //PICTR_FMT_SRGBF8_BGR888。
		p_TmpAmMediaType.subtype = MEDIASUBTYPE_RGB32; //PICTR_FMT_SRGBF8_BGRA8888。
		if( VdoInptPt->m_FilterGraphManagerPt->ConnectDirect( p_DecFilterOtptPinPt, p_SmplGrabberFilterInptPinPt, &p_TmpAmMediaType ) != S_OK ) //如果连接引脚失败。
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：连接解码过滤器与采样抓取过滤器的引脚失败。" ) );
			goto OutInitVdoInptDvc;
		}

		//设置采样抓取过滤器的回调函数。
		if( p_SmplGrabberFilterPt->QueryInterface( IID_ISampleGrabber, ( void * * )&p_SmplGrabberInterfacePt ) != S_OK )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：获取采样抓取过滤器的回调接口失败。" ) );
			goto OutInitVdoInptDvc;
		}
		if( p_SmplGrabberInterfacePt->SetCallback( VdoInptPt->m_VdoInptThrdPt, 1 ) != S_OK )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：设置采样抓取过滤器的回调函数失败。" ) );
			goto OutInitVdoInptDvc;
		}

		//设置视频输入过滤器图管理器开始运行。
		if( VdoInptPt->m_FilterGraphMediaCtrlPt->Run() != S_OK )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：设置视频输入过滤器图管理器开始运行失败。" ) );
			goto OutInitVdoInptDvc;
		}

		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化视频输入设备成功。" ) );

		p_Rslt = 0; //设置本函数执行成功。

		OutInitVdoInptDvc:
		if( p_VdoInptDvcFilterPt != NULL )
		{
			p_VdoInptDvcFilterPt->Release();
			p_VdoInptDvcFilterPt = NULL;
		}
		if( p_SelPinPt != NULL )
		{
			p_SelPinPt->Release();
			p_SelPinPt = NULL;
		}
		if( p_SelAmMediaTypePt != NULL )
		{
			DeleteMediaType( p_SelAmMediaTypePt );
			p_SelAmMediaTypePt = NULL;
		}
		if( p_DecFilterPt != NULL )
		{
			p_DecFilterPt->Release();
			p_DecFilterPt = NULL;
		}
		if( p_DecFilterInptPinPt != NULL )
		{
			p_DecFilterInptPinPt->Release();
			p_DecFilterInptPinPt = NULL;
		}
		if( p_DecFilterOtptPinPt != NULL )
		{
			p_DecFilterOtptPinPt->Release();
			p_DecFilterOtptPinPt = NULL;
		}
		if( p_SmplGrabberFilterPt != NULL )
		{
			p_SmplGrabberFilterPt->Release();
			p_SmplGrabberFilterPt = NULL;
		}
		if( p_SmplGrabberInterfacePt != NULL )
		{
			p_SmplGrabberInterfacePt->Release();
			p_SmplGrabberInterfacePt = NULL;
		}
		if( p_SmplGrabberFilterInptPinPt != NULL )
		{
			p_SmplGrabberFilterInptPinPt->Release();
			p_SmplGrabberFilterInptPinPt = NULL;
		}
		if( p_SmplGrabberFilterOtptPinPt != NULL )
		{
			p_SmplGrabberFilterOtptPinPt->Release();
			p_SmplGrabberFilterOtptPinPt = NULL;
		}
		if( p_Rslt != 0 ) //如果本函数执行失败。
		{
			goto Out;
		}
	} //初始化视频输入设备完毕。

	//初始化视频输入线程的临时变量。
	{
		VdoInptPt->m_IsInitVdoInptThrdTmpVar = 1; //设置已初始化视频输入线程的临时变量。
		VdoInptPt->m_LastVdoInptFrmTickMsec = 0; //初始化上一个视频输入帧的嘀嗒钟。
		VdoInptPt->m_VdoInptFrmTimeStepMsec = 1000.0 / VdoInptPt->m_MaxSmplRate; //初始化视频输入帧的时间步进。
		if( VdoInptPt->m_FrmWidth * VdoInptPt->m_FrmHeight >= VdoInptPt->m_BgraVdoInptSrcFrmWidth * VdoInptPt->m_BgraVdoInptSrcFrmHeight ) //如果视频输入帧指定的尺寸大于等于BGRA8888格式视频输入原始帧的尺寸。
		{
			VdoInptPt->m_VdoInptTmpFrmSzByt = VdoInptPt->m_FrmWidth * VdoInptPt->m_FrmHeight * 4; //初始化视频输入临时帧的大小。
		}
		else //如果视频输入帧指定的尺寸小于BGRA8888格式视频输入原始帧的尺寸。
		{
			VdoInptPt->m_VdoInptTmpFrmSzByt = VdoInptPt->m_BgraVdoInptSrcFrmLenByt; //初始化视频输入临时帧的大小。
		}
		VdoInptPt->m_VdoInptTmp1FrmPt = ( uint8_t * )malloc( VdoInptPt->m_VdoInptTmpFrmSzByt ); //初始化视频输入临时帧的指针。
		if( VdoInptPt->m_VdoInptTmp1FrmPt == NULL )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建视频输入临时帧的内存块失败。" ) );
			goto Out;
		}
		VdoInptPt->m_VdoInptTmp2FrmPt = ( uint8_t * )malloc( VdoInptPt->m_VdoInptTmpFrmSzByt ); //初始化视频输入临时帧的指针。
		if( VdoInptPt->m_VdoInptTmp2FrmPt == NULL )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建视频输入临时帧的内存块失败。" ) );
			goto Out;
		}
		VdoInptPt->m_VdoInptTmpFrmLenByt  = 0; //初始化视频输入临时帧的长度。
		VdoInptPt->m_VdoInptFrmElmPt = NULL; //初始化视频输入帧元素的指针。
		VdoInptPt->m_FrmLnkLstElmTotal = 0; //初始化帧链表的元素总数。
		VdoInptPt->m_LastTickMsec = 0; //初始化上次的嘀嗒钟。
		VdoInptPt->m_NowTickMsec = 0; //初始化本次的嘀嗒钟。
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：初始化视频输入线程的临时变量成功。" ) );
	}

	//初始化编码器。
	switch( VdoInptPt->m_UseWhatEncd )
	{
		case 0: //如果要使用YU12原始数据。
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：初始化YU12原始数据成功。" ) );
			break;
		}
		case 1: //如果要使用OpenH264编码器。
		{
			if( OpenH264EncdInit( &VdoInptPt->m_OpenH264EncdPt, VdoInptPt->m_FrmWidth, VdoInptPt->m_FrmHeight, VdoInptPt->m_OpenH264EncdVdoType, VdoInptPt->m_OpenH264EncdEncdBitrate, VdoInptPt->m_OpenH264EncdBitrateCtrlMode, VdoInptPt->m_MaxSmplRate, VdoInptPt->m_OpenH264EncdIDRFrmIntvl, VdoInptPt->m_OpenH264EncdCmplxt, VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：初始化OpenH264编码器成功。" ) );
			}
			else
			{
				if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化OpenH264编码器失败。原因：%vs" ), VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
			break;
		}
	}

	//初始化视频输入帧链表。
	if( VdoInptPt->m_VdoInptFrmLnkLst.Init( sizeof( VdoInptFrmElm * ), BufAutoAdjMethFreeNumber, 1, 0, 1, SIZE_MAX, VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
	{
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化视频输入帧链表成功。" ) );
	}
	else
	{
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化视频输入帧链表失败。原因：%vs" ), VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	//初始化视频输入空闲帧链表。
	if( VdoInptPt->m_VdoInptIdleFrmLnkLst.Init( sizeof( VdoInptFrmElm * ), BufAutoAdjMethFreeNumber, 1, 0, 1, SIZE_MAX, VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
	{
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化视频输入空闲帧链表成功。" ) );
	}
	else
	{
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化视频输入空闲帧链表失败。原因：%vs" ), VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
	{
		p_NowTickMsec = FuncGetTickAsMsec(); //记录初始化结束的嘀嗒钟。
		LOGFI( Cu8vstr( "媒体处理线程：初始化视频输入耗时 %uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
	}

	p_Rslt = 0; //设置本函数执行成功。
	
	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		VdoInptDstoy( VdoInptPt );
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoInptDstoy
 * 功能说明：销毁视频输入。
 * 参数说明：参数1名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             参数2名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             ……
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoInptDstoy( VdoInpt * VdoInptPt )
{
	//销毁视频输入设备。
	if( VdoInptPt->m_FilterGraphMediaCtrlPt != NULL )
	{
		VdoInptPt->m_FilterGraphMediaCtrlPt->Stop(); //让视频输入过滤器图管理器停止运行。
		VdoInptPt->m_FilterGraphMediaCtrlPt->Release();
		VdoInptPt->m_FilterGraphMediaCtrlPt = NULL;
	}
	if( VdoInptPt->m_FilterGraphMediaEventPt != NULL )
	{
		VdoInptPt->m_FilterGraphMediaEventPt->Release();
		VdoInptPt->m_FilterGraphMediaEventPt = NULL;
	}
	if( VdoInptPt->m_FilterGraphManagerPt != NULL )
	{
		VdoInptPt->m_FilterGraphManagerPt->Release();
		VdoInptPt->m_FilterGraphManagerPt = NULL;
		VdoInptPt->m_BgraVdoInptSrcFrmWidth = 0;
		VdoInptPt->m_BgraVdoInptSrcFrmHeight = 0;
		VdoInptPt->m_BgraVdoInptSrcFrmLenByt = 0;
		VdoInptPt->m_BgraVdoInptSrcFrmIsCrop = 0;
		VdoInptPt->m_BgraVdoInptSrcFrmCropX = 0;
		VdoInptPt->m_BgraVdoInptSrcFrmCropY = 0;
		VdoInptPt->m_BgraVdoInptSrcFrmCropWidth = 0;
		VdoInptPt->m_BgraVdoInptSrcFrmCropHeight = 0;
		VdoInptPt->m_BgraVdoInptSrcFrmIsScale = 0;
		VdoInptPt->m_BgraVdoInptSrcFrmScaleWidth = 0;
		VdoInptPt->m_BgraVdoInptSrcFrmScaleHeight = 0;
		VdoInptPt->m_BgraVdoInptSrcFrmScaleLenByt = 0;
		InvalidateRect( VdoInptPt->m_VdoInptPrvwWndHdl, NULL, TRUE ); //重绘视频输入预览窗口。
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁视频输入设备成功。" ) );
	}

	//销毁视频输入线程。
	if( VdoInptPt->m_VdoInptThrdPt != NULL )
	{
		delete VdoInptPt->m_VdoInptThrdPt;
		VdoInptPt->m_VdoInptThrdPt = NULL;
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁视频输入线程成功。" ) );
	}

	//销毁视频输入线程的临时变量。
	if( VdoInptPt->m_IsInitVdoInptThrdTmpVar != 0 )
	{
		VdoInptPt->m_IsInitVdoInptThrdTmpVar = 0; //设置未初始化视频输入线程的临时变量。
		VdoInptPt->m_LastVdoInptFrmTickMsec = 0; //销毁上一个视频输入帧的嘀嗒钟。
		VdoInptPt->m_VdoInptFrmTimeStepMsec = 0; //销毁视频输入帧的时间步进。
		if( VdoInptPt->m_VdoInptTmp1FrmPt != NULL ) //销毁视频临时帧的指针。
		{
			free( VdoInptPt->m_VdoInptTmp1FrmPt );
			VdoInptPt->m_VdoInptTmp1FrmPt = NULL;
		}
		if( VdoInptPt->m_VdoInptTmp2FrmPt != NULL ) //销毁视频临时帧的指针。
		{
			free( VdoInptPt->m_VdoInptTmp2FrmPt );
			VdoInptPt->m_VdoInptTmp2FrmPt = NULL;
		}
		VdoInptPt->m_VdoInptTmpFrmSzByt = 0; //销毁视频输入临时帧的大小。
		VdoInptPt->m_VdoInptTmpFrmLenByt  = 0; //销毁视频输入临时帧的长度。
		VdoInptPt->m_VdoInptFrmElmPt = NULL; //销毁视频输入帧元素的指针。
		VdoInptPt->m_FrmLnkLstElmTotal = 0; //销毁帧链表的元素总数。
		VdoInptPt->m_LastTickMsec = 0; //销毁上次的嘀嗒钟。
		VdoInptPt->m_NowTickMsec = 0; //销毁本次的嘀嗒钟。
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁视频输入线程的临时变量成功。" ) );
	}

	//销毁视频输入空闲帧链表。
	if( VdoInptPt->m_VdoInptIdleFrmLnkLst.m_ConstLenLnkLstPt != NULL )
	{
		while( VdoInptPt->m_VdoInptIdleFrmLnkLst.GetHead( NULL, &VdoInptPt->m_VdoInptFrmElmPt, NULL, 0, NULL ) == 0 )
		{
			if( VdoInptPt->m_VdoInptFrmElmPt->m_BgraVdoInptSrcFrmPt != NULL )
			{
				free( VdoInptPt->m_VdoInptFrmElmPt->m_BgraVdoInptSrcFrmPt );
				VdoInptPt->m_VdoInptFrmElmPt->m_BgraVdoInptSrcFrmPt = NULL;
			}
			if( VdoInptPt->m_VdoInptFrmElmPt->m_YU12VdoInptRsltFrmPt )
			{
				free( VdoInptPt->m_VdoInptFrmElmPt->m_YU12VdoInptRsltFrmPt );
				VdoInptPt->m_VdoInptFrmElmPt->m_YU12VdoInptRsltFrmPt = NULL;
			}
			if( VdoInptPt->m_VdoInptFrmElmPt->m_EncdVdoInptRsltFrmPt != NULL )
			{
				free( VdoInptPt->m_VdoInptFrmElmPt->m_EncdVdoInptRsltFrmPt );
				VdoInptPt->m_VdoInptFrmElmPt->m_EncdVdoInptRsltFrmPt = NULL;
			}
			free( VdoInptPt->m_VdoInptFrmElmPt );
			VdoInptPt->m_VdoInptFrmElmPt = NULL;
			VdoInptPt->m_VdoInptIdleFrmLnkLst.DelHead( 0, NULL );
		}
		if( VdoInptPt->m_VdoInptIdleFrmLnkLst.Dstoy( VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：销毁视频输入空闲帧链表成功。" ) );
		}
		else
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁视频输入空闲帧链表失败。原因：%vs" ), VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
	}

	//销毁视频输入帧链表。
	if( VdoInptPt->m_VdoInptFrmLnkLst.m_ConstLenLnkLstPt != NULL )
	{
		while( VdoInptPt->m_VdoInptFrmLnkLst.GetHead( NULL, &VdoInptPt->m_VdoInptFrmElmPt, NULL, 0, NULL ) == 0 )
		{
			if( VdoInptPt->m_VdoInptFrmElmPt->m_BgraVdoInptSrcFrmPt )
			{
				free( VdoInptPt->m_VdoInptFrmElmPt->m_BgraVdoInptSrcFrmPt );
				VdoInptPt->m_VdoInptFrmElmPt->m_BgraVdoInptSrcFrmPt = NULL;
			}
			if( VdoInptPt->m_VdoInptFrmElmPt->m_YU12VdoInptRsltFrmPt )
			{
				free( VdoInptPt->m_VdoInptFrmElmPt->m_YU12VdoInptRsltFrmPt );
				VdoInptPt->m_VdoInptFrmElmPt->m_YU12VdoInptRsltFrmPt = NULL;
			}
			if( VdoInptPt->m_VdoInptFrmElmPt->m_EncdVdoInptRsltFrmPt != NULL )
			{
				free( VdoInptPt->m_VdoInptFrmElmPt->m_EncdVdoInptRsltFrmPt );
				VdoInptPt->m_VdoInptFrmElmPt->m_EncdVdoInptRsltFrmPt = NULL;
			}
			free( VdoInptPt->m_VdoInptFrmElmPt );
			VdoInptPt->m_VdoInptFrmElmPt = NULL;
			VdoInptPt->m_VdoInptFrmLnkLst.DelHead( 0, NULL );
		}
		if( VdoInptPt->m_VdoInptFrmLnkLst.Dstoy( VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：销毁视频输入帧链表成功。" ) );
		}
		else
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁视频输入帧链表失败。原因：%vs" ), VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
	}

	//销毁编码器。
	switch( VdoInptPt->m_UseWhatEncd )
	{
		case 0: //如果要使用YU12原始数据。
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：销毁YU12原始数据成功。" ) );
			break;
		}
		case 1: //如果要使用OpenH264编码器。
		{
			if( VdoInptPt->m_OpenH264EncdPt != NULL )
			{
				if( OpenH264EncdDstoy( VdoInptPt->m_OpenH264EncdPt, NULL ) == 0 )
				{
					if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：销毁OpenH264编码器成功。" ) );
				}
				else
				{
					if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁OpenH264编码器失败。" ) );
				}
				VdoInptPt->m_OpenH264EncdPt = NULL;
			}
			break;
		}
	}
}
