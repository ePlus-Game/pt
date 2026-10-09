#include "kwin32.h"
#include "CoreShell.h"
#include "UiWorldCombatInfo.h"


extern iCoreShell* g_pCoreShell;

using namespace CEGUI;

template<> 
KUiWorldCombatInfo* KUiWndSingleton<KUiWorldCombatInfo>::ms_Singleton	= NULL;

/************************************************************************/
/*                                                                      */
/************************************************************************/
KUiWorldCombatInfo::KUiWorldCombatInfo( const CEGUI::String& id_name ):
KUiWndSingleton<KUiWorldCombatInfo>( id_name )
{
	
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
KUiWorldCombatInfo::~KUiWorldCombatInfo()
{
	
}

void KUiWorldCombatInfo::Init()
{
}

void	KUiWorldCombatInfo::Show( void )
{
	KUiWndSingleton<KUiWorldCombatInfo>::Show();
}

void    KUiWorldCombatInfo::Update(const WorldCombatUIParam * pInfo)
{
	if (ms_Singleton && pInfo)
	{	
		if (ms_Singleton->m_pThisWnd)
		{
			if (pInfo && pInfo->nInfoNum > 0)
			{
				for (int n = 0; n< pInfo->nInfoNum ; n ++)
				{
					char szChildName[256] = "";
					sprintf(szChildName,"TaharezLook/WorldCombatScore/Org%d",n);
					Window * pOrgChild = ms_Singleton->m_pThisWnd->getChild(szChildName);
					if (pOrgChild)
					{
						char szScoreInfo[128];
					    sprintf(szScoreInfo,"%d",pInfo->detail[n].nScore);

						pOrgChild->getChild(pOrgChild->getName() + "/Name")->setText(AnsiToUtf8(pInfo->detail[n].baseInfo.szOrgName));
						pOrgChild->getChild(pOrgChild->getName() + "/Score")->setText(AnsiToUtf8(szScoreInfo));
					}//endif

				}//end for n

			}//endif

		}//endif
		
	}//endif
}