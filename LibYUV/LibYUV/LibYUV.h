#include "Func.h"

#ifndef __LIBYUV_H__
#define __LIBYUV_H__

//LibYUV项目的DLL动态库文件导入导出符号宏。
#if( defined __NAME_LIBYUV__ ) //如果正在编译LibYUV项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __LIBYUV_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __LIBYUV_DLLAPI__ __declspec( dllexport )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __LIBYUV_DLLAPI__
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __LIBYUV_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __LIBYUV_DLLAPI__ __attribute__( ( visibility( "default" ) ) )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __LIBYUV_DLLAPI__
		#endif
	#else //如果正在使用未知编译器。
		#define __LIBYUV_DLLAPI__
	#endif
#else //如果正在编译其他项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __LNKLIB__ ) //如果正在链接LIB静态库文件。
			#define __LIBYUV_DLLAPI__
		#elif( defined __LNKDLL__ ) //如果正在链接DLL动态库文件。
			#define __LIBYUV_DLLAPI__ __declspec( dllimport )
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#define __LIBYUV_DLLAPI__
	#else //如果正在使用未知编译器。
		#define __LIBYUV_DLLAPI__
	#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

//图片格式。
enum PictrFmt
{
	PICTR_FMT_BT601F8_NV12,       //BT.601标准，Full Range，8位，内存排列：'YYYYYYYY……','UVUVUVUV……'。libyuv::FOURCC_NV12，libyuv::kYuvJPEGConstants，libyuv::kYvuJPEGConstants
	PICTR_FMT_BT601F8_NV21,       //BT.601标准，Full Range，8位，内存排列：'YYYYYYYY……','VUVUVUVU……'。libyuv::FOURCC_NV21，libyuv::kYuvJPEGConstants，libyuv::kYvuJPEGConstants
	PICTR_FMT_BT601F8_YU12_I420,  //BT.601标准，Full Range，8位，内存排列：'YYYYYYYY……','UUUU……','VVVV……'。libyuv::FOURCC_J420，libyuv::kYuvJPEGConstants，libyuv::kYvuJPEGConstants
	PICTR_FMT_BT601F8_YV12,       //BT.601标准，Full Range，8位，内存排列：'YYYYYYYY……','VVVV……','UUUU……'。libyuv::FOURCC_YV12，libyuv::kYuvJPEGConstants，libyuv::kYvuJPEGConstants
	PICTR_FMT_SRGBF8_RGB555,      //sRGB标准，Full Range，8位，内存排列：'R','G','B'。
	PICTR_FMT_SRGBF8_BGR555,      //sRGB标准，Full Range，8位，内存排列：'B','G','R'。MEDIASUBTYPE_RGB555
	PICTR_FMT_SRGBF8_RGB565,      //sRGB标准，Full Range，8位，内存排列：'R','G','B'。libyuv::FOURCC_RGBP，WINDOW_FORMAT_RGB_565
	PICTR_FMT_SRGBF8_BGR565,      //sRGB标准，Full Range，8位，内存排列：'B','G','R'。MEDIASUBTYPE_RGB565
	PICTR_FMT_SRGBF8_RGB888,      //sRGB标准，Full Range，8位，内存排列：'R','G','B'。
	PICTR_FMT_SRGBF8_BGR888,      //sRGB标准，Full Range，8位，内存排列：'B','G','R'。MEDIASUBTYPE_RGB24
	PICTR_FMT_SRGBF8_ARGB8888,    //sRGB标准，Full Range，8位，内存排列：'A','R','G','B'。
	PICTR_FMT_SRGBF8_ABGR8888,    //sRGB标准，Full Range，8位，内存排列：'A','B','G','R'。libyuv::FOURCC_ABGR，WINDOW_FORMAT_RGBA_8888
	PICTR_FMT_SRGBF8_RGBA8888,    //sRGB标准，Full Range，8位，内存排列：'R','G','B','A'。
	PICTR_FMT_SRGBF8_BGRA8888,    //sRGB标准，Full Range，8位，内存排列：'B','G','R','A'。libyuv::FOURCC_ARGB，MEDIASUBTYPE_RGB32
	PICTR_FMT_UNKOWN = 99         //未知格式。
};

//旋转角度。
enum RotateDegree
{
	ROTATE_DEGREE_0 = 0,      //0度。libyuv::kRotate0
	ROTATE_DEGREE_90 = 90,    //90度。libyuv::kRotate90
	ROTATE_DEGREE_180 = 180,  //180度。libyuv::kRotate180
	ROTATE_DEGREE_270 = 270   //270度。libyuv::kRotate270
};

__LIBYUV_DLLAPI__ int LibYUVPictrCrop( const uint8_t * SrcPictrPt, int32_t SrcPictrFmt, int32_t SrcPictrWidth, int32_t SrcPictrHeight,
									   int32_t CropX, int32_t CropY, int32_t CropWidth, int32_t CropHeight,
									   uint8_t * DstPictrPt, size_t DstPictrSzByt, size_t * DstPictrLenBytPt, int32_t * DstPictrWidthPt, int32_t * DstPictrHeightPt,
									   Vstr * ErrInfoVstrPt );
__LIBYUV_DLLAPI__ int LibYUVPictrCropWithStrd( const uint8_t * SrcPictrPlane1Pt, size_t SrcPictrPlane1StrdByt,
											   const uint8_t * SrcPictrPlane2Pt, size_t SrcPictrPlane2StrdByt,
											   const uint8_t * SrcPictrPlane3Pt, size_t SrcPictrPlane3StrdByt,
											   int32_t SrcPictrFmt, int32_t SrcPictrWidth, int32_t SrcPictrHeight,

											   int32_t CropX, int32_t CropY, int32_t CropWidth, int32_t CropHeight,

											   uint8_t * DstPictrPlane1Pt, size_t DstPictrPlane1SzByt, size_t DstPictrPlane1StrdByt, size_t * DstPictrPlane1LenBytPt,
											   uint8_t * DstPictrPlane2Pt, size_t DstPictrPlane2SzByt, size_t DstPictrPlane2StrdByt, size_t * DstPictrPlane2LenBytPt,
											   uint8_t * DstPictrPlane3Pt, size_t DstPictrPlane3SzByt, size_t DstPictrPlane3StrdByt, size_t * DstPictrPlane3LenBytPt,
											   int32_t * DstPictrWidthPt, int32_t * DstPictrHeightPt,

											   Vstr * ErrInfoVstrPt );
__LIBYUV_DLLAPI__ int LibYUVPictrMirror( const uint8_t * SrcPictrPt, int32_t SrcPictrFmt, int32_t SrcPictrWidth, int32_t SrcPictrHeight,
										 int32_t HorizontalOrVertical,
										 uint8_t * DstPictrPt, size_t DstPictrSzByt, size_t * DstPictrLenBytPt, int32_t * DstPictrWidthPt, int32_t * DstPictrHeightPt,
										 Vstr * ErrInfoVstrPt );
__LIBYUV_DLLAPI__ int LibYUVPictrMirrorWithStrd( const uint8_t * SrcPictrPlane1Pt, size_t SrcPictrPlane1StrdByt,
												 const uint8_t * SrcPictrPlane2Pt, size_t SrcPictrPlane2StrdByt,
												 const uint8_t * SrcPictrPlane3Pt, size_t SrcPictrPlane3StrdByt,
												 int32_t SrcPictrFmt, int32_t SrcPictrWidth, int32_t SrcPictrHeight,

												 int32_t HorizontalOrVertical,

												 uint8_t * DstPictrPlane1Pt, size_t DstPictrPlane1SzByt, size_t DstPictrPlane1StrdByt, size_t * DstPictrPlane1LenBytPt,
												 uint8_t * DstPictrPlane2Pt, size_t DstPictrPlane2SzByt, size_t DstPictrPlane2StrdByt, size_t * DstPictrPlane2LenBytPt,
												 uint8_t * DstPictrPlane3Pt, size_t DstPictrPlane3SzByt, size_t DstPictrPlane3StrdByt, size_t * DstPictrPlane3LenBytPt,
												 int32_t * DstPictrWidthPt, int32_t * DstPictrHeightPt,

												 Vstr * ErrInfoVstrPt );
__LIBYUV_DLLAPI__ int LibYUVPictrRotate( const uint8_t * SrcPictrPt, int32_t SrcPictrFmt, int32_t SrcPictrWidth, int32_t SrcPictrHeight,
										 int32_t RotateDegree,
										 uint8_t * DstPictrPt, size_t DstPictrSzByt, int32_t * DstPictrWidthPt, int32_t * DstPictrHeightPt,
										 Vstr * ErrInfoVstrPt );
__LIBYUV_DLLAPI__ int LibYUVPictrScale( const uint8_t * SrcPictrPt, int32_t SrcPictrFmt, int32_t SrcPictrWidth, int32_t SrcPictrHeight,
										int32_t Quality,
										uint8_t * DstPictrPt, size_t DstPictrSzByt, size_t * DstPictrLenBytPt, int32_t DstPictrWidth, int32_t DstPictrHeight,
										Vstr * ErrInfoVstrPt );
__LIBYUV_DLLAPI__ int LibYUVPictrFmtCnvrt( const uint8_t * SrcPictrPt, int32_t SrcPictrFmt, int32_t SrcPictrWidth, int32_t SrcPictrHeight,
										   uint8_t * DstPictrPt, size_t DstPictrSzByt, size_t * DstPictrLenBytPt, int32_t DstPictrFmt,
										   Vstr * ErrInfoVstrPt );
__LIBYUV_DLLAPI__ int LibYUVPictrFmtCnvrtWithStrd( const uint8_t * SrcPictrPlane1Pt, size_t SrcPictrPlane1StrdByt,
												   const uint8_t * SrcPictrPlane2Pt, size_t SrcPictrPlane2StrdByt,
												   const uint8_t * SrcPictrPlane3Pt, size_t SrcPictrPlane3StrdByt,
												   int32_t SrcPictrFmt, int32_t SrcPictrWidth, int32_t SrcPictrHeight,

												   uint8_t * DstPictrPlane1Pt, size_t DstPictrPlane1SzByt, size_t DstPictrPlane1StrdByt, size_t * DstPictrPlane1LenBytPt,
												   uint8_t * DstPictrPlane2Pt, size_t DstPictrPlane2SzByt, size_t DstPictrPlane2StrdByt, size_t * DstPictrPlane2LenBytPt,
												   uint8_t * DstPictrPlane3Pt, size_t DstPictrPlane3SzByt, size_t DstPictrPlane3StrdByt, size_t * DstPictrPlane3LenBytPt,
												   int32_t DstPictrFmt,

												   Vstr * ErrInfoVstrPt );
#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) )
__LIBYUV_DLLAPI__ int LibYUVPictrDrawToWnd( const uint8_t * SrcPictrPt, int32_t SrcPictrFmt, int32_t SrcPictrWidth, int32_t SrcPictrHeight,
											int32_t CenterOrStretch,
											HWND DstWndHdl,
											Vstr * ErrInfoVstrPt );
#endif
#ifdef __cplusplus
}
#endif

#endif