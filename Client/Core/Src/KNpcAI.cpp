#include "KCore.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "KMath.h"
#include "KObj.h"
#include "KPlayer.h"
#include "KNpcAI.h"
#ifndef _SERVER
#include "CoreShell.h"
#endif
#include "KSkills.h"

// flying add here, to use math lib
#include <math.h>
extern int GetRandomNumber(int nMin, int nMax);

#define		MAX_FOLLOW_DISTANCE		48
#define		FOLLOW_WALK_DISTANCE	100
enum
{
	enNear,
	enMiddle,
	enFar,
};


KNpcAI NpcAI;

KNpcAI::KNpcAI()
{
	m_nIndex = 0;
	m_bActivate = TRUE;
}

// flying modified this function.
// Jun.4.2003
void KNpcAI::Activate(int nIndex)
{
	m_nIndex = nIndex;
// lixuewu 2004.02.12
	if (Npc[m_nIndex].m_Kind == kind_creature || 
		Npc[m_nIndex].m_Kind == kind_producesrc) 
	{
		return;
	}
// lixuewu 2004.02.12
	if (Npc[m_nIndex].IsPlayer())
	{
		// 新增的Player AI在这里调用实现。
		ProcessPlayer();
		return;
	}
// 
// //==> Added By Rocker 2004.7.9
// #ifdef _SERVER
// 	// 让攻城器械也可以触发地图位置脚本
// 	if (Npc[m_nIndex].m_Kind == kind_siege_weapon)
// 	{
// 		TriggerMapTrap();
// 	}
// #endif
// //<== Added End
// 
// #ifdef _SERVER
// 	if (Npc[m_nIndex].m_CompAttrMgr[ncai_lifeuplimit] == 0)
// 		return;
// 	int nCurTime = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_dwCurrentTime;
// 	if (/*Npc[m_nIndex].m_nPeopleIdx ||*/Npc[m_nIndex].m_NextAITime <= nCurTime)
// 	{
// 		KNpc* pNpc = &Npc[Npc[m_nIndex].GetTargetNpc()];
// 		Npc[m_nIndex].m_NextAITime = nCurTime + Npc[m_nIndex].m_AIMAXTime;
// 		if (pNpc->m_Doing == do_death 
// 			|| pNpc->m_Doing == do_revive
// 			|| (pNpc->IsPlayer()))
// 			Npc[m_nIndex].SetTarget(type_npc, 0);
// 		switch(Npc[m_nIndex].m_AiMode)
// 		{
// 		case 1:
// 			ProcessAIType01();
// 			break;
// 		case 2:
// 			ProcessAIType02();
// 			break;
// 		case 3:
// 			ProcessAIType03();
// 			break;
// 		case 4:
// 			ProcessAIType04();
// 			break;
// 		case 5:
// 			ProcessAIType05();
// 			break;
// 		case 6:
// 			ProcessAIType06();
// 			break;
// 		case 7:
// 			ProcessAIType07();
// 			break;
// 		// lixuewu 2004.09.28
// 		case 10:
// 			ProcessBuildingAI01();
// 			break;
// /*		case 7:
// 			ProcessAIType7();
// 			break;
// 		case 8:
// 			ProcessAIType8();
// 			break;
// 		case 9:
// 			ProcessAIType9();
// 			break;
// 		case 10:
// 			ProcessAIType10();
// 			break;*/
// 		default:
// 			break;
// 		}
// 	}
// // flying add the code for the macro such as "_CLIENT".
// // because this code only run at client.
// #else
// 	int nCurTime = SubWorld[0].m_dwCurrentTime;
// 	
// 	if ( kind_pet == Npc[m_nIndex].m_Kind && Npc[m_nIndex].m_NextAITime <= nCurTime )
// 	{
// 		Npc[m_nIndex].m_NextAITime = nCurTime + (Npc[m_nIndex].m_AIMAXTime - Npc[m_nIndex].m_byPetHonor);
// 		ProcessPet();
// 	}
// 	else if (Npc[m_nIndex].m_Kind > 3 && Npc[m_nIndex].m_AiMode > 10)
// 	{	
// 		if (CanShowNpc())
// 		{
// 			// 让NPC随机休息一下，是个好建议。
// 			if (GetRandomNumber(0, 1))	
// 			{
// 				Npc[m_nIndex].m_AiParam[5] = 0;
// 				Npc[m_nIndex].m_AiParam[4] = 5;
// 				return;
// 			}
// 			if (!KeepActiveRange())
// 				ProcessShowNpc();
// 		}		
// 	}
// #endif
}
//---------------------------------------------------------------------
// flying add these functions
// Run at client.
#ifndef _SERVER
// 仅仅有画面效果的NPC
int KNpcAI::ProcessShowNpc()
{
    int nResult  = false;
    int nRetCode = false;

	switch (Npc[m_nIndex].m_AiMode)
	{
	// 飞鸟型
	case 11:
		nRetCode = ShowNpcType11();
        if (!nRetCode)
            goto Exit0;
		break;
	// 蜻蜓型
	case 12:
		nRetCode = ShowNpcType12();
        if (!nRetCode)
            goto Exit0;
		break;
	// 鱼类型
	case 13:
		nRetCode = ShowNpcType13();
        if (!nRetCode)
            goto Exit0;
		break;
	// 老鼠型
	case 14:
		nRetCode = ShowNpcType14();
        if (!nRetCode)
            goto Exit0;
		break;
	// 鸡犬型
	case 15:
		nRetCode = ShowNpcType15();
        if (!nRetCode)
            goto Exit0;
		break;
	// 兔子型
	case 16:
		nRetCode = ShowNpcType16();
        if (!nRetCode)
            goto Exit0;
		break;
	// 蝴蝶型
	case 17:
		nRetCode = ShowNpcType17();
        if (!nRetCode)
            goto Exit0;
		break;	
	default:
		break;
	}

    nResult = true;
Exit0:
	return nResult;
}

// 飞鸟型
int KNpcAI::ShowNpcType11()
{
    int nResult = false;
    int nRetCode = false;

	KNpc& aNpc = Npc[m_nIndex];
	// Go the distance between P1 to P2	
	int nDistance = 0;
	int nDesX = 0;
	int nDesY = 0;
	int nCurX = 0;
	int nCurY = 0;
	int nOffX = 0;
	int nOffY = 0;
	int nOffsetDir = 0;
	
	// 效果加强 随机调整高度
	aNpc.m_Height = GetRandomNumber(aNpc.m_AiParam[6] - 4, aNpc.m_AiParam[6]);

	aNpc.GetMpsPos(&nCurX, &nCurY);

	// 计算新角度 和 距离
	if (aNpc.m_AiParam[3] > 0)
		nOffsetDir = GetRandomNumber(aNpc.m_AiParam[3], aNpc.m_AiParam[2]);
	else
		nOffsetDir = aNpc.m_AiParam[2];
	
    if (GetRandomNumber(0, 1))
		nOffsetDir = -nOffsetDir;
	
    nDistance = GetRandomNumber(aNpc.m_AiParam[0] - aNpc.m_AiParam[1], aNpc.m_AiParam[0]);

	// 取得运动所需的时间，保存在参数表中
	if (aNpc.m_CompAttrMgr[ncai_walkspeed] > 0)
	{
		aNpc.m_AiParam[4] = (int) nDistance / aNpc.m_CompAttrMgr[ncai_walkspeed];
		aNpc.m_AiParam[5] = 0;
	}
	//if (KeepActiveShowRange())
	//	aNpc.m_Dir += 32;
	aNpc.m_UnaryAttrMgr.Set(nuai_dir, aNpc.m_UnaryAttrMgr[nuai_dir] + nOffsetDir);
	if (aNpc.m_UnaryAttrMgr[nuai_dir] < 0)
		aNpc.m_UnaryAttrMgr.Set(nuai_dir, aNpc.m_UnaryAttrMgr[nuai_dir] + 64);
	else
		aNpc.m_UnaryAttrMgr.Set(nuai_dir, aNpc.m_UnaryAttrMgr[nuai_dir] % 64);
	
    // 根据三角函数计算偏移的X、Y数值
	nRetCode = GetNpcMoveOffset(aNpc.m_UnaryAttrMgr[nuai_dir], nDistance, &nOffX, &nOffY);
    if (!nRetCode)
        goto Exit0;

	// 获取目标坐标
	nDesX = nCurX + nOffX;
	nDesY = nCurY + nOffY;
	//aNpc.SendCommand(do_walk, nDesX, nDesY);	
	aNpc.SendCommand(do_run, nDesX, nDesY);

    nResult = true;
Exit0:
	return nResult;
}

// 蜻蜓型
// done
int KNpcAI::ShowNpcType12()
{
    int nResult = false;
    int nRetCode = false;

	// Go the distance between P1 to P2	
	int nDistance = 0;
	int nDesX = 0;
	int nDesY = 0;
	int nCurX = 0;
	int nCurY = 0;
	int nOffX = 0;
	int nOffY = 0;
	int nOffsetDir = 0;
	KNpc& aNpc = Npc[m_nIndex];

	// 效果加强 随机调整高度
	aNpc.m_Height = GetRandomNumber(aNpc.m_AiParam[6] - 4, aNpc.m_AiParam[6]);

	aNpc.GetMpsPos(&nCurX, &nCurY);
		
	// 计算新角度 和 距离
	if (aNpc.m_AiParam[3] > 0)
		nOffsetDir = GetRandomNumber(aNpc.m_AiParam[3], aNpc.m_AiParam[2]);
	else
		nOffsetDir = aNpc.m_AiParam[2];
	if (GetRandomNumber(0, 1))
		nOffsetDir = -nOffsetDir;
	nDistance = GetRandomNumber(aNpc.m_AiParam[0] - aNpc.m_AiParam[1], aNpc.m_AiParam[0]);

	// 取得运动所需的时间，保存在参数表中
	int nCurWalkSpeed = aNpc.m_CompAttrMgr[ncai_walkspeed];
	if (nCurWalkSpeed > 0)
	{
		aNpc.m_AiParam[4] = (int) nDistance / nCurWalkSpeed;
		aNpc.m_AiParam[5] = 0;
	}
	else
	{
		aNpc.m_AiParam[4] = 0;
		aNpc.m_AiParam[5] = 0;
	}

	aNpc.m_UnaryAttrMgr.Set(nuai_dir, aNpc.m_UnaryAttrMgr[nuai_dir] + nOffsetDir);
	if (aNpc.m_UnaryAttrMgr[nuai_dir] < 0)
		aNpc.m_UnaryAttrMgr.Set(nuai_dir, aNpc.m_UnaryAttrMgr[nuai_dir] + 64);
	else
		aNpc.m_UnaryAttrMgr.Set(nuai_dir, aNpc.m_UnaryAttrMgr[nuai_dir] % 64);


	// 根据三角函数计算偏移的X、Y数值
	nRetCode = GetNpcMoveOffset(aNpc.m_UnaryAttrMgr[nuai_dir], nDistance, &nOffX, &nOffY);
	if (!nRetCode)
		goto Exit0;
	// 获取目标坐标
	nDesX = nCurX + nOffX;
	nDesY = nCurY + nOffY;
	//aNpc.SendCommand(do_walk, nDesX, nDesY);
	aNpc.SendCommand(do_run, nDesX, nDesY);

	nResult = true;
Exit0:
	return nResult;
}

// 鱼类型
// done
int KNpcAI::ShowNpcType13()
{
	int nResult  = false;
	int nRetCode = false;
	// Go the distance between P1 to P2	
	int nDistance = 0;
	int nDesX = 0;
	int nDesY = 0;
	int nCurX = 0;
	int nCurY = 0;
	int nOffX = 0;
	int nOffY = 0;
	int nOffsetDir = 0;
	int nIndex = 0;
	KNpc& aNpc = Npc[m_nIndex];

	aNpc.GetMpsPos(&nCurX, &nCurY);

	// 计算新角度 和 距离
	if (aNpc.m_AiParam[3] > 0)
		nOffsetDir = GetRandomNumber(aNpc.m_AiParam[3], aNpc.m_AiParam[2]);
	else
		nOffsetDir = aNpc.m_AiParam[2];
	if (GetRandomNumber(0, 1))
		nOffsetDir = -nOffsetDir;
	nDistance = GetRandomNumber(aNpc.m_AiParam[0] - aNpc.m_AiParam[1], aNpc.m_AiParam[0]);

	// 取得运动所需的时间，保存在参数表中
	int nCurWalkSpeed = aNpc.m_CompAttrMgr[ncai_walkspeed];
	if (nCurWalkSpeed > 0)
	{
		aNpc.m_AiParam[4] = (int) nDistance / nCurWalkSpeed;
		aNpc.m_AiParam[5] = 0;
	}
	else
	{
		aNpc.m_AiParam[4] = 0;
		aNpc.m_AiParam[5] = 0;
	}
	//if (KeepActiveShowRange())
	//	aNpc.m_Dir += 32;
	// 附近有玩家
	nIndex = IsPlayerCome();
	if (nIndex > 0)
	{
		// do flee
		DoShowFlee(nIndex);
		goto Exit0;
	}
	// 根据三角函数计算偏移的X、Y数值
	nRetCode = GetNpcMoveOffset(aNpc.m_UnaryAttrMgr[nuai_dir], nDistance, &nOffX, &nOffY);
	// 获取目标坐标
	nDesX = nCurX + nOffX;
	nDesY = nCurY + nOffY;
	//aNpc.SendCommand(do_walk, nDesX, nDesY);
	aNpc.SendCommand(do_run, nDesX, nDesY);

	nResult = true;
Exit0:
	return nResult;
}

// 老鼠型
// done
int KNpcAI::ShowNpcType14()
{
	int nResult  = false;
	int nRetCode = false;

	int nDistance = 0;
	int nDesX = 0;
	int nDesY = 0;
	int nCurX = 0;
	int nCurY = 0;
	int nOffX = 0;
	int nOffY = 0;
	int nRandom = 0;
	int nOffsetDir = 0;
	KNpc& aNpc = Npc[m_nIndex];
	int nCurWalkSpeed;

	nRandom = GetRandomNumber(1, 10);
	// 掉头就跑
	if (nRandom < 4)
		nDistance = -nDistance;
	// 嗷嗷发呆
	else if (nRandom < 7)
	{
		aNpc.SendCommand(do_stand);
		goto Exit0;
	}
	aNpc.GetMpsPos(&nCurX, &nCurY);
	// 计算新角度 和 距离
	if (aNpc.m_AiParam[3] > 0)
		nOffsetDir = GetRandomNumber(aNpc.m_AiParam[3], aNpc.m_AiParam[2]);
	else
		nOffsetDir = aNpc.m_AiParam[2];
	if (GetRandomNumber(0, 1))
		nOffsetDir = -nOffsetDir;
	nDistance = GetRandomNumber(aNpc.m_AiParam[0] - aNpc.m_AiParam[1], aNpc.m_AiParam[0]);

	// 取得运动所需的时间，保存在参数表中
	nCurWalkSpeed = aNpc.m_CompAttrMgr[ncai_walkspeed];
	if (nCurWalkSpeed > 0)
	{
		aNpc.m_AiParam[4] = (int) nDistance / nCurWalkSpeed;
		aNpc.m_AiParam[5] = 0;
	}
	else
	{
		aNpc.m_AiParam[4] = 0;
		aNpc.m_AiParam[5] = 0;
	}

	aNpc.m_UnaryAttrMgr.Set(nuai_dir, aNpc.m_UnaryAttrMgr[nuai_dir] + nOffsetDir);
	if (aNpc.m_UnaryAttrMgr[nuai_dir] < 0)
		aNpc.m_UnaryAttrMgr.Set(nuai_dir, aNpc.m_UnaryAttrMgr[nuai_dir] + 64);
	else
		aNpc.m_UnaryAttrMgr.Set(nuai_dir, aNpc.m_UnaryAttrMgr[nuai_dir] % 64);

	// 根据三角函数计算偏移的X、Y数值
	nRetCode = GetNpcMoveOffset(aNpc.m_UnaryAttrMgr[nuai_dir], nDistance, &nOffX, &nOffY);
	if (!nRetCode)
		goto Exit0;
	// 获取目标坐标
	nDesX = nCurX + nOffX;
	nDesY = nCurY + nOffY;
	//aNpc.SendCommand(do_walk, nDesX, nDesY);
	aNpc.SendCommand(do_run, nDesX, nDesY);

	nResult = true;
Exit0:
	return nResult;
}

// 鸡犬型
int KNpcAI::ShowNpcType15()
{
	int nResult  = false;
	int nRetCode = false;
	// Go the distance between P1 to P2	
	int nDistance = 0;
	int nDesX = 0;
	int nDesY = 0;
	int nCurX = 0;
	int nCurY = 0;
	int nOffX = 0;
	int nOffY = 0;
	int nOffsetDir = 0;
	int nIndex = 0;
	KNpc& aNpc = Npc[m_nIndex];

	aNpc.GetMpsPos(&nCurX, &nCurY);

	// 计算新角度 和 距离
	if (aNpc.m_AiParam[3] > 0)
		nOffsetDir = GetRandomNumber(aNpc.m_AiParam[3], aNpc.m_AiParam[2]);
	else
		nOffsetDir = aNpc.m_AiParam[2];
	if (GetRandomNumber(0, 1))
		nOffsetDir = -nOffsetDir;
	nDistance = GetRandomNumber(aNpc.m_AiParam[0] - aNpc.m_AiParam[1], aNpc.m_AiParam[0]);

	// 取得运动所需的时间，保存在参数表中
	int nCurWalkSpeed = aNpc.m_CompAttrMgr[ncai_walkspeed];
	if (nCurWalkSpeed > 0)
	{
		aNpc.m_AiParam[4] = (int) nDistance / nCurWalkSpeed;
		aNpc.m_AiParam[5] = 0;
	}
	else
	{
		aNpc.m_AiParam[4] = 0;
		aNpc.m_AiParam[5] = 0;
	}
	//if (KeepActiveShowRange())
	//	aNpc.m_Dir += 32;
	// 附近有玩家
	nIndex = IsPlayerCome();
	if (nIndex > 0)
	{
		// do flee
		DoShowFlee(nIndex);
		goto Exit0;
	}
	// 根据三角函数计算偏移的X、Y数值
	nRetCode = GetNpcMoveOffset(aNpc.m_UnaryAttrMgr[nuai_dir], nDistance, &nOffX, &nOffY);
	// 获取目标坐标
	nDesX = nCurX + nOffX;
	nDesY = nCurY + nOffY;
	//aNpc.SendCommand(do_walk, nDesX, nDesY);
	aNpc.SendCommand(do_run, nDesX, nDesY);

	nResult = true;
Exit0:
	return nResult;
}

// 兔子型
int KNpcAI::ShowNpcType16()
{
	int nResult  = false;
	int nRetCode = false;

	// Go the distance between P1 to P2
	register int nDistance = 0;
	int nDesX = 0;
	int nDesY = 0;
	int nCurX = 0;
	int nCurY = 0;
	int nOffX = 0;
	int nOffY = 0;
	int nOffsetDir = 0;
	int nIndex = 0;
	int nCurWalkSpeed;
	KNpc& aNpc = Npc[m_nIndex];

	aNpc.GetMpsPos(&nCurX, &nCurY);

	// 计算新角度 和 距离
	if (aNpc.m_AiParam[3] > 0)
		nOffsetDir = GetRandomNumber(aNpc.m_AiParam[3], aNpc.m_AiParam[2]);
	else
		nOffsetDir = aNpc.m_AiParam[2];
	if (GetRandomNumber(0, 1))
		nOffsetDir = -nOffsetDir;
	nDistance = GetRandomNumber(aNpc.m_AiParam[0] - aNpc.m_AiParam[1], aNpc.m_AiParam[0]);
	// 附近有玩家
	nIndex = IsPlayerCome();
	if (nIndex > 0)
	{
		// do flee
		nRetCode = DoShowFlee(nIndex);
		if (!nRetCode)
			goto Exit0;		
		goto Exit1;
	}

	// 计算距离
	nCurWalkSpeed = aNpc.m_CompAttrMgr[ncai_walkspeed];
	if (nCurWalkSpeed > 0)
	{
		aNpc.m_AiParam[4] = (int) nDistance / nCurWalkSpeed;
		aNpc.m_AiParam[5] = 0;
	}
	else
	{
		aNpc.m_AiParam[4] = 0;
		aNpc.m_AiParam[5] = 0;
	}

	// 计算新角度
	//if (KeepActiveShowRange())
	//	aNpc.m_Dir += 32;
	aNpc.m_UnaryAttrMgr.Set(nuai_dir, aNpc.m_UnaryAttrMgr[nuai_dir] + GetRandomNumber(0, 6));
	aNpc.m_UnaryAttrMgr.Set(nuai_dir, aNpc.m_UnaryAttrMgr[nuai_dir] % 64);
	// 根据三角函数计算偏移的X、Y数值
	nRetCode = GetNpcMoveOffset(aNpc.m_UnaryAttrMgr[nuai_dir], nDistance, &nOffX, &nOffY);
	if (!nRetCode)
		goto Exit0;
	// 获取目标坐标
	nDesX = nCurX + nOffX;
	nDesY = nCurY + nOffY;
	//aNpc.SendCommand(do_walk, nDesX, nDesY);
	aNpc.SendCommand(do_run, nDesX, nDesY);

Exit1:	
	nResult = true;
Exit0:
	return nResult;
}

// 蝴蝶型
int KNpcAI::ShowNpcType17()
{
	int nResult  = false;
	int nRetCode = false;

	// Go the distance between P1 to P2
	int nDistance = 0;
	int nDesX = 0;
	int nDesY = 0;
	int nCurX = 0;
	int nCurY = 0;
	int nOffX = 0;
	int nOffY = 0;
	int nOffsetDir = 0;
	KNpc& aNpc = Npc[m_nIndex];

	// 效果加强 随机调整高度
	aNpc.m_Height = GetRandomNumber(aNpc.m_AiParam[6] - 4, aNpc.m_AiParam[6]);

	aNpc.GetMpsPos(&nCurX, &nCurY);
		
	// 计算新角度 和 距离
	if (aNpc.m_AiParam[3] > 0)
		nOffsetDir = GetRandomNumber(aNpc.m_AiParam[3], aNpc.m_AiParam[2]);
	else
		nOffsetDir = aNpc.m_AiParam[2];
	if (GetRandomNumber(0, 1))
		nOffsetDir = -nOffsetDir;
	nDistance = GetRandomNumber(aNpc.m_AiParam[0] - aNpc.m_AiParam[1], aNpc.m_AiParam[0]);	

	// 取得运动所需的时间，保存在参数表中
	int nCurWalkSpeed = aNpc.m_CompAttrMgr[ncai_walkspeed];
	if (nCurWalkSpeed > 0)
	{
		aNpc.m_AiParam[4] = (int) nDistance / nCurWalkSpeed;
		aNpc.m_AiParam[5] = 0;
	}
	else
	{
		aNpc.m_AiParam[4] = 0;
		aNpc.m_AiParam[5] = 0;
	}
	if (KeepActiveRange())
	{
		//aNpc.SendCommand(do_walk, aNpc.m_OriginX, aNpc.m_OriginY);
		goto Exit0;
		//aNpc.m_Dir += 32;
	}
	aNpc.m_UnaryAttrMgr.Set(nuai_dir, aNpc.m_UnaryAttrMgr[nuai_dir] + nOffsetDir);
	if (aNpc.m_UnaryAttrMgr[nuai_dir] < 0)
		aNpc.m_UnaryAttrMgr.Set(nuai_dir, aNpc.m_UnaryAttrMgr[nuai_dir] + 64);
	else
		aNpc.m_UnaryAttrMgr.Set(nuai_dir, aNpc.m_UnaryAttrMgr[nuai_dir] % 64);
	// 根据三角函数计算偏移的X、Y数值
	nRetCode = GetNpcMoveOffset(aNpc.m_UnaryAttrMgr[nuai_dir], nDistance, &nOffX, &nOffY);
	if (!nRetCode)
		goto Exit0;
	// 获取目标坐标
	nDesX = nCurX + nOffX;
	nDesY = nCurY + nOffY;
	//aNpc.SendCommand(do_walk, nDesX, nDesY);
	aNpc.SendCommand(do_run, nDesX, nDesY);
	
	nResult = true;
Exit0:
	return nResult;
}

//-------> Ray [Luoliang] 2005-6-27
//宠物的AI
int KNpcAI::ProcessPet()
{
	int nRet = FALSE;
	switch( Npc[m_nIndex].m_AiMode )
	{
		case KPlayer::enPetTypeCat:
			nRet = PetAICat();
			break;
		case KPlayer::enPetTypeBird:
			nRet = PetAIBird();
			break;
		default:
			break;
	}
	return nRet;
}
//猫的AI
int KNpcAI::PetAICat()
{
	int nResult = FALSE;
	
	KNpc &pet = Npc[m_nIndex];
	KNpc &owner = Npc[pet.m_nPetIndex];
//	if ( pet.IsCommandExist(do_none) == FALSE )
//	{
//		return nResult;
//	}
	
	int nDesX = 0, nDesY = 0;
	int nOffX = 0, nOffY = 0;
	int nCurX = 0, nCurY = 0;
	int nDistance = 0;
	int nOwnerX, nOwnerY;

	owner.GetMpsPos(&nOwnerX, &nOwnerY);
	
	pet.GetMpsPos(&nCurX, &nCurY);
	
	nDistance = g_GetDistance(nOwnerX, nOwnerY, nCurX, nCurY);
	
	int nPos;

	if ( nDistance <= pet.m_AiParam[0] )
	{
		nPos = enNear;
	}
	else if ( nDistance > pet.m_AiParam[0] && nDistance <= pet.m_AiParam[1] )
	{
		nPos = enMiddle;
	}	
	else
	{
		nPos = enFar;
	}

	int nRand = GetRandomNumber(0, 100);
	switch( nPos )
	{
		case enNear:
			{				
				if ( pet.m_Doing != do_stand )
				{
					return TRUE;
				}
				
				if ( nRand <= pet.m_AiParam[2] )
				{
					pet.SendCommand(do_special3);
				}			
				else if ( nRand <= pet.m_AiParam[2] + pet.m_AiParam[3] )
				{
					pet.SendCommand(do_special4);
				}
				else if ( nRand <= pet.m_AiParam[2] + pet.m_AiParam[3] + pet.m_AiParam[4] )				
				{										
					nOffX = GetRandomNumber(50, pet.m_AiParam[0]);
					nOffY = GetRandomNumber(50, pet.m_AiParam[0]);
					
					if ( GetRandomNumber(0, 1) == 0 )
					{
						nOffX = -nOffX;
					}
					
					if ( GetRandomNumber(0, 1) == 0 )
					{
						nOffY = -nOffY;
					}

					//pet.SendCommand(do_walk, nOwnerX + nOffX, nOwnerY + nOffY);
					pet.SendCommand(do_run, nOwnerX + nOffX, nOwnerY + nOffY);
					
				}
				else if ( nRand <= pet.m_AiParam[2] + pet.m_AiParam[3] + pet.m_AiParam[4] + pet.m_AiParam[5] )
				{
					nOffX = GetRandomNumber(50, pet.m_AiParam[0]);
					nOffY = GetRandomNumber(50, pet.m_AiParam[0]);
					
					if ( GetRandomNumber(0, 1) == 0 )
					{
						nOffX = -nOffX;
					}
					
					if ( GetRandomNumber(0, 1) == 0 )
					{
						nOffY = -nOffY;
					}				

					pet.SendCommand(do_run, nOwnerX + nOffX, nOwnerY + nOffY);
				}
				else
				{
					pet.SendCommand(do_stand);
				}
			}
			break;
		case enMiddle:
			{
				nOffX = GetRandomNumber(50, pet.m_AiParam[0]);
				nOffY = GetRandomNumber(50, pet.m_AiParam[0]);
				
				if ( GetRandomNumber(0, 1) == 0 )
				{
					nOffX = -nOffX;
				}
				
				if ( GetRandomNumber(0, 1) == 0 )
				{
					nOffY = -nOffY;
				}

				if ( nRand < pet.m_AiParam[6] )
				{
					//pet.SendCommand(do_walk, nOwnerX + nOffX, nOwnerY + nOffY);
					pet.SendCommand(do_run, nOwnerX + nOffX, nOwnerY + nOffY);
				}
				else
				{
					pet.SendCommand(do_run, nOwnerX + nOffX, nOwnerY + nOffY);
				}				
			}
			break;
		default:	//enFar
			{
				nOffX = GetRandomNumber(50, pet.m_AiParam[0]);
				nOffY = GetRandomNumber(50, pet.m_AiParam[0]);
				
				if ( GetRandomNumber(0, 1) == 0 )
				{
					nOffX = -nOffX;
				}
				
				if ( GetRandomNumber(0, 1) == 0 )
				{
					nOffY = -nOffY;
				}
				pet.SendCommand(do_run, nOwnerX + nOffX, nOwnerY + nOffY);
			}
			break;
	}
	return TRUE;
}
//<------- End [Ray]

//鸟的AI
int KNpcAI::PetAIBird()
{
	int nResult = FALSE;
	
	KNpc &pet = Npc[m_nIndex];
	KNpc &owner = Npc[pet.m_nPetIndex];
//	if ( pet.IsCommandExist(do_none) == FALSE )
//	{
//		return nResult;
//	}
	
	int nDesX = 0, nDesY = 0;
	int nOffX = 0, nOffY = 0;
	int nCurX = 0, nCurY = 0;
	int nDistance = 0;
	int nOwnerX, nOwnerY;

	owner.GetMpsPos(&nOwnerX, &nOwnerY);
	
	pet.GetMpsPos(&nCurX, &nCurY);
	
	nDistance = g_GetDistance(nOwnerX, nOwnerY, nCurX, nCurY);
	
	int nPos;

	if ( nDistance <= pet.m_AiParam[0] )
	{
		nPos = enNear;
	}
	else if ( nDistance > pet.m_AiParam[0] && nDistance <= pet.m_AiParam[1] )
	{
		nPos = enMiddle;
	}	
	else
	{
		nPos = enFar;
	}

	static bool sbCircling = true;
	static int snCircleCounter = 0;
	
	int nRand = GetRandomNumber(0, 100);
	switch( nPos )
	{
		case enNear:
			{					
				if ( sbCircling )
				{		
//					if ( pet.m_Doing == do_walk )
//					{
//						return TRUE;
//					}
					if ( snCircleCounter++ > pet.m_AiParam[8] )
					{
						snCircleCounter = 0;
						sbCircling = false;						
					}
					else
					{
						int nPace = pet.m_CompAttrMgr[ncai_runspeed];
						int nDir = pet.m_UnaryAttrMgr[nuai_dir];
						if ( nDir + 8 >= 64 )
						{
							nDir = 0;
						}
						else 
						{
							nDir += 8;
						}

						nDesX = nCurX + ((g_DirSin(nDir, 64) * nPace) >> 8);
						nDesY = nCurY - ((g_DirCos(nDir, 64) * nPace) >> 8);

						//pet.SendCommand(do_walk, nDesX, nDesY);
						pet.SendCommand(do_run, nDesX, nDesY);
						
						return TRUE;
					}
				}
				
				
				if ( nRand <= pet.m_AiParam[2] )
				{
					pet.SendCommand(do_special3);
				}			
				else if ( nRand <= pet.m_AiParam[2] + pet.m_AiParam[3] )
				{
					pet.SendCommand(do_special4);
				}
				else if ( nRand <= pet.m_AiParam[2] + pet.m_AiParam[3] + pet.m_AiParam[4] )				
				{										
					nOffX = GetRandomNumber(50, pet.m_AiParam[0]);
					nOffY = GetRandomNumber(50, pet.m_AiParam[0]);
					
					if ( GetRandomNumber(0, 1) == 0 )
					{
						nOffX = -nOffX;
					}
					
					if ( GetRandomNumber(0, 1) == 0 )
					{
						nOffY = -nOffY;
					}

					//pet.SendCommand(do_walk, nOwnerX + nOffX, nOwnerY + nOffY);
					pet.SendCommand(do_run, nOwnerX + nOffX, nOwnerY + nOffY);	
					
				}
				else if ( nRand <= pet.m_AiParam[2] + pet.m_AiParam[3] + pet.m_AiParam[4] + pet.m_AiParam[5] )
				{
					nOffX = GetRandomNumber(50, pet.m_AiParam[0]);
					nOffY = GetRandomNumber(50, pet.m_AiParam[0]);
					
					if ( GetRandomNumber(0, 1) == 0 )
					{
						nOffX = -nOffX;
					}
					
					if ( GetRandomNumber(0, 1) == 0 )
					{
						nOffY = -nOffY;
					}				

					pet.SendCommand(do_run, nOwnerX + nOffX, nOwnerY + nOffY);
				}
				else if ( nRand <= pet.m_AiParam[2] + pet.m_AiParam[3] + pet.m_AiParam[4] + pet.m_AiParam[5] + pet.m_AiParam[6] )
				{
					sbCircling = true;				
				}
				else
				{
					pet.SendCommand(do_stand);
				}
			}
			break;
		case enMiddle:
			{
				sbCircling = false;
				nOffX = GetRandomNumber(50, pet.m_AiParam[0]);
				nOffY = GetRandomNumber(50, pet.m_AiParam[0]);
				
				if ( GetRandomNumber(0, 1) == 0 )
				{
					nOffX = -nOffX;
				}
				
				if ( GetRandomNumber(0, 1) == 0 )
				{
					nOffY = -nOffY;
				}

				if ( nRand < pet.m_AiParam[7] )
				{
					//pet.SendCommand(do_walk, nOwnerX + nOffX, nOwnerY + nOffY);
					pet.SendCommand(do_run, nOwnerX + nOffX, nOwnerY + nOffY);
				}
				else
				{
					pet.SendCommand(do_run, nOwnerX + nOffX, nOwnerY + nOffY);
				}				
			}
			break;
		default:	//enFar
			{
				sbCircling = false;
				nOffX = GetRandomNumber(50, pet.m_AiParam[0]);
				nOffY = GetRandomNumber(50, pet.m_AiParam[0]);
				
				if ( GetRandomNumber(0, 1) == 0 )
				{
					nOffX = -nOffX;
				}
				
				if ( GetRandomNumber(0, 1) == 0 )
				{
					nOffY = -nOffY;
				}
				pet.SendCommand(do_run, nOwnerX + nOffX, nOwnerY + nOffY);
			}
			break;
	}
	return TRUE;
}
//<------- End [Ray]

#endif
//---------------------------------------------------------------------
// Player AI add here.
// flying comment
void KNpcAI::ProcessPlayer()
{
#ifdef _SERVER
	TriggerObjectTrap();
	TriggerMapTrap();
// lixuewu 2004.02.11
	KCreature& aCreature = Player[Npc[m_nIndex].GetPlayerIdx()].m_Creature;
	if (aCreature.IsALive())
	{
		aCreature.ProcessAI();
		// 必须确保召唤兽的AI是最后处理的因为在AI函数中破坏了m_nIndex
		//ProcessCreatureAI();
		return;
	}
// lixuewu 2004.02.11

#else
	
// 	int i = Npc[m_nIndex].GetTargetObj();
// 	if (i > 0 && Npc[m_nIndex].GetTargetType()==type_obj)
// 	{
// 		FollowObject(i);
// 	}	

// 	int j = Npc[m_nIndex].GetTargetNpc();
// 	bool bCanFollowPeople = Npc[m_nIndex].GetCanFollowAndAttack();
// 
// 	int distance = NpcSet.GetDistance(j, m_nIndex);
// 	int	nRelation = NpcSet.GetRelation(m_nIndex, j);
// 	if ( (j > 0 && (distance > Npc[j].m_DialogRadius)) || (j > 0 && (nRelation == relation_enemy)) )
// 	{		
// 		if ( bCanFollowPeople )
// 		{
// 			FollowPeople(j);
// 		}
// 	}
#endif
}

void KNpcAI::TriggerMapTrap()
{
	Npc[m_nIndex].CheckTrap();
}

void KNpcAI::TriggerObjectTrap()
{
	return;
}

int KNpcAI::GetNearestNpc(int nRelation)
{
	int nRangeX = Npc[m_nIndex].m_CompAttrMgr[ncai_visionradius];
	int	nRangeY = nRangeX;
	int	nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
	int	nRegion = Npc[m_nIndex].m_RegionIndex;
	int	nMapX = Npc[m_nIndex].GetMapX();
	int	nMapY = Npc[m_nIndex].GetMapY();
	int	nRet;
	int	nRMx, nRMy, nSearchRegion;

	nRangeX = nRangeX / REGION_CELL_SIZE_X;
	nRangeY = nRangeY / REGION_CELL_SIZE_Y;	

	// 检查视野范围内的格子里的NPC
	for (int i = 0; i < nRangeX; i++)	// i, j由0开始而不是从-range开始是要保证Nearest
	{
		for (int j = 0; j < nRangeY; j++)
		{
			// 去掉边角几个格子，保证视野是椭圆形
			if ((i * i + j * j) > nRangeX * nRangeX)
				continue;

			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX + i;
			nRMy = nMapY + j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += REGION_CELL_WIDTH;
			}
			else if (nRMx >= REGION_CELL_WIDTH)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= REGION_CELL_WIDTH ;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += REGION_CELL_HEIGHT;
			}
			else if (nRMy >= REGION_CELL_HEIGHT)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= REGION_CELL_HEIGHT;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC			
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nIndex, nRelation);
			if (nRet > 0)
				return nRet;
			
			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX - i;
			nRMy = nMapY + j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += REGION_CELL_WIDTH;
			}
			else if (nRMx >= REGION_CELL_WIDTH)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= REGION_CELL_WIDTH;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += REGION_CELL_HEIGHT;
			}
			else if (nRMy >= REGION_CELL_HEIGHT)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= REGION_CELL_HEIGHT;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC			
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nIndex, nRelation);
			if (nRet > 0)
				return nRet;

			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX - i;
			nRMy = nMapY - j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += REGION_CELL_WIDTH;
			}
			else if (nRMx >= REGION_CELL_WIDTH)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= REGION_CELL_WIDTH;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += REGION_CELL_HEIGHT;
			}
			else if (nRMy >= REGION_CELL_HEIGHT)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= REGION_CELL_HEIGHT;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC			
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nIndex, nRelation);
			if (nRet > 0)
				return nRet;

			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX + i;
			nRMy = nMapY - j;
			nSearchRegion = nRegion;			
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += REGION_CELL_WIDTH;
			}
			else if (nRMx >= REGION_CELL_WIDTH)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= REGION_CELL_WIDTH;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += REGION_CELL_HEIGHT;
			}
			else if (nRMy >= REGION_CELL_HEIGHT)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= REGION_CELL_HEIGHT;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nIndex, nRelation);
			if (nRet > 0)
				return nRet;
		}
	}
	return 0;
}

#ifndef _SERVER
// flying add this
// 查找离某个NPC最近的玩家
int KNpcAI::IsPlayerCome()
{
	int nResult = 0;
	int nPlayer = 0;
	int X1 = 0;
	int Y1 = 0;
	int X2 = 0;
	int Y2 = 0;
	int nDistance = 0;

	nPlayer = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	nDistance = NpcSet.GetDistance(nPlayer, m_nIndex);
	// 鸡看的到的玩家
	if (nDistance < Npc[m_nIndex].m_CompAttrMgr[ncai_vision][idx_base_value])
	{
		// 分别处理走和跑
		if (Player[CLIENT_PLAYER_INDEX].m_RunStatus ||
			Npc[m_nIndex].m_CompAttrMgr[ncai_vision] > nDistance * 4)
		{
			nResult = nPlayer;
		}
	}
	return nResult;
}
#endif

int KNpcAI::GetNpcNumber(int nRelation)
{
	int nRangeX = Npc[m_nIndex].m_CompAttrMgr[ncai_visionradius];
	int	nRangeY = nRangeX;
	int	nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
	int	nRegion = Npc[m_nIndex].m_RegionIndex;
	int	nMapX = Npc[m_nIndex].GetMapX();
	int	nMapY = Npc[m_nIndex].GetMapY();
	int	nRet = 0;
	int	nRMx, nRMy, nSearchRegion;

	nRangeX = nRangeX / REGION_CELL_SIZE_X;
	nRangeY = nRangeY / REGION_CELL_SIZE_Y;

	// 检查视野范围内的格子里的NPC
	for (int i = -nRangeX; i < nRangeX; i++)
	{
		for (int j = -nRangeY; j < nRangeY; j++)
		{
			// 去掉边角几个格子，保证视野是椭圆形
			if ((i * i + j * j) > nRangeX * nRangeX)
				continue;

			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX + i;
			nRMy = nMapY + j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += REGION_CELL_WIDTH;
			}
			else if (nRMx >= REGION_CELL_WIDTH)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= REGION_CELL_WIDTH;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += REGION_CELL_HEIGHT;
			}
			else if (nRMy >= REGION_CELL_HEIGHT)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= REGION_CELL_HEIGHT;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC			
			int nNpcIdx = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nIndex, nRelation);
			if (nNpcIdx > 0)
				nRet++;
		}
	}
	return nRet;
}

void KNpcAI::KeepAttackRange(int nEnemy, int nRange)
{
	int nX1, nY1, nX2, nY2, nDir, nWantX, nWantY;

	Npc[m_nIndex].GetMpsPos(&nX1, &nY1);
	Npc[nEnemy].GetMpsPos(&nX2, &nY2);
	nDir = g_GetDirIndex(nX1, nY1, nX2, nY2);

	//往后退一步
	nWantX = nX2 - ((nRange * g_DirCos(nDir, 64)) >> 10);
	nWantY = nY2 - ((nRange * g_DirSin(nDir, 64)) >> 10);

	//Npc[m_nIndex].SendCommand(do_walk, nWantX, nWantY);
	Npc[m_nIndex].SendCommand(do_run, nWantX, nWantY);
}

void KNpcAI::FollowAttack(int i)
{
	if ( Npc[i].m_RegionIndex < 0 )
		return;

	int distance = NpcSet.GetDistance(m_nIndex, i);

#define	MINI_ATTACK_RANGE	32

	if (distance <= MINI_ATTACK_RANGE)
	{
		KeepAttackRange(i, MINI_ATTACK_RANGE + Npc[i].m_CompAttrMgr[ncai_walkspeed]);
		return;
	}
	// Attack Enemy
	if (distance <= Npc[m_nIndex].m_CompAttrMgr[ncai_attackradius] && InEyeshot(i))
	{

#ifdef _SERVER
		Npc[m_nIndex].SendCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, i);
#else
		SendClientCmdSkill(Npc[m_nIndex].m_ActiveSkillID, -1, i);
#endif

		//<-- End
		return;
	}

	// Move to Enemy
	int x, y;
	Npc[i].GetMpsPos(&x, &y);

	//Npc[m_nIndex].SendCommand(do_walk, x, y);
	Npc[m_nIndex].SendCommand(do_run, x, y);
}

BOOL KNpcAI::InEyeshot(int nIdx)
{
	int distance = NpcSet.GetDistance(nIdx, m_nIndex);

	return (Npc[m_nIndex].m_CompAttrMgr[ncai_visionradius] > distance);
}

void KNpcAI::CommonAction()
{
	// 如果是对话类的NPC，就原地不动
	if ((Npc[m_nIndex].m_Kind == kind_dialoger)||(Npc[m_nIndex].m_Kind == kind_siege_weapon))
	{
		Npc[m_nIndex].SendCommand(do_stand);
		return;
	}
	int	nOffX, nOffY;
	if (g_RandPercent(80))
	{
		nOffX = 0;
		nOffY = 0;
	}
	else
	{
		
		nOffX = g_Random(Npc[m_nIndex].m_CompAttrMgr[ncai_activeradius] / 2);
		nOffY = g_Random(Npc[m_nIndex].m_CompAttrMgr[ncai_activeradius] / 2);
		if (nOffX & 1)
		{
			nOffX = - nOffX;
		}
		if (nOffY & 1)
		{
			nOffY = - nOffY;
		}
	}
	//Npc[m_nIndex].SendCommand(do_walk, Npc[m_nIndex].m_OriginX + nOffX, Npc[m_nIndex].m_OriginY + nOffY);
	Npc[m_nIndex].SendCommand(do_run, Npc[m_nIndex].m_OriginX + nOffX, Npc[m_nIndex].m_OriginY + nOffY);
}

BOOL KNpcAI::KeepActiveRange()
{
	int x, y;
	
	Npc[m_nIndex].GetMpsPos(&x, &y);
	int	nRange = g_GetDistance(Npc[m_nIndex].m_OriginX, Npc[m_nIndex].m_OriginY, x, y);

	// 发现超出当前活动范围，往回走
	if (Npc[m_nIndex].m_CompAttrMgr[ncai_activeradius] < nRange)
	{
		//Npc[m_nIndex].SendCommand(do_walk, Npc[m_nIndex].m_OriginX, Npc[m_nIndex].m_OriginY);
		Npc[m_nIndex].SendCommand(do_run, Npc[m_nIndex].m_OriginX, Npc[m_nIndex].m_OriginY);
		return TRUE;
	}
	else	// 在当前活动范围内，恢复当前活动范围大小。
	{
		Npc[m_nIndex].m_CompAttrMgr.Set(ncai_activeradius, idx_append_value, 0);
		Npc[m_nIndex].m_CompAttrMgr.Set(ncai_activeradius, idx_append_percent, 0);
		return FALSE;
	}
}

#ifndef _SERVER
// 15/16 AiMode NPC的逃逸动作
int KNpcAI::DoShowFlee(int nIdx)
{
	int nResult  = false;
	int nRetCode = false;
	
	int x1, y1, x2, y2;
	int nDistance = Npc[m_nIndex].m_AiParam[6];

	Npc[m_nIndex].GetMpsPos(&x1, &y1);
	//Npc[nIdx].GetMpsPos(&x2, &y2);
	Npc[m_nIndex].m_UnaryAttrMgr.Set(nuai_dir,Npc[nIdx].m_UnaryAttrMgr[nuai_dir] );
	nRetCode = GetNpcMoveOffset(Npc[m_nIndex].m_UnaryAttrMgr[nuai_dir], nDistance, &x2, &y2);
	if (!nRetCode)
		goto Exit0;
	Npc[m_nIndex].m_AiParam[4] = (int) nDistance / Npc[m_nIndex].m_CompAttrMgr[ncai_walkspeed][idx_base_value];
	Npc[m_nIndex].m_AiParam[5] = 0;
	//Npc[m_nIndex].SendCommand(do_walk, x1 + x2, y1 + y2);
	Npc[m_nIndex].SendCommand(do_run, x1 + x2, y1 + y2);
	nResult = true;
Exit0:
	return nResult;
}

#endif

// 逃离Npc[nIdx]
void KNpcAI::Flee(int nIdx)
{
	int x1, y1, x2, y2;

	Npc[m_nIndex].GetMpsPos(&x1, &y1);
	Npc[nIdx].GetMpsPos(&x2, &y2);

	x1 = x1 * 2 - x2;
	y1 = y1 * 2 - y2;

	//Npc[m_nIndex].SendCommand(do_walk, x1, y1);
	Npc[m_nIndex].SendCommand(do_run, x1, y1);
}

//------------------------------------------------------------------------------
//	功能：普通主动类1的变化版本
//	m_AiParam[0] 无敌人时候的巡逻概率
//	m_AiParam[1、2、3、4] 四种技能的使用概率，分别对应SkillList里的技能1 2 3 4
//	m_AiParam[5、6] 看见敌人但比较远时，待机、巡逻的概率
//------------------------------------------------------------------------------
void	KNpcAI::ProcessAIType07()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;

	int nEnemyIdx = Npc[m_nIndex].GetTargetNpc();
	
	// 如果原本没有锁定敌人或者这个敌人跑太远或者敌人坐到车里面去了，重新锁定敌人
	//--> Rocker 2004/09/14 修改条件，lixuewu 2004.09.22 补充条件Npc保护
	if (nEnemyIdx <= 0 || Npc[nEnemyIdx].m_dwID <= 0 || !InEyeshot(nEnemyIdx))
	{
		nEnemyIdx = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].SetTarget(type_npc, nEnemyIdx);
	}

	if(nEnemyIdx > 0 && Npc[nEnemyIdx].m_nSafeGuardLevel >= 2)
	{
		Npc[m_nIndex].SetTarget(type_npc, 0);
		nEnemyIdx = 0;
	}
	//<-- End

	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// 是否已超过活动半径
		if (KeepActiveRange())
			return;
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}

	// 如果敌人在所有技能攻击范围之外，一定概率选择待机/巡逻/向敌人靠近
	if (KNpcSet::GetDistanceSquare(m_nIndex, nEnemyIdx) > pAIParam[MAX_AI_PARAM - 1])
	{
		int		nRand;
		nRand = g_Random(100);
		if (nRand < pAIParam[5])	// 待机
			return;
		if (nRand < pAIParam[5] + pAIParam[6])	// 巡逻
		{
			CommonAction();
			return;
		}
		FollowAttack(nEnemyIdx);	// 向敌人靠近
		return;
	}

	// 敌人在最大技能攻击范围之内，选择一种技能攻击
	int		nRand;
	nRand = g_Random(100);
	if (nRand < pAIParam[1])
	{
		if (!Npc[m_nIndex].SetActiveSkill(0))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2])
	{
		if (!Npc[m_nIndex].SetActiveSkill(1))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2] + pAIParam[3])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2] + pAIParam[3] + pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else	// 待机
	{
		return;
	}

	FollowAttack(nEnemyIdx);
}


//------------------------------------------------------------------------------
//	功能：普通主动类1
//	m_AiParam[0] 无敌人时候的巡逻概率
//	m_AiParam[1、2、3、4] 四种技能的使用概率，分别对应SkillList里的技能1 2 3 4
//	m_AiParam[5、6] 看见敌人但比较远时，待机、巡逻的概率
//------------------------------------------------------------------------------
void	KNpcAI::ProcessAIType01()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;
	// 是否已超过活动半径
	if (KeepActiveRange())
		return;

	int nEnemyIdx = Npc[m_nIndex].GetTargetNpc();
	
	// 如果原本没有锁定敌人或者这个敌人跑太远或者敌人坐到车里面去了，重新锁定敌人
	//--> Rocker 2004/09/14 修改条件，lixuewu 2004.09.22 补充条件Npc保护
	if (nEnemyIdx <= 0 || Npc[nEnemyIdx].m_dwID <= 0 || !InEyeshot(nEnemyIdx))
	{
		nEnemyIdx = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].SetTarget(type_npc, nEnemyIdx);
	}

	if(nEnemyIdx > 0 && Npc[nEnemyIdx].m_nSafeGuardLevel >= 2)
	{
		Npc[m_nIndex].SetTarget(type_npc, 0);
		nEnemyIdx = 0;
	}
	//<-- End

	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}

	// 如果敌人在所有技能攻击范围之外，一定概率选择待机/巡逻/向敌人靠近
	if (KNpcSet::GetDistanceSquare(m_nIndex, nEnemyIdx) > pAIParam[MAX_AI_PARAM - 1])
	{
		int		nRand;
		nRand = g_Random(100);
		if (nRand < pAIParam[5])	// 待机
			return;
		if (nRand < pAIParam[5] + pAIParam[6])	// 巡逻
		{
			CommonAction();
			return;
		}
		FollowAttack(nEnemyIdx);	// 向敌人靠近
		return;
	}

	// 敌人在最大技能攻击范围之内，选择一种技能攻击
	int		nRand;
	nRand = g_Random(100);
	if (nRand < pAIParam[1])
	{
		if (!Npc[m_nIndex].SetActiveSkill(0))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2])
	{
		if (!Npc[m_nIndex].SetActiveSkill(1))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2] + pAIParam[3])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2] + pAIParam[3] + pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else	// 待机
	{
		return;
	}

	FollowAttack(nEnemyIdx);
}


//------------------------------------------------------------------------------
//	功能：普通主动类2
//	m_AiParam[0] 无敌人时候的巡逻概率
//	m_AiParam[1] 剩余生命低于这个百分比的时候执行相应处理
//	m_AiParam[2] 在m_AiParam[1]的情况出现的时候是否执行相应处理的概率
//	m_AiParam[3] 在m_AiParam[1]的情况出现并决定要执行相应处理，使用回复技能的概率 对应SkillList里面的技能 1
//	m_AiParam[4、5、6] 三种攻击技能的使用概率，分别对应SkillList里的技能 2 3 4
//	m_AiParam[7、8] 看见敌人但比较远时，待机、巡逻的概率
//------------------------------------------------------------------------------
void	KNpcAI::ProcessAIType02()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;
	// 是否已超过活动半径
	if (KeepActiveRange())
		return;

	int nEnemyIdx = Npc[m_nIndex].GetTargetNpc();

	if (nEnemyIdx == m_nIndex)
	{
		return;
	}
	
	// 如果原本没有锁定敌人或者这个敌人跑太远或者敌人坐到车里，重新锁定敌人
	if (nEnemyIdx <= 0 || Npc[nEnemyIdx].m_dwID <= 0 || !InEyeshot(nEnemyIdx) )
	{
		nEnemyIdx = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].SetTarget(type_npc, nEnemyIdx);
	}
	if(nEnemyIdx > 0 && Npc[nEnemyIdx].m_nSafeGuardLevel >= 2)
	{
		Npc[m_nIndex].SetTarget(type_npc, 0);
		nEnemyIdx = 0;
	}
	
	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}

	// 检测剩余生命是否符合条件，生命太少一定概率使用补血技能或逃跑
	if (  Npc[m_nIndex].m_UnaryAttrMgr[nuai_curlife] * 100 
		/ Npc[m_nIndex].m_CompAttrMgr[ncai_lifeuplimit] < pAIParam[1])
	{
		if (g_RandPercent(pAIParam[2]))	// 是否使用补血技能或逃跑
		{
			if (Npc[m_nIndex].m_AiAddLifeTime < pAIParam[9] && g_RandPercent(pAIParam[3]))	// 使用补血技能
			{
				Npc[m_nIndex].SetActiveSkill(0);

#ifdef _SERVER
				Npc[m_nIndex].SendCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, m_nIndex);
#else
				SendClientCmdSkill(Npc[m_nIndex].m_ActiveSkillID, -1, m_nIndex);
#endif

				Npc[m_nIndex].m_AiAddLifeTime++;
				return;
			}
			else	// 逃跑
			{
				Flee(nEnemyIdx);
				return;
			}
		}
	}

	// 如果敌人在所有技能攻击范围之外，一定概率选择待机/巡逻/向敌人靠近
	if (KNpcSet::GetDistanceSquare(m_nIndex, nEnemyIdx) > pAIParam[MAX_AI_PARAM - 1])
	{
		int		nRand;
		nRand = g_Random(100);
		if (nRand < pAIParam[7])	// 待机
			return;
		if (nRand < pAIParam[7] + pAIParam[8])	// 巡逻
		{
			CommonAction();
			return;
		}
		FollowAttack(nEnemyIdx);	// 向敌人靠近
		return;
	}

	// 敌人在最大技能攻击范围之内，选择一种技能攻击
	int		nRand;
	nRand = g_Random(100);
	if (nRand < pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(1))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5] + pAIParam[6])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else	// 待机
	{
		return;
	}
	FollowAttack(nEnemyIdx);
}

//------------------------------------------------------------------------------
//	功能：普通主动类3
//	m_AiParam[0] 无敌人时候的巡逻概率
//	m_AiParam[1] 剩余生命低于这个百分比的时候执行相应处理
//	m_AiParam[2] 在m_AiParam[1]的情况出现的时候是否执行相应处理的概率
//	m_AiParam[3] 在m_AiParam[1]的情况出现并决定要执行相应处理，使用攻击技能的概率 对应SkillList里面的技能 1
//	m_AiParam[4、5、6] 三种攻击技能的使用概率，分别对应SkillList里的技能 2 3 4
//	m_AiParam[7、8] 看见敌人但比较远时，待机、巡逻的概率
//------------------------------------------------------------------------------
void	KNpcAI::ProcessAIType03()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;
	// 是否已超过活动半径
	if (KeepActiveRange())
		return;

	int nEnemyIdx = Npc[m_nIndex].GetTargetNpc();
	// 如果原本没有锁定敌人或者这个敌人跑太远或者敌人坐到车里，重新锁定敌人
	//--> Rocker 2004/09/14 lixuewu 2004.09.22 补充条件Npc保护
	if (nEnemyIdx <= 0 || Npc[nEnemyIdx].m_dwID <= 0 || !InEyeshot(nEnemyIdx))
	{
		nEnemyIdx = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].SetTarget(type_npc, nEnemyIdx);
	}
	if(nEnemyIdx > 0 && Npc[nEnemyIdx].m_nSafeGuardLevel >= 2)
	{
		Npc[m_nIndex].SetTarget(type_npc, 0);
		nEnemyIdx = 0;
	}
	//<-- End

	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}

	// 检测剩余生命是否符合条件，生命太少一定概率使用攻击技能或逃跑
	if (  Npc[m_nIndex].m_UnaryAttrMgr[nuai_curlife] * 100 
		/ Npc[m_nIndex].m_CompAttrMgr[ncai_lifeuplimit] < pAIParam[1])
	{
		if (g_RandPercent(pAIParam[2]))	// 是否使用攻击技能或逃跑
		{
			if (g_RandPercent(pAIParam[3]))	// 使用攻击技能
			{
				Npc[m_nIndex].SetActiveSkill(0);
				FollowAttack(nEnemyIdx);
				return;
			}
			else	// 逃跑
			{
				Flee(nEnemyIdx);
				return;
			}
		}
	}

	// 如果敌人在所有技能攻击范围之外，一定概率选择待机/巡逻/向敌人靠近
	if (KNpcSet::GetDistanceSquare(m_nIndex, nEnemyIdx) > pAIParam[MAX_AI_PARAM - 1])
	{
		int		nRand;
		nRand = g_Random(100);
		if (nRand < pAIParam[7])	// 待机
			return;
		if (nRand < pAIParam[7] + pAIParam[8])	// 巡逻
		{
			CommonAction();
			return;
		}
		FollowAttack(nEnemyIdx);	// 向敌人靠近
		return;
	}

	// 敌人在最大技能攻击范围之内，选择一种技能攻击
	int		nRand;
	nRand = g_Random(100);
	if (nRand < pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(1))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5] + pAIParam[6])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else	// 待机
	{
		return;
	}
	FollowAttack(nEnemyIdx);
}

//------------------------------------------------------------------------------
//	功能：普通被动类1
//	m_AiParam[0] 无敌人时候的巡逻概率
//	m_AiParam[1、2、3、4] 四种攻击技能的使用概率，分别对应SkillList里的技能 1 2 3 4
//	m_AiParam[5、6] 看见敌人但比较远时，待机、巡逻的概率
//------------------------------------------------------------------------------
void	KNpcAI::ProcessAIType04()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;

	int nEnemyIdx = Npc[m_nIndex].GetTargetNpc();
	// 是否受到攻击，否，一定概率选择待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}

	// 是否已超过活动半径
	if (KeepActiveRange())
		return;
	
	// 如果敌人在所有技能攻击范围之外，一定概率选择待机/巡逻/向敌人靠近
	if (KNpcSet::GetDistanceSquare(m_nIndex, nEnemyIdx) > pAIParam[MAX_AI_PARAM - 1])
	{
		int		nRand;
		nRand = g_Random(100);
		if (nRand < pAIParam[5])	// 待机
			return;
		if (nRand < pAIParam[5] + pAIParam[6])	// 巡逻
		{
			CommonAction();
			return;
		}
		FollowAttack(nEnemyIdx);	// 向敌人靠近
		return;
	}

	// 敌人在最大技能攻击范围之内，选择一种技能攻击
	int		nRand;
	nRand = g_Random(100);
	if (nRand < pAIParam[1])
	{
		if (!Npc[m_nIndex].SetActiveSkill(0))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2])
	{
		if (!Npc[m_nIndex].SetActiveSkill(1))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2] + pAIParam[3])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2] + pAIParam[3] + pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else	// 待机
	{
		return;
	}
	FollowAttack(nEnemyIdx);
}

//------------------------------------------------------------------------------
//	功能：普通被动类2
//	m_AiParam[0] 无敌人时候的巡逻概率
//	m_AiParam[1] 剩余生命低于这个百分比的时候执行相应处理
//	m_AiParam[2] 在m_AiParam[1]的情况出现的时候是否执行相应处理的概率
//	m_AiParam[3] 在m_AiParam[1]的情况出现并决定要执行相应处理，使用回复技能的概率 对应SkillList里面的技能 1
//	m_AiParam[4、5、6] 三种攻击技能的使用概率，分别对应SkillList里的技能 2 3 4
//	m_AiParam[7、8] 看见敌人但比较远时，待机、巡逻的概率
//------------------------------------------------------------------------------
void	KNpcAI::ProcessAIType05()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;

	int nEnemyIdx = Npc[m_nIndex].GetTargetNpc();
	// 是否受到攻击，否，一定概率选择待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}

	// 是否已超过活动半径
	if (KeepActiveRange())
		return;

	// 检测剩余生命是否符合条件，生命太少一定概率使用补血技能或逃跑
	if (  Npc[m_nIndex].m_UnaryAttrMgr[nuai_curlife] * 100 
		/ Npc[m_nIndex].m_CompAttrMgr[ncai_lifeuplimit] < pAIParam[1])
	{
		if (g_RandPercent(pAIParam[2]))	// 是否使用补血技能或逃跑
		{
			if (Npc[m_nIndex].m_AiAddLifeTime < pAIParam[9] && g_RandPercent(pAIParam[3]))	// 使用补血技能
			{
				Npc[m_nIndex].m_AiAddLifeTime++;
				Npc[m_nIndex].SetActiveSkill(0);

#ifdef _SERVER
				Npc[m_nIndex].SendCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, m_nIndex);
#else
				SendClientCmdSkill(Npc[m_nIndex].m_ActiveSkillID, -1, m_nIndex);
#endif

				return;
			}
			else	// 逃跑
			{
				Flee(nEnemyIdx);
				return;
			}
		}
	}

	// 如果敌人在所有技能攻击范围之外，一定概率选择待机/巡逻/向敌人靠近
	if (KNpcSet::GetDistanceSquare(m_nIndex, nEnemyIdx) > pAIParam[MAX_AI_PARAM - 1])
	{
		int		nRand;
		nRand = g_Random(100);
		if (nRand < pAIParam[7])	// 待机
			return;
		if (nRand < pAIParam[7] + pAIParam[8])	// 巡逻
		{
			CommonAction();
			return;
		}
		FollowAttack(nEnemyIdx);	// 向敌人靠近
		return;
	}

	// 敌人在最大技能攻击范围之内，选择一种技能攻击
	int		nRand;
	nRand = g_Random(100);
	if (nRand < pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(1))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5] + pAIParam[6])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else	// 待机
	{
		return;
	}
	FollowAttack(nEnemyIdx);
}

//------------------------------------------------------------------------------
//	功能：普通被动类3
//	m_AiParam[0] 无敌人时候的巡逻概率
//	m_AiParam[1] 剩余生命低于这个百分比的时候执行相应处理
//	m_AiParam[2] 在m_AiParam[1]的情况出现的时候是否执行相应处理的概率
//	m_AiParam[3] 在m_AiParam[1]的情况出现并决定要执行相应处理，使用攻击技能的概率 对应SkillList里面的技能 1
//	m_AiParam[4、5、6] 三种攻击技能的使用概率，分别对应SkillList里的技能 2 3 4
//	m_AiParam[7、8] 看见敌人但比较远时，待机、巡逻的概率
//------------------------------------------------------------------------------
void	KNpcAI::ProcessAIType06()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;

	int nEnemyIdx = Npc[m_nIndex].GetTargetNpc();
	// 是否受到攻击，否，一定概率选择待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}

	// 是否已超过活动半径
	if (KeepActiveRange())
		return;

	// 检测剩余生命是否符合条件，生命太少一定概率使用攻击技能或逃跑
	if (  Npc[m_nIndex].m_UnaryAttrMgr[nuai_curlife] * 100 / Npc[m_nIndex].m_CompAttrMgr[ncai_lifeuplimit] < pAIParam[1])
	{
		if (g_RandPercent(pAIParam[2]))	// 是否使用攻击技能或逃跑
		{
			if (g_RandPercent(pAIParam[3]))	// 使用攻击技能
			{
				Npc[m_nIndex].SetActiveSkill(0);
				FollowAttack(nEnemyIdx);	// 向敌人靠近
				return;
			}
			else	// 逃跑
			{
				Flee(nEnemyIdx);
				return;
			}
		}
	}

	// 如果敌人在所有技能攻击范围之外，一定概率选择待机/巡逻/向敌人靠近
	if (KNpcSet::GetDistanceSquare(m_nIndex, nEnemyIdx) > pAIParam[MAX_AI_PARAM - 1])
	{
		int		nRand;
		nRand = g_Random(100);
		if (nRand < pAIParam[7])	// 待机
			return;
		if (nRand < pAIParam[7] + pAIParam[8])	// 巡逻
		{
			CommonAction();
			return;
		}
		FollowAttack(nEnemyIdx);	// 向敌人靠近
		return;
	}

	// 敌人在最大技能攻击范围之内，选择一种技能攻击
	int		nRand;
	nRand = g_Random(100);
	if (nRand < pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(1))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5] + pAIParam[6])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else	// 待机
	{
		return;
	}
	FollowAttack(nEnemyIdx);
}

//////////////////////////////////////////////////////////////////////////
// 箭塔最基础AI
#ifdef _SERVER
void KNpcAI::ProcessBuildingAI01()
{
}
#endif

















