#include "stdafx.h"
#include "MD5.h"
#include <Wincrypt.h>

#define CHECK_NULL_RET(bCondition) if (!bCondition) goto Exit0
#define BUFSIZE 1024
#define MD5LEN  16

CMD5::CMD5()
{
}


CMD5::~CMD5()
{
}

BOOL GetMD5(
	BYTE *pszFilePath,
	BOOL bFile,
	BOOL bUpperCase,
	TCHAR *pszResult,
	DWORD &dwStatus)
{
	BOOL bResult = FALSE;
	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;
	HANDLE hFile = NULL;
	BYTE rgbFile[BUFSIZE];
	DWORD cbRead = 0;
	BYTE rgbHash[MD5LEN];
	DWORD cbHash = 0;
	CHAR rgbDigitsL[] = "0123456789abcdef";
	CHAR rgbDigitsU[] = "0123456789ABCDEF";
	CHAR *rgbDigits = bUpperCase ? rgbDigitsU : rgbDigitsL;
	TCHAR szResult[MD5LEN * 2 + 1] = { 0 };

	dwStatus = 0;
	bResult = CryptAcquireContext(&hProv,
		NULL,
		NULL,
		PROV_RSA_FULL,
		CRYPT_VERIFYCONTEXT);
	CHECK_NULL_RET(bResult);

	bResult = CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash);
	CHECK_NULL_RET(bResult);

	if (bFile)
	{
		hFile = CreateFile((TCHAR *)pszFilePath,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_SEQUENTIAL_SCAN,
			NULL);
		CHECK_NULL_RET(!(INVALID_HANDLE_VALUE == hFile));

		while (bResult = ReadFile(hFile, rgbFile, BUFSIZE,
			&cbRead, NULL))
		{
			if (0 == cbRead)
			{
				break;
			}

			bResult = CryptHashData(hHash, rgbFile, cbRead, 0);
			CHECK_NULL_RET(bResult);
		}
	}
	else
	{
		bResult = CryptHashData(hHash, pszFilePath, strlen((CHAR *)pszFilePath), 0);
		CHECK_NULL_RET(bResult);
	}

	cbHash = MD5LEN;
	if (bResult = CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
	{
		TCHAR szTmpBuff[3] = { 0 };
		for (DWORD i = 0; i < cbHash; i++)
		{
			swprintf_s(szTmpBuff,3, TEXT("%c%c"), rgbDigits[rgbHash[i] >> 4],
				rgbDigits[rgbHash[i] & 0xf]);
			lstrcat(szResult, szTmpBuff);
		}
		bResult = TRUE;
	}

Exit0:
	dwStatus = GetLastError();
	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);
	CloseHandle(hFile);

	lstrcpy(pszResult, szResult);

	return bResult;
}

string ComputeMD5(const char* szData, size_t len)
{
	HCRYPTPROV hCryptProv;
	HCRYPTHASH hHash;
	BYTE bHash[0x7f];
	DWORD dwHashLen = 16;
	string digest;

	if (CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL,
		CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET) &&
		CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hHash) &&
		CryptHashData(hHash, (BYTE*)szData, (DWORD)len, 0) &&
		CryptGetHashParam(hHash, HP_HASHVAL, bHash, &dwHashLen, 0))
	{
		digest.resize(32);
		BYTE value;
		for (int i = 0; i < 16; i++)
		{
			value = bHash[i] >> 4;
			digest[i * 2] = value < 10 ? char(value + '0') : char(value - 10 + 'a');
			value = bHash[i] & 0x0F;
			digest[i * 2 + 1] = value < 10 ? char(value + '0') : char(value - 10 + 'a');
		}
	}

	CryptDestroyHash(hHash);
	CryptReleaseContext(hCryptProv, 0);
	return digest;
}

// 计算文件的MD5字符串，支持大文件(large file)
string ComputeFileMD5(const TCHAR* szFilename)
{
	HCRYPTPROV hCryptProv = 0;
	HCRYPTHASH hHash = 0;
	HANDLE hMapFile = 0;

	BYTE bHash[0x7f];
	DWORD dwHashLen = 16;
	BYTE* pBuf = nullptr;
	string digest;
	DWORD dwFileSize = 0, dwHighFileSize = 0;

	HANDLE hFile = CreateFile(szFilename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return digest;
	}

	dwFileSize = GetFileSize(hFile, &dwHighFileSize);

	hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);

	if (hMapFile == NULL || hMapFile == INVALID_HANDLE_VALUE)
	{
		goto release;
	}

	if (CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET))
	{
		if (CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hHash))
		{
			UINT64 filesize = (UINT64)dwHighFileSize << 32 | dwFileSize;
			for (UINT64 i = 0; i <= filesize; i += 1048576)// 1024*1024
			{
				size_t bufsize = size_t(min(filesize - i, 1048576));
				pBuf = (BYTE*)MapViewOfFile(hMapFile, FILE_MAP_READ, (DWORD)(i >> 32), (DWORD)i, bufsize);

				if (pBuf == NULL)
				{
					goto release;
				}

				if (!CryptHashData(hHash, pBuf, bufsize, 0))
				{
					goto release;
				}
				UnmapViewOfFile(pBuf);
			}
			if (CryptGetHashParam(hHash, HP_HASHVAL, bHash, &dwHashLen, 0))
			{
				digest.resize(32);
				BYTE value;
				for (int i = 0; i < 16; i++)
				{
					value = bHash[i] >> 4;
					digest[i * 2] = value < 10 ? char(value + '0') : char(value - 10 + 'a');
					value = bHash[i] & 0x0F;
					digest[i * 2 + 1] = value < 10 ? char(value + '0') : char(value - 10 + 'a');
				}
			}
		}
	}

release:
	CryptDestroyHash(hHash);
	CryptReleaseContext(hCryptProv, 0);
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
	CloseHandle(hFile);
	return digest;
}