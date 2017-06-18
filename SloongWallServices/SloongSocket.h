#pragma once

typedef LPTHREAD_START_ROUTINE SLOONGCALLBACKFUN;

struct AcceptProcParam
{
	SOCKET* m_pSocket;
	HANDLE* m_hIocp;
	SLOONGCALLBACKFUN pCallBackFun;
};

struct RecvParam
{
	HANDLE m_hIocp;
	SLOONGCALLBACKFUN pCallBackFun;
};

struct RecvCallback
{
	LPWSABUF lpBuffer;
	SOCKADDR_IN ClientAddr;
	SOCKET sock;
};

typedef class CSloongSocket
{
public:
	CSloongSocket();
	~CSloongSocket();
	HRESULT Initialize(SLOONGCALLBACKFUN AcceptFun, SLOONGCALLBACKFUN recvFun);
	HRESULT Listen(USHORT nPort, int Backlog = SOMAXCONN);
	VOID Accept();
	HRESULT Connect(USHORT nPort, LPCTSTR szIp);
	void Close();
	DWORD Send( LPWSABUF pWsaBuf,SOCKET sock,int flag = 0);
	HRESULT Recv(LPBYTE lpData, DWORD dwLen,int flag = 0);
	LPCTSTR GetConnectIP();
	SOCKET GetSocket();
	BOOL IsClientSocket();
	DWORD GetClientNum();
	SOCKADDR_IN GetAddr();

protected:
	DWORD				m_dwClientID;
	TCHAR				m_szClientIP[30];
	SOCKET				m_hSocket;
	SOCKADDR_IN			m_stClientAddr;
	HANDLE				m_hIocp;
	SLOONGCALLBACKFUN	m_recvFun;
	SLOONGCALLBACKFUN	m_acceptFun;
	static vector<CSloongSocket*> m_vClient;
	static DWORD WINAPI AcceptInThread(LPVOID lpParma);
	static DWORD WINAPI ServerWorkThread(LPVOID CompletionPortID);
	static DWORD WINAPI ServerSendThread(LPVOID IpParam);
}SLSOCKET,*LPSLSOCKET;

