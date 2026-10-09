// UiGenPersonalInfo.h: interface for the KUiGenPersonalInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIGENPERSONALINFO_H__1082B63B_D07D_4E74_8C24_9EE61990D101__INCLUDED_)
#define AFX_UIGENPERSONALINFO_H__1082B63B_D07D_4E74_8C24_9EE61990D101__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../uicommon.h"
#include "TLEditbox.h"
#include "TLRadioButton.h"

class KUiGenPersonalInfo : public KUiWndSingleton<KUiGenPersonalInfo>
{
public:
	KUiGenPersonalInfo( const CEGUI::String& id_name );
	virtual ~KUiGenPersonalInfo();
	
	static void	
	Show();

	void
	Init();

	void
	UpdateInfo( const UIPlayerRealInfoEx& newInfo );

	void
	UpdateShow();

	void
	SetEnableEdit( bool enable );

private:
	bool 
	btnOK_Clicked( const EventArgs& e );

	bool 
	btnEdit_Clicked( const EventArgs& e );

	bool 
	btnCancel_Clicked( const EventArgs& e );


	void
	CopyUtf8ToAnsi( char* dest, const String& src, const int length );

	bool
	isControlsValid();

	void
	setReadOnly( bool flag );

	void
	beforeShow();

private:
	TLEditbox* m_edtName;
	TLEditbox* m_edtSpouse;
	TLEditbox* m_edtAge;
	TLEditbox* m_edtArea;
	TLEditbox* m_edtQQ;
	TLEditbox* m_edtMSN;
	TLEditbox* m_edtIS;
	TLEditbox* m_edtUT;
	TLEditbox* m_edtTele;
	TLEditbox* m_edtMobile;

	TLRadioButton* m_rbMale;
	TLRadioButton* m_rbFemale;
	TLButton*		m_btnOK;
	TLButton*		m_btnEdit;

	bool m_enableEdit;
	bool m_editing;
};

#endif // !defined(AFX_UIGENPERSONALINFO_H__1082B63B_D07D_4E74_8C24_9EE61990D101__INCLUDED_)
