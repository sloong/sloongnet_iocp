//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Recordset.h: implementation of the CRecordset class.                                            +                                           
// Author: �� С�� @China                                                                          + 
//                                                                                                 + 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef YXB_CRECORDSET_H_H
#define YXB_CRECORDSET_H_H

////////////////////////////////////////////////////////////////////////////////////////////////////
// include flies                                                                                  //
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <afx.h>
#include "ADOCONST.h"
#include "Connection.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// class difine                                                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace ADOCONST;

class CRecordset  
{
	
////////////////////////////////////////////////////////////////////////////////////////////////////
// Member                                                                                         //
////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	CString          m_sErrorMessage;
	
public:
		_RecordsetPtr     m_pRst;
////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction                                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	CRecordset();
	virtual ~CRecordset();
	
	
////////////////////////////////////////////////////////////////////////////////////////////////////
// Property Get/Set Method                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	//Property: RecordCount
	long GetRecordCount() const;
	
	//Property: PageCount
	long GetPageCount() const;
	
	//Property: PageSize
	long GetPageSize() const;
	void SetPageSize(long pageSize);
	
	//Property: AbsolutePage
	long GetAbsolutePage() const;
	void SetAbsolutePage(long page);
	
	//Property: AbsolutePosition
	long GetAbsolutePosition() const;
	void SetAbsolutePosition(long pos);
	
	//Property: State
	ObjectStateEnum GetState() const;
	
	//Property: CursorLocation
	void SetCursorLocation(CursorLocationEnum CursorLocation);
	
	
////////////////////////////////////////////////////////////////////////////////////////////////////
// Core  Method                                                                                   //
//////////////////////////////////////////////////////////////////////////////////////////////////// 
	
public:
	void Open(CString Source, _ConnectionPtr pConn, CursorTypeEnum CursorType=CursorType::adOpenStatic,LockTypeEnum LockType=LockType::adLockOptimistic,long Options=CommandType::adCmdText);	
	void Open(CString Source, CConnection & ActiveConn, CursorTypeEnum CursorType=CursorType::adOpenStatic,LockTypeEnum LockType=LockType::adLockOptimistic,long Options=CommandType::adCmdText);	
	bool Close();
    void Release();
	
	
////////////////////////////////////////////////////////////////////////////////////////////////////
// Position                                                                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////// 
	void MoveNext();
	void MovePrevious();
	void MoveLast();
	void MoveFirst();
	void Move(long pos);
	bool rstEOF();
	bool rstBOF();
	
	
////////////////////////////////////////////////////////////////////////////////////////////////////
// Recordset update db                                                                            //
//////////////////////////////////////////////////////////////////////////////////////////////////// 
    void AddNew();
	void Delete(AffectEnum Option);
    void Cancel();
    void Update();
    
////////////////////////////////////////////////////////////////////////////////////////////////////
// GetValues form recordset                                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////
	int GetInt(CString columnName);
	CString GetString(CString columnName);
	
	
////////////////////////////////////////////////////////////////////////////////////////////////////
// SetValues form recordset                                                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////// 
	void SetInt(CString columnName,int value);
	void SetString(CString columnName,CString value);
	
};

#endif //#ifndef YXB_CRECORDSET_H_H

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++