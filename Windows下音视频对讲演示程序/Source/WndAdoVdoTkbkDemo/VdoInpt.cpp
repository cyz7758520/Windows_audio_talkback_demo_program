#include "VdoInpt.h"
#include "qedit.h"
#include "MediaPocsThrd.h"

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
		if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输入线程：读取一个Bgra8888格式原始帧。" ) );

		{
			//丢弃采样频率过快的Bgra8888格式原始帧。
			m_VdoInptPt->m_Thrd.m_LastTickMsec = FuncGetTickAsMsec();
			if( m_VdoInptPt->m_Thrd.m_LastFrmTickMsec != 0 ) //如果已经设置过上一个帧的嘀嗒钟。
			{
				if( m_VdoInptPt->m_Thrd.m_LastTickMsec - m_VdoInptPt->m_Thrd.m_LastFrmTickMsec >= m_VdoInptPt->m_Thrd.m_FrmTimeStepMsec )
				{
					m_VdoInptPt->m_Thrd.m_LastFrmTickMsec += m_VdoInptPt->m_Thrd.m_FrmTimeStepMsec;
				}
				else
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输入线程：采样频率过快，本次帧丢弃。" ) );
					goto OutPocs;
				}
			}
			else //如果没有设置过上一个帧的嘀嗒钟。
			{
				m_VdoInptPt->m_Thrd.m_LastFrmTickMsec = m_VdoInptPt->m_Thrd.m_LastTickMsec;
			}

			//获取一个空闲帧。
			m_VdoInptPt->m_IdleFrmCntnr.GetTotal( &m_VdoInptPt->m_Thrd.m_ElmTotal, 1, NULL ); //获取空闲帧容器的元素总数。
			if( m_VdoInptPt->m_Thrd.m_ElmTotal > 0 ) //如果空闲帧容器中有帧。
			{
				m_VdoInptPt->m_IdleFrmCntnr.GetHead( &m_VdoInptPt->m_Thrd.m_FrmPt, NULL, 1, 1, NULL ); //从空闲帧容器中取出并删除第一个帧。
				if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )  LOGFI( Cu8vstr( "视频输入线程：从空闲帧容器中取出并删除第一个帧，空闲帧容器元素总数：%uzd。" ), m_VdoInptPt->m_Thrd.m_ElmTotal );
			}
			else //如果空闲帧容器中没有帧。
			{
				m_VdoInptPt->m_FrmCntnr.GetTotal( &m_VdoInptPt->m_Thrd.m_ElmTotal, 1, NULL ); //获取帧容器的元素总数。
				if( m_VdoInptPt->m_Thrd.m_ElmTotal <= 20 )
				{
					//创建一个空闲帧。
					{
						int p_CreateVdoInptIdleFrmRslt = -1; //存放本处理段执行结果，为0表示成功，为非0表示失败。

						m_VdoInptPt->m_Thrd.m_FrmPt = ( VdoInpt::Frm * )calloc( 1, sizeof( VdoInpt::Frm ) );
						if( m_VdoInptPt->m_Thrd.m_FrmPt == NULL )
						{
							goto OutCreateVdoInptIdleFrm;
						}
						m_VdoInptPt->m_Thrd.m_FrmPt->m_BgraSrcFrmPt = ( uint8_t * )malloc( m_VdoInptPt->m_Dvc.m_BgraSrcFrmLenByt );
						if( m_VdoInptPt->m_Thrd.m_FrmPt->m_BgraSrcFrmPt == NULL )
						{
							goto OutCreateVdoInptIdleFrm;
						}
						m_VdoInptPt->m_Thrd.m_FrmPt->m_Yu12RsltFrmPt = ( uint8_t * )malloc( m_VdoInptPt->m_Yu12FrmLenByt );
						if( m_VdoInptPt->m_Thrd.m_FrmPt->m_Yu12RsltFrmPt == NULL )
						{
							goto OutCreateVdoInptIdleFrm;
						}
						if( m_VdoInptPt->m_UseWhatEncd != 0 )
						{
							m_VdoInptPt->m_Thrd.m_FrmPt->m_EncdRsltFrmPt = ( uint8_t * )malloc( m_VdoInptPt->m_Yu12FrmLenByt );
							if( m_VdoInptPt->m_Thrd.m_FrmPt->m_EncdRsltFrmPt == NULL )
							{
								goto OutCreateVdoInptIdleFrm;
							}
						}

						p_CreateVdoInptIdleFrmRslt = 0; //设置本处理段执行成功。

						OutCreateVdoInptIdleFrm:
						if( p_CreateVdoInptIdleFrmRslt != 0 ) //如果本处理段执行失败。
						{
							if( m_VdoInptPt->m_Thrd.m_FrmPt != NULL )
							{
								if( m_VdoInptPt->m_Thrd.m_FrmPt->m_BgraSrcFrmPt != NULL ) free( m_VdoInptPt->m_Thrd.m_FrmPt->m_BgraSrcFrmPt );
								if( m_VdoInptPt->m_Thrd.m_FrmPt->m_Yu12RsltFrmPt != NULL ) free( m_VdoInptPt->m_Thrd.m_FrmPt->m_Yu12RsltFrmPt );
								if( m_VdoInptPt->m_Thrd.m_FrmPt->m_EncdRsltFrmPt != NULL ) free( m_VdoInptPt->m_Thrd.m_FrmPt->m_EncdRsltFrmPt );
								free( m_VdoInptPt->m_Thrd.m_FrmPt );
								m_VdoInptPt->m_Thrd.m_FrmPt = NULL;
							}
						}
					}

					if( m_VdoInptPt->m_Thrd.m_FrmPt != NULL )
					{
						if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：空闲帧容器中没有帧，创建一个空闲帧成功。" ) );
					}
					else
					{
						if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：空闲帧容器中没有帧，创建一个空闲帧失败。原因：内存不足。" ) );
						goto OutPocs;
					}
				}
				else
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "视频输入线程：帧容器中帧总数为%uzd已经超过上限20，不再创建空闲帧，本次帧丢弃。" ), m_VdoInptPt->m_Thrd.m_ElmTotal );
					goto OutPocs;
				}
			}

			{
				memcpy( m_VdoInptPt->m_Thrd.m_FrmPt->m_BgraSrcFrmPt, pBuffer, m_VdoInptPt->m_Dvc.m_BgraSrcFrmLenByt ); //复制Bgra8888格式原始帧。
				memcpy( m_VdoInptPt->m_Thrd.m_TmpFrm1Pt, pBuffer, m_VdoInptPt->m_Dvc.m_BgraSrcFrmLenByt ); //将Bgra8888格式原始帧复制到临时帧，方便处理。
			}

			//裁剪Bgra8888格式原始帧。
			if( m_VdoInptPt->m_Dvc.m_BgraSrcFrmIsCrop != 0 )
			{
				if( LibYUVPictrCrop( m_VdoInptPt->m_Thrd.m_TmpFrm1Pt, PictrFmtSrgbF8Bgra8888, m_VdoInptPt->m_Dvc.m_BgraSrcFrmWidth, m_VdoInptPt->m_Dvc.m_BgraSrcFrmHeight,
									 m_VdoInptPt->m_Dvc.m_BgraSrcFrmCropX, m_VdoInptPt->m_Dvc.m_BgraSrcFrmCropY, m_VdoInptPt->m_Dvc.m_BgraSrcFrmCropWidth, m_VdoInptPt->m_Dvc.m_BgraSrcFrmCropHeight,
									 m_VdoInptPt->m_Thrd.m_TmpFrm2Pt, m_VdoInptPt->m_Thrd.m_TmpFrmSzByt, &m_VdoInptPt->m_Thrd.m_TmpFrmLenByt, NULL, NULL,
									 NULL ) == 0 )
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：裁剪Bgra8888格式原始帧成功。" ) );
					SWAPPT( m_VdoInptPt->m_Thrd.m_TmpFrm1Pt, m_VdoInptPt->m_Thrd.m_TmpFrm2Pt ); //交换临时帧的指针。
				}
				else
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：裁剪Bgra8888格式原始帧失败，本次帧丢弃。" ) );
					goto OutPocs;
				}
			}

			//垂直翻转镜像Bgra8888格式原始帧。因为Bgra8888格式原始帧是自底向上的。
			{
				if( LibYUVPictrMirror( m_VdoInptPt->m_Thrd.m_TmpFrm1Pt, PictrFmtSrgbF8Bgra8888, m_VdoInptPt->m_Dvc.m_BgraSrcFrmCropWidth, m_VdoInptPt->m_Dvc.m_BgraSrcFrmCropHeight,
									   1,
									   m_VdoInptPt->m_Thrd.m_TmpFrm2Pt, m_VdoInptPt->m_Thrd.m_TmpFrmSzByt, &m_VdoInptPt->m_Thrd.m_TmpFrmLenByt, NULL, NULL,
									   NULL ) == 0 )
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：垂直翻转镜像Bgra8888格式原始帧成功。" ) );
					SWAPPT( m_VdoInptPt->m_Thrd.m_TmpFrm1Pt, m_VdoInptPt->m_Thrd.m_TmpFrm2Pt ); //交换临时帧的指针。
				}
				else
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：垂直翻转镜像Bgra8888格式原始帧失败，本次帧丢弃。" ) );
					goto OutPocs;
				}
			}

			//缩放Bgra8888格式原始帧。
			if( m_VdoInptPt->m_Dvc.m_BgraSrcFrmIsScale != 0 )
			{
				if( LibYUVPictrScale( m_VdoInptPt->m_Thrd.m_TmpFrm1Pt, PictrFmtSrgbF8Bgra8888, m_VdoInptPt->m_Dvc.m_BgraSrcFrmCropWidth, m_VdoInptPt->m_Dvc.m_BgraSrcFrmCropHeight,
									  3,
									  m_VdoInptPt->m_Thrd.m_TmpFrm2Pt, m_VdoInptPt->m_Thrd.m_TmpFrmSzByt, &m_VdoInptPt->m_Thrd.m_TmpFrmLenByt, m_VdoInptPt->m_Dvc.m_BgraSrcFrmScaleWidth, m_VdoInptPt->m_Dvc.m_BgraSrcFrmScaleHeight,
									  NULL ) == 0 )
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：缩放Bgra8888格式原始帧成功。" ) );
					SWAPPT( m_VdoInptPt->m_Thrd.m_TmpFrm1Pt, m_VdoInptPt->m_Thrd.m_TmpFrm2Pt ); //交换临时帧的指针。
				}
				else
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：缩放Bgra8888格式原始帧失败，本次帧丢弃。" ) );
					goto OutPocs;
				}
			}

			//预览Bgra8888格式原始帧。
			if( m_VdoInptPt->m_Dvc.m_PrvwWndHdl != NULL )
			{
				//水平翻转镜像Bgra8888格式原始帧。
				if( LibYUVPictrMirror( m_VdoInptPt->m_Thrd.m_TmpFrm1Pt, PictrFmtSrgbF8Bgra8888, m_VdoInptPt->m_Dvc.m_BgraSrcFrmScaleWidth, m_VdoInptPt->m_Dvc.m_BgraSrcFrmScaleHeight,
									   0,
									   m_VdoInptPt->m_Thrd.m_TmpFrm2Pt, m_VdoInptPt->m_Thrd.m_TmpFrmSzByt, &m_VdoInptPt->m_Thrd.m_TmpFrmLenByt, NULL, NULL,
									   NULL ) == 0 )
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：水平翻转镜像Bgra8888格式原始帧成功。" ) );
				}
				else
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：水平翻转镜像Bgra8888格式原始帧失败，本次帧丢弃。" ) );
					goto OutPocs;
				}

				//绘制Bgra8888格式原始帧到预览窗口。
				if( LibYUVPictrDrawToWnd( m_VdoInptPt->m_Thrd.m_TmpFrm2Pt, PictrFmtSrgbF8Bgra8888, m_VdoInptPt->m_Dvc.m_BgraSrcFrmScaleWidth, m_VdoInptPt->m_Dvc.m_BgraSrcFrmScaleHeight,
										  0,
										  m_VdoInptPt->m_Dvc.m_PrvwWndHdl,
										  NULL ) == 0 )
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：绘制Bgra8888格式原始帧到预览窗口成功。" ) );
				}
				else
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：绘制Bgra8888格式原始帧到预览窗口失败，本次帧丢弃。" ) );
					goto OutPocs;
				}
			}

			//将Bgra8888格式原始帧转为Yu12格式结果帧。
			if( LibYUVPictrFmtCnvrt( m_VdoInptPt->m_Thrd.m_TmpFrm1Pt, PictrFmtSrgbF8Bgra8888, m_VdoInptPt->m_Dvc.m_BgraSrcFrmScaleWidth, m_VdoInptPt->m_Dvc.m_BgraSrcFrmScaleHeight,
									 m_VdoInptPt->m_Thrd.m_FrmPt->m_Yu12RsltFrmPt, m_VdoInptPt->m_Yu12FrmLenByt, NULL, PictrFmtBt601F8Yu12I420,
									 NULL ) == 0 )
			{
				if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：将Bgra8888格式原始帧转为Yu12格式结果帧成功。" ) );
			}
			else
			{
				if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：将Bgra8888格式原始帧转为Yu12格式结果帧失败，本次帧丢弃。" ) );
				goto OutPocs;
			}
			//FILE * p_File = fopen( "123.yuv", "wb+" ); fwrite( m_VdoInptPt->m_Thrd.m_FrmPt->m_Yu12RsltFrmPt, 1, m_VdoInptPt->m_Yu12FrmLenByt, p_File ); fclose( p_File );
			//FILE * p_File = fopen( "123.yuv", "rb" ); size_t p_SzByt = fread( m_VdoInptPt->m_Thrd.m_FrmPt->m_Yu12RsltFrmPt, 1, m_VdoInptPt->m_Yu12FrmLenByt, p_File ); fclose( p_File );

			//判断设备是否黑屏。在视频输入处理完后再设置黑屏，这样可以保证视频输入处理器的连续性。
			if( m_VdoInptPt->m_Dvc.m_IsBlack != 0 )
			{
				size_t p_TmpLenByt = m_VdoInptPt->m_FrmWidth * m_VdoInptPt->m_FrmHeight;
				memset( m_VdoInptPt->m_Thrd.m_FrmPt->m_Yu12RsltFrmPt, 0, p_TmpLenByt );
				memset( m_VdoInptPt->m_Thrd.m_FrmPt->m_Yu12RsltFrmPt + p_TmpLenByt, 128, p_TmpLenByt / 2 );
			}

			//使用编码器。
			switch( m_VdoInptPt->m_UseWhatEncd )
			{
				case 0: //如果要使用Yu12原始数据。
				{
					if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：使用Yu12原始数据。" ) );
					break;
				}
				case 1: //如果要使用OpenH264编码器。
				{
					if( OpenH264EncdPocs( m_VdoInptPt->m_OpenH264Encd.m_Pt,
										  m_VdoInptPt->m_Thrd.m_FrmPt->m_Yu12RsltFrmPt, m_VdoInptPt->m_FrmWidth, m_VdoInptPt->m_FrmHeight, m_VdoInptPt->m_Thrd.m_LastTickMsec,
										  m_VdoInptPt->m_Thrd.m_FrmPt->m_EncdRsltFrmPt, m_VdoInptPt->m_Yu12FrmLenByt, &m_VdoInptPt->m_Thrd.m_FrmPt->m_EncdRsltFrmLenByt,
										  NULL ) == 0 )
					{
						if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输入线程：使用OpenH264编码器成功。H264格式结果帧的长度：%uzd，时间戳：%uz64d，类型：%hhd。" ), m_VdoInptPt->m_Thrd.m_FrmPt->m_EncdRsltFrmLenByt, m_VdoInptPt->m_Thrd.m_LastTickMsec, m_VdoInptPt->m_Thrd.m_FrmPt->m_EncdRsltFrmPt[ 4 ] );
					}
					else
					{
						if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：使用OpenH264编码器失败，本次帧丢弃。" ) );
						goto OutPocs;
					}
					break;
				}
			}

			m_VdoInptPt->m_Thrd.m_FrmPt->m_TimeStampMsec = m_VdoInptPt->m_Thrd.m_LastTickMsec; //设置时间戳。

			//追加本次帧到帧容器。注意：从取出到放入过程中可以跳出，跳出后会重新放入到空闲帧容器。
			{
				m_VdoInptPt->m_FrmCntnr.PutTail( &m_VdoInptPt->m_Thrd.m_FrmPt, NULL, 1, NULL );
				m_VdoInptPt->m_Thrd.m_FrmPt = NULL;
			}

			if( m_VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
			{
				m_VdoInptPt->m_Thrd.m_NowTickMsec = FuncGetTickAsMsec();
				LOGFI( Cu8vstr( "视频输入线程：本次帧处理完毕，耗时 %uz64d 毫秒。" ), m_VdoInptPt->m_Thrd.m_NowTickMsec - m_VdoInptPt->m_Thrd.m_LastTickMsec );
			}
		}
		OutPocs:;

		if( m_VdoInptPt->m_Thrd.m_FrmPt != NULL ) //如果获取的空闲帧没有追加到帧容器。
		{
			m_VdoInptPt->m_IdleFrmCntnr.PutTail( &m_VdoInptPt->m_Thrd.m_FrmPt, NULL, 1, NULL );
			m_VdoInptPt->m_Thrd.m_FrmPt = NULL;
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
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
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

	//初始化线程。必须在初始化设备前初始化线程，因为设备会使用线程的指针。
	{
		VdoInptPt->m_Thrd.m_ThrdPt = new VdoInptThrd( VdoInptPt );
		if( VdoInptPt->m_Thrd.m_ThrdPt != NULL )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输入：初始化线程成功。" ) );
		}
		else
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：初始化线程失败。" ) );
			goto Out;
		}
	}

	//初始化设备。
	{
		int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
		UINT p_VdoInptDvcId; //存放视频输入设备的标识符，取值范围为从0到视频输入设备的总数减一。

		ICreateDevEnum * p_CreateDevEnumPt = NULL;
		IEnumMoniker * p_EnumMonikerPt = NULL;
		IMoniker * p_MonikerPt = NULL;
		IEnumPins * p_EnumPinsPt = NULL;
		IPin * p_PinPt = NULL;
		PIN_DIRECTION p_PinDirection;
		IEnumMediaTypes * p_EnumMediaTypesPt = NULL;
		AM_MEDIA_TYPE * p_AmMediaTypePt = NULL;

		IBaseFilter * p_DvcFilterPt = NULL; //存放设备过滤器的指针。
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
		
		//设置视频输入设备标识符。
		if( MediaPocsThrdGetVdoInptDvcId( VdoInptPt->m_Dvc.m_NameVstrPt, &p_VdoInptDvcId, VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：获取视频输入设备名称[%vs]的标识符失败。原因：%vs" ), VdoInptPt->m_Dvc.m_NameVstrPt, VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{
				MediaPocsThrd::ThrdMsgVdoInptDvcClos p_ThrdMsgVdoInptDvcClos;
				if( MsgQueueSendMsg( VdoInptPt->m_MediaPocsThrdPt->m_ThrdMsgQueuePt, 0, 0, MediaPocsThrd::ThrdMsgTypVdoInptDvcClos, &p_ThrdMsgVdoInptDvcClos, sizeof( p_ThrdMsgVdoInptDvcClos ), VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
				{
					if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：发送视频输入设备关闭线程消息失败。原因：%vs" ), VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto OutInitVdoInptDvc;
				}
			}
			p_Rslt = 0; //这里返回成功是为了防止媒体处理线程报错退出。
			goto OutInitVdoInptDvc;
		}

		//创建过滤器图管理器。
		if( CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, ( void * * )&VdoInptPt->m_Dvc.m_FilterGraphManagerPt ) != S_OK )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：创建过滤器图管理器失败。" ) );
			goto OutInitVdoInptDvc;
		}

		//获取过滤器图媒体事件器。
		if( VdoInptPt->m_Dvc.m_FilterGraphManagerPt->QueryInterface( IID_IMediaEvent, ( LPVOID * )&VdoInptPt->m_Dvc.m_FilterGraphMediaEventPt ) != S_OK )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：获取过滤器图媒体事件器失败。" ) );
			goto OutInitVdoInptDvc;
		}

		//获取过滤器图媒体控制器。
		if( VdoInptPt->m_Dvc.m_FilterGraphManagerPt->QueryInterface( IID_IMediaControl, ( LPVOID * )&VdoInptPt->m_Dvc.m_FilterGraphMediaCtrlPt ) != S_OK )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：获取过滤器图媒体控制器失败。" ) );
			goto OutInitVdoInptDvc;
		}

		//创建系统设备枚举器。
		if( CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, ( void * * )&p_CreateDevEnumPt ) != S_OK )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：创建系统设备枚举器失败。" ) );
			goto OutInitVdoInptDvc;
		}

		//查找并创建指定的设备过滤器。
		if( p_CreateDevEnumPt->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, &p_EnumMonikerPt, 0 ) == S_OK )
		{
			for( p_TmpInt = 0; p_EnumMonikerPt->Next( 1, &p_MonikerPt, NULL ) == S_OK; p_TmpInt++ )
			{
				if( p_TmpInt == p_VdoInptDvcId )
				{
					if( p_MonikerPt->BindToObject( NULL, NULL, IID_IBaseFilter, ( void * * )&p_DvcFilterPt ) == S_OK )
					{
						p_MonikerPt->Release();
						p_MonikerPt = NULL;
						break;
					}
					else
					{
						p_MonikerPt->Release();
						p_MonikerPt = NULL;
						if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：根据Moniker创建设备过滤器失败。" ) );
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
		if( p_DvcFilterPt == NULL ) //如果创建设备过滤器失败。
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：创建设备过滤器失败。原因：可能没有视频输入设备。" ) );
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsShowToast != 0 ) ToastFmt( VdoInptPt->m_MediaPocsThrdPt->m_ShowToastWndHdl, 3000, NULL, Cu8vstr( "媒体处理线程：视频输入：创建设备过滤器失败。原因：可能没有视频输入设备。" ) );
			goto OutInitVdoInptDvc;
		}
		if( VdoInptPt->m_Dvc.m_FilterGraphManagerPt->AddFilter( p_DvcFilterPt, L"Video Capture" ) != S_OK ) //如果添加设备过滤器到过滤器图失败。
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：添加设备过滤器到过滤器图失败。" ) );
			goto OutInitVdoInptDvc;
		}

		//选择设备过滤器上合适的引脚和媒体类型。
		if( p_DvcFilterPt->EnumPins( &p_EnumPinsPt ) == S_OK ) //创建引脚枚举器。
		{
			int32_t p_TgtFrmWidth = VdoInptPt->m_FrmWidth; //存放目标的帧宽度，单位为像素。
			int32_t p_TgtFrmHeight = VdoInptPt->m_FrmHeight; //存放目标的帧高度，单位为像素。
			double p_TgtFrmWidthToHeightRatio = ( double )VdoInptPt->m_FrmWidth / ( double )VdoInptPt->m_FrmHeight; //存放目标的帧宽高比。
			int32_t p_TgtAvgTimePerFrm = 1000.0 / ( VdoInptPt->m_MaxSmplRate / 10.0 / 1000.0 ); //存放目标的帧间隔时间，单位为100纳秒。
			int32_t p_SrcAvgTimePerFrm = ( VdoInptPt->m_SrcMaxSmplRate == 0 ) ? INT32_MAX : ( 1000.0 / ( VdoInptPt->m_SrcMaxSmplRate / 10.0 / 1000.0 ) ); //存放原始的帧间隔时间，单位为100纳秒。
			double p_BufFrmAspectRatio; //存放本次的缓冲区帧宽高比。
			int32_t p_BufFrmCropX; //存放本次的缓冲区帧裁剪区域左上角横坐标，单位像素。
			int32_t p_BufFrmCropY; //存放本次的缓冲区帧裁剪区域左上角纵坐标，单位像素。
			int32_t p_BufFrmCropWidth; //存放本次的缓冲区帧裁剪区域宽度，单位像素。
			int32_t p_BufFrmCropHeight; //存放本次的缓冲区帧裁剪区域高度，单位像素。
			int32_t p_IsSetSelCur; //存放是否设置选择的为本次的，为0表示不设置，为非0表示要设置。
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输入：目标的媒体格式：帧间隔时间：%z32d，%lf，宽度：%ld，高度：%ld。" ),
																		 p_TgtAvgTimePerFrm, 1000.0 / ( p_TgtAvgTimePerFrm / 10.0 / 1000.0 ),
																		 p_TgtFrmWidth, p_TgtFrmHeight );
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输入：指定的媒体格式：帧间隔时间：%z32d，%lf，宽度：%ld，高度：%ld。" ),
																		 p_SrcAvgTimePerFrm, 1000.0 / ( p_SrcAvgTimePerFrm / 10.0 / 1000.0 ),
																		 VdoInptPt->m_SrcFrmWidth, VdoInptPt->m_SrcFrmHeight );

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

								if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输入：设备支持的媒体格式：子类型：%s，帧间隔时间：%lld，%.1f，宽度：%ld，高度：%ld。" ),
																							 ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_MJPG ) ? "MJPEG" : ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_YUY2 ) ? "YUY2" : ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_RGB24 ) ? "RGB24" : "unkown",
																							 VIDEOINFOHEADER_AvgTimePerFrm( p_AmMediaTypePt ), 1000.0 / ( VIDEOINFOHEADER_AvgTimePerFrm( p_AmMediaTypePt ) / 10.0 / 1000.0 ),
																							 VIDEOINFOHEADER_Width( p_AmMediaTypePt ), VIDEOINFOHEADER_Height( p_AmMediaTypePt ) );

								if( ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_MJPG ) || ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_YUY2 ) ) //如果媒体格式为MJPEG或YUY2才进行选择。
								{
									//设置本次的缓冲区帧宽高比、裁剪宽度、裁剪高度、裁剪区域左上角坐标。
									p_BufFrmAspectRatio = ( double )VIDEOINFOHEADER_Width( p_AmMediaTypePt ) / ( double )VIDEOINFOHEADER_Height( p_AmMediaTypePt );
									if( p_BufFrmAspectRatio >= p_TgtFrmWidthToHeightRatio ) //如果本次的缓冲区帧宽高比比目标的大，就表示需要裁剪宽度。
									{
										p_BufFrmCropWidth = ( int )( ( double )VIDEOINFOHEADER_Height( p_AmMediaTypePt ) * p_TgtFrmWidthToHeightRatio ); //设置本次的缓冲区帧裁剪区域宽度，使裁剪区域居中。
										p_BufFrmCropWidth -= p_BufFrmCropWidth % 2;
										p_BufFrmCropHeight = VIDEOINFOHEADER_Height( p_AmMediaTypePt ); //设置本次的缓冲区帧裁剪区域高度，使裁剪区域居中。

										p_BufFrmCropX = ( VIDEOINFOHEADER_Width( p_AmMediaTypePt ) - p_BufFrmCropWidth ) / 2; //设置本次的缓冲区帧裁剪区域左上角横坐标，使裁剪区域居中。
										p_BufFrmCropX -= p_BufFrmCropX % 2;
										p_BufFrmCropY = 0; //设置本次的缓冲区帧裁剪区域左上角纵坐标。
									}
									else //如果本次的缓冲区帧宽高比比目标的小，就表示需要裁剪高度。
									{
										p_BufFrmCropWidth = VIDEOINFOHEADER_Width( p_AmMediaTypePt ); //设置本次的缓冲区帧裁剪区域宽度，使裁剪区域居中。
										p_BufFrmCropHeight = ( int )( ( double )VIDEOINFOHEADER_Width( p_AmMediaTypePt ) / p_TgtFrmWidthToHeightRatio ); //设置本次的缓冲区帧裁剪区域高度，使裁剪区域居中。
										p_BufFrmCropHeight -= p_BufFrmCropHeight % 2;

										p_BufFrmCropX = 0; //设置本次的缓冲区帧裁剪区域左上角横坐标。
										p_BufFrmCropY = ( VIDEOINFOHEADER_Height( p_AmMediaTypePt ) - p_BufFrmCropHeight ) / 2; //设置本次的缓冲区帧裁剪区域左上角纵坐标，使裁剪区域居中。
										p_BufFrmCropY -= p_BufFrmCropY % 2;
									}
									
									//如果本次的帧率和帧的大小符合原始的，就设置选择的为本次的。
									//如果要自动选择原始帧率和帧的大小，或选择的帧率和帧的大小不符合原始的，就判断本次的和目标的。
									p_IsSetSelCur = 0;
									if( ( VIDEOINFOHEADER_AvgTimePerFrm( p_AmMediaTypePt ) <= p_SrcAvgTimePerFrm ) && ( VIDEOINFOHEADER_Width( p_AmMediaTypePt ) == VdoInptPt->m_SrcFrmWidth ) && ( VIDEOINFOHEADER_Height( p_AmMediaTypePt ) == VdoInptPt->m_SrcFrmHeight ) )
									{
										p_IsSetSelCur = 1;
									}
									else if( ( ( VdoInptPt->m_SrcMaxSmplRate == 0 ) && ( VdoInptPt->m_SrcFrmWidth == 0 ) && ( VdoInptPt->m_SrcFrmHeight == 0 ) ) ||
											 ( ( p_SelAmMediaTypePt == NULL ) || ( VIDEOINFOHEADER_AvgTimePerFrm( p_AmMediaTypePt ) > p_SrcAvgTimePerFrm ) || ( VIDEOINFOHEADER_Width( p_AmMediaTypePt ) != VdoInptPt->m_SrcFrmWidth ) || ( VIDEOINFOHEADER_Height( p_AmMediaTypePt ) != VdoInptPt->m_SrcFrmHeight ) ) )
									{
										//如果选择的帧裁剪区域不满足目标的，则只要本次的帧裁剪区域比选择的大，就设置选择的为本次的。
										//如果选择的帧裁剪区域满足目标的，但帧率不满足目标的，则只要本次的帧裁剪区域满足目标的，且本次的帧率比选择的高，就设置选择的为本次的。
										//如果选择的帧裁剪区域和帧率都满足目标的，就只要本次的帧裁剪区域和帧率都满足目标的，且本次的帧裁剪区域比选择的小、或本次的帧裁剪区域相同但裁剪量比选择的小、或本次的帧裁剪区域和裁剪量都相同但帧率比选择的低，就设置选择的为本次的。
										if( ( VdoInptPt->m_Dvc.m_BgraSrcFrmCropWidth < p_TgtFrmWidth ) && ( VdoInptPt->m_Dvc.m_BgraSrcFrmCropHeight < p_TgtFrmHeight ) )
										{
											if( ( p_BufFrmCropWidth > VdoInptPt->m_Dvc.m_BgraSrcFrmCropWidth ) && ( p_BufFrmCropHeight > VdoInptPt->m_Dvc.m_BgraSrcFrmCropHeight ) )
											{
												p_IsSetSelCur = 1;
											}
										}
										else if( VIDEOINFOHEADER_AvgTimePerFrm( p_SelAmMediaTypePt ) > p_TgtAvgTimePerFrm )
										{
											if( ( p_BufFrmCropWidth >= p_TgtFrmWidth ) && ( p_BufFrmCropHeight >= p_TgtFrmHeight ) )
											{
												if( VIDEOINFOHEADER_AvgTimePerFrmCom( p_AmMediaTypePt, < , p_SelAmMediaTypePt ) )
												{
													p_IsSetSelCur = 1;
												}
											}
										}
										else
										{
											if( ( p_BufFrmCropWidth >= p_TgtFrmWidth ) && ( p_BufFrmCropHeight >= p_TgtFrmHeight ) )
											{
												if( VIDEOINFOHEADER_AvgTimePerFrm( p_AmMediaTypePt ) <= p_TgtAvgTimePerFrm )
												{
													if( ( p_BufFrmCropWidth < VdoInptPt->m_Dvc.m_BgraSrcFrmCropWidth ) && ( p_BufFrmCropHeight < VdoInptPt->m_Dvc.m_BgraSrcFrmCropHeight ) )
													{
														p_IsSetSelCur = 1;
													}
													else if( ( p_BufFrmCropWidth == VdoInptPt->m_Dvc.m_BgraSrcFrmCropWidth ) && ( p_BufFrmCropHeight == VdoInptPt->m_Dvc.m_BgraSrcFrmCropHeight ) )
													{
														if( p_BufFrmCropX + p_BufFrmCropY < VdoInptPt->m_Dvc.m_BgraSrcFrmCropX + VdoInptPt->m_Dvc.m_BgraSrcFrmCropY )
														{
															p_IsSetSelCur = 1;
														}
														else if( p_BufFrmCropX + p_BufFrmCropY == VdoInptPt->m_Dvc.m_BgraSrcFrmCropX + VdoInptPt->m_Dvc.m_BgraSrcFrmCropY )
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

										VdoInptPt->m_Dvc.m_BgraSrcFrmWidth = VIDEOINFOHEADER_Width( p_SelAmMediaTypePt );
										VdoInptPt->m_Dvc.m_BgraSrcFrmHeight = VIDEOINFOHEADER_Height( p_SelAmMediaTypePt );
										VdoInptPt->m_Dvc.m_BgraSrcFrmLenByt = VdoInptPt->m_Dvc.m_BgraSrcFrmWidth * VdoInptPt->m_Dvc.m_BgraSrcFrmHeight * 4;

										VdoInptPt->m_Dvc.m_BgraSrcFrmCropX = p_BufFrmCropX;
										VdoInptPt->m_Dvc.m_BgraSrcFrmCropY = p_BufFrmCropY;
										VdoInptPt->m_Dvc.m_BgraSrcFrmCropWidth = p_BufFrmCropWidth;
										VdoInptPt->m_Dvc.m_BgraSrcFrmCropHeight = p_BufFrmCropHeight;
									}
								}
								else
								{
									if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：视频输入：本次设备支持的媒体格式不是为MJPEG或YUY2，不能被选择。" ) );
								}
							}
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
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：选择设备过滤器上合适的引脚和媒体类型失败。" ) );
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsShowToast != 0 ) Toast( VdoInptPt->m_MediaPocsThrdPt->m_ShowToastWndHdl, 3000, NULL, Cu8vstr( "媒体处理线程：视频输入：选择设备过滤器上合适的引脚和媒体类型失败。" ) );
			goto OutInitVdoInptDvc;
		}
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输入：选择设备的媒体格式：子类型：%s，帧间隔时间：%lld，%.1f，宽度：%ld，高度：%ld。" ),
																	 ( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_MJPG ) ? "MJPEG" : ( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_YUY2 ) ? "YUY2" : ( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_RGB24 ) ? "RGB24" : "unkown",
																	 VIDEOINFOHEADER_AvgTimePerFrm( p_SelAmMediaTypePt ), 1000.0 / ( VIDEOINFOHEADER_AvgTimePerFrm( p_SelAmMediaTypePt ) / 10.0 / 1000.0 ),
																	 VIDEOINFOHEADER_Width( p_SelAmMediaTypePt ), VIDEOINFOHEADER_Height( p_SelAmMediaTypePt ) );

		//判断缓冲区帧是否裁剪。
		if( ( VdoInptPt->m_Dvc.m_BgraSrcFrmWidth > VdoInptPt->m_Dvc.m_BgraSrcFrmCropWidth ) || //如果Bgra8888格式原始帧的宽度比裁剪宽度大，就表示需要裁剪宽度。
			( VdoInptPt->m_Dvc.m_BgraSrcFrmHeight > VdoInptPt->m_Dvc.m_BgraSrcFrmCropHeight ) ) //如果Bgra8888格式原始帧的高度比裁剪高度大，就表示需要裁剪高度。
		{
			VdoInptPt->m_Dvc.m_BgraSrcFrmIsCrop = 1; //设置Bgra8888格式原始帧要裁剪。
		}
		else //如果缓冲区帧的宽度和高度与裁剪宽度和高度一致，就表示不需要裁剪。
		{
			VdoInptPt->m_Dvc.m_BgraSrcFrmIsCrop = 0; //设置Bgra8888格式原始帧不裁剪。
		}
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输入：Bgra8888格式原始帧是否裁剪：%z32d，裁剪区域左上角的横坐标：%z32d，纵坐标：%z32d，裁剪区域的宽度：%z32d，高度：%z32d。" ), VdoInptPt->m_Dvc.m_BgraSrcFrmIsCrop, VdoInptPt->m_Dvc.m_BgraSrcFrmCropX, VdoInptPt->m_Dvc.m_BgraSrcFrmCropY, VdoInptPt->m_Dvc.m_BgraSrcFrmCropWidth, VdoInptPt->m_Dvc.m_BgraSrcFrmCropHeight );

		//判断缓冲区帧是否缩放。
		if( ( VdoInptPt->m_Dvc.m_BgraSrcFrmCropWidth != VdoInptPt->m_FrmWidth ) || ( VdoInptPt->m_Dvc.m_BgraSrcFrmCropHeight != VdoInptPt->m_FrmHeight ) )
		{
			VdoInptPt->m_Dvc.m_BgraSrcFrmIsScale = 1; //设置Bgra8888格式原始帧要缩放。
		}
		else
		{
			VdoInptPt->m_Dvc.m_BgraSrcFrmIsScale = 0; //设置Bgra8888格式原始帧不缩放。
		}
		VdoInptPt->m_Dvc.m_BgraSrcFrmScaleWidth = VdoInptPt->m_FrmWidth; //设置Bgra8888格式原始帧缩放后的宽度。
		VdoInptPt->m_Dvc.m_BgraSrcFrmScaleHeight = VdoInptPt->m_FrmHeight; //设置Bgra8888格式原始帧缩放后的高度。
		VdoInptPt->m_Dvc.m_BgraSrcFrmScaleLenByt = VdoInptPt->m_FrmWidth * VdoInptPt->m_FrmHeight * 4; //设置Bgra8888格式原始帧缩放后的长度。
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输入：Bgra8888格式原始帧是否缩放：%z32d，缩放后的宽度：%z32d，缩放后的高度：%z32d，缩放后的长度：%zd。" ), VdoInptPt->m_Dvc.m_BgraSrcFrmIsScale, VdoInptPt->m_Dvc.m_BgraSrcFrmScaleWidth, VdoInptPt->m_Dvc.m_BgraSrcFrmScaleHeight, VdoInptPt->m_Dvc.m_BgraSrcFrmScaleLenByt );

		//创建设备过滤器上选择的引脚和媒体类型对应的解码过滤器，并连接引脚。
		if( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_MJPG )
		{
			if( CoCreateInstance( CLSID_MjpegDec, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, ( void * * )&p_DecFilterPt ) != S_OK )
			{
				if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：创建MJPG解码过滤器失败。" ) );
				goto OutInitVdoInptDvc;
			}
		}
		else if( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_YUY2 )
		{
			if( CoCreateInstance( CLSID_AVIDec, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, ( void * * )&p_DecFilterPt ) != S_OK )
			{
				if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：创建AVI解码过滤器失败。" ) );
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
		if( VdoInptPt->m_Dvc.m_FilterGraphManagerPt->AddFilter( p_DecFilterPt, L"Decompressor" ) != S_OK ) //如果添加解码过滤器到过滤器图失败。
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：添加解码过滤器到过滤器图失败。" ) );
			goto OutInitVdoInptDvc;
		}
		if( VdoInptPt->m_Dvc.m_FilterGraphManagerPt->ConnectDirect( p_SelPinPt, p_DecFilterInptPinPt, p_SelAmMediaTypePt ) != S_OK ) //如果连接引脚失败。
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：连接设备过滤器与解码过滤器的引脚失败。" ) );
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsShowToast != 0 ) ToastFmt( VdoInptPt->m_MediaPocsThrdPt->m_ShowToastWndHdl, 3000, NULL, Cu8vstr( "媒体处理线程：视频输入：连接设备过滤器与解码过滤器的引脚失败。原因：可能是视频输入设备无法正常工作。" ) );
			goto OutInitVdoInptDvc;
		}

		//创建采样抓取过滤器，并连接引脚。
		if( CoCreateInstance( CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, ( void * * )&p_SmplGrabberFilterPt ) != S_OK )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：创建采样抓取过滤器失败。" ) );
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
		if( VdoInptPt->m_Dvc.m_FilterGraphManagerPt->AddFilter( p_SmplGrabberFilterPt, L"Sample Grabber" ) != S_OK ) //如果采样抓取过滤器到过滤器图失败。
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：添加采样抓取过滤器到过滤器图失败。" ) );
			goto OutInitVdoInptDvc;
		}
		p_TmpAmMediaType.majortype = MEDIATYPE_Video;
		//p_TmpAmMediaType.subtype = MEDIASUBTYPE_RGB555; //PICTR_FMT_SRGBF8_BGR555。
		//p_TmpAmMediaType.subtype = MEDIASUBTYPE_RGB565; //PICTR_FMT_SRGBF8_BGR565。
		//p_TmpAmMediaType.subtype = MEDIASUBTYPE_RGB24; //PICTR_FMT_SRGBF8_BGR888。
		p_TmpAmMediaType.subtype = MEDIASUBTYPE_RGB32; //PictrFmtSrgbF8Bgra8888。
		if( VdoInptPt->m_Dvc.m_FilterGraphManagerPt->ConnectDirect( p_DecFilterOtptPinPt, p_SmplGrabberFilterInptPinPt, &p_TmpAmMediaType ) != S_OK ) //如果连接引脚失败。
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：连接解码过滤器与采样抓取过滤器的引脚失败。" ) );
			goto OutInitVdoInptDvc;
		}

		//设置采样抓取过滤器的回调函数。
		if( p_SmplGrabberFilterPt->QueryInterface( IID_ISampleGrabber, ( void * * )&p_SmplGrabberInterfacePt ) != S_OK )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：获取采样抓取过滤器的回调接口失败。" ) );
			goto OutInitVdoInptDvc;
		}
		if( p_SmplGrabberInterfacePt->SetCallback( VdoInptPt->m_Thrd.m_ThrdPt, 1 ) != S_OK )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：设置采样抓取过滤器的回调函数失败。" ) );
			goto OutInitVdoInptDvc;
		}

		//设置过滤器图管理器开始运行。
		if( VdoInptPt->m_Dvc.m_FilterGraphMediaCtrlPt->Run() != S_OK )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：设置过滤器图管理器开始运行失败。" ) );
			goto OutInitVdoInptDvc;
		}
		
		//发送视频输入设备改变线程消息。
		{
			MediaPocsThrd::ThrdMsgVdoInptDvcChg p_ThrdMsgVdoInptDvcChg;
			if( MsgQueueSendMsg( VdoInptPt->m_MediaPocsThrdPt->m_ThrdMsgQueuePt, 0, 0, MediaPocsThrd::ThrdMsgTypVdoInptDvcChg, &p_ThrdMsgVdoInptDvcChg, sizeof( p_ThrdMsgVdoInptDvcChg ), VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
			{
				if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：发送视频输入设备改变线程消息失败。原因：%vs" ), VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
		}
		
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输入：初始化设备成功。" ) );

		p_Rslt = 0; //设置本函数执行成功。

		OutInitVdoInptDvc:
		if( p_DvcFilterPt != NULL )
		{
			p_DvcFilterPt->Release();
			p_DvcFilterPt = NULL;
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
	} //初始化设备完毕。

	//初始化线程的临时变量。
	{
		VdoInptPt->m_Thrd.m_IsInitThrdTmpVar = 1; //设置已初始化线程的临时变量。
		VdoInptPt->m_Thrd.m_LastTickMsec = 0; //初始化上一个帧的嘀嗒钟。
		VdoInptPt->m_Thrd.m_FrmTimeStepMsec = 1000.0 / VdoInptPt->m_MaxSmplRate; //初始化帧的时间步进。
		if( VdoInptPt->m_Dvc.m_BgraSrcFrmScaleLenByt >= VdoInptPt->m_Dvc.m_BgraSrcFrmLenByt ) //如果Bgra8888格式原始帧缩放后的长度大于等于Bgra8888格式原始帧的长度。
		{
			VdoInptPt->m_Thrd.m_TmpFrmSzByt = VdoInptPt->m_Dvc.m_BgraSrcFrmScaleLenByt; //初始化临时帧的大小。
		}
		else //如果Bgra8888格式原始帧缩放后的长度小于Bgra8888格式原始帧的长度。
		{
			VdoInptPt->m_Thrd.m_TmpFrmSzByt = VdoInptPt->m_Dvc.m_BgraSrcFrmLenByt; //初始化临时帧的大小。
		}
		VdoInptPt->m_Thrd.m_TmpFrm1Pt = ( uint8_t * )malloc( VdoInptPt->m_Thrd.m_TmpFrmSzByt ); //初始化临时帧的指针。
		if( VdoInptPt->m_Thrd.m_TmpFrm1Pt == NULL )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：创建临时帧的内存块失败。" ) );
			goto Out;
		}
		VdoInptPt->m_Thrd.m_TmpFrm2Pt = ( uint8_t * )malloc( VdoInptPt->m_Thrd.m_TmpFrmSzByt ); //初始化临时帧的指针。
		if( VdoInptPt->m_Thrd.m_TmpFrm2Pt == NULL )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：创建临时帧的内存块失败。" ) );
			goto Out;
		}
		VdoInptPt->m_Thrd.m_TmpFrmLenByt  = 0; //初始化临时帧的长度。
		VdoInptPt->m_Thrd.m_FrmPt = NULL; //初始化帧的指针。
		VdoInptPt->m_Thrd.m_ElmTotal = 0; //初始化元素总数。
		VdoInptPt->m_Thrd.m_LastTickMsec = 0; //初始化上次的嘀嗒钟。
		VdoInptPt->m_Thrd.m_NowTickMsec = 0; //初始化本次的嘀嗒钟。
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：视频输入：初始化线程的临时变量成功。" ) );
	}

	//初始化编码器。
	switch( VdoInptPt->m_UseWhatEncd )
	{
		case 0: //如果要使用Yu12原始数据。
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：视频输入：初始化Yu12原始数据成功。" ) );
			break;
		}
		case 1: //如果要使用OpenH264编码器。
		{
			if( OpenH264EncdInit( VdoInptPt->m_MediaPocsThrdPt->m_LicnCodePt, &VdoInptPt->m_OpenH264Encd.m_Pt, VdoInptPt->m_FrmWidth, VdoInptPt->m_FrmHeight, VdoInptPt->m_OpenH264Encd.m_VdoType, VdoInptPt->m_OpenH264Encd.m_EncdBitrate, VdoInptPt->m_OpenH264Encd.m_BitrateCtrlMode, VdoInptPt->m_MaxSmplRate, VdoInptPt->m_OpenH264Encd.m_IDRFrmIntvl, VdoInptPt->m_OpenH264Encd.m_Cmplxt, VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：视频输入：初始化OpenH264编码器成功。" ) );
			}
			else
			{
				if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：初始化OpenH264编码器失败。原因：%vs" ), VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
			break;
		}
	}

	//初始化帧容器。
	if( VdoInptPt->m_FrmCntnr.Init( sizeof( VdoInpt::Frm * ), 1, BufAutoAdjMethFreeNumber, 1, SIZE_MAX, VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
	{
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输入：初始化帧容器成功。" ) );
	}
	else
	{
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：初始化帧容器失败。原因：%vs" ), VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	//初始化空闲帧容器。
	if( VdoInptPt->m_IdleFrmCntnr.Init( sizeof( VdoInpt::Frm * ), 1, BufAutoAdjMethFreeNumber, 1, SIZE_MAX, VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
	{
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输入：初始化空闲帧容器成功。" ) );
	}
	else
	{
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：初始化空闲帧容器失败。原因：%vs" ), VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
	{
		p_NowTickMsec = FuncGetTickAsMsec(); //记录初始化结束的嘀嗒钟。
		LOGFI( Cu8vstr( "媒体处理线程：视频输入：初始化耗时 %uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
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
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoInptDstoy( VdoInpt * VdoInptPt )
{
	uint64_t p_LastTickMsec = 0;
	uint64_t p_NowTickMsec = 0;
	
	if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) p_LastTickMsec = FuncGetTickAsMsec(); //记录销毁开始的嘀嗒钟。

	//销毁设备。
	if( VdoInptPt->m_Dvc.m_FilterGraphMediaCtrlPt != NULL )
	{
		VdoInptPt->m_Dvc.m_FilterGraphMediaCtrlPt->Stop(); //让过滤器图管理器停止运行。
		VdoInptPt->m_Dvc.m_FilterGraphMediaCtrlPt->Release();
		VdoInptPt->m_Dvc.m_FilterGraphMediaCtrlPt = NULL;
	}
	if( VdoInptPt->m_Dvc.m_FilterGraphMediaEventPt != NULL )
	{
		VdoInptPt->m_Dvc.m_FilterGraphMediaEventPt->Release();
		VdoInptPt->m_Dvc.m_FilterGraphMediaEventPt = NULL;
	}
	if( VdoInptPt->m_Dvc.m_FilterGraphManagerPt != NULL )
	{
		VdoInptPt->m_Dvc.m_FilterGraphManagerPt->Release();
		VdoInptPt->m_Dvc.m_FilterGraphManagerPt = NULL;
		VdoInptPt->m_Dvc.m_BgraSrcFrmWidth = 0;
		VdoInptPt->m_Dvc.m_BgraSrcFrmHeight = 0;
		VdoInptPt->m_Dvc.m_BgraSrcFrmLenByt = 0;
		VdoInptPt->m_Dvc.m_BgraSrcFrmIsCrop = 0;
		VdoInptPt->m_Dvc.m_BgraSrcFrmCropX = 0;
		VdoInptPt->m_Dvc.m_BgraSrcFrmCropY = 0;
		VdoInptPt->m_Dvc.m_BgraSrcFrmCropWidth = 0;
		VdoInptPt->m_Dvc.m_BgraSrcFrmCropHeight = 0;
		VdoInptPt->m_Dvc.m_BgraSrcFrmIsScale = 0;
		VdoInptPt->m_Dvc.m_BgraSrcFrmScaleWidth = 0;
		VdoInptPt->m_Dvc.m_BgraSrcFrmScaleHeight = 0;
		VdoInptPt->m_Dvc.m_BgraSrcFrmScaleLenByt = 0;
		InvalidateRect( VdoInptPt->m_Dvc.m_PrvwWndHdl, NULL, TRUE ); //重绘预览窗口。
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：视频输入：销毁设备成功。" ) );
	}

	//销毁线程。
	if( VdoInptPt->m_Thrd.m_ThrdPt != NULL )
	{
		delete VdoInptPt->m_Thrd.m_ThrdPt;
		VdoInptPt->m_Thrd.m_ThrdPt = NULL;
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：视频输入：销毁线程成功。" ) );
	}

	//销毁线程的临时变量。
	if( VdoInptPt->m_Thrd.m_IsInitThrdTmpVar != 0 )
	{
		VdoInptPt->m_Thrd.m_IsInitThrdTmpVar = 0; //设置未初始化线程的临时变量。
		VdoInptPt->m_Thrd.m_LastFrmTickMsec = 0; //销毁上一个帧的嘀嗒钟。
		VdoInptPt->m_Thrd.m_FrmTimeStepMsec = 0; //销毁帧的时间步进。
		if( VdoInptPt->m_Thrd.m_TmpFrm1Pt != NULL ) //销毁视频临时帧的指针。
		{
			free( VdoInptPt->m_Thrd.m_TmpFrm1Pt );
			VdoInptPt->m_Thrd.m_TmpFrm1Pt = NULL;
		}
		if( VdoInptPt->m_Thrd.m_TmpFrm2Pt != NULL ) //销毁视频临时帧的指针。
		{
			free( VdoInptPt->m_Thrd.m_TmpFrm2Pt );
			VdoInptPt->m_Thrd.m_TmpFrm2Pt = NULL;
		}
		VdoInptPt->m_Thrd.m_TmpFrmSzByt = 0; //销毁临时帧的大小。
		VdoInptPt->m_Thrd.m_TmpFrmLenByt  = 0; //销毁临时帧的长度。
		VdoInptPt->m_Thrd.m_FrmPt = NULL; //销毁帧的指针。
		VdoInptPt->m_Thrd.m_ElmTotal = 0; //销毁元素总数。
		VdoInptPt->m_Thrd.m_LastTickMsec = 0; //销毁上次的嘀嗒钟。
		VdoInptPt->m_Thrd.m_NowTickMsec = 0; //销毁本次的嘀嗒钟。
		if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：视频输入：销毁线程的临时变量成功。" ) );
	}

	//销毁空闲帧容器。
	if( VdoInptPt->m_IdleFrmCntnr.m_CQueuePt != NULL )
	{
		while( VdoInptPt->m_IdleFrmCntnr.GetHead( &VdoInptPt->m_Thrd.m_FrmPt, NULL, 1, 0, NULL ) == 0 )
		{
			if( VdoInptPt->m_Thrd.m_FrmPt->m_BgraSrcFrmPt != NULL )
			{
				free( VdoInptPt->m_Thrd.m_FrmPt->m_BgraSrcFrmPt );
				VdoInptPt->m_Thrd.m_FrmPt->m_BgraSrcFrmPt = NULL;
			}
			if( VdoInptPt->m_Thrd.m_FrmPt->m_Yu12RsltFrmPt )
			{
				free( VdoInptPt->m_Thrd.m_FrmPt->m_Yu12RsltFrmPt );
				VdoInptPt->m_Thrd.m_FrmPt->m_Yu12RsltFrmPt = NULL;
			}
			if( VdoInptPt->m_Thrd.m_FrmPt->m_EncdRsltFrmPt != NULL )
			{
				free( VdoInptPt->m_Thrd.m_FrmPt->m_EncdRsltFrmPt );
				VdoInptPt->m_Thrd.m_FrmPt->m_EncdRsltFrmPt = NULL;
			}
			free( VdoInptPt->m_Thrd.m_FrmPt );
			VdoInptPt->m_Thrd.m_FrmPt = NULL;
		}
		if( VdoInptPt->m_IdleFrmCntnr.Dstoy( VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输入：销毁空闲帧容器成功。" ) );
		}
		else
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：销毁空闲帧容器失败。原因：%vs" ), VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
	}

	//销毁帧容器。
	if( VdoInptPt->m_FrmCntnr.m_CQueuePt != NULL )
	{
		while( VdoInptPt->m_FrmCntnr.GetHead( &VdoInptPt->m_Thrd.m_FrmPt, NULL, 1, 0, NULL ) == 0 )
		{
			if( VdoInptPt->m_Thrd.m_FrmPt->m_BgraSrcFrmPt != NULL )
			{
				free( VdoInptPt->m_Thrd.m_FrmPt->m_BgraSrcFrmPt );
				VdoInptPt->m_Thrd.m_FrmPt->m_BgraSrcFrmPt = NULL;
			}
			if( VdoInptPt->m_Thrd.m_FrmPt->m_Yu12RsltFrmPt != NULL )
			{
				free( VdoInptPt->m_Thrd.m_FrmPt->m_Yu12RsltFrmPt );
				VdoInptPt->m_Thrd.m_FrmPt->m_Yu12RsltFrmPt = NULL;
			}
			if( VdoInptPt->m_Thrd.m_FrmPt->m_EncdRsltFrmPt != NULL )
			{
				free( VdoInptPt->m_Thrd.m_FrmPt->m_EncdRsltFrmPt );
				VdoInptPt->m_Thrd.m_FrmPt->m_EncdRsltFrmPt = NULL;
			}
			free( VdoInptPt->m_Thrd.m_FrmPt );
			VdoInptPt->m_Thrd.m_FrmPt = NULL;
		}
		if( VdoInptPt->m_FrmCntnr.Dstoy( VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输入：销毁帧容器成功。" ) );
		}
		else
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：销毁帧容器失败。原因：%vs" ), VdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
	}

	//销毁编码器。
	switch( VdoInptPt->m_UseWhatEncd )
	{
		case 0: //如果要使用Yu12原始数据。
		{
			if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输入：销毁Yu12原始数据成功。" ) );
			break;
		}
		case 1: //如果要使用OpenH264编码器。
		{
			if( VdoInptPt->m_OpenH264Encd.m_Pt != NULL )
			{
				if( OpenH264EncdDstoy( VdoInptPt->m_OpenH264Encd.m_Pt, NULL ) == 0 )
				{
					if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输入：销毁OpenH264编码器成功。" ) );
				}
				else
				{
					if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入：销毁OpenH264编码器失败。" ) );
				}
				VdoInptPt->m_OpenH264Encd.m_Pt = NULL;
			}
			break;
		}
	}
	
	if( VdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
	{
		p_NowTickMsec = FuncGetTickAsMsec(); //记录销毁结束的嘀嗒钟。
		LOGFI( Cu8vstr( "媒体处理线程：视频输入：销毁耗时 %uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
	}
}
