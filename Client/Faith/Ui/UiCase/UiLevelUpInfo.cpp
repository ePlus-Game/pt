//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 03/27/2007 9:50
//      File_base        : UiLevelUpInfo
//      File_ext         : cpp
//      Author           : Lucien (LIU Siliang)
//      Description      : 文件功能描述 登陆等待
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "UiLevelUpInfo.h"
#include "..\KMessageCentre.h"
#include "CoreShell.h"
#include "UiLevelUp.h"

extern iCoreShell*		g_pCoreShell;
const int				g_coefficient = 1024;

template<>
KUiLevelUpInfo* KUiWndSingleton<KUiLevelUpInfo>::ms_Singleton = NULL;

KUiLevelUpInfo::KUiLevelUpInfo( const CEGUI::String& id_name )
: KUiWndSingleton<KUiLevelUpInfo>( id_name )
, d_AttributeText(NULL)
, d_tipText(NULL)
{
}

KUiLevelUpInfo::~KUiLevelUpInfo()
{

}

void KUiLevelUpInfo::Show()
{
	KUiWndSingleton<KUiLevelUpInfo>::Show();
}

void KUiLevelUpInfo::Init()
{		
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		d_AttributeText	= (TLStaticText*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/LevelUpInfo/LevelUpInfo");
		d_tipText		= (TLStaticText*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/LevelUpInfo/Prompt");
		
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/LevelUpInfo/Close")->subscribeEvent( PushButton::EventMouseClick, Event::Subscriber(&KUiLevelUpInfo::handleClose, ms_Singleton) );
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/LevelUpInfo/Quit")->subscribeEvent( PushButton::EventMouseClick, Event::Subscriber(&KUiLevelUpInfo::handleClose, ms_Singleton) );
		PushButton* pCloseButton = (PushButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/LevelUpInfo/Quit");
		if(pCloseButton)
			pCloseButton->hide();
	}
}
bool KUiLevelUpInfo::handleClose( const CEGUI::EventArgs& args )	
{
	m_pThisWnd->hide();
	return true;
}

void KUiLevelUpInfo::GetLevelUpInfo(const LevelUpAdd* plevelupAdd)
{
	char text[MAX_TEXT_LEN];
	char attributeInfo[MAX_TEXT_LEN];
	char tipInfo[MAX_TEXT_LEN];
	ZeroMemory(text, MAX_TEXT_LEN);
	ZeroMemory(attributeInfo, MAX_TEXT_LEN);
	ZeroMemory(tipInfo, MAX_TEXT_LEN);
	
	KUiPlayerBaseInfo		playBaseInfo;
	//KUiPlayerRuntimeInfo	playInfoRuntime;
	KUiPlayerAttribute		playInfoAttribute;
	g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (unsigned int)&playBaseInfo, 0);
	//g_pCoreShell->GetGameData(GDI_PLAYER_RT_INFO, (unsigned int)&playInfoRuntime, 0);
	g_pCoreShell->GetGameData(GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&playInfoAttribute, 0);
	
	KUiLevelUp::GetSingletonPtr()->GetPlayerInfo(playBaseInfo.Name, playInfoAttribute.nLevel+1);
	
	const KUiCfgLoader::LevelUpData& levelUpCfg = KUiCfgLoader::getSingleton().getLevelUpData();
	// 新等级任务属性
	// 如果有需求的话排版
	sprintf(attributeInfo, "<Layout width=%d><Seg text-align=center float=wrap><Obj color=%s font-family=%s>%s%d->%d</Obj></Seg>", 
							levelUpCfg.maxWidth, levelUpCfg.TitleColor, levelUpCfg.TitleFont,
							KMessageCentre::GetMessage( levelup_info, 4 ), playInfoAttribute.nLevel, playInfoAttribute.nLevel+1 );

	strcat( attributeInfo, "<Seg text-align=left float=wrap>" );
	ZeroMemory( text, MAX_TEXT_LEN );
	sprintf( text, "%s %d -> %d ", KMessageCentre::GetMessage(levelup_info, 5), playInfoAttribute.nBody - plevelupAdd->Attribute[0], playInfoAttribute.nBody );
	PrintText( levelUpCfg.NormalColor, levelUpCfg.NormalFont, text, attributeInfo );
	ZeroMemory( text, MAX_TEXT_LEN );
	sprintf( text, "(%d)   ", plevelupAdd->Attribute[0] );
	PrintText( levelUpCfg.SpecialColor, levelUpCfg.SpecialFont, text, attributeInfo );
	/*strcat( attributeInfo, "</Seg>" );

	strcat( attributeInfo, "<Seg text-align=left float=wrap>" );//*/
	ZeroMemory( text, MAX_TEXT_LEN );
	sprintf( text, "%s %d -> %d ", KMessageCentre::GetMessage(levelup_info, 6), playInfoAttribute.nNimbus - plevelupAdd->Attribute[1], playInfoAttribute.nNimbus );
	PrintText( levelUpCfg.NormalColor, levelUpCfg.NormalFont, text, attributeInfo );
	ZeroMemory( text, MAX_TEXT_LEN );
	sprintf( text, "(%d)", plevelupAdd->Attribute[1] );
	PrintText( levelUpCfg.SpecialColor, levelUpCfg.SpecialFont, text, attributeInfo );
	strcat( attributeInfo, "</Seg>" );

	strcat( attributeInfo, "<Seg text-align=left float=wrap>" );
	ZeroMemory( text, MAX_TEXT_LEN );
	sprintf( text, "%s %d -> %d ", KMessageCentre::GetMessage(levelup_info, 7), (playInfoAttribute.nStrength - plevelupAdd->Attribute[2])/g_coefficient, (playInfoAttribute.nStrength)/g_coefficient );
	PrintText( levelUpCfg.NormalColor, levelUpCfg.NormalFont, text, attributeInfo );
	ZeroMemory( text, MAX_TEXT_LEN );
	sprintf( text, "(%d)   ", plevelupAdd->Attribute[2]/g_coefficient );
	PrintText( levelUpCfg.SpecialColor, levelUpCfg.SpecialFont, text, attributeInfo );
	/*strcat( attributeInfo, "</Seg>" );

	strcat( attributeInfo, "<Seg text-align=left float=wrap>" );//*/
	ZeroMemory( text, MAX_TEXT_LEN );
	sprintf( text, "%s %d -> %d ", KMessageCentre::GetMessage(levelup_info, 8), (playInfoAttribute.nArt - plevelupAdd->Attribute[3])/g_coefficient, (playInfoAttribute.nArt)/g_coefficient );
	PrintText( levelUpCfg.NormalColor, levelUpCfg.NormalFont, text, attributeInfo );
	ZeroMemory( text, MAX_TEXT_LEN );
	sprintf( text, "(%d)", plevelupAdd->Attribute[3]/g_coefficient );
	PrintText( levelUpCfg.SpecialColor, levelUpCfg.SpecialFont, text, attributeInfo );
	strcat( attributeInfo, "</Seg>" );
	
	strcat( attributeInfo, "</Layout>" );
	
	d_AttributeText->useLayout();
	d_AttributeText->getLayout()->SetText( attributeInfo );// (char*)AnsiToUtf8(attributeInfo) );
	
	// 新等级任务信息提示
	sprintf(tipInfo, "<Layout width=%d><Seg text-align=center float=wrap><Obj color=%s font-family=%s>%s</Obj></Seg>", 
					  levelUpCfg.maxWidth, levelUpCfg.TitleColor, levelUpCfg.TitleFont,
					  KMessageCentre::GetMessage( levelup_info, 9 ) );
	
	strcat( tipInfo, "<Seg text-align=left float=wrap>" );

	if ( strcmp( levelUpCfg.TipColor, "" ) )
		PrintText( levelUpCfg.TipColor, levelUpCfg.TipFont, plevelupAdd->Tip, tipInfo );
	else if ( *(plevelupAdd->Tip) == '<' )
		strcat( tipInfo, plevelupAdd->Tip );
	else
		PrintText( "255,255,255", "stzhongs-10", plevelupAdd->Tip, tipInfo );
	strcat( tipInfo, "</Seg>" );

	strcat( tipInfo, "</Layout>" );

	d_tipText->useLayout();
	d_tipText->getLayout()->SetText( (char*)AnsiToUtf8(tipInfo) );
	d_tipText->getLayout()->flashLayout();

	/*sprintf(attributeInfo, "<Layout width=250><Seg text-align=center float=wrap><Obj color=255,0,255 font-family=stzhongs-12>%s%d->%d</Obj></Seg>", 
							KMessageCentre::GetMessage( levelup_info, 4 ), playInfoAttribute.nLevel, playInfoAttribute.nLevel+1 );

	ZeroMemory( text, MAX_TEXT_LEN );
	sprintf( text, "%s %d -> %d (%d)", KMessageCentre::GetMessage(levelup_info, 5), playInfoAttribute.nBody - plevelupAdd->Attribute[0], playInfoAttribute.nBody, plevelupAdd->Attribute[0] );
	PrintText( "0,255,255", "stzhongs-10", text, attributeInfo );
	ZeroMemory( text, MAX_TEXT_LEN );
	sprintf( text, "%s %d -> %d (%d)", KMessageCentre::GetMessage(levelup_info, 6), playInfoAttribute.nNimbus - plevelupAdd->Attribute[1], playInfoAttribute.nNimbus, plevelupAdd->Attribute[1] );
	PrintText( "0,255,255", "stzhongs-10", text, attributeInfo );
	ZeroMemory( text, MAX_TEXT_LEN );
	sprintf( text, "%s %d -> %d (%d)", KMessageCentre::GetMessage(levelup_info, 7), (playInfoAttribute.nStrength - plevelupAdd->Attribute[2])/g_coefficient, (playInfoAttribute.nStrength)/g_coefficient, (plevelupAdd->Attribute[2])/g_coefficient );
	PrintText( "0,255,255", "stzhongs-10", text, attributeInfo );
	ZeroMemory( text, MAX_TEXT_LEN );
	sprintf( text, "%s %d -> %d (%d)", KMessageCentre::GetMessage(levelup_info, 8), (playInfoAttribute.nArt - plevelupAdd->Attribute[3])/g_coefficient, (playInfoAttribute.nArt)/g_coefficient, (plevelupAdd->Attribute[3])/g_coefficient );
	PrintText( "0,255,255", "stzhongs-10", text, attributeInfo );

	strcat( attributeInfo, "</Layout>" );
	
	d_AttributeText->useLayout();
	d_AttributeText->getLayout()->SetText(attributeInfo);
	
	// 新等级任务信息提示
	sprintf(tipInfo, "<Layout width=250><Seg text-align=center float=wrap><Obj color=255,0,255 font-family=stzhongs-12>%s</Obj></Seg><Seg text-align=left float=wrap><Obj color=0,255,255 font-family=stzhongs-10>%s</Obj></Seg></Layout>", 
					   KMessageCentre::GetMessage( levelup_info, 9 ), plevelupAdd->Tip);
//*/
	d_tipText->useLayout();
	d_tipText->getLayout()->SetText( tipInfo );
}

void KUiLevelUpInfo::PrintText( const char* color, const char* font, const char* sourceText, char* destText )
{
	strcat(	destText, "<Obj color=" );
	strcat( destText, color );
	strcat( destText, " font-family=" );
	strcat( destText, font );
	strcat( destText, ">");
	strcat( destText, sourceText );
	strcat(	destText, "</Obj>" );
}