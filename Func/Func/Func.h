#pragma once

#include <stdio.h>							//*printf、*scanf、fopen、fseek、fread、fwrite、fclose、SEEK_CUR、SEEK_END、SEEK_SET
#include <stdlib.h>							//*alloc、free、exit、system、rand
#include <stddef.h>							//size_t、wchar_t、NULL、offsetof
#include <stdint.h>							//int*_t、uint*_t、INT*_MAX、INT*_MIN、UINT*_MAX、UINT*_MIN
#include <stdarg.h>							//va_list、va_start、va_arg、va_end
#include <time.h>							//clock_t、time_t、struct tm、asctime、clock、ctime、difftime、gmtime、localtime、mktime、strftime、time
#include <limits.h>							//CHAR_BIT、SCHAR_MIN、SCHAR_MAX、UCHAR_MAX、CHAR_MIN、CHAR_MAX、MB_LEN_MAX、SHRT_MIN、SHRT_MAX、USHRT_MAX、INT_MIN、INT_MAX、UINT_MAX、LONG_MIN、LONG_MAX、ULONG_MAX、LLONG_MIN、LLONG_MIN、ULLONG_MAX
#include <errno.h>							//errno
#include <string.h>							//memchr、memcmp、memcpy、memmove、memset、strcat、strncat、strchr、strcmp、strncmp、strcoll、strcpy、strncpy、strcspn、strerror、strlen、strpbrk、strrchr、strspn、strstr、strtok、strxfrm
#include <inttypes.h>						//PRId*、PRIi*、PRIu*、PRIo*、PRIx*、PRIX*、SCNd*、SCNi*、SCNu*、SCNo*、SCNx*
#include <locale.h>							//setlocale
#define _USE_MATH_DEFINES					//设置包含数学定义宏
#include <math.h>							//M_E、M_PI

#ifdef __cplusplus
#include <iostream>							//std::cin、std::cout
#include <string>
#ifndef _LIBCPP_HAS_NO_THREADS
#include <thread>							//std::thread
#endif
#include <queue>							//std::queue
#include <list>								//std::list
#include <map>								//std::map
#include <atomic>							//std::atomic
#include <cfloat>							//FLT_MAX、DBL_MAX
#include <typeinfo>
//using namespace std;						//会导致std::byte冲突。
#endif

#if( defined __MS_VCXX__ )
#include <winsdkver.h>						//包含Windows SDK支持的最高版本Windows的相关宏、_WIN32_MAXVER、_WIN32_WINDOWS_MAXVER、NTDDI_MAXVER、_WIN32_IE_MAXVER、_WIN32_WINNT_MAXVER、WINVER_MAXVER、
#define _WIN32_WINNT     0xFFFF				//设置_WIN32_WINNT宏为最高版本Windows
#define WINVER           0xFFFF				//设置WINVER宏为最高版本Windows
#define NTDDI_VERSION    0xFFFFFFFF			//设置NTDDI_VERSION宏为最高版本Windows
#define _WIN32_IE        0xFFFF				//设置_WIN32_IE宏为最高版本IE
#define WIN32_LEAN_AND_MEAN					//设置WIN32_LEAN_AND_MEAN宏，防止windows.h头文件包含winsock.h头文件，从而防止AF_IPX宏重定义
#define NOMINMAX							//设置NOMINMAX宏，防止min和max被定义为宏
#define _CRTDBG_MAP_ALLOC					//设置检查内存泄漏宏
#include <windows.h>						//包含整个Windows SDK支持的API
#include <windowsx.h>						//ComboBox_AddString、ComboBox_ResetContent
#include <commctrl.h>						//ListView_InsertItem、ListView_DeleteItem、ListView_DeleteAllItems、ListView_SetItem、ListView_GetItem
#include <isa_availability.h>				//__ISA_AVAILABLE_X86、__ISA_AVAILABLE_SSE2、__ISA_AVAILABLE_SSE42、__ISA_AVAILABLE_AVX、__ISA_AVAILABLE_ENFSTRG、__ISA_AVAILABLE_AVX2、__ISA_AVAILABLE_AVX512、__ISA_AVAILABLE_ARMNT、__ISA_AVAILABLE_NEON、__ISA_AVAILABLE_NEON_ARM64
#include <tchar.h>							//_tmain、_tWinMain、TCHAR
#include <process.h>						//
#include <psapi.h>							//GetProcessImageFileName
#include <shellapi.h>						//ShellExecute
#include <lmerr.h>							//NERR_BASE、MAX_NERR
#include <wininet.h>						//INTERNET_ERROR_BASE、INTERNET_ERROR_LAST
#include <pdhmsg.h>							//PDH_CSTATUS_NO_MACHINE、PDH_QUERY_PERF_DATA_TIMEOUT
#include <Ws2tcpip.h>						//Windows套接字相关
#include <conio.h>							//getch、getche
#include <endpointvolume.h>
#include <Propkey.h>
#include <wmcodecdsp.h>						//CLSID_CWMAudioAEC
#include <dmort.h>							//MoInitMediaType
#include <Audioclient.h>					//IAudioClient、IAudioCaptureClient、IAudioRenderClient、WAVE_FORMAT_EXTENSIBLE
#include <audiopolicy.h>					//IAudioSessionControl、IAudioSessionManager
#include <Functiondiscoverykeys_devpkey.h>	//PKEY_Device_FriendlyName、PKEY_Device_DeviceDesc、PKEY_DeviceInterface_FriendlyName
#include <Objbase.h>						//CoInitialize、CoInitializeEx
#include <shlwapi.h>						//DLLVERSIONINFO
#include <crtdbg.h>							//_CrtSetBreakAlloc、_CrtSetDbgFlag、_CrtCheckMemory、_CrtDumpMemoryLeaks。
#include <intrin.h>							//_mm_pause、__cpuid、__cpuidex
#include <io.h>								//_open、_wopen
#include <sys/stat.h>						//struct stat、fstat、_stat64、_wstat64
#include <sys/types.h>						//ino_t、dev_t、off_t
#include <fcntl.h>							//O_RDONLY、O_WRONLY、O_Rd_Wr、O_APPEND、O_CREAT、O_TRUNC、O_EXCL、O_TEXT、O_BINARY、O_RAW、O_TEMPORARY、O_NOINHERIT、O_SEQUENTIAL、O_RANDOM
#include <dbt.h>							//DEV_BROADCAST_DEVICEINTERFACE
#include <Dshow.h>							//waveInGetNumDevs、waveInGetDevCaps、waveInOpen、waveInPrepareHeader、waveInAddBuffer、waveInStart、waveInUnprepareHeader、waveInReset、waveInClose、WIM_OPEN、WIM_DATA、WIM_CLOSE、waveOutGetNumDevs、waveOutGetDevCaps、waveOutOpen、waveOutPrepareHeader、waveOutAddBuffer、waveOutStart、waveOutUnprepareHeader、waveOutReset、waveOutClose、WOM_OPEN、WOM_DATA、WOM_CLOSE
#include <mmsystem.h>						//MMRESULT
#include <initguid.h>						//必须放在mmdeviceapi.h前面，其他的后面。
#include <mmdeviceapi.h>					//IMMDevice、IMMDeviceEnumerator、IMMDeviceCollection、IMMNotificationClient、PROPVARIANT、

#undef StrCpy
#undef StrTrim
#undef StrToInt

//Windows Core Audio API的相关宏。
#define CLSID_MMDeviceEnumerator __uuidof( MMDeviceEnumerator )
#define IID_IMMDeviceEnumerator __uuidof( IMMDeviceEnumerator )
#define IID_IAudioClient __uuidof( IAudioClient )
#define IID_IAudioClient2 __uuidof( IAudioClient2 )
#define IID_IAudioCaptureClient __uuidof( IAudioCaptureClient )
#define IID_IAudioRenderClient __uuidof( IAudioRenderClient )
#define IID_IAcousticEchoCancellationControl __uuidof( IAcousticEchoCancellationControl )

//获取位置结构体的宽度和高度。
#define RectWidth( Rect ) ( ( Rect ).right - ( Rect ).left )
#define RectHeight( Rect ) ( ( Rect ).bottom - ( Rect ).top )

//屏幕位置转客户区位置。
#define RectScrnToClnt( ClntWndHdl, Rect ) ( ScreenToClient( ClntWndHdl, ( LPPOINT )&( ( Rect ).left ) ), ScreenToClient( ClntWndHdl, ( LPPOINT )&( ( Rect ).right ) ) )

//屏幕位置转客户区位置。
#define RectClntToScrn( ClntWndHdl, Rect ) ( ClientToScreen( ClntWndHdl, ( LPPOINT )&( ( Rect ).left ) ), ClientToScreen( ClntWndHdl, ( LPPOINT )&( ( Rect ).right ) ) )

#endif

#if( defined __CYGWIN_GCC__ )
#include <winsdkver.h>						//包含Windows SDK支持的最高版本Windows的相关宏
#define _WIN32_WINNT     0xFFFF				//设置_WIN32_WINNT宏为最高版本Windows
#define WINVER           0xFFFF				//设置WINVER宏为最高版本Windows
#define NTDDI_VERSION    0xFFFFFFFF			//设置NTDDI_VERSION宏为最高版本Windows
#define _WIN32_IE        0xFFFF				//设置_WIN32_IE宏为最高版本IE
#define WIN32_LEAN_AND_MEAN					//设置WIN32_LEAN_AND_MEAN宏，防止windows.h头文件包含winsock.h头文件，从而防止AF_IPX宏重定义
#include <windows.h>						//包含整个Windows SDK支持的API
#include <windowsx.h>						//ComboBox_AddString、ComboBox_ResetContent
#include <tchar.h>							//_tmain、_tWinMain、TCHAR
#include <process.h>						//
#include <psapi.h>							//GetProcessImageFileName
#include <shellapi.h>						//ShellExecute
#include <lmerr.h>							//NERR_BASE、MAX_NERR
#include <wininet.h>						//INTERNET_ERROR_BASE、INTERNET_ERROR_LAST
#include <pdhmsg.h>							//PDH_CSTATUS_NO_MACHINE、PDH_QUERY_PERF_DATA_TIMEOUT
//#include <Ws2tcpip.h>						//不包含该头文件，因为会与Linux套接字相关声明冲突。
//#include <conio.h>						//不包含该头文件，因为Cygwin下没有。
//#include <Dshow.h>						//不包含该头文件，因为会编译报错。
#include <mmsystem.h>						//MMRESULT
#include <mmdeviceapi.h>					//IMMDevice、IMMDeviceEnumerator、IMMDeviceCollection、PROPVARIANT、
#include <endpointvolume.h>
#include <Propkey.h>
//#include <wmcodecdsp.h>					//CLSID_CWMAudioAEC
//#include <dmort.h>						//MoInitMediaType
#include <Audioclient.h>					//IAudioClient、IAudioCaptureClient、IAudioRenderClient、WAVE_FORMAT_EXTENSIBLE
#include <audiopolicy.h>					//IAudioSessionControl、IAudioSessionManager
#include <Functiondiscoverykeys_devpkey.h>	//PKEY_Device_FriendlyName、PKEY_Device_DeviceDesc、PKEY_DeviceInterface_FriendlyName
#include <Objbase.h>						//CoInitialize、CoInitializeEx
#include <shlwapi.h>						//DLLVERSIONINFO
//#include <crtdbg.h>						//不包含该头文件，因为Cygwin下没有。
#include <immintrin.h>						//_mm_pause
#include <io.h>								//_open、_wopen
#include <sys/types.h>						//ushort、uint、ulong、blkcnt_t、blksize_t、clock_t、time_t、daddr_t、caddr_t、fsblkcnt_t、fsfilcnt_t、id_t、ino_t、addr_t、vm_offset_t、vm_size_t、off_t、dev_t、uid_t、gid_t、pid_t、key_t、ssize_t、mode_t、nlink_t、clockid_t、timer_t、useconds_t、suseconds_t、sbintime_t
#include <sys/socket.h>						//socket、bind、listen、accept、connect、getpeername、getsockname、send、sendto、sendmsg、recv、recvfrom、recvmsg、setsockopt、getsockopt、shutdown
#include <netinet/tcp.h>					//TCP_NODELAY
#include <sys/select.h>						//fd_set、FD_SETSIZE、FD_ZERO、FD_SET、FD_ISSET、FD_CLR、FD_COPY、select
#include <sys/ioctl.h>						//ioctl
#include <sys/stat.h>						//struct stat、fstat、chmod、fchmod、mkdir、stat
#include <sys/time.h>						//gettimeofday
#include <fcntl.h>							//open、fcntl
#include <unistd.h>							//getpid、getopt、close
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>						//pthread_create、pthread_join
#include <intrin.h>							//_mm_pause、__cpuid、__cpuidex

#undef StrCpy
#undef StrTrim
#undef StrToInt

//Windows Core Audio API的相关宏。
#define CLSID_MMDeviceEnumerator __uuidof( MMDeviceEnumerator )
#define IID_IMMDeviceEnumerator __uuidof( IMMDeviceEnumerator )
#define IID_IAudioClient __uuidof( IAudioClient )
#define IID_IAudioCaptureClient __uuidof( IAudioCaptureClient )
#define IID_IAudioRenderClient __uuidof( IAudioRenderClient )
#endif

#if( defined __LINUX_GCC__ )
#include <sys/types.h>						//ushort、uint、ulong、blkcnt_t、blksize_t、clock_t、time_t、daddr_t、caddr_t、fsblkcnt_t、fsfilcnt_t、id_t、ino_t、addr_t、vm_offset_t、vm_size_t、off_t、dev_t、uid_t、gid_t、pid_t、key_t、ssize_t、mode_t、nlink_t、clockid_t、timer_t、useconds_t、suseconds_t、sbintime_t
#include <sys/socket.h>						//socket、bind、listen、accept、connect、getpeername、getsockname、send、sendto、sendmsg、recv、recvfrom、recvmsg、setsockopt、getsockopt、shutdown
#include <netinet/tcp.h>					//TCP_NODELAY
#include <sys/select.h>						//fd_set、FD_SETSIZE、FD_ZERO、FD_SET、FD_ISSET、FD_CLR、FD_COPY、select
#include <sys/ioctl.h>						//ioctl
#include <sys/syscall.h>
#include <sys/stat.h>						//struct stat、fstat、chmod、fchmod、mkdir、stat
#include <sys/time.h>						//gettimeofday
#include <fcntl.h>							//open、fcntl
#include <unistd.h>							//getpid、getopt、close
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>						//pthread_create、pthread_join
#include <immintrin.h>						//_mm_pause

#define gettid() ( pid_t )syscall( SYS_gettid )

typedef int HANDLE;
#define Sleep( Msec ) usleep( ( Msec ) * 1000 )

#define __cdecl __attribute__( ( __cdecl__ ) )
#define __stdcall __attribute__( ( __stdcall__ ) )
#endif

#if( defined __ANDROID_GCC__ )
#include <sys/types.h>						//ushort、uint、ulong、blkcnt_t、blksize_t、clock_t、time_t、daddr_t、caddr_t、fsblkcnt_t、fsfilcnt_t、id_t、ino_t、addr_t、vm_offset_t、vm_size_t、off_t、dev_t、uid_t、gid_t、pid_t、key_t、ssize_t、mode_t、nlink_t、clockid_t、timer_t、useconds_t、suseconds_t、sbintime_t
#include <sys/socket.h>						//socket、bind、listen、accept、connect、getpeername、getsockname、send、sendto、sendmsg、recv、recvfrom、recvmsg、setsockopt、getsockopt、shutdown
#include <netinet/tcp.h>					//TCP_NODELAY
#include <sys/select.h>						//fd_set、FD_SETSIZE、FD_ZERO、FD_SET、FD_ISSET、FD_CLR、FD_COPY、select
#include <asm/ioctls.h>						//ioctl
#include <sys/syscall.h>
#include <sys/stat.h>						//struct stat、fstat、chmod、fchmod、mkdir、stat
#include <sys/time.h>						//gettimeofday
#include <fcntl.h>							//open、fcntl
#include <unistd.h>							//getpid、getopt、close
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>						//pthread_create、pthread_join
#if( ( defined __X86__ ) || ( defined __X64__ ) )
#include <immintrin.h>						//_mm_pause
#endif

typedef int HANDLE;
#define Sleep( Msec ) usleep( ( Msec ) * 1000 )

#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <sys/system_properties.h>
#endif

#if( defined __KEIL_ARMC__ )
#include <alloca.h>
#endif

#ifndef ssize_t
#if( ( defined __X86__ ) || ( defined __ARMAT32__ ) )
typedef int32_t ssize_t;
#elif( ( defined __X64__ ) || ( defined __ARMA64__ ) )
typedef int64_t ssize_t;
#endif
#endif

//路径字符串最大长度。
#undef MAX_PATH
#undef PATH_MAX
#define MAX_PATH PATH_MAX
#define PATH_MAX 1024

//线程局部变量宏。
#if( defined __MS_VCXX__ )
#define __thread __declspec( thread )
#endif

//交换两个指针变量。
#define SWAPPT( Pt1, Pt2 ) { size_t Pt; Pt = ( size_t )Pt1; *( ( size_t * )&Pt1 ) = ( size_t )Pt2; *( ( size_t * )&Pt2 ) = Pt; }

//函数默认参数。
#define DEFARG( Type, Name, DefVal ) ( ( #Name[0] ) ? ( Type )( Name + 0 ) : ( DefVal ) )

//计算字节对齐。
#define ALIGNUP( Val, ByteAlign ) ( ( ( ( Val ) & ~( ( ByteAlign ) - 1 ) ) != ( ~( ( ByteAlign ) - 1 ) ) ) ? ( ( ( Val ) + ( ByteAlign ) - 1 ) & ( ~( ( ByteAlign ) - 1 ) ) ) : ( ~( ( ByteAlign ) - 1 ) ) ) //向上字节对齐。
#define ALIGNDOWN( Val, ByteAlign ) ( ( Val ) & ~( ( ByteAlign ) - 1 ) ) //向下字节对齐。

//设置结束指针，指向最后一个元素的后面一个元素。
#define SetEndPt( StartTyp, StartPt, SzTyp, EndTyp, EndPt ) \
{ \
	if( ( ( SIZE_MAX - ( size_t )( StartPt ) ) / sizeof( StartTyp ) ) > ( SzTyp ) ) EndPt = ( EndTyp * )( ( StartTyp * )( StartPt )+( SzTyp ) ); /*不会溢出。*/\
	else EndPt = ( EndTyp * )( ( StartTyp * )( SIZE_MAX - sizeof( StartTyp ) ) ); /*会溢出。*/ \
}

//获取数组的大小。
#define SzOfArr( Arr ) ( sizeof( Arr ) / sizeof( *( Arr ) ) )

//字符串字符集。
#if( defined __MS_VCXX__ ) //MSVC++编译器要使用UTF-16字符集，不要使用多字节字符集，因为国际间不通用。
#define L( Str ) L ## Str
#elif( defined __CYGWIN_GCC__ ) || ( defined __LINUX_GCC__ ) || ( defined __ANDROID_GCC__ ) //GCC编译器要使用UTF-8字符集，不要使用UTF-32字符集，因为UTF-32字符集太占空间。
#define L( Str ) L ## Str
#endif

//计算最大值和最小值。
#ifndef __cplusplus
#ifndef max
#define max( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#endif
#ifndef min
#define min( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#endif
#endif

//数据类型。
typedef enum DataTyp
{
	DtChar,
	DtUChar,
	DtShort,
	DtUShort,
	DtInt,
	DtUInt,
	DtLong,
	DtULong,
	DtLLong,
	DtULLong,
	DtFlt,
	DtDbl,
	DtLDbl,
	DtOther,
} DataTyp;

#ifndef __cplusplus
#define GetDataTyp( x ) ( _Generic( ( x ), \
	char : DtChar, \
	unsigned char : DtUChar, \
	short : DtShort, \
	unsigned short : DtUShort, \
	int : DtInt, \
	unsigned int : DtUInt, \
	long : DtLong, \
	unsigned long : DtULong, \
	long long : DtLLong, \
	unsigned long long : DtULLong, \
	float : DtFlt, \
	double : DtDbl, \
	long double : DtLDbl, \
	default : DtOther \
	) )
#else
#define GetDataTyp( x ) ( \
	( typeid( x ) == typeid( char ) ) ? DtChar : \
	( typeid( x ) == typeid( unsigned char ) ) ? DtUChar : \
	( typeid( x ) == typeid( short ) ) ? DtShort : \
	( typeid( x ) == typeid( unsigned short ) ) ? DtUShort : \
	( typeid( x ) == typeid( int ) ) ? DtInt : \
	( typeid( x ) == typeid( unsigned int ) ) ? DtUInt : \
	( typeid( x ) == typeid( long ) ) ? DtLong : \
	( typeid( x ) == typeid( unsigned long ) ) ? DtULong : \
	( typeid( x ) == typeid( long long ) ) ? DtLLong : \
	( typeid( x ) == typeid( unsigned long long ) ) ? DtULLong : \
	( typeid( x ) == typeid( float ) ) ? DtFlt : \
	( typeid( x ) == typeid( double ) ) ? DtDbl : \
	( typeid( x ) == typeid( long double ) ) ? DtLDbl : \
	DtOther \
	)
#endif

//方向。
typedef enum Drct_t
{
	NoDrct,    //没有方向。
	LftToRit,  //从左向右。
	RitToLft,  //从右向左。
}Drct_t;

//位置。
typedef enum Pos_t
{
	NoPos = 0,                    //没有位置。
	Lft = 1,                      //左边。
	Mid = 2,                      //中间。
	Rit = 4,                      //右边。
	LftRit = Lft | Rit,           //左边和右边。
	LftMidRit = Lft | Mid | Rit,  //左边、中间和右边。
}Pos_t;

//字符集。
typedef enum ChrSet
{
	Utf8  = 1,  //Utf8字符集。
	Utf16 = 2,  //Utf16字符集。
	Utf32 = 4,  //Utf32字符集。
}ChrSet;
//检查字符集参数是否不正确。
#define CHKCHRSET( ChrSet ) ( ( ChrSet != Utf8 ) && ( ChrSet != Utf16 ) && ( ChrSet != Utf32 ) )

//缓冲区自动调整方式。
typedef enum BufAutoAdjMeth
{
	BufAutoAdjMethNone,        //缓冲区不自动调整。
	BufAutoAdjMethFreeAtio,    //缓冲区按空闲与已用的比率自动调整。
	BufAutoAdjMethFreeNumber,  //缓冲区按空闲的个数自动调整。
}BufAutoAdjMeth;

//Func项目的DLL动态库文件导入导出符号宏。
#if( defined __NAME_FUNC__ ) //如果正在编译Func项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __FUNC_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __FUNC_DLLAPI__ __declspec( dllexport )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __FUNC_DLLAPI__
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __FUNC_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __FUNC_DLLAPI__ __attribute__( ( visibility( "default" ) ) )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __FUNC_DLLAPI__
		#endif
	#else //如果正在使用未知编译器。
		#define __FUNC_DLLAPI__
	#endif
#else //如果正在编译其他项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __LNKLIB__ ) //如果正在链接LIB静态库文件。
			#define __FUNC_DLLAPI__
		#elif( defined __LNKDLL__ ) //如果正在链接DLL动态库文件。
			#define __FUNC_DLLAPI__ __declspec( dllimport )
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#define __FUNC_DLLAPI__
	#else //如果正在使用未知编译器。
		#define __FUNC_DLLAPI__
	#endif
#endif

#include "Vstr.h"
#include "Log.h"
#include "ThrdLock.h"
#include "ChrStrMem.h"
#include "Utf.h"
#include "DateTime.h"
#include "File.h"
#include "PocsThrd.h"
#include "Toast.h"
#include "Ado.h"
#if( defined __ANDROID_GCC__ )
#include "JavaNtvIntfc.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

//Func初始化函数。
__FUNC_DLLAPI__ int FuncInit();

//CPU信息。
#if( ( defined __X86__ ) || ( defined __X64__ ) )
__FUNC_DLLAPI__ void __cdecl GetCpuId( uint32_t Eax, uint32_t Ecx, uint32_t * EaxPt, uint32_t * EbxPt, uint32_t * EcxPt, uint32_t * EdxPt );
__FUNC_DLLAPI__ extern char g_CpuNameStrArr[ 4 * 4 * 3 + 1 ]; //Cpu名称字符串数组。
__FUNC_DLLAPI__ extern uint16_t g_CpuCacheLineSzByt; //Cpu缓存行的大小，单位为字节。
__FUNC_DLLAPI__ extern int8_t g_CpuIsSupt64Bit; //Cpu是否支持64位。
__FUNC_DLLAPI__ extern int8_t g_CpuIsSuptEnhancedFastStringsRepMovsbStosb; //Cpu是否支持增强的快速字符串Rep Movsb/Stosb指令。
__FUNC_DLLAPI__ extern int8_t g_CpuIsSuptMMX; //Cpu是否支持MMX指令。
__FUNC_DLLAPI__ extern int8_t g_CpuIsSuptSSE; //Cpu是否支持SSE指令。
__FUNC_DLLAPI__ extern int8_t g_CpuIsSuptSSE2; //Cpu是否支持SSE2指令。
__FUNC_DLLAPI__ extern int8_t g_CpuIsSuptSSE3; //Cpu是否支持SSE3指令。
__FUNC_DLLAPI__ extern int8_t g_CpuIsSuptSSSE3; //Cpu是否支持SSSE3指令。
__FUNC_DLLAPI__ extern int8_t g_CpuIsSuptSSE4_1; //Cpu是否支持SSE4.1指令。
__FUNC_DLLAPI__ extern int8_t g_CpuIsSuptSSE4_2; //Cpu是否支持SSE4.2指令。
__FUNC_DLLAPI__ extern int8_t g_CpuIsSuptAVX; //Cpu是否支持AVX指令。
__FUNC_DLLAPI__ extern int8_t g_CpuIsSuptAVX2; //Cpu是否支持AVX2指令。
__FUNC_DLLAPI__ extern int8_t g_CpuIsSuptAVX512F; //Cpu是否支持AVX512F指令。
#elif( ( defined __ARMAT32__ ) || ( defined __ARMA64__ ) )
#endif

//获取函数。
#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) )
__FUNC_DLLAPI__ int FuncGetPrivateProfileStringFromSectionMem( const char * SectionMemPt, const char * KeyStrPt, const char * DfltValStrPt, char * ValStrPt, size_t ValStrSzByt, size_t * ValStrLenBytPt );
#endif

//设置函数。
__FUNC_DLLAPI__ int FuncGetCurExePath( Vstr * FullAbsPathVstrPt, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ int FuncSetCurActPath( const Vstr * FullPathVstrPt, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ int FuncGetCurActPath( Vstr * FullAbsPathVstrPt, Vstr * ErrInfoVstrPt );
#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) )
__FUNC_DLLAPI__ int FuncSetWndCenter( HWND SpecWndHdl, HWND OtherWndHdl, Vstr * ErrInfoVstrPt );
#endif

//判断函数。
#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) )
__FUNC_DLLAPI__ int FuncIsForegroundFullscreen();
#endif

//管道函数。
#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) )
__FUNC_DLLAPI__ int FuncCreatePipe( HANDLE * PipeReadHdl, HANDLE * PipeWriteHdl, LPVOID * SecurityDescriptor, BOOL InheritHdl, DWORD BufSz );
__FUNC_DLLAPI__ int FuncReadPipe( HANDLE PipeReadHdl, char * * BufPtPt, size_t * BufSzBytPt, size_t * BufLenBytPt, const char * EndMemPt, size_t EndMemLenByt, int IsAllowRealloc, uint64_t TmotMsec );
__FUNC_DLLAPI__ void FuncClosePipe( HANDLE PipeReadHdl, HANDLE PipeWriteHdl );
#endif

//休眠函数。
#if( defined __MS_VCXX__ )
#define FuncSleep( SleepTimeMesc ) WaitForSingleObject( ( HANDLE )-1, SleepTimeMesc )
#elif( ( defined __CYGWIN_GCC__ ) || ( defined __LINUX_GCC__ ) || ( defined __ANDROID_GCC__ ) )
#define FuncSleep( SleepTimeMesc ) usleep( SleepTimeMesc * 1000 )
#endif

#ifdef __cplusplus
}
#endif
