#pragma once
#include "IOCPServer.h"
#include <list>
#include "log.h"
#include "MessageParser.h"

typedef unsigned (WINAPI *PTHREADFUN)(LPVOID lpParameter);

template <class T>
class CQueue : public list < T >
{
	CRITICAL_SECTION m_ListLock;
public:
	CQueue(void)
	{
		::InitializeCriticalSection(&m_ListLock);

	}
	virtual ~CQueue(void)
	{
		::DeleteCriticalSection(&m_ListLock);
	}

	unsigned long GetSize()
	{
		unsigned long size = 0;
		::EnterCriticalSection(&m_ListLock);
		size = size();
		::LeaveCriticalSection(&m_ListLock);
		return size;
	}

	void Push(T lpData)
	{
		::EnterCriticalSection(&m_ListLock);
		push_front(lpData);
		::LeaveCriticalSection(&m_ListLock);
	}

	T Pop()
	{
		T lpData = NULL;
		::EnterCriticalSection(&m_ListLock);
		if (size())
		{
			lpData = front();
			pop_front();
		}
		::LeaveCriticalSection(&m_ListLock);
		return lpData;
	}
	bool Empty()
	{
		bool bRet = false;
		::EnterCriticalSection(&m_ListLock);
		bRet = empty();
		::LeaveCriticalSection(&m_ListLock);
		return bRet;
	}
};

class CSloongServer :
	public CIOCPServer
{
public:
	CSloongServer();
	~CSloongServer();

public:
	void AddPacketToRecvlist(PACKET* lpPacket)
	{
		m_listRecvMsg.Push(lpPacket);
	}
	PACKET* PopPacketFromRecvList()
	{
		return m_listRecvMsg.Pop();
	}
	void AddPacketToSendlist(PACKET* lpPacket)
	{
		m_listSendMsg.Push(lpPacket);
	}
	PACKET* PopPacketFromSendList()
	{
		return m_listSendMsg.Pop();
	}
	//void AddPacketToDelaylist(PACKET* lpPacket)
	//{
	//	m_listDelayMsg.Push(lpPacket);
	//}
	//PACKET* PopPacketFromDelayList()
	//{
	//	return m_listDelayMsg.Pop();
	//}
private:
	CQueue<PACKET*> m_listRecvMsg;
	CQueue<PACKET*> m_listSendMsg;
	//CQueue<PACKET*> m_listDelayMsg;
	//��Ϣ�����߳�
private:
	HANDLE m_hRecvWait, m_hSendWait/*,m_hDelayWait*/;
	bool m_bRecvRun, m_bSendRun/*,m_bDelayRun*/;
	HANDLE m_hRecvThread, m_hSendThread/*,m_hDelayThread*/;

	static DWORD WINAPI RecvThread(LPVOID lpParameter);
	static DWORD WINAPI SendThread(LPVOID lpParameter);
	//static DWORD WINAPI DelayThread(LPVOID lpParameter);

	bool StartupAllMsgThread();
	void CloseAllMsgThread();
	virtual void HandleRecvMessage(PACKET* lpPacket) = 0;
	//��Ϣ�ڴ����

protected:
	PACKET* AllocatePacket();
	void ReleasePacket(PACKET *pPacket);
	void FreePacket();
private:
	PACKET *m_pFreePacketList;
	int m_nFreePacketCount;
	int m_nMaxPacketBuffers;
	CRITICAL_SECTION m_FreePacketListLock;
	//�����¼�֪ͨ
private:
	void OnConnectionEstablished(CIOCPContext *pContext, CIOCPBuffer *pBuffer);
	void OnConnectionClosing(CIOCPContext *pContext, CIOCPBuffer *pBuffer);
	void OnConnectionError(CIOCPContext *pContext, CIOCPBuffer *pBuffer, int nError);
	void OnReadCompleted(CIOCPContext *pContext, CIOCPBuffer *pBuffer);
	void OnWriteCompleted(CIOCPContext *pContext, CIOCPBuffer *pBuffer);
	bool SplitPacket(CIOCPContext *pContext, CIOCPBuffer *pBuffer);
	void OnPacketError(){ /*WriteToLog("�������!\r\n");*/ }
};
