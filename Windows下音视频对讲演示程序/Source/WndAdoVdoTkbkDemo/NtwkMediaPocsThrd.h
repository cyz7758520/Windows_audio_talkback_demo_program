#pragma once

#include "Func.h"
#include "DataStruct.h"
#include "Sokt.h"
#include "Ajb.h"
#include "MediaPocsThrd.h"
#include "TkbkNtwk.h"
#include "BdctNtwk.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct NtwkMediaPocsThrd //网络媒体处理线程。
{
	MediaPocsThrd * m_MediaPocsThrdPt; //存放媒体处理线程的指针。

	typedef enum PktTyp //数据包类型。
	{
		PktTypTkbkMode, //对讲模式包。
		PktTypAdoFrm, //音频输入输出帧包。
		PktTypVdoFrm, //视频输入输出帧包。
		PktTypExit, //退出包。
	} PktTyp;
	
	typedef enum TkbkMode //对讲模式。
	{
		TkbkModeNone, //空。
		TkbkModeAdo, //音频。
		TkbkModeVdo, //视频。
		TkbkModeAdoVdo = TkbkModeAdo | TkbkModeVdo, //音视频。
		TkbkModeNoChg, //不变。
	} TkbkMode;

	typedef enum CnctSts //连接状态。
	{
		CnctStsWait, //等待远端接受连接。
		CnctStsCnct, //已连接。
		CnctStsTmot, //超时未接收到任何数据包。异常断开。
		CnctStsDsct, //已断开。
	} CnctSts;
	
    TkbkNtwk m_TkbkNtwk; //存放对讲网络。
    BdctNtwk m_BdctNtwk; //存放广播网络。
	
    int m_LclTkbkMode; //存放本端对讲模式。
    int m_MaxCnctTimes; //存放最大连接次数，取值区间为[1,2147483647]。
    int m_IsAutoRqirExit; //存放是否自动请求退出，为0表示手动，为1表示在所有连接销毁时自动请求退出，为2表示在所有连接和服务端都销毁时自动请求退出。
    AudpSokt * m_AudpClntSoktPt; //存放本端高级Udp协议客户端套接字的指针。

	//临时变量。
	Vstr * m_LclNodeAddrPt; //存放本地节点地址的指针。
	Vstr * m_LclNodePortPt; //存放本地节点端口的指针。
	Vstr * m_RmtNodeAddrPt; //存放远程节点地址的指针。
	Vstr * m_RmtNodePortPt; //存放远程节点端口的指针。

	int8_t * m_TmpBytePt; //存放临时数据的指针。
	size_t m_TmpByteSz; //存放临时数据大小。
	int8_t * m_TmpByte2Pt; //存放临时数据的指针。
	size_t m_TmpByte2Sz; //存放临时数据大小。
	int8_t * m_TmpByte3Pt; //存放临时数据的指针。
	size_t m_TmpByte3Sz; //存放临时数据大小。
	
	typedef enum UserMsgTyp //用户消息。
	{
		UserMsgTypSrvrInit, //服务端初始化。
		UserMsgTypSrvrDstoy, //服务端销毁。
		UserMsgTypCnctInit, //连接初始化。
		UserMsgTypCnctAct, //连接激活。
		UserMsgTypCnctDstoy, //连接销毁。
		UserMsgTypLclTkbkMode, //本端对讲模式。
		UserMsgTypPttBtnDown, //一键即按即通按钮按下。
		UserMsgTypPttBtnUp, //一键即按即通按钮弹起。
		UserMsgTypBdctCnctInit, //广播连接初始化。
		UserMsgTypBdctCnctAllDstoy, //广播连接全部销毁。
	} UserMsgTyp;
	typedef struct UserMsgSrvrInit
	{
		UserMsgTyp m_UserMsgTyp;
		Vstr * m_SrvrUrlVstrPt;
	} UserMsgSrvrInit;
	typedef struct UserMsgSrvrDstoy
	{
		UserMsgTyp m_UserMsgTyp;
	} UserMsgSrvrDstoy;
	typedef struct UserMsgCnctInit
	{
		UserMsgTyp m_UserMsgTyp;
		int m_IsTcpOrAudpPrtcl;
		Vstr * m_RmtNodeNameVstrPt;
		Vstr * m_RmtNodeSrvcVstrPt;
	} UserMsgCnctInit;
	typedef struct UserMsgCnctAct
	{
		UserMsgTyp m_UserMsgTyp;
		int m_CnctNum;
	} UserMsgCnctAct;
	typedef struct UserMsgCnctDstoy
	{
		UserMsgTyp m_UserMsgTyp;
		int m_CnctNum;
	} UserMsgCnctDstoy;
	typedef struct UserMsgLclTkbkMode
	{
		UserMsgTyp m_UserMsgTyp;
		int m_LclTkbkMode;
	} UserMsgLclTkbkMode;
	typedef struct UserMsgPttBtnDown
	{
		UserMsgTyp m_UserMsgTyp;
	} UserMsgPttBtnDown;
	typedef struct UserMsgPttBtnUp
	{
		UserMsgTyp m_UserMsgTyp;
	} UserMsgPttBtnUp;
	typedef struct UserMsgBdctCnctInit
	{
		UserMsgTyp m_UserMsgTyp;
		int m_IsTcpOrAudpPrtcl;
		Vstr * m_RmtNodeNameVstrPt;
		Vstr * m_RmtNodeSrvcVstrPt;
	} UserMsgBdctCnctInit;
	typedef struct UserMsgBdctCnctAllDstoy
	{
		UserMsgTyp m_UserMsgTyp;
	} UserMsgBdctCnctAllDstoy;
	typedef struct UserMsg
	{
		UserMsgTyp m_UserMsgTyp;
		uint8_t m_UserMsgArr[];
	} UserMsg;

	//用户定义的相关回调函数。

	//存放用户数据的指针。注意：在C++的NtwkMediaPocsThrdCls类中，本变量存放this指针，请勿修改。
	void * m_UserDataPt;
	
	//用户定义的网络媒体处理线程初始化函数。
	typedef void( __cdecl * NtwkMediaPocsThrdUserNtwkMediaPocsThrdInitFuncPt )( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt );
	NtwkMediaPocsThrdUserNtwkMediaPocsThrdInitFuncPt m_UserNtwkMediaPocsThrdInitFuncPt;

	//用户定义的网络媒体处理线程销毁函数。
	typedef void( __cdecl * NtwkMediaPocsThrdUserNtwkMediaPocsThrdDstoyFuncPt )( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt );
	NtwkMediaPocsThrdUserNtwkMediaPocsThrdDstoyFuncPt m_UserNtwkMediaPocsThrdDstoyFuncPt;
	
	//用户定义的服务端初始化函数。
	typedef void( __cdecl * NtwkMediaPocsThrdUserSrvrInitFuncPt )( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt );
	NtwkMediaPocsThrdUserSrvrInitFuncPt m_UserSrvrInitFuncPt;

	//用户定义的服务端销毁函数。
	typedef void( __cdecl * NtwkMediaPocsThrdUserSrvrDstoyFuncPt )( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt );
	NtwkMediaPocsThrdUserSrvrDstoyFuncPt m_UserSrvrDstoyFuncPt;
	
	//用户定义的显示日志函数。
	typedef void( __cdecl * NtwkMediaPocsThrdUserShowLogFuncPt )( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, Vstr * InfoVstrPt );
	NtwkMediaPocsThrdUserShowLogFuncPt m_UserShowLogFuncPt;

	//用户定义的显示Toast函数。
	typedef void( __cdecl * NtwkMediaPocsThrdUserShowToastFuncPt )( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, Vstr * InfoVstrPt );
	NtwkMediaPocsThrdUserShowToastFuncPt m_UserShowToastFuncPt;
	
	//用户定义的连接初始化函数。
	typedef void( __cdecl * NtwkMediaPocsThrdUserCnctInitFuncPt )( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, TkbkNtwk::CnctInfo * CnctInfoPt, int IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt );
	NtwkMediaPocsThrdUserCnctInitFuncPt m_UserCnctInitFuncPt;
	
	//用户定义的连接状态函数。
	typedef void( __cdecl * NtwkMediaPocsThrdUserCnctStsFuncPt )( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, TkbkNtwk::CnctInfo * CnctInfoPt, int CurCnctSts );
	NtwkMediaPocsThrdUserCnctStsFuncPt m_UserCnctStsFuncPt;
	
	//用户定义的连接激活函数。
	typedef void( __cdecl * NtwkMediaPocsThrdUserCnctActFuncPt )( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, TkbkNtwk::CnctInfo * CnctInfoPt, int IsAct );
	NtwkMediaPocsThrdUserCnctActFuncPt m_UserCnctActFuncPt;

	//用户定义的连接本端对讲模式函数。
	typedef void( __cdecl * NtwkMediaPocsThrdUserCnctLclTkbkModeFuncPt )( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, TkbkNtwk::CnctInfo * CnctInfoPt, int IsRqirAct, int LclTkbkMode );
	NtwkMediaPocsThrdUserCnctLclTkbkModeFuncPt m_UserCnctLclTkbkModeFuncPt;
	
	//用户定义的连接远端对讲模式函数。
	typedef void( __cdecl * NtwkMediaPocsThrdUserCnctRmtTkbkModeFuncPt )( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, TkbkNtwk::CnctInfo * CnctInfoPt, int IsRqirAct, int RmtTkbkMode );
	NtwkMediaPocsThrdUserCnctRmtTkbkModeFuncPt m_UserCnctRmtTkbkModeFuncPt;
	
	//用户定义的连接销毁函数。
	typedef void( __cdecl * NtwkMediaPocsThrdUserCnctDstoyFuncPt )( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, TkbkNtwk::CnctInfo * CnctInfoPt );
	NtwkMediaPocsThrdUserCnctDstoyFuncPt m_UserCnctDstoyFuncPt;
} NtwkMediaPocsThrd;
extern const char * const g_TkbkModeU8strArrPt[ 5 ];

int NtwkMediaPocsThrdInit( NtwkMediaPocsThrd * * NtwkMediaPocsThrdPtPt, void * UserDataPt,
						   NtwkMediaPocsThrd::NtwkMediaPocsThrdUserNtwkMediaPocsThrdInitFuncPt UserNtwkMediaPocsThrdInitFuncPt, NtwkMediaPocsThrd::NtwkMediaPocsThrdUserNtwkMediaPocsThrdDstoyFuncPt UserNtwkMediaPocsThrdDstoyFuncPt,
						   NtwkMediaPocsThrd::NtwkMediaPocsThrdUserSrvrInitFuncPt UserSrvrInitFuncPt, NtwkMediaPocsThrd::NtwkMediaPocsThrdUserSrvrDstoyFuncPt UserSrvrDstoyFuncPt,
						   NtwkMediaPocsThrd::NtwkMediaPocsThrdUserShowLogFuncPt UserShowLogFuncPt, NtwkMediaPocsThrd::NtwkMediaPocsThrdUserShowToastFuncPt UserShowToastFuncPt,
						   NtwkMediaPocsThrd::NtwkMediaPocsThrdUserCnctInitFuncPt UserCnctInitFuncPt, NtwkMediaPocsThrd::NtwkMediaPocsThrdUserCnctStsFuncPt UserCnctStsFuncPt, NtwkMediaPocsThrd::NtwkMediaPocsThrdUserCnctActFuncPt UserCnctActFuncPt, NtwkMediaPocsThrd::NtwkMediaPocsThrdUserCnctLclTkbkModeFuncPt UserCnctLclTkbkModeFuncPt, NtwkMediaPocsThrd::NtwkMediaPocsThrdUserCnctRmtTkbkModeFuncPt UserCnctRmtTkbkModeFuncPt, NtwkMediaPocsThrd::NtwkMediaPocsThrdUserCnctDstoyFuncPt UserCnctDstoyFuncPt,
						   Vstr * ErrInfoVstrPt );
int NtwkMediaPocsThrdDstoy( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, Vstr * ErrInfoVstrPt );
int NtwkMediaPocsThrdSrvrInit( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, int IsBlockWait, Vstr * SrvrUrlVstrPt, Vstr * ErrInfoVstrPt );
int NtwkMediaPocsThrdSrvrDstoy( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt );
int NtwkMediaPocsThrdCnctInit( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, int IsBlockWait, int IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt, Vstr * ErrInfoVstrPt );
int NtwkMediaPocsThrdCnctAct( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, int IsBlockWait, int CnctNum, Vstr * ErrInfoVstrPt );
int NtwkMediaPocsThrdCnctDstoy( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, int IsBlockWait, int CnctNum, Vstr * ErrInfoVstrPt );
int NtwkMediaPocsThrdLclTkbkMode( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, int IsBlockWait, int LclTkbkMode, Vstr * ErrInfoVstrPt );
int NtwkMediaPocsThrdPttBtnDown( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt );
int NtwkMediaPocsThrdPttBtnUp( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt );
int NtwkMediaPocsThrdBdctCnctInit( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, int IsBlockWait, int IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt, Vstr * ErrInfoVstrPt );
int NtwkMediaPocsThrdBdctCnctAllDstoy( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt );
void NtwkMediaPocsThrdIsAutoRqirExit( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, Vstr * ErrInfoVstrPt );
void NtwkMediaPocsThrdSetTkbkMode( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt );
int NtwkMediaPocsThrdUserInit( MediaPocsThrd * MediaPocsThrdPt );
int NtwkMediaPocsThrdUserPocs( MediaPocsThrd * MediaPocsThrdPt );
void NtwkMediaPocsThrdUserDstoy( MediaPocsThrd * MediaPocsThrdPt );
int NtwkMediaPocsThrdUserMsg( MediaPocsThrd * MediaPocsThrdPt, void * UserMsgPt );
void NtwkMediaPocsThrdUserReadAdoVdoInptFrm( MediaPocsThrd * MediaPocsThrdPt,
											 int16_t * AdoInptPcmSrcFrmPt, int16_t * AdoInptPcmRsltFrmPt, size_t AdoInptPcmFrmLenUnit, int32_t AdoInptPcmRsltFrmVoiceActSts,
											 uint8_t * AdoInptEncdRsltFrmPt, size_t AdoInptEncdRsltFrmLenByt, int32_t AdoInptEncdRsltFrmIsNeedTrans,
											 uint8_t * VdoInptBgraSrcFrmPt, int32_t VdoInptBgraSrcFrmWidth, int32_t VdoInptBgraSrcFrmHeight, size_t VdoInptBgraSrcFrmLenByt,
											 uint8_t * VdoInptYu12RsltFrmPt, int32_t VdoInptYu12RsltFrmWidth, int32_t VdoInptYu12RsltFrmHeight, size_t VdoInptYu12RsltFrmLenByt,
											 uint8_t * VdoInptEncdRsltFrmPt, size_t VdoInptEncdRsltFrmLenByt );
void NtwkMediaPocsThrdUserWriteAdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx,
										   int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
										   uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmSzByt, size_t * AdoOtptEncdSrcFrmLenBytPt );
void NtwkMediaPocsThrdUserGetAdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx,
										 int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
										 uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmLenByt );
void NtwkMediaPocsThrdUserWriteVdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx,
										   uint8_t * VdoOtptYu12SrcFrmPt, int32_t * VdoOtptYu12SrcFrmWidthPt, int32_t * VdoOtptYu12SrcFrmHeightPt,
										   uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmSzByt, size_t * VdoOtptEncdSrcFrmLenBytPt );
void NtwkMediaPocsThrdUserGetVdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx,
										 uint8_t * VdoOtptYu12SrcFrmPt, int32_t VdoOtptYu12SrcFrmWidth, int32_t VdoOtptYu12SrcFrmHeight,
										 uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmLenByt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class NtwkMediaPocsThrdCls
{
public:
	NtwkMediaPocsThrd * m_NtwkMediaPocsThrdPt;
	MediaPocsThrdCls * m_MediaPocsThrdClsPt;

	NtwkMediaPocsThrdCls() { m_NtwkMediaPocsThrdPt = NULL; m_MediaPocsThrdClsPt = NULL; }
	~NtwkMediaPocsThrdCls() { Dstoy( NULL ); }
	
	//用户定义的网络媒体处理线程初始化函数。
	virtual void UserNtwkMediaPocsThrdInit() = 0;

	//用户定义的网络媒体处理线程销毁函数。
	virtual void UserNtwkMediaPocsThrdDstoy() = 0;
	
	//用户定义的服务端初始化函数。
	virtual void UserSrvrInit() = 0;

	//用户定义的服务端销毁函数。
	virtual void UserSrvrDstoy() = 0;
	
	//用户定义的显示日志函数。
	virtual void UserShowLog( Vstr * InfoVstrPt ) = 0;

	//用户定义的显示Toast函数。
	virtual void UserShowToast( Vstr * InfoVstrPt ) = 0;
	
	//用户定义的连接初始化函数。
	virtual void UserCnctInit( TkbkNtwk::CnctInfo * CnctInfoPt, int IsSrvrOrClntCnct, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt ) = 0;
	
	//用户定义的连接状态函数。
	virtual void UserCnctSts( TkbkNtwk::CnctInfo * CnctInfoPt, int CurCnctSts ) = 0;
	
	//用户定义的连接激活函数。
	virtual void UserCnctAct( TkbkNtwk::CnctInfo * CnctInfoPt, int IsAct ) = 0;

	//用户定义的连接本端对讲模式函数。
	virtual void UserCnctLclTkbkMode( TkbkNtwk::CnctInfo * CnctInfoPt, int IsRqirAct, int LclTkbkMode ) = 0;
	
	//用户定义的连接远端对讲模式函数。
	virtual void UserCnctRmtTkbkMode( TkbkNtwk::CnctInfo * CnctInfoPt, int IsRqirAct, int RmtTkbkMode ) = 0;
	
	//用户定义的连接销毁函数。
	virtual void UserCnctDstoy( TkbkNtwk::CnctInfo * CnctInfoPt ) = 0;

	int Init( VstrCls * ErrInfoVstrPt );
	int Dstoy( VstrCls * ErrInfoVstrPt ) { int p_Rslt = NtwkMediaPocsThrdDstoy( m_NtwkMediaPocsThrdPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); m_NtwkMediaPocsThrdPt = NULL; m_MediaPocsThrdClsPt = NULL; return p_Rslt; }

	int SrvrInit( int IsBlockWait, VstrCls * SrvrUrlVstrPt, VstrCls * ErrInfoVstrPt ) { return NtwkMediaPocsThrdSrvrInit( m_NtwkMediaPocsThrdPt, IsBlockWait, ( SrvrUrlVstrPt != NULL ) ? SrvrUrlVstrPt->m_VstrPt : NULL, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SrvrDstoy( int IsBlockWait, VstrCls * ErrInfoVstrPt ) { return NtwkMediaPocsThrdSrvrDstoy( m_NtwkMediaPocsThrdPt, IsBlockWait, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int CnctInit( int IsBlockWait, int IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt, VstrCls * ErrInfoVstrPt ) { return NtwkMediaPocsThrdCnctInit( m_NtwkMediaPocsThrdPt, IsBlockWait, IsTcpOrAudpPrtcl, RmtNodeNameVstrPt, RmtNodeSrvcVstrPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int CnctAct( int IsBlockWait, int CnctNum, VstrCls * ErrInfoVstrPt ) { return NtwkMediaPocsThrdCnctAct( m_NtwkMediaPocsThrdPt, IsBlockWait, CnctNum, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int CnctDstoy( int IsBlockWait, int CnctNum, VstrCls * ErrInfoVstrPt ) { return NtwkMediaPocsThrdCnctDstoy( m_NtwkMediaPocsThrdPt, IsBlockWait, CnctNum, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int LclTkbkMode( int IsBlockWait, int LclTkbkMode, VstrCls * ErrInfoVstrPt ) { return NtwkMediaPocsThrdLclTkbkMode( m_NtwkMediaPocsThrdPt, IsBlockWait, LclTkbkMode, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int PttBtnDown( int IsBlockWait, VstrCls * ErrInfoVstrPt ) { return NtwkMediaPocsThrdPttBtnDown( m_NtwkMediaPocsThrdPt, IsBlockWait, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int PttBtnUp( int IsBlockWait, VstrCls * ErrInfoVstrPt ) { return NtwkMediaPocsThrdPttBtnUp( m_NtwkMediaPocsThrdPt, IsBlockWait, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int BdctCnctInit( int IsBlockWait, int IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt, VstrCls * ErrInfoVstrPt ) { return NtwkMediaPocsThrdBdctCnctInit( m_NtwkMediaPocsThrdPt, IsBlockWait, IsTcpOrAudpPrtcl, RmtNodeNameVstrPt, RmtNodeSrvcVstrPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int BdctCnctAllDstoy( int IsBlockWait, VstrCls * ErrInfoVstrPt ) { return NtwkMediaPocsThrdBdctCnctAllDstoy( m_NtwkMediaPocsThrdPt, IsBlockWait, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	void SetTkbkMode( int IsBlockWait, VstrCls * ErrInfoVstrPt ) { return NtwkMediaPocsThrdSetTkbkMode( m_NtwkMediaPocsThrdPt, IsBlockWait, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
};
#endif
