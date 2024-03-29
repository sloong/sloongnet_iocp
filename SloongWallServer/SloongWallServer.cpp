#include "stdafx.h"
#include "SloongWallServer.h"
#include "SloongSocket.h"
#include "random.h"
#include "IUniversal.h"
#include "Command.h"
#include "Connection.h"
#include "Recordset.h"
#include "UserInfo.h"

using namespace SoaringLoong;

CSloongWallServer* CSloongWallServer::pThis = NULL;
#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","rstEOF") rename("BOF","rstBOF")

CSloongWallServer::CSloongWallServer()
{
	pThis = this;
	m_pCmd = new CCommand();
	m_pConn = new CConnection();
	m_pRst = new CRecordset();
}

CSloongWallServer::~CSloongWallServer()
{
	Shutdown();
}

HRESULT CSloongWallServer::Initialize(IUniversal* pUni, ILogSystem* pLog)
{
	m_pUniversal = pUni;
	m_pLog = pLog;
	//CString str(_T("Provider=SQLNCLI11;Server=SERV-SQL\\SLSQLSERVER;Database=SloongWalls;Trusted_Connection=yes"));
// 	wstring str(L"Provider=SQLNCLI11;Server=serv-sql\\slsqlserver;Database=SLOONG_UserCenter;Uid=sa;Pwd=SoaringLoong!");
// 	m_pConn->SetConnectionString(str);
// 	m_pConn->Open(str, _T(""), _T(""), ConnectOption::adConnectUnspecified);
// 
// 	m_pCmd->SetActiveConnection(*m_pConn);

	return S_OK;
}



void CSloongWallServer::Shutdown()
{
	m_hServerSocket->Close();
	SAFE_DELETE(m_hServerSocket);
}


DWORD CSloongWallServer::OnAccept(LPVOID lpParameters)
{
	CSloongSocket* pConnect = (CSloongSocket*)lpParameters;
	pThis->m_pLog->Log(INF,0,pThis->m_pUniversal->Format(_T("%s is connect, id is %d."), pConnect->GetConnectIP(), pConnect->GetClientNum()));
	return S_OK;
}

void CSloongWallServer::WorkLoop()
{
	while (true)
	{
		TCHAR control[10];
		wscanf_s(TEXT("%s"), &control, _countof(control));
		if (_tcsicmp(TEXT("exit"), control) == 0 || _tcsicmp(TEXT("end"), control) == 0)
		{
			return;
		}
		else
		{
			continue;
		}
	}
}


DWORD LoadImageBuffer(LPCTSTR szFilePath,LPBYTE* lpBuffer)
{
	HANDLE nFileHandle;
	DWORD nSize = 0;

	nFileHandle = CreateFile(szFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == nFileHandle)
	{
		return GetLastError();
	}
	long lLength = GetFileSize(nFileHandle,NULL);
	*lpBuffer = new BYTE[lLength+1];
	memset(*lpBuffer, 0, lLength+1);
	BOOL bRes = ReadFile(nFileHandle, *lpBuffer, lLength, &nSize, NULL);
	CloseHandle(nFileHandle);
	if (FALSE == bRes)
	{
		return -1;
	}
	else
	{
		return nSize;
	}
}


HRESULT CSloongWallServer::Run()
{
	m_pLog->Log(INF,0, TEXT("Server is running."));

	FindAddFile(_T("D:\\OnlineDisk\\Sexy\\Beautyleg\\"), &m_vList);
// 	m_hServerSocket = new CSloongSocket();
// 	m_hServerSocket->Initialize(CSloongWallServer::OnAccept, OnRecv);
// 	m_hServerSocket->Listen(5000);
// 	m_hServerSocket->Accept();
	Start(8889);

	WorkLoop();

	return S_OK;
}

DWORD CSloongWallServer::GetNextIndex()
{
	int max = m_vList.size();
	int min = 0;
	int nNext = min + mtirand() % (max - min);

	return nNext;
}

DWORD CSloongWallServer::OnRecv(LPVOID lpParam)
{
	if (pThis)
	{
		pThis->Recv(lpParam);
	}
	return S_OK;
}

DWORD CSloongWallServer::FindAddFile(LPCTSTR szBaseDirectory, vector<wstring>* vList)
{
// 	CFileFind oFinder;
// 	CString strDir = szBaseDirectory;
// 	if (strDir.Right(1) != _T("\\"))
// 		strDir += _T("\\");
// 	strDir += _T("*.*");
// 	BOOL bWorking = oFinder.FindFile(strDir);
// 	while (bWorking)
// 	{
// 		bWorking = oFinder.FindNextFile();
// 		CString DataFile = oFinder.GetFilePath(); //GetFilePath：得到全路径名
// 		if (oFinder.IsDirectory() && !oFinder.IsDots())    //如果是目录 
// 		{
// 			FindAddFile(DataFile, vList);
// 		}
// 		else if (!oFinder.IsDirectory() && !oFinder.IsDots())//不为目录
// 		{
// 			CString FileName = oFinder.GetFilePath();   //GetFileName:得到带后缀的文件名
// 			vList->push_back(FileName);
// 		}
// 	}
	return vList->size();
}
#define SEND_STYLE_LIST			0x01
#define SEND_PICTURE_LIST		0x02
#define SEND_OPERATION_RESULT	0x03

#define RECV_STYLE_LIST			0x01
#define RECV_PICTURE_LIST		0x02
#define RECV_USER_LOGIN			0x03
#define RECV_USER_UPDATE		0x04

void CSloongWallServer::Recv(LPVOID lpParam)
{
	RecvCallback* pParam = (RecvCallback*)lpParam;
	WSABUF* pWsa = pParam->lpBuffer;

	TCHAR szClientIP[30] = { 0 };
	InetNtop(AF_INET, &pParam->ClientAddr.sin_addr, szClientIP, 30);
	m_pLog->Log(INF, 0, m_pUniversal->Format(TEXT("%s client need %d images."), szClientIP, pWsa->len));
	switch (pWsa->buf[0])
	{
	case RECV_STYLE_LIST:
		break;
	case RECV_PICTURE_LIST:
		RecvPictureList(pWsa, pParam->sock);
		break;
	case RECV_USER_LOGIN:
		
		break;
	case RECV_USER_UPDATE:
		break;
	default:
		break;
	}
}

int CSloongWallServer::RecvPictureList(WSABUF* pWsa, SOCKET client)
{/*
	// Compute the target style
	int nIndex = 1;
	int nSize = 0;
	memcpy_s(&nSize, sizeof(nSize), &pWsa->buf[nIndex], sizeof(nSize));
	nIndex += sizeof(nSize);

	LPTSTR str = new TCHAR[nSize + 1];
	memcpy_s(&str, nSize + 1, &pWsa->buf[nIndex], nSize);
	nIndex += nSize;
	str[nSize + 1] = '\0';

	int nCount = 0;
	memcpy_s(&nCount, sizeof(nCount), &pWsa->buf[nIndex], sizeof(nCount));
	
	// Message head
	WCHAR szHeader[12] = { _T("SloongWalls") };
	int nHeaderSize = wcslen(szHeader);

	for (int i = 0; i < nCount; i++ )
	{

	}

	// Next file 
	DWORD nNext = pThis->GetNextIndex();
	wstring strNextFile = m_vList[nNext];
	int nFileNameSize = strNextFile.length();

	// Load file
	LPBYTE lpFileData = NULL;
	int nFileSize = LoadImageBuffer(strNextFile, &lpFileData);

	// Build send message
	DWORD nMsgSize = sizeof(nHeaderSize) + nHeaderSize + sizeof(nFileNameSize) + nFileNameSize + sizeof(nFileSize) + nFileSize;
	LPBYTE pBuffer = new BYTE[nMsgSize];
	memset(pBuffer, 0, nMsgSize);
	// Copy header
	int index = 0;
	memcpy_s(pBuffer + index, nHeaderSize, szHeader, nHeaderSize);
	index += nHeaderSize;
	// Copy Send pack size
	memcpy_s(pBuffer + index, sizeof(nMsgSize), (LPBYTE)&nMsgSize, sizeof(nMsgSize));
	index += sizeof(nMsgSize);
	// Copy filename
	memcpy_s(pBuffer + index, sizeof(nFileNameSize), (LPBYTE)&nFileNameSize, sizeof(nFileNameSize));
	index += sizeof(nFileNameSize);
	memcpy_s(pBuffer + index, nFileNameSize, strNextFile, nFileNameSize);
	index += nFileNameSize;
	// Copy file
	memcpy_s(pBuffer + index, sizeof(nFileSize), (LPBYTE)&nFileSize, sizeof(nFileSize));
	index += sizeof(nFileSize);
	memcpy_s(pBuffer + index, nFileSize, lpFileData, nFileSize);
	index += nFileSize;

	// check 
	if (nMsgSize != index)
	{
		m_pLog->Log(INF, 0, m_pUniversal->Format(TEXT("Build send picture list data have warning: compare size:%d, data size:%d."), nMsgSize, index));
	}

	// Send
	WSABUF sendBuf;
	sendBuf.buf = (CHAR*)pBuffer;
	sendBuf.len = nMsgSize;
	DWORD sendSize = pThis->m_hServerSocket->Send(&sendBuf, client);
	m_pLog->Log(INF, 0, m_pUniversal->Format(TEXT("Pack size: %d .Send Size: %d."), nMsgSize, sendSize));
	SAFE_DELETE_ARR(lpFileData);
	SAFE_DELETE_ARR(pBuffer);
	return sendSize;*/
	return 0;
}

DWORD CSloongWallServer::QueryInfo()
{
	m_pCmd->SetCommandText(_T("select * from [Table]"));
	m_pCmd->ExecuteQuery(*m_pRst, CommandType::adCmdText);

	ShowData();
	return 0;
}

DWORD CSloongWallServer::AddInfoToDB()
{
	wstring sql = _T("");
	//sql.Format(_T("insert into [Table](id,name,phone,email) values(%d,'%s','%s','%s')"), m_id, m_name, m_phone, m_email);
	CUserInfo user;
	user.m_nID = 1;
	user.m_strEMail = _T("Admin@sloong.com");
	user.m_strIP = _T("127.0.0.1");
	user.m_strName = _T("Admin");
	user.m_strNickname = _T("TestUser");
	user.m_strPassword = _T("SoaringLoong");
	sql = user.BuildAddSql();
	m_pCmd->SetCommandText(sql);
	long rows = 0;
	m_pCmd->ExecuteUpdate(rows, *m_pRst, CommandType::adCmdText);

// 	wstring msgStr;
// 	msgStr.Format(_T("成功添加了%d行。"), rows);
	return rows;
}

DWORD CSloongWallServer::RemoveFromDB( int nId)
{
// 	wstring sql;
// 	sql.Format(_T("delete from [Table] where id='%d'"), nId);
// 
// 	m_pCmd->SetCommandText(sql);
 	long rows = 0;
// 	m_pCmd->ExecuteUpdate(rows, *m_pRst, CommandType::adCmdText);
// 
// 	CString msgStr;
// 	msgStr.Format(_T("删除了%d行。"), rows);
	return rows;
}

DWORD CSloongWallServer::GetFirst()
{
	m_pRst->MoveFirst();
	ShowData();
	return 0;
}

DWORD CSloongWallServer::GetPrev()
{
	m_pRst->MovePrevious();
	ShowData();
	return 0;
}

DWORD CSloongWallServer::GetNext()
{
	m_pRst->MoveNext();
	ShowData();
	return 0;
}

DWORD CSloongWallServer::GetLast()
{
	m_pRst->MoveLast();
	ShowData();
	return 0;
}

DWORD CSloongWallServer::ShowData()
{
	if (m_pRst->GetRecordCount() > 0)
	{
		int id = m_pRst->GetInt(_T("id"));
		wstring name = m_pRst->GetString(_T("name"));
		wstring phone = m_pRst->GetString(_T("phone"));
		wstring email = m_pRst->GetString(_T("email"));

		// 当前数据索引
		int m_cur = m_pRst->GetAbsolutePosition();
		// 数据总个数
		int m_total = m_pRst->GetRecordCount();
	}
	else
	{
		//MessageBox(_T("No data!"));

		m_pRst->Release();
		m_pCmd->Release();
		m_pConn->Release();
	}
	return 0;
}

void CSloongWallServer::HandleRecvMessage(PACKET* lpPacket)
{
	OnRecv(lpPacket);
}


void MakeThumbnail(LPCTSTR originalImagePath, LPCTSTR thumbnailPath, int width, int height)
{
// 	CBitmap* pImage = NULL;
// 	HBITMAP	hBmp = NULL;
// 	Bitmap img(originalImagePath);
// 	//获得缩略图
// 	Bitmap* pThumbnail = static_cast<Bitmap*>(img.GetThumbnailImage(width, height, NULL, NULL));
// 	pThumbnail->GetHBITMAP(NULL, &hBmp);
// 	pImage = new CBitmap();
// 	pImage->Attach(hBmp);
}
