#ifndef __LOG_H__
#define __LOG_H__

#include "VarStr.h"

//日志级别
#define LOG_LVL_ERROR                 1   //错误级别
#define LOG_LVL_WARN                  2   //警告级别
#define LOG_LVL_INFO                  3   //信息级别
#define LOG_LVL_DEBUG                 4   //调试级别
#define LOG_LVL_DEFAULT   LOG_LVL_DEBUG   //默认级别

//日志文件备份模式
#define LOG_BACKUP_MODE_NONE         1              //不备份
#define LOG_BACKUP_MODE_SIZE         2              //按文件大小备份，文件大小参考LOG_BACKUP_DEFAULT_FILESIZE宏
#define LOG_BACKUP_MODE_HOUR         3              //按一个小时为一个日志文件备份
#define LOG_BACKUP_MODE_DAY          4              //按一天为一个日志文件备份

#define LOG_BACKUP_MODE_DEFAULT      LOG_BACKUP_MODE_HOUR   //默认的日志备份模式

#define LOG_BACKUP_DEFAULT_FILESIZE          1024*1024*32   //默认的日志文件备份大小，单位字节，日志文件一旦达到这么大就备份，然后创建一个新的日志文件

//查找最老日志文件的判定条件
#define LOG_FIND_FILE_MAXLEN         1      //所有的日志文件的总大小达到多大，包括正在使用的日志
#define LOG_FIND_FILE_FREEPERCENT    2      //日志文件所在磁盘的剩余空间的百分比
#define LOG_FIND_FILE_DAYSBEFORE     3      //日志文件的最后修改时间相对当前系统时间在多少天以前
#define LOG_FIND_FILE_END            0      //判定条件参数结束

//自定义日志头函数动态参数
#define LOG_HEAD_REMARK       1   //备注。
#define LOG_HEAD_TIME         2   //日志打印时间。
#define LOG_HEAD_LEVEL        3   //日志级别。
#define LOG_HEAD_FILE         4   //源文件、函数和行号。
#define LOG_HEAD_PID          5   //进程PID。
#define LOG_HEAD_TID          6   //线程TID。
#define LOG_HEAD_INTERVALS    7   //距上次打印日志的间隔时间。
#define LOG_HEAD_END          0   //自定义日志头函数参数结束。

#define LOG_DEFAULT_HEAD      LOG_HEAD_TIME,LOG_HEAD_INTERVALS,LOG_HEAD_FILE,LOG_HEAD_PID,LOG_HEAD_TID,LOG_HEAD_LEVEL,LOG_HEAD_END   //默认日志头。

#ifdef __cplusplus
extern "C"
{
#endif
	
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

__FUNC_DLLAPI__ int LogInit( VarStr * ErrInfoVarStrPt );
__FUNC_DLLAPI__ int LogOpenFile( const char * LogFilePathStrPt, size_t * FileNumPt, VarStr * ErrInfoVarStrPt );
__FUNC_DLLAPI__ int LogSetDfltFile( size_t FileNum, VarStr * ErrInfoVarStrPt );
__FUNC_DLLAPI__ int LogSetLvl( int8_t Lvl, VarStr * ErrInfoVarStrPt );
__FUNC_DLLAPI__ void LogSetHeadLst( int HeadLst, ... );


#define LOG( Lvl, Text ) LogWrite( SIZE_MAX, Lvl, __FILE__, __LINE__, __FUNCTION__, Text )
#define LOGE( Text ) LogWrite( SIZE_MAX, LOG_LVL_ERROR, __FILE__, __LINE__, __FUNCTION__, Text )
#define LOGW( Text ) LogWrite( SIZE_MAX, LOG_LVL_WARN, __FILE__, __LINE__, __FUNCTION__, Text )
#define LOGI( Text ) LogWrite( SIZE_MAX, LOG_LVL_INFO, __FILE__, __LINE__, __FUNCTION__, Text )
#define LOGD( Text ) LogWrite( SIZE_MAX, LOG_LVL_DEBUG, __FILE__, __LINE__, __FUNCTION__, Text )

#define LOGN( FileNum, Lvl, Text ) LogWrite( FileNum, Lvl, __FILE__, __LINE__, __FUNCTION__, Text )
#define LOGNE( FileNum, Text ) LogWrite( FileNum, LOG_LVL_ERROR, __FILE__, __LINE__, __FUNCTION__, Text )
#define LOGNW( FileNum, Text ) LogWrite( FileNum, LOG_LVL_WARN, __FILE__, __LINE__, __FUNCTION__, Text )
#define LOGNI( FileNum, Text ) LogWrite( FileNum, LOG_LVL_INFO, __FILE__, __LINE__, __FUNCTION__, Text )
#define LOGND( FileNum, Text ) LogWrite( FileNum, LOG_LVL_DEBUG, __FILE__, __LINE__, __FUNCTION__, Text )

__FUNC_DLLAPI__ void LogWrite( size_t FileNum, int8_t Lvl, const char * SrcFileNameStrPt, long SrcFileLine, const char * FuncNameStrPt, const char * TextStrPt );


#define LOGF( Lvl, Fmt, ... ) LogFmtWrite( SIZE_MAX, Lvl, __FILE__, __LINE__, __FUNCTION__, Fmt, __VA_ARGS__ )
#define LOGFE( Fmt, ... ) LogFmtWrite( SIZE_MAX, LOG_LVL_ERROR, __FILE__, __LINE__, __FUNCTION__, Fmt, __VA_ARGS__ )
#define LOGFW( Fmt, ... ) LogFmtWrite( SIZE_MAX, LOG_LVL_WARN, __FILE__, __LINE__, __FUNCTION__, Fmt, __VA_ARGS__ )
#define LOGFI( Fmt, ... ) LogFmtWrite( SIZE_MAX, LOG_LVL_INFO, __FILE__, __LINE__, __FUNCTION__, Fmt, __VA_ARGS__ )
#define LOGFD( Fmt, ... ) LogFmtWrite( SIZE_MAX, LOG_LVL_DEBUG, __FILE__, __LINE__, __FUNCTION__, Fmt, __VA_ARGS__ )

#define LOGNF( FileNum, Lvl, Fmt, ... ) LogFmtWrite( FileNum, Lvl, __FILE__, __LINE__, __FUNCTION__, Fmt, __VA_ARGS__ )
#define LOGNFE( FileNum, Fmt, ... ) LogFmtWrite( FileNum, LOG_LVL_ERROR, __FILE__, __LINE__, __FUNCTION__, Fmt, __VA_ARGS__ )
#define LOGNFW( FileNum, Fmt, ... ) LogFmtWrite( FileNum, LOG_LVL_WARN, __FILE__, __LINE__, __FUNCTION__, Fmt, __VA_ARGS__ )
#define LOGNFI( FileNum, Fmt, ... ) LogFmtWrite( FileNum, LOG_LVL_INFO, __FILE__, __LINE__, __FUNCTION__, Fmt, __VA_ARGS__ )
#define LOGNFD( FileNum, Fmt, ... ) LogFmtWrite( FileNum, LOG_LVL_DEBUG, __FILE__, __LINE__, __FUNCTION__, Fmt, __VA_ARGS__ )

__FUNC_DLLAPI__ void LogFmtWrite( size_t FileNum, int8_t Lvl, const char * SrcFileNameStrPt, long SrcFileLine, const char * FuncNameStrPt, const char * FmtStrPt, ... );


#define LOGH( Lvl, Text, TextSz ) LogHexWrite( SIZE_MAX, Lvl, __FILE__, __LINE__, __FUNCTION__, Text, TextSz )
#define LOGHE( Text, TextSz ) LogHexWrite( SIZE_MAX, LOG_LVL_ERROR, __FILE__, __LINE__, __FUNCTION__, Text, TextSz )
#define LOGHW( Text, TextSz ) LogHexWrite( SIZE_MAX, LOG_LVL_WARN, __FILE__, __LINE__, __FUNCTION__, Text, TextSz )
#define LOGHI( Text, TextSz ) LogHexWrite( SIZE_MAX, LOG_LVL_INFO, __FILE__, __LINE__, __FUNCTION__, Text, TextSz )
#define LOGHD( Text, TextSz ) LogHexWrite( SIZE_MAX, LOG_LVL_DEBUG, __FILE__, __LINE__, __FUNCTION__, Text, TextSz )

#define LOGHN( FileNum, Lvl, Text, TextSz ) LogHexWrite( FileNum, Lvl, __FILE__, __LINE__, __FUNCTION__, Text, TextSz )
#define LOGHNE( FileNum, Text, TextSz ) LogHexWrite( FileNum, LOG_LVL_ERROR, __FILE__, __LINE__, __FUNCTION__, Text, TextSz )
#define LOGHNW( FileNum, Text, TextSz ) LogHexWrite( FileNum, LOG_LVL_WARN, __FILE__, __LINE__, __FUNCTION__, Text, TextSz )
#define LOGHNI( FileNum, Text, TextSz ) LogHexWrite( FileNum, LOG_LVL_INFO, __FILE__, __LINE__, __FUNCTION__, Text, TextSz )
#define LOGHND( FileNum, Text, TextSz ) LogHexWrite( FileNum, LOG_LVL_DEBUG, __FILE__, __LINE__, __FUNCTION__, Text, TextSz )

__FUNC_DLLAPI__ void LogHexWrite( size_t FileNum, int8_t Lvl, const char * SrcFileNameStrPt, long SrcFileLine, const char * FuncNameStrPt, const char * TextPt, size_t TextSz );


#define LOGHF( Lvl, Fmt, ... ) LogHexFmtWrite( SIZE_MAX, Lvl, __FILE__, __LINE__, __FUNCTION__, Fmt, __VA_ARGS__ )
#define LOGHFE( Fmt, ... ) LogHexFmtWrite( SIZE_MAX, LOG_LVL_ERROR, __FILE__, __LINE__, __FUNCTION__, Fmt, __VA_ARGS__ )
#define LOGHFW( Fmt, ... ) LogHexFmtWrite( SIZE_MAX, LOG_LVL_WARN, __FILE__, __LINE__, __FUNCTION__, Fmt, __VA_ARGS__ )
#define LOGHFI( Fmt, ... ) LogHexFmtWrite( SIZE_MAX, LOG_LVL_INFO, __FILE__, __LINE__, __FUNCTION__, Fmt, __VA_ARGS__ )
#define LOGHFD( Fmt, ... ) LogHexFmtWrite( SIZE_MAX, LOG_LVL_DEBUG, __FILE__, __LINE__, __FUNCTION__, Fmt, __VA_ARGS__ )

#define LOGHNF( FileNum, Lvl, Fmt, ... ) LogHexFmtWrite( FileNum, Lvl, __FILE__, __LINE__, __FUNCTION__, Fmt, __VA_ARGS__ )
#define LOGHNFE( FileNum, Fmt, ... ) LogHexFmtWrite( FileNum, LOG_LVL_ERROR, __FILE__, __LINE__, __FUNCTION__, Fmt, __VA_ARGS__ )
#define LOGHNFW( FileNum, Fmt, ... ) LogHexFmtWrite( FileNum, LOG_LVL_WARN, __FILE__, __LINE__, __FUNCTION__, Fmt, __VA_ARGS__ )
#define LOGHNFI( FileNum, Fmt, ... ) LogHexFmtWrite( FileNum, LOG_LVL_INFO, __FILE__, __LINE__, __FUNCTION__, Fmt, __VA_ARGS__ )
#define LOGHNFD( FileNum, Fmt, ... ) LogHexFmtWrite( FileNum, LOG_LVL_DEBUG, __FILE__, __LINE__, __FUNCTION__, Fmt, __VA_ARGS__ )

__FUNC_DLLAPI__ void LogHexFmtWrite( size_t FileNum, int8_t Lvl, const char * SrcFileNameStrPt, long SrcFileLine, const char * FuncNameStrPt, const char * FmtStrPt, ... );


__FUNC_DLLAPI__ void LogDestroy();

#ifdef __cplusplus
}
#endif

#endif
