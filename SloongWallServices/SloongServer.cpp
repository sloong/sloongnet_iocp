#include "stdafx.h"
#include "SloongServer.h"
#include <process.h>

CSloongServer::CSloongServer()
{
	m_pFreePacketList = NULL;
	m_nMaxPacketBuffers = 20000;
	m_nFreePacketCount = 0;
	::InitializeCriticalSection(&m_FreePacketListLock);
}
CSloongServer::~CSloongServer()
{
	m_pFreePacketList = NULL;
	m_nMaxPacketBuffers = 0;
	m_nFreePacketCount = 0;
	FreePacket();
	::DeleteCriticalSection(&m_FreePacketListLock);
}
PACKET *CSloongServer::AllocatePacket()
{
	PACKET *pPacket = NULL;

	// Ϊ���������������ڴ�
	::EnterCriticalSection(&m_FreePacketListLock);
	if (m_pFreePacketList == NULL)  // �ڴ��Ϊ�գ������µ��ڴ�
	{
		pPacket = (PACKET *)::HeapAlloc(GetProcessHeap(),
			HEAP_ZERO_MEMORY, sizeof(PACKET));
	}
	else	// ���ڴ����ȡһ����ʹ��
	{
		pPacket = m_pFreePacketList;
		m_pFreePacketList = m_pFreePacketList->pNext;
		pPacket->pNext = NULL;
		m_nFreePacketCount--;
	}
	::LeaveCriticalSection(&m_FreePacketListLock);

	return pPacket;
}

void CSloongServer::ReleasePacket(PACKET *pPacket)
{
	::EnterCriticalSection(&m_FreePacketListLock);

	if (m_nFreePacketCount <= m_nMaxPacketBuffers)	// ��Ҫ�ͷŵ��ڴ���ӵ������б���
	{
		memset(pPacket, 0, sizeof(PACKET));
		pPacket->pNext = m_pFreePacketList;
		m_pFreePacketList = pPacket;
		m_nFreePacketCount++;
	}
	else			// �Ѿ��ﵽ���ֵ���������ͷ��ڴ�
	{
		::HeapFree(::GetProcessHeap(), 0, pPacket);
	}
	::LeaveCriticalSection(&m_FreePacketListLock);
}
void CSloongServer::FreePacket()
{
	// ����m_pFreeBufferList�����б��ͷŻ��������ڴ�
	::EnterCriticalSection(&m_FreePacketListLock);

	PACKET *pFreePacket = m_pFreePacketList;
	PACKET *pNextPacket;
	while (pFreePacket != NULL)
	{
		pNextPacket = pFreePacket->pNext;
		if (!::HeapFree(::GetProcessHeap(), 0, pFreePacket))
		{
#ifdef _DEBUG
			::OutputDebugString(L"  FreeBuffers�ͷ��ڴ����");
#endif // _DEBUG
			break;
		}
		else
		{
#ifdef _DEBUG
			OutputDebugString(L"  FreeBuffers�ͷ��ڴ棡");
#endif // _DEBUG
		}

		pFreePacket = pNextPacket;
	}
	m_pFreePacketList = NULL;
	m_nFreePacketCount = 0;

	::LeaveCriticalSection(&m_FreePacketListLock);
}
bool CSloongServer::StartupAllMsgThread()
{
	m_bRecvRun = true;
	m_hRecvThread = NULL;
	m_hRecvWait = NULL;
	m_hRecvWait = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_hRecvWait == NULL)
		return false;
	m_hRecvThread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, NULL, (PTHREADFUN)RecvThread, this, 0, NULL));
	if (m_hRecvThread == NULL)
	{
		return false;
	}
	m_bSendRun = true;
	m_hSendThread = NULL;
	m_hSendWait = NULL;
	m_hSendWait = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_hSendWait == NULL)
		return false;
	m_hSendThread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, NULL, (PTHREADFUN)SendThread, this, 0, NULL));
	if (m_hSendThread == NULL)
	{
		return false;
	}
	//m_bDelayRun = true;
	//m_hDelayThread = NULL;
	//m_hDelayWait = NULL;
	//m_hDelayWait = CreateEvent(NULL,FALSE,FALSE,NULL);
	//if (m_hDelayWait==NULL)
	//	return false;
	//m_hDelayThread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, NULL, (PTHREADFUN) DelayThread, this, 0, NULL)); 
	//if (m_hDelayThread == NULL) 
	//{ 
	//	return false;
	//} 
	return true;

}
void CSloongServer::CloseAllMsgThread()
{
	m_bRecvRun = false;
	SetEvent(m_hRecvWait);
	if (WaitForSingleObject(m_hRecvThread, 10000) != WAIT_OBJECT_0)
		TerminateThread(m_hRecvThread, 0);
	CloseHandle(m_hRecvThread);
	CloseHandle(m_hRecvWait);
	m_bSendRun = false;
	SetEvent(m_hSendWait);
	if (WaitForSingleObject(m_hSendThread, 10000) != WAIT_OBJECT_0)
		TerminateThread(m_hSendThread, 0);
	CloseHandle(m_hSendThread);
	CloseHandle(m_hSendWait);
	//m_bDelayRun = false;
	//SetEvent(m_hDelayWait);
	//if(WaitForSingleObject(m_hDelayThread,10000)!= WAIT_OBJECT_0)
	//	TerminateThread(m_hDelayThread, 0);
	//CloseHandle(m_hDelayThread);
	//CloseHandle(m_hDelayWait);
}
DWORD WINAPI CSloongServer::RecvThread(LPVOID lpParameter)
{
	CSloongServer *lpUserServer = (CSloongServer*)lpParameter;
	while (lpUserServer->m_bRecvRun)
	{
		PACKET* lpPacket = NULL;
		if (!lpUserServer->m_listRecvMsg.Empty())
		{
			lpPacket = lpUserServer->m_listRecvMsg.Pop();
			if (lpPacket != NULL)
			{
				lpUserServer->HandleRecvMessage(lpPacket);

			}
			if (lpPacket != NULL)
				lpUserServer->ReleasePacket(lpPacket);
		}
		WaitForSingleObject(lpUserServer->m_hRecvWait, 1);
	}
	return 0;
}
DWORD WINAPI CSloongServer::SendThread(LPVOID lpParameter)
{
	return 0;
}
//DWORD WINAPI lpUserServer::DelayThread(LPVOID lpParameter)
//{
//	return 0;
//}
void CSloongServer::OnConnectionEstablished(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
{
	char szIP[30] = { 0 };
	InetNtopA(AF_INET, &pContext->addrRemote.sin_addr, szIP, 30);
	printf(" ���յ�һ���µ����ӣ�%d���� %s \n", GetCurrentConnection(), szIP);

	SendText(pContext, pBuffer->buff, pBuffer->nLen);
}

void CSloongServer::OnConnectionClosing(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
{
	printf(" һ�����ӹرգ� \n");
}

void CSloongServer::OnConnectionError(CIOCPContext *pContext, CIOCPBuffer *pBuffer, int nError)
{
	printf(" һ�����ӷ������� %d \n ", nError);
}

void CSloongServer::OnReadCompleted(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
{
	SplitPacket(pContext, pBuffer);
	SendText(pContext, pBuffer->buff, pBuffer->nLen);
}

void CSloongServer::OnWriteCompleted(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
{
	printf(" ���ݷ��ͳɹ���\n ");
}
bool CSloongServer::SplitPacket(CIOCPContext *pContext, CIOCPBuffer *pBuffer)
{
	CIOCPContext* lpSession = pContext;
	//ԭʼ����

	DWORD dwDataLen = (DWORD)(lpSession->lpBufEnd - lpSession->lpBufBegin);
	//�յ�����
	DWORD dwByteCount = pBuffer->nLen;


	//��������������ˣ���������ǰ��
	if (USE_DATA_LONGTH - (lpSession->lpBufEnd - lpSession->arrayDataBuf) < (int)dwByteCount)
	{
		//ƴ������������ǰ��
		//�����������Ȼ�������ͽ�֮ǰ������ȫ������
		if (USE_DATA_LONGTH - dwDataLen < dwByteCount)
		{
			dwDataLen = 0;
			lpSession->lpBufBegin = lpSession->lpBufEnd = lpSession->arrayDataBuf;
			OnPacketError();
			return false;
		}
		else
		{
			memcpy(lpSession->arrayDataBuf, lpSession->lpBufBegin, dwDataLen);//�ƶ�����
			lpSession->lpBufBegin = lpSession->arrayDataBuf;
			lpSession->lpBufEnd = lpSession->lpBufBegin + dwDataLen;//����βָ��
		}

	}

	//copy���ݵ�������β��
	memcpy(lpSession->lpBufEnd, pBuffer->buff, dwByteCount);
	lpSession->lpBufEnd += dwByteCount;//���»���βָ��
	dwDataLen = (DWORD)(lpSession->lpBufEnd - lpSession->lpBufBegin);//���»��泤��
	while (dwDataLen)
	{
		BYTE Mask = lpSession->lpBufBegin[0];

		if (Mask != 128)
		{
			lpSession->lpBufBegin = lpSession->lpBufEnd = lpSession->arrayDataBuf;
			OnPacketError();
			return false;

		}
		if (dwDataLen <= 3)//û���յ����ݰ��ĳ��� // byte 128 WORD longth; 
			break;
		short int longth = *(short int*)(lpSession->lpBufBegin + 1);
		//���ݳ��ȳ����Ϸ�����
		if (longth > NET_DATA_LONGTH || longth < 3)
		{
			lpSession->lpBufBegin = lpSession->lpBufEnd = lpSession->arrayDataBuf;
			OnPacketError();
			return false;
		}
		if (longth + 3 >(long)dwDataLen)//û���γ����������ݰ�
			break;
		//if(*(long*)(lpSession->m_lpBufBegin+3) != NET_MESSAGE_CHECK_NET)
		//{
		//	LPGAMEMSG lpGameMsg = m_Msg_Pool.MemPoolAlloc();
		//	lpGameMsg->length = longth;
		//	memset(lpGameMsg->arrayDataBuf,0,USE_DATA_LONGTH);
		//	*(long*)lpGameMsg->arrayDataBuf = longth;
		//	memcpy(lpGameMsg->arrayDataBuf+sizeof(long),lpSession->m_lpBufBegin+3,longth);
		//	lpGameMsg->lpSession = lpSession;
		//	m_Msg_Queue.Push(lpGameMsg);
		//	lpGameMsg = NULL;
		//}
		////���»���ͷָ��
		unsigned char arraybuffer[USE_DATA_LONGTH];
		ZeroMemory(arraybuffer, sizeof(arraybuffer));
		*(long*)arraybuffer = longth;
		memcpy(arraybuffer + sizeof(long), lpSession->lpBufBegin + 3, longth);

		DumpBuffToScreen(arraybuffer, longth + 4);

		lpSession->lpBufBegin += longth + 3;
		dwDataLen = (WORD)(lpSession->lpBufEnd - lpSession->lpBufBegin);

	}
	return true;

}