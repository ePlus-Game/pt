#include "UiMDLInterface.h"
#include "GameDataDef.h"
#include "SocialComDef.h"
#include "CoreShell.h"
#include "chatWindow/ChatClanPlayerData.h"

extern iCoreShell* g_pCoreShell;

ChatClanPlayerData::ChatClanPlayerData()
: m_iPlayerNum(0)
{
	memset(m_pClanName, 0, CLIENT_NAME_AND_TITLE_MAX);
	memset(m_DataList, 0, sizeof(TongPageData) * _CLAN_MAX_MEMBER_NUM);
}

ChatClanPlayerData::~ChatClanPlayerData()
{

}

BOOL ChatClanPlayerData::InitInterface()
{
	if (m_pInterface != NULL)
	{
		return TRUE;
	}
	GetMDLPtr(&m_pInterface);
	if (m_pInterface != NULL)
	{
		TongOperParam tagTongParam;
		IUIMDLDataset* pOperDataset = NULL; 
		if ( success_errorcode != m_pInterface ->queryDataSet(tong_dataset, &pOperDataset))
		{
			m_pInterface = NULL;
			return FALSE;
		}
		
		if (pOperDataset != NULL)
		{
			pOperDataset->setEventHandle((IUIMDLEvent *)&ChatClanPlayerData::GetPlayerData());
		}
		else
		{
			m_pInterface = NULL;
			return FALSE;
		}
		return TRUE;
	}
	else
	{
		m_pInterface = NULL;
		return FALSE;
	}
}

void ChatClanPlayerData::onCreate(UIMDLEvent& rEvent)
{

}

void ChatClanPlayerData::onRelease(UIMDLEvent& rEvent)
{

}

void ChatClanPlayerData::onChange(UIMDLEvent& rEvent)
{
	int index = 0;
	m_iPlayerNum = 0;
	index = rEvent.nRecordIndex;
	IUIMDLDataset * dataset = NULL;
	dataset = rEvent.pDataSet;
	if (dataset != NULL)
	{
		UIMDLDatasetRecord * record = NULL;
		record = &dataset ->getDataRecord(index);
		if (record != NULL)
		{
			TongData * playerData = NULL;
			playerData = (TongData *)record ->pRecordData;
			if (playerData != NULL)
			{
				switch (playerData ->eOperationClientID)
				{
				case get_society_baseinfo_name:
					{
						memcpy(m_pClanName, playerData ->szName, COMMON_CLIENT_MSG_LEN_16);
					}
					break;
				case get_society_memberlist_operation:
					{
						for (int i = 0; i < TONGMEMBER_MAX_NUM - 10; i++)
						{
							if (playerData ->memberList[i].szName[0] == NULL)
							{
								continue;
							}
							else
							{
								memcpy(&m_DataList[m_iPlayerNum], &playerData ->memberList[i], sizeof(TongPageData));
								m_iPlayerNum++;
							}
						}
						PostMessage(m_hCallWnd, WM_DATA_REQUEST_SUCCEED, 0, 0);
						m_hCallWnd = NULL;
					}
					break;
				case get_society_shizulist_operation:
					{
						for (int i = 0; i < TONGMEMBER_MAX_NUM - 10; i++)
						{
							if (playerData ->memberList[i].szName[0] == NULL)
							{
								continue;
							}
							else
							{
								memcpy(&m_DataList[m_iPlayerNum], &playerData ->memberList[i], sizeof(TongPageData));
								m_iPlayerNum++;
							}
						}
						PostMessage(m_hGetListWnd, WM_CLAN_DATA_REQUEST_SUCCEED, 0, 0);
						m_hGetListWnd = NULL;
					}
					break;
				case get_society_baseinfo_info:
					{
						if (playerData ->nLayerID == enSULayer_Gens)
						{
							strcpy(m_ClanAnnoucement, playerData ->szTip);
						}
						else if (playerData ->nLayerID == enSULayer_Tong)
						{
							strcpy(m_LuedAnnoucement, playerData ->szTip);
						}
					}
					break;
				}
			}
		}
	}
}

ChatClanPlayerData & ChatClanPlayerData::GetPlayerData()
{
	static ChatClanPlayerData playData;
	return playData;
}

void ChatClanPlayerData::RequestDataList(HWND callWnd, int layerID, FSGUID * id)
{
	if (callWnd == NULL)
	{
		return;
	}

	SocietyInfoIndex tagSocietyIdx;
	tagSocietyIdx.TemplateId	= enSUTplId_Tong;
	int nTopLayer = 0;
	g_pCoreShell->GetGameData( GDI_GET_SOCIETY_PLAYER, (unsigned int)&tagSocietyIdx, (int)&nTopLayer );

	if (m_pInterface != NULL)
	{
		FSGUID guid;
		TongOperParam tagTongOper;
		ZeroMemory(&tagTongOper, sizeof(tagTongOper));
		tagTongOper.nTemplateID		= enSUTplId_Tong;
		tagTongOper.nOperationID	= enSUO_GetSubList;
		if (id != NULL)
		{
			tagTongOper.id = *id;
		}
		else
		{
			tagTongOper.id = guid;
		}
		switch(layerID)
		{
		case enSULayer_Gens:
			{
				if (nTopLayer > 1)
				{
					tagTongOper.nLayerID = enSULayer_Gens;
					IUIMDLDataset* pTongOper = NULL;
					if ( success_errorcode != m_pInterface ->queryDataSet(tong_operation, &pTongOper))
					{
						return;
					}
					if (pTongOper)
					{
						m_hCallWnd = callWnd;
						pTongOper ->updateRecord(tagTongOper.nOperationID, &tagTongOper, sizeof(TongOperParam));
					}
				}
			}
			break;
		case enSULayer_Tong:
			{
				if (nTopLayer > 2)
				{
					tagTongOper.nLayerID = enSULayer_Tong;
					IUIMDLDataset* pTongOper = NULL;
					if ( success_errorcode != m_pInterface ->queryDataSet(tong_operation, &pTongOper))
					{
						return;
					}
					if (pTongOper)
					{
						m_hGetListWnd = callWnd;
						pTongOper ->updateRecord(tagTongOper.nOperationID, &tagTongOper, sizeof(TongOperParam));
					}
				}
			}
			break;			
		}
	}
}

void ChatClanPlayerData::AddMember(const char * name, HWND callWnd, int layerID)
{
	if (name == NULL)
	{
		return;
	}
	int nameLength = 0;
	nameLength = strlen(name);
	if (nameLength > CLIENT_NAME_AND_TITLE_MAX)
	{
		return;
	}
	if (m_pInterface != NULL)
	{
		FSGUID guid;
		TongOperParam tagTongOper;
		ZeroMemory(&tagTongOper, sizeof(tagTongOper));
		memcpy(tagTongOper.szName, name, strlen(name));
		tagTongOper.nTemplateID		= enSUTplId_Tong;
		tagTongOper.nOperationID	= enSUO_AddSubUnit;
		tagTongOper.id				= guid;

		switch(layerID)
		{
		case enSULayer_Gens:
			{
				tagTongOper.nLayerID = enSULayer_Gens;
			}
			break;
		case enSULayer_Tong:
			{
				tagTongOper.nLayerID = enSULayer_Tong;
			}
			break;
		}

		IUIMDLDataset* pTongOper = NULL;
		if ( success_errorcode != m_pInterface ->queryDataSet(tong_operation, &pTongOper))
		{
			return;
		}
		if (pTongOper)
		{
			m_hCallWnd = callWnd;
			pTongOper ->updateRecord(tagTongOper.nTemplateID, &tagTongOper, sizeof(TongOperParam));
		}
	}
}

void ChatClanPlayerData::DeleteMember(FSGUID guid, HWND callWnd, int layerID)
{
	if (guid.data == NULL)
	{
		return;
	}
	if (m_pInterface != NULL)
	{
		TongOperParam tagTongOper;
		ZeroMemory(&tagTongOper, sizeof(tagTongOper));
		tagTongOper.nTemplateID		= enSUTplId_Tong;
		tagTongOper.nOperationID	= enSUO_RemoveSubUnit;
		tagTongOper.id				= guid;

		switch(layerID)
		{
		case enSULayer_Gens:
			{
				tagTongOper.nLayerID = enSULayer_Gens;
			}
			break;
		case enSULayer_Tong:
			{
				tagTongOper.nLayerID = enSULayer_Tong;
			}
			break;
		}

		IUIMDLDataset* pTongOper = NULL;
		if (success_errorcode != m_pInterface ->queryDataSet(tong_operation, &pTongOper))
		{
			return;
		}
		if (pTongOper)
		{
			m_hCallWnd = callWnd;
			pTongOper ->updateRecord(tagTongOper.nOperationID, &tagTongOper, sizeof(TongOperParam));
		}
	}
}

void ChatClanPlayerData::Demise(FSGUID guid, int layerID)
{
	if (guid.data == NULL)
	{
		return;
	}
	if (m_pInterface != NULL)
	{
		TongOperParam tagTongOper;
		ZeroMemory(&tagTongOper, sizeof(tagTongOper));
		tagTongOper.nTemplateID		= enSUTplId_Tong;
		tagTongOper.nOperationID	= enSUO_ChangeOwner;
		tagTongOper.id				= guid;

		switch(layerID)
		{
		case enSULayer_Gens:
			{
				tagTongOper.nLayerID = enSULayer_Gens;
			}
			break;
		case enSULayer_Tong:
			{
				tagTongOper.nLayerID = enSULayer_Tong;
			}
			break;
		}

		IUIMDLDataset* pTongOper = NULL;
		if (success_errorcode != m_pInterface ->queryDataSet(tong_operation, &pTongOper))
		{
			return;
		}
		if (pTongOper)
		{
			pTongOper ->updateRecord(tagTongOper.nOperationID, &tagTongOper, sizeof(TongOperParam));
		}
	}
}

void ChatClanPlayerData::ForbidChat(FSGUID guid, int layerID)
{
	if (guid.data == NULL)
	{
		return;
	}
	if (m_pInterface != NULL)
	{
		TongOperParam tagTongOper;
		ZeroMemory(&tagTongOper, sizeof(tagTongOper));
		tagTongOper.nTemplateID		= enSUTplId_Tong;
		tagTongOper.nOperationID	= enSUO_ForbidChat;
		tagTongOper.id				= guid;

		switch(layerID)
		{
		case enSULayer_Gens:
			{
				tagTongOper.nLayerID = enSULayer_Gens;
			}
			break;
		case enSULayer_Tong:
			{
				tagTongOper.nLayerID = enSULayer_Tong;
			}
			break;
		}

		IUIMDLDataset* pTongOper = NULL;
		if (success_errorcode != m_pInterface ->queryDataSet(tong_operation, &pTongOper))
		{
			return;
		}
		if (pTongOper)
		{
			pTongOper ->updateRecord(tagTongOper.nOperationID, &tagTongOper, sizeof(TongOperParam));
		}
	}
}

void ChatClanPlayerData::UnforbidChat(FSGUID guid, int layerID)
{
	if (guid.data == NULL)
	{
		return;
	}
	if (m_pInterface != NULL)
	{
		TongOperParam tagTongOper;
		ZeroMemory(&tagTongOper, sizeof(tagTongOper));
		tagTongOper.nTemplateID		= enSUTplId_Tong;
		tagTongOper.nOperationID	= enSUO_UnForbidChat;
		tagTongOper.id				= guid;

		switch(layerID)
		{
		case enSULayer_Gens:
			{
				tagTongOper.nLayerID = enSULayer_Gens;
			}
			break;
		case enSULayer_Tong:
			{
				tagTongOper.nLayerID = enSULayer_Tong;
			}
			break;
		}

		IUIMDLDataset* pTongOper = NULL;
		if (success_errorcode != m_pInterface ->queryDataSet(tong_operation, &pTongOper))
		{
			return;
		}
		if (pTongOper)
		{
			pTongOper ->updateRecord(tagTongOper.nOperationID, &tagTongOper, sizeof(TongOperParam));
		}
	}
}

void ChatClanPlayerData::ModifyAnnoucement(const char * text, int layerID)
{
	if (text == NULL)
	{
		return;
	}
	if (m_pInterface != NULL)
	{
		TongOperParam tagTongOper;
		ZeroMemory(&tagTongOper, sizeof(tagTongOper));
		tagTongOper.nTemplateID		= enSUTplId_Tong;
		tagTongOper.nOperationID	= enSUO_PubAnnouncement;
		strcpy(tagTongOper.szTip, text);

		switch(layerID)
		{
		case enSULayer_Gens:
			{
				tagTongOper.nLayerID = enSULayer_Gens;
			}
			break;
		case enSULayer_Tong:
			{
				tagTongOper.nLayerID = enSULayer_Tong;
			}
			break;
		}

		IUIMDLDataset* pTongOper = NULL;
		if (success_errorcode != m_pInterface ->queryDataSet(tong_operation, &pTongOper))
		{
			return;
		}
		if (pTongOper)
		{
			pTongOper ->updateRecord(tagTongOper.nOperationID, &tagTongOper, sizeof(TongOperParam));
		}
	}
}

void ChatClanPlayerData::GetAnnoucement(int layerID)
{
	if (m_pInterface != NULL)
	{
		TongOperParam tagTongOper;
		ZeroMemory(&tagTongOper, sizeof(tagTongOper));
		tagTongOper.nTemplateID		= enSUTplId_Tong;
		tagTongOper.nOperationID	= enSUO_GetAnnouncement;

		switch(layerID)
		{
		case enSULayer_Gens:
			{
				tagTongOper.nLayerID = enSULayer_Gens;
			}
			break;
		case enSULayer_Tong:
			{
				tagTongOper.nLayerID = enSULayer_Tong;
			}
			break;
		}

		IUIMDLDataset* pTongOper = NULL;
		if (success_errorcode != m_pInterface ->queryDataSet(tong_operation, &pTongOper))
		{
			return;
		}
		if (pTongOper)
		{
			pTongOper ->updateRecord(tagTongOper.nOperationID, &tagTongOper, sizeof(TongOperParam));
		}
	}
}