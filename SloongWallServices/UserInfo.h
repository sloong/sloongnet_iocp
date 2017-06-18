#pragma once
class CUserInfo
{
public:
	CUserInfo();
	~CUserInfo();
	wstring BuildAddSql();
	wstring BuildUpdateSql();

public:
	int m_nID;
	wstring m_strName;
	wstring m_strNickname;
	wstring m_strPassword;
	wstring m_strEMail;
	wstring m_strIP;
};

