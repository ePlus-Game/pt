// ServerListDlg.cpp : implementation file
//

#include "stdafx.h"
// #include "autoupdate.h"
// #include "ServerListDlg.h"
// //#include "ServerList.h"
// 
// #ifdef _DEBUG
// #define new DEBUG_NEW
// #undef THIS_FILE
// static char THIS_FILE[] = __FILE__;
// #endif

/////////////////////////////////////////////////////////////////////////////
// CServerListDlg dialog


// CServerListDlg::CServerListDlg(CWnd* pParent /*=NULL*/)
// 	: CDialog(CServerListDlg::IDD, pParent)
// {
	//{{AFX_DATA_INIT(CServerListDlg)
	//}}AFX_DATA_INIT
/*}*/


// void CServerListDlg::DoDataExchange(CDataExchange* pDX)
// {
// 	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServerListDlg)
	DDX_Control(pDX, IDC_LIST, m_List);
	//}}AFX_DATA_MAP
/*}*/


/*BEGIN_MESSAGE_MAP(CServerListDlg, CDialog)*/
	//{{AFX_MSG_MAP(CServerListDlg)
// 	ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
// 	ON_BN_CLICKED(IDC_LIST_OK, OnListOk)
// 	ON_BN_CLICKED(IDC_LIST_CANCEL, OnListCancel)
	//}}AFX_MSG_MAP
/*END_MESSAGE_MAP()*/

/////////////////////////////////////////////////////////////////////////////
// CServerListDlg message handlers

// BOOL CServerListDlg::OnInitDialog() 
// {
// 	CDialog::OnInitDialog();
// 	
// 	// TODO: Add extra initialization here
// 	ASSERT(g_ServerList.IsReady());
// 
// 	REGION_GROUP * pRegionGroup=g_ServerList.GetRegions();
// 	for (int iRegion=0;iRegion<g_ServerList.GetRegionNum();iRegion++)
// 	{
//       for (int iServer=0;iServer<pRegionGroup[iRegion].mServerNum;iServer++)
// 	  {
//           m_List.AddString(pRegionGroup[iRegion].mServerGroupName+":"+pRegionGroup[iRegion].mServers[iServer].mTitle+"("+pRegionGroup[iRegion].mServers[iServer].mState+")");
// 	  }//end for iServer
//       
// 	}//end for iRegion
// 
// 	return TRUE;  // return TRUE unless you set the focus to a control
// 	              // EXCEPTION: OCX Property Pages should return FALSE
// }


// void CServerListDlg::OnDblclkList() 
// {
// 	// TODO: Add your control notification handler code here
// 	int index=m_List.GetCurSel();
// 	ASSERT(g_ServerList.IsReady());
// 
// 	REGION_GROUP * pRegionGroup=g_ServerList.GetRegions();
// 	int iDest=0;
// 	for (int iRegion=0;iRegion<g_ServerList.GetRegionNum();iRegion++)
// 	{
// 		for (int iServer=0;iServer<pRegionGroup[iRegion].mServerNum;iServer++)
// 		{
// 			if (iDest==index)
// 			{
//                  m_ServerAddr=pRegionGroup[iRegion].mServers[iServer].mAddress;
// 			}//endif
// 			iDest++;
// 		}//end for iServer
// 		
// 	}//end for iRegion
// 
// 	CDialog::OnCancel();
/*}*/

// CString  CServerListDlg::GetTheServerResult(void)const
// {
// 	return m_ServerAddr;
// }
// 
// void CServerListDlg::OnListOk() 
// {
// 	// TODO: Add your control notification handler code here
// 	int index=m_List.GetCurSel();
// 	ASSERT(g_ServerList.IsReady());
// 
// 	REGION_GROUP * pRegionGroup=g_ServerList.GetRegions();
// 	int iDest=0;
// 	for (int iRegion=0;iRegion<g_ServerList.GetRegionNum();iRegion++)
// 	{
// 		for (int iServer=0;iServer<pRegionGroup[iRegion].mServerNum;iServer++)
// 		{
// 			if (iDest==index)
// 			{
//                  m_ServerAddr=pRegionGroup[iRegion].mServers[iServer].mAddress;
// 			}//endif
// 			iDest++;
// 		}//end for iServer
// 		
// 	}//end for iRegion
// 
// 	CDialog::OnCancel();
// }
// 
// void CServerListDlg::OnListCancel() 
// {
// 	// TODO: Add your control notification handler code here
// 	CDialog::OnCancel();
// }
