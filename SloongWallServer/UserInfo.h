#pragma once
class CUserInfo
{
public:
	CUserInfo();
	~CUserInfo();
	CString BuildAddSql();
	CString BuildUpdateSql();

public:
	int m_nID;
	CString m_strName;
	CString m_strNickname;
	CString m_strPassword;
	CString m_strEMail;
	CString m_strIP;
};

