#ifndef _KEX_SERVER_H_
#define _KEX_SERVER_H_

#include "WndTool.h"
#include "BitmapDialog.h"
#include "BtnST.h"
#include "resource.h"
#include "bmpbutton.h"

class CExServerDlg : public CBitmapDialog
{
	public:
		CExServerDlg(CWnd* pParent = NULL);
		virtual ~CExServerDlg();

		CBmpButton m_contrNormal;
		CBmpButton m_contrExMode;
		CBmpButton m_close;

		enum {IDD = IDD_SER_MODE_DIALOG};

		BOOL OnInitDialog();
		

		enum
		{
			COUNT_BMPBUTTON = 3
		};

		static BMPButton  m_bmpBtns[COUNT_BMPBUTTON + 1];

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);

	private:
		void InitUI();

	protected:
		afx_msg void OnNormal();
		afx_msg void OnExMode();
		afx_msg void OnQuit();

		DECLARE_MESSAGE_MAP();
};




#endif
