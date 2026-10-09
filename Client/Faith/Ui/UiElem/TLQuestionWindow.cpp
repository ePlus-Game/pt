// TLQuestionWindow.cpp: implementation of the TLQuestionWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "TLQuestionWindow.h"
#include "CoreShell.h"
#include "../UiCommon.h"
#include "../UiConfigManager.h"
#include "../ConvertUTF.h"
#include "../../Login/Login.h"
#include "GameDataDef.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const utf8	 TLQuestionWindow::WidgetTypeName[]		   = "TaharezLook/QuestionWindow";
const String TLQuestionWindow::TemplateLayoutsFileName = "uisettings/layouts/QuestionWndTemplate.ls";

extern iCoreShell*	g_pCoreShell;

/********************************************************************
/*						class: TLQuestionWindow
*********************************************************************/

TLQuestionWindow::TLQuestionWindow( const String& type, const String& name )
: StaticImage(type, name)
{
	m_mainWindow = NULL;
	m_IEWindow = NULL;
	m_InputBox = NULL;
	m_BtnOK = NULL;
	m_txtTimeShow = NULL;
	m_parent = NULL;
	m_root = NULL;
	m_txtTimeTitle = NULL;
	m_txtTimeUnit = NULL;

	m_totalSeconds = 30;
	m_currentSeconds = 0;
	m_oldTime = 0;

	addEvent( EventAnswerTextChanged );
	addEvent( EventAnswerCommitted );
	addEvent( EventAnserTimeUp );
}

TLQuestionWindow::~TLQuestionWindow()
{

}

void TLQuestionWindow::initialise()
{
	m_mainWindow = WindowManager::getSingleton().loadWindowLayout( TemplateLayoutsFileName , getName() + "__MainWindow__", "", NULL, NULL, true );
	addChildWindow( m_mainWindow );

	m_sMainWindowName = m_mainWindow->getName();
	m_mainWindow->setPosition( Absolute, Point( 0, 0 ) );

	m_txtTimeShow = static_cast< TLStaticText* >( m_mainWindow->getChild( m_sMainWindowName + "/txtTime" ) );

	m_txtShow =  static_cast< TLStaticText* >( m_mainWindow->getChild( m_sMainWindowName + "/IEText" ) );
	if ( m_txtShow )
	{
		m_txtShow->useLayout();
	}

	m_txtPlusShow = static_cast< TLStaticText* >( m_mainWindow->getChild( m_sMainWindowName + "/IETextPlus" ) );
	if ( m_txtPlusShow )
	{
		m_txtPlusShow->useLayout();
	}
	
	m_IEWindow = static_cast< TLStaticImage* >( m_mainWindow->getChild( m_sMainWindowName + "/IEWindow" ) );
	bool bDisableInput = ( KUiCfgLoader::getSingleton().getQuestionWindowCfg().DisableInput != 0 );
	m_InputBox = static_cast< TLEditbox* >( m_mainWindow->getChild( m_sMainWindowName + "/edtAnswer" ) );
	m_InputBox->subscribeEvent( 
		TLEditbox::EventTextChanged,
		Event::Subscriber( &TLQuestionWindow::InputBox_TextChanged, this ) );

	m_InputBox->subscribeEvent( 
		TLEditbox::EventKeyDown,
		Event::Subscriber( &TLQuestionWindow::InputBox_KeyDown, this ) );

	m_BtnOK = static_cast< TLButton* >( m_mainWindow->getChild( m_sMainWindowName + "/btnOK" ) );
	m_BtnOK->subscribeEvent(
		TLButton::EventClicked,
		Event::Subscriber( &TLQuestionWindow::btnOK_Clicked, this ) );

	m_totalSeconds	= KUiCfgLoader::getSingleton().getQuestionWindowCfg().TotalTime;
	m_cfgQuestionFilePath = KUiCfgLoader::getSingleton().getQuestionWindowCfg().QuestionFilePath;

	m_txtTimeTitle = static_cast< TLStaticText* >( m_mainWindow->getChild( m_sMainWindowName + "/txtTimeTitle" ) );
	m_txtTimeUnit  = static_cast< TLStaticText* >( m_mainWindow->getChild( m_sMainWindowName + "/txtTimeUnit" ) );

}

void TLQuestionWindow::onShown( WindowEventArgs& e )
{
	StaticImage::onShown( e );
	if ( NULL != m_InputBox )
	{
		m_InputBox->setText( "" );
		m_InputBox->activate();

#ifdef __QUESTION_WND_TEST
 		int k = g_Random( 9999 );
 		m_InputBox->setText( iToString( k ) );
 		sendAnswer();
#endif
	}

	if ( m_totalSeconds > 0 )
	{
#ifndef __QUESTION_WND_TEST
		beginUpdate();
#endif
	}
}

void TLQuestionWindow::onHidden( WindowEventArgs& e )
{
	StaticImage::onHidden( e );
	if ( NULL != m_InputBox )
	{
		m_InputBox->deactivate();
	}
	stopUpdate();
}


bool CEGUI::TLQuestionWindow::InputBox_TextChanged( const CEGUI::EventArgs& e )
{
	CEGUI::EventArgs args_textchanged = e;
	fireEvent( EventAnswerTextChanged, args_textchanged );
	return true;
}

bool CEGUI::TLQuestionWindow::InputBox_KeyDown( const CEGUI::EventArgs& e )
{
	Key::Scan inputCode = static_cast< const KeyEventArgs& >( e ).scancode;
	switch ( inputCode )
    {
    case Key::Return: case Key::NumpadEnter:
		return sendAnswer();
        break;
		
    default:
        return false;
    }
	
    return true;	
}

bool CEGUI::TLQuestionWindow::btnOK_Clicked( const CEGUI::EventArgs& e )
{	
	return sendAnswer();
}

void CEGUI::TLQuestionWindow::injectTimePulse( DWORD curUpdateTime )
{
	DWORD curTime = ::GetTickCount();
	
	if ( curTime - m_oldTime >= 1000 )
	{	
		m_currentSeconds ++;

		if ( m_currentSeconds > m_totalSeconds )
		{
			m_currentSeconds = m_totalSeconds;
		}

		if ( NULL != m_txtTimeShow )
		{
			m_txtTimeShow->setText( iToString( m_totalSeconds - m_currentSeconds ) );
		}
		
		if ( m_currentSeconds >= m_totalSeconds )
		{
			//hideParent();
			
			CEGUI::EventArgs args_timeup;
			fireEvent( EventAnserTimeUp,  args_timeup );
			stopUpdate();
		}

		m_oldTime = curTime;
	}
}

bool CEGUI::TLQuestionWindow::RefreshFile( unsigned int uParam )
{
	if ( m_mainWindow )
	{
		m_parent = m_mainWindow->getParent();
	}

	UIQuestionData* questionData = (UIQuestionData*) uParam;
	if ( questionData == NULL ||
		m_txtShow == NULL ||
		m_IEWindow == NULL || 
		m_parent == NULL )
	{
		return false;
	}
	int nLen = questionData->TextDataLength < UI_QUESTION_BUFF_SIZE -1  ? questionData->TextDataLength : UI_QUESTION_BUFF_SIZE -1;
	if ( nLen <= 0)
	{
		return false;
	}
	if ( ImagesetManager::getSingleton().isImagesetPresent("question_haha") )
		ImagesetManager::getSingleton().destroyImageset("question_haha");
	ImagesetManager::getSingleton().createImagesetFromMemory("question_haha", questionData->ImgData );
	m_IEWindow->setImage( "question_haha", "full_image");
	int w = ImagesetManager::getSingleton().getImageset("question_haha")->getImage("full_image").getWidth();
	int h = ImagesetManager::getSingleton().getImageset("question_haha")->getImage("full_image").getHeight();
	m_IEWindow->setSize( Absolute, Size(w, h) );

	int pW = m_parent->getWidth(Absolute);
	int newX = (pW - w) / 2;
	int newY = m_IEWindow->getAbsoluteYPosition();
	m_IEWindow->setPosition(Absolute,Point(newX, newY));

	questionData->TextData[nLen] = 0;
	m_txtShow->getLayout()->SetText(questionData->TextData);
	m_txtShow->getLayout()->flashLayout();

	questionData->AppendDescStrId[UI_QUESTION_BUFF_SIZE -1] = 0;
	m_txtPlusShow->getLayout()->SetText(questionData->AppendDescStrId);
	m_txtPlusShow->getLayout()->flashLayout();
	
	return true;
}

bool CEGUI::TLQuestionWindow::sendAnswer( bool isEmpty )
{
	if ( NULL == m_InputBox )
	{
		return false;
	}
	
	CEGUI::String answer = m_InputBox->getText();
	utf8* answer_utf8 = ( utf8* )( answer.c_str() );
	
	utf8* temp_pUTF8 = answer_utf8;
	
	int utf8_buffsize = 0;
	while ( *temp_pUTF8++ )
		utf8_buffsize++;
	
	const UTF8** sourceStart = ( const UTF8** )( &answer_utf8 );
	const UTF8* sourceEnd = ( const UTF8* )( answer_utf8 + utf8_buffsize );
	
	UTF16 target_utf16[ COMMON_CLIENT_MSG_LEN_128 ] = { 0 };
	UTF16* pTarget = target_utf16;
	UTF16** targetStart = ( UTF16** )( &pTarget );
	UTF16* targetEnd = target_utf16 + COMMON_CLIENT_MSG_LEN_128;
	
	ConversionResult cr = ConvertUTF8toUTF16( sourceStart, sourceEnd, targetStart, targetEnd, strictConversion );
	if ( conversionOK != cr )
	{
		m_InputBox->activate();
		return false;
	}
	
	UTF16* temp_pUTF16 = target_utf16;
	int utf16_buffsize = 0;
	while ( *temp_pUTF16++ )
		utf16_buffsize++;
	
	if ( utf16_buffsize <= 0 )
	{
		m_InputBox->activate();
		return false;
	}
	
	UTF16* pTarget_param = target_utf16;

	if ( isEmpty )
	{
		g_pCoreShell->OperationRequest( GOI_ANSWER_QUESTION, ( unsigned int )( pTarget_param ), 0 );
	}
	else
	{
		g_pCoreShell->OperationRequest( GOI_ANSWER_QUESTION, ( unsigned int )( pTarget_param ), utf16_buffsize * sizeof( UTF16 ) );
	}
	
	
	CEGUI::EventArgs args_answerCommitted;
	fireEvent( EventAnswerCommitted, args_answerCommitted );
	
	return true;
}

void CEGUI::TLQuestionWindow::setTotalTime( int totalTime )
{
	m_totalSeconds = totalTime; 
	m_currentSeconds = 0;

	if ( m_totalSeconds <= 0 )
	{
		//不使用倒计时
		setCountDownVisible( false );
	}
	else
	{
		setCountDownVisible( true );
	}
}

void CEGUI::TLQuestionWindow::setCountDownVisible( bool visible )
{
	if ( ( NULL != m_txtTimeShow ) && ( NULL != m_txtTimeUnit ) && ( NULL != m_txtTimeTitle ) )
	{
		m_txtTimeShow->setVisible( visible );
		m_txtTimeUnit->setVisible( visible );
		m_txtTimeTitle->setVisible( visible );
	}
}

void CEGUI::TLQuestionWindow::SendEmptyAnswer()
{
	m_InputBox->setText( "empty" );
	sendAnswer( true );
}

/********************************************************************
/*						class: TLQuestionWindowFactory
*********************************************************************/

Window* TLQuestionWindowFactory::createWindow( const String& name )
{
	return new TLQuestionWindow( d_type, name );	
}
