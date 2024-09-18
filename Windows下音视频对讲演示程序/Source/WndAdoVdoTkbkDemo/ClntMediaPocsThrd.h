#pragma once

#include "Func.h"
#include "DataStruct.h"
#include "Sokt.h"
#include "Ajb.h"
#include "MediaPocsThrd.h"
#include "TkbkClnt.h"
#include "BdctClnt.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct ClntMediaPocsThrd //客户端媒体处理线程。
{
	MediaPocsThrd * m_MediaPocsThrdPt; //存放媒体处理线程的指针。

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
	
    TkbkClnt m_TkbkClnt; //存放对讲客户端。
    BdctClnt m_BdctClnt; //存放广播客户端。
	
    int m_MaxCnctTimes; //存放最大连接次数，取值区间为[1,2147483647]。
	int m_IsReferRmtTkbkModeSetTkbkMode; //存放是否参考远端对讲模式来设置对讲模式，为1表示要参考，为0表示不参考。
    int m_IsAutoRqirExit; //存放是否自动请求退出，为0表示手动，为1表示在对讲客户端的连接销毁且广播客户端销毁时自动请求退出。
    AudpSokt * m_AudpClntSoktPt; //存放本端高级Udp协议客户端套接字的指针。

	struct //线程。
	{
		Vstr * m_LclNodeAddrPt; //存放本地节点地址的指针。
		Vstr * m_LclNodePortPt; //存放本地节点端口的指针。
		Vstr * m_RmtNodeAddrPt; //存放远程节点地址的指针。
		Vstr * m_RmtNodePortPt; //存放远程节点端口的指针。

		int8_t * m_TmpBytePt; //存放临时数据的指针。
		size_t m_TmpByteSz; //存放临时数据大小。
	} m_Thrd;
	
	typedef enum UserMsgTyp //用户消息。
	{
		UserMsgTypTkbkClntSetIsTstNtwkDly, //对讲客户端设置是否测试网络延迟。
		UserMsgTypTkbkClntCnctInit, //对讲客户端的连接初始化。
		UserMsgTypTkbkClntCnctDstoy, //对讲客户端的连接销毁。
		UserMsgTypTkbkClntLclTkbkMode, //对讲客户端的本端对讲模式。
		UserMsgTypTkbkClntPttBtnDown, //对讲客户端的一键即按即通按钮按下。
		UserMsgTypTkbkClntPttBtnUp, //对讲客户端的一键即按即通按钮弹起。

		UserMsgTypBdctClntInit, //广播客户端初始化。
		UserMsgTypBdctClntDstoy, //广播客户端销毁。
		UserMsgTypBdctClntCnctInit, //广播客户端的连接初始化。
		UserMsgTypBdctClntCnctDstoy, //广播客户端的连接销毁。
	} UserMsgTyp;
	typedef struct UserMsgTkbkClntSetIsTstNtwkDly
	{
		int32_t m_IsTstNtwkDly;
		uint64_t m_SendIntvlMsec;
	} UserMsgTkbkClntSetIsTstNtwkDly;
	typedef struct UserMsgTkbkClntCnctInit
	{
		int32_t m_IsTcpOrAudpPrtcl;
		Vstr * m_RmtNodeNameVstrPt;
		Vstr * m_RmtNodeSrvcVstrPt;
	} UserMsgTkbkClntCnctInit;
	typedef struct UserMsgTkbkClntCnctDstoy
	{
	} UserMsgTkbkClntCnctDstoy;
	typedef struct UserMsgTkbkClntLclTkbkMode
	{
		int32_t m_LclTkbkMode;
	} UserMsgTkbkClntLclTkbkMode;
	typedef struct UserMsgTkbkClntPttBtnDown
	{
	} UserMsgTkbkClntPttBtnDown;
	typedef struct UserMsgTkbkClntPttBtnUp
	{
	} UserMsgTkbkClntPttBtnUp;
	typedef struct UserMsgBdctClntInit
	{
		int32_t m_CnctNumIsDecr;
	} UserMsgBdctClntInit;
	typedef struct UserMsgBdctClntDstoy
	{
	} UserMsgBdctClntDstoy;
	typedef struct UserMsgBdctClntCnctInit
	{
		int32_t m_IsTcpOrAudpPrtcl;
		Vstr * m_RmtNodeNameVstrPt;
		Vstr * m_RmtNodeSrvcVstrPt;
	} UserMsgBdctClntCnctInit;
	typedef struct UserMsgBdctClntCnctDstoy
	{
		int32_t m_CnctNum;
	} UserMsgBdctClntCnctDstoy;
	typedef struct UserMsg
	{
		uint8_t m_UserMsgArr[];
	} UserMsg;

	//用户定义的相关回调函数。

	//存放用户数据的指针。注意：在C++的ClntMediaPocsThrdCls类中，本变量存放this指针，请勿修改。
	void * m_UserDataPt;
	
	//用户定义的显示日志函数。
	typedef void( __cdecl * ClntMediaPocsThrdUserShowLogFuncPt )( ClntMediaPocsThrd * ClntMediaPocsThrdPt, Vstr * InfoVstrPt );
	ClntMediaPocsThrdUserShowLogFuncPt m_UserShowLogFuncPt;

	//用户定义的显示Toast函数。
	typedef void( __cdecl * ClntMediaPocsThrdUserShowToastFuncPt )( ClntMediaPocsThrd * ClntMediaPocsThrdPt, Vstr * InfoVstrPt );
	ClntMediaPocsThrdUserShowToastFuncPt m_UserShowToastFuncPt;
	
	//用户定义的客户端媒体处理线程初始化函数。
	typedef void( __cdecl * ClntMediaPocsThrdUserClntMediaPocsThrdInitFuncPt )( ClntMediaPocsThrd * ClntMediaPocsThrdPt );
	ClntMediaPocsThrdUserClntMediaPocsThrdInitFuncPt m_UserClntMediaPocsThrdInitFuncPt;

	//用户定义的客户端媒体处理线程销毁函数。
	typedef void( __cdecl * ClntMediaPocsThrdUserClntMediaPocsThrdDstoyFuncPt )( ClntMediaPocsThrd * ClntMediaPocsThrdPt );
	ClntMediaPocsThrdUserClntMediaPocsThrdDstoyFuncPt m_UserClntMediaPocsThrdDstoyFuncPt;
	
	//用户定义的对讲客户端连接初始化函数。
	typedef void( __cdecl * ClntMediaPocsThrdUserTkbkClntCnctInitFuncPt )( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt );
	ClntMediaPocsThrdUserTkbkClntCnctInitFuncPt m_UserTkbkClntCnctInitFuncPt;
	
	//用户定义的对讲客户端连接销毁函数。
	typedef void( __cdecl * ClntMediaPocsThrdUserTkbkClntCnctDstoyFuncPt )( ClntMediaPocsThrd * ClntMediaPocsThrdPt );
	ClntMediaPocsThrdUserTkbkClntCnctDstoyFuncPt m_UserTkbkClntCnctDstoyFuncPt;

	//用户定义的对讲客户端连接状态函数。
	typedef void( __cdecl * ClntMediaPocsThrdUserTkbkClntCnctStsFuncPt )( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int32_t CurCnctSts );
	ClntMediaPocsThrdUserTkbkClntCnctStsFuncPt m_UserTkbkClntCnctStsFuncPt;
	
	//用户定义的对讲客户端我的对讲索引函数。
	typedef void( __cdecl * ClntMediaPocsThrdUserTkbkClntMyTkbkIdxFuncPt )( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int32_t MyTkbkIdx );
	ClntMediaPocsThrdUserTkbkClntMyTkbkIdxFuncPt m_UserTkbkClntMyTkbkIdxFuncPt;

	//用户定义的对讲客户端本端对讲模式函数。
	typedef void( __cdecl * ClntMediaPocsThrdUserTkbkClntLclTkbkModeFuncPt )( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int32_t OldLclTkbkMode, int32_t NewLclTkbkMode );
	ClntMediaPocsThrdUserTkbkClntLclTkbkModeFuncPt m_UserTkbkClntLclTkbkModeFuncPt;
	
	//用户定义的对讲客户端对讲信息初始化函数。
	typedef void( __cdecl * ClntMediaPocsThrdUserTkbkClntTkbkInfoInitFuncPt )( ClntMediaPocsThrd * ClntMediaPocsThrdPt, TkbkClnt::TkbkInfo * TkbkInfoPt );
	ClntMediaPocsThrdUserTkbkClntTkbkInfoInitFuncPt m_UserTkbkClntTkbkInfoInitFuncPt;
	
	//用户定义的对讲客户端对讲信息销毁函数。
	typedef void( __cdecl * ClntMediaPocsThrdUserTkbkClntTkbkInfoDstoyFuncPt )( ClntMediaPocsThrd * ClntMediaPocsThrdPt, TkbkClnt::TkbkInfo * TkbkInfoPt );
	ClntMediaPocsThrdUserTkbkClntTkbkInfoDstoyFuncPt m_UserTkbkClntTkbkInfoDstoyFuncPt;
	
	//用户定义的对讲客户端对讲信息远端对讲模式函数。
	typedef void( __cdecl * ClntMediaPocsThrdUserTkbkClntTkbkInfoRmtTkbkModeFuncPt )( ClntMediaPocsThrd * ClntMediaPocsThrdPt, TkbkClnt::TkbkInfo * TkbkInfoPt, int32_t OldRmtTkbkMode, int32_t NewRmtTkbkMode );
	ClntMediaPocsThrdUserTkbkClntTkbkInfoRmtTkbkModeFuncPt m_UserTkbkClntTkbkInfoRmtTkbkModeFuncPt;
	
	//用户定义的对讲客户端测试网络延迟函数。
	typedef void( __cdecl * ClntMediaPocsThrdUserTkbkClntTstNtwkDlyFuncPt )( ClntMediaPocsThrd * ClntMediaPocsThrdPt, uint64_t NtwkDlyMsec );
	ClntMediaPocsThrdUserTkbkClntTstNtwkDlyFuncPt m_UserTkbkClntTstNtwkDlyFuncPt;

	//用户定义的广播客户端初始化函数。
	typedef void( __cdecl * ClntMediaPocsThrdUserBdctClntInitFuncPt )( ClntMediaPocsThrd * ClntMediaPocsThrdPt );
	ClntMediaPocsThrdUserBdctClntInitFuncPt m_UserBdctClntInitFuncPt;
	
	//用户定义的广播客户端销毁函数。
	typedef void( __cdecl * ClntMediaPocsThrdUserBdctClntDstoyFuncPt )( ClntMediaPocsThrd * ClntMediaPocsThrdPt );
	ClntMediaPocsThrdUserBdctClntDstoyFuncPt m_UserBdctClntDstoyFuncPt;
	
	//用户定义的广播客户端连接初始化函数。
	typedef void( __cdecl * ClntMediaPocsThrdUserBdctClntCnctInitFuncPt )( ClntMediaPocsThrd * ClntMediaPocsThrdPt, BdctClnt::CnctInfo * CnctInfoPt, int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt );
	ClntMediaPocsThrdUserBdctClntCnctInitFuncPt m_UserBdctClntCnctInitFuncPt;
	
	//用户定义的广播客户端连接销毁函数。
	typedef void( __cdecl * ClntMediaPocsThrdUserBdctClntCnctDstoyFuncPt )( ClntMediaPocsThrd * ClntMediaPocsThrdPt, BdctClnt::CnctInfo * CnctInfoPt );
	ClntMediaPocsThrdUserBdctClntCnctDstoyFuncPt m_UserBdctClntCnctDstoyFuncPt;

	//用户定义的广播客户端连接状态函数。
	typedef void( __cdecl * ClntMediaPocsThrdUserBdctClntCnctStsFuncPt )( ClntMediaPocsThrd * ClntMediaPocsThrdPt, BdctClnt::CnctInfo * CnctInfoPt, int32_t CurCnctSts );
	ClntMediaPocsThrdUserBdctClntCnctStsFuncPt m_UserBdctClntCnctStsFuncPt;
} ClntMediaPocsThrd;
extern const char * const g_TkbkModeU8strArrPt[ 17 ];

int ClntMediaPocsThrdInit( ClntMediaPocsThrd * * ClntMediaPocsThrdPtPt, const void * LicnCodePt, void * UserDataPt,
						   ClntMediaPocsThrd::ClntMediaPocsThrdUserShowLogFuncPt UserShowLogFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserShowToastFuncPt UserShowToastFuncPt,
						   ClntMediaPocsThrd::ClntMediaPocsThrdUserClntMediaPocsThrdInitFuncPt UserClntMediaPocsThrdInitFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserClntMediaPocsThrdDstoyFuncPt UserClntMediaPocsThrdDstoyFuncPt,
						   ClntMediaPocsThrd::ClntMediaPocsThrdUserTkbkClntCnctInitFuncPt UserTkbkClntCnctInitFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserTkbkClntCnctDstoyFuncPt UserTkbkClntCnctDstoyFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserTkbkClntCnctStsFuncPt UserTkbkClntCnctStsFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserTkbkClntMyTkbkIdxFuncPt UserTkbkClntMyTkbkIdxFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserTkbkClntLclTkbkModeFuncPt UserTkbkClntLclTkbkModeFuncPt, 
						   ClntMediaPocsThrd::ClntMediaPocsThrdUserTkbkClntTkbkInfoInitFuncPt UserTkbkClntTkbkInfoInitFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserTkbkClntTkbkInfoDstoyFuncPt UserTkbkClntTkbkInfoDstoyFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserTkbkClntTkbkInfoRmtTkbkModeFuncPt UserTkbkClntTkbkInfoRmtTkbkModeFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserTkbkClntTstNtwkDlyFuncPt UserTkbkClntTstNtwkDlyFuncPt,
						   ClntMediaPocsThrd::ClntMediaPocsThrdUserBdctClntInitFuncPt UserBdctClntInitFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserBdctClntDstoyFuncPt UserBdctClntDstoyFuncPt,
						   ClntMediaPocsThrd::ClntMediaPocsThrdUserBdctClntCnctInitFuncPt UserBdctClntCnctInitFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserBdctClntCnctDstoyFuncPt UserBdctClntCnctDstoyFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserBdctClntCnctStsFuncPt UserBdctClntCnctStsFuncPt,
						   Vstr * ErrInfoVstrPt );
int ClntMediaPocsThrdDstoy( ClntMediaPocsThrd * ClntMediaPocsThrdPt, Vstr * ErrInfoVstrPt );
int ClntMediaPocsThrdSendTkbkClntSetIsTstNtwkDlyMsg( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int IsBlockWait, int32_t IsTstNtwkDly, uint64_t SendIntvlMsec, Vstr * ErrInfoVstrPt );
int ClntMediaPocsThrdSendTkbkClntCnctInitMsg( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int IsBlockWait, int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt, Vstr * ErrInfoVstrPt );
int ClntMediaPocsThrdSendTkbkClntCnctDstoyMsg( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt );
int ClntMediaPocsThrdSendTkbkClntLclTkbkModeMsg( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int IsBlockWait, int32_t LclTkbkMode, Vstr * ErrInfoVstrPt );
int ClntMediaPocsThrdSendTkbkClntPttBtnDownMsg( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt );
int ClntMediaPocsThrdSendTkbkClntPttBtnUpMsg( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt );
int ClntMediaPocsThrdSendBdctClntInitMsg( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int IsBlockWait, int32_t CnctNumIsDecr, Vstr * ErrInfoVstrPt );
int ClntMediaPocsThrdSendBdctClntDstoyMsg( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt );
int ClntMediaPocsThrdSendBdctClntCnctInitMsg( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int IsBlockWait, int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt, Vstr * ErrInfoVstrPt );
int ClntMediaPocsThrdSendBdctClntCnctDstoyMsg( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int IsBlockWait, int32_t CnctNum, Vstr * ErrInfoVstrPt );
void ClntMediaPocsThrdIsAutoRqirExit( ClntMediaPocsThrd * ClntMediaPocsThrdPt, Vstr * ErrInfoVstrPt );
void ClntMediaPocsThrdSetTkbkMode( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int IsBlockWait, int SetMode );

int ClntMediaPocsThrdUserInit( MediaPocsThrd * MediaPocsThrdPt );
int ClntMediaPocsThrdUserPocs( MediaPocsThrd * MediaPocsThrdPt );
void ClntMediaPocsThrdUserDstoy( MediaPocsThrd * MediaPocsThrdPt );
int ClntMediaPocsThrdUserMsg( MediaPocsThrd * MediaPocsThrdPt, unsigned int MsgTyp, void * MsgPt, size_t MsgLenByt );
void ClntMediaPocsThrdUserReadAdoVdoInptFrm( MediaPocsThrd * MediaPocsThrdPt,
											 int16_t * AdoInptPcmSrcFrmPt, int16_t * AdoInptPcmRsltFrmPt, size_t AdoInptPcmFrmLenUnit, int32_t AdoInptPcmRsltFrmVoiceActSts,
											 uint8_t * AdoInptEncdRsltFrmPt, size_t AdoInptEncdRsltFrmLenByt, int32_t AdoInptEncdRsltFrmIsNeedTrans,
											 uint8_t * VdoInptBgraSrcFrmPt, int32_t VdoInptBgraSrcFrmWidth, int32_t VdoInptBgraSrcFrmHeight, size_t VdoInptBgraSrcFrmLenByt,
											 uint8_t * VdoInptYu12RsltFrmPt, int32_t VdoInptYu12RsltFrmWidth, int32_t VdoInptYu12RsltFrmHeight, size_t VdoInptYu12RsltFrmLenByt,
											 uint8_t * VdoInptEncdRsltFrmPt, size_t VdoInptEncdRsltFrmLenByt );
void ClntMediaPocsThrdUserWriteAdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx,
										   int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
										   uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmSzByt, size_t * AdoOtptEncdSrcFrmLenBytPt );
void ClntMediaPocsThrdUserGetAdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx,
										 int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
										 uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmLenByt );
void ClntMediaPocsThrdUserWriteVdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx,
										   uint8_t * VdoOtptYu12SrcFrmPt, int32_t * VdoOtptYu12SrcFrmWidthPt, int32_t * VdoOtptYu12SrcFrmHeightPt,
										   uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmSzByt, size_t * VdoOtptEncdSrcFrmLenBytPt );
void ClntMediaPocsThrdUserGetVdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx,
										 uint8_t * VdoOtptYu12SrcFrmPt, int32_t VdoOtptYu12SrcFrmWidth, int32_t VdoOtptYu12SrcFrmHeight,
										 uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmLenByt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class ClntMediaPocsThrdCls
{
public:
	ClntMediaPocsThrd * m_ClntMediaPocsThrdPt;
	MediaPocsThrdCls * m_MediaPocsThrdClsPt;

	ClntMediaPocsThrdCls() { m_ClntMediaPocsThrdPt = NULL; m_MediaPocsThrdClsPt = NULL; }
	~ClntMediaPocsThrdCls() { Dstoy( NULL ); }
	
	//用户定义的显示日志函数。
	virtual void UserShowLog( Vstr * InfoVstrPt ) = 0;

	//用户定义的显示Toast函数。
	virtual void UserShowToast( Vstr * InfoVstrPt ) = 0;
	
	//用户定义的客户端媒体处理线程初始化函数。
	virtual void UserClntMediaPocsThrdInit() = 0;

	//用户定义的客户端媒体处理线程销毁函数。
	virtual void UserClntMediaPocsThrdDstoy() = 0;
	
	//用户定义的对讲客户端连接初始化函数。
	virtual void UserTkbkClntCnctInit( int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt ) = 0;
	
	//用户定义的对讲客户端连接销毁函数。
	virtual void UserTkbkClntCnctDstoy() = 0;
	
	//用户定义的对讲客户端连接状态函数。
	virtual void UserTkbkClntCnctSts( int32_t CurCnctSts ) = 0;
	
	//用户定义的对讲客户端我的对讲索引函数。
	virtual void UserTkbkClntMyTkbkIdx( int32_t MyTkbkIdx ) = 0;
	
	//用户定义的对讲客户端本端对讲模式函数。
	virtual void UserTkbkClntLclTkbkMode( int32_t OldLclTkbkMode, int32_t NewLclTkbkMode ) = 0;
	
	//用户定义的对讲客户端对讲信息初始化函数。
	virtual void UserTkbkClntTkbkInfoInit( TkbkClnt::TkbkInfo * TkbkInfoPt ) = 0;
	
	//用户定义的对讲客户端对讲信息销毁函数。
	virtual void UserTkbkClntTkbkInfoDstoy( TkbkClnt::TkbkInfo * TkbkInfoPt ) = 0;
	
	//用户定义的对讲客户端对讲信息远端对讲模式函数。
	virtual void UserTkbkClntTkbkInfoRmtTkbkMode( TkbkClnt::TkbkInfo * TkbkInfoPt, int32_t OldRmtTkbkMode, int32_t NewRmtTkbkMode ) = 0;
	
	//用户定义的对讲客户端测试网络延迟函数。
	virtual void UserTkbkClntTstNtwkDly( uint64_t NtwkDlyMsec ) = 0;

	//用户定义的广播客户端初始化函数。
	virtual void UserBdctClntInit() = 0;

	//用户定义的广播客户端销毁函数。
	virtual void UserBdctClntDstoy() = 0;
	
	//用户定义的广播客户端连接初始化函数。
	virtual void UserBdctClntCnctInit( BdctClnt::CnctInfo * CnctInfoPt, int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt ) = 0;
	
	//用户定义的广播客户端连接销毁函数。
	virtual void UserBdctClntCnctDstoy( BdctClnt::CnctInfo * CnctInfoPt ) = 0;
	
	//用户定义的广播客户端连接状态函数。
	virtual void UserBdctClntCnctSts( BdctClnt::CnctInfo * CnctInfoPt, int32_t CurCnctSts ) = 0;
	
	int Init( const void * LicnCodePt, VstrCls * ErrInfoVstrPt );
	int Dstoy( VstrCls * ErrInfoVstrPt ) { int p_Rslt = ClntMediaPocsThrdDstoy( m_ClntMediaPocsThrdPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); m_ClntMediaPocsThrdPt = NULL; m_MediaPocsThrdClsPt = NULL; return p_Rslt; }
	
	int SendTkbkClntSetIsTstNtwkDlyMsg( int IsBlockWait, int32_t IsTstNtwkDly, uint64_t SendIntvlMsec, VstrCls * ErrInfoVstrPt ) { return ClntMediaPocsThrdSendTkbkClntSetIsTstNtwkDlyMsg( m_ClntMediaPocsThrdPt, IsBlockWait, IsTstNtwkDly, SendIntvlMsec, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SendTkbkClntCnctInitMsg( int IsBlockWait, int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt, VstrCls * ErrInfoVstrPt ) { return ClntMediaPocsThrdSendTkbkClntCnctInitMsg( m_ClntMediaPocsThrdPt, IsBlockWait, IsTcpOrAudpPrtcl, RmtNodeNameVstrPt, RmtNodeSrvcVstrPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SendTkbkClntCnctDstoyMsg( int IsBlockWait, VstrCls * ErrInfoVstrPt ) { return ClntMediaPocsThrdSendTkbkClntCnctDstoyMsg( m_ClntMediaPocsThrdPt, IsBlockWait, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SendTkbkClntLclTkbkModeMsg( int IsBlockWait, int32_t LclTkbkMode, VstrCls * ErrInfoVstrPt ) { return ClntMediaPocsThrdSendTkbkClntLclTkbkModeMsg( m_ClntMediaPocsThrdPt, IsBlockWait, LclTkbkMode, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SendTkbkClntPttBtnDownMsg( int IsBlockWait, VstrCls * ErrInfoVstrPt ) { return ClntMediaPocsThrdSendTkbkClntPttBtnDownMsg( m_ClntMediaPocsThrdPt, IsBlockWait, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SendTkbkClntPttBtnUpMsg( int IsBlockWait, VstrCls * ErrInfoVstrPt ) { return ClntMediaPocsThrdSendTkbkClntPttBtnUpMsg( m_ClntMediaPocsThrdPt, IsBlockWait, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SendBdctClntInitMsg( int IsBlockWait, int32_t CnctNumIsDecr, VstrCls * ErrInfoVstrPt ) { return ClntMediaPocsThrdSendBdctClntInitMsg( m_ClntMediaPocsThrdPt, IsBlockWait, CnctNumIsDecr, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SendBdctClntDstoyMsg( int IsBlockWait, VstrCls * ErrInfoVstrPt ) { return ClntMediaPocsThrdSendBdctClntDstoyMsg( m_ClntMediaPocsThrdPt, IsBlockWait, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SendBdctClntCnctInitMsg( int IsBlockWait, int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt, VstrCls * ErrInfoVstrPt ) { return ClntMediaPocsThrdSendBdctClntCnctInitMsg( m_ClntMediaPocsThrdPt, IsBlockWait, IsTcpOrAudpPrtcl, RmtNodeNameVstrPt, RmtNodeSrvcVstrPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SendBdctClntCnctDstoyMsg( int IsBlockWait, int32_t CnctNum, VstrCls * ErrInfoVstrPt ) { return ClntMediaPocsThrdSendBdctClntCnctDstoyMsg( m_ClntMediaPocsThrdPt, IsBlockWait, CnctNum, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
};
#endif
