//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 04/22/2008
//      File_base        : UiBattleResult
//      File_ext         : cpp
//      Author           : DarkMagic(DuanMu)
//      Description      : 战场统计
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "Ui/UiCase/UiBattleResult.h"
#include "GameDataDef.h"
#include "CoreShell.h"
#include "../KMessageCentre.h"

extern iCoreShell * g_pCoreShell;

template<> 
KUiBattleResult * KUiWndSingleton<KUiBattleResult>::ms_Singleton = NULL;

KUiBattleResult::KUiBattleResult(const CEGUI::String & strPath)
: KUiWndSingleton<KUiBattleResult>(strPath)
{
	m_pListPanel = NULL;
	m_pCloseBnt = NULL;
}

KUiBattleResult::~KUiBattleResult()
{

}

void KUiBattleResult::onCreate(UIMDLEvent& rEvent)
{

}

void KUiBattleResult::onChange(UIMDLEvent& rEvent)
{

}

void KUiBattleResult::onRelease(UIMDLEvent& rEvent)
{

}

void KUiBattleResult::Init()
{

#ifndef _DEBUG
	try
#endif
	{
		m_pCloseBnt = static_cast<TLButton *>(m_pThisWnd->getChild("TaharezLook/BattleResult/Close"));
		m_pListPanel = static_cast<TLStaticImage *>(m_pThisWnd->getChild("TaharezLook/BattleResult/Panel"));
		m_pCloseBnt->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiBattleResult::OnCloseBnt, this));
	}
#ifndef _DEBUG
	catch (...)
	{
		ms_Singleton = NULL;
		return;
	}
#endif
}

bool KUiBattleResult::OnCloseBnt(const CEGUI::EventArgs & args)
{
	if (ms_Singleton != NULL)
	{
		m_pThisWnd->hide();
		return true;
	}
	return false;
}

void KUiBattleResult::Show()
{
	if (!g_pCoreShell->GetGameData( GDI_IS_PLAYER_IN_COMBAT_WORLD, NULL, NULL ))
	{
		return;
	}

	if (ms_Singleton != NULL)
	{
		char _camp[COMMON_CLIENT_MSG_LEN_128];
		g_pCoreShell->GetGameData(GOI_GET_SELF_COMBAT_ORG_NAME,(unsigned int)_camp, 0);
		_camp[COMMON_CLIENT_MSG_LEN_128 - 1];
#ifndef _DEBUG
		try
#endif
		{
			TLStaticText * _titleText = static_cast<TLStaticText *>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/BattleResult/CampText"));
			_titleText->setText(AnsiToUtf8(_camp));
		}
#ifndef _DEBUG
		catch (...)
		{
			KUiWndSingleton<KUiBattleResult>::Hide();
			return;
		}
#endif

		char _mainPath[COMMON_CLIENT_MSG_LEN_128];
		char _particularPath[COMMON_CLIENT_MSG_LEN_128];
		char _strAttribute[COMMON_CLIENT_MSG_LEN_32];
		TLStaticText * _pAttribute = NULL;
		TLStaticImage * _pItem = NULL;

		for (int i = 0; i < UI_BATTLE_RESULT_MAX_LIST_NUM; i++)
		{
			if (ms_Singleton->m_TopPlayerInfo[i].Name[0] != 0)
			{
				_mainPath[0] = 0;
				sprintf(_mainPath, "TaharezLook/BattleResult/Panel/Item%d", i);
				_mainPath[COMMON_CLIENT_MSG_LEN_128 - 1] = 0;
#ifndef _DEBUG
				try
#endif
				{
					_pItem = static_cast<TLStaticImage *>(ms_Singleton->m_pListPanel->getChild(_mainPath));	
				}
#ifndef _DEBUG
				catch (...)
				{
					KUiWndSingleton<KUiBattleResult>::Hide();
					return;
				}
#endif

				_strAttribute[0] = 0;
				_particularPath[0] = 0;
				sprintf(_particularPath, "%s/Name", _mainPath);
				_particularPath[COMMON_CLIENT_MSG_LEN_128 - 1] = 0;
#ifndef _DEBUG
				try
#endif
				{
					_pAttribute = static_cast<TLStaticText *>(_pItem->getChild(_particularPath));
					_pAttribute->setText(AnsiToUtf8(ms_Singleton->m_TopPlayerInfo[i].Name));
				}
#ifndef _DEBUG
				catch (...)
				{
					KUiWndSingleton<KUiBattleResult>::Hide();
					return;
				}
#endif

				_strAttribute[0] = 0;
				_particularPath[0] = 0;
				sprintf(_particularPath, "%s/Level", _mainPath);
				_particularPath[COMMON_CLIENT_MSG_LEN_128 - 1] = 0;
#ifndef _DEBUG
				try
#endif
				{
					_pAttribute = static_cast<TLStaticText *>(_pItem->getChild(_particularPath));
					int _level = 0;
					_level = ms_Singleton->m_TopPlayerInfo[i].Level;
					sprintf(_strAttribute, "%d", _level);
					_pAttribute->setText(AnsiToUtf8(_strAttribute));
				}
#ifndef _DEBUG
				catch (...)
				{
					KUiWndSingleton<KUiBattleResult>::Hide();
					return;
				}
#endif

				_strAttribute[0] = 0;
				_particularPath[0] = 0;
				sprintf(_particularPath, "%s/Score", _mainPath);
				_particularPath[COMMON_CLIENT_MSG_LEN_128 - 1] = 0;
#ifndef _DEBUG
				try
#endif
				{
					_pAttribute = static_cast<TLStaticText *>(_pItem->getChild(_particularPath));
					int _score = 0;
					_score = ms_Singleton->m_TopPlayerInfo[i].Score;
					sprintf(_strAttribute, "%d", _score);
					_pAttribute->setText(AnsiToUtf8(_strAttribute));
				}
#ifndef _DEBUG
				catch (...)
				{
					KUiWndSingleton<KUiBattleResult>::Hide();
					return;
				}
#endif				
				_strAttribute[0] = 0;
				_particularPath[0] = 0;
			
				char skillMetire = ms_Singleton->m_TopPlayerInfo[i].SkillSeries;
				char metire = ms_Singleton->m_TopPlayerInfo[i].Class;
				switch (metire)
				{
				case 0:
					{
						switch (skillMetire)
						{
						case -1:
							{
								sprintf(_strAttribute, ROLE_CAREER_JS);
							}
							break;
						case 0:
							{
								sprintf(_strAttribute, ROLE_CAREER_JS_1);
							}
							break;
						case 1:
							{
								sprintf(_strAttribute, ROLE_CAREER_JS_0);
							}
							break;
						default:
							{
								sprintf(_strAttribute, "");
							}
							break;
						}
					}
					break;
				case 1:
					{
						switch (skillMetire)
						{
						case -1:
							{
								sprintf(_strAttribute, ROLE_CAREER_DS);
							}
							break;
						case 0:
							{
								sprintf(_strAttribute, ROLE_CAREER_DS_1);
							}
							break;
						case 1:
							{
								sprintf(_strAttribute, ROLE_CAREER_DS_0);
							}
							break;
						default:
							{
								sprintf(_strAttribute, "");
							}
							break;
						}
					}
					break;
				case 2:
					{
						switch (skillMetire)
						{
						case -1:
							{
								sprintf(_strAttribute, ROLE_CAREER_YR);
							}
							break;
						case 0:
							{
								sprintf(_strAttribute, ROLE_CAREER_YR_0);
							}
							break;
						case 1:
							{
								sprintf(_strAttribute, ROLE_CAREER_YR_1);
							}
							break;
						default:
							{
								sprintf(_strAttribute, "");
							}
							break;
						}
					}
					break;
				default:
					{
						sprintf(_strAttribute, "");
					}
					break;
				}
				sprintf(_particularPath, "%s/Class", _mainPath);
				_particularPath[COMMON_CLIENT_MSG_LEN_128 - 1] = 0;
#ifndef _DEBUG
				try
#endif
				{
					_pAttribute = static_cast<TLStaticText *>(_pItem->getChild(_particularPath));
					_pAttribute->setText(AnsiToUtf8(_strAttribute));
				}
#ifndef _DEBUG
				catch (...)
				{
					KUiWndSingleton<KUiBattleResult>::Hide();
					return;
				}
#endif	
			}
			else
			{
				_mainPath[0] = 0;
				sprintf(_mainPath, "TaharezLook/BattleResult/Panel/Item%d", i);
				_mainPath[COMMON_CLIENT_MSG_LEN_128 - 1] = 0;
#ifndef _DEBUG
				try
#endif
				{
					_pItem = static_cast<TLStaticImage *>(ms_Singleton->m_pListPanel->getChild(_mainPath));	
				}
#ifndef _DEBUG
				catch (...)
				{
					KUiWndSingleton<KUiBattleResult>::Hide();
					return;
				}
#endif

				_particularPath[0] = 0;
				sprintf(_particularPath, "%s/Name", _mainPath);
				_particularPath[COMMON_CLIENT_MSG_LEN_128 - 1] = 0;
#ifndef _DEBUG
				try
#endif
				{
					_pAttribute = static_cast<TLStaticText *>(_pItem->getChild(_particularPath));
					_pAttribute->setText(AnsiToUtf8(""));
				}
#ifndef _DEBUG
				catch (...)
				{
					KUiWndSingleton<KUiBattleResult>::Hide();
					return;
				}
#endif

				_particularPath[0] = 0;
				sprintf(_particularPath, "%s/Level", _mainPath);
				_particularPath[COMMON_CLIENT_MSG_LEN_128 - 1] = 0;
#ifndef _DEBUG
				try
#endif
				{
					_pAttribute = static_cast<TLStaticText *>(_pItem->getChild(_particularPath));
					_pAttribute->setText(AnsiToUtf8(""));
				}
#ifndef _DEBUG
				catch (...)
				{
					KUiWndSingleton<KUiBattleResult>::Hide();
					return;
				}
#endif

				_particularPath[0] = 0;
				sprintf(_particularPath, "%s/Score", _mainPath);
				_particularPath[COMMON_CLIENT_MSG_LEN_128 - 1] = 0;
#ifndef _DEBUG
				try
#endif
				{
					_pAttribute = static_cast<TLStaticText *>(_pItem->getChild(_particularPath));
					_pAttribute->setText(AnsiToUtf8(""));
				}
#ifndef _DEBUG
				catch (...)
				{
					KUiWndSingleton<KUiBattleResult>::Hide();
					return;
				}
#endif

				_particularPath[0] = 0;
				sprintf(_particularPath, "%s/Class", _mainPath);
				_particularPath[COMMON_CLIENT_MSG_LEN_128 - 1] = 0;
#ifndef _DEBUG	
				try
#endif
				{
					_pAttribute = static_cast<TLStaticText *>(_pItem->getChild(_particularPath));
					_pAttribute->setText(AnsiToUtf8(""));
				}
#ifndef _DEBUG
				catch (...)
				{
					KUiWndSingleton<KUiBattleResult>::Hide();
					return;
				}
#endif
			}
		}
	}
	KUiWndSingleton<KUiBattleResult>::Show();
}

void KUiBattleResult::Hide()
{
	if (ms_Singleton != NULL)
	{
		KUiWndSingleton<KUiBattleResult>::Hide();
	}
}

void KUiBattleResult::RefreshList(UICombatTopMemberInfo * playerInfo, int num)
{
	if (playerInfo == NULL)
	{
		return;
	}

	if (ms_Singleton == NULL)
	{
		return;
	}

	for (int i = 0; i < num; i++)
	{
		memcpy(&m_TopPlayerInfo[i], &playerInfo[i], sizeof(UICombatTopMemberInfo));
	}
	m_iInfoNum = num;
}

bool KUiBattleResult::OnNameListClick(const CEGUI::EventArgs & args)
{
	return true;
}

bool KUiBattleResult::OnMouseHover(const CEGUI::EventArgs & args)
{
	return true;
}



//////////////////////////////////////////////////////////////////////////
///					KUiSmallBattleFieldResult
//////////////////////////////////////////////////////////////////////////
template<> 
KUiSmallBattleFieldResult * KUiWndSingleton<KUiSmallBattleFieldResult>::ms_Singleton = NULL;

KUiSmallBattleFieldResult::KUiSmallBattleFieldResult( const CEGUI::String& strPath )
: KUiWndSingleton<KUiSmallBattleFieldResult>(strPath)
{
	m_pTime		= NULL;
	m_pScore	= NULL;
	m_pRepute	= NULL;
}

KUiSmallBattleFieldResult::~KUiSmallBattleFieldResult()
{
	
}

bool KUiSmallBattleFieldResult::btnClose_MouseClick( const CEGUI::EventArgs& args )
{
	Hide();
	return true;
}

void KUiSmallBattleFieldResult::Init()
{
	if ( ( NULL != ms_Singleton ) && ( NULL != ms_Singleton->m_pThisWnd ) )
	{
		m_pTime		= static_cast< TLStaticText* >( m_pThisWnd->getChild( "TaharezLook/SmallBattleFieldResult/Time" ) );
		m_pScore	= static_cast< TLStaticText* >( m_pThisWnd->getChild( "TaharezLook/SmallBattleFieldResult/Score" ) );
		m_pRepute	= static_cast< TLStaticText* >( m_pThisWnd->getChild( "TaharezLook/SmallBattleFieldResult/Repute" ) );

		m_pScore->setText( "" );
		m_pScore->useLayout();

		m_pThisWnd->getChild( "TaharezLook/SmallBattleFieldResult/btnClose" )->subscribeEvent(
			PushButton::EventMouseClick, 
			Event::Subscriber( &KUiSmallBattleFieldResult::btnClose_MouseClick, this ) );

		m_pThisWnd->getChild( "TaharezLook/SmallBattleFieldResult/btnOK" )->subscribeEvent(
			PushButton::EventMouseClick, 
			Event::Subscriber( &KUiSmallBattleFieldResult::btnClose_MouseClick, this ) );

		scoreDes_win	= KMessageCentre::GetMessage( battlefield_message, 1 );
		scoreDes_lose	= KMessageCentre::GetMessage( battlefield_message, 2 );
		scoreDes_draw	= KMessageCentre::GetMessage( battlefield_message, 3 );
	}
}

void KUiSmallBattleFieldResult::ShowResult( SMALL_BATTLE_FIELD_RESULT* pResult )
{
//	int selfScore = 1, emenyScore = 1, timeMinute = 45, repute = 10;

	m_pTime->setText( iToString( pResult->PersistTime / 60 ) );
	m_pRepute->setText( iToString( pResult->Repute ) );

	string strLayout = KMessageCentre::GetMessage( battlefield_message, 0 );
	char scoreShow[ COMMON_CLIENT_MSG_LEN_512 ] = { 0 };
	_snprintf( 
		scoreShow, 
		sizeof( scoreShow ), 
		strLayout.c_str(), 
		pResult->SelfScore, 
		pResult->EnemyScore, 
		getResultDes( pResult->SelfScore, pResult->EnemyScore ).c_str() );
	
	m_pScore->getLayout()->formatText( scoreShow );
	m_pScore->getLayout()->SetText( scoreShow );
	m_pScore->getLayout()->flashLayout();

	Show();
}

string& KUiSmallBattleFieldResult::getResultDes( int selfScore, int emenyScore )
{
	if ( selfScore > emenyScore )
	{
		return scoreDes_win;
	}
	else if ( selfScore < emenyScore )
	{
		return scoreDes_lose;
	}
	else
	{
		return scoreDes_draw;
	}
	
	return scoreDes_draw;
}

void KUiSmallBattleFieldResult::Show()
{
	KUiWndSingleton<KUiSmallBattleFieldResult>::Show();
}