//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KViewItem.cpp
// Date:	2003.07.28
// Code:	边城浪子
// Desc:	KViewItem Class
//---------------------------------------------------------------------------

#include	"KCore.h"

#ifndef _SERVER
#include	"CoreShell.h"
#include	"KViewItem.h"
#include	"KNpcSet.h"

KViewItem	g_cViewItem;

KViewItem::KViewItem()
{
	Init();
}

void KViewItem::Init()
{
	m_npcId = 0;
	m_level = 1;	
	memset(m_name, 0, sizeof(m_name));
	memset(m_sItem, 0, sizeof(m_sItem));
	m_ArmorSetMonitor.Init();
	m_YaoMonitor.Init();	
}

void	KViewItem::ApplyViewEquip(DWORD dwNpcID)
{
	VIEW_EQUIP_COMMAND	sView;
	sView.ProtocolType = c2s_viewequip;
	sView.m_dwNpcID = dwNpcID;
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,&sView, sizeof(sView));
}

void	KViewItem::DeleteAll()
{
	m_npcId	= 0;
	m_level	= 1;

	m_name[0] = 0;
	m_chenghao[0] = 0;

	m_shizu[0] = 0;
	m_zhuhou[0] = 0;

	for (int i = 0; i < itempart_num; i ++)
	{
		if (m_sItem[i].nIdx > 0)
			ItemSet.Remove(m_sItem[i].nIdx);
		m_sItem[i].nIdx = 0;
		m_EquipItem[i].nEquipIdx = -1;
	}
}

#define VIEW_ITEM_MAX_BUFF  4000

void KViewItem::GetData(BYTE* pMsg)
{
	if (NULL == pMsg)
		return;

	DeleteAll();

	VIEW_EQUIP_SYNC	*pViewMsg = (VIEW_EQUIP_SYNC*)pMsg;
	//Decompression................................
	int nOldSize = pViewMsg->Len - sizeof(VIEW_EQUIP_SYNC) + 1 + PROTOCOL_SIZE;
	
	unsigned char szCompressionBuff[VIEW_ITEM_MAX_BUFF];
	unsigned int nLen = VIEW_ITEM_MAX_BUFF;
	lzo1x_decompress(
		pViewMsg->data,
		nOldSize,
		(unsigned char *)szCompressionBuff,
		&nLen,
		NULL);
	//Decompression end...................................................
	VIEW_EQUIP_SYNC_INFO * pView = (VIEW_EQUIP_SYNC_INFO *)szCompressionBuff;

	m_npcId   = pView->npcId;
	m_pkValue = pView->pkValue;

	int npcIndex = NpcSet.SearchID(m_npcId);
	if (npcIndex > 0)
	{
		m_level = Npc[npcIndex].m_Level;
		strcpy(m_name, Npc[npcIndex].Name);
	}

	strcpy(m_chenghao, pView->chenghao);
	strcpy(m_shizu, pView->shizu);
	strcpy(m_zhuhou, pView->zhuhou);
	strcpy(m_lianmen, pView->lianmen);

	for(int i = 0; i < itempart_num; i ++)
	{
		if (pView->m_sInfo[i].m_ID == 0)
			continue;

		int itemIndex = ItemSet.Add(
			pView->m_sInfo[i].m_Genre,
			pView->m_sInfo[i].m_Detail,
			pView->m_sInfo[i].m_Particur,
			pView->m_sInfo[i].m_Level,1);

		if (itemIndex <= 0)
			continue;
		
		m_EquipItem[i].nEquipIdx = itemIndex;

		Item[itemIndex].SetID(pView->m_sInfo[i].m_ID);
		Item[itemIndex].SetMaxDurability(pView->m_sInfo[i].m_MaxDurability);
		Item[itemIndex].SetDurability(pView->m_sInfo[i].m_Durability);
		Item[itemIndex].SetLevelupTimes(pView->m_sInfo[i].m_LevelupTimes);	
		Item[itemIndex].SetLevelupType(pView->m_sInfo[i].m_nLevelupType);
		Item[itemIndex].SetYaoID(pView->m_sInfo[i].m_YaoID);
		for (int yaoAddonBuffLoopCount = 0; yaoAddonBuffLoopCount < YAO_ADDON_BUFF_COUNT; yaoAddonBuffLoopCount++)
		{
			Item[itemIndex].SetYaoAddOn(yaoAddonBuffLoopCount, pView->m_sInfo[i].m_YaoAddOnBuffSet[yaoAddonBuffLoopCount]);
		}	
		Item[itemIndex].SetCompBuffTemplateSet( pView->m_sInfo[i].m_compBuffTemplateSet, sizeof( WORD ) * COMPOUND_COUNT );
		Item[itemIndex].SetPlusInfo( (char*)pView->m_sInfo[i].m_szPlusInfo );
		Item[itemIndex].SetTalismanPotential(pView->m_sInfo[i].m_TalismanPotential);
		for (int talismanEnchaseLoopCount = 0; talismanEnchaseLoopCount < TM_HOLE_NUM; talismanEnchaseLoopCount++)
		{
			Item[itemIndex].SetTalismanEnchase(talismanEnchaseLoopCount, pView->m_sInfo[i].m_TalismanEnchaseSet[talismanEnchaseLoopCount]);
		}	
		Item[itemIndex].SetBind( !pView->m_sInfo[i].m_bExchange );

		Item[itemIndex].ClearSocketSet();
		for ( int nSocketIdx = 0; nSocketIdx < MAX_INLAY_COUNT; ++nSocketIdx )
		{
			InlayStuff stuff;
			if ( pView->m_sInfo[i].m_socketSet[nSocketIdx].nGenre == -1 &&
				pView->m_sInfo[i].m_socketSet[nSocketIdx].nDetail == -1 &&
				pView->m_sInfo[i].m_socketSet[nSocketIdx].nParticular == -1 &&
				pView->m_sInfo[i].m_socketSet[nSocketIdx].nLevel == -1 )
			{
				continue;
			}
			else
			{

				stuff.nGenre		= pView->m_sInfo[i].m_socketSet[nSocketIdx].nGenre;
				stuff.nDetail		= pView->m_sInfo[i].m_socketSet[nSocketIdx].nDetail;
				stuff.nParticular	= pView->m_sInfo[i].m_socketSet[nSocketIdx].nParticular;
				stuff.nLevel		= pView->m_sInfo[i].m_socketSet[nSocketIdx].nLevel;
				if ( stuff.nGenre == 0 && 
					stuff.nDetail == 0 &&
					stuff.nParticular == 0 && 
					stuff.nLevel == 0 )
				{
					Item[itemIndex].CreateSocket();
				}
				else
				{
					Item[itemIndex].SetSocketSet( stuff );
				}
				
			}
		}
		Item[itemIndex].SetInlayBaseBuffSet((short *)pView->m_sInfo[i].m_InlayBaseBuffSet );
		Item[itemIndex].SetInlayYaoBuffSet((short *)pView->m_sInfo[i].m_InlayYaoBuffSet );
		Item[itemIndex].SetInlaySpecialBuffSet((short *)pView->m_sInfo[i].m_InlaySpecialBuffSet );

		m_sItem[i].nIdx = itemIndex;
	}

	m_ArmorSetMonitor.Update(0, m_EquipItem);
	m_YaoMonitor.Update(0, m_EquipItem);
	
	//通知界面
	CoreDataChanged(GDCNI_EQUIPMENT_VIEW_NOTIFY, NULL, NULL);
}





#endif













