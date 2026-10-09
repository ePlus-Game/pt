#include "KCore.h"
#include "KNpc.h"
#include "KMissle.h"
#include "KItem.h"
#include "KBuySell.h"
#include "KPlayer.h"
#include "iRepresentshell.h"
#include "KSubWorldSet.h"
#include "scene/KScenePlaceC.h"
#include "ImgRef.h"
#include "GameDataDef.h"
#include "KObjSet.h"
#include "KOption.h"
#include "KNpcSet.h"

#define SHOW_SPACE_HEIGHT 5

//--> Rocker 2004.7.30
namespace CoreDraw
{
	int		gCurrentPlayerPaintedNum = 0;
}
//<-- End

void	CoreDrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, int nParam)
{
	switch(uObjGenre)
	{
	case CGOG_NPC:
		if (uId > 0)
		{
			if ((nParam & IPOT_RL_INFRONTOF_ALL) == IPOT_RL_INFRONTOF_ALL)
			{
				int nHeight = Npc[uId].GetNpcPate();			
				//Npc[uId].PaintBlood(nHeight);	//冒血
				Npc[uId].GetCombatInfoShower().Draw(nHeight);
				
				int nnHeight = nHeight;
				if (nHeight == nnHeight)	//没有聊天信息时绘制人物信息
				{
					if (NpcSet.CheckShowLife() || uId == Player[CLIENT_PLAYER_INDEX].GetSelectNpc())
					{
						nHeight = Npc[uId].PaintLife(nnHeight, false);
					}

					if ( (NpcSet.CheckShowName() || uId == Player[CLIENT_PLAYER_INDEX].GetSelectNpc()) )
					{
						if (nnHeight != nHeight)	//有内力显示时
						{
							nHeight += SHOW_SPACE_HEIGHT;//好看
						}
						if (Player[CLIENT_PLAYER_INDEX].GetTargetNpc() == uId || uId == Player[CLIENT_PLAYER_INDEX].GetSelectNpc() )
							nHeight = Npc[uId].PaintInfo(nHeight, true );	//被选中的人名放大显示
						else
							nHeight = Npc[uId].PaintInfo(nHeight, false);
						nHeight += 0;
					}

					if(!NpcSet.CheckShowName())
					{
						if(Player[CLIENT_PLAYER_INDEX].GetTargetNpc() != uId && uId != Player[CLIENT_PLAYER_INDEX].GetSelectNpc())
							Npc[uId].HideInfo();
					}
				}

				if(Npc[uId].GetBubble())
				{
					// 绘制人物附近区域聊天框
					Npc[uId].PaintBubble();
				}
			}
			else if ((nParam & IPOT_RL_OBJECT) == IPOT_RL_OBJECT)
			{
				if(Npc[uId].IsPlayer())
				{
					//自己
					Npc[uId].Paint();
				}
				else if(Npc[uId].GetKind() == kind_player)
				{
					//其他玩家
					if(NpcSet.isShowPlayer())
					{
						Npc[uId].Paint();
					}
				}
				else
				{
					//NPC
					Npc[uId].Paint();
				}

				if(Npc[uId].GetBubble())
				{
					// 绘制人物附近区域聊天框
					Npc[uId].PaintBubble();
				}
			}			
		}
		break;
	case CGOG_NPC_UI:
		{
			Npc[uId].PaintUI(x, y);
		}
		break;
	case CGOG_MISSLE:
		if (uId > 0)
			Missle[uId].Paint();
		break;
	case CGOG_MENU_NPC:
		if (nParam)
		{
			((KNpcRes *)nParam)->SetPos(0, x + Width / 2, y + Height / 2 + 28, 0, FALSE, TRUE);
#define		STAND_TOTAL_FRAME	15
			int nFrame = g_SubWorldSet.m_nLoopRate % STAND_TOTAL_FRAME;
			((KNpcRes *)nParam)->Draw(0, 0, STAND_TOTAL_FRAME, nFrame, true);
		}
		break;
	
	case CGOG_NPC_BLUR_DETAIL(1):
		Npc[uId].GetNpcRes()->m_cNpcBlur.Draw(1);
		break;
	case CGOG_NPC_BLUR_DETAIL(2):
		Npc[uId].GetNpcRes()->m_cNpcBlur.Draw(2);
		break;
	case CGOG_NPC_BLUR_DETAIL(3):
		Npc[uId].GetNpcRes()->m_cNpcBlur.Draw(3);
		break;
	case CGOG_NPC_BLUR_DETAIL(4):
		Npc[uId].GetNpcRes()->m_cNpcBlur.Draw(4);
		break;
	case CGOG_NPC_BLUR_DETAIL(5):
		Npc[uId].GetNpcRes()->m_cNpcBlur.Draw(5);
		break;
	case CGOG_NPC_BLUR_DETAIL(6):
		Npc[uId].GetNpcRes()->m_cNpcBlur.Draw(6);
		break;
	case CGOG_NPC_BLUR_DETAIL(7):
		Npc[uId].GetNpcRes()->m_cNpcBlur.Draw(7);
		break;
	case CGOG_OBJECT:
		if (uId)
		{
			if ((nParam & IPOT_RL_INFRONTOF_ALL) == IPOT_RL_INFRONTOF_ALL)
			{
				
			}
			else 
			{
				Object[uId].Draw();
				if ((int)uId == Player[CLIENT_PLAYER_INDEX].GetSelectObj())
					Object[uId].DrawBorder();
				if (ObjSet.CheckShowName())
					Object[uId].DrawInfo();
			}
		}
		break;
	case CGOG_BUILDING: //建设时的图
		break;
	case CGOG_ICON_BUILDING: // 所有建筑物图标
		break;
	case CGOG_ICON_CAN_BUILD: // 可建设建筑物图标
		break;
	case CGOG_ICON_TOTEM: // 图腾图标
		break;
	case CGOG_ICON_PLANT:
		break;
	case CGOG_ICON_TERRAIN: // 地形图标
		break;
	default:
		break;
	}
}

