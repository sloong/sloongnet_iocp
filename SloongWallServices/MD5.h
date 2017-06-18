#pragma once
class CMD5
{
public:
	CMD5();
	~CMD5();
	BOOL GetMD5(BYTE *pszFilePath,
		BOOL bFile,
		BOOL bUpperCase,
		TCHAR *pszResult,
		DWORD &dwStatus);
	string ComputeFileMD5(const TCHAR* szFilename);
	string ComputeMD5(const char* szData, size_t len);
};

