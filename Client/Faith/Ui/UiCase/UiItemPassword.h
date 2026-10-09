// UiItemPassword.h: interface for the KUiItemPassword class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIITEMPASSWORD_H__FC852673_7546_424D_A083_CAD8E76DA5C3__INCLUDED_)
#define AFX_UIITEMPASSWORD_H__FC852673_7546_424D_A083_CAD8E76DA5C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../uicommon.h"
#include "../UiElem/TLEditbox.h"

void SendItemStoreError( int code );

/********************************************************************
/*						class: KUiItemPassword
*********************************************************************/
class KUiItemPassword : public KUiWndSingleton<KUiItemPassword>
{
public:
	KUiItemPassword( const CEGUI::String& id_name );
	virtual ~KUiItemPassword();
	
	static void	Show();

	void		Init();

private:
	void		clearEditbox();
	bool		sendRequest();

	bool		btnOK_Clicked( const CEGUI::EventArgs& e );
	bool		btnCancel_Clicked( const CEGUI::EventArgs& e );
	bool		edtPassword_KeyDown( const CEGUI::EventArgs& e );

private:
	TLEditbox*	m_edtPassword;
};


/********************************************************************
/*						class: KUiItemPassword_Create
*********************************************************************/
class KUiItemPassword_Create : public KUiWndSingleton<KUiItemPassword_Create>
{
public:
	KUiItemPassword_Create( const CEGUI::String& id_name );
	virtual ~KUiItemPassword_Create();
	
	static void	Show();
	
	void		Init();

private:
	void		clearEditbox();
	bool		sendRequest();
	
	bool		btnOK_Clicked( const CEGUI::EventArgs& e );
	bool		btnCancel_Clicked( const CEGUI::EventArgs& e );
	bool		edtPassword_KeyDown( const CEGUI::EventArgs& e );

private:
	TLEditbox*	m_edtNewPassword;
	TLEditbox*	m_edtConfirmPassword;
};


/********************************************************************
/*						class: KUiItemPassword_Modify
*********************************************************************/
class KUiItemPassword_Modify : public KUiWndSingleton<KUiItemPassword_Modify>
{
public:
	KUiItemPassword_Modify( const CEGUI::String& id_name );
	virtual ~KUiItemPassword_Modify();
	
	static void	Show();
	
	void		Init();

private:
	void		clearEditbox();
	bool		sendRequest();

	bool		btnOK_Clicked( const CEGUI::EventArgs& e );
	bool		btnCancel_Clicked( const CEGUI::EventArgs& e );
	bool		edtPassword_KeyDown( const CEGUI::EventArgs& e );

private:
	TLEditbox*	m_edtOldPassword;
	TLEditbox*	m_edtNewPassword;
	TLEditbox*	m_edtConfirmPassword;
};

#endif // !defined(AFX_UIITEMPASSWORD_H__FC852673_7546_424D_A083_CAD8E76DA5C3__INCLUDED_)
