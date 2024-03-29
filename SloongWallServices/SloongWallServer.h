#pragma once
#include "SloongServer.h"

class CSloongSocket;
namespace SoaringLoong
{
	class IUniversal;
	class ILogSystem;
}
using namespace SoaringLoong;

class CCommand;
class CConnection;
class CRecordset;

class CSloongWallServer : public CSloongServer
{
public:
	CSloongWallServer();
	~CSloongWallServer();

	HRESULT Initialize(IUniversal* pUni, ILogSystem* pLog);
	void Shutdown();
	HRESULT Run();

	
	void WorkLoop(); 
	
	DWORD FindAddFile(LPCTSTR szBaseDirectory, vector<wstring>* vList);
	DWORD GetNextIndex();

	void Recv(LPVOID lpParam);
	int RecvPictureList(WSABUF* wsa, SOCKET client);
	DWORD AddInfoToDB();
	DWORD RemoveFromDB( int id);
	DWORD QueryInfo();
	DWORD GetFirst();
	DWORD GetPrev();
	DWORD GetNext();
	DWORD GetLast();
	DWORD ShowData();
	void HandleRecvMessage(PACKET* lpPacket);
public:
	static DWORD WINAPI OnAccept(LPVOID lpParameters);
	static DWORD WINAPI OnRecv(LPVOID lpParameters);
	static void AcceptInThread(CSloongSocket* sock);

private:
	CSloongSocket* m_hServerSocket;
	vector<wstring> m_vList;
public:
	ILogSystem* m_pLog;
	IUniversal* m_pUniversal;
	static CSloongWallServer* pThis;

	CConnection* m_pConn;
	CRecordset*  m_pRst;
	CCommand*    m_pCmd;
};