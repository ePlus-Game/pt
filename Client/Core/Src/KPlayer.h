#ifndef KPlayerH
#define	KPlayerH

#include "KInventory.h"
#include "KPlayerTask.h"
#include "KPlayerTrade.h"
#include "IBMoney.h"
#include "insurance_mgr.h"

#ifndef _SERVER
#include "KPlayerTeam_C.h"
#include <utility>
#include <list>
#else
#include "KPlayerTeam_S.h"
#endif

#include "world_combat_instance.h"

#include "KItemList.h"
#include "KNpc.h"
#include "KPlayerDef.h"
#include "KLevelUp.h"
#include "KCreature.h"
#include "BaseValue.h"
#include "ConfigManager.h"

#ifdef _SERVER
#include "time.h"
#include "KItemCompounder.h"
#endif

#ifndef WIN32
#define min(x,y) x<y?x:y
#endif

#include <set>
#ifdef _SERVER
#include "CoreServerShell.h"
#endif

#ifdef _SERVER
#include "ServerAuctionMgr.h"
#include "server_playerrealinfo_mgr.h"
#else
#include "ClientAuctionMgr.h"
#endif

#include "relation_set.h"

#ifndef _SERVER
#include "ClientSocialUnitMgr.h"
#include "client_social_relation.h"
#include "client_playerrealinfo_mgr.h"
#endif

#ifdef _SERVER
#include "AntiEnthrall.h"
#endif

#ifdef _SERVER
#include "action_delayer.h"
#include "player_statistic.h"
#include "KServerFuryMgr.h"
#include "employ.h"
#include "question.h"
#endif

#include "ai_player_controller.h"
#include "exp_insruance.h"
#include "pluspoint.h"
#include "title.h"

using namespace std;

#define		MAX_ANSWERNUM					10
#define		PLAYER_LIFE_REPLENISH			0
#define		PLAYER_MANA_REPLENISH			0

#define		STRENGTH_SET_DAMAGE_VALUE		5
#define		DEXTERITY_SET_DAMAGE_VALUE		5

#define		MAX_AVENGE_NUM					4

#define		MAX_BOXPASSWORD					16

#ifdef _SERVER
#define		MAXNUM_PRENTICE					3
#define		MARRY_FAILED_MSG_ID				30000
#define		DIVORCE_FAILED_MSG_ID			30010
#define		COUPLE_DEAD_MSG_ID				30020
#endif


#define		CORDIAL_SKILL_ID_START			52
#define		CORDIAL_SKILL_ID_END			61

#ifdef _SERVER
#define		TIME_WEARINESS					180		//minute
#define		TIME_INSALUBRITY				300		//minute
#define		DEFAULT_RECOMMENDER_OP_INTERVAL	5		//默认推荐人操作间隔
#endif

#define		MAX_DWORD_VALUE					3000000000		//合法的DWORD值上限
#define		MAX_INT_VALUE					2000000000		//合法的INT值上限
#define		MAX_ADD_EXP						1000000000		//添加经验上限
#define		MAX_EXP							MAX_DWORD_VALUE		//经验上限
#define		MAX_ADD_SKILL_EXP				1000000000		//添加蕴魂上限
#define		MAX_SKILL_EXP					MAX_DWORD_VALUE		//蕴魂上限
#define		MAX_ADD_EMPLOY_TIME				1000000000		//添加雇用时间上限
#define		MAX_EMPLOY_TIME_LIMIT			MAX_DWORD_VALUE		//雇用时间上限
#define		MAX_RECOMMENDER_REWARD			MAX_INT_VALUE	//奖励代金券数量上限
#define		MAX_ADD_RECOMMENDER_REWARD		10000000		//添加奖励代金券数量上限
#define		MAX_ACTIVE_DEGREE				1000000000		//最大活跃度
#define		DEFAULT_NEW_CONSUME_POINT_INDEX	13				//默认新消费积分序号

#ifdef _SERVER
#define MAX_MARRIED_TIMES	9999		//最大结婚次数
#endif

#include "KTaisuiWheel.h"

enum	UIInfo							// 脚本通知显示的界面类型
{
	UI_SELECTDIALOG,
	UI_TALKDIALOG,
	UI_NOTEINFO,
	UI_MSGINFO,							// 自右向左冒出来的信息
	UI_PLAYMUSIC,
	UI_OPENTONGUI,
	UI_ITEM_UPDATE,						// 升级
	UI_ITEM_ADDMAGIC,					// 加持
	UI_ITEM_SETYAO,						// 附爻
	UI_ITEM_GETYAO,						// 拆爻
	UI_ITEM_MAKE,
	UI_CHANGECAMPTONGUI,				// 帮会的改变帮会阵营的界面
	UI_INPUTDIALOG,						// 弹出输入数字的对话框，脚本要求
	UI_CLOSE_DIALOG,					// 关闭对话界面
	UI_TOP_INFO,						// 顶部的和拣东西一样的提示信息
	UI_SKILL_STUDY_DLG,					// 技能学习窗口
	UI_MAIL_CENTRE,						// 邮件窗口
	UI_CREATE_SHIZU,					// 创建氏族
	UI_CREATE_ZHUHOU,					// 创建诸侯
	UI_CITY_DLG,						// 城市窗口
	UI_AUCTION_DLG,						// 拍卖窗口
	UI_ACCEPT_QUEST,					// 接任务面板
	UI_SHOW_QUEST,						// 交任务面板
	UI_TAISUI_DLG,                      // 太岁之轮面板
	UI_MOVIE_SCENE,						// 电影场景
	UI_NAVIGATION_WND,					// 功能面板
	UI_NAVIGATIONEX_WND,				// 扩展功能面板
	UI_ACTIVE_NAVIGATION_BUTTON,		// 扩展功能面板
	UI_SHORTCUT_WND,					// 快捷栏面板
	UI_SHORTCUTPLUS_WND,				// 扩展快捷栏面板
	UI_OPEN_ANY_WINDOW,					// 打开窗口 nparam=windowname
	UI_OPEN_TIMER,						// 打开计时器
	UI_OPEN_TONG_RECRUIT,               // 打开社会关系招募中心
	UI_OPEN_INSTANCE_REWARD_WND,		// 打开副本奖励
	UI_OPEN_CREDIT_SHOP_WND,			// 打开信用商店
	UI_OPEN_USEITEM_DLG,				// 拍卖窗口
	UI_WORLD_COMBAT_SCORE,              // 战场界面
	UI_STUDENT_REPORT,					// 被推荐人汇报
	UI_CREATE_LIANMENG,                 // 创建联盟
};

enum  SelectUIType
{
	select_sayortalk,
	select_inputdialog,
};

struct MarriageInfo
{
	char			m_CoupleName[32];
	unsigned long	m_CoupleLastOffLineTime;
	unsigned long	m_MarriageTime;		//结婚时间
	unsigned short	m_MarriedTimes;		//结婚次数

	MarriageInfo()
	{
		memset(m_CoupleName, 0, sizeof(m_CoupleName));
		m_CoupleLastOffLineTime = 0;
		m_MarriageTime			= 0;
		m_MarriedTimes			= 0;
	}

	MarriageInfo & operator = (MarriageInfo & marriageInfo)
	{
		memcpy(m_CoupleName, marriageInfo.m_CoupleName, sizeof(m_CoupleName));
		m_CoupleLastOffLineTime = marriageInfo.m_CoupleLastOffLineTime;
		m_MarriageTime			= marriageInfo.m_MarriageTime;
		m_MarriedTimes			= marriageInfo.m_MarriedTimes;
	}

	void ResetInfo()
	{
		memset(m_CoupleName, 0, sizeof(m_CoupleName));
		m_CoupleLastOffLineTime = 0;
		m_MarriageTime			= 0;
	}
};

// 重生点位置信息
typedef struct PLAYER_REVIVAL_POS_DATA
{
	int				m_nSubWorldID;		// 重生点地图
	int				m_ReviveID;			// 重生点索引
	int				m_nMpsX;			// 重生点地图位置 x
	int				m_nMpsY;			// 重生点地图位置 y
} PLAYER_REVIVAL_POS;


typedef struct
{
	int				m_nSubWorldId;		// 传送门世界ID
	int				m_nTime;			// 传送门保持时间
	int				m_nMpsX;
	int				m_nMpsY;
} PLAYER_TOWNPORTAL_POS;

typedef struct 
{
	DWORD			m_dwMapID;
	int				m_nX;
	int				m_nY;
} PLAYER_EXCHANGE_POS;

typedef struct
{
	BYTE	shopType;
	int		m_nBuyIdx;
	int		m_nSmithShopIdx;
	DWORD	m_SubWorldID;
	int		m_nMpsX;
	int		m_nMpsY;
	void	Clear() {shopType = ST_Invalid; m_nBuyIdx = -1; m_SubWorldID = -1; m_nMpsX = 0; m_nMpsY = 0;m_nSmithShopIdx = -1;}
} BuySellInfo;
class KIniFile;


typedef struct tagTEMPADDSTATUSINFO
{
	int nAddValue;
	int nAddValue2;
	int nExistTime;
}TEMPADDSTATUSINFO, *PTEMPADDSTATUSINFO;

typedef enum enTEMPADDSTATUSTYPE
{
	enTempAddType_Credit = 0, 
	enTempAddType_Skill, 
	enTempAddType_AllSkill, 
}TEMPADDSTATUSTYPE, *PTEMPADDSTATUSTYPE;

enum ens2c_PetProtocol
{
	pet_s2c_init_sync,
	pet_s2c_normal_sync,
	pet_s2c_change_name,
	pet_s2c_open_panel,
	pet_s2c_pet_chat,				//宠物说话
	pet_s2c_invalid_name,			//名字非法
};

enum enc2s_PetProtocal
{
	pet_c2s_change_name,
	pet_c2s_feed_pet,	
	pet_c2s_open_panel,
};

#define TICKCOUNT	0xFF
#define TICKBEGINE	0x00
#define TICKFREE	0x01
#define TICKSEND	0x02

struct Motion
{
	BYTE	btMotionId;
	time_t	nBeginStamp;
	int		nFullTime;
#ifdef _SERVER
	DWORD	dwSciprtID;
#endif
};

enum MotionEventType
{
	enMotionInterupt,			// 强制中断动作
	enMotionChangeTime,			// 延长动作的时间
	enMotionEventTypeNum,
};

//同步属性
enum enumSyncAttribute
{
	attr_Exp,
	attr_SkillExp,
	attr_WeightMax,
	attr_CanPickup,
	attr_IsBlockClientControl,
	attr_pkmode,
	attr_jinshanbi,
	attr_maxcreditpoint,
	attr_creditpoint,
	attr_creditstate,
	attr_creditreturndata,
	attr_point,
	attr_combatscore,
	attr_employtime,
	attr_gmflag,

	attr_passward_state,

	attr_pluspoint0,
	attr_pluspoint1,
	attr_pluspoint2,
	attr_pluspoint3,
	attr_pluspoint4,
	attr_pluspoint5,
	attr_pluspoint6,
	attr_pluspoint7,
	attr_pluspoint8,
	attr_pluspoint9,
	attr_pluspoint10,
	attr_pluspoint11,
	attr_pluspoint12,
	attr_pluspoint13,
	attr_pluspoint14,
	attr_pluspoint15,
	attr_pluspoint16,
	attr_pluspoint17,
	attr_pluspoint18,
	attr_pluspoint19,

	playerSyncAttr_Count
};

//玩家事件
enum enumPlayerEvent
{
	player_event_join_team,		//加入队伍
	player_event_leave_team,	//离开队伍
	player_event_team_changed,	//队伍发生变化
	player_event_exit_world,	//离开世界
	player_event_enter_world,	//进入世界
};

//玩家操作
enum enumPlayerOpType
{
	player_op_post = 0,			//上榜
	player_op_employ,			//雇用
	player_op_fire,				//解雇
	player_op_search,			//查询
	player_op_commit_question,	//提交问题
	player_op_add_student,		//指定推荐人
	player_op_update_student,	//徒弟汇报
	player_op_list_student,		//列出徒弟
	player_op_list_master,		//列出师傅
	player_op_get_master_reward,	//领取推荐人奖励
	player_op_select_title,			//选择称号
	player_op_refresh_shizu_popularity,		//刷新氏族人气排行
	player_op_refresh_zhuhou_popularity,	//刷新诸侯人气排行
	player_op_refresh_player_properties,	//刷新玩家属性页属性

	player_op_count
};

//延迟添加BUFF的类型
enum DelayAddBuffType
{
	delay_add_buff_type_transfer = 1,
	delay_add_buff_type_revive,
};

#define INTERACTIVE_SCRIPT_INT_PARAM_COUNT 3
#define INTERACTIVE_SCRIPT_STR_PARAM_COUNT 3
#define INTERACTIVE_SCRIPT_STR_PARAM_LENGTH 128
#define INTERACTIVE_SCRIPT_FUNC_NAME_LENGTH 64
#define INTERACTIVE_SCRIPT_ORIGINAL_PARAM_COUNT 3
#define INTERACTIVE_SCRIPT_TIMEOUT 30

struct InteractiveScriptState
{
	bool IsProcessing;
	DWORD ScriptId;
	char FuncName[INTERACTIVE_SCRIPT_FUNC_NAME_LENGTH];
	int OriginalParam1;
	int OriginalParam2;
	int OriginalParam3;
	int Step;
	DWORD Timeout;
	int IntParam[INTERACTIVE_SCRIPT_INT_PARAM_COUNT];
	char StrParam[INTERACTIVE_SCRIPT_STR_PARAM_COUNT][INTERACTIVE_SCRIPT_STR_PARAM_LENGTH];
};

struct IBBuy_Param 
{
	bool	bOnceItem;
	MoneyType eMoneyType;
	int nPrice;
	enumIBItemType eIBItemType;
	int nItemGenre;
	int nItemDetail;
	int nItemParticular;
	int nItemCount;
	DWORD dwOverdueTime;
};

struct IBUse_Param 
{
	int nItemGenre;
	int nItemDetail;
	int nItemParticular;
	INT64 IBGuid;
};

//Add By Brianyao2007
#ifndef _SERVER
struct ITianXiang;
#endif

enum enumCreditState
{
	disable,
	good,
	bad,
};

//GM操作类型
enum enumGMOpType
{
	gm_op_kick = 1,			//踢下线
	gm_op_no_chat,			//禁言
	gm_op_no_login,			//禁止登陆（冻结角色）
	gm_op_freeze_account,	//冻结账号
	gm_op_transfer,			//传送
	gm_op_view_ip,			//查看IP
};

struct ITaisuiWheel;
//End

#ifdef _SERVER
//副本信息
class InstanceInfo
{
public:
	InstanceInfo();
	~InstanceInfo();

	DWORD GetInstanceId(int worldTemplateId) const;//得到副本编号
	void SetInstanceId(int worldTemplateId, DWORD instanceId);//设置副本编号

	int Save(BYTE *pSaveBuf, int buffSize);//保存
	bool Load(BYTE *pLoadBuf, int dataSize);//载入

private:
	int SaveVersion1(BYTE *pSaveBuf, int buffSize);//保存（版本1）
	bool LoadVersion1(BYTE *pLoadBuf, int dataSize);//载入（版本1）

	DWORD m_InstanceId[INSTANCE_SUBWORLD_START];
};
#endif

#ifdef _SERVER

enum PLAYER_UI_SERVER
{
	player_ui_repository = 0,
	player_ui_mail,
	player_ui_auction,
	player_ui_employ,
	player_ui_recommender_master,
	player_ui_recommender_strudent,
	player_ui_num
};

enum PLAYER_UI_SERVER_STATE
{
	player_ui_state_invalid = -1,
	player_ui_state_close = 0,
	player_ui_state_open,
	player_ui_state_num
};

class KPlayerUIState
{
public:
	KPlayerUIState(void);
	~KPlayerUIState(void);
public:
	void                   Init( void );
	void                   SetUIState( const PLAYER_UI_SERVER nServerIdx,const PLAYER_UI_SERVER_STATE nState);
	PLAYER_UI_SERVER_STATE GetUIState( const PLAYER_UI_SERVER nServerIdx);
	void                   PlayerWalkNotify( void );
	void                   PlayerOffLineNotify( void );
private:
	PLAYER_UI_SERVER_STATE m_UiState[player_ui_num];
};

#endif

class KPlayer
{
public:

	KPlayer();
	~KPlayer();

#ifdef _SERVER
	bool DoMarry(KPlayer & couplePlayer);
	bool UnMarry();
	void SetMarriageInfo(const char * coupleName, unsigned long marryTime);
#endif
	DWORD GetPlusPointRecord(int plusPointRecordIdx);
	DWORD GetPlusPoint(int plusPointIdx);
	bool AddPlusPoint(int plusPointIdx, DWORD plusPoint );
	bool DecPlusPoint(int plusPointIdx, DWORD plusPoint );

	
	bool IsValid() const;//是否为合法的玩家
	int GetNpcIndex() const;//得到NPC序号
	KItemList& GetItemList();//得到ItemList

	int GetLevel() const;//得到等级
	int GetSeries() const;//得到职业
	RoleSkillSeries GetSkillSeries() const;//得到职业分支		

	DWORD GetExpMax() const;//得到经验最大值
	DWORD GetExp() const;//得到经验值
	void SetExp(DWORD exp);//设置经验值
		
	DWORD GetSkillExpMax() const;//得到技能经验最大值
	DWORD GetSkillExp() const;//得到技能经验值
	void SetSkillExp(DWORD skillExp);//设置技能经验值	

	int GetWeightCurrent(int *pWeightTaken, int *pWeightMax);//得到当前负重和负重上限；返回TRUE或FALSE。
	int	GetWeightTaken();//得到当前负重
	int GetWeightMax() const;//得到负重最大值
	void SetWeightMax(int nWeightMax);//设置负重最大值
	void AddWeightMax(int nDelta, bool bSync, bool bTempAdd = FALSE);//添加负重最大值
	void CheckWeight();//检查是否超重

	bool CanPickup() const;//是否可以拾取
	void SetCanPickup(bool canPickup);//设置是否可以拾取物品

	DWORD GetTicket() const;
	void SetTicket(DWORD ticket);

	void SetNewPlayer(BYTE bNew);
	void SetPlayerIndex(int nNo);						// 设定 m_nPlayerIndex
	void GetAboutPos(KMapPos *pMapPos);					// 获得玩家附近一个空位置
	int GetPlayerIndex();								// 获得本实例在 Player 数组中的位置	
	BOOL ExecuteScript(char * ScriptFileName, char * szFunName, int nParam = 0, unsigned int nResultCount = 0);
	BOOL ExecuteScript(char * ScriptFileName, char * szFunName, char * szParams, unsigned int nResultCount = 0);
	BOOL ExecuteScript(DWORD dwScriptId, char * szFunName, char *  szParams, unsigned int nResultCount = 0);
	BOOL ExecuteScript(DWORD dwScriptId,  char * szFunName, int nParam, unsigned int nResultCount = 0);
	BOOL ExecuteScript2Param(DWORD dwScriptId, LPCSTR cFuncName, int nResultCount, int nParam1, int nParam2);
	BOOL ExecuteScript3Param(DWORD dwScriptId, LPCSTR cFuncName, int nResultCount, int nParam1, int nParam2, int nParam3);
	BOOL DoScript(LPSTR pScriptCommand);
	BOOL ExecuteScript(char * ScriptFileName);
	void Release();
	void Active();									// 玩家每次游戏循环都需要处理的东西
	BOOL NewPlayerGetBaseAttribute(int Series);		// 新玩家登陆时根据五行属性产生 力量 敏捷 活力 精力 四项数值
	void AddBaseLucky(int nData);					// 增加基本运气
	void UpdataCurData();
	void ChangePlayerCamp(int nCamp);				// 改变玩家阵营
	void Revive(int nType);							// 重生
	BOOL CheckTrading();
	void SetBaseSpeedAndRadius();
	void AddPlayerLevelToCredit(int nLevel);
	void SetMateName(LPCSTR pcName);
	LPCSTR GetMateName();
	BOOL IsMarried();
	void AddMasterPRValue(int nAdd);
	BOOL DecMasterPRValue(int nDec);
	void UpdateMasterPRValue(int nValue);
	TitleManager& GetTitleManager();
	
	//----------------------------------------------->
	bool ChongZhiLeftMoney( void );
	void ProcessPaysys( const char* pChar, int nSize );
	int UseSilver(unsigned uType, unsigned uUseType, unsigned uCount);// uType = 0: 大银票 uType = 1: 小银票；uUseType = 0: 转换为点数 uUseType = 1: 转为包（周）月
	void SetExtPoint(int nIndex, int nPoint);
	void SetExtPoint(const tagExtPointInfo& ExtPoint );
	BOOL SetExtPointImmediately(int m_nIndex, int nPoint);
	int	GetExtPoint(int nIndex);
	BOOL PayExtPoint(int nIndex, int nPoint);
	BOOL AddExtPoint(int nIndex, int nPoint);
	
	int SetIBPoint( MoneyType type, long money );
	int BuyIBItem( IBBuy_Param ibbuy_param );
	int UseIBItem( IBUse_Param ibuse_param );
	int BuyIBItem( );
	int UseIBItem( );	

	void CheckCreditState();

	void SetCreditState( unsigned char uCreditState )
	{
		m_uCreditState = uCreditState;
	}//*/

	DWORD GetNewReturnDate( DWORD uReturnDate )
	{
		ConfigManager& cm = ConfigManager::Singleton();
		int day = cm.GetIBGlobalVariable( ib_global_var_credit_day_default );

		tm cur;           
		time_t   clock;   
		memcpy( &cur, localtime((const long *)&uReturnDate) ,sizeof(cur) );  

		cur.tm_hour = 23;
		cur.tm_min = 0;
		cur.tm_sec = 0;

		if ( cur.tm_mon == 11 )
		{
			cur.tm_mon = 0; 
			cur.tm_year++;
		}
		else
		{
			cur.tm_mon++;
		}

		if ( cur.tm_mday > 28 )
		{
			cur.tm_mday = 28;
		}

		if ( day > 0 )
		{
			cur.tm_mday = day;
		}
		else
		{
			tm old;
			if ( m_uReturnDate != 0 )
			{
				memcpy( &old, localtime((const long *)&m_uReturnDate) ,sizeof(old) ); 
				cur.tm_mday = old.tm_mday;
			}
		}

		clock = mktime(&cur);

		return clock;
	}

	DWORD GetFirstNewReturnDate( DWORD uReturnDate )
	{
		ConfigManager& cm = ConfigManager::Singleton();
		int day = cm.GetIBGlobalVariable( ib_global_var_credit_day_default );

		tm cur;           
		time_t   clock;   
		memcpy( &cur, localtime((const long *)&uReturnDate) ,sizeof(cur) );  

		cur.tm_hour = 23;
		cur.tm_min = 0;
		cur.tm_sec = 0;

		if ( cur.tm_mon == 10 )
		{
			cur.tm_mon = 0; 
			cur.tm_year++;
		}
		else if ( cur.tm_mon == 11 )
		{
			cur.tm_mon = 1;
			cur.tm_year++;
		}
		else
		{
			cur.tm_mon += 2;
		}

		if ( cur.tm_mday > 28 )
		{
			cur.tm_mday = 28;
		}

		if ( day > 0 )
		{
			cur.tm_mday = day;
		}
		else
		{
			tm old;
			if ( m_uReturnDate != 0 )
			{
				memcpy( &old, localtime((const long *)&m_uReturnDate) ,sizeof(old) ); 
				cur.tm_mday = old.tm_mday;
			}
		}

		clock = mktime(&cur);

		return clock;
	}


	void SetCreditReturnTime( DWORD uReturnDate )
	{
		m_uReturnDate = uReturnDate;
	}

	DWORD ValidReturnDate( DWORD uReturnDate );

	unsigned char GetCreditState( void )
	{
		return m_uCreditState;
	}//*/

	DWORD GetCreditReturnTime( void )
	{
		return m_uReturnDate;
	}
	
	long GetIBMoney( MoneyType type )          //获取IB货币可交易值
	{
		if ( type == jinshanbi )
		{
			return 0;
		}
		else
		{
			return m_moneyMgr.GetMoney( type );
		}		
	}

	void SetIBMoney( MoneyType type, void* money) //设置IB货币可交易值
	{
		if ( type == jinshanbi )
		{
			DWORD* val = (DWORD*)money;
			m_moneyMgr.SetJinshanbi( *val );
		}//endif
		else
		{
			long* val = (long*)money;
			m_moneyMgr.SetMoney( type, *val );
		}//end else
	}

	long GetIBPlus( MoneyType type)            //获取IB货币非可交易附加值
	{
		return m_moneyMgr.GetMoneyPlus(type);
	}

	void SetIBPlus(MoneyType type,const DWORD dwValue) //设置IB附加值
	{
		m_moneyMgr.SetMoneyPlus(type,dwValue);
	}

	DWORD GetJinshanbi( )
	{
		return m_moneyMgr.GetJinshanbi();
	}


	void SetIBMoneySize( MoneyType type,  int min, int max )
	{
		 m_moneyMgr.SetMoneySize( type, min, max );
	}

	void GetIBMoneySize( MoneyType type,  int& min, int& max )
	{
		m_moneyMgr.GetMoneySize( type, min, max );
	}

#ifdef _SERVER
	void ActiveInCombatMap( void );
	void AddTotolJinshanbi( DWORD jinshabi )
	{
		m_dwTotolJinshabi += jinshabi;
	}
	void AddRecentJinshanbi( DWORD jinshabi )
	{
		DWORD dwCurTime = UNIX_TMIE_STAMP;
		if ( dwCurTime - m_dwRecentlyTime >= 86400 )
		{
			m_dwRecentJinshabi = jinshabi;
			m_dwRecentlyTime = dwCurTime;
		}
		else
		{
			m_dwRecentJinshabi += jinshabi;
		}
	}

	DWORD GetTotolJinshanbi( void )
	{
		return m_dwTotolJinshabi;
	}
	DWORD GetRecentTime( void )
	{
		return m_dwRecentlyTime;
	}
	DWORD GetRecentJinshanbi( void )
	{
		return m_dwRecentJinshabi;
	}

	bool  IsPreventAddFriend( void )
	{
		return m_bPreventAddFriend;
	}

	void  SetPreventAddFriend( const bool  bPrevent)
	{
		m_bPreventAddFriend =  bPrevent;
	}

private:
	void PayJinShanBiIncPoint(const int nJinshanB)
	{
		if (nJinshanB > 0)
			m_moneyMgr.AddReq(point,(void*)&nJinshanB);
	}

	int JinshanbiToCredit(const int nJinshanbi);    //这里用int 是为了能检查参数 且默认一次交易量不会超过2~31
public:
#endif

	//----------------------------------------------->
	
	//交易相关……begin（都放在这里，谢谢合作，谢鉷2007年1月30日）
	void tradeClientSendRequest(int oppositePlayerNpcId);	//客户端发送交易请求
	void tradeClientReciveRequest(int oppositePlayerNpcId);	//客户端收到交易请求
	void tradeClientSendAccept(int destId);			//客户端发送接受交易
	void tradeClientSendRefuse(int destId);			//客户端发送拒绝交易
	bool tradeClientMoveMoney(int nMoney);			//交易时输入自己的钱
	void tradeClientReciveOppositeMoneyChanged(int money);//客户端收到对方交易金钱改变通知
	void tradeClientStateChange(int oppositePlayerNpcId, int state);	//客户端收到服务器状态变化的通知
	void tradeClientSendLock();						//客户端发送交易锁定请求
	void tradeClientReciveLock(bool self);			//客户端收到服务器端发送的对方或自己的锁定同步信息（表示之前的操作已经成功）
	void tradeClientReciveUnlock();					//客户端收到服务器端发送的解除自己的锁定同步信息（表示之前的操作已经成功）
	void tradeClientSendEndTrade();					//客户端发送交易完成请求
	void tradeClientReciveEndTrade(bool self);		//客户端收到服务器端发送的结束交易同步信息
	void tradeClientReciveTradeOk();				//客户端收到服务器端发送的交易成功同步消息
	void tradeClientSendTradeCancel();				//客户端发送取消交易消息
	void tradeClientReciveTradeCancel();			//客户端收到服务器端发送的取消交易同步消息
	void tradeServerReciveRequest(int oppositePlayerNpcId);	//服务器端收到交易请求，并向对端发送交易请求
	void tradeServerReciveAccept(int oppositeNpcId);//服务器端收到确认响应
	void tradeServerReciveRefuse(int oppositeNpcId);//服务器端收到确认响应
	void tradeServerMoveMoney(int money);			//服务器端收到申请交易中money的改变
	void tradeServerReciveSelfLock();				//服务器端收到己方锁定命令
	void tradeServerSendUnlockToOpposite();			//服务器端发送解除对方锁定命令
	bool tradeServerReciveTradeEnd();				//服务器端收到客户端交易完成的消息，执行交易操作
	bool tradeServerReciveCancel();					//服务器端收到客户端终止交易请求
	bool tradeServerDoCanceTrade();					//服务器端执行终止交易操作	
	//交易相关……end

	//储物箱锁定、解锁……begin
#ifdef _SERVER
	void lockStoreBox();                           //注意：现在用这些函数用来代表安全状态，不仅仅是仓库锁定，还包括 IB 商店锁定的问题.
	bool isStoreBoxLocked();
	bool checkStoreBox();                          //如果安全状态被锁定则返回 true 并向客户端发包寻求密码 否则返回 false
	void unlockStoreBox();
	//储物箱锁定、解锁……end
	
	KServerFuryMgr & GetFurySys();
	void SetCombatInfoOrg(const unsigned long dwOrgId);
	bool DecCombatInfoScore(const int nScoreNew,LogEvent logEvent, BOOL nEffectOrgScore = TRUE);
	void AddCombatScore(const int nScore,LogEvent logEvent, BOOL bEffectOrgScore = TRUE); //Notice: Server used only and only in combat war to addCombatScore
	bool GetCamoflag(void);                         //是否处于蒙面状态
	void SetComoflag(const bool bSet);              //设置蒙面状态
#endif


	//玩家移动……begin
	void onPlayerRun();								//玩家开始跑动的时候的处理
	//玩家移动……end

	void DoScriptAction(PLAYER_SCRIPTACTION_SYNC * pUIInfo);		//通知该客户端显示某个UI界面
	void ProcessPlayerSelectFromUI(BYTE* pProtocol);				// 处理当玩家从选择菜单选择某项时的操作

	int GetCowrie() const;
	void AddCowrie(int delta);	
	void ResetCowrie();
	int GetNetConnectIdx() const;
	int IsNewPlayer();
	int GetScriptResultNumber();
	DWORD GetPlayerID();
	LPSTR GetPlayerName();

	bool IsBlockClientControl() const;//是否拦客户端控制
	void SetBlockClientControl(bool block);//设置拦截客户端控制

	KPlayerTeam& GetTeamInfo();//得到玩家组队信息

	//Add By Brianyao2007
	inline ITaisuiWheel     * GetTaisuiWheelSys(void);
	const  PlayerCombatInfo & GetCombatInfo(void)const;
	void   CheckSendWorldCombatInfo(void);
	//End

	DWORD GetEmployTime() const;//取得雇用时间
	void SetEmployTime(DWORD time);//设置雇用时间

	bool IsGM() const;//是否GM

private:

	inline void SetScriptResult(int nResult) { m_nScriptResult = nResult; };
	inline void ClearScriptResult();

	bool tradeExchangePrecheck();					//物品交换前的检查，例如：是否格子够等
#ifdef _SERVER
	bool tradeProcessTrade();						//执行交易	
#endif
//	void S2CExecuteScript(char * ScriptName, char * szParam);	

#ifdef _SERVER // Begin of Server Code---------------------------------------------------------------------

public:

	const FSGUID& GetGUID() const;//得到GUID
	void SetGUID(const FSGUID& guid);//设置GUID

#ifdef _SERVER
	bool SendInvitation(const char * szTitle, const char * szContent);
	bool SendInvitationToFriends(const char * szTitle, const char * szContent);
	bool SendInvitationToMembers(const char * szTitle, const char * szContent);
#endif

	int GetPkValue();//得到PK值
	void SetPkValue(int pkValue);//设置PK值
	int PkPunish(int beKilledPlayerIndex);//杀人惩罚
	int RecalePKValue(int beKilledPlayerIndex);
	void DeathPunish();//死亡惩罚
	bool IsKiller();//是否“杀手”
	void SetKiller(bool isKiller);//设置“杀手”状态
	void CheckNameColor();//检查名字颜色
	void DropItem(int itemIndex);//掉落物品
	void Offline();//下线
	int CheckPickupObject(int pickupObjectId);//检查是否能够拾取
	int CheckCanPickOBJByIndex(int nPickupObjectIndex);
	int DoPickupObject(int pickupObjectId, int posX, int posY);//实际拾取物品
	int CheckDialogNpc(int dialogNpcId);//检查是否能够对话NPC
	int DoDialogNpc(int dialogNpcId);//实际对话NPC
	ActionDelayer& GetActionDelayer();//得到动作延迟器
	void AddTalismanPotential(DWORD potentialAdded);//增长法宝蕴魂
	void AddSkillExp(int skillExp, int tragetLevel);//根据等级差异添加技能经验
	void DirectAddSkillExp(DWORD skillExp);//直接添加技能经验
	PlayerStatistic& GetPlayerStatistic();//得到数据统计
	DWORD GetCreateTime() const;//得到角色创建时间
	
	inline int GetPetType() { return m_byPetType; }
	inline int GetPetHonor() const { return m_byPetHonor; }
	inline const char* GetPetName() const { return m_szPetName; }
	inline void SetPetColor(BYTE byColor) { m_byPetColor = byColor; }
	inline BYTE GetPetColor() const { return m_byPetColor; }
	inline bool IsPetReleased() const { return m_bPetReleased; }
	inline PLAYER_REVIVAL_POS* GetDeathRevivalPos() { return &m_sDeathRevivalPos; }
	inline BOOL IsUseReviveIdWhenLogin() { return m_bUseReviveIdWhenLogin; }
	inline void SetLoginType(BOOL bUseReviveId) { m_bUseReviveIdWhenLogin = bUseReviveId; }
	inline BYTE GetCityAddExpState();
	inline void	SwitchCityAddExpState(BYTE nEnable);
	inline void UpdateCityAddExpTimer();
	inline unsigned int GetPlayCard(void) const { return m_uPlayCard; }
	inline void ClearPlayCard(void) { m_uPlayCard = 0; }
	inline BOOL ModifyPlayCard(unsigned int uNewPick);
	inline void SetReviveFlag(BOOL bRevive) { m_bUseReviveIdWhenLogin = bRevive; };
	bool MarkToCreature(int nNpcIdx);
	void ClearMarkCreature();
	int  GetMarkCreature();
	void ReturnMarkCreature();
	
	void GetItemTransData(TItemtransfersData& data, DWORD dwItemId);
	void GetItemName(char *pName, int nBufSize, DWORD dwItemId);
	void GetItemGuid(FSGUID &guid, DWORD dwItemId);
	int AddItem(const TItemtransfersData *pItemData);
	void CreditUpTimer();
	void SetLastUsedSkillType(int nSkillType);
	int GetLastUsedSkillType();
	void SetPetHonor(BYTE byPetHonor);
	void SetPetName(const char * szPetName);
	void SetMasterName(LPCSTR pcName);
	LPCSTR GetMasterName();
	BOOL IsHaveMaster();
	void SetPrenticeCount(int nCount);
	int GetPrenticeCount();
	void GetRevivePos(PLAYER_REVIVAL_POS* pos);
	void SaveQuickly() { m_ulLastSaveTime = 0; }
	DWORD GetOnlineTime() { return  m_dwPlayGameTime + UNIX_TMIE_STAMP - m_dwLoginTime; }//这个是在游戏中的总计时间（从角色建立开始计算）
	void repairItem(DWORD dwItemID, bool special);
	void repairItemByItem(DWORD dwItemIdx, int nRepairPresent = 100);
	int	FindAroundPlayer(DWORD dwNpcID);		// 寻找玩家周围的某个指定npc id的player index
	int	FindAroundNpc(DWORD dwNpcID);			// 寻找玩家周围的某个指定npc id的npc index
	BOOL CheckPlayerAround(int nPlayerIdx);		// 判断某玩家是否在周围

	BOOL IsWaitingRemove();
	BOOL IsCanRemove();
	BOOL IsLoginTimeOut();
	void WaitForRemove();
	void LoginTimeOut();
	void GetLoginRevivalPos(int *lpnSubWorld, int *lpnMpsX, int *lpnMpsY);		// 获取玩家登入重生点位置
	void GetDeathRevivalPos(int *lpnSubWorld, int *lpnMpsX, int *lpnMpsY);		// 获取玩家死亡重生点位置
	void SetRevivalPos(int nSubWorld, int nRevalId);								// 设定玩家重生点ID
	void ClearDeathRevivaPos( );
	BOOL Save( BYTE* pBuf, BOOL bOffline = FALSE );									// 保存玩家数据
	int SaveItemData( );

	int  SaveIBData( );
	int  SaveInsurance( );
	int  SaveBaseInfoData( );
	BOOL CanSave();
	void ProcessUser();
	BOOL SendSyncData( int nType );				// 发送同步数据
	BOOL SendPetSyncData(int nClient);
	BOOL SendPetNormalSyncData(int nClient);
	BOOL SendSyncData_Skill();					// 发送同步数据 - 技能
	void SendCurNormalSyncData();				// 发送平时给自己的同步数据
	void BuyItem(BYTE* pProtocol);
	void SellItem(BYTE* pProtocol);

	void ServerPickUpItem(BYTE* pProtocol);		// 收到客户端消息鼠标点击某个obj拣起装备或金钱
	void EatItem(BYTE* pProtocol);				// 收到客户端消息吃药
	void ServerMoveItem(BYTE* pProtocol);		// 收到客户端消息移动物品
	void ServerThrowAwayItem(BYTE* pProtocol);	// 收到客户端消息丢弃物品
	void ChatResendAllFriend(BYTE* pProtocol);
	void ChatSendOneFriendData(BYTE* pProtocol);		
	void SendEquipItemInfo(int nTargetPlayer);	// 发送自己装备在身上的装备信息给别人看
	void NotifyLogout();
	BOOL IsNotifyLogout();
	BOOL IsExistBoxPassword();
	BOOL SetBoxPassword(const char* pcNewPassword, const char* pcOldPassword);
	BOOL CheckBoxPassword(const char* pcPassword);
	BOOL ReplaceBoxPassword(const char* pcSecondPassword, BOOL &bNeedREQ);
	int	GetCredit();
	void SetTempAddStatus(TEMPADDSTATUSTYPE enAddType, const TEMPADDSTATUSINFO &tagAddStatus);

	void RestoreLiveData();							//重生后恢复玩家的基本数据
//	void SetTimer(DWORD nTime, int nTimeTaskId);		//时间任务脚本，开启计时器
//	void CloseTimer();								//关闭时间计时器
	int	PlayerDbOpComplete( int nType, int nOpResult, BYTE* pRetData, int nDataSize );
	int	PlayerDbOpComplete( IProcRet* pRet );
	
	BOOL GetNewPlayerFromIni(KIniFile * pIniFile, BYTE * pRoleBuffer);
	int DeletePlayer(char * szPlayerName = NULL);	//注意：本函数是清除玩家帐号！！！，不能乱用
	void LaunchPlayer();
	BOOL Pay(int nMoney, bool statisticFlag = true);
	BOOL Earn(int nMoney, bool statisticFlag = true);
	void DialogNpc(BYTE * pProtocol);
//	void AutoAddItem(unsigned int uIndex);
	unsigned long OfferPostId() const;
	void OfferPostId(unsigned long);
	unsigned long ApplyPostId() const;
	void ApplyPostId(unsigned long);
	unsigned char MissionCount() const;
	void MissionCount(unsigned char);
	int ProcessQuestion();
	void LevelUp();
	void SyncAttribute(enumSyncAttribute attr);
	void SyncSocialRelation(int templateId);
	void SendSyncCombatTop10(int orgId);
	void BroadCastSocialInfo(void);
	RelationSet& GetRelationSet();//得到关系集
	void AddExp(int exp, int tragetLevel);//根据等级差异添加经验
	void QuestAddExp(int exp);//任务奖励经验
	void DirectAddExp(DWORD exp, bool absorbExpFlag = false);//直接添加经验
	void InternalDirectAddExp(DWORD exp);//内部直接添加经验
	void StartLogoutTimer();
	void StopLogoutTimer();
	bool IsLogoutTiming();	
	bool ShowPredefinedMsg(int msgId);//显示预置消息
	int GetExpPercentage() const;//得到经验获得百分比
	void SetExpPercentage(int percentage);//设置经验获得百分比
	int GetQuestExpPercentage() const;//得到任务经验获得百分比
	void SetQuestExpPercentage(int percentage);//设置任务经验获得百分比
	int GetSkillExpPercentage() const;//得到蕴魂获得百分比
	void SetSkillExpPercentage(int percentage);//设置蕴魂获得百分比
	void PromptAddBuff(int buffSender, int buffId);//添加BUFF提示
	void CancelPromptAddBuff();//取消添加BUFF提示
	bool DelayAddBuff(int buffSender, int buffId, int delay);//延迟添加BUFF
	void AddEnterWorldBuff();//添加进入世界BUFF
	void AddExitWorldBuff();//添加离开世界BUFF
	void ValidateInstance();//验证绑定的副本
	void ValidatePlayerState();//检查玩家状态
	void OnEvent(enumPlayerEvent eventType, const void* eventParam = NULL);//发生事件
	bool TransferToRevivePos();//传送到重生点
	bool CanChat();//是否可以聊天
	void NoChat(DWORD noChatSeconds);//禁言时间
	int GetSpyLevel() const;//得到监视等级
	void SetSpyLevel(int spyLevel);//设置监视等级
	void SetPkPunish(int punishLevel);//设置是否计PK值，>0:计算PK值；<=0:不计PK值
	void SetDeathPunish(int punishLevel);//设置是否死亡掉装，>0:死亡掉装；<=0:死亡不掉装
	int GetPkPunish() const;//得到是否计PK值
	int GetDeathPunish() const;//得到是否死亡掉装
	void Kick();//踢出（强制下线）
	void RememberCurrentPos();//记录当前位置
	int TransferToRememberPos();//传送到记录的位置
	void GetRememberPos(int& subworldId, int& posX, int& posY) const;//得到记录位置
	void ClearSelectedSkill();//清空选中技能
	void SetSelectedSkill(CastSkillParam& skillParam);//设置选中技能
	void SetNextSkill(CastSkillParam& skillParam);//设置下一个技能

	int RecordPlayerReward( unsigned int rewardTag );//记录玩家中奖
	int RecordPlayerContact( unsigned int rewardTag, const char* realName, unsigned int sex, const char* tel, const char* eMail, const char* address, const char* code);//记录玩家信息

	int GetGMCmd( );//延迟执行GM指令
	void OnGetGMCmdRet( IProcRet* pRet );

	int Withdraw( const char* pSN );//兑换奖券
	void OnWithdrawRet( IProcRet* pRet );
	
	//GM留言板——begin
	int RecordPlayerQuestion( unsigned int questionType, const char* question );
	int GetGMReply( );
	void OnGetGMReplyRet( IProcRet* pRet );
	//GM留言板——end

	Employee& GetEmployee();
	bool CanDoOp(enumPlayerOpType op) const;//是否可以做某种操作
	void UpdateOpTime(enumPlayerOpType op, DWORD interval);//更新某种操作的允许时间

	int ExecuteInteractiveScript(DWORD scriptId, const char* funcName, int originalParam1, int originalParam2, int originalParam3);
	void InteractiveScriptNextStep();
	void InteractiveScriptDone();
	int GetInteractiveScriptStep() const;
	int GetInteractiveScriptIntParam(int paramIndex) const;
	void GetInteractiveScriptStrParam(int paramIndex, char* pOutStrBuff, int outBuffSize) const;
	void SetInteractiveScriptParam(int paramIndex, int intValue);
	void SetInteractiveScriptParam(int paramIndex, const char* strValue);

	InstanceInfo& GetInstanceInfo();
	int GetMyCombatScore() {return m_CombatScoreOneTime;}    //获取一次战场中得的积分
	void AddOnceCombatScore(int score);                        //增加一次战场中得的积分
	void DecOnecCombatScore(int score);
// 	bool LoadInstanceData(BYTE *pInBuf, int nSize);
// 	int SaveInstanceData(BYTE *pOutBuf);
// 	bool LoadInstanceDataVersion1(BYTE *pInBuf, int nSize);
// 	int SaveInstanceDataVersion1(BYTE *pOutBuf);

	//推荐人系统
	int AddStudent(const char* recommenderName);
	int UpdateStudent();
	int ListStudent();
	int ListMaster();
//	int GetMasterReward();
	bool AddRecommenderReward(int reward);
	int GetRecommenderRewardTicket();
	KPlayerUIState & GetUIServerState(void);
	QuestionState& GetQuestionState();

	//GM操作
	void ProcessGMOperation(BYTE* pProtocol);
	void LogGMOperation(LogEvent event, const char* szTargetPlayerName, DWORD opParam = 0, const char* additionalInfo = NULL);

	//礼品卡激活
	bool RequireActivatePresent( const char * szPresentCode); 
	void ProcessActivatePresentRet ( KAccountActivePresentCodeRet * pRet );

	//物品存储经验
	void GetItemExpState(int& itemIndex, DWORD& playerExpGainPercent, DWORD& itemExpGainPercent) const;//得到物品存储经验状态
	void SetItemExpState(int itemIndex, DWORD playerExpGainPercent, DWORD itemExpGainPercent);//设置物品存储经验状态

private:
/*
	static int BaseDBOpCallBack(int nType, char* pSaveBuf, unsigned int nSize, void* Param);	
	static int BuffDBOpCallBack(int nType, char* pSaveBuf, unsigned int nSize, void* Param);	
	static int BuffDBLoadCallBack(int nType, char* pSaveBuf, unsigned int nSize, void* Param);
	static int LoadSkillInfoCallBack(int nType, char *pSaveBuf, unsigned int nSize, void *Param);
	static int SaveSkillInfoCallBack(int nType, char *pSaveBuf, unsigned int nSize, void *Param);
	static int ItemlistDBSaveCallBack(int nType, char* pSaveBuf, unsigned int nSize, void* Param);	
	static int ItemlistDBLoadCallBack(int nType, char* pSaveBuf, unsigned int nSize, void* Param);	
	static int LoadTaskListCallBack(int nType, char *pSaveBuf, unsigned int nSize, void *Param);
	static int SaveTaskListCallBack(int nType, char *pSaveBuf, unsigned int nSize, void *Param);
	static int OnlineCallBack(int nType, char *pSaveBuf, unsigned int nSize, void *Param);
	static int OfflineCallBack(int nType, char *pSaveBuf, unsigned int nSize, void *Param);
	
	int	LoadPlayerSkillInfo(BYTE *pData, int nSize);
	int SavePlayerSkillInfo(BYTE *pOutBuf, int nBufSize);	
	int	LoadPlayerTaskList(BYTE * pRoleBuffer, int nSize );
	int	SavePlayerTaskList(BYTE * pRoleBuffer, int nSize );
	
*/

	void InitPlayerSaveTimeInterval();

	int	ParseBaseInfo( BYTE * pRoleBuffer );
	int PackBaseInfo(BYTE * pRoleBuffer);

	int LoadBaseInfoData( IProcRet* pRet );

	int SaveSkillData( );
	int LoadSkill( );
	int LoadSkillData( IProcRet* pRet );

	int ParseItem(BYTE * pRoleBuffer, int nSize );
	int	PackItem(BYTE * pRoleBuffer, int nSize );
	int LoadItem( );
	int LoadItemData( IProcRet* pRet );

	int SaveTaskData( );
	int LoadTask( );
	int LoadTaskData( IProcRet* pRet );

	int SaveEnhanceData( );
	int LoadEnhance( );
	int LoadEnhanceData( IProcRet* pRet );

	int SaveFriendData( );
	int LoadFriend( );
	int LoadFriendData( IProcRet* pRet );

	int SaveReserveData( );
	int LoadReserve( );
	int LoadReserveData( IProcRet* pRet );


	int SaveSocialData( );
	int LoadSocial( );
	int LoadSocialData( IProcRet* pRet );

	int DBOnline( );
	int DBOffline( );

	void ProcessAutoAttack();//处理自动攻击逻辑

	void CheckInteractiveScriptTimeout();//检查交互脚本是否超时

	//推荐人系统
	void AddStudentDBRet( IProcRet* pRet );
	void UpdateStudentDBRet( IProcRet* pRet );
	void ListStudentDBRet( IProcRet* pRet );
	void ListMasterDBRet( IProcRet* pRet );
	void GetMasterRewardDBRet( IProcRet* pRet );

	//结婚相关
	int Marry( const char* playerName );//与指定玩家结婚
	void MarryRet( IProcRet* pRet );//与指定玩家结婚数据库返回
	int Divorce( );//离婚
	void DivorceRet( IProcRet* pRet );//离婚数据库返回
	int GetMarriageData( const char* playerName );//得到结婚数据
	void GetMarriageDataRet( IProcRet* pRet );//得到结婚数据数据库返回

#else // Begin of Client Code---------------------------------------------------------------------

public:

	inline int GetLeftSkill() { return 0; }
	inline int GetRightSkill() { return 0; }
	inline int GetSelectNpc() const { return m_nPeapleIdx; }
	inline int GetSelectObj() const { return m_nObjectIdx; }
	inline int GetSelectBuilding() const { return m_nBuildingIdx; }
	void SetTargetNpc(int n);
	inline int GetTargetNpc() { return m_nPeapleIdx; }
	ClientSocialRelation& GetClientSocialRelation() { return m_SocialRelation; }//得到客户端社会关系数据对象

	int FindSelectNpc(int x, int y, int nRelation, bool bSearchSelf = false , bool bNoPlayer = false);
	int FindSelectObject(int x, int y);
//	void Walk(int nDir, int nSpeed);
	void TurnLeft();
	void TurnRight();
	void TurnBack();
	void DrawSelectInfo();
	BOOL ConformIdx(int nIdx);	
	void RecvSyncData();                   // 接收同步数据
	void ShowScoreGet(const int nScoreGet);//显示获得积分信息

//	void ApplySetPK(BOOL bPK);						// 玩家向服务器申请打开、关闭pk开关
//	void ApplyAddBaseAttribute(int nAttribute, int nNo);// 队长向服务器申请增加四项属性中某一项的点数(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
	BOOL ApplyUseItem(int nItemID, ItemPos SrcPos, int nTargetID, ItemPos TargetPos);	// 向服务器申请使用某个物品（鼠标右键点击该物品）
	void PickUpObj(int nObjIndex);					// 客户端鼠标点击obj检起某个物品，向服务器发消息
	void ObjMouseClick(int nObjIndex);				// 客户端鼠标点击obj，向服务器发消息
	void MoveItem(ItemPos sourPos, ItemPos destPos, int moveItemCount);	// DownPos 不能是手，UpPos 必须是手
	int ThrowAwayItem( int nItemID );
//	void ChatAddFriend(int nPlayerIdx);				// 客户端通过别人的添加聊天好友的申请
//	void ChatRefuseFriend(int nPlayerIdx);			// 客户端拒绝别人的添加聊天好友的申请

	void SetChatCurChannel(int nChannelNo);			// 设定当前聊天频道
	void SyncCurPlayer(BYTE* pMsg);
	void s2cLevelUp(BYTE* pMsg);
//	void s2cGetCurAttribute(BYTE* pMsg);
//	void s2cSetExp(int nExp);
	void s2cSyncMoney(BYTE* pMsg);
	void CheckObject(int nIdx);
	void PutBackThatIRememberPickUp();				// 把所有我记得拿起来了的东西放回去
	void clientSendMoveItemCmd(const ItemPos& sourPos, const ItemPos& destPos);
	void clientSendSplitItemCmd(const ItemPos &sourPos, const ItemPos &destPos, int nSplitCount);
	void CheckStoragePSW(const char *pcPassword);
	void CreateStoragePSW(const char *pcPassword);
	void ModifyStoragePSW(const char *pcNewPassword, const char *pcOldPassword);
//	void ReplaceStoragePSW(const char *pcSecondPassword);
	void SendCloseStorageCMD();
	void DialogNpc(int nIndex);
	void OnSelectFromUI(PLAYER_SELECTUI_COMMAND * pSelectUI, UIInfo eUIInfo);//当玩家从选择框中选择某项后，将向服务器发送			
	void OnScriptAction(PLAYER_SCRIPTACTION_SYNC * );
	void RecvAttributeSync(BYTE attr, DWORD val);
	void ReplyPrompt(enumPromptEvent promptEvent, bool accept);
	int SendGMOperation(
		const char* szPlayerName,
		enumGMOpType opType,
		int opParam1 = 0,
		int opParam2 = 0,
		int opParam3 = 0) const;

private:

	friend LuaInitStandAloneGame(Lua_State * L);
		
#endif // End of Client Code---------------------------------------------------------------------

public:

	enum enPetType
	{
		enPetTypeNone,
		enPetTypeCat,
		enPetTypeBird,
		enPetTypeNum,
	};

	MarriageInfo	m_MarriageInfo;				// 结婚信息
	int				m_nBeNpcKillOption;			// 被怪物杀死时的选项 0=虚弱状态 1=掉经验值 
	int				m_nDeathDecExp;				// 死亡时应该减少的经验
	RoleSkillSeries	m_SkillSeries;				// 技能系，-1无效，0，1分别表示一个职业的2系
	int				m_nEarnMoreMoneyP;			// Get more money from Npc in percent
	int				m_nGetMoreExpP;				// Get more experience in percent
	int				m_nGetMoreSkillExpP;		// 获得更多技能经验	
	KIndexNode		m_Node;
	FSGUID			m_GUID;
	int				m_DebugMode;
	DWORD			m_dwID;						// 玩家的32位ID
	DWORD			m_dwUniqueId;				// 数据库中的玩家唯一ID
	int				m_nIndex;					// 玩家的Npc编号
	int				m_nNetConnectIdx;			// 第几个网络连接
	KItemList		m_ItemList;					// 玩家的装备列表
	BuySellInfo		m_BuyInfo;					// 进行的交易列表
	KTrade			m_cTrade;					// 交易模块
	BYTE			m_btChatSpecialChannel;		
	char			m_PlayerName[32];
	int				m_nExpPercentage;				// 经验获取百分比
	int				m_nQuestExpPercentage;			// 任务经验获取百分比
	int				m_nSkillExpPercentage;			// 蕴魂获取百分比
	int				m_nSubExp;						// 最近一次死亡减少的
	DWORD			m_nSkillExp;					// 技能经验值
	KPlayerTeam		m_cTeam;						// 玩家的组队信息
	DWORD			m_dwDeathScriptId;			
	char			m_szTaskAnswerFun[MAX_ANSWERNUM][32];
	char			m_szRelayCallbackFun[64];		//脚本函数向Relay发送指令，如果需要回调函数，则函数名存储在此
	int				m_nAvailableAnswerNum;			//当前选择界面下，最大回答数。
	bool			m_bMultiSelection;				//是否处于多选状态
	bool			m_bWaitingPlayerFeedBack;		//当前是否正等待玩家在客户端的反馈。该状态下，当前脚本不置空.类式对话选择情况
	DWORD			m_dwWaitingPlayerFeedBackSeed;	//当前是否正等待玩家在客户端的反馈。该状态下，当前脚本不置空.类式对话选择情况
    int             m_nDialogNpcKind;
	BYTE			m_btTryExecuteScriptTimes;	
	int				m_nScriptResult;				//当执行脚本函数时，CallFunction("fun",1,...)，如果有返回值时，将数字返回值放到这个地方，字符串放到下面去
	int				m_nWorldStat;
	BOOL			m_bRandomAddAttr;	
	KCreature		m_Creature;
	

	InsuranceMgr    m_InsuranceMgr;                 //保险

#ifdef _SERVER
	TCompoundResultEx	m_smithResult;			//打造结果

#endif

#ifdef _SERVER
    KExpInsuranceMgr    m_ExpInsuranceMgr;
	KQuestInsuranceMgr  m_QuestInsuranceMgr;
#else
	int                 m_CurrentExpReward;
	bool                m_IsExpInsuraceValid;

	int                 m_CurrentQuestReward;
	bool                m_IsQuestInsuranceValid;
#endif

#ifndef _SERVER
	BOOL                m_IsPasswordExist;
#endif

private:
	DWORD				m_lastChongZhiTime;
	//Lucifer~yu(zhangjianyu) 03/19/2008 Modify IB 相关
	//Begin-------------------------------------------------------------------
	unsigned char	m_uCreditState;
	DWORD			m_uReturnDate;
	MoneyMgr		m_moneyMgr;			
	//End---------------------------------------------------------------------

	DWORD			m_plusPointArray[MAX_PLUS_POINT_COUNT];
	DWORD			m_plusPointRecord[MAX_PLUS_POINT_COUNT];

	DWORD			m_nExp;						// 当前经验值(当前等级在npc身上)
	KTaisuiWheel    m_TaisuiSys;
	int				m_nWeightMax;
	bool			m_CanPickup;
	int				m_nWeightMaxTempAdd;
	KLuaScript *m_pGMScript;
	Motion			m_Motion;
	int				m_nPeapleIdx;
	int				m_nObjectIdx;
	int				m_nBuildingIdx;
	int				m_nPickObjectIdx;
	int				m_nPlayerIndex;				// 本实例在 Player 数组中的位置
	KCacheNode *	m_pLastScriptCacheNode;
	DWORD           m_dwLastSwitchHorseTime;    // 上一次切换上下马的时间印记
	BYTE			m_bNewPlayer;				// 是否新手
	bool			m_IsBlockClient;			// 是否拦截客户端操作
	DWORD			m_Ticket;
	DWORD			m_EmployTime;				// 雇用时间
	PlayerCombatInfo m_CombatInfo;              //战场信息
	bool			m_IsGM;
	TitleManager	m_TitleManager;				//称号管理器

#ifdef _SERVER // Begin of Server Code---------------------------------------------------------------------

public:

	enum
	{
		FF_CHAT = 0x01,
	};

	enum	enPlayerDBDataMask
	{
		enPDBMask_BaseInfo = 0x1,
		enPDBMask_Friend = 0x2,
		enPDBMask_Skill = 0x4,
		enPDBMask_Item = 0x8,
		enPDBMask_Buff = 0x10,
		enPDBMask_Task = 0x20,
		enPDBMask_Reserve = 0x40,
	};

	KPlayerUIState      m_PlayerUIState;
	DWORD			    m_dwLastOfflineTime;
	FSGUID              m_Guid;
	time_t              m_LastAddExpTime;
	static unsigned int ms_uCurSystemTime;
	int				    m_PreFightMode;	
	DWORD			    m_dwQuestionScriptId;
	char			    m_szSecPW[64];
	char			    m_szClientSecPW[64];
	DWORD			    m_dwLastLoginIP;
	bool			    m_bPermitChangeServer;
	bool			    m_bPermitAttach;
    time_t              m_tmtCurBegin;
    time_t              m_tmtCurEnd;
    time_t              m_tmtLastEnd;
    DWORD               m_dwPowerValue;
    BYTE                m_byGiftType;
    int                 m_nLSkillID;	//左键技能ID
    int                 m_nRSkillID;	//右键技能ID
    int                 m_SendFlag;		//仅仅为了兼容老客户端	
	BOOL			m_bIsQuiting;
	BOOL			m_bIsCanRemove;
	UINT			m_uMustSave;
	DWORD			m_ulLastSaveTime;
	DWORD			m_dwLoginTime;
	char			m_AccoutName[32];				
	BYTE*			m_pCurStatusOffset;			//二进制时，记录读到指针位置了
	BOOL			m_bFinishLoading;			//完成加载
	int				m_nLastNetOperationTime;	//最后一次网络操作时间
	BOOL			m_bSleepMode;
	int				m_nViewEquipTime;			// 最后一次察看他人装备的时间
	BOOL			m_bNotifyLogout;
	char			m_szBoxPassword[MAX_BOXPASSWORD];
	int				m_nCreditUpTime;
	int				m_nLastUsedSkillType;
	char			m_szPetName[32];//	宠物的名字和类型 服务器端放在KPlayer上,客户端放在Pet所在的KNpc上
	BYTE			m_byPetType;	
	BYTE			m_byPetHonor;
	BYTE			m_byPetColor;
	bool			m_bPetReleased;
	DWORD			m_dwPetTimer;			//宠物在线时间计算
	DWORD			m_dwPetChatTimer;		//宠物上次说话的时间
	BYTE			m_btMorphHue;		  // 变身状态下的HUE
	bool			m_bMorphSendHue;	// 是否已经发送了变身HUE
	bool            m_bPreventAddFriend; //是否处于不可添加好友的状态
	TEMPADDSTATUSINFO	m_tagTempAddCredit;
	TEMPADDSTATUSINFO	m_tagTempAddSkill;
	TEMPADDSTATUSINFO	m_tagTempAddAllSkill;	
	unsigned long		m_offerPostId;
	unsigned long		m_applyPostId;
	unsigned char		m_missionCount;	
	DWORD				m_dwLastDeathTime;
	DWORD				m_dwInitTreasureCount;
	KPlayerTask		m_cTask;						// 玩家任务系统(变量)
	char m_szMasterName[32];
	int m_nPrenticeNum;
	BYTE m_byPrenticeLevel;
	BYTE m_byMasterPRValueTemp;
	DWORD m_NextTeamTime;//下次可以请求队伍列表的时间
	ServerAuctionMgr	m_serverAucMgr;
	AntiEnthrall		m_AntiEnthrall;
	ServerPlayerRealInfoManager m_serverPRIMgr;
private:
	DWORD m_dwTotolJinshabi;
	DWORD m_dwRecentlyTime;
	DWORD m_dwRecentJinshabi;

	const static DWORD	m_DBLoadFinishedFlag;
	DWORD		m_DBLoadProcessFlag;				// 玩家数据全部加载完成才允许自动存盘

	unsigned int m_uPlayCard;
	bool			m_bIsWillRide;
	PLAYER_REVIVAL_POS		m_sLoginRevivalPos;	// 登入重生点位置（会存盘）
	PLAYER_REVIVAL_POS		m_sDeathRevivalPos;	// 死亡重生点（默认为登入重生点，不存盘）	
	PLAYER_REVIVAL_POS		m_sRememberPos;	// 记住的位置
	BOOL			m_bUseReviveIdWhenLogin;
    tagExtPointInfo m_ExtPointInfo;             // 活动点数
	DWORD			m_dwPlayGameTime;			// 角色游戏时间（单位：秒）
	DWORD			m_LoginTime;				// 角色登录时间（单位：秒）
	unsigned long m_OverweightBuffIndex;//超重BUFF的序号
	int m_PkValue;//PK值
	bool m_IsKiller;//是否杀手
	int m_DeathPunish;//是否有死亡惩罚
	int m_PkPunish;//是否有杀人惩罚
	ActionDelayer m_ActionDelayer;//动作延迟器
	RelationSet m_RelationSet;//关系集
	int	m_LogoutTimer;
	PlayerStatistic m_PlayerStatistic;//玩家数据统计
	InstanceInfo	m_InstanceInfo;//副本信息
	int m_SpyLevel;//监视等级
	int m_EnterMapId;
	int m_EnterMapPosX;
	int m_EnterMapPosY;
	int m_RememberSubworldId;
	int m_RememberPosX;
	int m_RememberPosY;
	CastSkillParam m_SelectedSkill;
	CastSkillParam m_NextSkill;
	KServerFuryMgr m_FuryMgr;
	int				m_loadOwnTreeInterval;
	int				m_PlayerSaveTimeInterval;
	DWORD			m_NoChatTime;
	int				m_MarkCreatureNpcIdx;
	Employee		m_Employee;
	DWORD m_NextOpTime[player_op_count];
	InteractiveScriptState m_interactiveScriptState;
	int				m_CombatScoreOneTime;			//本场玩家得分
	int				m_nMyTurn;						//分散玩家同步战场配名数据流的一个参数
	int				m_RecommenderRewardToAdd;//推荐人可以领取的代金券
	int				m_RecommenderRewardTicketAdded;//推荐人已经领取的代金券
	DWORD			m_CreateTime;//角色创建时间
	bool			m_LoginGameScriptDone;//角色登录执行脚本完毕
	QuestionState	m_QuestionState;
	int				m_ExpItemIndex;//经验存储物品
	DWORD			m_PlayerExpGainPercent;//物品存储经验状态下玩家获得的经验百分比
	DWORD			m_ItemExpGainPercent;//物品存储经验状态下物品存储的经验百分比
	
#else // Begin of Client Code---------------------------------------------------------------------

public:

	char			szQuestionDescripte[QUESTIONSIZE / 50];
	char			szAnswerSet[QUESTIONSIZE / 50];
	char			szQuestionBuffer[QUESTIONSIZE];
	unsigned int	m_nQuestionClientLen;
	bool			m_bIsPreRide;				//　是否正在上马
	int				m_RunStatus;				// 是跑还是走
	int				m_nSendMoveFrames;			// 用于控制客户端向服务器发送移动(走或跑)协议的频率，使之不能发送大量的移动协议，减小带宽压力
	bool			m_bEnableToThrowAwayItem;	//是否允许玩家把东西扔到地上
//	bool			m_bBeset;					//是否在打造物品
	void			EnableToThrowAwayItem(BOOL bEnable);
	BOOL			GetThrowAwayItemPermit();

	int				m_nCurrentAttrSyncTime;
	
	DWORD			m_dwWeakTime;
	int				m_nLoseExp;
	ItemPos			m_HandItemPos;
	int				m_CityTaxRate;
	int				m_CityGoodsDiscount;
	ClientAuctionMgr	m_clientAucMgr;
	ClientSocialUnitMgr	m_clientSUMgr;
	ClientPlayerRealInfoManager m_clientPRIMgr;

private:
	bool			m_IsOverweight;
	ClientSocialRelation m_SocialRelation;
	DWORD			m_Money;//游戏币
private:
	
    typedef struct tagRunPackageRecord
	{
		int           nMpsX;
		int           nMpsY;
		bool          bEnble;
		unsigned long nLastFrame;
	}RunPackageRecord;

	RunPackageRecord   m_RunPkRecord;
public:
    void  PushRunPackageRecord(int iDestX,int iDestY);
	void  ClearRunPackageRecord(void);
	void  ActivatePakcageRecord(void);

#endif// End of Client Code---------------------------------------------------------------------

};

inline const PlayerCombatInfo &  KPlayer::GetCombatInfo()const
{
	   return m_CombatInfo;
}


inline void KPlayer::AddCowrie(int delta)
{
}

inline void KPlayer::ResetCowrie()
{
}

inline void KPlayer::ClearScriptResult()
{
	m_nScriptResult = 0;
}

inline RoleSkillSeries KPlayer::GetSkillSeries() const
{
	return m_SkillSeries;
}

inline int KPlayer::GetSeries() const
{
	return Npc[m_nIndex].GetSeries();
}

inline int KPlayer::GetLevel() const
{
	return Npc[m_nIndex].GetLevel();
}

inline DWORD KPlayer::GetExp() const
{
	return m_nExp;
}

inline DWORD KPlayer::GetSkillExp() const
{
	return m_nSkillExp;
}

inline int KPlayer::GetWeightMax() const
{
	return m_nWeightMax;
}

inline bool KPlayer::IsValid() const
{
	return m_dwID > 0;
}

inline int KPlayer::GetNpcIndex() const
{
	return m_nIndex;
}

inline KItemList& KPlayer::GetItemList()
{
	return m_ItemList;
}

inline int KPlayer::GetNetConnectIdx() const
{
	return m_nNetConnectIdx;
}

inline int KPlayer::IsNewPlayer()
{
	return m_bNewPlayer;
}

inline int KPlayer::GetScriptResultNumber()
{
	return m_nScriptResult;
}

inline DWORD KPlayer::GetPlayerID()
{
	return m_dwID;
}

inline LPSTR KPlayer::GetPlayerName()
{
	return m_PlayerName;
}

inline int KPlayer::GetPlayerIndex()
{
	return m_nPlayerIndex;
}

inline bool KPlayer::CanPickup() const
{
	return m_CanPickup;
}

inline bool KPlayer::IsBlockClientControl() const
{
	return m_IsBlockClient;
}

inline void KPlayer::SetBlockClientControl(bool block)
{
	m_IsBlockClient = block;
}

inline KPlayerTeam& KPlayer::GetTeamInfo()
{
	return m_cTeam;
}

inline DWORD KPlayer::GetTicket() const
{
	return m_Ticket;
}

inline void KPlayer::SetTicket(DWORD ticket)
{
	m_Ticket = ticket;
}

inline TitleManager& KPlayer::GetTitleManager()
{
	return m_TitleManager;
}

#ifdef _SERVER
inline void KPlayer::StartLogoutTimer()
{
	int	nLogoutSecs = ConfigManager::Singleton().GetGlobalVariable(global_var_player_logout_time);
	m_LogoutTimer = nLogoutSecs * GAME_FPS;
}

inline void KPlayer::StopLogoutTimer()
{
	m_LogoutTimer = 0;
}

inline bool KPlayer::IsLogoutTiming()
{
	return m_LogoutTimer > 0;
}

inline int KPlayer::GetExpPercentage() const
{
	return m_nExpPercentage;
}

inline void KPlayer::SetExpPercentage(int percentage)
{
	m_nExpPercentage = percentage;
}

inline int KPlayer::GetQuestExpPercentage() const
{
	return m_nQuestExpPercentage;
}

inline void KPlayer::SetQuestExpPercentage(int percentage)
{
	m_nQuestExpPercentage = percentage;
}

inline int KPlayer::GetSkillExpPercentage() const
{
	return m_nSkillExpPercentage;
}

inline void KPlayer::SetSkillExpPercentage(int percentage)
{
	m_nSkillExpPercentage = percentage;
}

inline void KPlayer::GetItemExpState(int& itemIndex, DWORD& playerExpGainPercent, DWORD& itemExpGainPercent) const
{
	itemIndex = m_ExpItemIndex;
	playerExpGainPercent = m_PlayerExpGainPercent;
	itemExpGainPercent = m_ItemExpGainPercent;
}

inline void KPlayer::SetItemExpState(int itemIndex, DWORD playerExpGainPercent, DWORD itemExpGainPercent)
{
	m_ExpItemIndex = itemIndex;
	m_PlayerExpGainPercent = playerExpGainPercent;
	m_ItemExpGainPercent = itemExpGainPercent;
}

#endif

inline DWORD KPlayer::GetEmployTime() const
{
	return m_EmployTime;
}

inline void KPlayer::SetEmployTime(DWORD time)
{
	if (time > MAX_EMPLOY_TIME_LIMIT)
		return;

	m_EmployTime = time;

#ifdef _SERVER
	SyncAttribute(attr_employtime);
#else
	//TODO 通知界面
#endif
}

inline bool KPlayer::IsGM() const
{
	return m_IsGM;
}

#ifdef _SERVER // Begin of Server Code ------------------------------------------------------

inline KServerFuryMgr & KPlayer::GetFurySys()
{
	return m_FuryMgr;
}

inline bool KPlayer::MarkToCreature(int nNpcIdx)
{
	if( IsValidNpc(nNpcIdx) && kind_creature == Npc[nNpcIdx].m_Kind )
	{
		m_MarkCreatureNpcIdx = nNpcIdx;
		return true;
	}
	else
		return false;
}

inline void KPlayer::ClearMarkCreature()
{
	m_MarkCreatureNpcIdx = 0;
}

inline int  KPlayer::GetMarkCreature()
{
	return m_MarkCreatureNpcIdx;
}

inline const FSGUID& KPlayer::GetGUID() const
{
	return m_GUID;
}

inline void KPlayer::SetGUID(const FSGUID& guid)
{
	memcpy(&m_GUID, &guid, sizeof(FSGUID));
}

inline BOOL KPlayer::ModifyPlayCard(unsigned int uNewPick) 
{
	_ASSERT((m_uPlayCard & uNewPick) == 0);
	m_uPlayCard |= uNewPick;
	return TRUE;
}	

inline void KPlayer::NotifyLogout()
{
	m_bNotifyLogout = TRUE;
}

inline BOOL KPlayer::IsNotifyLogout()
{
	return m_bNotifyLogout;
}

inline BOOL KPlayer::IsExistBoxPassword()
{
	if(strlen(m_szBoxPassword) > 0)
	{
		return TRUE;
	}

	return FALSE;
}

inline BOOL KPlayer::SetBoxPassword(const char* pcNewPassword, 
								    const char* pcOldPassword)
{
	if(strlen(pcNewPassword) >= MAX_BOXPASSWORD)
	{
		return FALSE;
	}
	if(strcmp(m_szBoxPassword, pcOldPassword) == 0)
	{
		BOOL OldExistState = IsExistBoxPassword();
		
		strncpy(m_szBoxPassword, pcNewPassword,sizeof(m_szBoxPassword));
		m_szBoxPassword[sizeof(m_szBoxPassword) - 1] = 0;
		
		BOOL NewExistState = IsExistBoxPassword();
		
		if (OldExistState != NewExistState)
			SyncAttribute(attr_passward_state);

		return TRUE;
	}

	return FALSE;
}

inline BOOL KPlayer::ReplaceBoxPassword(const char* pcSecondPassword, BOOL &bNeedREQ)
{
	bNeedREQ = FALSE;

	if(m_szSecPW[0] == '\0')
	{
		strcpy(m_szClientSecPW, pcSecondPassword);
		bNeedREQ = TRUE;
		return FALSE;
	}

	if(strcmp(pcSecondPassword, m_szSecPW) == 0)
	{
		memset(m_szBoxPassword, 0, MAX_BOXPASSWORD);
		return TRUE;
	}

	return FALSE;
}

inline BOOL KPlayer::CheckBoxPassword(const char* pcPassword)
{
	if(strlen(m_szBoxPassword) == 0 || 
		strcmp(m_szBoxPassword, pcPassword) == 0)
	{
		return TRUE;
	}

	return FALSE;
}

inline unsigned long  
KPlayer::OfferPostId() const
{
	return m_offerPostId;
}

inline void		   
KPlayer::OfferPostId(unsigned long id)
{
	m_offerPostId = id;
}

inline unsigned long  
KPlayer::ApplyPostId() const
{
	return m_applyPostId;
}

inline void		   
KPlayer::ApplyPostId(unsigned long id)
{
	m_applyPostId = id;
}

inline unsigned char 
KPlayer::MissionCount() const
{
	return m_missionCount;
}

inline void		   
KPlayer::MissionCount(unsigned char c)
{
	m_missionCount = c;
}

inline BOOL KPlayer::IsWaitingRemove()
{
	if (!m_dwID)
		return FALSE;
	return m_bIsQuiting;
}

inline BOOL KPlayer::IsCanRemove( )
{
	if (!m_dwID)
		return FALSE;
	return m_bIsCanRemove;
}

inline void KPlayer::SetLastUsedSkillType(int nSkillType)
{
	m_nLastUsedSkillType = nSkillType;
}

inline int KPlayer::GetLastUsedSkillType()
{
	return m_nLastUsedSkillType;
}

inline void KPlayer::SetMasterName(LPCSTR pcName)
{
	strcpy(m_szMasterName, pcName);
}

inline LPCSTR KPlayer::GetMasterName()
{
	return m_szMasterName;
}

inline BOOL KPlayer::IsHaveMaster()
{
	return (m_szMasterName[0] != '\0');
}

inline void KPlayer::SetPrenticeCount(int nCount)
{
	m_nPrenticeNum = nCount;
}

inline int KPlayer::GetPrenticeCount()
{
	return m_nPrenticeNum;
}

inline int KPlayer::GetPkValue()
{
	return m_PkValue;
}

inline bool KPlayer::IsKiller()
{
	return m_IsKiller;
}

inline ActionDelayer& KPlayer::GetActionDelayer()
{
	return m_ActionDelayer;
}

inline RelationSet& KPlayer::GetRelationSet()
{
	return m_RelationSet;
}

inline PlayerStatistic& KPlayer::GetPlayerStatistic()
{
	return m_PlayerStatistic;
}

inline bool KPlayer::CanChat()
{
	return ( UNIX_TMIE_STAMP > m_NoChatTime );
}

inline int KPlayer::GetSpyLevel() const
{
	return m_SpyLevel;
}

inline void KPlayer::SetSpyLevel(int spyLevel)
{
	m_SpyLevel = spyLevel;
}

inline void KPlayer::SetPkPunish(int punishLevel)
{
	m_PkPunish = punishLevel;
}

inline void KPlayer::SetDeathPunish(int punishLevel)
{
	m_DeathPunish = punishLevel;
}

inline int KPlayer::GetPkPunish() const
{
	return m_PkPunish;
}

inline int KPlayer::GetDeathPunish() const
{
	return m_DeathPunish;
}

inline void KPlayer::ClearSelectedSkill()
{
	m_SelectedSkill.SkillId = INVALID_SKILL_ID;
	m_SelectedSkill.Param1 = 0;
	m_SelectedSkill.Param2 = 0;
}

inline void KPlayer::SetSelectedSkill(CastSkillParam& skillParam)
{
	m_SelectedSkill = skillParam;
}

inline void KPlayer::SetNextSkill(CastSkillParam& skillParam)
{
	m_NextSkill = skillParam;
}
/*
inline bool KPlayer::PayMoney(DWORD money)
{
	DWORD currentMoney = GetMoney();
	if (money > 0 && currentMoney >= money)
	{
		SetMoney(currentMoney - money);
		return true;
	}
	else
	{
		return false;
	}
}//*/

inline Employee& KPlayer::GetEmployee()
{
	return m_Employee;
}

inline bool KPlayer::CanDoOp(enumPlayerOpType op) const
{
	if (op >= 0 && op < player_op_count)
		return (m_NextOpTime[op] <= UNIX_TMIE_STAMP);
	else
		return false;
}

inline void KPlayer::UpdateOpTime(enumPlayerOpType op, DWORD interval)
{
	if (op >= 0 && op < player_op_count)
		m_NextOpTime[op] = UNIX_TMIE_STAMP + interval;
}

inline int KPlayer::GetInteractiveScriptStep() const
{
	return m_interactiveScriptState.Step;
}

inline void KPlayer::SetInteractiveScriptParam(int paramIndex, int intValue)
{
	if (paramIndex >= 0 && paramIndex < INTERACTIVE_SCRIPT_INT_PARAM_COUNT)
		m_interactiveScriptState.IntParam[paramIndex] = intValue;
}

inline void KPlayer::SetInteractiveScriptParam(int paramIndex, const char* strValue)
{
	if (paramIndex >= 0 && paramIndex < INTERACTIVE_SCRIPT_STR_PARAM_COUNT)
		strncpy(m_interactiveScriptState.StrParam[paramIndex], strValue, INTERACTIVE_SCRIPT_STR_PARAM_LENGTH);
}

inline int KPlayer::GetInteractiveScriptIntParam(int paramIndex) const
{
	if (paramIndex >= 0 && paramIndex < INTERACTIVE_SCRIPT_INT_PARAM_COUNT)
		return m_interactiveScriptState.IntParam[paramIndex];
	else
		return 0;
}

inline void KPlayer::GetInteractiveScriptStrParam(int paramIndex, char* pOutStrBuff, int outBuffSize) const
{
	if (pOutStrBuff && outBuffSize > 0)
	{
		if (paramIndex >= 0 && paramIndex < INTERACTIVE_SCRIPT_STR_PARAM_COUNT)
		{
			strncpy(pOutStrBuff, m_interactiveScriptState.StrParam[paramIndex], outBuffSize);
		}
	}
}

inline InstanceInfo& KPlayer::GetInstanceInfo()
{
	return m_InstanceInfo;
}

inline InstanceInfo::InstanceInfo()
{
	memset(m_InstanceId, 0, sizeof(m_InstanceId));
}

inline InstanceInfo::~InstanceInfo()
{
}

inline DWORD InstanceInfo::GetInstanceId(int worldTemplateId) const
{
	if (worldTemplateId >= 0 && worldTemplateId < INSTANCE_SUBWORLD_START)
		return m_InstanceId[worldTemplateId];
	else
		return INVALID_INSTANCE_ID;
}

inline void InstanceInfo::SetInstanceId(int worldTemplateId, DWORD instanceId)
{
	if (worldTemplateId >= 0 && worldTemplateId < INSTANCE_SUBWORLD_START)
		m_InstanceId[worldTemplateId] = instanceId;
}

inline void KPlayer::GetRememberPos(int& subworldId, int& posX, int& posY) const
{
	subworldId = m_RememberSubworldId;
	posX = m_RememberPosX;
	posY = m_RememberPosY;
}

inline bool KPlayer::AddRecommenderReward(int reward)
{
	if (reward <= 0 || reward > MAX_ADD_RECOMMENDER_REWARD || m_RecommenderRewardToAdd > MAX_RECOMMENDER_REWARD)
		return false;

	if (m_RecommenderRewardToAdd + reward > MAX_RECOMMENDER_REWARD)
		return false;

	m_RecommenderRewardToAdd += reward;
	return true;
}

inline DWORD KPlayer::GetCreateTime() const
{
	return m_CreateTime;
}

inline QuestionState& KPlayer::GetQuestionState()
{
	return m_QuestionState;
}

#else // Begin of Client Code ------------------------------------------------------

#endif // End of Client Code ------------------------------------------------------

// 新提供的默认登入地图 lixuewu 2004.11.30
#define	defTRANSFER_PORT_ID		99
#define	defTRANSFER_PORT_X		51360
#define defTRANSFER_PORT_Y		102944

extern KPlayer	*Player;

//判断是否是有效的PlayerIndex，推荐对每个不确定的PlayerIndex进行判断
inline bool IsValidPlayer(int playerIndex)
{
#ifdef _SERVER
	return (playerIndex > 0 && playerIndex < MAX_PLAYER && Player[playerIndex].IsValid());
#else
	return (playerIndex > 0 && playerIndex < MAX_PLAYER);
#endif
}

#ifndef _SERVER
//得到客户端的玩家
inline KPlayer& GetClientPlayer()
{
	return Player[CLIENT_PLAYER_INDEX];
}
#endif

inline ITaisuiWheel * KPlayer::GetTaisuiWheelSys(void)
{
		return &m_TaisuiSys;
}
//End

//-----------------------------------------------------------------------
// 拆分外观与配色 lixuewu 2004.11.25
//caolei+ 2008.12.10  修改了拆分的位的使用方法，Pal使用高位，Type使用低位
static inline void SplitTP(BYTE Val, int& nType, int& nPal)
{
// 	nPal = (Val & 0x07);
// 	nType = ((Val >> 3) & 0x1F);

	//caolei modified 2008.12.10
	nType = ( Val & 0x1F );
	nPal = ( Val >> 5 ) & 0x07;
}

static inline void SplitTPfromWORD(WORD Val, int& nType, int& nPal)
{
// 	nPal = (Val & 0x3F);
// 	nType = ((Val >> 6) & 0x3FF);
	
	//caolei modified 2008.12.10
	nType = ( Val & 0x3FF );
	nPal = ( Val >> 10 ) & 0x3F;
}


// 合并外观与色相用 lixuewu 2004.11.25
static inline BYTE CombinTP(int nType, int nPal)
{
	//return (((nType & 0x1F) << 3) | (nPal & 0x07));
	//caolei modified 2008.12.10
	return (((nPal & 0x07) << 5) | (nType & 0x1F));
}

static inline WORD CombinTP2WORD(int nType, int nPal)
{
	//return (((nType & 0x3FF) << 6) | (nPal & 0x3F));
	//caolei modified 2008.12.10
	return (((nPal & 0x3F) << 10) | (nType & 0x3FF));
}
//-----------------------------------------------------------------------



#endif //KPlayerH
