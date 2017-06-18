//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Recordset.cpp: implementation of the CRecordset class.                                          +                                           
// Author: —î Ð¡±ø @China                                                                          + 
//                                                                                                 +
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include            "stdafx.h"
#include            "Recordset.h"
using namespace     ADOCONST;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction                                                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////// 

CRecordset::CRecordset()
{
	CoInitialize(NULL);  
	m_pRst.CreateInstance("ADODB.Recordset");
	//m_pRst.CreateInstance(__uuidof(Recordset));

	//default set

}

CRecordset::~CRecordset()
{
	Release();
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// Property Get/Set Method                                                                        //
//////////////////////////////////////////////////////////////////////////////////////////////////// 

//Property: RecordCount
long CRecordset::GetRecordCount() const
{
	return m_pRst->GetRecordCount();
}

//Property: PageCount
long CRecordset::GetPageCount() const
{
	return m_pRst->GetPageCount();
}

//Property: PageSize
long CRecordset::GetPageSize() const
{
	return m_pRst->GetPageSize();
}

void CRecordset::SetPageSize(long pageSize)
{
	m_pRst->PutPageSize(pageSize);
}

//Property: AbsolutePage
long CRecordset::GetAbsolutePage() const
{
	return m_pRst->GetAbsolutePage();
}

void CRecordset::SetAbsolutePage(long page)
{
	m_pRst->PutAbsolutePage((PositionEnum)page);	
}

//Property: AbsolutePosition
long CRecordset::GetAbsolutePosition() const
{
	return m_pRst->GetAbsolutePosition();
}

void CRecordset::SetAbsolutePosition(long pos)
{
	m_pRst->PutAbsolutePosition(PositionEnum(pos));
}

//Property: State
ObjectStateEnum CRecordset::GetState() const
{
	return (ObjectStateEnum)m_pRst->GetState();
}

//Property: CursorLocation
void CRecordset::SetCursorLocation(CursorLocationEnum CursorLocation)
{
	m_pRst->PutCursorLocation(CursorLocation);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// Core Method                                                                                    //
//////////////////////////////////////////////////////////////////////////////////////////////////// 

//Open
void CRecordset::Open(CString Source,CConnection & ActiveConn,CursorTypeEnum CursorType,LockTypeEnum LockType,long Options)
{
    m_pRst->Open(_variant_t(Source),ActiveConn.GetConnection().GetInterfacePtr(),CursorType,LockType,Options);
}


void CRecordset::Open(CString Source,_ConnectionPtr pConn,CursorTypeEnum CursorType, LockTypeEnum LockType,long Options)
{
	m_pRst->Open(_variant_t(Source),pConn.GetInterfacePtr(),CursorType,LockType,Options);	
}


//Close
bool CRecordset::Close() 
{
	if(m_pRst==NULL)
	{
        m_sErrorMessage="Recordset is not available!";
		return true;
	}
	
	try
	{
		ObjectStateEnum state=(ObjectStateEnum)(m_pRst->GetState());
		
		if(state==ObjectState::adStateOpen)
		{
			HRESULT hRsut=m_pRst->Close();
			if(SUCCEEDED(hRsut))
			{
				return true;
			}
			else
			{
				m_sErrorMessage="Failed to close the Recordset!";
				return false;
			}
			
		}
		
		return true;
	}
	catch(_com_error e)
	{
		m_sErrorMessage="There is an error when called then function: Close()!";
		return false;	
	}
}

//Release
void CRecordset::Release()
{
	if(m_pRst!=NULL)
	{
		try
		{
			Close();
			m_pRst.Release();
			m_pRst=NULL;
		}
		catch(_com_error e)
		{
            throw e;
		}
	}
}



//rstBOF
bool CRecordset::rstBOF()
{
	return (m_pRst->GetrstBOF()==VARIANT_TRUE?true:false);
}

//rstEOF
bool CRecordset::rstEOF()
{
	return (m_pRst->GetrstEOF()==VARIANT_TRUE?true:false);
}

//MoveFirst: Move the cursor to the first record.
void CRecordset::MoveFirst()  
{
	m_pRst->MoveFirst();
}

//MoveLast: Move the cursor to the last record.
void CRecordset::MoveLast()
{
	m_pRst->MoveLast();
}

//MovePrevious
void CRecordset::MovePrevious()
{
	if(m_pRst->GetrstBOF()!=VARIANT_TRUE)
	{
		m_pRst->MovePrevious();
	}
}

//MoveNext
void CRecordset::MoveNext()
{
	if(m_pRst->GetrstEOF()!=VARIANT_TRUE)
	{
		m_pRst->MoveNext();
	}
}

//MoveNext
void CRecordset::Move(long pos)
{
	if(m_pRst->GetrstEOF()!=VARIANT_TRUE)
	{
		m_pRst->Move(pos);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Recordset update db                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////

//AddNew
void CRecordset::AddNew()
{
	m_pRst->AddNew();
}

//Delete
void CRecordset::Delete(AffectEnum Option)
{
	m_pRst->Delete(Option);
}

//Update
void CRecordset::Update()
{
	m_pRst->Update();
}

//Cancel
void CRecordset::Cancel()
{
	m_pRst->Cancel();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Method GetValues                                                                               //
//////////////////////////////////////////////////////////////////////////////////////////////////// 

//GetString
CString CRecordset::GetString(CString columnName)
{
	try
	{
		return (LPTSTR)(_bstr_t)m_pRst->GetCollect(_variant_t(columnName));
	}
	catch(_com_error e)
	{
		throw e;	
	}
}

//GetInt
int CRecordset::GetInt(CString columnName)
{
	try
	{
		return (short)m_pRst->GetCollect(_variant_t(columnName));
	}
	catch(_com_error e)
	{
		throw e;	
	}
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//






