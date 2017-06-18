#include "stdafx.h"
#include "UserInfo.h"


CUserInfo::CUserInfo()
{
}


CUserInfo::~CUserInfo()
{
}

CString CUserInfo::BuildAddSql()
{
	CString strDateNow;
	CString addSql;
	CString strTemp;
	addSql.Format(_T("insert into [UserList] (Name,Nickname,Password,EMail,RegistTime,RegistIP,LastLoginTime,LastLoginIP) VALUES ('%s','%s','%s','%s','%s','%s','%s','%s')"),
		m_strName, m_strNickname, m_strPassword, m_strEMail, strDateNow, m_strIP, strDateNow, m_strIP);
// 	CString item;
// 	strTemp.Format(_T("ID=%d,"), m_nID);
// 	item += strTemp;
// 	strTemp.Format(_T("Name=%s,"), m_strName);
// 	item += strTemp;
// 	strTemp.Format(_T("Nickname=%s,"),m_strNickname);
// 	item += strTemp;
// 	strTemp.Format(_T("Password=%s,"), m_strPassword);
// 	item += strTemp;
// 	strTemp.Format(_T("EMail=%s,"), m_strEMail);
// 	item += strTemp;
// 	strTemp.Format(_T("RegistTime=%s,"), strDateNow);
// 	item += strTemp;
// 	strTemp.Format(_T("RegistIP=%s,"), m_strIP);
// 	item += strTemp;
// 	strTemp.Format(_T("LastLoginTime=%s,"), strDateNow);
// 	item += strTemp;
// 	strTemp.Format(_T("LastLoginIP=%s,"), m_strIP);
// 	item += strTemp;
// 	addSql += item;
	return addSql;
}

CString CUserInfo::BuildUpdateSql()
{
	CString updateSql;
	CString item;
	CString temp;
	if (!m_strName.IsEmpty())
	{
		temp.Format(_T("Name=%s"),m_strName);
		item += temp;
	}
	return L"";
}
