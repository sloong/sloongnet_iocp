#include "stdafx.h"
#include "ServerMain.h"
#include "SloongWallServer.h"
#include "IUniversal.h"
#pragma comment(lib,"Universal.lib")
using namespace SoaringLoong;

SERVICE_STATUS              ssStatus;
SERVICE_STATUS_HANDLE		sshStatusHandle;
DWORD                       dwErr = 0;
TCHAR                       szErr[256];
ILogSystem*					pLog = NULL;
IUniversal*					pUniversal = NULL;
CSloongWallServer			g_oSloongWallServer;

int _tmain(int argc, _TCHAR* argv[])
{
	CoInitialize(NULL);
	
	HRESULT hRes = CreateUniversal((LPVOID*)&pUniversal);
	pUniversal->CreateLogSystem(pUniversal, &pLog);
	pLog->Initialize(pUniversal, _T("D:\\Log"),LOGLEVEL::All,LOGTYPE::DAY);
	pLog->Log(LOGLEVEL::INF, 0, pUniversal->Format(TEXT("Server Start. Server Path is %s."), argv[0]));
	
	SERVICE_TABLE_ENTRY dispatchTable[] =
	{
		{ SZSERVICENAME, (LPSERVICE_MAIN_FUNCTION)Service_Main },
		{ NULL, NULL }
	};

	pLog->Log(INF, 0, pUniversal->Format(TEXT("Command line is %s."), argv[1]));
	if ((argc > 1) && ((*argv[1] == '-') || (*argv[1] == '/')))
	{
		if (_tcsicmp(_T("install"), argv[1] + 1) == 0)
		{
			InstallService();
		}
		else if (_tcsicmp(_T("remove"), argv[1] + 1) == 0)
		{
			RemoveService();
		}
		else if (_tcsicmp(_T("debug"), argv[1] + 1) == 0)
		{
			_tprintf(TEXT("Server is runing in debug mode.\r\nPlease input the control code.\r\n"));
			ServiceStart(NULL, NULL);
		}
	}
	else
	{
		if (!StartServiceCtrlDispatcher(dispatchTable))
		{
			pLog->Log(LOGLEVEL::INF, 0, TEXT("StartServiceCtrlDispatcher failed."));
		}
		else
		{
			pLog->Log(LOGLEVEL::INF, 0, TEXT("StartServiceCtrlDispatcher OK."));
		}
	}
	CoUninitialize();
	return 0;
}

void RemoveService()
{
	pLog->Log(INF, 0, _T("Remove Service start. "));
	SC_HANDLE schService;
	SC_HANDLE schSCManager;
	TCHAR szPath[512];

	if (GetModuleFileName(NULL, szPath, 512) == 0)
	{
		pLog->Log(ERR,GetLastError(),pUniversal->Format(TEXT("Unable to remove %s - %s /n"),SZAPPNAME,GetLastError()));
		return;
	}

	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (schSCManager)
	{
		schService = OpenService(schSCManager, SZAPPNAME, SC_MANAGER_ALL_ACCESS);
		if (schService)
		{
			pLog->Log(ERR, GetLastError(), pUniversal->Format(TEXT("%s Remove. /n"), SZAPPNAME));
			DeleteService(schService);
			CloseServiceHandle(schService);
		}
		else
		{
			pLog->Log(ERR, GetLastError(), pUniversal->Format(TEXT("Remove Service failed - %s /n"), GetLastError()));
		}
		CloseServiceHandle(schSCManager);
	}
	else
		pLog->Log(ERR, GetLastError(), pUniversal->Format(TEXT("OpenSCManager failed - %s /n"), GetLastError()));
}

void  WINAPI  Service_Main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	pLog->Log(INF,0,TEXT("Server main is called."));
	sshStatusHandle = RegisterServiceCtrlHandler(SZSERVICENAME, Service_Ctrl);

	if (sshStatusHandle==(SERVICE_STATUS_HANDLE)0)
	{
		pLog->Log(ERR, GetLastError(), pUniversal->Format(TEXT("注册服务控制处理函数失败.")));
		return;
	}

	HRESULT hRes = InitService();
	if (FAILED(hRes))
	{
		ReportStatusToSCMgr(SERVICE_STOPPED, ERROR_SERVICE_SPECIFIC_ERROR, 1000);
		return;
	}

	ReportStatusToSCMgr(SERVICE_RUNNING,NO_ERROR,0);

	ServiceStart(dwArgc, lpszArgv);
	return;
}


BOOL ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
	pLog->WriteLine(pUniversal->Format(_T("Set server status to %d."), dwCurrentState));
	ssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;

	ssStatus.dwServiceSpecificExitCode = 0;
	ssStatus.dwServiceType = SERVICE_WIN32;
	ssStatus.dwCurrentState = dwCurrentState;
	ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	ssStatus.dwWin32ExitCode = dwWin32ExitCode;
	ssStatus.dwServiceSpecificExitCode = dwWin32ExitCode;
	ssStatus.dwCheckPoint = 0;
	ssStatus.dwWaitHint = dwWaitHint;

	BOOL bRes = SetServiceStatus(sshStatusHandle, &ssStatus);
	return bRes;
}


void ServiceStop()
{
	ReportStatusToSCMgr(SERVICE_STOP, NO_ERROR, 500);
	exit(0);
}

void WINAPI Service_Ctrl(DWORD dwCtrlCode)
{
	pLog->Log(INF,0,pUniversal->Format(pUniversal->Format(_T("Server ctrl is called. code is %d"), dwCtrlCode)));

	switch (dwCtrlCode)
	{
	case SERVICE_CONTROL_STOP:
		ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 500);
		ServiceStop();
		return;
	case SERVICE_CONTROL_PAUSE:
		ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 500);
		//ServicePause(); 
		ssStatus.dwCurrentState = SERVICE_PAUSED;
		return;
	case SERVICE_CONTROL_CONTINUE:
		ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 500);
		//ServiceContinue();
		ssStatus.dwCurrentState = SERVICE_RUNNING;
		return;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	default:
		break;
	}
	ReportStatusToSCMgr(ssStatus.dwCurrentState, NO_ERROR, 0);
}

void InstallService()
{
	SC_HANDLE schService;
	SC_HANDLE schSCManager;
	TCHAR szPath[512];

	if (GetModuleFileName(NULL, szPath, 512) == 0)
	{
		pLog->Log(ERR,GetLastError(),pUniversal->Format(TEXT("Unable to install %s - %s /n"),SZAPPNAME,GetLastError()));
		return;
	}

	// Open Server control manager
	schSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);

	if (schSCManager)
	{
		schService = CreateService(schSCManager,SZSERVICENAME,SZAPPNAME, SERVICE_ALL_ACCESS,SERVICE_WIN32_OWN_PROCESS,
									SERVICE_DEMAND_START,SERVICE_ERROR_NORMAL,szPath,NULL, NULL,NULL,NULL,NULL);
		if (schService)
		{
			pLog->Log(INF,0,pUniversal->Format(TEXT("%s installed. /n"), SZAPPNAME));
			CloseServiceHandle(schService);
		}
		else
		{
			pLog->Log(ERR, GetLastError(), pUniversal->Format(TEXT("CreateService failed - %s /n"), GetLastError()));
		}
		CloseServiceHandle(schSCManager);
	}
	else
		pLog->Log(ERR, GetLastError(), pUniversal->Format(TEXT("OpenSCManager failed - %s /n"), GetLastError()));

}


HRESULT InitService()
{
	return S_OK;
}

void ServiceStart(DWORD dwArgc, LPTSTR* lpszArgv)
{
	pLog->Log(INF, 0, TEXT("ServerStart is called."));
	g_oSloongWallServer.Initialize(pUniversal, pLog);
	g_oSloongWallServer.Run();
	SAFE_RELEASE(pLog);
	SAFE_RELEASE(pUniversal);
	return ;
}
