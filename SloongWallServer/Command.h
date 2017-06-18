//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Command.h:                                                                                      +
// Author: —î Ð¡±ø @China                                                                          +
//                                                                                                 +
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef YXB_CCOMMAND_H_H
#define YXB_CCOMMAND_H_H

////////////////////////////////////////////////////////////////////////////////////////////////////
// include flies                                                                                  //      
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "ADOCONST.h"

#include "Connection.h"
#include "Recordset.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
using namespace ADOCONST;

class CCommand  
{
private:
	_CommandPtr m_pCmd;
    wstring     m_sErrorMessage;

public:
	CCommand();
	virtual ~CCommand();


////////////////////////////////////////////////////////////////////////////////////////////////////
// Property Get/Set Method                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	//Property: ActiveConnection
	void SetActiveConnection(CConnection &ActiveConn);
	
    //Property: CommandText
	void SetCommandText(wstring strCmd);
	
	//Property: CommandTimeout
	void SetCommandTimeout(long time);
	
	//Property: CommandType
	//void SetCommandType(CommandTypeEnum CommandType);

	//Property: State
    ObjectStateEnum GetState() const;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Other Method                                                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	bool ExecuteQuery(CRecordset &Rst,CommandTypeEnum CommandType=CommandType::adCmdText);
	bool ExecuteUpdate(long &AffectedRows,CRecordset &Rst,CommandTypeEnum CommandType=CommandType::adCmdText);
	void Release();
	bool Cancel();
};

#endif //#ifndef YXB_COMMAND_H_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//