#ifndef _CHAT_FACE_DIALOG_H
#define _CHAT_FACE_DIALOG_H
#define STATIC_ID_MIN   100
#define STATIC_ID_MAX   156

//#define _CHAT_FACE_DEFAULT_STRING   "<Layout width = 100><Seg text-align=left></Seg></Layout>"
typedef class FaceDialog
{
public:
	FaceDialog();
	~FaceDialog();
	static BOOL CALLBACK FaceDialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	void   FaceDialogAdjustWindow(HWND hwnd);
	void   FaceDialogInit(HWND hDlg);
	void   FaceDialogDrawBk(HWND hDlg);
	void   FaceDialogShow(BOOL bShow);
	void   FaceDialogOwnerDraw(LPDRAWITEMSTRUCT lpdis);
	BOOL   FaceDialogIsShow() const {return isShow;}
	void   FaceDialogClickFace();
	HWND   FaceDialogGetHandle() const {return hShowFace;}
	
	HWND hFaceDlg;

protected:
	RECT wndRect;
	int offsetX ;
	int offsetY ;
	ILayout* pFaceLayOut;
	HRGN   hRgn;
	int bkSrcIdx;
	BOOL    isShow;
	HWND    hShowFace;
	int     numberFacePerLine;
	int     nWidth;
	int     nHeight;
	int     startFaceX;
	int     startFaceY;

}FACEDIALOG,*PFACEDIALOG;
#endif
