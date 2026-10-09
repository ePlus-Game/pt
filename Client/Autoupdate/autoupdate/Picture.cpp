// Picture.cpp: implementation of the CPicture class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "autoupdate.h"
#include "Picture.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPicture::CPicture()
{

}

CPicture::~CPicture()
{

}

//-----------------------------------------------------------------------------
// Does:   Free The Allocated Memory That Holdes The IPicture Interface Data
// ~~~~    And Clear Picture Information
//
// Note:   This Might Also Be Useful If U Only Need To Show The Picture Once
// ~~~~~   Or If U Copy The Picture To The Device Context, So It Can Still
//         Remain On Screen - But IPicture Data Is Not Needed No More
//
//-----------------------------------------------------------------------------
void CPicture::UnloadPicture()
//=============================================================================
{
	if(m_pPict != NULL){
		m_pPict->Release();
		m_pPict = NULL;
	}
}
//-----------------------------------------------------------------------------
// Does:   Open a Resource And Load It Into IPicture (Interface)
// ~~~~    (.BMP .DIB .EMF .GIF .ICO .JPG .WMF)
//
// Note:   When Adding a Bitmap Resource It Would Automatically Show On "Bitmap"
// ~~~~    This NOT Good Coz We Need To Load It From a Custom Resource "BMP"
//         To Add a Custom Rresource: Import Resource -> Open As -> Custom
//         (Both .BMP And .DIB Should Be Found Under "BMP")
//
// InPut:  ResourceName - As a UINT Defined (Example: IDR_PICTURE_RESOURCE)
// ~~~~~   ResourceType - Type Name (Example: "JPG")
//
// OutPut: TRUE If Succeeded...
// ~~~~~~
//-----------------------------------------------------------------------------
BOOL CPicture::Load(HINSTANCE hInstance,LPCTSTR lpszResourceName, LPCSTR ResourceType)
//=============================================================================
{
	HGLOBAL  hGlobal = NULL;
	HRSRC  hSource = NULL;
	LPVOID  lpVoid  = NULL;
	int   nSize   = 0;
	BOOL  bResult=FALSE;
	if(m_pPict != NULL) UnloadPicture(); // Important - Avoid Leaks...
	
	hSource = FindResource(hInstance, lpszResourceName, ResourceType);
	
	if(hSource == NULL)
	{
		HWND hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
		//MessageBoxEx(hWnd, "FindResource() Failed\t", ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		return FALSE;
	}
	hGlobal = LoadResource(hInstance, hSource);
	if(hGlobal == NULL)
	{
		HWND hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
		//MessageBoxEx(hWnd, "LoadResource() Failed\t", ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		return(FALSE);
	}
	lpVoid = LockResource(hGlobal);
	if(lpVoid == NULL)
	{
		HWND hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
		//MessageBoxEx(hWnd, "LockResource() Failed\t", ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		return(FALSE);
	}
	
	nSize = (UINT)SizeofResource(hInstance, hSource);
	if(LoadPictureData((BYTE*)hGlobal, nSize)) bResult = TRUE;
	
	
	
	UnlockResource(hGlobal); // 16Bit Windows Needs This
	FreeResource(hGlobal); // 16Bit Windows Needs This (32Bit - Automatic Release)
	return(bResult);
}



//-----------------------------------------------------------------------------
// Does:   Open a File And Load It Into IPicture (Interface)
// ~~~~    (.BMP .DIB .EMF .GIF .ICO .JPG .WMF)
//
// InPut:  sFilePathName - Path And FileName Target To Save
// ~~~~~   
//
// OutPut: TRUE If Succeeded...
// ~~~~~~
//-----------------------------------------------------------------------------
BOOL CPicture::Load(CString sFilePathName)
//=============================================================================
{
	//if(!PathFileExists(sFilePathName))return FALSE;
	BOOL bResult = FALSE;
	CFile PictureFile;
	CFileException e;
	int nSize = 0;
	
	if(m_pPict != NULL)
		UnloadPicture(); // Important - Avoid Leaks...
	
	if(PictureFile.Open(sFilePathName, CFile::modeRead | CFile::typeBinary, &e))
	{
		nSize = PictureFile.GetLength();
		BYTE* pBuffer = new BYTE[nSize];
		
		if(PictureFile.Read(pBuffer, nSize) > 0)
		{
			if(LoadPictureData(pBuffer, nSize)) bResult = TRUE;
		}
		PictureFile.Close();
		delete [] pBuffer;
	}
	else // Open Failed...
	{
//		TCHAR szCause[255];
//		e.GetErrorMessage(szCause, 255, NULL);
//		HWND hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
		//MessageBoxEx(hWnd, szCause, ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		bResult = FALSE;
	}
	return(bResult);
}



//-----------------------------------------------------------------------------
// Does:   Read The Picture Data From a Source (File / Resource)
// ~~~~    And Load It Into The Current IPicture Object In Use
//
// InPut:  Buffer Of Data Source (File / Resource) And Its Size
// ~~~~~   
//
// OutPut: Feed The IPicture Object With The Picture Data
// ~~~~~~  (Use Draw Functions To Show It On a Device Context)
//         TRUE If Succeeded...
//-----------------------------------------------------------------------------
BOOL CPicture::LoadPictureData(BYTE *pBuffer, int nSize)
//=============================================================================
{
	BOOL bResult = FALSE;
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, nSize);
	
	if(hGlobal == NULL)
	{
		HWND hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
		//MessageBoxEx(hWnd, "Can not allocate enough memory\t", ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		return(FALSE);
	}
	
	void* pData = GlobalLock(hGlobal);
	memcpy(pData, pBuffer, nSize);
	GlobalUnlock(hGlobal);
	
	IStream* pStream = NULL;
	if(CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) == S_OK)
	{
		HRESULT hr;
		if((hr = OleLoadPicture(pStream, nSize, FALSE, IID_IPicture, (LPVOID *)&m_pPict)) == E_NOINTERFACE)
		{
			HWND hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
			//MessageBoxEx(hWnd, "IPicture interface is not supported\t", ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
			return(FALSE);
		}
		else // S_OK
		{
			pStream->Release();
			pStream = NULL;
			bResult = TRUE;
			
		}
	}
	FreeResource(hGlobal); // 16Bit Windows Needs This (32Bit - Automatic Release)
	return(bResult);
}



//-----------------------------------------------------------------------------
// Does:   Draw The Loaded Picture Direct To The Client DC
// ~~~~
//
// Note:   Bigger OR Smaller Dimentions Than The Original Picture Size
// ~~~~    Will Draw The Picture Streached To Its New Given NEW Dimentions...
//
// InPut:  pDC - Given DC To Draw On
// ~~~~~   pSrcRect- Dimentions Of The Picture To Draw From(As a Rectangle)
//   DrawRect - Dimentions Of The Picture To Draw To(As a Rectangle)
// OutPut: TRUE If Succeeded...
// ~~~~~~
//-----------------------------------------------------------------------------
//=============================================================================



void CPicture::Render(CDC* pDC, LPRECT pDrawRect, LPRECT pSrcRect/*=NULL*/,LPCRECT prcWBounds/*=NULL*/)
{
    if (pDC == NULL || m_pPict == NULL) return ;
    
	CRect recrDest(pDrawRect);
	
    long Width  = 0;
    long Height = 0;
    m_pPict->get_Width(&Width);
    m_pPict->get_Height(&Height);
	
	CRect SrcRect(0,0,Width,Height);
	if(pSrcRect){
		SrcRect=*pSrcRect;
	}
	CRect DrawRect(pDrawRect);
    HRESULT hrP = NULL;
	
    hrP = m_pPict->Render(pDC->m_hDC,
		DrawRect.left,                  // Left
		DrawRect.top,                   // Top
		DrawRect.Width(), // Right
		DrawRect.Height(), // Bottom
		SrcRect.left,
		SrcRect.Height(),
		SrcRect.Width(),
		-SrcRect.Height(),
		prcWBounds);
	
    if (SUCCEEDED(hrP)) return;
	AfxThrowMemoryException();
    return;
}//-----------------------------------------------------------------------------
// Does:   Saves The Picture That Is Stored In The IPicture Object As a Bitmap
// ~~~~    (Converts From Any Known Picture Type To a Bitmap / Icon File)
//
// InPut:  sFilePathName - Path And FileName Target To Save
// ~~~~~
//
// OutPut: TRUE If Succeeded...
// ~~~~~~
//-----------------------------------------------------------------------------
BOOL CPicture::SaveAsBitmap(CString sFilePathName)
//=============================================================================
{
	BOOL bResult = FALSE;
	ILockBytes *Buffer = 0;
	IStorage   *pStorage = 0;
	IStream    *FileStream = 0;
	BYTE    *BufferBytes;
	STATSTG  BytesStatistics;
	DWORD  OutData;
	long  OutStream;
	CFile  BitmapFile; CFileException e;
	double  SkipFloat = 0;
	DWORD  ByteSkip = 0;
	_ULARGE_INTEGER RealData;
	
	CreateILockBytesOnHGlobal(NULL, TRUE, &Buffer); // Create ILockBytes Buffer
	HRESULT hr = ::StgCreateDocfileOnILockBytes(Buffer,
		STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, &pStorage);
	hr = pStorage->CreateStream(L"PICTURE",
		STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, 0, &FileStream);
	
	m_pPict->SaveAsFile(FileStream, TRUE, &OutStream); // Copy Data Stream
	FileStream->Release();
	pStorage->Release();
	Buffer->Flush(); 

	// Get Statistics For Final Size Of Byte Array
	Buffer->Stat(&BytesStatistics, STATFLAG_NONAME);
	// Cut UnNeeded Data Coming From SaveAsFile() (Leave Only "Pure" Picture Data)
	SkipFloat = (double(OutStream) / 512); // Must Be In a 512 Blocks...
	if(SkipFloat > DWORD(SkipFloat)) ByteSkip = (DWORD)SkipFloat + 1;
	else ByteSkip = (DWORD)SkipFloat;
	ByteSkip = ByteSkip * 512; // Must Be In a 512 Blocks...
	
	// Find Difference Between The Two Values
	ByteSkip = (DWORD)(BytesStatistics.cbSize.QuadPart - ByteSkip);
	// Allocate Only The "Pure" Picture Data
	RealData.LowPart = 0;
	RealData.HighPart = 0;
	RealData.QuadPart = ByteSkip;
	BufferBytes = (BYTE*)malloc(OutStream);
	if(BufferBytes == NULL)
	{
		Buffer->Release();
		HWND hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
		//MessageBoxEx(hWnd, "Can not allocate enough memory\t", ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
	}
	Buffer->ReadAt(RealData, BufferBytes, OutStream, &OutData);
	if(BitmapFile.Open(sFilePathName, CFile::typeBinary | CFile::modeCreate | CFile::modeWrite, &e))
	{
		BitmapFile.Write(BufferBytes, OutData);
		BitmapFile.Close();
		bResult = TRUE;
	}
	else // Write File Failed...
	{
		TCHAR szCause[255];
		e.GetErrorMessage(szCause, 255, NULL);
		HWND hWnd = AfxGetApp()->GetMainWnd()->m_hWnd;
		//MessageBoxEx(hWnd, szCause, ERROR_TITLE, MB_OK | MB_ICONSTOP, LANG_ENGLISH);
		bResult = FALSE;
	}
	Buffer->Release();
	free(BufferBytes);
	return(bResult);
}

LONG CPicture::get_Height()
{
	LONG     nHeight = 0;
	if (m_pPict != NULL)
	{
		m_pPict->get_Height(&nHeight);
	}
	return nHeight;
}

LONG CPicture::get_Width()
{
	LONG     nWidth = 0;
	if (m_pPict != NULL)
	{
		m_pPict->get_Width(&nWidth);
	}
	return nWidth;
}

