#include "Func.h"

#ifndef __LOG_H__
#define __LOG_H__

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

//日志头项目。
typedef enum LogHeadItem
{
	LogHeadRmk = 1,	  //备注。
	LogHeadTime = 2,  //日志打印时间。
	LogHeadLvl = 3,	  //日志级别。
	LogHeadFile = 4,  //源文件、函数和行号。
	LogHeadPid = 5,	  //进程ID。
	LogHeadTid = 6,	  //线程ID。
	LogHeadIntvl = 7, //距上次打印日志的间隔时间。
	LogHeadEnd = 0    //日志头列表结束。
}LogHeadItem;

#define DFT_LOG_HEAD_LST      LogHeadTime,LogHeadIntvl,LogHeadFile,LogHeadPid,LogHeadTid,LogHeadLvl,LogHeadEnd   //默认日志头列表。

#ifdef __cplusplus
extern "C"
{
#endif

__FUNC_DLLAPI__ int LogInit( Vstr * ErrInfoVstrPt );

__FUNC_DLLAPI__ int LogOpenFile( const Vstr * LogFilePathVstrPt, size_t * FileNumPt, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ int LogSetDfltFile( size_t FileNum, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ int LogSetLvl( int Lvl, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ void LogSetHeadLst( LogHeadItem HeadItem, ... );


#define LOG( Lvl, TextVstrPt ) LogWrite( SIZE_MAX, Lvl, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, TextVstrPt )
#define LOGE( TextVstrPt ) LogWrite( SIZE_MAX, LOG_LVL_ERROR, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, TextVstrPt )
#define LOGW( TextVstrPt ) LogWrite( SIZE_MAX, LOG_LVL_WARN, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, TextVstrPt )
#define LOGI( TextVstrPt ) LogWrite( SIZE_MAX, LOG_LVL_INFO, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, TextVstrPt )
#define LOGD( TextVstrPt ) LogWrite( SIZE_MAX, LOG_LVL_DEBUG, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, TextVstrPt )

#define LOGN( FileNum, Lvl, TextVstrPt ) LogWrite( FileNum, Lvl, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, TextVstrPt )
#define LOGNE( FileNum, TextVstrPt ) LogWrite( FileNum, LOG_LVL_ERROR, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, TextVstrPt )
#define LOGNW( FileNum, TextVstrPt ) LogWrite( FileNum, LOG_LVL_WARN, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, TextVstrPt )
#define LOGNI( FileNum, TextVstrPt ) LogWrite( FileNum, LOG_LVL_INFO, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, TextVstrPt )
#define LOGND( FileNum, TextVstrPt ) LogWrite( FileNum, LOG_LVL_DEBUG, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, TextVstrPt )

__FUNC_DLLAPI__ void LogWrite( size_t FileNum, int Lvl, const uint8_t * SrcFileNameStrPt, uint64_t SrcFileLine, const uint8_t * FuncNameStrPt, const Vstr * TextVstrPt );


#define LOGF( Lvl, FmtVstrPt, ... ) LogFmtWrite( SIZE_MAX, Lvl, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, FmtVstrPt, __VA_ARGS__ )
#define LOGFE( FmtVstrPt, ... ) LogFmtWrite( SIZE_MAX, LOG_LVL_ERROR, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, FmtVstrPt, __VA_ARGS__ )
#define LOGFW( FmtVstrPt, ... ) LogFmtWrite( SIZE_MAX, LOG_LVL_WARN, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, FmtVstrPt, __VA_ARGS__ )
#define LOGFI( FmtVstrPt, ... ) LogFmtWrite( SIZE_MAX, LOG_LVL_INFO, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, FmtVstrPt, __VA_ARGS__ )
#define LOGFD( FmtVstrPt, ... ) LogFmtWrite( SIZE_MAX, LOG_LVL_DEBUG, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, FmtVstrPt, __VA_ARGS__ )

#define LOGNF( FileNum, Lvl, FmtVstrPt, ... ) LogFmtWrite( FileNum, Lvl, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, FmtVstrPt, __VA_ARGS__ )
#define LOGNFE( FileNum, FmtVstrPt, ... ) LogFmtWrite( FileNum, LOG_LVL_ERROR, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, FmtVstrPt, __VA_ARGS__ )
#define LOGNFW( FileNum, FmtVstrPt, ... ) LogFmtWrite( FileNum, LOG_LVL_WARN, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, FmtVstrPt, __VA_ARGS__ )
#define LOGNFI( FileNum, FmtVstrPt, ... ) LogFmtWrite( FileNum, LOG_LVL_INFO, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, FmtVstrPt, __VA_ARGS__ )
#define LOGNFD( FileNum, FmtVstrPt, ... ) LogFmtWrite( FileNum, LOG_LVL_DEBUG, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, FmtVstrPt, __VA_ARGS__ )

__FUNC_DLLAPI__ void LogFmtWrite( size_t FileNum, int Lvl, const uint8_t * SrcFileNameStrPt, uint64_t SrcFileLine, const uint8_t * FuncNameStrPt, const Vstr * FmtVstrPt, ... );


#define LOGH( Lvl, LineLen, DataPt, DataSz ) LogHexWrite( SIZE_MAX, Lvl, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, LineLen, DataPt, DataSz )
#define LOGHE( LineLen, DataPt, DataSz ) LogHexWrite( SIZE_MAX, LOG_LVL_ERROR, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, LineLen, DataPt, DataSz )
#define LOGHW( LineLen, DataPt, DataSz ) LogHexWrite( SIZE_MAX, LOG_LVL_WARN, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, LineLen, DataPt, DataSz )
#define LOGHI( LineLen, DataPt, DataSz ) LogHexWrite( SIZE_MAX, LOG_LVL_INFO, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, LineLen, DataPt, DataSz )
#define LOGHD( LineLen, DataPt, DataSz ) LogHexWrite( SIZE_MAX, LOG_LVL_DEBUG, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, LineLen, DataPt, DataSz )

#define LOGHN( FileNum, Lvl, LineLen, DataPt, DataSz ) LogHexWrite( FileNum, Lvl, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, LineLen, DataPt, DataSz )
#define LOGHNE( FileNum, LineLen, DataPt, DataSz ) LogHexWrite( FileNum, LOG_LVL_ERROR, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, LineLen, DataPt, DataSz )
#define LOGHNW( FileNum, LineLen, DataPt, DataSz ) LogHexWrite( FileNum, LOG_LVL_WARN, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, LineLen, DataPt, DataSz )
#define LOGHNI( FileNum, LineLen, DataPt, DataSz ) LogHexWrite( FileNum, LOG_LVL_INFO, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, LineLen, DataPt, DataSz )
#define LOGHND( FileNum, LineLen, DataPt, DataSz ) LogHexWrite( FileNum, LOG_LVL_DEBUG, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, LineLen, DataPt, DataSz )

__FUNC_DLLAPI__ void LogHexWrite( size_t FileNum, int Lvl, const uint8_t * SrcFileNameStrPt, uint64_t SrcFileLine, const uint8_t * FuncNameStrPt, size_t LineLen, const void * DataPt, size_t DataSz );


#define LOGHF( Lvl, LineLen, FmtVstrPt, ... ) LogHexFmtWrite( SIZE_MAX, Lvl, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, LineLen, FmtVstrPt, __VA_ARGS__ )
#define LOGHFE( LineLen, FmtVstrPt, ... ) LogHexFmtWrite( SIZE_MAX, LOG_LVL_ERROR, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, LineLen, FmtVstrPt, __VA_ARGS__ )
#define LOGHFW( LineLen, FmtVstrPt, ... ) LogHexFmtWrite( SIZE_MAX, LOG_LVL_WARN, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, LineLen, FmtVstrPt, __VA_ARGS__ )
#define LOGHFI( LineLen, FmtVstrPt, ... ) LogHexFmtWrite( SIZE_MAX, LOG_LVL_INFO, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, LineLen, FmtVstrPt, __VA_ARGS__ )
#define LOGHFD( LineLen, FmtVstrPt, ... ) LogHexFmtWrite( SIZE_MAX, LOG_LVL_DEBUG, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, LineLen, FmtVstrPt, __VA_ARGS__ )

#define LOGHNF( FileNum, Lvl, LineLen, FmtVstrPt, ... ) LogHexFmtWrite( FileNum, Lvl, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, LineLen, FmtVstrPt, __VA_ARGS__ )
#define LOGHNFE( FileNum, LineLen, FmtVstrPt, ... ) LogHexFmtWrite( FileNum, LOG_LVL_ERROR, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, LineLen, FmtVstrPt, __VA_ARGS__ )
#define LOGHNFW( FileNum, LineLen, FmtVstrPt, ... ) LogHexFmtWrite( FileNum, LOG_LVL_WARN, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, LineLen, FmtVstrPt, __VA_ARGS__ )
#define LOGHNFI( FileNum, LineLen, FmtVstrPt, ... ) LogHexFmtWrite( FileNum, LOG_LVL_INFO, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, LineLen, FmtVstrPt, __VA_ARGS__ )
#define LOGHNFD( FileNum, LineLen, FmtVstrPt, ... ) LogHexFmtWrite( FileNum, LOG_LVL_DEBUG, ( const uint8_t * )__FILE__, __LINE__, ( const uint8_t * )__FUNCTION__, LineLen, FmtVstrPt, __VA_ARGS__ )

__FUNC_DLLAPI__ void LogHexFmtWrite( size_t FileNum, int Lvl, const uint8_t * SrcFileNameStrPt, uint64_t SrcFileLine, const uint8_t * FuncNameStrPt, size_t LineLen, const Vstr * FmtVstrPt, ... );


__FUNC_DLLAPI__ void LogDstoy();

#ifdef __cplusplus
}
#endif

#endif
