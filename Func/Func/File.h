#include "Func.h"

#ifndef __FILE_H__
#define __FILE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __LINUX_GCC__ ) || ( defined __ANDROID_GCC__ ) )
//文件。
typedef struct File File;

//文件权限模式。
typedef enum FilePrmsn
{
	NoRd_NoWr = 0,  //不读不写。
	Rd_NoWr = 1,    //可读不写。
	NoRd_Wr = 2,    //不读可写。
	Rd_Wr = 3,      //可读可写。
	NoRd_Apwr = 4,  //不读可追加写。
	Rd_Apwr = 5     //可读可追加写。
}FilePrmsn;

//文件创建模式。
typedef enum FileCreate
{
	NoCreate = 0,            //不创建。
	Create_NoExist = 1,       //创建，不允许存在。
	Create_AlExist_NoClr = 2,  //创建，允许存在，不清空。
	Create_AlExist_Clr = 3,    //创建，允许存在，要清空，需要可写权限。
}FileCreate;

//文件起始位置。
typedef enum FileOrgnPos
{
	StartPos = SEEK_SET,  //文件开头的位置，也就是文件的第一个字节。
	CurPos = SEEK_CUR,    //文件当前的位置。
	EndPos = SEEK_END,    //文件结尾的位置，也就是文件最后一个字节的后一个字节。
}FileOrgnPos;

//新建、读取、写入文件。
__FUNC_DLLAPI__ int FileInitByPath( File * * FilePtPt, const Vstr * FullPathVstrPt, FilePrmsn PrmsnMode, FileCreate CreateMode, size_t WrBufSzByt, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ int FileInitByFd( File * * FilePtPt, int32_t Fd, size_t WrBufSzByt, Vstr * ErrInfoVstrPt );

__FUNC_DLLAPI__ int FileLocked( File * FilePt, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ int FileUnlock( File * FilePt, Vstr * ErrInfoVstrPt );

__FUNC_DLLAPI__ int FileGetFd( File * FilePt, int32_t * FdPt, Vstr * ErrInfoVstrPt );

__FUNC_DLLAPI__ int FileSetPos( File * FilePt, FileOrgnPos OrgnPos, uint64_t Offset, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ int FileGetPos( File * FilePt, uint64_t * StartOffsetPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__FUNC_DLLAPI__ int FileRead( File * FilePt, void * DataBufPt, size_t MaxReadLen, size_t * DataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ int FileWrite( File * FilePt, const void * DataBufPt, size_t DataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ int FileFmtWrite( File * FilePt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt, const Vstr * FmtVstrPt, ... );
__FUNC_DLLAPI__ int FileFlush( File * FilePt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__FUNC_DLLAPI__ int FileSetSz( File * FilePt, uint64_t SzByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ int FileGetSz( File * FilePt, uint64_t * SzBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__FUNC_DLLAPI__ int FileDstoy( File * FilePt, int32_t IsDstoyFd, Vstr * ErrInfoVstrPt );
#endif

//删除文件。
__FUNC_DLLAPI__ int DelFile( const Vstr * FullPathVstrPt, Vstr * ErrInfoVstrPt );

//获取文件大小。
#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __LINUX_GCC__ ) || ( defined __ANDROID_GCC__ ) )
__FUNC_DLLAPI__ int FuncGetFileSzByPath( const Vstr * FullPathVstrPt, uint64_t * FileSzBytPt, Vstr * ErrInfoVstrPt );
#endif
#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) )
__FUNC_DLLAPI__ int FuncGetFileSzByHdl( HANDLE FileHdl, uint64_t * FileSzBytPt, Vstr * ErrInfoVstrPt );
#endif

//路径字符串查找文件名。
__FUNC_DLLAPI__ int FuncPathFindFileName( const Vstr * FullPathVstrPt, const void * * FileNameStrPtPt );

//路径字符串移除文件名。
__FUNC_DLLAPI__ int FuncPathRmvFileName( Vstr * FullPathVstrPt );

#ifdef __cplusplus
}
#endif

#endif
