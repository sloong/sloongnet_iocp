/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// import msado15.dll                                                                                                  //  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef YXB_MSADO15DLL_H_H
#define YXB_MSADO15DLL_H_H
#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","rstEOF") rename("BOF","rstBOF")
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef YXB_ADOCONST_H_H
#define YXB_ADOCONST_H_H

namespace ADOCONST
{
	namespace CursorLocation
	{
        CursorLocationEnum const adUseNone = CursorLocationEnum(1);             //donot use cursor
		CursorLocationEnum const adUseServer = CursorLocationEnum(2);           //default¡£
		CursorLocationEnum const adUseClient = CursorLocationEnum(3);           //
		CursorLocationEnum const adUseClientBatch = CursorLocationEnum(3);      //
	}

	namespace CursorType
	{
		CursorTypeEnum const adOpenUnspecified = CursorTypeEnum(-1);            //
		CursorTypeEnum const adOpenForwardOnly = CursorTypeEnum(0);             //
		CursorTypeEnum const adOpenKeyset = CursorTypeEnum(1);                  //
		CursorTypeEnum const adOpenDynamic = CursorTypeEnum(2);                 //
		CursorTypeEnum const adOpenStatic = CursorTypeEnum(3);                  //
	}

	namespace LockType
	{
		LockTypeEnum const adLockUnspecified = LockTypeEnum(-1);                //
		LockTypeEnum const adLockReadOnly = LockTypeEnum(1);                    //
		LockTypeEnum const adLockPessimistic = LockTypeEnum(2);                 //
		LockTypeEnum const adLockOptimistic = LockTypeEnum(3);                  //
		LockTypeEnum const adLockBatchOptimistic = LockTypeEnum(4);             //
	}

	namespace ObjectState
	{
		ObjectStateEnum const adStateClosed = ObjectStateEnum(0);               //
		ObjectStateEnum const adStateOpen = ObjectStateEnum(1);                 //
		ObjectStateEnum const adStateConnecting = ObjectStateEnum(2);           //
		ObjectStateEnum const adStateExecuting = ObjectStateEnum(4);            //
		ObjectStateEnum const adStateFetching = ObjectStateEnum(8);             //
	}

	
	namespace ConnectOption
	{
		ConnectOptionEnum const adConnectUnspecified = ConnectOptionEnum(-1);   //
		ConnectOptionEnum const adAsyncConnect = ConnectOptionEnum(16);         //
	}
	
	namespace CommandType
	{
		CommandTypeEnum const adCmdUnspecified = CommandTypeEnum(-1);           //
		CommandTypeEnum const adCmdText = CommandTypeEnum(1);                   //
		CommandTypeEnum const adCmdTable = CommandTypeEnum(2);                  //
		CommandTypeEnum const adCmdStoredProc = CommandTypeEnum(4);             //
		CommandTypeEnum const adCmdUnknown = CommandTypeEnum(8);                //
		CommandTypeEnum const adCmdFile = CommandTypeEnum(256);                 //
		CommandTypeEnum const adCmdTableDirect = CommandTypeEnum(512);          //
	}

	namespace ExecuteOption
	{
		ExecuteOptionEnum const adOptionUnspecified = ExecuteOptionEnum(-1);       //
		ExecuteOptionEnum const adAsyncExecute = ExecuteOptionEnum(16);            //
		ExecuteOptionEnum const adAsyncFetch = ExecuteOptionEnum(32);              //
		ExecuteOptionEnum const adAsyncFetchNonBlocking = ExecuteOptionEnum(64);   //
		ExecuteOptionEnum const adExecuteNoRecords = ExecuteOptionEnum(128);       //
		ExecuteOptionEnum const adExecuteStream = ExecuteOptionEnum(1024);         //
		ExecuteOptionEnum const adExecuteRecord = ExecuteOptionEnum(2048);         //
	}



	namespace Affect
	{
		AffectEnum const adAffectCurrent = AffectEnum(1);
		AffectEnum const adAffectGroup = AffectEnum(2);
		AffectEnum const adAffectAll = AffectEnum(3);
		AffectEnum const adAffectAllChapters = AffectEnum(4);
	}


}
#endif