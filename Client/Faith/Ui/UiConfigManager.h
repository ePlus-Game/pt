//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 04/3/2007
//      File_base        : KUiConfigManager.h
//      File_ext         : cpp
//      Author           : xiehong
//      Description      : 文件功能描述
//							界面通用配置项（只与界面相关的配置，请不要在此填写与逻辑相关的内容）
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef UI_CFG_MGR_H
#define UI_CFG_MGR_H

#include "ItemCommonDef.h"
#include <vector>
#include <map>

#include "./UiCase/UiItemTip.h"

#define UI_CFG_MAX_CHAN_COUNT 10

enum enAuctionSearchKind
{
	enAuction_Kind_All = 0,
	enAuction_Kind_Equipment,
	enAuction_Kind_Potion,
	enAuction_Kind_Material,
	enAuction_Kind_Gem,
	enAuction_Kind_Others,

	enAuction_Kind_Weapon,
	enAuction_Kind_Armor,
	enAuction_Kind_Ring,
	enAuction_Kind_Amulet,
	enAuction_Kind_Boots,
	enAuction_Kind_Shoulder,
	enAuction_Kind_Helm,
	enAuction_Kind_Cuff,
	enAuction_Kind_Pendant,
	enAuction_Kind_Count
};

using namespace std;

struct RankingCfg 
{
	string itemName;
	unsigned int titleNum;
	vector<string> titleName;
	int param;
	RankingCfg() : titleNum( 0 ), param ( 0 )
	{}
	RankingCfg(const RankingCfg& item)
	{
		itemName=item.itemName;
		titleNum=item.titleNum;
		param=item.param;
		titleName=item.titleName;
	}
};
class KUiCfgLoader  
{
public:
	struct LRSkill 
	{
		int lSkillID;
		int rSkillID;
	};

	struct DefaultLRSkill 
	{
		LRSkill lrSkill[3];
	};

	struct CommonData
	{
		char jinImage[COMMON_CLIENT_MSG_LEN_64];
		char yinImage[COMMON_CLIENT_MSG_LEN_64];
		char tongImage[COMMON_CLIENT_MSG_LEN_64];
		char yesString[COMMON_CLIENT_MSG_LEN_8];
		char noString[COMMON_CLIENT_MSG_LEN_8];
		char comfirmString[COMMON_CLIENT_MSG_LEN_8];
		char cancelString[COMMON_CLIENT_MSG_LEN_8];
		char deleteItemMsg[COMMON_CLIENT_MSG_LEN_256];
		char sellItemMsg[COMMON_CLIENT_MSG_LEN_256];
		char deleteMailMsg[COMMON_CLIENT_MSG_LEN_64];
		char payMoneyMsg[COMMON_CLIENT_MSG_LEN_512];
		char postMoneyMsg[COMMON_CLIENT_MSG_LEN_512];
		char sendBackMailMsg[COMMON_CLIENT_MSG_LEN_64];
		char lifeDetailTemplate[COMMON_CLIENT_MSG_LEN_256];
		char manaDetailTemplate[COMMON_CLIENT_MSG_LEN_256];
		char buyIBItemComfirm[COMMON_CLIENT_MSG_LEN_256];
		int messageBallSpeed;
		int messageBallPlayCycCount;
	};

	struct FaceCfgData
	{
		int index;
		char image[COMMON_CLIENT_MSG_LEN_128];
		char description[COMMON_CLIENT_MSG_LEN_32];
		FaceCfgData(){}
		FaceCfgData(const FaceCfgData& other)
		{
			index = other.index;
			strncpy(image, other.image, COMMON_CLIENT_MSG_LEN_128);
			strncpy(description, other.description, COMMON_CLIENT_MSG_LEN_32 );
		}
	};

	struct FacePanelCfgData
	{
		vector<FaceCfgData> faceList;
		int wndWidth;
		char tipLayoutText[COMMON_CLIENT_MSG_LEN_256];
	};

	struct ChatFrameCfg
	{
		string			frameName;
		vector<string>	channelName;
	};

	struct ChannelInfo
	{
		string chanName;
		string chanNameColor;
		string chanNameFont;
		string chanContentColor;
		string chanContentFont;

		ChannelInfo(string newChanName, string newNameColor, string newNameFont, string newContentColor, string newContentFont)
		{
			chanName			= newChanName;
			chanNameColor		= newNameColor;
			chanNameFont		= newNameColor;
			chanContentColor	= newContentColor;
			chanContentFont		= newContentFont;
		}
		ChannelInfo(const ChannelInfo& other)
		{
			chanName			= other.chanName;
			chanNameColor		= other.chanNameColor;
			chanNameFont		= other.chanNameFont;
			chanContentColor	= other.chanContentColor;
			chanContentFont		= other.chanContentFont;
		}
		ChannelInfo()
		{
			chanName			= "";
			chanNameColor		= "";
			chanNameFont		= "";
			chanContentColor	= "";
			chanContentFont		= "";
		}
	};

	struct ChannelCfgData
	{
		int maxCentence;
		int inputBoxMaxWordCount;
		int windowMinHeight;
		int windowMaxHeight;
		char sayText[COMMON_CLIENT_MSG_LEN_32];
		char reciveText[COMMON_CLIENT_MSG_LEN_32];
		//物品
		char itemNameColor[quality_count][COMMON_CLIENT_MSG_LEN_32];
		char itemNameFont[COMMON_CLIENT_MSG_LEN_32];
		//人物名称
		char playerNameColor[COMMON_CLIENT_MSG_LEN_32];
		char playerNameFont[COMMON_CLIENT_MSG_LEN_32];

		//频道信息
		vector<ChannelInfo> chanStyle;
		
		//频道名
		char systemChanName[COMMON_CLIENT_MSG_LEN_16];
		char worldChanName[COMMON_CLIENT_MSG_LEN_16];
		char localChanName[COMMON_CLIENT_MSG_LEN_16];
		char cozeChanName[COMMON_CLIENT_MSG_LEN_16];
		char shizuChanName[COMMON_CLIENT_MSG_LEN_16];
		char zhuhouChanName[COMMON_CLIENT_MSG_LEN_16];
		char leagueChanName[COMMON_CLIENT_MSG_LEN_16];
		char guojiaChanName[COMMON_CLIENT_MSG_LEN_16];
		char combatChanName[COMMON_CLIENT_MSG_LEN_16];
		char teamChanName[COMMON_CLIENT_MSG_LEN_16];
		char mapChanName[COMMON_CLIENT_MSG_LEN_16];
		char battleChanName[COMMON_CLIENT_MSG_LEN_16];
		char gmChanName[COMMON_CLIENT_MSG_LEN_16];

		//特殊称号
		char zhuzhang[COMMON_CLIENT_MSG_LEN_16];
		char zhuzhangColor[COMMON_CLIENT_MSG_LEN_16];
		char zhuzhangFont[COMMON_CLIENT_MSG_LEN_16];

		char houzhu[COMMON_CLIENT_MSG_LEN_16];
		char houzhuColor[COMMON_CLIENT_MSG_LEN_16];
		char houzhuFont[COMMON_CLIENT_MSG_LEN_16];

		char leagueLeader[COMMON_CLIENT_MSG_LEN_16];
		char leagueColor[COMMON_CLIENT_MSG_LEN_16];
		char leagueFont[COMMON_CLIENT_MSG_LEN_16];
		
		//聊天分页和频道对应关系
		vector<ChatFrameCfg>	framesCfg;

		//聊天官方/非官方字符串
		char personalText[COMMON_CLIENT_MSG_LEN_16];
		char personalTextColor[COMMON_CLIENT_MSG_LEN_16];
		char personalTextFont[COMMON_CLIENT_MSG_LEN_16];
		char officialText[COMMON_CLIENT_MSG_LEN_16];
		char officialTextColor[COMMON_CLIENT_MSG_LEN_16];
		char officialTextFont[COMMON_CLIENT_MSG_LEN_16];
	};

	struct SmithData
	{	
		char yunhunText[COMMON_CLIENT_MSG_LEN_16];
		char moneyText[COMMON_CLIENT_MSG_LEN_16];
		char yaoRateText[COMMON_CLIENT_MSG_LEN_256];
		char normalRateText[COMMON_CLIENT_MSG_LEN_256];
		char randRateText[COMMON_CLIENT_MSG_LEN_256];
		char conditionUnfillColor[COMMON_CLIENT_MSG_LEN_16];
		char conditionFillColor[COMMON_CLIENT_MSG_LEN_16];
		char normalTextColor[COMMON_CLIENT_MSG_LEN_16];
		char textFont[COMMON_CLIENT_MSG_LEN_16];
		char yaoRateImage[COMMON_CLIENT_MSG_LEN_64];
		char normalRateImage[COMMON_CLIENT_MSG_LEN_64];
		char randRateImage[COMMON_CLIENT_MSG_LEN_64];
	};
	
	struct ShopCfgData
	{
		char moneyTextColor[COMMON_CLIENT_MSG_LEN_32];
		char moneyNotEnoughTextColor[COMMON_CLIENT_MSG_LEN_32];
		char itemNameColor[COMMON_CLIENT_MSG_LEN_32];
		char moneyFont[COMMON_CLIENT_MSG_LEN_32];
		char itemFont[COMMON_CLIENT_MSG_LEN_32];
		char noNeedRepairText[COMMON_CLIENT_MSG_LEN_32];
		char normalRepairTipTileText[COMMON_CLIENT_MSG_LEN_32];
		char specialRepairTipTileText[COMMON_CLIENT_MSG_LEN_32];
		char repairTipTileTextFont[COMMON_CLIENT_MSG_LEN_32];
		char repairTipTileTextColor[COMMON_CLIENT_MSG_LEN_32];
	};

	struct NetInfoData
	{
		char NetInfoFont[COMMON_CLIENT_MSG_LEN_32];
		char NetInfoColor[COMMON_CLIENT_MSG_LEN_32];
		int	maxWidth;
	};

	struct LevelUpData 
	{
		char TitleFont[COMMON_CLIENT_MSG_LEN_32];
		char TitleColor[COMMON_CLIENT_MSG_LEN_32];
		char NormalFont[COMMON_CLIENT_MSG_LEN_32];
		char NormalColor[COMMON_CLIENT_MSG_LEN_32];
		char TipFont[COMMON_CLIENT_MSG_LEN_32];
		char TipColor[COMMON_CLIENT_MSG_LEN_32];
		char SpecialFont[COMMON_CLIENT_MSG_LEN_32];
		char SpecialColor[COMMON_CLIENT_MSG_LEN_32];
		int	maxWidth;
	};
	
	struct TipCfgData 
	{
		int windowWidth;
		int topMargin;
		int bottomMargin;
		int leftMargin;
		int RightMargin;
	};

	struct QuestCfgData 
	{
		char descriptionTitleText[COMMON_CLIENT_MSG_LEN_32];
		char descriptionTitleFont[COMMON_CLIENT_MSG_LEN_32];
		char descriptionTitleColor[COMMON_CLIENT_MSG_LEN_32];
		char descriptionFont[COMMON_CLIENT_MSG_LEN_32];
		char descriptionColor[COMMON_CLIENT_MSG_LEN_32];

		char aimTitleText[COMMON_CLIENT_MSG_LEN_32];
		char aimTitleFont[COMMON_CLIENT_MSG_LEN_32];
		char aimTitleColor[COMMON_CLIENT_MSG_LEN_32];
		char aimFont[COMMON_CLIENT_MSG_LEN_32];
		char aimColor[COMMON_CLIENT_MSG_LEN_32];

		char requestFont[COMMON_CLIENT_MSG_LEN_32];
		char requestNormalColor[COMMON_CLIENT_MSG_LEN_32];
		char requestCompleteColor[COMMON_CLIENT_MSG_LEN_32];

		char expText[COMMON_CLIENT_MSG_LEN_64];
		char expFont[COMMON_CLIENT_MSG_LEN_32];
		char expColor[COMMON_CLIENT_MSG_LEN_32];

		char moneyText[COMMON_CLIENT_MSG_LEN_64];
		char moneyFont[COMMON_CLIENT_MSG_LEN_32];
		char moneyColor[COMMON_CLIENT_MSG_LEN_32];

		char aimTip[COMMON_CLIENT_MSG_LEN_1024];

		int wordExtSpace;
		int lineExtSpace;
	};

	struct QuestTrackCfgData 
	{
		int windowWidth;

		char questNameColor[COMMON_CLIENT_MSG_LEN_32];
		char questNameFont[COMMON_CLIENT_MSG_LEN_32];

		char IncompleteColor[COMMON_CLIENT_MSG_LEN_32];
		char IncompleteFont[COMMON_CLIENT_MSG_LEN_32];

		char CompleteColor[COMMON_CLIENT_MSG_LEN_32];
		char CompleteFont[COMMON_CLIENT_MSG_LEN_32];
	};

	struct NpcMsgCfgData 
	{
		char npcMsgFont[COMMON_CLIENT_MSG_LEN_32];
		char npcMsgColor[COMMON_CLIENT_MSG_LEN_32];
		char commitQuestNotSelectErrMsg[COMMON_CLIENT_MSG_LEN_64];
	};

	struct ChatBubble
	{
		int windowWidth;
		int topMargin;
		int bottomMargin;
		int leftMargin;
		int RightMargin;
	};


	struct BuffRestTime
	{
		int		topOffset;
		int		warningTime;
		char	buffFont[COMMON_CLIENT_MSG_LEN_32];
	};

	struct _image_set
	{
		char imageSet[COMMON_CLIENT_MSG_LEN_32];
		char image[COMMON_CLIENT_MSG_LEN_32];
	};

	typedef std::vector<_image_set> _mapimage;

	struct ChangeMapParam 
	{
		DWORD		loadingTime;
		int			tipCount;	
		int			mapCount;
		_mapimage	mapList800;
		_mapimage	mapList1024;
	};

	struct MapInfo
	{
		char	name[COMMON_CLIENT_MSG_LEN_16];
		char	text[COMMON_CLIENT_MSG_LEN_16];
		MapInfo(){}
		MapInfo(const MapInfo& other)
		{
			strcpy(name, other.name);
			strcpy(text, other.text);
		}
	};

	struct SceneMapCfg 
	{
		char	tipColor[COMMON_CLIENT_MSG_LEN_32];
		char	tipFont[COMMON_CLIENT_MSG_LEN_32];
		int		topMargin;
		int		bottomMargin;
		int		leftMargin;
		int		RightMargin;
		vector<MapInfo> mapList;
	};

	struct BigMapPartCfg
	{
		int		index;
		char	name[COMMON_CLIENT_MSG_LEN_16];
		char	normalImage[COMMON_CLIENT_MSG_LEN_128];
		char	hoverImage[COMMON_CLIENT_MSG_LEN_128];
		char	tipText[COMMON_CLIENT_MSG_LEN_1024];
		BigMapPartCfg(){}
		BigMapPartCfg(const BigMapPartCfg& other)
		{
			index = other.index;
			strcpy(name,		other.name);
			strcpy(normalImage, other.normalImage);
			strcpy(hoverImage,	other.hoverImage);
			strcpy(tipText,		other.tipText);
		}
	};
	
	struct BigMapCfg
	{
		vector<BigMapPartCfg> part;
		int tipFidInTime;
	};

	struct PosLinkCfg
	{
		char	color[COMMON_CLIENT_MSG_LEN_32];
		char	font[COMMON_CLIENT_MSG_LEN_32];
	};

	
	struct FriendListCfg
	{
		char	offLineNormalColor[COMMON_CLIENT_MSG_LEN_32];
		char	offLinePushedColor[COMMON_CLIENT_MSG_LEN_32];
		char	offLineHoverColor[COMMON_CLIENT_MSG_LEN_32];
	};

	struct ChatRoomCfg 
	{
		char	p2pSelfNameColor[COMMON_CLIENT_MSG_LEN_32];
		char	p2pSelfNameFont[COMMON_CLIENT_MSG_LEN_32];
		char	p2pTargetNameColor[COMMON_CLIENT_MSG_LEN_32];
		char	p2pTargetNameFont[COMMON_CLIENT_MSG_LEN_32];

		char	p2pSelfChatColor[COMMON_CLIENT_MSG_LEN_32];
		char	p2pSelfChatFont[COMMON_CLIENT_MSG_LEN_32];
		char	p2pTargetChatColor[COMMON_CLIENT_MSG_LEN_32];
		char	p2pTargetChatFont[COMMON_CLIENT_MSG_LEN_32];

		char	p2rNameColor[COMMON_CLIENT_MSG_LEN_32];
		char	p2rMsgColor[COMMON_CLIENT_MSG_LEN_32];
		char	p2rNameFont[COMMON_CLIENT_MSG_LEN_32];
		char	p2rMsgFont[COMMON_CLIENT_MSG_LEN_32];

		char	SendFont[COMMON_CLIENT_MSG_LEN_32];
		char	SendColor[COMMON_CLIENT_MSG_LEN_32];
	};

	struct TrafficLight
	{
		char		type[COMMON_CLIENT_MSG_LEN_32];
		Position	pos;
		char		imagePath[COMMON_CLIENT_MSG_LEN_256];
		bool		imageCyc;
		char		tipText[COMMON_CLIENT_MSG_LEN_256];
		KUiItemTip::TipPos	tipPos;
		TrafficLight(){}
		TrafficLight(const TrafficLight& other)
		{
			strcpy(type, other.type);
			pos = other.pos;
			strcpy(imagePath, other.imagePath);
			imageCyc = other.imageCyc;
			strcpy(tipText, other.tipText);
			tipPos = other.tipPos;
		}
	};
	typedef vector<TrafficLight> TrafficLightList;

	struct ProfessionCfg
	{
		void GetName(int series, int skillSeries, char* pName, size_t size) const
		{
			if (series >= 0 && series <= 2 && skillSeries >= -1 && skillSeries <= 1 && pName != NULL)
				strncpy(pName, m_Name[series][skillSeries + 1], (size < COMMON_CLIENT_MSG_LEN_32 ? size : COMMON_CLIENT_MSG_LEN_32));
		}

		void GetColor(int series, int skillSeries, char* pColor, size_t size) const
		{
			if (series >= 0 && series <= 2 && skillSeries >= -1 && skillSeries <= 1 && pColor != NULL)
				strncpy(pColor, m_Color[series][skillSeries + 1], (size < COMMON_CLIENT_MSG_LEN_32 ? size : COMMON_CLIENT_MSG_LEN_32));
		}

		void GetImage(int series, int skillSeries, char* pImageSet, size_t sizeImageSet, char* pImageName, size_t sizeImageName) const
		{
			if (series >= 0 && series <= 2 && skillSeries >= -1 && skillSeries <= 1 && pImageSet != NULL && pImageName != NULL)
			{
				strncpy(pImageSet, m_ImageSet[series][skillSeries + 1], (sizeImageSet < COMMON_CLIENT_MSG_LEN_32 ? sizeImageSet : COMMON_CLIENT_MSG_LEN_32));
				strncpy(pImageName, m_ImageName[series][skillSeries + 1], (sizeImageName < COMMON_CLIENT_MSG_LEN_32 ? sizeImageName : COMMON_CLIENT_MSG_LEN_32));
			}
		}

		char m_Color[3][3][COMMON_CLIENT_MSG_LEN_32];
		char m_Name[3][3][COMMON_CLIENT_MSG_LEN_32];
		char m_ImageSet[3][3][COMMON_CLIENT_MSG_LEN_32];
		char m_ImageName[3][3][COMMON_CLIENT_MSG_LEN_32];
	};

	struct TradeCfgData
	{
		char		beginTradeMsg[COMMON_CLIENT_MSG_LEN_64];
		char		cancelTradeMsg[COMMON_CLIENT_MSG_LEN_64];
		char		completeTradeMsg[COMMON_CLIENT_MSG_LEN_64];
		char		oppositePickMsg[COMMON_CLIENT_MSG_LEN_64];
		char		oppositeDropMsg[COMMON_CLIENT_MSG_LEN_64];
		char		oppositeRefuseMsg[COMMON_CLIENT_MSG_LEN_64];
		char		tradeForbiddenMsg[COMMON_CLIENT_MSG_LEN_128];
	};

	struct SoundEffectCfg
	{
		char		recvNewCoze[COMMON_CLIENT_MSG_LEN_64];
		char		repair[COMMON_CLIENT_MSG_LEN_64];
		char		costMoney[COMMON_CLIENT_MSG_LEN_64];
		char		pickup[COMMON_CLIENT_MSG_LEN_64];
		char		dropdown[COMMON_CLIENT_MSG_LEN_64];
	};
	
	struct RaidCfg
	{
		char		xingtianHeadImg[COMMON_CLIENT_MSG_LEN_64];
		char		xuanfengHeadImg[COMMON_CLIENT_MSG_LEN_64];
		char		zhenrenHeadImg[COMMON_CLIENT_MSG_LEN_64];
		char		tianshiHeadImg[COMMON_CLIENT_MSG_LEN_64];
		char		yishiHeadImg[COMMON_CLIENT_MSG_LEN_64];
		char		shoushiHeadImg[COMMON_CLIENT_MSG_LEN_64];
		char		normalColor[COMMON_CLIENT_MSG_LEN_32];
		char		hoverColor[COMMON_CLIENT_MSG_LEN_32];
		char		topMemberNormalColor[COMMON_CLIENT_MSG_LEN_32];
		char		topMemberHoverColor[COMMON_CLIENT_MSG_LEN_32];
		int			dragAlpha;
		bool		showLife;
		int			colCount;
	};
	
	struct SKCfg
	{
		char		successMsg[COMMON_CLIENT_MSG_LEN_64];
		char		failMsg[COMMON_CLIENT_MSG_LEN_64];
		char		keyUnbindMsg[COMMON_CLIENT_MSG_LEN_64];
	};

	struct MovieSceneCfg
	{
		char		textColor[COMMON_CLIENT_MSG_LEN_32];
		char		textFont[COMMON_CLIENT_MSG_LEN_32];
	};

	struct CityImageCfg
	{
        int         CityMapID[4];
		char        CityIConSet[COMMON_CLIENT_MSG_LEN_64];
		char        CityImage[4][COMMON_CLIENT_MSG_LEN_128];
	};

	struct CityResCfg
	{
		int         CityMapID[4];
		int         CityResB[4];
		int         CityResF[4];
		int         CityResW[4];
		char        CityDesc[4][COMMON_CLIENT_MSG_LEN_512];
		bool		UseRes;
	};

	struct TongConditionCfg
	{
        char        szShizuCond[COMMON_CLIENT_MSG_LEN_512];
		char        szZhuhouCond[COMMON_CLIENT_MSG_LEN_512];
	};
	
	struct TalismanCfg
	{
		char		activeHoleIms[COMMON_CLIENT_MSG_LEN_32];
		char		activeHoleImage[COMMON_CLIENT_MSG_LEN_32];
		char		deactiveHoleIms[COMMON_CLIENT_MSG_LEN_32];
		char		deactiveHoleImage[COMMON_CLIENT_MSG_LEN_32];
    };

	struct DuraAlertCfg
	{
		char		tipLayout[COMMON_CLIENT_MSG_LEN_128];
		char		redTipSeg[COMMON_CLIENT_MSG_LEN_128];
		char		yellowTipSeg[COMMON_CLIENT_MSG_LEN_128];
		char		normalTipSeg[COMMON_CLIENT_MSG_LEN_128];
		bool		showNormal;
	};

	struct TongSmallImageCfg
	{
        char        GensImagePathOnline[COMMON_CLIENT_MSG_LEN_256];
		char        GensImagePathOffline[COMMON_CLIENT_MSG_LEN_256];
		char	    TongImagePathOnline[COMMON_CLIENT_MSG_LEN_256];
		char	    TongImagePathOffline[COMMON_CLIENT_MSG_LEN_256];
		char	    ForbidChatImagePathOnline[COMMON_CLIENT_MSG_LEN_256];
		char	    ForbidChatPathOffline[COMMON_CLIENT_MSG_LEN_256];
	};

	struct EquipmentCfg
	{
		pair<int, int> pos[gua_pos_count];
		char	guaEffect[GT_Num][GED_Num][COMMON_CLIENT_MSG_LEN_64];
		char	nextLevelColor[COMMON_CLIENT_MSG_LEN_32];
		char	notGrantColor[COMMON_CLIENT_MSG_LEN_32];
		char	grantColor[COMMON_CLIENT_MSG_LEN_32];
	};

	struct DefaultAscIIFontName
	{
		int count;
		vector<String> vecFontName;
	};

	struct PetCfg
	{
		char	selfPetExtendName[COMMON_CLIENT_MSG_LEN_32];
		char	selfPetHeadImage[COMMON_CLIENT_MSG_LEN_128];
		pair<int, int>	selfPetBloodSize;
		pair<int, int>	selfPetPos;
	};

	struct ShizuBannerCfg
	{
		pair<int, int>	size;
		pair<int, int>	pos;
	};

	struct AuctionCfg 
	{
		int timeVeryLong;
		int timeLong;
		int timeMiddle;
		int timeShort;
		int timeVeryShort;

		char	type[enAuction_Kind_Count][COMMON_CLIENT_MSG_LEN_64];
	};

	struct TopMessageCfg
	{
		vector<string> fonts;
		vector<string> msgs;
	};

	struct ElfCfg
	{
		unsigned long elfChangeInterval;
		unsigned long randomHelpInterval;
	};

	struct IBShopCfg
	{
		unsigned long jinshanbiRate;
		unsigned long creditRate;
		unsigned long isUseIE;	
	};

	struct RecommendCfg
	{
		char studentReportText[COMMON_CLIENT_MSG_LEN_256];
		char okText[COMMON_CLIENT_MSG_LEN_16];
		char cancelText[COMMON_CLIENT_MSG_LEN_16];
	};
	struct ServerListCfg
	{
		int	 RecentFileVersion;
		int  MaxRecentServerCount;
	};

	struct MailCfg
	{
		int MailNpcIndex_Chaoge;
		int MailNpcIndex_Jiuli;
		int MailNpcIndex_Beihai;
		int MailNpcIndex_Kunlun;

		int Max_MailCount;
		int Red_MailCount;
		int Red_Day;
		int Green_Day;
	};

	struct InfoBarCfg
	{
		int LagStateSpace;
		int TimeFlipInterval;
	};

	struct QuestionWindowCfg
	{
		int		TotalTime;
		string	QuestionFilePath;
		int		DisableInput;
	};


private:
	CommonData			_commData;
	FacePanelCfgData	_faceData;

	int					_manCount;
	int					_womanCount;

	FacePanelCfgData	_portraitManData;
	FacePanelCfgData	_portraitWomanData;

	FacePanelCfgData	_portraitMidManData;
	FacePanelCfgData	_portraitMidWomanData;

	FacePanelCfgData	_portraitMinManData;
	FacePanelCfgData	_portraitMinWomanData;

	SmithData			_smithData;
	ShopCfgData			_shopData;
	ChannelCfgData		_chanData;
	TipCfgData			_tipData;	
	NetInfoData			_netInfoData;
	LevelUpData			_levelUpData;
	QuestCfgData		_questData;
	QuestTrackCfgData	_questTrackCfg;
	NpcMsgCfgData		_npcMsgData;
	ChatBubble			_chatBubbleData;
	map<int, string>	_commImage;
	BuffRestTime		_buffRestTime;
	ChangeMapParam		_changeMapParam;
	SceneMapCfg			_sceneMapCfg;
	BigMapCfg			_bigMapCfg;
	PosLinkCfg			_posCfg;
	FriendListCfg		_friendListCfg;
	ChatRoomCfg			_chatRoomCfg;
	TrafficLightList	_trafficLightCfg;
	ProfessionCfg		_professionCfg;
	TradeCfgData		_tradeCfg;
	SoundEffectCfg		_soundEffectCfg;
	RaidCfg				_raidCfg;
	SKCfg				_skCfg;
	MovieSceneCfg		_movieSceneCfg;
	CityImageCfg        _cityImageCfg;
	CityResCfg          _cityResCfg;
	TongConditionCfg    _tongConditionCfg;
	TalismanCfg			_talismanCfg;
	DuraAlertCfg		_duraAlertCfg;
	TongSmallImageCfg   _SmallTongImage;
	EquipmentCfg		_equipmentCfg;

	DefaultAscIIFontName _ascIIFontName;

	PetCfg				_petCfg;
	ShizuBannerCfg		_shizuBannerCfg;
	AuctionCfg			_auctionCfg;
	DefaultLRSkill		_defaultLRSkill;
	TopMessageCfg		_topMsgCfg;
	ElfCfg				_elfCfg;
	IBShopCfg			_ibshopCfg;
	ServerListCfg		_serverListCfg;
	MailCfg				_mailCfg;
	InfoBarCfg			_infoBarCfg;
	QuestionWindowCfg   _questionWindowCfg;

	RecommendCfg		_recommendCfg;
	int					_playTime;
	int					_breathTrequency;
	bool				_isOpenAnimationForSkill;
	vector<RankingCfg>  _strengeItemContainer;
    vector<RankingCfg>  _functionItemContainer; 
	colour				_RankingTextColor;
	int					_uncheckSkill;
public:
	KUiCfgLoader();
	~KUiCfgLoader();

	void reload();
	
	//common
	char*	getJinImagePath();
	char*	getYinImagePath();
	char*	getTongImagePath();
	
	//smith
	char* getYunhunText();
	char* getMoneyText();
	char* getYaoRateText();
	char* getNormalRateText();
	
	const DefaultLRSkill&		getDefaultLRSkill()	{ return _defaultLRSkill; };
	const CommonData&			getCommonCfg()		{		return _commData;		};
	const FacePanelCfgData&		getFaceData()		{		return _faceData;		};
	const FacePanelCfgData&		getManPortraitData(){		return _portraitManData;	};
	const FacePanelCfgData&		getWomanPortraitData(){		return _portraitWomanData;	};
	int							getManPortraitCount( void );
	int							getWomanPortraitCount( void );
	void						getManPortraitPath( int nIdx, std::string& imageset, std::string& image );
	void						getWomanPortraitPath( int nIdx, std::string& imageset, std::string& image );
	void						getMidManPortraitPath( int nIdx, std::string& imageset, std::string& image );
	void						getMidWomanPortraitPath( int nIdx, std::string& imageset, std::string& image );
	void						getMinManPortraitPath( int nIdx, std::string& imageset, std::string& image );
	void						getMinWomanPortraitPath( int nIdx, std::string& imageset, std::string& image );

	const SmithData&			getSmithData()		{		return _smithData;		};
	const ShopCfgData&			getShopCfg()		{		return _shopData;		};
		  ChannelCfgData&		getChannelData()	{		return _chanData;		};
	const TipCfgData&			getTipData()		{		return _tipData;		};	
	const NetInfoData&			getNetInfoData()	{		return _netInfoData;	}
	const LevelUpData&			getLevelUpData()	{		return _levelUpData;	};
	const QuestCfgData&			getQuestData()		{		return _questData;		};
	const QuestTrackCfgData&	getQuestTrackData()	{		return _questTrackCfg;	};
	const NpcMsgCfgData&		getNpcMsgData()		{		return _npcMsgData;		};
	const ChatBubble&			getChatBubbleCfg()	{		return _chatBubbleData;	};
	const map<int, string>&		getCommImage()		{		return _commImage;		};
	const BuffRestTime&			getBuffRestTime()	{		return _buffRestTime;	};
	const ChangeMapParam&		getChangeMapParam() {		return _changeMapParam; };
	const SceneMapCfg&			getSceneMapCfg()	{		return _sceneMapCfg;	};
	const BigMapCfg&			getBigMapCfg()		{		return _bigMapCfg;		};
	const PosLinkCfg&			getPosLinkCfg()		{		return _posCfg;			};
	const FriendListCfg&		getFriendListCfg()	{		return _friendListCfg;	};
	const ChatRoomCfg&			getChatRoomCfg()	{		return _chatRoomCfg;	};
	const TrafficLightList&		getTrafficLightCfg(){		return _trafficLightCfg;};
	const ProfessionCfg&		getProfessionCfg()	{		return _professionCfg;	};
	const TradeCfgData&			getTradeCfg()		{		return _tradeCfg;		};
	const SoundEffectCfg&		getSoundEffectCfg()	{		return _soundEffectCfg;	};
	const RaidCfg&				getRaidCfg()		{		return _raidCfg;		};
	const SKCfg&				getSkCfg()			{		return _skCfg;			};
	const MovieSceneCfg&		getMovieSceneCfg()	{		return _movieSceneCfg;	};
	const CityImageCfg &        getCityImageCfg()   {       return _cityImageCfg;   }; 
	const CityResCfg   &        getCityResCfg()     {       return _cityResCfg;     };
	const TongConditionCfg &    getTongConditionCfg(){      return _tongConditionCfg;};
	const TalismanCfg&			getTalismanCfg()	{		return _talismanCfg;	};
	const DuraAlertCfg&			getDuraAlert()		{		return _duraAlertCfg;	};
	const TongSmallImageCfg&    getTongSmallImageCfg(){       return _SmallTongImage; };
	const EquipmentCfg&			getEquipmentCfg(){       return _equipmentCfg; };
	const DefaultAscIIFontName&	getFontName()		{		return _ascIIFontName;	};
	const PetCfg&				getPetCfg()			{		return _petCfg;			};
	const ShizuBannerCfg&		getShizuBannerCfg()	{		return _shizuBannerCfg;	};
	const AuctionCfg&			getAuctionCfg()		{		return _auctionCfg;		};
	const TopMessageCfg&		getTopMessageCfg()	{		return _topMsgCfg;		};
	const ElfCfg&				getElfCfg()			{		return _elfCfg;			};
	const IBShopCfg&			getIBShopCfg()		{		return _ibshopCfg;		};
	const RecommendCfg&			getRecommendCfg()	{		return _recommendCfg;	};
	const ServerListCfg&		getServerListCfg()	{		return _serverListCfg;	};
	const MailCfg&				getMailCfg()		{		return _mailCfg;		};
	const InfoBarCfg&			getInfoBarCfg()		{		return _infoBarCfg;		};
	const QuestionWindowCfg&	getQuestionWindowCfg(){		return _questionWindowCfg; };
	int							getPlayTime(){				return _playTime;			};
	int							getBreathTrequency(){		return _breathTrequency;   };
	bool						getAnimationOpenStatus(){	return _isOpenAnimationForSkill; }
	const vector<RankingCfg>&   getStrengItemContainer(){	return _strengeItemContainer;};
    const vector<RankingCfg>&   getFunctionItemContainer(){ return _functionItemContainer; };
	const colour&               getRankingTextColor(){      return _RankingTextColor; };
	int							getUnCheckSkill()			{   return _uncheckSkill;		};
	static KUiCfgLoader& getSingleton()
	{
		static KUiCfgLoader loader;
		return loader;
	}

	void save();
};

#endif