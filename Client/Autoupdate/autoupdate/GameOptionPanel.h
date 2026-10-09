#if !defined(AFX_GAMEOPTIONPANEL_H__053869DE_B932_48D7_A310_842AF8B57B9B__INCLUDED_)
#define AFX_GAMEOPTIONPANEL_H__053869DE_B932_48D7_A310_842AF8B57B9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GameOptionPanel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// GameOptionPanel dialog

#include"lientGameOptionProcess.h"
#include<fstream>
#include<memory>
#include<iterator>
#include"BitmapSlider.h"
#include "BitmapDialog.h"
#include "BtnST.h"
#include "TransparentStatic.h"
#include "bmpbutton.h"
#include "WndTool.h"
using namespace std;

void DisplayErrorInfo(string& ErrorInfo);



//确定机器最优表现方式函数类型

class GameOptionPanel : public CBitmapDialog
{
// Construction
public:
	GameOptionPanel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(GameOptionPanel)
	enum { IDD = IDD_DIALOGBAR };
//	CComboBox	m_ScreenSel;
//	CComboBox   m_ImageSel;      // 动画质量

	CEdit	m_editPath;
//	CTransparentStatic	m_staticPicPath;
//	CTransparentStatic	m_staticTitle;
//	CTransparentStatic	m_staticWarningInfo2;
	CTransparentStatic	m_staticWarnigInfo;
	CBitmapSlider       m_MusicBar;
	CBitmapSlider       m_SoundBar;
//	CBmpButton	m_buttonMiniClose;
	CButtonST	m_buttonOpen;
	CButtonST	m_buttonOk;
	CButtonST	m_buttonCancel;
	CButtonST	m_buttonDefault;
//	CButtonST	m_WindowOptionCtl;
	CButton	m_3DOptionCtl;
//	CButtonST	m_FullScreenCtl;
	CButton	m_2DOptionCtl;
	CButton	m_DynaLightEnableCtl;
	int		m_2DOptionValue;
	int		m_FullScreenValue;
	int		m_distinguish;
	BOOL	m_DynaLightEnableValue;
	TextSet NewOptionS;       // 用来保存最新的设置选项  
	HICON m_hIcon;
	CString	m_txtCapPath;
	CButtonST m_contrFullS;
	CButtonST m_contrWindows;
	CButtonST m_distinguish1;
	CButtonST m_distinguish2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(GameOptionPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(GameOptionPanel)
	afx_msg void OnOk();
	afx_msg void OnCancel();
	afx_msg void On3DOptionSelected();
	afx_msg void On2DOption();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnDefault();
	afx_msg void OnBtnCappath();
//	afx_msg void OnButtonMiniclose();
	afx_msg void OnFullScreen();
	afx_msg void OnWindowOption();
	afx_msg void OnPixellow();
	afx_msg void OnPixelhigh();
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void OnPostEraseBkgnd(CDC* pDC);
private:
	void InitUI();
private:
	enum
	{
		COUNT_CONTROL = 12,	
		//COUNT_BMPBUTTON = 1
	};
	//静态控件的窗口位置	
	static WindowRect m_rectStaticCtl[COUNT_CONTROL + 1];
	//位图按钮的个数	
//	static BMPButton  m_bmpBtns[COUNT_BMPBUTTON + 1];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMEOPTIONPANEL_H__053869DE_B932_48D7_A310_842AF8B57B9B__INCLUDED_)
