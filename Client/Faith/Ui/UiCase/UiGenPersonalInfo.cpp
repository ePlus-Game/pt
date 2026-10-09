// UiGenPersonalInfo.cpp: implementation of the KUiGenPersonalInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "CoreShell.h"
#include "UiGenPersonalInfo.h"

extern iCoreShell*	g_pCoreShell;

template<> 
KUiGenPersonalInfo* KUiWndSingleton<KUiGenPersonalInfo>::ms_Singleton = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
KUiGenPersonalInfo::KUiGenPersonalInfo( const CEGUI::String& id_name )
: KUiWndSingleton<KUiGenPersonalInfo>( id_name )
{
	m_edtName	= NULL;
	m_edtSpouse	= NULL;
	m_edtAge	= NULL;
	m_edtArea	= NULL;
	m_edtQQ		= NULL;
	m_edtMSN	= NULL;
	m_edtIS		= NULL;
	m_edtUT		= NULL;
	m_edtTele	= NULL;
	m_edtMobile	= NULL;

	m_rbMale	= NULL;
	m_rbFemale	= NULL;
	m_btnOK		= NULL;
	m_btnEdit	= NULL;
	m_enableEdit = false;
	m_editing	 = false;
}

KUiGenPersonalInfo::~KUiGenPersonalInfo()
{

}

void 
KUiGenPersonalInfo::Init()
{
	if ( NULL == ms_Singleton )
		return;
	if ( NULL == ms_Singleton->m_pThisWnd )
		return;

	Window* pInfoPanel = m_pThisWnd->getChild( "TaharezLook/GenPersonalInfo/infoPanel" );
	TLButton* pBtnCancel = static_cast< TLButton* >( m_pThisWnd->getChild( "TaharezLook/GenPersonalInfo/btnCancel" ) );

	m_edtName	= static_cast< TLEditbox* >( pInfoPanel->getChild( pInfoPanel->getName() + "/edtName"	) );
	m_edtSpouse	= static_cast< TLEditbox* >( pInfoPanel->getChild( pInfoPanel->getName() + "/edtSpouse" ) );
	m_edtAge	= static_cast< TLEditbox* >( pInfoPanel->getChild( pInfoPanel->getName() + "/edtAge"	) );
	m_edtArea	= static_cast< TLEditbox* >( pInfoPanel->getChild( pInfoPanel->getName() + "/edtArea"	) );
	m_edtQQ		= static_cast< TLEditbox* >( pInfoPanel->getChild( pInfoPanel->getName() + "/edtQQ"		) );
	m_edtMSN	= static_cast< TLEditbox* >( pInfoPanel->getChild( pInfoPanel->getName() + "/edtMSN"	) );
	m_edtIS		= static_cast< TLEditbox* >( pInfoPanel->getChild( pInfoPanel->getName() + "/edtIS"		) );
	m_edtUT		= static_cast< TLEditbox* >( pInfoPanel->getChild( pInfoPanel->getName() + "/edtUT"		) );
	m_edtTele	= static_cast< TLEditbox* >( pInfoPanel->getChild( pInfoPanel->getName() + "/edtTele"	) );
	m_edtMobile	= static_cast< TLEditbox* >( pInfoPanel->getChild( pInfoPanel->getName() + "/edtMobile" ) );

	m_rbMale	= static_cast< TLRadioButton* >( pInfoPanel->getChild( pInfoPanel->getName() + "/rbMale"	) );
	m_rbFemale	= static_cast< TLRadioButton* >( pInfoPanel->getChild( pInfoPanel->getName() + "/rbFemale"	) );
	m_btnOK		= static_cast< TLButton* >( m_pThisWnd->getChild( m_pThisWnd->getName() + "/btnOK"	) );
	m_btnEdit	= static_cast< TLButton* >( m_pThisWnd->getChild( m_pThisWnd->getName() + "/btnEdit"	) );

	
	m_btnOK->subscribeEvent( 
		PushButton::EventMouseClick, 
		Event::Subscriber( &KUiGenPersonalInfo::btnOK_Clicked, ms_Singleton ) );

	m_btnEdit->subscribeEvent( 
		PushButton::EventMouseClick, 
		Event::Subscriber( &KUiGenPersonalInfo::btnEdit_Clicked, ms_Singleton ) );
	
	pBtnCancel->subscribeEvent( 
		PushButton::EventMouseClick, 
		Event::Subscriber( &KUiGenPersonalInfo::btnCancel_Clicked, ms_Singleton ) );
}

bool 
KUiGenPersonalInfo::btnOK_Clicked( const EventArgs& e )
{
	if ( ! isControlsValid() )
		return false;
	if ( ! m_editing )
		return false;

	UIPlayerRealInfo info;
	info.Sex	= m_rbMale->isSelected() ? 0 : 1;
	string sAge = Utf8ToAnsi( m_edtAge->getText() );
	int inputAge= ::atoi( sAge.c_str() );
	info.Age = inputAge > 255 ? 255 : inputAge;
	
	CopyUtf8ToAnsi( info.Name		, m_edtName->getText()	, sizeof( info.Name			) );
	CopyUtf8ToAnsi( info.Address	, m_edtArea->getText()	, sizeof( info.Address		) );
	CopyUtf8ToAnsi( info.QQNumber	, m_edtQQ->getText()	, sizeof( info.QQNumber		) );
	CopyUtf8ToAnsi( info.MSNNumber	, m_edtMSN->getText()	, sizeof( info.MSNNumber	) );
	CopyUtf8ToAnsi( info.ISNumber	, m_edtIS->getText()	, sizeof( info.ISNumber		) );
	CopyUtf8ToAnsi( info.UTNumber	, m_edtUT->getText()	, sizeof( info.UTNumber		) );
	CopyUtf8ToAnsi( info.TeleNumber	, m_edtTele->getText()	, sizeof( info.TeleNumber	) );
	CopyUtf8ToAnsi( info.MobleNumber, m_edtMobile->getText(), sizeof( info.MobleNumber	) );
	
	BOOL nRet = g_pCoreShell->OperationRequest( GOI_SET_PLAYER_REAL_INFO, reinterpret_cast< unsigned int >( &info ), NULL );
	
	if ( FALSE != nRet )
	{
		setReadOnly( true );
		m_editing = false;
		Hide();
	}

	return nRet != FALSE;
}

void 
KUiGenPersonalInfo::CopyUtf8ToAnsi( char* dest, const String& src, const int length )
{
	string sAnsi =  Utf8ToAnsi( src );
	strncpy( dest, sAnsi.c_str(), length - 1 );
	dest[ length - 1 ] = 0;
}

bool KUiGenPersonalInfo::isControlsValid()
{
	return NULL != m_edtName
		&& NULL != m_edtSpouse
		&& NULL != m_edtAge
		&& NULL != m_edtArea
		&& NULL != m_edtQQ
		&& NULL != m_edtMSN
		&& NULL != m_edtIS
		&& NULL != m_edtUT
		&& NULL != m_edtTele
		&& NULL != m_edtMobile
		&& NULL != m_rbMale
		&& NULL != m_rbFemale
		&& NULL != m_btnOK
		&& NULL != m_btnEdit;
}

bool 
KUiGenPersonalInfo::btnCancel_Clicked( const EventArgs& e )
{
	Hide();
	return true;
}

void 
KUiGenPersonalInfo::UpdateInfo( const UIPlayerRealInfoEx& newInfo )
{
	if ( ! isControlsValid() )
		return;

	m_edtName->setText( AnsiToUtf8( newInfo.Name ) );
	m_edtAge->setText( iToString( newInfo.Age ) );
	m_edtArea->setText( AnsiToUtf8( newInfo.Address ) );
	m_edtIS->setText( AnsiToUtf8( newInfo.ISNumber) );
	m_edtQQ->setText( AnsiToUtf8( newInfo.QQNumber) );
	m_edtMSN->setText( AnsiToUtf8( newInfo.MSNNumber) );
	m_edtUT->setText( AnsiToUtf8( newInfo.UTNumber) );
	m_edtTele->setText( AnsiToUtf8( newInfo.TeleNumber ) );
	m_edtMobile->setText( AnsiToUtf8( newInfo.MobleNumber ) );
	m_edtSpouse->setText( AnsiToUtf8( newInfo.Consort ) );
	if ( 0 == newInfo.Sex )
	{
		m_rbMale->setSelected( true );
	}
	else
	{
		m_rbFemale->setSelected( true );
	}

	Show();
}

void KUiGenPersonalInfo::SetEnableEdit( bool enable )
{
	m_enableEdit = enable;
}

void KUiGenPersonalInfo::Show()
{
	if ( NULL == ms_Singleton )
		return;

	ms_Singleton->beforeShow();
	KUiWndSingleton< KUiGenPersonalInfo >::Show();
}

void KUiGenPersonalInfo::setReadOnly( bool flag )
{
	if ( !isControlsValid() )
		return;

	m_edtName->setReadOnly( flag );
	m_edtAge->setReadOnly( flag );
	m_edtArea->setReadOnly( flag );
	m_edtIS->setReadOnly( flag );
	m_edtQQ->setReadOnly( flag );
	m_edtMSN->setReadOnly( flag );
	m_edtUT->setReadOnly( flag );
	m_edtTele->setReadOnly( flag );
	m_edtMobile->setReadOnly( flag );
	m_edtSpouse->setReadOnly( true );
	m_rbFemale->setEnabled( ! flag );
	m_rbMale->setEnabled( ! flag );
	m_btnOK->setVisible( ! flag );
	m_btnEdit->setVisible( flag );
	m_editing = !flag;
}

void KUiGenPersonalInfo::beforeShow()
{
	if ( !isControlsValid() )
		return;

	setReadOnly( true );
	m_btnEdit->setEnabled( m_enableEdit );
}

bool 
KUiGenPersonalInfo::btnEdit_Clicked( const EventArgs& e )
{
	setReadOnly( false );
	return true;	
}