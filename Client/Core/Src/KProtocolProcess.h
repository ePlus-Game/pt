//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-6-12 10:26
//      File_base        : KProtocolProcess
//      File_ext         : h
//      Author           : 
//      Description      : 
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef KProtocolProcessH
#define	KProtocolProcessH

#include "KProtocol.h"
class KProtocolProcess
{
private:
#ifndef _SERVER
	void (KProtocolProcess::*ProcessFunc[s2c_end])(BYTE* pMsg);
#else
	void (KProtocolProcess::*ProcessFunc[c2s_end])(int nIndex, BYTE* pMsg, int nSize);
#endif
public:
	KProtocolProcess();
	~KProtocolProcess();
#ifndef _SERVER
	void ProcessNetMsg(BYTE* pMsg);
#else
	void ProcessNetMsg(int nIndex, BYTE* pMsg, int nSize);
#endif
private:
#ifndef _SERVER
	void	s2cNpcInlayCount(BYTE * pMsg);
	void	s2cAccoutLoginResult(BYTE * pMsg);
	void	SyncCurNormalData(BYTE* pMsg);
	void	SyncWorld(BYTE* pMsg);
	void	SyncNpc(BYTE* pMsg);
	void	SyncNpcMin(BYTE* pMsg);
	void	SyncNpcMinPlayer(BYTE* pMsg);
	void	SyncPlayer(BYTE* pMsg);
	void	SyncPlayerMin(BYTE* pMsg);
	void	SyncCurPlayer(BYTE* pMsg);
	void	SyncObjectAdd(BYTE* pMsg);
	void	SyncObjectState(BYTE* pMsg);
	void	SyncObjectDir(BYTE* pMsg);
	void	SyncObjectRemove(BYTE* pMsg);
	void	SyncObjectTrap(BYTE* pMsg);
	void	NetCommandWalk(BYTE* pMsg);
	void	NetCommandRun(BYTE* pMsg);
	void	NetCommandJump(BYTE* pMsg);
	void	NetCommandSkill(BYTE* pMsg);
	void	NetCommandHurt(BYTE* pMsg);
	void	NetCommandDeath(BYTE* pMsg);
	void	NetCommandRemoveNpc(BYTE* pMsg);
	void	NetCommandChgCurCamp(BYTE* pMsg);
	void	NetCommandChgCamp(BYTE* pMsg);
	void	NetCommandSit(BYTE* pMsg);
	void	OpenSaleBox(BYTE* pMsg);
	void	OpenStoreBox(BYTE* pMsg);
	void	s2cUpdataSelfTeamInfo(BYTE* pMsg);
	void	s2cApplyTeamInfoFalse(BYTE* pMsg);
	void	s2cApplyCreateTeamFalse(BYTE* pMsg);
	void	s2cSetTeamState(BYTE* pMsg);
	void	s2cTeamAddMember(BYTE* pMsg);
	void	s2cLeaveTeam(BYTE* pMsg);
	void	s2cTeamChangeCaptain(BYTE* pMsg);
	void	s2cLevelUp(BYTE* pMsg);
	void	s2cSyncSkillInfo(BYTE* pMsg);
	void	s2cSyncAllSkill(BYTE * pMsg);
	void	s2cSyncMoney(BYTE* pMsg);
	void	s2cMoveItem(BYTE* pMsg);
	void	s2cRemoveItem(BYTE* pMsg);
	void	s2cSyncItem(BYTE* pMsg);
 	void	s2cRefreshItem(BYTE *pMsg);
	void	SyncScriptAction(BYTE* pMsg);
	void	SyncEnd(BYTE* pMsg);
	void	s2cTradeChangeState(BYTE* pMsg);
	void	s2cTradeMoneySync(BYTE* pMsg);
	void	s2cTradeDecision(BYTE* pMsg);
	void	s2cTeamInviteAdd(BYTE * pMsg);
	void	s2cPing(BYTE* pMsg);
	void	PlayerRevive(BYTE* pMsg);
	void	RequestNpcFail(BYTE* pMsg);
	void	s2cTradeRequest(BYTE* pMsg);
	void	s2cItemAutoMove(BYTE* pMsg);
	void	s2cPKSyncNormalFlag(BYTE* pMsg);
	void	s2cPKSyncEnmityState(BYTE* pMsg);
	void	s2cPKSyncExerciseState(BYTE* pMsg);
	void	s2cPKValueSync(BYTE* pMsg);
	void	s2cViewEquip(BYTE* pMsg);
	void	EnchaserItemResult(BYTE * pMsg);
	void	s2cCheckStoragePswOK(BYTE* pMsg);
	void	s2cCreateStoragePswOK(BYTE* pMsg);
	void	s2cModifyStoragePswOK(BYTE* pMsg);
	void	s2cSyncTeamMemberInfo(BYTE* pMsg);
	void	s2cCreatureSync(BYTE* pMsg); // lixuewu 同步召唤兽状态
	void	s2cNewPlayer(BYTE* pMsg);
	void	s2c_InitSyncItem(BYTE* pMsg);
	int		SyncItem(const ITEM_SYNC * pItemSync);
	void	s2cByteExtend(BYTE* pMsg);
	void	s2cPetProtocol(BYTE* pMsg);
	void	s2cFindPathSync(BYTE* pMsg);
	void	s2cChangeNpcColor(BYTE* pMsg);
	void	s2cShowDamage(BYTE* pMsg);
	void	s2cNpcRealPosition(BYTE* pMsg);
	void	s2cChatFamily(BYTE *pMsg);
	void	s2cBuffFamily(BYTE* pMsg);
	void	s2cQuestFamily(BYTE *pMsg);
	void	s2cSyncSkillSeries(BYTE* pMsg);
	void	s2cSyncNpcAttr(BYTE *pMsg);
	void	s2cSyncPlayerAttr(BYTE *pMsg);
	void	s2cSyncItemAttr(BYTE * pMsg);
	void	s2cAuctionSync(BYTE *pMsg);
	void	s2cSocialRelationSync(BYTE *pMsg);
	void	s2cSocialRelationInfoSync(BYTE *pMsg);
	void	s2cDelayedAction(BYTE *pMsg);
	void	s2cTalismanFamily(BYTE *pMsg);
	void	s2cSyncTalismanEnchase(BYTE *pMsg);
	void	s2cSyncNpcEquipTalisman(BYTE *pMsg);
	void	s2cTeamInviteRefuse(BYTE *pMsg);
	void	s2cShowPredefinedMsg(BYTE* pMsg);
	void	s2cIBFamily(BYTE *pMsg);
	void	s2cShowBanner(BYTE *pMsg);
	void	s2cShowBannerById(BYTE *pMsg);
	void	s2cGMFeedBack(BYTE *pMsg);
	void	s2cFindFamily(BYTE *pMsg);
	void    s2cTaisuiWheel(BYTE * pMsg);
	void	s2cTeamOperationResult(BYTE *pMsg);
	void	s2cUpdateTeamMemberInfo(BYTE *pMsg);
	void	s2cPrompt(BYTE *pMsg);
	void	s2cCancelPrompt(BYTE *pMsg);
	void    s2cPlayerStop(BYTE * pMsg);
	void    s2cPosEdition(BYTE * pMsg);
	void    s2cFurySync(BYTE * pMsg);
	void	s2cApplyJoinTeam(BYTE * pMsg);
	void	s2cNpcSyncToWorld(BYTE *pMsg);
	void	s2cNpcSyncToWorldMin(BYTE *pMsg);
	void	s2cNpcSyncToWorldDel(BYTE *pMsg);
	void	s2cListTeam(BYTE *pMsg);
	void	s2cSpecialQuestData(BYTE *pMsg);
	void	s2cHireDataListExp(BYTE *pMsg);
	void	s2cHireDataListFighter(BYTE *pMsg);
	void	s2cHireRetCode(BYTE *pMsg);
	void    s2cWorldCombatInfo(BYTE* pMsg);
	void    s2cCommoFlag(BYTE * pMsg);
	void    s2cListStudent(BYTE * pMsg);
	void	s2cWorldCombatTop10Info(BYTE * pMsg);
	void    s2cInsurance(BYTE * pMsg);
	void    s2cWorldPlayerSync(BYTE * pMsg);
	void	s2cWarCommanderSync(BYTE * pMsg);
	void	s2cWorldCustomString(BYTE * pMsg);
	void	s2cChangeTitle(BYTE * pMsg);
	void	s2cUpdateSelfTitle(BYTE * pMsg);
	void	s2cSyncSelfTitle(BYTE * pMsg);
	void	s2cSelectTitleResult(BYTE * pMsg);
	void    s2cPlusPointTopN(BYTE * pMsg);
	void	s2cShizuPopularityTopN(BYTE * pMsg);
	void	s2cZhuhouPopularityTopN(BYTE * pMsg);
	void	s2cPlayerProperties(BYTE * pMsg);
	void    SyncCurNormalDataEx(BYTE * pMsg);
	void    s2cPlayerRealInfoSync(BYTE * pMsg);

	void	s2cExtend(BYTE* pMsg);
	void	s2cExtendChat(BYTE* pMsg);
	void	s2cExtendFriend(BYTE* pMsg);
#else
	void	NpcRequestCommand(int nIndex, BYTE* pMsg, int nSize);
	void	ObjRequestCommand(int nIndex, BYTE* pProtocol, int nSize);
	void	NpcRunCommand(int nIndex, BYTE* pProtocol, int nSize);
	void	NpcSkillCommand(int nIndex, BYTE* pProtocol, int nSize);
	void	PlayerApplyTeamInfo(int nIndex, BYTE* pProtocol, int nSize);
	void	PlayerApplyCreateTeam(int nIndex, BYTE* pProtocol, int nSize);
	void	PlayerApplyLeaveTeam(int nIndex, BYTE* pProtocol, int nSize);
	void	PlayerApplyTeamKickMember(int nIndex, BYTE* pProtocol, int nSize);
	void	PlayerApplyTeamChangeCaptain(int nIndex, BYTE* pProtocol, int nSize);
	void	PlayerApplyTeamDismiss(int nIndex, BYTE* pProtocol, int nSize);
	void	PlayerEatItem(int nIndex, BYTE* pProtocol, int nSize);
	void	PlayerPickUpItem(int nIndex, BYTE* pProtocol, int nSize);
	void	PlayerMoveItem(int nIndex, BYTE* pProtocol, int nSize);
	void	PlayerSellItem(int nIndex, BYTE* pProtocol, int nSize);
	void	PlayerBuyItem(int nIndex, BYTE* pProtocol, int nSize);
	void	PlayerDropItem(int nIndex, BYTE* pProtocol, int nSize);
	void	PlayerSelUI(int nIndex, BYTE* pProtocol, int nSize);
	void	c2sTradeRequest(int nIndex, BYTE* pProtocol, int nSize);	
	void	TradeMoveMoney(int nIndex, BYTE* pProtocol, int nSize);
	void	TradeDecision(int nIndex, BYTE* pProtocol, int nSize);
	void	DialogNpc(int nIndex, BYTE * pProtocol, int nSize);
	void	TeamInviteAdd(int nIndex, BYTE * pProtocol, int nSize);
	void	TeamReplyInvite(int nIndex, BYTE * pProtocol, int nSize);
	void	ObjMouseClick(int nIndex, BYTE* pProtocol, int nSize);
	void	StoreMoneyCommand(int nIndex, BYTE* pProtocol, int nSize);
	void	NpcReviveCommand(int nIndex, BYTE* pProtocol, int nSize);
	void	c2sTradeReplyStart(int nIndex, BYTE* pProtocol, int nSize);
	void	c2sViewEquip(int nIndex, BYTE* pProtocol, int nSize);
	void	LadderQuery(int nIndex, BYTE* pProtocol, int nSize);
	void	ItemRepair(int nIndex, BYTE* pProtocol, int nSize);
	void	EnchaserItem(int nIndex, BYTE * pProtocol, int nSize);
	int		IsPermitedNow(int nIndex, BYTE* pMsg, int nSize); //根据目前的状态判断是否禁止该协议,1:允许,0:禁止	    

	void	c2sSplitPileItem(int nIndex, BYTE* pMsg, int nSize);
	void	c2sCheckStoragePassword(int nIndex, BYTE* pMsg, int nSize);
	void	c2sCreateStoragePassword(int nIndex, BYTE* pMsg, int nSize);
	void	c2sModifyStoragePassword(int nIndex, BYTE* pMsg, int nSize);
	void	c2sCloseStorage(int nIndex, BYTE* pMsg, int nSize);
	void	c2sByteExtend(int nIndex, BYTE* pMsg, int nSize);

	void	c2sPetProtocol(int nIndex, BYTE* pMsg, int nSize);

	void	c2sChatFamily(int nIndex, BYTE *pMsg, int nSize);
	void	c2sBuffFamily(int nIndex, BYTE *pMsg, int nSize);
	void	c2sQuestFamily(int nIndex, BYTE *pMsg, int nSize);

	void	c2sChgPKMode(int nIndex, BYTE *pMsg, int nSize);
	void	c2sSkillSync(int nIndex, BYTE *pMsg, int nSize);
	void	c2sAuctionSync(int nIndex, BYTE *pMsg, int nSize);

	void	c2sPlayerStopNotify(int nIndex, BYTE *pMsg, int nSize);
	void	c2sSocialRelationSync(int nIndex, BYTE *pMsg, int nSize);
	void	c2sTalismanFamily(int nIndex, BYTE *pMsg, int nSize);
	void	c2sPlayerLogout(int nIndex, BYTE *pMsg, int nSize);
	void	c2sIBFamily(int nIndex, BYTE *pMsg, int nSize);
	void	c2sFindFamily(int nIndex, BYTE *pMsg, int nSize);
	void    c2sTaisuiWheel(int nIndex,BYTE * pMsg,int nSize);
	void	c2sTeamOperation(int nIndex, BYTE *pMsg, int nSize);
	void	c2sReplyPrompt(int nIndex, BYTE *pMsg, int nSize);
	void    c2sPosSync(int nIndex, BYTE *pMsg, int nSize);
	void	c2sSelectSkill(int nIndex, BYTE *pMsg, int nSize);
	void    c2sFuryExplode(int nIndex, BYTE *pMsg,int nSize);
	void	c2sRequestSyncToWorldNpc(int nIndex, BYTE *pMsg, int nSize);
	void	c2sRequestTeamList(int nIndex, BYTE *pMsg, int nSize);
	void	c2sReqSpecialQuestData(int nIndex, BYTE *pMsg, int nSize);
	void	c2sCMCommunication(int nIndex, BYTE *pMsg, int nSize);
	void	c2sReqTobeHired(int nIndex, BYTE *pMsg, int nSize);
	void	c2sReqHireList(int nIndex, BYTE *pMsg, int nSize);
	void	c2sReqHire(int nIndex, BYTE *pMsg, int nSize);
	void	c2sInteractiveScriptInput(int nIndex, BYTE *pMsg, int nSize);
	void	c2sRecommender(int nIndex, BYTE *pMsg, int nSize);
	void    c2sInsurance(int nIndex,BYTE * pMsg,int nSize);
	void	c2sSelectTitle(int nIndex,BYTE * pMsg,int nSize);
	void    c2sPlusPointTopN(int nIndex , BYTE * pMsg,int nSize);
	void    c2sPlayerRealInfoSync(int nIndex, BYTE * pMsg, int nSize);
#endif
};

extern KProtocolProcess g_ProtocolProcess;
#endif