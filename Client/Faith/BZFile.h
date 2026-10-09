// BZFile.h: interface for the CBZFile class.
// Version: 1.0
// by Cooler 2003-11-11 liuyujun@263.net
// Crossing platform (Win32/Linux)
//////////////////////////////////////////////////////////////////////

#ifndef _BZFILE_H_
#define _BZFILE_H_

class CBZFile  
{
protected:
	FILE	*m_pFOpen;

public:
	// File flag
	enum OpenFlags {
		modeCreate =        0x0001,
		modeRead =          0x0010,
		modeWrite =         0x0020,
		modeReadWrite =     0x0040,
		modeAppend =		0x0080
		};

	CBZFile();
	virtual ~CBZFile();

	BOOL	Open(LPCSTR lpszFileName, DWORD dwOpenFlags);
	void	Close();

	long	GetLength();

	int		SeekToBegin();
	int		SeekToEnd();
	int		SeekTo(int nOffset);

	int		Read(LPSTR pBuf, DWORD dwBufSize);
	int		Write(LPCSTR pcBuf, DWORD dwBufSize);
};

#endif // _BZFILE_H_
