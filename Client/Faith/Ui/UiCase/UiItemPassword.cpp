// UiItemPassword.cpp: implementation of the KUiItemPassword class.
//
//////////////////////////////////////////////////////////////////////

#include "UiItemPassword.h"
#include "CoreShell.h"
#include "UiErrorMessageBox.h"
#include "../KMessageCentre.h"

extern iCoreShell*	g_pCoreShell;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

template<> 
KUiItemPassword* KUiWndSingleton<KUiItemPassword>::ms_Singleton	= NULL;

KUiItemPassword::KUiItemPassword( const CEGUI::String& id_name )
: KUiWndSingleton<KUiItemPassword>( id_name )
{
	m_edtPassword = NULL;
}

KUiItemPassword::~KUiItemPassword()
{
}

void KUiItemPassword::Init()
{
	if ( ( NULL != ms_Singleton ) && ( NULL != ms_Singleton->m_pThisWnd ) )
	{
		m_edtPassword = static_cast< TLEditbox* >( m_pThisWnd->getChild( "TaharezLook/ItemPassword/edtPassword" ) );
		m_edtPassword->setAscIICharacters( true );
		m_edtPassword->subscribeEvent(
			Window::EventKeyDown,
			Event::Subscriber( &KUiItemPassword::edtPassword_KeyDown, this ) );
		
		m_pThisWnd->getChild( "TaharezLook/ItemPassword/OK" )->subscribeEvent(
			Window::EventClicked,
			Event::Subscriber( &KUiItemPassword::btnOK_Clicked, this ) );

		m_pThisWnd->getChild( "TaharezLook/ItemPassword/Cancel" )->subscribeEvent(
			Window::EventClicked,
			Event::Subscriber( &KUiItemPassword::btnCancel_Clicked, this ) );

		m_pThisWnd->getChild( "TaharezLook/ItemPassword/Close" )->subscribeEvent(
			Window::EventClicked,
			Event::Subscriber( &KUiItemPassword::btnCancel_Clicked, this ) );
	}
}

void KUiItemPassword::Show()
{
	if ( NULL != ms_Singleton )
	{
		ms_Singleton->clearEditbox();
	}
	KUiWndSingleton<KUiItemPassword>::Show();
}

void KUiItemPassword::clearEditbox()
{
	if ( NULL != m_edtPassword )
	{
		m_edtPassword->setText( "" );
		m_edtPassword->activate();
	}
}

bool KUiItemPassword::btnOK_Clicked( const CEGUI::EventArgs& e )
{
	sendRequest();
	return true;	
}

bool KUiItemPassword::btnCancel_Clicked( const CEGUI::EventArgs& e )
{
	Hide();
	return true;
}

bool KUiItemPassword::edtPassword_KeyDown( const CEGUI::EventArgs& e )
{
	Key::Scan inputCode = static_cast< const KeyEventArgs& >( e ).scancode;
	switch ( inputCode )
    {
    case Key::Return: 
	case Key::NumpadEnter:
		return sendRequest();
        break;
		
    default:
        return false;
    }
	
    return true;
}

bool KUiItemPassword::sendRequest()
{
	if ( NULL == m_edtPassword )
	{
		return false;
	}

	if ( ! m_edtPassword->getText().empty() )
	{
		string pw = Utf8ToAnsi( m_edtPassword->getText() );
		g_pCoreShell->OperationRequest( GOI_STOREBOX_ENTER_PASSWORD, reinterpret_cast< unsigned int >( pw.c_str() ), strlen( pw.c_str() ) );
		Hide();

		return true;
	}
	else
	{
		SendItemStoreError( 12 );
		return false;
	}
}

/********************************************************************
/*						class: KUiItemPassword_Create
*********************************************************************/
template<> 
KUiItemPassword_Create* KUiWndSingleton<KUiItemPassword_Create>::ms_Singleton	= NULL;

KUiItemPassword_Create::KUiItemPassword_Create( const CEGUI::String& id_name )
: KUiWndSingleton<KUiItemPassword_Create>( id_name )
{
	m_edtNewPassword = NULL;
	m_edtConfirmPassword = NULL;
}

KUiItemPassword_Create::~KUiItemPassword_Create()
{
	
}

void KUiItemPassword_Create::Init()
{
	if ( ( NULL != ms_Singleton ) && ( NULL != ms_Singleton->m_pThisWnd ) )
	{
		m_edtNewPassword = static_cast< TLEditbox* >( m_pThisWnd->getChild( "TaharezLook/ItemPassword_Create/edtNewPassword" ) );
		m_edtNewPassword->setAscIICharacters( true );
		m_edtNewPassword->subscribeEvent(
			Window::EventKeyDown,
			Event::Subscriber( &KUiItemPassword_Create::edtPassword_KeyDown, this ) );

		m_edtConfirmPassword = static_cast< TLEditbox* >( m_pThisWnd->getChild( "TaharezLook/ItemPassword_Create/edtConfirmPassword" ) );
		m_edtConfirmPassword->setAscIICharacters( true );
		m_edtConfirmPassword->subscribeEvent(
			Window::EventKeyDown,
			Event::Subscriber( &KUiItemPassword_Create::edtPassword_KeyDown, this ) );

		m_pThisWnd->getChild( "TaharezLook/ItemPassword_Create/OK" )->subscribeEvent(
			Window::EventClicked,
			Event::Subscriber( &KUiItemPassword_Create::btnOK_Clicked, this ) );
		
		m_pThisWnd->getChild( "TaharezLook/ItemPassword_Create/Cancel" )->subscribeEvent(
			Window::EventClicked,
			Event::Subscriber( &KUiItemPassword_Create::btnCancel_Clicked, this ) );

		m_pThisWnd->getChild( "TaharezLook/ItemPassword_Create/Close" )->subscribeEvent(
			Window::EventClicked,
			Event::Subscriber( &KUiItemPassword_Create::btnCancel_Clicked, this ) );
	}
}

void KUiItemPassword_Create::Show()
{
	if ( NULL != ms_Singleton )
	{
		ms_Singleton->clearEditbox();
	}
	KUiWndSingleton<KUiItemPassword_Create>::Show();
}

void KUiItemPassword_Create::clearEditbox()
{
	if ( ( NULL != m_edtNewPassword ) && ( NULL != m_edtConfirmPassword ) )
	{
		m_edtNewPassword->setText( "" );
		m_edtConfirmPassword->setText( "" );
		
		m_edtNewPassword->activate();
	}
}

bool KUiItemPassword_Create::btnOK_Clicked( const CEGUI::EventArgs& e )
{
	sendRequest();
	return true;	
}

bool KUiItemPassword_Create::btnCancel_Clicked( const CEGUI::EventArgs& e )
{
	Hide();
	return true;
}

bool KUiItemPassword_Create::sendRequest()
{
	if ( ( NULL == m_edtNewPassword ) || ( NULL == m_edtConfirmPassword ) )
	{
		return false;
	}

	if ( m_edtNewPassword->getText().empty() )
	{
		SendItemStoreError( 12 );
		return false;
	}
	
	if ( m_edtConfirmPassword->getText() != m_edtNewPassword->getText() )
	{
		SendItemStoreError( 2 );
		return false;
	}

	string pw = Utf8ToAnsi( m_edtNewPassword->getText() );
	g_pCoreShell->OperationRequest( GOI_STOREBOX_CREATE_PASSWORD, reinterpret_cast< unsigned int >( pw.c_str() ), strlen( pw.c_str() ) );
	Hide();

	return true;
}

bool KUiItemPassword_Create::edtPassword_KeyDown( const CEGUI::EventArgs& e )
{
	Key::Scan inputCode = static_cast< const KeyEventArgs& >( e ).scancode;
	switch ( inputCode )
    {
    case Key::Return: 
	case Key::NumpadEnter:
		{
			if ( m_edtNewPassword->isActive() )
			{
				m_edtConfirmPassword->activate();
			}
			else
			{
				return sendRequest();
			}
		}
        break;

	case Key::Tab:
		{
			if ( m_edtConfirmPassword->isActive() )
			{
				m_edtNewPassword->activate();
			}
			else
			{
				m_edtConfirmPassword->activate();
			}
		}
		break;
		
    default:
        return false;
    }
	
    return true;	
}

/********************************************************************
/*						class: KUiItemPassword_Modify
*********************************************************************/
template<> 
KUiItemPassword_Modify* KUiWndSingleton<KUiItemPassword_Modify>::ms_Singleton	= NULL;

KUiItemPassword_Modify::KUiItemPassword_Modify( const CEGUI::String& id_name )
: KUiWndSingleton<KUiItemPassword_Modify>( id_name )
{
	m_edtOldPassword = NULL;
	m_edtNewPassword = NULL;
	m_edtConfirmPassword = NULL;
}

KUiItemPassword_Modify::~KUiItemPassword_Modify()
{
	
}

void KUiItemPassword_Modify::Show()
{
	if ( NULL != ms_Singleton )
	{
		ms_Singleton->clearEditbox();
	}
	KUiWndSingleton<KUiItemPassword_Modify>::Show();
}

void KUiItemPassword_Modify::Init()
{
	if ( ( NULL != ms_Singleton ) && ( NULL != ms_Singleton->m_pThisWnd ) )
	{
		m_edtOldPassword = static_cast< TLEditbox* >( m_pThisWnd->getChild( "TaharezLook/ItemPassword_Modify/edtOldPassword" ) );
		m_edtOldPassword->setAscIICharacters( true );
		m_edtOldPassword->subscribeEvent(
			Window::EventKeyDown,
			Event::Subscriber( &KUiItemPassword_Modify::edtPassword_KeyDown, this ) );

		m_edtNewPassword = static_cast< TLEditbox* >( m_pThisWnd->getChild( "TaharezLook/ItemPassword_Modify/edtNewPassword" ) );
		m_edtNewPassword->setAscIICharacters( true );
		m_edtNewPassword->subscribeEvent(
			Window::EventKeyDown,
			Event::Subscriber( &KUiItemPassword_Modify::edtPassword_KeyDown, this ) );
		
		m_edtConfirmPassword = static_cast< TLEditbox* >( m_pThisWnd->getChild( "TaharezLook/ItemPassword_Modify/edtConfirmPassword" ) );
		m_edtConfirmPassword->setAscIICharacters( true );
		m_edtConfirmPassword->subscribeEvent(
			Window::EventKeyDown,
			Event::Subscriber( &KUiItemPassword_Modify::edtPassword_KeyDown, this ) );

		m_pThisWnd->getChild( "TaharezLook/ItemPassword_Modify/OK" )->subscribeEvent(
			Window::EventClicked,
			Event::Subscriber( &KUiItemPassword_Modify::btnOK_Clicked, this ) );
		
		m_pThisWnd->getChild( "TaharezLook/ItemPassword_Modify/Cancel" )->subscribeEvent(
			Window::EventClicked,
			Event::Subscriber( &KUiItemPassword_Modify::btnCancel_Clicked, this ) );

		m_pThisWnd->getChild( "TaharezLook/ItemPassword_Modify/Close" )->subscribeEvent(
			Window::EventClicked,
			Event::Subscriber( &KUiItemPassword_Modify::btnCancel_Clicked, this ) );
	}
}

void KUiItemPassword_Modify::clearEditbox()
{
	if ( ( NULL != m_edtNewPassword ) && ( NULL != m_edtConfirmPassword ) && ( NULL != m_edtOldPassword ) )
	{
		m_edtNewPassword->setText( "" );
		m_edtConfirmPassword->setText( "" );
		m_edtOldPassword->setText( "" );
		
		m_edtOldPassword->activate();
	}	
}

bool KUiItemPassword_Modify::btnOK_Clicked( const CEGUI::EventArgs& e )
{
	sendRequest();
	return true;
}

bool KUiItemPassword_Modify::btnCancel_Clicked( const CEGUI::EventArgs& e )
{
	Hide();
	return true;
}

bool KUiItemPassword_Modify::sendRequest()
{
	if ( ( NULL == m_edtOldPassword ) || ( NULL == m_edtNewPassword ) || ( NULL == m_edtConfirmPassword ) )
	{
		return false;
	}

// 	if ( m_edtNewPassword->getText().empty() || m_edtOldPassword->getText().empty() )
// 	{
// 		SendItemStoreError( 12 );
// 		return false;
// 	}

	if ( m_edtNewPassword->getText() != m_edtConfirmPassword->getText() )
	{
		SendItemStoreError( 2 );
		return false;
	}

	string oldPw = Utf8ToAnsi( m_edtOldPassword->getText() );
	string newPw = Utf8ToAnsi( m_edtNewPassword->getText() );

	g_pCoreShell->OperationRequest( 
		GOI_STOREBOX_CHANGE_PASSWORD, 
		reinterpret_cast< unsigned int >( oldPw.c_str() ), 
		reinterpret_cast< unsigned int >( newPw.c_str() ) );

	Hide();
	return true;	
}

bool KUiItemPassword_Modify::edtPassword_KeyDown( const CEGUI::EventArgs& e )
{
	Key::Scan inputCode = static_cast< const KeyEventArgs& >( e ).scancode;
	switch ( inputCode )
    {
    case Key::Return: 
	case Key::NumpadEnter:
		{
			if ( m_edtOldPassword->isActive() )
			{
				m_edtNewPassword->activate();
			}
			else if ( m_edtNewPassword->isActive() )
			{
				m_edtConfirmPassword->activate();
			}
			else
			{
				return sendRequest();
			}
		}
		
        break;
	
	case Key::Tab:
		{
			if ( m_edtOldPassword->isActive() )
			{
				m_edtNewPassword->activate();
			}
			else if ( m_edtNewPassword->isActive() )
			{
				m_edtConfirmPassword->activate();
			}
			else
			{
				m_edtOldPassword->activate();
			}
		}
		break;
		
    default:
        return false;
    }
	
    return true;	
}

void SendItemStoreError( int code )
{
	char* msg = KMessageCentre::GetMessage( storebox_error_message, code );
	if ( NULL != msg )
	{
		KUiErrorMessageBox::GetSingleton().AddMessage( AnsiToUtf8( msg ) );
	}		
}