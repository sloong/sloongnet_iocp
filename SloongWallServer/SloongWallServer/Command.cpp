//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Command.cpp: implementation of the CConnection class.                                           +                                              
// Author: —î Ð¡±ø @China                                                                          +
//                                                                                                 +
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "stdafx.h"
#include "Command.h"
using namespace ADOCONST;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction                                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////

CCommand::CCommand()
{
   CoInitialize(NULL);  
   m_pCmd.CreateInstance("ADODB.Command");
   //m_pCmd.CreateInstance(__uuidof(Command));

   //default set

}

CCommand::~CCommand()
{
	Release();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Property Get/Set Method                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////

//Property: ActiveConnection
void CCommand::SetActiveConnection(CConnection &ActiveConn)
{
	m_pCmd->PutActiveConnection(ActiveConn.GetConnection().GetInterfacePtr());
}

//Property: CommandText
void CCommand::SetCommandText(CString strCmd)
{
    m_pCmd->PutCommandText(_bstr_t(strCmd));
}

//Property: CommandTimeout
void CCommand::SetCommandTimeout(long time)
{
	m_pCmd->PutCommandTimeout(time);
}

//Property: State
ObjectStateEnum CCommand::GetState() const
{
	return (ObjectStateEnum)(m_pCmd->GetState());
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Core Method                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////

//ExecuteQuery
bool CCommand::ExecuteQuery(CRecordset &Rst,CommandTypeEnum CommandType)
{
	
    Rst.m_pRst=m_pCmd->Execute(NULL,NULL,CommandType);
	return true;	
}

//ExecuteUpdate
bool CCommand::ExecuteUpdate(long &AffectedRows,CRecordset &Rst,CommandTypeEnum CommandType)
{	
	VARIANT rows; 
    rows.vt = VT_I4; 
    Rst.m_pRst=m_pCmd->Execute(&rows,NULL,CommandType);
	AffectedRows=rows.lVal ;

	return true;	
}

//Release
void CCommand::Release()
{
	if(m_pCmd!=NULL)
	{
		try
		{
			m_pCmd.Release();
			m_pCmd=NULL;
		}
		catch(_com_error e)
		{
            throw e;
		}
	}
}

//Cancel
bool CCommand::Cancel()
{
	if(m_pCmd==NULL)
	{
		//m_sErrorMessage
        m_sErrorMessage="Command is not available!";
		return false;
	}
	
	try
	{
		HRESULT hRsut=m_pCmd->Cancel();
		if(SUCCEEDED(hRsut))
		{
			return true;
		}
		else
		{
			//m_sErrorMessage
			m_sErrorMessage="Failed to cancel!";
			return false;
		}
	}
	catch(_com_error e)
	{
		//m_sErrorMessage
		m_sErrorMessage="There is an error when called then function: Cancel()!";
		return false;	
	}
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//