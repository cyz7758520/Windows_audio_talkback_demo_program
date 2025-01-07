#pragma once

#include "DataStruct.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct MsgQueue //消息队列。
{
	VQueueCls m_MsgCntnr; //存放消息容器。
	ThrdInfo * m_MsgPocsThrdInfoPt; //存放消息处理线程信息的指针。
	
	//存放用户数据的指针。注意：在C++的MsgQueueCls类中，本变量存放this指针，请勿修改。
	void * m_UserDataPt;
	
	//用户定义的消息处理回调函数。
	typedef int( __cdecl * MsgQueueUserMsgPocsFuncPt )( MsgQueue * MsgQueuePt, void * UserDataPt, unsigned int MsgTyp, void * MsgParmPt, size_t MsgParmLenByt );
	MsgQueueUserMsgPocsFuncPt m_MsgQueueUserMsgPocsFuncPt;
}MsgQueue;

__DATASTRUCT_DLLAPI__ int MsgQueueInit( MsgQueue * * MsgQueuePtPt, ThrdInfo * MsgPocsThrdInfoPt, void * UserDataPt, MsgQueue::MsgQueueUserMsgPocsFuncPt MsgQueueUserMsgPocsFuncPt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int MsgQueueDstoy( MsgQueue * MsgQueuePt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int MsgQueueMsgPocsThrdMsgPocs( MsgQueue * MsgQueuePt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int MsgQueueSendMsg( MsgQueue * MsgQueuePt, int IsBlockWait, int AddFirstOrLast, unsigned int MsgTyp, void * MsgParmPt, size_t MsgParmLenByt, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class MsgQueueCls
{
public:
	MsgQueue * m_Pt;

	MsgQueueCls() { m_Pt = NULL; }
	~MsgQueueCls() { Dstoy( NULL ); }
	
	//用户定义的消息处理回调函数。
	virtual int UserMsgPocs( unsigned int MsgTyp, void * MsgParmPt, size_t MsgSzByt ) = 0;

	int Init( ThrdInfo * MsgPocsThrdInfoPt, Vstr * ErrInfoVstrPt );
	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = MsgQueueDstoy( m_Pt, ErrInfoVstrPt ); m_Pt = NULL; return p_Rslt; }
	
	int MsgPocsThrdMsgPocs( Vstr * ErrInfoVstrPt ) { return MsgQueueMsgPocsThrdMsgPocs( m_Pt, ErrInfoVstrPt ); }
	int SendMsg( int IsBlockWait, int AddFirstOrLast, unsigned int MsgTyp, void * MsgParmPt, size_t MsgParmLenByt, Vstr * ErrInfoVstrPt ) { return MsgQueueSendMsg( m_Pt, IsBlockWait, AddFirstOrLast, MsgTyp, MsgParmPt, MsgParmLenByt, ErrInfoVstrPt ); }
};
#endif
