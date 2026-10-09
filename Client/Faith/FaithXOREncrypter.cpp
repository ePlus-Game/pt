
#include "KWin32.h"
#include "FaithEncrypter.h"

#define XOR_KEY_LEN 64
#define XOR_KEY    "Designed by Brianyao 2007 Kingsoft Blaze Game Studio. Copyright "
#define MAX_FILE_BUFFER 256

class FaithXOREncrypter:public IFaithEncrypter
{
	static const char *        m_szKey;
	static const unsigned long m_KeyLen;

    char                       m_FileBuffer[MAX_FILE_BUFFER];
public:
	FaithXOREncrypter();
	~FaithXOREncrypter();
public:
  	inline void Encrypt(const char * szSourceBuffer,char * szDestBuffer,const unsigned long dwSize);
	inline BOOL Encrypt(const char * szSourceFileName, const char * szDestFileName);
    void        Unencrypt(const char * szSourceBuffer,char * szDestBuffer,const unsigned long dwSize);
	BOOL        Unencrypt(const char * szSourceFileName, const char * szDestFileName);
private:
 	FaithXOREncrypter(const FaithXOREncrypter &);
 	const FaithXOREncrypter & operator = (const FaithXOREncrypter &);
};

const char * FaithXOREncrypter::m_szKey=XOR_KEY;
const unsigned long FaithXOREncrypter::m_KeyLen=strlen(XOR_KEY);

IFaithEncrypter * GetXOREncrypter(void)
{
	static FaithXOREncrypter single;
	return &single;
}


FaithXOREncrypter::FaithXOREncrypter()
{/*Do Nothing at all*/}

FaithXOREncrypter::~FaithXOREncrypter()
{/*Do Nothing at all*/}

inline void FaithXOREncrypter::Encrypt(const char * szSourceBuffer,char * szDestBuffer,const unsigned long dwSize)
{
//ASSERT(strlen(XOR_KEY)==XOR_KEY_LEN);
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

void FaithXOREncrypter::Unencrypt(const char * szSourceBuffer,char * szDestBuffer,const unsigned long dwSize)
{
	Encrypt(szSourceBuffer,szDestBuffer,dwSize);
}

BOOL FaithXOREncrypter::Encrypt(const char * szSourceFileName, const char * szDestFileName)
{
	if (szSourceFileName==NULL || szDestFileName==NULL)
		return FALSE;

	HANDLE hRead = CreateFile(szSourceFileName,GENERIC_READ,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if ( hRead==INVALID_HANDLE_VALUE )
	{
		return FALSE;
	}//endif
	
	unsigned long   dwFileSizeHigh  = 0;
	unsigned long   dwFileSizeLow   = GetFileSize(hRead,&dwFileSizeHigh);
	
//	_//ASSERT(dwFileSizeHigh == 0);
	
    char * pTempBuffer   = new char [dwFileSizeLow];
	unsigned long   dwReaded = 0;
	
	BOOL hRes=ReadFile(hRead,pTempBuffer,dwFileSizeLow,&dwReaded,NULL);
	CloseHandle(hRead);
//	//ASSERT(hRes && dwFileSizeLow==dwReaded);
	if (!hRes || dwFileSizeLow!=dwReaded)
	{
		delete [] pTempBuffer;
		return FALSE;
	}//endif

	
	Encrypt(pTempBuffer,pTempBuffer,dwReaded);
	
	HANDLE  hWrite=CreateFile(szDestFileName,GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	//ASSERT(hWrite);
	if (hWrite==INVALID_HANDLE_VALUE)
	{
		delete [] pTempBuffer;
		return FALSE;
	}//Write
	
	hRes=WriteFile(hWrite,pTempBuffer,dwFileSizeLow,&dwReaded,NULL);
	//ASSERT(hRes && dwReaded==dwFileSizeLow);
	CloseHandle(hWrite);
	if (!hRes || dwReaded!=dwFileSizeLow)
	{
		delete [] pTempBuffer;
		return FALSE;
	}//endif
	
	delete [] pTempBuffer;
	
	return TRUE;
}

BOOL FaithXOREncrypter::Unencrypt(const char * szSourceFileName, const char * szDestFileName)
{
	return Encrypt(szSourceFileName,szDestFileName);
}