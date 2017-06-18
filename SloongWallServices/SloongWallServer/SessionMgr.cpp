#include "stdafx.h"
#include "SessionMgr.h"
CSessionMangage::CSessionMangage()
{
	m_pFreeSessionList = NULL;
	m_nFreeSessionCount = 0;
	m_nMaxFreeSessions = 3000;
	::InitializeCriticalSection(&m_FreeSessionListLock);
}
CSessionMangage::~CSessionMangage()
{
	::DeleteCriticalSection(&m_FreeSessionListLock);

}
CSession *CSessionMangage::AllocateSession()
{
	CSession *pSession;

	// ����һ��CIOCPContext����
	::EnterCriticalSection(&m_FreeSessionListLock);
	if(m_pFreeSessionList == NULL)
	{
		pSession = (CSession *)
				::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(CSession)); 

		::InitializeCriticalSection(&pSession->Lock);
	}
	else	
	{
		// �ڿ����б�������
		pSession = m_pFreeSessionList;
		m_pFreeSessionList = m_pFreeSessionList->pNext;
		pSession->pNext = NULL;

		m_nFreeSessionCount --;
	}
	::LeaveCriticalSection(&m_FreeSessionListLock);
	return pSession;
}

void CSessionMangage::ReleaseSession(CSession *pSession)
{
	if(pSession == NULL)
		return ;
	::EnterCriticalSection(&m_FreeSessionListLock);
	
	if(m_nFreeSessionCount <= m_nMaxFreeSessions) // ��ӵ������б�
	{
		// �Ƚ��ؼ�����α������浽һ����ʱ������
		CRITICAL_SECTION cstmp = pSession->Lock;
		// ��Ҫ�ͷŵ������Ķ����ʼ��Ϊ0
		memset(pSession, 0, sizeof(CSession));

		// �ٷŻ�ؼ�����α�������Ҫ�ͷŵ������Ķ�����ӵ������б�ı�ͷ
		pSession->Lock = cstmp;
		pSession->pNext = m_pFreeSessionList;
		m_pFreeSessionList = pSession;
		
		// ���¼���
		m_nFreeSessionCount ++;
	}
	else
	{
		::DeleteCriticalSection(&pSession->Lock);
		::HeapFree(::GetProcessHeap(), 0, pSession);
	}

	::LeaveCriticalSection(&m_FreeSessionListLock);
}
void CSessionMangage::FreeSession()
{
	// ����m_pFreeContextList�����б��ͷŻ��������ڴ�
	::EnterCriticalSection(&m_FreeSessionListLock);
	
	CSession *pFreeSession = m_pFreeSessionList;
	CSession *pNextSession;
	while(pFreeSession != NULL)
	{
		pNextSession = pFreeSession->pNext;
		
		::DeleteCriticalSection(&pFreeSession->Lock);
		if(!::HeapFree(::GetProcessHeap(), 0, pFreeSession))
		{
#ifdef _DEBUG
			::OutputDebugString(_T("  FreeContexts�ͷ��ڴ����"));
#endif // _DEBUG
			break;
		}
		else
		{
#ifdef _DEBUG
			OutputDebugString(_T("  FreeContexts�ͷ��ڴ棡\r\n"));
#endif // _DEBUG
		}
		pFreeSession = pNextSession;
	}
	m_pFreeSessionList = NULL;
	m_nFreeSessionCount = 0;

	::LeaveCriticalSection(&m_FreeSessionListLock);
}
void CSessionMangage::OnMessage(PACKET* lpPacket)
{

}