// EASYCrypt.h: interface for the CPinCrypt class.
// Version: 1.2
// by Cooler 2003-06-02 liuyujun@263.net
// Crossing platform (Win32/Linux)
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EASYCRYPT_H__5A24C28D_EEA4_47D2_B751_09E1BAC4F637__INCLUDED_)
#define AFX_EASYCRYPT_H__5A24C28D_EEA4_47D2_B751_09E1BAC4F637__INCLUDED_

#include "bzcomdef.h"

// Macro define region
#define EASYCRYPT_INITCODE			"kSbzFS"
#define EASYCRYPT_INITCODELEN		6
#define MAXSIZE_EASYCRYPTKEY		(2+EASYCRYPT_INITCODELEN+10)
#define SIZE_CRYPTSUMMARY			16
#define SIZE_CRYPTSUMMARYCHUNK		5

// Struct define region
typedef struct tagEASYCRYPTKEY
{
	CHAR szEncryptKey[MAXSIZE_EASYCRYPTKEY];
	WORD wEncryptKeyLen;
}EASYCRYPTKEY, *PEASYCRYPTKEY;

class CPinCrypt  
{
protected:
	static void CryptData(LPSTR pData, 
				DWORD dwDataLen, 
				CONST EASYCRYPTKEY tagKey);
	static void GenCryptSummary(LPCSTR pData, 
				DWORD dwDataLen, 
				LPSTR pSummary);

public:
	CPinCrypt();
	virtual ~CPinCrypt();

	static BOOL Encrypt(LPSTR pEncryptData, 
				DWORD dwEncryptDataLen, 
				DWORD dwCryptSign);

	static BOOL Decrypt(LPSTR pDecryptData, 
				DWORD dwDecryptDataLen, 
				DWORD dwCryptSign);
};

#endif // !defined(AFX_EASYCRYPT_H__5A24C28D_EEA4_47D2_B751_09E1BAC4F637__INCLUDED_)
