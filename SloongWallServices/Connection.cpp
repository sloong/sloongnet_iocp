//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Connection.cpp: implementation of the CConnection class.                                        +                                    
// Author: —î Ð¡±ø @China                                                                          +
//                                                                                                 +
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "stdafx.h"
#include "Connection.h"
using namespace ADOCONST;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction                                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////

CConnection::CConnection()
{
	CoInitialize(NULL);  
	m_pConn.CreateInstance("ADODB.Connection");
    //m_pConn.CreateInstance(__uuidof(Connection));

	//default set
    m_pConn->PutCursorLocation(CursorLocation::adUseClient);

}

CConnection::~CConnection()
{
    Release();
	CoUninitialize();  
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Property Get/Set Method                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////

//Property: ConnectionString  
wstring CConnection::GetConnectionString() const
{
    return (LPTSTR)(m_pConn->GetConnectionString());
}

void CConnection::SetConnectionString(wstring strConn)
{
    m_pConn->PutConnectionString(_bstr_t(strConn.c_str()));
}

void CConnection::SetConnectionString(char *charConn)
{
	m_pConn->PutConnectionString(_bstr_t(charConn));
}


//Property: ConnectionTimeout  
long CConnection::GetConnectionTimeout() const
{
    return m_pConn->GetConnectionTimeout();
}

void CConnection::SetConnectionTimeout(long time)
{
    m_pConn->PutConnectionTimeout(time);
}

//Property: CursorLocation  
CursorLocationEnum CConnection::GetCursorLocation() const
{
	return m_pConn->GetCursorLocation();
}

void CConnection::SetCursorLocation(CursorLocationEnum CursorLocation)
{
    m_pConn->PutCursorLocation(CursorLocation);
}

//Property: CommandTimeout  
long CConnection::GetCommandTimeout() const
{
    return m_pConn->GetCommandTimeout();
}

void CConnection::SetCommandTimeout(long time)
{
    m_pConn->PutCommandTimeout(time);
}

//Property: State  
ObjectStateEnum CConnection::GetState() const
{
    return (ObjectStateEnum)(m_pConn->GetState());
}


//Property: m_sErrorMessage
wstring CConnection::GetErrorMessage() const
{
	return m_sErrorMessage;
}

//Property: m_pConn
_ConnectionPtr CConnection::GetConnection() const
{
	return m_pConn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Core Method                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////

//Open 
bool CConnection::Open(wstring ConnectionString,wstring UserID,wstring Password, ConnectOptionEnum ConnectOption)
{
	try
	{
		if(m_pConn==NULL)
		{
			m_pConn.CreateInstance("ADODB.Connection");
		}

		HRESULT hRsut=m_pConn->Open((_bstr_t)ConnectionString.c_str(),(_bstr_t)UserID.c_str(),(_bstr_t)Password.c_str(),(long)ConnectOption);
		
		if(SUCCEEDED(hRsut))
		{
			return true;
		}
		else
		{
			m_sErrorMessage=L"There is an error when open the connection!";
			return false;
		}
	}
    catch(_com_error e)
	{
		//m_sErrorMessage
		m_sErrorMessage=L"There is an error when called then function: Close()!";
		return false;	
	}
}


//Close
bool CConnection::Close()
{
	if(m_pConn==NULL)
	{
        m_sErrorMessage=L"Connectin is not available!";
		return true;
	}
	
	try
	{
		ObjectStateEnum state=(ObjectStateEnum)(m_pConn->GetState());
		
		if(state==ObjectState::adStateOpen)
		{
			HRESULT hRsut=m_pConn->Close();
			if(SUCCEEDED(hRsut))
			{
				return true;
			}
			else
			{
				//m_sErrorMessage
				m_sErrorMessage=L"Failed to close the connection!";
				return false;
			}
			
		}

		return true;
	}
	catch(_com_error e)
	{
		//m_sErrorMessage
		m_sErrorMessage=L"There is an error when called then function: Close()!";
		return false;	
	}
}


//Cancel
bool CConnection::Cancel()
{
	if(m_pConn==NULL)
	{
		//m_sErrorMessage
        m_sErrorMessage=L"Connectin is not available!";
		return false;
	}
	
	try
	{
		HRESULT hRsut=m_pConn->Cancel();
		if(SUCCEEDED(hRsut))
		{
			return true;
		}
		else
		{
			//m_sErrorMessage
			m_sErrorMessage=L"Failed to cancel!";
			return false;
		}
	}
	catch(_com_error e)
	{
		//m_sErrorMessage
		m_sErrorMessage=L"There is an error when called then function: Cancel()!";
		return false;	
	}
}


//Release
void CConnection::Release()
{
	if(m_pConn!=NULL)
	{
		try
		{
            Close();
			m_pConn.Release();
			m_pConn=NULL;
		}
		catch(_com_error e)
		{
            throw e;
		}
	}
}


//BeginTrans
long CConnection::BeginTrans()
{
	return m_pConn->BeginTrans();
}


//CommitTrans
bool  CConnection::CommitTrans()
{
	if(m_pConn==NULL)
	{
		//m_sErrorMessage
        m_sErrorMessage=L"Connectin is not available!";
		return false;
	}
	
	try
	{
		HRESULT hRsut=m_pConn->CommitTrans();
		if(SUCCEEDED(hRsut))
		{
			return true;
		}
		else
		{
			//m_sErrorMessage
			m_sErrorMessage=L"Failed to commit the tranction!";
			return false;
		}
	}
	catch(_com_error e)
	{
		//m_sErrorMessage
		m_sErrorMessage=L"There is an error when called then function: CommitTrans()!";
		return false;	
	}
}


//RollbackTrans
bool CConnection::RollbackTrans()
{
	if(m_pConn==NULL)
	{
		//m_sErrorMessage
        m_sErrorMessage=L"Connectin is not available!";
		return false;
	}
	
	try
	{
		HRESULT hRsut=m_pConn->RollbackTrans();
		if(SUCCEEDED(hRsut))
		{
			return true;
		}
		else
		{
			//m_sErrorMessage
			m_sErrorMessage=L"Failed to rollback the tranction!";
			return false;
		}
	}
	catch(_com_error e)
	{
		//m_sErrorMessage
		m_sErrorMessage=L"There is an error when called then function: RollbackTrans()!";
		return false;	
	}

}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//