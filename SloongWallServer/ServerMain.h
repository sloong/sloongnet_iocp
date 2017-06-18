#pragma once

#define SZAPPNAME      _T("SloongWallServer")

#define SZSERVICENAME  _T("SloongWallServer")

//下面的函数由程序实现

void  WINAPI  Service_Main(DWORD dwArgc, LPTSTR *lpszArgv);

void  WINAPI  Service_Ctrl(DWORD dwCtrlCode);

void InstallService();

HRESULT InitService();

void ServiceStart(DWORD dwArgc, LPTSTR* lpszArgv);

void RemoveService();

//void debugService(int argc, TCHAR** argv);

BOOL ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);

//void AddToMessageLog(LPTSTR lpszMsg);
