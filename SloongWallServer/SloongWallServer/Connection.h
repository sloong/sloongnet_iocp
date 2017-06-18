//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Connection.h:                                                                                   +
// Author: 楊 小兵 @China                                                                          +
//                                                                                                 +
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef YXB_CCONNECTION_H_H
#define YXB_CCONNECTION_H_H

////////////////////////////////////////////////////////////////////////////////////////////////////
// include flies                                                                                  //
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <afx.h>
#include "ADOCONST.h"


////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace ADOCONST;

class CConnection  
{
private:
	_ConnectionPtr  m_pConn; 
	CString         m_sErrorMessage;  //used to save error message


////////////////////////////////////////////////////////////////////////////////////////////////////
//Construction/Destruction                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	CConnection();
	virtual ~CConnection();

////////////////////////////////////////////////////////////////////////////////////////////////////
// Property Get/Set Method                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	//Property: ConnectionString
	CString GetConnectionString() const;
	void SetConnectionString(char * charConn);
	void SetConnectionString(CString strConn);

	//Property: ConnectionTimeout
	long GetConnectionTimeout() const;
	void SetConnectionTimeout(long time);

    //Property: CursorLocation
	CursorLocationEnum GetCursorLocation() const;
	void SetCursorLocation(CursorLocationEnum CursorLocation);

    //Property: CommandTimeout
	long GetCommandTimeout() const;
	void SetCommandTimeout(long time);


    //Property: State
	ObjectStateEnum GetState() const;

    //Property: m_sErrorMessage
    CString GetErrorMessage() const;

	//Property: m_pConn
	_ConnectionPtr GetConnection() const;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Core Method                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////    
public:
    bool Open(CString ConnectionString,CString UserID=_T(""),CString Password=_T(""), ConnectOptionEnum ConnectOption=ConnectOption::adConnectUnspecified);//ID和Password为空，因为要再连接字符串中去指明ID和密码
    bool Close();
	bool Cancel();
	void Release();
	bool RollbackTrans();
	bool CommitTrans();
	long BeginTrans();
};

#endif // #ifndef YXB_CONNECTION_H_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//