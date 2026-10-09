// GameOptionPanel.cpp : implementation file
//

#include "stdafx.h"
#include "autoupdate.h"
#include "GameOptionPanel.h"
#define NO_DIRECT_X
#include "KWin32.h"
#include "KIniFile.h"
#include "KFilePath.h"
#include "DlgGetPath.h"	
#include "ShareUIInfo.h"
#include "windows.h"

//#pragma COMPILE_MSG("bad include file")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// GameOptionPanel dialog
const string FullScreenSectorName = "FullScreen";
const string RepresentSectorName  = "Represent";
const string DynaLightSectorName  = "DynamicLight";

const string FullScreenMode    = FullScreenSectorName + "=1";
const string WindowScreenMode  = FullScreenSectorName + "=0";

const string Represent2DMode   = RepresentSectorName + "=2";
const string Represent3DMode   = RepresentSectorName + "=3";

const string DynaLightMode     = DynaLightSectorName + "=1";
const string NotDynaLightMode  = DynaLightSectorName + "=0";

const string ConfileFileName   =  "config.ini";

const string Represent3DModuleFileName ="Represent3.dll";

const string MessageTitle = "JxoinlineOption";

//*********************************************************************
// macro : 计算某个CWnd成员变量在CUpdateDialog类对象中的偏移
//*********************************************************************
#define OFFSETOF_UPDATEDLG_CWND_MEMBER(Member)		\
OFFSETOF_MEMBER(GameOptionPanel, CWnd, Member)
//*********************************************************************
// macro : 计算某个CHyperlinkStatic成员变量在CUpdateDialog类对象中的偏移
//*********************************************************************
#define OFFSETOF_UPDATEDLG_URL_MEMBER(Member)		\
OFFSETOF_MEMBER(GameOptionPanel, CHyperlinkStatic, Member)

#define OFFSETOF_UPDATEDLG_BMPBTN_MEMBER(Member)	\
OFFSETOF_MEMBER(GameOptionPanel, CBmpButton, Member)

#define OFFSETOF_UPDATEDLG_URLBMPBTN_MEMBER(Member)		\
OFFSETOF_MEMBER(GameOptionPanel, CURLBmpButton, Member)

#define OFFSETOF_UPDATEDLG_TRANSPARENTSTATIC_MEMBER(Member) \
OFFSETOF_MEMBER(GameOptionPanel, CTransparentStatic, Member)

//位图按钮的属性
/*BMPButton GameOptionPanel::m_bmpBtns[COUNT_BMPBUTTON + 1] = 
{
	//关闭按钮
	OFFSETOF_UPDATEDLG_BMPBTN_MEMBER(m_buttonMiniClose), TRUE, UI_TD_MINICLOSE_POS,  UI_TD_MINICLOSE_SIZE, 
	RT_BITMAP, enumDRAW_USE_COLORKEY,	IDB_BITMAP_MINCLOSE_UP, IDB_BITMAP_MINCLOSE_DOWN, IDB_BITMAP_MINCLOSE_OVER, IDB_BITMAP_MINCLOSE_OVER,
	UI_TD_MINICLOSE_CK,

	-1, {0},
};*/

WindowRect GameOptionPanel::m_rectStaticCtl[COUNT_CONTROL + 1] =
{
//	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_staticTitle), TRUE, UI_TD_CAPTION,		//窗口标题	

//	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_FullScreenCtl), TRUE, UI_TD_FUL_CHECK_RECT,		//全屏选择
//	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_WindowOptionCtl), TRUE, UI_TD_WIN_CHECK_RECT,		//窗口选择
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_staticWarnigInfo), TRUE, UI_TD_PICTURE_TIP,		//警告信息1
//	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_staticWarningInfo2), FALSE, {140, 110, 140 + 200, 110 + 19},		//警告信息2

//	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_staticPicPath), TRUE, UI_TD_PICTRUE_PATH,		//路径提示
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_editPath), TRUE, UI_TD_PICTRUE_EDIT,		//路径edit
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_buttonOpen), TRUE, {119 + 200, 180, 119 +200 + 22, 180 + 22},		//选择路径按钮

	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_buttonDefault), TRUE, UI_TD_BTN_DEFAULT_RC,		//默认按钮
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_buttonOk), TRUE, UI_TD_BTN_OK,			//确定按钮	
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_buttonCancel), TRUE, UI_TD_CANCEL,		//取消按钮	

	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_SoundBar), TRUE,UI_TD_MUSIC_RC,			//音乐滑条	
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_MusicBar), TRUE, UI_TD_SOUND_RC ,		//声音滑条	

	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_contrFullS), TRUE, UI_TD_FULLS,
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_contrWindows), TRUE, UI_TD_WINDOWS,
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_distinguish1), TRUE, UI_TD_800,
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_distinguish2), TRUE, UI_TD_1024,

//	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_Screen800), TRUE, {60, 60, 60 + 90, 60 + 25},		//全屏选择
//	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_Screen1024), TRUE, {230, 60, 230 + 90, 60 + 25},		//窗口选择
   -1,
	
};

GameOptionPanel::GameOptionPanel(CWnd* pParent /*=NULL*/)
	: CBitmapDialog(GameOptionPanel::IDD, pParent)
{
	//{{AFX_DATA_INIT(GameOptionPanel)
	m_2DOptionValue = -1;
	m_FullScreenValue = -1;
	m_DynaLightEnableValue = FALSE;
	m_txtCapPath = _TT("");
	//}}AFX_DATA_INIT
}


void GameOptionPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(GameOptionPanel)
//	DDX_Control(pDX, IDC_SCREEN, m_ScreenSel);
	DDX_Control(pDX, IDC_TXT_CAPPATH, m_editPath);
//	DDX_Control(pDX, IDC_STATIC_PICPATH, m_staticPicPath);
//	DDX_Control(pDX, IDC_STATIC_TITLE, m_staticTitle);
//	DDX_Control(pDX, IDC_STATIC_WARNINGINFO2, m_staticWarningInfo2);
	DDX_Control(pDX, IDC_STATIC_WARNINGINFO1, m_staticWarnigInfo);
//	DDX_Control(pDX, IDC_BUTTON_MINICLOSE, m_buttonMiniClose);
	DDX_Control(pDX, IDC_BTN_CAPPATH, m_buttonOpen);
	DDX_Control(pDX, IDC_OK, m_buttonOk);
	DDX_Control(pDX, IDC_CANCEL, m_buttonCancel);
	DDX_Control(pDX, IDC_DEFAULT, m_buttonDefault);
//	DDX_Control(pDX, IDC_WindowOption, m_WindowOptionCtl);
	DDX_Control(pDX, IDC_3DOption, m_3DOptionCtl);
//	DDX_Control(pDX, IDC_FullScreen, m_FullScreenCtl);
	DDX_Control(pDX, IDC_2DOption, m_2DOptionCtl);
	DDX_Control(pDX, IDC_DynaLightEnable, m_DynaLightEnableCtl);
	DDX_Control(pDX, IDC_STATIC_SOUND, m_SoundBar);
	DDX_Control(pDX, IDC_STATIC_MUSIC, m_MusicBar);
//	DDX_Control(pDX, IDC_COMBO_IMAGE,m_ImageSel);
	DDX_Check(pDX, IDC_2DOption, m_2DOptionValue);
//	DDX_Check(pDX, IDC_FullScreen, m_FullScreenValue);
	DDX_Check(pDX, IDC_DynaLightEnable, m_DynaLightEnableValue);
	DDX_Text(pDX, IDC_TXT_CAPPATH, m_txtCapPath);
	DDX_Control(pDX, IDC_WindowOption, m_contrWindows);
	DDX_Control(pDX, IDC_FullScreen, m_contrFullS);
	DDX_Control(pDX, IDC_800, m_distinguish1);
	DDX_Control(pDX, IDC_1024, m_distinguish2);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(GameOptionPanel, CBitmapDialog)
	//{{AFX_MSG_MAP(GameOptionPanel)
	ON_BN_CLICKED(IDC_OK, OnOk)
	ON_BN_CLICKED(IDC_3DOption, On3DOptionSelected)
	ON_BN_CLICKED(IDC_2DOption, On2DOption)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_BN_CLICKED(IDC_BTN_CAPPATH, OnBtnCappath)
//	ON_BN_CLICKED(IDC_BUTTON_MINICLOSE, OnButtonMiniclose)
	ON_BN_CLICKED(IDC_FullScreen, OnFullScreen)
	ON_BN_CLICKED(IDC_WindowOption, OnWindowOption)
	ON_BN_CLICKED(IDC_800, OnPixellow)
	ON_BN_CLICKED(IDC_1024, OnPixelhigh)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GameOptionPanel message handlers

void GameOptionPanel::OnOk() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

//	try
	{
		//写入config.ini文件
		char aConfigPath[MAX_PATH] ={0};
		g_GetRootPath(aConfigPath);
		strcat(aConfigPath, "\\");
		strcat(aConfigPath, ConfileFileName.c_str());
		char aCapPath[MAX_PATH] = {0};
		if (m_txtCapPath == "")
		{
			g_GetRootPath(aCapPath);
			strcat(aCapPath,"\\Snapshot");			
		}
		else
			strcpy(aCapPath, m_txtCapPath.GetBuffer(1));
		WritePrivateProfileString("Client","CapPath",aCapPath, aConfigPath);

		/*
		if (m_2DOptionCtl.GetCheck())
		{
			WritePrivateProfileString("Client","Represent","2", aConfigPath);
			WritePrivateProfileString("Client","DynamicLight","0", aConfigPath); 
		}
		else
		{
			WritePrivateProfileString("Client","Represent","3", aConfigPath);
			if (m_DynaLightEnableCtl.GetCheck())
				WritePrivateProfileString("Client","DynamicLight","1", aConfigPath);
			else
				WritePrivateProfileString("Client","DynamicLight","0", aConfigPath);
		}
		*/
		
/*		if (m_FullScreenCtl.GetCheck())
		{
			WritePrivateProfileString("GameSetting","FullOrWin","1", aConfigPath);
		}
		else
		{
			WritePrivateProfileString("GameSetting","FullOrWin","0", aConfigPath);
		}*/
		if (m_FullScreenValue)
		{
			WritePrivateProfileString("GameSetting","FullOrWin","1", aConfigPath);
		}
		else
		{
			WritePrivateProfileString("GameSetting","FullOrWin","0", aConfigPath);
		}

		if (m_distinguish)
		{
			WritePrivateProfileString("GameSetting","ScreenWidth","1024", aConfigPath);
			WritePrivateProfileString("GameSetting","ScreenHeight","768", aConfigPath);
		}
		else
		{
			WritePrivateProfileString("GameSetting","ScreenWidth","800", aConfigPath);
			WritePrivateProfileString("GameSetting","ScreenHeight","600", aConfigPath);
		}

/*		if ( m_ScreenSel.GetCurSel() == 1 )
		{
			WritePrivateProfileString("GameSetting","ScreenWidth","1024", aConfigPath);
			WritePrivateProfileString("GameSetting","ScreenHeight","768", aConfigPath);
		}
		if ( m_ScreenSel.GetCurSel() == 0 )
		{
			WritePrivateProfileString("GameSetting","ScreenWidth","800", aConfigPath);
			WritePrivateProfileString("GameSetting","ScreenHeight","600", aConfigPath);
		}*/

		int iMusic=m_MusicBar.GetPos();
		int iSound=m_SoundBar.GetPos();

		char buffer[64];
		sprintf(buffer,"%d",iMusic);
		WritePrivateProfileString("GameSetting","MusicSet", buffer, aConfigPath);

		sprintf(buffer,"%d",iSound);
		WritePrivateProfileString("GameSetting","VoiceSet", buffer, aConfigPath);

 /*       int iQualityIndex = m_ImageSel.GetCurSel();
		if (iQualityIndex==0)
        {
			sprintf(buffer,"%d",100);
        }
		else if (iQualityIndex==1)
		{
			sprintf(buffer,"%d",50);
		}
		else if (iQualityIndex==2)
		{
			sprintf(buffer,"%d",0);
		}

		WritePrivateProfileString("GameSetting","CatonQulity", buffer, aConfigPath );
*/
		CDialog::OnOK();
	
	}
//	catch(exception Error)
//	{
//		MessageBox(Error.what(),MessageTitle.c_str(),MB_ICONERROR);
//
//	}
	

}

void GameOptionPanel::OnCancel() 
{
	// TODO: Add your control notification handler code here
	char aConfigPath[MAX_PATH] ={0};
	g_GetRootPath(aConfigPath);
	strcat(aConfigPath, "\\");
	strcat(aConfigPath, ConfileFileName.c_str());
	char aCapPath[MAX_PATH] = {0};
	GetPrivateProfileString("Client","CapPath","",aCapPath, MAX_PATH, aConfigPath);
	if (aCapPath[0] == '\0')
	{
		g_GetRootPath(aCapPath);
		strcat(aCapPath,"\\Snapshot");
		WritePrivateProfileString("Client","CapPath",aCapPath, aConfigPath);		
	}
	
	CDialog::OnCancel();
}

void GameOptionPanel::On3DOptionSelected() 
{
	// TODO: Add your control notification handler code here
	m_DynaLightEnableCtl.EnableWindow();

}

void GameOptionPanel::On2DOption() 
{
	// TODO: Add your control notification handler code here
	m_DynaLightEnableCtl.EnableWindow(FALSE);
	m_DynaLightEnableCtl.SetCheck(0);
	

}

BOOL GameOptionPanel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
    
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	if(m_hIcon != NULL)
	{
		SetIcon(m_hIcon, TRUE);
	}
	else
	{
		DisplayErrorInfo(string(U_LOADICON_ERR) );
	}

	//读取config.ini文件
	char aConfigPath[MAX_PATH] ={0};
	g_GetRootPath(aConfigPath);
	strcat(aConfigPath, "\\");
	strcat(aConfigPath, ConfileFileName.c_str());
	char aCapPath[MAX_PATH] = {0};
	GetPrivateProfileString("Client","CapPath","",aCapPath, MAX_PATH, aConfigPath);
	if (aCapPath[0] == '\0')
	{
		g_GetRootPath(aCapPath);
		strcat(aCapPath,"\\Screenshots");
	}

	m_txtCapPath = aCapPath;
	UpdateData(false);
	/*
	int nRepresentValue = GetPrivateProfileInt("Client","Represent",2, aConfigPath);
	if( nRepresentValue == 2 )
	{
		m_2DOptionCtl.SetCheck(1);
		m_3DOptionCtl.SetCheck(0);
		m_DynaLightEnableCtl.SetCheck(0);
		m_DynaLightEnableCtl.EnableWindow(false);
	}
	else
	{
		m_2DOptionCtl.SetCheck(0);
		m_3DOptionCtl.SetCheck(1);
		m_DynaLightEnableCtl.EnableWindow();
		int nDynamicLight = GetPrivateProfileInt("Client","DynamicLight", 1, aConfigPath);
		if (nDynamicLight == 0)
			m_DynaLightEnableCtl.SetCheck(0);
		else
			m_DynaLightEnableCtl.SetCheck(1);
	}
	*/
	
	int nFullScreenValue = GetPrivateProfileInt("GameSetting","FullOrWin", 1, aConfigPath);
	if (nFullScreenValue == 0)
	{
		m_contrFullS.SetCheck(0);
		m_contrWindows.SetCheck(1);
	}
	else
	{
		m_contrFullS.SetCheck(1);
		m_contrWindows.SetCheck(0);
	}
	m_FullScreenValue = nFullScreenValue;

	//begin-------------------------------------------
	int nScreenWidth  = 0;
	int nScreenHeight = 0;
	nScreenWidth  = GetPrivateProfileInt("GameSetting","ScreenWidth", 800, aConfigPath );
	nScreenHeight = GetPrivateProfileInt("GameSetting","ScreenHeight", 600, aConfigPath );

	if ( nScreenHeight == 600 && nScreenWidth == 800 )
	{
		m_distinguish1.SetCheck(1);
		m_distinguish2.SetCheck(0);
		m_distinguish = 0;
	}
	else if ( nScreenHeight == 768 && nScreenWidth == 1024 )
	{
		m_distinguish1.SetCheck(0);
		m_distinguish2.SetCheck(1);
		m_distinguish = 1;
	}

	int iMusic= GetPrivateProfileInt("GameSetting","MusicSet", 100, aConfigPath );
	int iSound= GetPrivateProfileInt("GameSetting","VoiceSet", 100, aConfigPath );
    
	if (iMusic>100)
		iMusic=100;

	if (iSound>100)
		iSound=100;

	m_MusicBar.SetRange(0, 100);
	m_MusicBar.SetPos(iMusic);

	m_SoundBar.SetRange(0, 100);
	m_SoundBar.SetPos(iSound);


/*	int iQuality = GetPrivateProfileInt("GameSetting","CatonQulity", 100, aConfigPath );
	if (iQuality>50)
	{
        m_ImageSel.SetCurSel(0);
	}//endif
    else
    if (iQuality==50)
	{
		m_ImageSel.SetCurSel(1);
	}
	else if (iQuality<50)
	{
		m_ImageSel.SetCurSel(2);
	}
*/
 
	//end---------------------------------------------

	InitUI();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void GameOptionPanel::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	DeleteObject(m_hIcon);
}

void GameOptionPanel::OnDefault() 
{
/*
	try
	{
		HMODULE Represent3DModule  = LoadLibrary(Represent3DModuleFileName.c_str());
		
		
		if(Represent3DModule == NULL)
		{
			string ErrorInfo = string(U_LOAD_DLL) +Represent3DModuleFileName + string(U_TAIL_ERROR);
			throw exception (ErrorInfo.c_str());
		}
		
		fnRepresentIsModuleRecommended Check_Represent; 
		Check_Represent =(fnRepresentIsModuleRecommended) GetProcAddress(Represent3DModule,Check_Represent_FunctionName.c_str());
		
		if(Check_Represent == NULL)
		{
			FreeLibrary(Represent3DModule);
			string ErrorInfo = string(U_GET_FUN_ADDRESS) +Check_Represent_FunctionName + string(U_TAIL_ERROR);
			throw exception (ErrorInfo.c_str());
		}
*/		
		char aCapPath[MAX_PATH] = {0};
		g_GetRootPath(aCapPath);
		strcat(aCapPath,"\\Snapshot");
		m_txtCapPath = aCapPath;
		UpdateData(false);

/*		if((Check_Represent)())
		{
			m_2DOptionCtl.SetCheck(0);
			m_3DOptionCtl.SetCheck(1);
			m_DynaLightEnableCtl.SetCheck(1);
			m_DynaLightEnableCtl.EnableWindow();
			
			
		}
		else
		{
			m_2DOptionCtl.SetCheck(1);
			m_3DOptionCtl.SetCheck(0);
			m_DynaLightEnableCtl.SetCheck(0);
			m_DynaLightEnableCtl.EnableWindow(false);
		}
*/
		m_contrFullS.SetCheck(0);
		m_contrWindows.SetCheck(1);
		m_FullScreenValue = 0;

//		m_ScreenSel.SetCurSel(0);
        m_SoundBar.SetPos(100);
		m_MusicBar.SetPos(100);
//		m_ImageSel.SetCurSel(0);
		m_distinguish1.SetCheck(1);
		m_distinguish = 0;
		m_distinguish2.SetBitmaps(IDB_BITMAP_PITCH_OFF, RGB(0, 0, 0));
		//	m_distinguish2.DrawTransparent();
		m_distinguish1.SetBitmaps(IDB_BITMAP_PITCH_ON, RGB(0, 0, 0));
//		m_distinguish1.DrawTransparent();
		m_FullScreenValue = 0;
		m_contrWindows.SetCheck(1);
		m_contrFullS.SetCheck(0);
		m_contrFullS.SetBitmaps(IDB_BITMAP_PITCH_OFF, RGB(0, 0, 0));
		//	m_contrFullS.DrawTransparent();
		m_contrWindows.SetBitmaps(IDB_BITMAP_PITCH_ON, RGB(0, 0, 0));
//		m_contrWindows.DrawTransparent();

		
/*		if(!FreeLibrary(Represent3DModule))
		{
			string ErrorInfo = string(U_RELEASE_MODULE) +Represent3DModuleFileName + string(U_TAIL_ERROR);
			throw exception (ErrorInfo.c_str());	
		}
		
	}
	catch(...)
	{
	}
*/
//	catch(exception& Error)
//	{
//		DisplayErrorInfo(string(Error.what()));
//				
//		CDialog::OnCancel();
//
//	}
}

void GameOptionPanel::OnBtnCappath() 
{
	UpdateData();
	LPITEMIDLIST pidlRoot=NULL; 
	SHGetSpecialFolderLocation(m_hWnd,CSIDL_DESKTOP,&pidlRoot); 

	BROWSEINFO bi;   //必须传入的参数,下面就是这个结构的参数的初始化 
	//CString strDisplayName;   //用来得到,你选择的活页夹路径,相当于提供一个缓冲区 
	bi.hwndOwner=GetSafeHwnd();   //得到父窗口Handle值 
	bi.pidlRoot=pidlRoot;   //这个变量就是我们在上面得到的. 
	bi.pszDisplayName=NULL;//strDisplayName.GetBuffer(MAX_PATH+1);   //得到缓冲区指针, 
	bi.lpszTitle = U_SELECT_FOLDER;   //设置标题 
	bi.ulFlags=0;   //设置标志 
	bi.lpfn=NULL; 
	bi.lParam=0; 
	bi.iImage=0;   //上面这个是一些无关的参数的设置,最好设置起来, 


	//Lucifer~yu Del for Modify 
	//begin--------------------------------------------------------------
/*	LPITEMIDLIST pIIL =SHBrowseForFolder(&bi);   //打开对话框 
	
	if(pIIL == NULL)return;
	//strDisplayName.ReleaseBuffer();   //和上面的GetBuffer()相对应 

	TCHAR szInitialDir[MAX_PATH];
	BOOL bRet = ::SHGetPathFromIDList(pIIL, (char*)&szInitialDir);
	m_txtCapPath = szInitialDir;
	UpdateData(false);//*/
	//end----------------------------------------------------------------

	//Lucifer~yu 2005-7-25 add
	//begin--------------------------
	CDlgGetPath dlg;
	dlg.SetPath("C:\\");
	if( dlg.DoModal() == IDOK )
	{
		m_txtCapPath = dlg.GetPath( );	
		UpdateData(false);
	}
	//end----------------------------


}

void GameOptionPanel::OnPostEraseBkgnd(CDC* pDC)
{
	m_buttonDefault.SetBk(pDC);
	m_buttonOk.SetBk(pDC);
	m_buttonCancel.SetBk(pDC);
	m_buttonOpen.SetBk(pDC);
//	m_WindowOptionCtl.SetBk(pDC);
//	m_FullScreenCtl.SetBk(pDC);
}

#define SETBUTTONCOLOR(btn, value)\
	btn.SetColor(CButtonST::BTNST_COLOR_FG_OUT, value);\
	btn.SetColor(CButtonST::BTNST_COLOR_FG_IN, value);\
	btn.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, value);

void GameOptionPanel::InitUI()
{
	m_MusicBar.SetBitmapChannel(IDB_BITMAP_SETTING_BAR, IDB_BITMAP_SETTING_BAR, TRUE,0x00FF00FF);
 	m_MusicBar.SetBitmapThumb(IDB_SETTING_BLOCK, IDB_SETTING_BLOCK, TRUE, RGB(0, 0, 0));
 	m_MusicBar.DrawFocusRect( FALSE );
	m_MusicBar.SetPageSize(2);
	m_MusicBar.EableUIEvent(TRUE);
 
	m_SoundBar.SetBitmapChannel(IDB_BITMAP_SETTING_BAR, IDB_BITMAP_SETTING_BAR, TRUE,0x00FF00FF);
    m_SoundBar.SetBitmapThumb(IDB_SETTING_BLOCK, IDB_SETTING_BLOCK, TRUE, RGB(0, 0, 0));
	m_SoundBar.DrawFocusRect( FALSE );
	m_SoundBar.SetPageSize(2);
	m_SoundBar.EableUIEvent(TRUE);

	CWndTool theWndTool(this);
	theWndTool.ShowWindows(&m_rectStaticCtl[0]);
//	theWndTool.ShowBmpButtons(&m_bmpBtns[0]);

	SETBUTTONCOLOR(m_buttonDefault, RGB(239, 199, 140));
	SETBUTTONCOLOR(m_buttonCancel, RGB(239, 199, 140));
	SETBUTTONCOLOR(m_buttonOk, RGB(239, 199, 140));

	m_buttonDefault.SetIcon(IDI_ICON_DEFAULT, (int)BTNST_AUTO_DARKER);
	m_buttonDefault.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(255, 255, 255));
	m_buttonDefault.DrawTransparent();	
	
	m_buttonOk.SetIcon(IDI_ICON_OK, (int)BTNST_AUTO_DARKER);
	m_buttonOk.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(255, 255, 255));
	m_buttonOk.DrawTransparent();		

	m_buttonCancel.SetIcon(IDI_ICON_CANCEL, (int)BTNST_AUTO_DARKER);
	m_buttonCancel.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(255, 255, 255));
	m_buttonCancel.DrawTransparent();
	
	m_buttonOpen.SetIcon(IDI_ICON_OPEN, (int)BTNST_AUTO_DARKER);
	m_buttonOpen.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(255, 255, 255));
	m_buttonOpen.DrawTransparent();

	m_staticWarnigInfo.SetCaptionColor(RGB(255, 0, 0));
//	m_staticWarningInfo2.SetCaptionColor(RGB(255, 0, 0));

/*	m_WindowOptionCtl.SetIcon(IDI_ICON_CHECKWINDOW, IDI_ICON_UNCHECKWINDOW);
	m_WindowOptionCtl.DrawTransparent();

	m_FullScreenCtl.SetIcon(IDI_ICON_CHECKSCREEN, IDI_ICON_UNCHECKSCREEN);
	m_FullScreenCtl.DrawTransparent();

	
	SETBUTTONCOLOR(m_WindowOptionCtl, RGB(0, 0, 0));
	SETBUTTONCOLOR(m_FullScreenCtl, RGB(0, 0, 0));*/
	
	if (m_FullScreenValue == 0)
	{
		m_contrFullS.SetBitmaps(IDB_BITMAP_PITCH_OFF, RGB(0, 0, 0));
		m_contrFullS.DrawTransparent();
		m_contrWindows.SetBitmaps(IDB_BITMAP_PITCH_ON, RGB(0, 0, 0));
		m_contrWindows.DrawTransparent();
	}
	else
	{
		m_contrFullS.SetBitmaps(IDB_BITMAP_PITCH_ON, RGB(0, 0, 0));
		m_contrFullS.DrawTransparent();
		m_contrWindows.SetBitmaps(IDB_BITMAP_PITCH_OFF, RGB(0, 0, 0));
		m_contrWindows.DrawTransparent();
	}

	if (m_distinguish == 0)
	{
		m_distinguish2.SetBitmaps(IDB_BITMAP_PITCH_OFF, RGB(0, 0, 0));
		m_distinguish2.DrawTransparent();
		m_distinguish1.SetBitmaps(IDB_BITMAP_PITCH_ON, RGB(0, 0, 0));
		m_distinguish1.DrawTransparent();
	} 
	else
	{
		m_distinguish2.SetBitmaps(IDB_BITMAP_PITCH_ON, RGB(0, 0, 0));
		m_distinguish2.DrawTransparent();
		m_distinguish1.SetBitmaps(IDB_BITMAP_PITCH_OFF, RGB(0, 0, 0));
		m_distinguish1.DrawTransparent();
	}
//	m_contrWindows.SetBitmap(IDB_BITMAP_PITCH_OFF);
//	m_distinguish1.
//	m_distinguish2.



	ModifyStyle(WS_CAPTION, WS_MINIMIZEBOX, SWP_DRAWFRAME);
    SetBitmap(IDB_BITMAP_VERSIONSELECTBACKGROUD);
    SetTransparentColor(RGB(255, 0, 255));
	EnableEasyMove(TRUE);
	SetTransparent(TRUE); 

//	m_staticTitle.SetCaptionColor(RGB(255, 255, 255));
//	m_staticPicPath.SetCaptionColor(RGB(0, 0, 0));
}

// void GameOptionPanel::OnButtonMiniclose() 
// {
// 	OnCancel();
// }

void GameOptionPanel::OnFullScreen() 
{
	m_FullScreenValue = 1;
	m_contrFullS.SetCheck(1);
	m_contrWindows.SetCheck(0);
	m_contrFullS.SetBitmaps(IDB_BITMAP_PITCH_ON, RGB(0, 0, 0));
//	m_contrFullS.DrawTransparent();
	m_contrWindows.SetBitmaps(IDB_BITMAP_PITCH_OFF, RGB(0, 0, 0));
//		m_contrWindows.DrawTransparent();
//	m_WindowOptionCtl.SetCheck(!m_FullScreenCtl.GetCheck());		
}

void GameOptionPanel::OnWindowOption() 
{
	m_FullScreenValue = 0;
	m_contrWindows.SetCheck(1);
	m_contrFullS.SetCheck(0);
	m_contrFullS.SetBitmaps(IDB_BITMAP_PITCH_OFF, RGB(0, 0, 0));
//	m_contrFullS.DrawTransparent();
	m_contrWindows.SetBitmaps(IDB_BITMAP_PITCH_ON, RGB(0, 0, 0));
//	m_contrWindows.DrawTransparent();
//	m_FullScreenCtl.SetCheck(!m_WindowOptionCtl.GetCheck());
}

void GameOptionPanel::OnPixellow()
{
	m_distinguish1.SetCheck(1);
	m_distinguish = 0;
	m_distinguish2.SetBitmaps(IDB_BITMAP_PITCH_OFF, RGB(0, 0, 0));
//	m_distinguish2.DrawTransparent();
	m_distinguish1.SetBitmaps(IDB_BITMAP_PITCH_ON, RGB(0, 0, 0));
//	m_distinguish1.DrawTransparent();
}

void GameOptionPanel::OnPixelhigh()
{
	m_distinguish2.SetCheck(1);
	m_distinguish = 1;
	m_distinguish2.SetBitmaps(IDB_BITMAP_PITCH_ON, RGB(0, 0, 0));
//	m_distinguish2.DrawTransparent();
	m_distinguish1.SetBitmaps(IDB_BITMAP_PITCH_OFF, RGB(0, 0, 0));
//	m_distinguish1.DrawTransparent();
}


