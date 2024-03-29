#include "stdafx.h"
#include "SloongSocket.h"


/**
* 结构体名称：PER_IO_DATA
* 结构体功能：重叠I/O需要用到的结构体，临时记录IO数据
**/
const int DataBuffSize = 2 * 1024;
typedef struct
{
	OVERLAPPED overlapped;
	WSABUF databuff;
	char buffer[DataBuffSize];
	int BufferLen;
	int operationType;
}PER_IO_OPERATEION_DATA, *LPPER_IO_OPERATION_DATA, *LPPER_IO_DATA, PER_IO_DATA;

// 定义全局变量
HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);

vector<LPSLSOCKET> CSloongSocket::m_vClient;

CSloongSocket::CSloongSocket()
{
//	param = NULL;
}


CSloongSocket::~CSloongSocket()
{
	closesocket(m_hSocket);

	size_t len = m_vClient.size();

	for (size_t i = 0; i < len; i++) 
	{
		LPSLSOCKET pSocket = m_vClient[i];
		if (pSocket != NULL)
		{
			delete pSocket;
		}
	}
}

HRESULT CSloongSocket::Initialize(SLOONGCALLBACKFUN AcceptFun, SLOONGCALLBACKFUN recvFun)
{
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	DWORD err = WSAStartup(wVersionRequested, &wsaData);
	m_acceptFun = AcceptFun;
	m_recvFun = recvFun;
	if (0 != err)
	{
		cerr << "Request Windows Socket Library Error!\n";
		system("pause");
		return -1;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		cerr << "Request Windows Socket Version 2.2 Error!\n";
		system("pause");
		return -1;
	}

	// 创建IOCP的内核对象
	/**
	* 需要用到的函数的原型：
	* HANDLE WINAPI CreateIoCompletionPort(
	*    __in   HANDLE FileHandle,		// 已经打开的文件句柄或者空句柄，一般是客户端的句柄
	*    __in   HANDLE ExistingCompletionPort,	// 已经存在的IOCP句柄
	*    __in   ULONG_PTR CompletionKey,	// 完成键，包含了指定I/O完成包的指定文件
	*    __in   DWORD NumberOfConcurrentThreads // 真正并发同时执行最大线程数，一般推介是CPU核心数*2
	* );
	**/
	m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (NULL == m_hIocp)
	{
		cerr << "CreateIoCompletionPort failed. Error:" << GetLastError() << endl;
		system("pause");
		return -1;
	}

	// 创建IOCP线程--线程里面创建线程池

	// 确定处理器的核心数量
	SYSTEM_INFO mySysInfo;
	GetSystemInfo(&mySysInfo);

	// 基于处理器的核心数量创建线程
	for (DWORD i = 0; i < (mySysInfo.dwNumberOfProcessors * 2); ++i)
	{
		// 创建服务器工作器线程，并将完成端口传递到该线程
		RecvParam* param = new RecvParam();
		param->m_hIocp = m_hIocp;
		param->pCallBackFun = m_recvFun;
		HANDLE ThreadHandle = CreateThread(NULL, 0, ServerWorkThread, param, 0, NULL);
		if (NULL == ThreadHandle)
		{
			cerr << "Create Thread Handle failed. Error:" << GetLastError() << endl;
			system("pause");
			return -1;
		}
		CloseHandle(ThreadHandle);
	}

	// 建立流式套接字
	m_hSocket = WSASocket(AF_INET, SOCK_STREAM, 0,NULL,0,WSA_FLAG_OVERLAPPED);
		//socket(AF_INET, SOCK_STREAM, 0);
	//HANDLE sendThread = CreateThread(NULL, 0, ServerSendThread, 0, 0, NULL);
	return S_OK;
}

HRESULT CSloongSocket::Listen(USHORT nPort, int Backlog /* = SOMAXCONN */)
{
	SOCKADDR_IN srvAddr;
	srvAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(nPort);
	int bindResult = ::bind(m_hSocket, (SOCKADDR*)&srvAddr, sizeof(SOCKADDR));
	if (SOCKET_ERROR == bindResult){
		cerr << "Bind failed. Error:" << GetLastError() << endl;
		system("pause");
		return -1;
	}

	int listenResult = listen(m_hSocket, 10);
	if (SOCKET_ERROR == listenResult){
		cerr << "Listen failed. Error: " << GetLastError() << endl;
		system("pause");
		return -1;
	}
	return S_OK;
}

void CSloongSocket::Accept()
{
	AcceptProcParam*	param;
	param = new AcceptProcParam();
	param->m_hIocp = &m_hIocp;
	param->m_pSocket = &m_hSocket;
	param->pCallBackFun = m_acceptFun;
	HANDLE pHandle = CreateThread(NULL, 0, AcceptInThread, (LPVOID)param, 0, NULL);
	if (NULL == pHandle)
	{
		cerr << "Create Thread Handle failed. Error:" << GetLastError() << endl;
		system("pause");
		return;
	}
	CloseHandle(pHandle);
}


DWORD CSloongSocket::AcceptInThread( LPVOID lpParam )
{
	AcceptProcParam* pParam = (AcceptProcParam*)lpParam;
	SLOONGCALLBACKFUN CallFun = pParam->pCallBackFun;
	SOCKET hSocket = *pParam->m_pSocket;
	HANDLE hIocp = *pParam->m_hIocp;
	SOCKADDR_IN sockClient;
	int len = sizeof(sockClient);

	if (pParam)
	{
		delete pParam;
		pParam = NULL;
	}

	while (true)
	{
		LPSLSOCKET pConnect = NULL;
		
		SOCKET sockConnect = WSAAccept(hSocket, (SOCKADDR*)&sockClient, &len, NULL, NULL);
		if (SOCKET_ERROR == sockConnect)
		{
			cerr << "Accept Socket Error: " << GetLastError() << endl;
			system("pause");
			return -1;
		}

		pConnect = new SLSOCKET();
		pConnect->m_hSocket = sockConnect;
		memcpy(&pConnect->m_stClientAddr, &sockClient, len);
		m_vClient.push_back(pConnect);

		// 将接受套接字和完成端口关联
		CreateIoCompletionPort((HANDLE)pConnect->m_hSocket, hIocp, ULONG_PTR(pConnect), 0);

		// 开始在接受套接字上处理I/O使用重叠I/O机制
		// 在新建的套接字上投递一个或多个异步
		// WSARecv或WSASend请求，这些I/O请求完成后，工作者线程会为I/O请求提供服务	
		// 单I/O操作数据(I/O重叠)
		LPPER_IO_OPERATION_DATA PerIoData = NULL;
		PerIoData = (LPPER_IO_OPERATION_DATA)GlobalAlloc(GPTR, sizeof(PER_IO_OPERATEION_DATA));
		ZeroMemory(&(PerIoData->overlapped), sizeof(OVERLAPPED));
		PerIoData->databuff.len = 1024;
		PerIoData->databuff.buf = PerIoData->buffer;
		PerIoData->operationType = 0;	// read

		DWORD RecvBytes;
		DWORD Flags = 0;
		WSARecv(pConnect->m_hSocket, &(PerIoData->databuff), 1, &RecvBytes, &Flags, &(PerIoData->overlapped), NULL);

		(*CallFun)(pConnect);
	}
}

HRESULT CSloongSocket::Connect(USHORT nPort, LPCTSTR szIp)
{
	char cIp[30] = { 0 };
	size_t i;
	wcstombs_s(&i,cIp,szIp, _tcslen(szIp) * 2);
	IN_ADDR arr;
	SOCKADDR_IN addr;
	addr.sin_addr.S_un.S_addr = InetPton(AF_INET, szIp, &arr); //inet_addr(cIp);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPort);

	connect(m_hSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR));
	return S_OK;
}

DWORD CSloongSocket::Send(LPWSABUF pWsaBuf, SOCKET sock, int flag /*= 0*/)
{
	DWORD lpSendByte = 0;
//	WSAOVERLAPPED lpOverlapped;
	//WSASend(sock, pWsaBuf, 1,&lpSendByte, 0, &lpOverlapped, NULL );
	lpSendByte = send(sock, pWsaBuf->buf, pWsaBuf->len, 0);
	return lpSendByte;
}

SOCKET CSloongSocket::GetSocket()
{
	return m_hSocket;
}

SOCKADDR_IN CSloongSocket::GetAddr()
{
	return m_stClientAddr;
}

BOOL CSloongSocket::IsClientSocket()
{
	if ( m_dwClientID == -1 )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

LPCTSTR CSloongSocket::GetConnectIP()
{
	InetNtop(AF_INET, &m_stClientAddr.sin_addr, m_szClientIP, 30);
	return m_szClientIP;
}

DWORD CSloongSocket::GetClientNum()
{
	return m_dwClientID;
}


DWORD WINAPI CSloongSocket::ServerWorkThread(LPVOID IpParam)
{
	RecvParam* param = (RecvParam*)IpParam;
	HANDLE CompletionPort = param->m_hIocp;
	SLOONGCALLBACKFUN pCallBackFun = param->pCallBackFun;
	DWORD BytesTransferred;
	LPOVERLAPPED IpOverlapped;
	LPSLSOCKET PerHandleData = NULL;
	LPPER_IO_DATA PerIoData = NULL;
	DWORD RecvBytes;
	DWORD Flags = 0;
	BOOL bRet = false;
	if ( param )
	{
		delete param;
		param = NULL;
	}
	while (true)
	{
		bRet = GetQueuedCompletionStatus(CompletionPort, &BytesTransferred, (PULONG_PTR)&PerHandleData, (LPOVERLAPPED*)&IpOverlapped, INFINITE);
		if (bRet == 0)
		{
			cerr << "GetQueuedCompletionStatus Error: " << GetLastError() << endl;
			return -1;
		}
		
		PerIoData = (LPPER_IO_DATA)CONTAINING_RECORD(IpOverlapped, PER_IO_DATA, overlapped);

		// 检查在套接字上是否有错误发生
		if (0 == BytesTransferred)
		{
			closesocket(PerHandleData->GetSocket());
			GlobalFree(PerHandleData);
			GlobalFree(PerIoData);
			continue;
		}
		PerIoData->databuff.len = BytesTransferred;
		// 开始数据处理，接收来自客户端的数据
		WaitForSingleObject(hMutex, INFINITE);
		RecvCallback* back = new RecvCallback();
		back->ClientAddr = PerHandleData->GetAddr();
		back->lpBuffer = &PerIoData->databuff;
		back->sock = PerHandleData->GetSocket();
		(*pCallBackFun)(back);
		if (back)
		{
			delete back;
			back = NULL;
		}
		ReleaseMutex(hMutex);

		// 为下一个重叠调用建立单I/O操作数据
		ZeroMemory(&(PerIoData->overlapped), sizeof(OVERLAPPED)); // 清空内存
		PerIoData->databuff.len = 1024;
		PerIoData->databuff.buf = PerIoData->buffer;
		PerIoData->operationType = 0;	// read
		WSARecv(PerHandleData->GetSocket(), &(PerIoData->databuff), 1, &RecvBytes, &Flags, &(PerIoData->overlapped), NULL);
	}

	return 0;
}

DWORD WINAPI CSloongSocket::ServerSendThread(LPVOID IpParam)
{
	while (1)
	{
		
		WaitForSingleObject(hMutex, INFINITE);

		//send(m_vClient[i]->socket, talk, 200, 0);	// 发送信息

		ReleaseMutex(hMutex);
	}
	return 0;
}

void CSloongSocket::Close()
{
	closesocket(m_hSocket);
}
