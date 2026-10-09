// ProgressCtrlST.h: interface for the CProgressCtrlST class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROGRESSCTRLST_H__EFA5AAD6_BEAD_4338_B2B5_DFC59EDF1967__INCLUDED_)
#define AFX_PROGRESSCTRLST_H__EFA5AAD6_BEAD_4338_B2B5_DFC59EDF1967__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CProgressCtrlST : public CProgressCtrl
{
// Construction
public:
	CProgressCtrlST();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressCtrlST)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetBitmap(UINT progressbarBkBmp, UINT bkTransColor, UINT blockBmp, UINT blockTransColor);
	void SetBitmap(LPCTSTR progressbarBkBmp, UINT bkTransColor, LPCTSTR blockBmp, UINT blockTransColor);
	void SetClientRect(const CRect & rt)
	{
		m_rectClient = rt;
	}
	virtual ~CProgressCtrlST();

	// Generated message map functions
protected:
	//{{AFX_MSG(CProgressCtrlST)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CBitmap m_bmpProgressBk;
	void PaintProgressBar(CDC * pDC);
	void PaintBk(CDC * pDC);
	CBitmap			m_bmpBlock;
	CDC*			m_pbmpOldBk;
	CBitmap			m_bmpBk;
	CDC				m_dcBk;
	CRect			m_rectClient;
	int				m_nProgressBarWidth;
	int				m_nProgressBarHeight;
	int				m_nBlockWidth;
	int				m_nBlockHeight;
	UINT			m_uBKTransColor;
	UINT			m_uBlockTransColor;
	int				m_nBlockSpace;
};
#endif //AFX_PROGRESSCTRLST_H__EFA5AAD6_BEAD_4338_B2B5_DFC59EDF1967__INCLUDED_
/////////////////////////////////////////////////////////////////////////////
