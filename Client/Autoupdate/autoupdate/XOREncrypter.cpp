#include"stdafx.h"
#include"Encrypter.h"

#define XOR_KEY_LEN 64
#define XOR_KEY    "Designed by Brianyao 2007 Kingsoft Blaze Game Studio. Copyright "
#define MAX_FILE_BUFFER 256

class XOREncrypter:public IEncrypter
{
	static const char *        m_szKey;
	static const unsigned long m_KeyLen;

    char                       m_FileBuffer[MAX_FILE_BUFFER];
public:
	XOREncrypter();
	~XOREncrypter();
public:
  	inline void Encrypt(const char * szSourceBuffer,char * szDestBuffer,const unsigned long dwSize);
	inline BOOL Encrypt(const char * szSourceFileName,char * szDestFileName);
    void        Unencrypt(const char * szSourceBuffer,char * szDestBuffer,const unsigned long dwSize);
	BOOL        Unencrypt(const char * szSourceFileName,char * szDestFileName);
private:
	XOREncrypter(const XOREncrypter &);
	const XOREncrypter & operator = (const XOREncrypter &);
};

const char * XOREncrypter::m_szKey=XOR_KEY;
const unsigned long XOREncrypter::m_KeyLen=strlen(XOR_KEY);

IEncrypter * GetXOREncrypter(void)
{
	return new XOREncrypter;
}


XOREncrypter::XOREncrypter()
{/*Do Nothing at all*/}

XOREncrypter::~XOREncrypter()
{/*Do Nothing at all*/}

inline void XOREncrypter::Encrypt(const char * szSourceBuffer,char * szDestBuffer,const unsigned long dwSize)
{
   ASSERT(strlen(XOR_KEY)==XOR_KEY_LEN);
   unsigned long dwKeyIndex=0;
   for (int index=0;index<(int)dwSize;index++)
   {
        unsigned long value=szSourceBuffer[index];
        unsigned long key=m_szKey[dwKeyIndex];
		value=(value ^ key);
		szDestBuffer[index]=(char)value;
		dwKeyIndex=(dwKeyIndex+1) & XOR_KEY_LEN;
   }//endfor index
}

void XOREncrypter::Unencrypt(const char * szSourceBuffer,char * szDestBuffer,const unsigned long dwSize)
{
	Encrypt(szSourceBuffer,szDestBuffer,dwSize);
}

BOOL XOREncrypter::Encrypt(const char * szSourceFileName,char * szDestFileName)
{
	if (szSourceFileName==NULL || szDestFileName==NULL)
		return FALSE;

	HANDLE hRead = CreateFile(szSourceFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if ( hRead==INVALID_HANDLE_VALUE )
	{
		return FALSE;
	}//endif
	
	unsigned long   dwFileSizeHigh  = 0;
	unsigned long   dwFileSizeLow   = GetFileSize(hRead,&dwFileSizeHigh);
	
	_ASSERT(dwFileSizeHigh == 0);
	
    char * pTempBuffer   = new char [dwFileSizeLow];
	unsigned long   dwReaded = 0;
	
	BOOL hRes=ReadFile(hRead,pTempBuffer,dwFileSizeLow,&dwReaded,NULL);
	ASSERT(hRes && dwFileSizeLow==dwReaded);
	CloseHandle(hRead);
	if (!hRes || dwFileSizeLow!=dwReaded)
	{
		return FALSE;
	}//endif

	Encrypt(pTempBuffer,pTempBuffer,dwReaded);
	
	HANDLE  hWrite=CreateFile(szDestFileName,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	ASSERT(hWrite);
	if (hWrite==INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}//Write
	
	hRes=WriteFile(hWrite,pTempBuffer,dwFileSizeLow,&dwReaded,NULL);
	ASSERT(hRes && dwReaded==dwFileSizeLow);
	CloseHandle(hWrite);
	if (!hRes || dwReaded!=dwFileSizeLow)
	{
		return FALSE;
	}//endif
	
	delete [] pTempBuffer;
	
	return TRUE;
}

BOOL XOREncrypter::Unencrypt(const char * szSourceFileName,char * szDestFileName)
{
	return Encrypt(szSourceFileName,szDestFileName);
}