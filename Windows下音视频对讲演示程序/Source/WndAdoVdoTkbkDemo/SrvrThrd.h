#pragma once

#include "Func.h"
#include "DataStruct.h"
#include "Sokt.h"
#include "WndAdoVdoTkbk.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct SrvrThrd //服务端线程。
{
	typedef enum ThrdMsgTyp //线程消息。
	{
		ThrdMsgTypSetIsUsePrvntSysSleep,
		ThrdMsgTypSetIsTstNtwkDly,

		ThrdMsgTypSrvrInit,
		ThrdMsgTypSrvrDstoy,

		ThrdMsgTypCnctDstoy,

		ThrdMsgTypRqirExit,

		ThrdMsgTypUserMsgMinVal = 100, //用户消息的最小值。
	} ThrdMsgTyp;
	typedef struct
	{
		int32_t m_IsUsePrvntSysSleep;
	} ThrdMsgSetIsUsePrvntSysSleep;
	typedef struct
	{
		int32_t m_IsTcpOrAudpPrtcl;
		Vstr * m_LclNodeNameVstrPt;
		Vstr * m_LclNodeSrvcVstrPt;
		int32_t m_MaxCnctNum;
		int32_t m_IsAutoRqirExit;
	} ThrdMsgSrvrInit;
	typedef struct
	{
	} ThrdMsgSrvrDstoy;
	typedef struct
	{
		int32_t m_CnctNum;
	} ThrdMsgCnctDstoy;
	typedef struct
	{
		int32_t m_IsTstNtwkDly;
		uint64_t m_SendIntvlMsec;
	} ThrdMsgSetIsTstNtwkDly;
	typedef struct
	{
	} ThrdMsgRqirExit;
	typedef struct
	{
		uint8_t m_UserMsgArr[];
	} ThrdMsgUserMsg;
	MsgQueue * m_ThrdMsgQueuePt; //存放线程消息队列的指针。
	
	typedef enum PktTyp //数据包类型。
	{
		PktTypTkbkIdx, //对讲索引包。
		PktTypTkbkMode, //对讲模式包。
		PktTypAdoFrm, //音频输入输出帧包。
		PktTypVdoFrm, //视频输入输出帧包。
		PktTypTstNtwkDly, //测试网络延迟包。
		PktTypTstNtwkDlyRply, //测试网络延迟应答包。
		PktTypExit, //退出包。
	} PktTyp;
	
	typedef enum TkbkMode //对讲模式。
	{
		TkbkModeNone = 0, //空。
		TkbkModeAdoInpt = 1, //音频输入。
		TkbkModeAdoOtpt = 2, //音频输出。
		TkbkModeVdoInpt = 4, //视频输入。
		TkbkModeVdoOtpt = 8, //视频输出。
		TkbkModeAdo = TkbkModeAdoInpt | TkbkModeAdoOtpt, //音频。
		TkbkModeVdo = TkbkModeVdoInpt | TkbkModeVdoOtpt, //视频。
		TkbkModeAdoVdo = TkbkModeAdo | TkbkModeVdo, //音视频。
		TkbkModeNoChg = TkbkModeVdoOtpt << 1, //不变。
	} TkbkMode;
	
	typedef enum CnctSts //连接状态。
	{
		CnctStsWait, //等待远端接受连接。
		CnctStsCnct, //已连接。
		CnctStsTmot, //超时未接收任何数据包。异常断开。
		CnctStsDsct, //已断开。
	} CnctSts;
	
	int32_t m_IsRqirExit; //存放是否请求退出，为0表示未请求退出，为1表示已请求退出。
	
	const void * m_LicnCodePt; //存放授权码的指针。

	int32_t m_IsPrintLog; //存放是否打印Log日志，为非0表示要打印，为0表示不打印。
	int32_t m_IsShowToast; //存放是否显示Toast，为非0表示要显示，为0表示不显示。
	HWND m_ShowToastWndHdl; //存放显示Toast窗口的句柄，为NULL表示桌面窗口。

	int32_t m_IsUsePrvntSysSleep; //存放是否使用阻止系统睡眠，为非0表示要使用，为0表示不使用。
	
	struct TstNtwkDly //存放测试网络延迟。
	{
		int32_t m_IsTstNtwkDly; //存放是否测试网络延迟。
		uint64_t m_SendIntvlMsec; //存放发送间隔，单位为毫秒。
	} m_TstNtwkDly;

	int32_t m_SrvrIsInit; //存放服务端是否初始化，为0表示未初始化，为1表示已初始化。
	TcpSrvrSokt * m_TcpSrvrSoktPt; //存放本端Tcp协议服务端套接字的指针。
	AudpSokt * m_AudpSrvrSoktPt; //存放本端高级Udp协议服务端套接字的指针。
	int32_t m_IsAutoRqirExit; //存放是否自动请求退出，为0表示手动，为1表示在所有连接销毁时自动请求退出，为2表示在所有连接和服务端都销毁时自动请求退出。

	typedef struct CnctInfo //连接信息。
	{
		size_t m_Idx; //存放索引，从0开始，连接信息容器的唯一索引，连接中途不会改变。
		int32_t m_IsInit; //存放连接信息是否初始化，为0表示未初始化，为非0表示已初始化。
		int32_t m_Num; //存放序号，从0开始，随着前面的连接销毁而递减。

		int32_t m_IsTcpOrAudpPrtcl; //存放是否是Tcp或Udp协议，为0表示Tcp协议，为1表示高级Udp协议。
		Vstr * m_RmtNodeNameVstrPt; //存放远端套接字绑定的远端节点名称动态字符串的指针。
		Vstr * m_RmtNodeSrvcVstrPt; //存放远端套接字绑定的远端节点服务动态字符串的指针。
		TcpClntSokt * m_TcpClntSoktPt; //存放本端Tcp协议客户端套接字的指针。
		size_t m_AudpClntCnctIdx; //存放本端高级Udp协议客户端连接索引。
		int32_t m_IsRqstDstoy; //存放是否请求销毁，为0表示未请求，为1表示已请求。
		
		int32_t m_CurCnctSts; //存放当前连接状态，为[-m_MaxCnctTimes,0]表示等待远端接受连接。
		int32_t m_RmtTkbkMode; //存放远端对讲模式。

		int32_t m_IsRecvExitPkt; //存放是否接收退出包，为0表示未接收，为1表示已接收。
		
		struct TstNtwkDly //存放测试网络延迟。
		{
			uint64_t m_LastSendTickMsec; //存放最后发送的嘀嗒钟，单位为毫秒。
			int32_t m_IsRecvRplyPkt; //存放是否接收应答包，为0表示未接收，为1表示已接收。
		} m_TstNtwkDly;
	} CnctInfo;
	CQueue * m_CnctInfoCntnrPt; //存放连接信息容器的指针。
	int32_t m_CnctInfoCurMaxNum; //存放连接信息的当前最大序号。
	int32_t m_MaxCnctNum; //存放最大连接数。
	
	struct //存放线程。
	{
		Vstr * m_LclNodeAddrPt; //存放本地节点地址的指针。
		Vstr * m_LclNodePortPt; //存放本地节点端口的指针。
		Vstr * m_RmtNodeAddrPt; //存放远程节点地址的指针。
		Vstr * m_RmtNodePortPt; //存放远程节点端口的指针。

		int8_t * m_TmpBytePt; //存放临时数据的指针。
		size_t m_TmpByteSz; //存放临时数据大小。

		ThrdInfo * m_ThrdInfoPt; //存放线程信息的指针。
	} m_Thrd;
	
	Vstr * m_ErrInfoVstrPt; //存放错误信息动态字符串的指针。

	//用户定义的相关回调函数。

	//存放用户数据的指针。注意：在C++的SrvrThrdCls类中，本变量存放this指针，请勿修改。
	void * m_UserDataPt;
	
	//用户定义的初始化函数。
	typedef void( __cdecl * SrvrThrdUserInitFuncPt )( SrvrThrd * SrvrThrdPt );
	SrvrThrdUserInitFuncPt m_UserInitFuncPt;

	//用户定义的销毁函数。
	typedef void( __cdecl * SrvrThrdUserDstoyFuncPt )( SrvrThrd * SrvrThrdPt );
	SrvrThrdUserDstoyFuncPt m_UserDstoyFuncPt;
	
	//用户定义的处理函数。
	typedef void( __cdecl * SrvrThrdUserPocsFuncPt )( SrvrThrd * SrvrThrdPt );
	SrvrThrdUserPocsFuncPt m_UserPocsFuncPt;
	
	//用户定义的消息函数。
	typedef int( __cdecl * SrvrThrdUserMsgFuncPt )( SrvrThrd * SrvrThrdPt, unsigned int MsgTyp, void * MsgParmPt, size_t MsgParmLenByt );
	SrvrThrdUserMsgFuncPt m_UserMsgFuncPt;
	
	//用户定义的显示日志函数。
	typedef void( __cdecl * SrvrThrdUserShowLogFuncPt )( SrvrThrd * SrvrThrdPt, Vstr * InfoVstrPt );
	SrvrThrdUserShowLogFuncPt m_UserShowLogFuncPt;

	//用户定义的显示Toast函数。
	typedef void( __cdecl * SrvrThrdUserShowToastFuncPt )( SrvrThrd * SrvrThrdPt, Vstr * InfoVstrPt );
	SrvrThrdUserShowToastFuncPt m_UserShowToastFuncPt;
	
	//用户定义的服务端初始化函数。
	typedef void( __cdecl * SrvrThrdUserSrvrInitFuncPt )( SrvrThrd * SrvrThrdPt );
	SrvrThrdUserSrvrInitFuncPt m_UserSrvrInitFuncPt;

	//用户定义的服务端销毁函数。
	typedef void( __cdecl * SrvrThrdUserSrvrDstoyFuncPt )( SrvrThrd * SrvrThrdPt );
	SrvrThrdUserSrvrDstoyFuncPt m_UserSrvrDstoyFuncPt;
	
	//用户定义的连接初始化函数。
	typedef void( __cdecl * SrvrThrdUserCnctInitFuncPt )( SrvrThrd * SrvrThrdPt, CnctInfo * CnctInfoPt, int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt );
	SrvrThrdUserCnctInitFuncPt m_UserCnctInitFuncPt;
	
	//用户定义的连接销毁函数。
	typedef void( __cdecl * SrvrThrdUserCnctDstoyFuncPt )( SrvrThrd * SrvrThrdPt, CnctInfo * CnctInfoPt );
	SrvrThrdUserCnctDstoyFuncPt m_UserCnctDstoyFuncPt;

	//用户定义的连接状态函数。
	typedef void( __cdecl * SrvrThrdUserCnctStsFuncPt )( SrvrThrd * SrvrThrdPt, CnctInfo * CnctInfoPt, int32_t CurCnctSts );
	SrvrThrdUserCnctStsFuncPt m_UserCnctStsFuncPt;
	
	//用户定义的连接远端对讲模式函数。
	typedef void( __cdecl * SrvrThrdUserCnctRmtTkbkModeFuncPt )( SrvrThrd * SrvrThrdPt, CnctInfo * CnctInfoPt, int32_t OldRmtTkbkMode, int32_t NewRmtTkbkMode );
	SrvrThrdUserCnctRmtTkbkModeFuncPt m_UserCnctRmtTkbkModeFuncPt;

	//用户定义的连接测试网络延迟函数。
	typedef void( __cdecl * SrvrThrdUserCnctTstNtwkDlyFuncPt )( SrvrThrd * SrvrThrdPt, CnctInfo * CnctInfoPt, uint64_t NtwkDlyMsec );
	SrvrThrdUserCnctTstNtwkDlyFuncPt m_UserCnctTstNtwkDlyFuncPt;
} SrvrThrd;
extern const char * const g_TkbkModeU8strArrPt[ 17 ];

__WNDADOVDOTKBK_DLLAPI__ int SrvrThrdInit( SrvrThrd * * SrvrThrdPtPt, const void * LicnCodePt, void * UserDataPt,
										   SrvrThrd::SrvrThrdUserInitFuncPt UserInitFuncPt, SrvrThrd::SrvrThrdUserDstoyFuncPt UserDstoyFuncPt,
										   SrvrThrd::SrvrThrdUserPocsFuncPt UserPocsFuncPt, SrvrThrd::SrvrThrdUserMsgFuncPt UserMsgFuncPt,
										   SrvrThrd::SrvrThrdUserShowLogFuncPt UserShowLogFuncPt, SrvrThrd::SrvrThrdUserShowToastFuncPt UserShowToastFuncPt,
										   SrvrThrd::SrvrThrdUserSrvrInitFuncPt UserSrvrInitFuncPt, SrvrThrd::SrvrThrdUserSrvrDstoyFuncPt UserSrvrDstoyFuncPt,
										   SrvrThrd::SrvrThrdUserCnctInitFuncPt UserCnctInitFuncPt, SrvrThrd::SrvrThrdUserCnctDstoyFuncPt UserCnctDstoyFuncPt, SrvrThrd::SrvrThrdUserCnctStsFuncPt UserCnctStsFuncPt, SrvrThrd::SrvrThrdUserCnctRmtTkbkModeFuncPt UserCnctRmtTkbkModeFuncPt, SrvrThrd::SrvrThrdUserCnctTstNtwkDlyFuncPt UserCnctTstNtwkDlyFuncPt,
										   Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int SrvrThrdDstoy( SrvrThrd * SrvrThrdPt, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int SrvrThrdSetIsPrintLogShowToast( SrvrThrd * SrvrThrdPt, int32_t IsPrintLog, int32_t IsShowToast, HWND ShowToastWndHdl, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int SrvrThrdSendSetIsUsePrvntSysSleepMsg( SrvrThrd * SrvrThrdPt, int IsBlockWait, int32_t IsUsePrvntSysSleep, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int SrvrThrdSendSetIsTstNtwkDlyMsg( SrvrThrd * SrvrThrdPt, int IsBlockWait, int32_t IsTstNtwkDly, uint64_t SendIntvlMsec, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int SrvrThrdSendSrvrInitMsg( SrvrThrd * SrvrThrdPt, int IsBlockWait, int32_t IsTcpOrAudpPrtcl, Vstr * LclNodeNameVstrPt, Vstr * LclNodeSrvcVstrPt, int32_t MaxCnctNum, int32_t IsAutoRqirExit, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int SrvrThrdSendSrvrDstoyMsg( SrvrThrd * SrvrThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int SrvrThrdSendCnctDstoyMsg( SrvrThrd * SrvrThrdPt, int IsBlockWait, int32_t CnctNum, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int SrvrThrdStart( SrvrThrd * SrvrThrdPt, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int SrvrThrdSendRqirExitMsg( SrvrThrd * SrvrThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int SrvrThrdSendUserMsg( SrvrThrd * SrvrThrdPt, int IsBlockWait, unsigned int MsgTyp, void * MsgParmPt, size_t MsgParmLenByt, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
__WNDADOVDOTKBK_DLLAPI__ class SrvrThrdCls
{
public:
	SrvrThrd * m_SrvrThrdPt;

	SrvrThrdCls() { m_SrvrThrdPt = NULL; }
	~SrvrThrdCls() { Dstoy( NULL ); }
	
	//用户定义的初始化函数。
	virtual void UserInit() = 0;

	//用户定义的销毁函数。
	virtual void UserDstoy() = 0;
	
	//用户定义的处理函数。
	virtual void UserPocs() = 0;

	//用户定义的消息函数。
	virtual int UserMsg( unsigned int MsgTyp, void * MsgParmPt, size_t MsgParmLenByt ) = 0;
	
	//用户定义的显示日志函数。
	virtual void UserShowLog( Vstr * InfoVstrPt ) = 0;
	
	//用户定义的显示Toast函数。
	virtual void UserShowToast( Vstr * InfoVstrPt ) = 0;
	
	//用户定义的服务端初始化函数。
	virtual void UserSrvrInit() = 0;

	//用户定义的服务端销毁函数。
	virtual void UserSrvrDstoy() = 0;
	
	//用户定义的连接初始化函数。
	virtual void UserCnctInit( SrvrThrd::CnctInfo * CnctInfoPt, int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt ) = 0;
	
	//用户定义的连接销毁函数。
	virtual void UserCnctDstoy( SrvrThrd::CnctInfo * CnctInfoPt ) = 0;

	//用户定义的连接状态函数。
	virtual void UserCnctSts( SrvrThrd::CnctInfo * CnctInfoPt, int32_t CurCnctSts ) = 0;
	
	//用户定义的连接远端对讲模式函数。
	virtual void UserCnctRmtTkbkMode( SrvrThrd::CnctInfo * CnctInfoPt, int32_t OldRmtTkbkMode, int32_t NewRmtTkbkMode ) = 0;
	
	//用户定义的连接测试网络延迟函数。
	virtual void UserCnctTstNtwkDly( SrvrThrd::CnctInfo * CnctInfoPt, uint64_t NtwkDlyMsec ) = 0;

	int Init( const void * LicnCodePt, Vstr * ErrInfoVstrPt );
	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = SrvrThrdDstoy( m_SrvrThrdPt, ErrInfoVstrPt ); m_SrvrThrdPt = NULL; return p_Rslt; }

	int SetIsPrintLogShowToast( int32_t IsPrintLog, int32_t IsShowToast, HWND ShowToastWndHdl, Vstr * ErrInfoVstrPt ) { return SrvrThrdSetIsPrintLogShowToast( m_SrvrThrdPt, IsPrintLog, IsShowToast, ShowToastWndHdl, ErrInfoVstrPt ); }
	int SendSetIsUsePrvntSysSleepMsg( int IsBlockWait, int32_t IsUsePrvntSysSleep, Vstr * ErrInfoVstrPt ) { return SrvrThrdSendSetIsUsePrvntSysSleepMsg( m_SrvrThrdPt, IsBlockWait, IsUsePrvntSysSleep, ErrInfoVstrPt ); }
	int SendSetIsTstNtwkDlyMsg( int IsBlockWait, int32_t IsTstNtwkDly, uint64_t SendIntvlMsec, Vstr * ErrInfoVstrPt ) { return SrvrThrdSendSetIsTstNtwkDlyMsg( m_SrvrThrdPt, IsBlockWait, IsTstNtwkDly, SendIntvlMsec, ErrInfoVstrPt ); }

	int SendSrvrInitMsg( int IsBlockWait, int32_t IsTcpOrAudpPrtcl, Vstr * LclNodeNameVstrPt, Vstr * LclNodeSrvcVstrPt, int32_t MaxCnctNum, int32_t IsAutoRqirExit, Vstr * ErrInfoVstrPt ) { return SrvrThrdSendSrvrInitMsg( m_SrvrThrdPt, IsBlockWait, IsTcpOrAudpPrtcl, LclNodeNameVstrPt, LclNodeSrvcVstrPt, MaxCnctNum, IsAutoRqirExit, ErrInfoVstrPt ); }
	int SendSrvrDstoyMsg( int IsBlockWait, Vstr * ErrInfoVstrPt ) { return SrvrThrdSendSrvrDstoyMsg( m_SrvrThrdPt, IsBlockWait, ErrInfoVstrPt ); }
	int SendCnctDstoyMsg( int IsBlockWait, int32_t CnctNum, Vstr * ErrInfoVstrPt ) { return SrvrThrdSendCnctDstoyMsg( m_SrvrThrdPt, IsBlockWait, CnctNum, ErrInfoVstrPt ); }

	int Start( Vstr * ErrInfoVstrPt ) { return SrvrThrdStart( m_SrvrThrdPt, ErrInfoVstrPt ); }
	int SendRqirExitMsg( int IsBlockWait, Vstr * ErrInfoVstrPt ) { return SrvrThrdSendRqirExitMsg( m_SrvrThrdPt, IsBlockWait, ErrInfoVstrPt ); }
	int SendUserMsg( int IsBlockWait, unsigned int MsgTyp, void * MsgParmPt, size_t MsgParmLenByt, Vstr * ErrInfoVstrPt ) { return SrvrThrdSendUserMsg( m_SrvrThrdPt, IsBlockWait, MsgTyp, MsgParmPt, MsgParmLenByt, ErrInfoVstrPt ); }
};
#endif
