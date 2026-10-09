//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 04/3/2007
//      File_base        : KUiConfigManager.cpp
//      File_ext         : cpp
//      Author           : xiehong
//      Description      : 文件功能描述
//							界面通用配置项（只与界面相关的配置，请不要在此填写与逻辑相关的内容）
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include <crtdbg.h>

#include "KWin32.h"
#include "UiConfigManager.h"
#include "UiCase/UiBubble.h"
#include "CoreUseNameDef.h"
#include "KIniFile.h"
#include "GameDataDef.h"
#include "cfs_filelogs.h"

KUiCfgLoader::KUiCfgLoader()
{
	reload();
}

KUiCfgLoader::~KUiCfgLoader()
{
}

void KUiCfgLoader::reload()
{
	KIniFile ini;
	KIniFile channelFrameIni;
	if(ini.Load(UI_CFG_STRING) == false)
	{
		return;
	}

	if(channelFrameIni.Load(CONFIG_INI) == false)
	{
		return;
	}

	char tempText[COMMON_CLIENT_MSG_LEN_32];

	//表情

	int imageCount = 0;
	char index[COMMON_CLIENT_MSG_LEN_8];
	char imageText[COMMON_CLIENT_MSG_LEN_256];
	int i = 0;
	ZeroMemory( index, sizeof(index) );
	ZeroMemory( imageText, sizeof(imageText) );
	_faceData.faceList.clear();
	ini.GetString("Face" , "TipLayoutText",		"", _faceData.tipLayoutText,	sizeof(_faceData.tipLayoutText));

	ini.GetInteger("Face", "Count", 0, &imageCount);
	ini.GetInteger("Face", "Width", 0, &_faceData.wndWidth);
	
	_faceData.faceList.reserve(imageCount);
	for( i = 0; i < imageCount; ++i)
	{
		sprintf(index, "%d", i);
		ini.GetString("Face" , index, "", imageText, COMMON_CLIENT_MSG_LEN_64);

		FaceCfgData aFace;
		aFace.index = i;
		sscanf(imageText, "%[^-]--%s", aFace.image, aFace.description);
		_faceData.faceList.push_back(aFace);
	}//*/

	//男头像
	_portraitManData.faceList.clear();
	imageCount = 0;
	ZeroMemory( index, sizeof(index) );
	ZeroMemory( imageText, sizeof(imageText) );

	ini.GetInteger("ManPortrait", "Count", 0, &imageCount);
	ini.GetInteger("ManPortrait", "Width", 0, &_portraitManData.wndWidth);
	_manCount = imageCount;	

	_portraitManData.faceList.reserve(imageCount);
	for(i = 0; i < imageCount; ++i)
	{
		sprintf(index, "%d", i);
		ini.GetString("ManPortrait" , index, "", imageText, COMMON_CLIENT_MSG_LEN_128);

		FaceCfgData aFace;
		aFace.index = i;
		sscanf(imageText, "%[^-]--%s", aFace.image, aFace.description);
		_portraitManData.faceList.push_back(aFace);
	}

	//女头像
	_portraitWomanData.faceList.clear();
	imageCount = 0;
	ZeroMemory( index, sizeof(index) );
	ZeroMemory( imageText, sizeof(imageText) );

	ini.GetInteger("WomanPortrait", "Count", 0, &imageCount);
	ini.GetInteger("WomanPortrait", "Width", 0, &_portraitWomanData.wndWidth);
	_womanCount = imageCount;
	_portraitWomanData.faceList.reserve(imageCount);
	for(i = 0; i < imageCount; ++i)
	{
		sprintf(index, "%d", i);
		ini.GetString("WomanPortrait" , index, "", imageText, COMMON_CLIENT_MSG_LEN_128);

		FaceCfgData aFace;
		aFace.index = i;
		sscanf(imageText, "%[^-]--%s", aFace.image, aFace.description);
		_portraitWomanData.faceList.push_back(aFace);
	}

	//男头像中
	_portraitMidManData.faceList.clear();
	imageCount = 0;
	ZeroMemory( index, sizeof(index) );
	ZeroMemory( imageText, sizeof(imageText) );

	ini.GetInteger("MidManPortrait", "Count", 0, &imageCount);
	ini.GetInteger("MidManPortrait", "Width", 0, &_portraitMidManData.wndWidth);
	
	_portraitMidManData.faceList.reserve(imageCount);
	for(i = 0; i < imageCount; ++i)
	{
		sprintf(index, "%d", i);
		ini.GetString("MidManPortrait" , index, "", imageText, COMMON_CLIENT_MSG_LEN_128);

		FaceCfgData aFace;
		aFace.index = i;
		sscanf(imageText, "%[^-]--%s", aFace.image, aFace.description);
		_portraitMidManData.faceList.push_back(aFace);
	}

	//女头像中
	_portraitMidWomanData.faceList.clear();
	imageCount = 0;
	ZeroMemory( index, sizeof(index) );
	ZeroMemory( imageText, sizeof(imageText) );

	ini.GetInteger("MidWomanPortrait", "Count", 0, &imageCount);
	ini.GetInteger("MidWomanPortrait", "Width", 0, &_portraitMidWomanData.wndWidth);
	
	_portraitMidWomanData.faceList.reserve(imageCount);
	for(i = 0; i < imageCount; ++i)
	{
		sprintf(index, "%d", i);
		ini.GetString("MidWomanPortrait" , index, "", imageText, COMMON_CLIENT_MSG_LEN_128);

		FaceCfgData aFace;
		aFace.index = i;
		sscanf(imageText, "%[^-]--%s", aFace.image, aFace.description);
		_portraitMidWomanData.faceList.push_back(aFace);
	}

	//男头像小
	_portraitMinManData.faceList.clear();
	imageCount = 0;
	ZeroMemory( index, sizeof(index) );
	ZeroMemory( imageText, sizeof(imageText) );

	ini.GetInteger("MinManPortrait", "Count", 0, &imageCount);
	ini.GetInteger("MinManPortrait", "Width", 0, &_portraitMinManData.wndWidth);
	
	_portraitMinManData.faceList.reserve(imageCount);
	for(i = 0; i < imageCount; ++i)
	{
		sprintf(index, "%d", i);
		ini.GetString("MinManPortrait" , index, "", imageText, COMMON_CLIENT_MSG_LEN_128);

		FaceCfgData aFace;
		aFace.index = i;
		sscanf(imageText, "%[^-]--%s", aFace.image, aFace.description);
		_portraitMinManData.faceList.push_back(aFace);
	}

	//女头像小
	_portraitMinWomanData.faceList.clear();
	imageCount = 0;
	ZeroMemory( index, sizeof(index) );
	ZeroMemory( imageText, sizeof(imageText) );

	ini.GetInteger("MinWomanPortrait", "Count", 0, &imageCount);
	ini.GetInteger("MinWomanPortrait", "Width", 0, &_portraitMinWomanData.wndWidth);
	
	_portraitMinWomanData.faceList.reserve(imageCount);
	for(i = 0; i < imageCount; ++i)
	{
		sprintf(index, "%d", i);
		ini.GetString("MinWomanPortrait" , index, "", imageText, COMMON_CLIENT_MSG_LEN_128);

		FaceCfgData aFace;
		aFace.index = i;
		sscanf(imageText, "%[^-]--%s", aFace.image, aFace.description);
		_portraitMinWomanData.faceList.push_back(aFace);
	}

	//通用数据
	ini.GetString("Common" , "JinImage",			"", _commData.jinImage,							COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("Common" , "YinImage",			"", _commData.yinImage,							COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("Common" , "TongImage",			"", _commData.tongImage,						COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("Common" , "Comfirm",				"", _commData.comfirmString,					COMMON_CLIENT_MSG_LEN_8);
	ini.GetString("Common" , "Cancel",				"", _commData.cancelString,						COMMON_CLIENT_MSG_LEN_8);
	ini.GetString("Common" , "Yes",					"", _commData.yesString,						COMMON_CLIENT_MSG_LEN_8);
	ini.GetString("Common" , "No",					"", _commData.noString,							COMMON_CLIENT_MSG_LEN_8);
	ini.GetString("Common" , "DeleteItemMsg",		"", _commData.deleteItemMsg,					COMMON_CLIENT_MSG_LEN_256);
	ini.GetString("Common" , "SellItemMsg",			"", _commData.sellItemMsg,						COMMON_CLIENT_MSG_LEN_256);
	ini.GetString("Common" , "DeleteMailMsg",		"", _commData.deleteMailMsg,					COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("Common" , "PayMoneyMsg",			"", _commData.payMoneyMsg,						COMMON_CLIENT_MSG_LEN_512);
	ini.GetString("Common" , "PostMoneyMsg",		"", _commData.postMoneyMsg,						COMMON_CLIENT_MSG_LEN_512);
	ini.GetString("Common" , "SendBackMailMsg",		"", _commData.sendBackMailMsg,					COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("Common" , "LifeDetailTemplate",	"", _commData.lifeDetailTemplate,				COMMON_CLIENT_MSG_LEN_256);
	ini.GetString("Common" , "ManaDetailTemplate",	"", _commData.manaDetailTemplate,				COMMON_CLIENT_MSG_LEN_256);
	ini.GetString("Common" , "BuyIBItemComfirm",	"", _commData.buyIBItemComfirm,					COMMON_CLIENT_MSG_LEN_256);
	ini.GetInteger("Common", "MessageBallSpeed",	5,	&_commData.messageBallSpeed);
	ini.GetInteger("Common", "MessageBallPlayCycCount",	-1,	&_commData.messageBallPlayCycCount);

	//打造数据
	ini.GetString("Smith" , "YunhunString",			"", _smithData.yunhunText,						COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Smith" , "MoneyString",			"", _smithData.moneyText,						COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Smith" , "YaoRateString",		"", _smithData.yaoRateText,						COMMON_CLIENT_MSG_LEN_256);
	ini.GetString("Smith" , "NormalRateString",		"", _smithData.normalRateText,					COMMON_CLIENT_MSG_LEN_256);
	ini.GetString("Smith" , "RandRateString",		"", _smithData.randRateText,					COMMON_CLIENT_MSG_LEN_256);
	ini.GetString("Smith" , "ConditionUnfillColor",	"", _smithData.conditionUnfillColor,			COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Smith" , "ConditionFillColor",	"", _smithData.conditionFillColor,				COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Smith" , "NormalTextColor",		"", _smithData.normalTextColor,					COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Smith" , "TextFont",				"", _smithData.textFont,						COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Smith" , "YaoRateImage",			"", _smithData.yaoRateImage,					COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("Smith" , "NormalRateImage",		"", _smithData.normalRateImage,					COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("Smith" , "RandRateImage",		"", _smithData.randRateImage,					COMMON_CLIENT_MSG_LEN_64);
	
	//商店数据
	ini.GetString("Shop" , "NormalMoneyColor",		"", _shopData.moneyTextColor,					COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Shop" , "NotEnoughMoneyColor",	"", _shopData.moneyNotEnoughTextColor,			COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Shop" , "ItemNameColor",			"", _shopData.itemNameColor,					COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Shop" , "ItemNameFont",			"", _shopData.itemFont,							COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Shop" , "MoneyFont",				"", _shopData.moneyFont,						COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Shop" , "RATipTitleColor",		"", _shopData.repairTipTileTextColor,			COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Shop" , "RATipTitleFont",		"", _shopData.repairTipTileTextFont,			COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Shop" , "NRATipTitleText",		"", _shopData.normalRepairTipTileText,			COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Shop" , "SRATipTitleText",		"", _shopData.specialRepairTipTileText,			COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Shop" , "NoNeedRepairText",		"", _shopData.noNeedRepairText,					COMMON_CLIENT_MSG_LEN_32);

	//聊天数据
	ini.GetString("Channel",	"SayText",				"",	_chanData.sayText,				COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Channel",	"ReciveText",			"",	_chanData.reciveText,			COMMON_CLIENT_MSG_LEN_32);
	ini.GetInteger("Channel",	"MaxCentence",			0,	&_chanData.maxCentence);
	ini.GetInteger("Channel",	"InputBoxMaxWordCount",	0,	&_chanData.inputBoxMaxWordCount);
	ini.GetInteger("Channel",	"WindowMinHeight",		0,	&_chanData.windowMinHeight);
	ini.GetInteger("Channel",	"WindowMaxHeight",		0,	&_chanData.windowMaxHeight);

	for(int j = 0; j < quality_count; ++j)
	{
		sprintf(tempText, "ItemNameColor%d", j + 1);
		ini.GetString("Channel" , tempText,				"", _chanData.itemNameColor[j],			COMMON_CLIENT_MSG_LEN_32);
	}

	ini.GetString("Channel", "PlayerNameColor",			"", _chanData.playerNameColor,			COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Channel", "PlayerNameFont",			"", _chanData.playerNameFont,			COMMON_CLIENT_MSG_LEN_32);

	int chanCount = 0;
	ini.GetInteger("Channel", "ChanCount", 0, &chanCount);

	char chanName[COMMON_CLIENT_MSG_LEN_8];
	char chanNameColor[COMMON_CLIENT_MSG_LEN_32];
	char chanNameFont[COMMON_CLIENT_MSG_LEN_32];
	char chanContentColor[COMMON_CLIENT_MSG_LEN_32];
	char chanContentFont[COMMON_CLIENT_MSG_LEN_32];

	for(int chanIndex = 0; chanIndex < chanCount; ++chanIndex)
	{
		char propertyName[COMMON_CLIENT_MSG_LEN_64];
		sprintf(propertyName, "Chan%dName",			chanIndex + 1);
		ini.GetString("Channel", propertyName,	"", chanName,			COMMON_CLIENT_MSG_LEN_8);
		sprintf(propertyName, "Chan%dNameColor",	chanIndex + 1);
		ini.GetString("Channel", propertyName,	"", chanNameColor,		COMMON_CLIENT_MSG_LEN_32);
		sprintf(propertyName, "Chan%dNameFont",		chanIndex + 1);
		ini.GetString("Channel", propertyName,	"", chanNameFont,		COMMON_CLIENT_MSG_LEN_32);
		sprintf(propertyName, "Chan%dContentColor", chanIndex + 1);
		ini.GetString("Channel", propertyName,	"", chanContentColor,	COMMON_CLIENT_MSG_LEN_32);
		sprintf(propertyName, "Chan%dContentFont",	chanIndex + 1);
		ini.GetString("Channel", propertyName,	"", chanContentFont,	COMMON_CLIENT_MSG_LEN_32);
		_chanData.chanStyle.push_back(ChannelInfo(chanName, chanNameColor, chanNameFont, chanContentColor, chanContentFont));
	}

	//常用频道名
	ini.GetString("Channel", "System",	"", _chanData.systemChanName,			COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Channel", "World",	"", _chanData.worldChanName,			COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Channel", "Combat",	"", _chanData.combatChanName,			COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Channel", "Shizhu",	"", _chanData.shizuChanName,			COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Channel", "Zhuhou",	"", _chanData.zhuhouChanName,			COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Channel", "League",	"", _chanData.leagueChanName,			COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Channel", "GuoJia",	"", _chanData.guojiaChanName,			COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Channel", "Coze",	"", _chanData.cozeChanName,				COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Channel", "Local",	"", _chanData.localChanName,			COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Channel", "Map",		"", _chanData.mapChanName,				COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Channel", "Team",	"", _chanData.teamChanName,				COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Channel", "Battle",	"", _chanData.battleChanName,			COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Channel", "GM",		"", _chanData.gmChanName,				COMMON_CLIENT_MSG_LEN_16);

	//特殊称号
	ini.GetString("Channel", "ZhuZhang",		"", _chanData.zhuzhang,			COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Channel", "ZhuZhangColor",	"", _chanData.zhuzhangColor,	COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Channel", "ZhuZhangFont",	"", _chanData.zhuzhangFont,		COMMON_CLIENT_MSG_LEN_16);

	ini.GetString("Channel", "HouZhu",			"", _chanData.houzhu,			COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Channel", "HouZhuColor",		"", _chanData.houzhuColor,		COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Channel", "HouZhuFont",		"", _chanData.houzhuFont,		COMMON_CLIENT_MSG_LEN_16);

	ini.GetString("Channel", "LeagueLeader",	"", _chanData.leagueLeader,		COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Channel", "LeagueColor",		"", _chanData.leagueColor,		COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Channel", "LeagueFont",		"", _chanData.leagueFont,		COMMON_CLIENT_MSG_LEN_16);

	int frameCount = 0;
	ini.GetInteger("Channel", "FrameCount",	0,	&frameCount);
	_chanData.framesCfg.clear();
	for(int frameIndex = 0; frameIndex < frameCount; ++frameIndex)
	{
		ChatFrameCfg frameCfg;

		sprintf(tempText, "Frame%dName", frameIndex);
		char frameName[COMMON_CLIENT_MSG_LEN_16];
		ini.GetString("Channel", tempText,	"", frameName,	COMMON_CLIENT_MSG_LEN_16);
		channelFrameIni.GetString("Channel", tempText,	frameName, frameName,	COMMON_CLIENT_MSG_LEN_16);
		frameCfg.frameName = string(frameName);

		for(int chanIndex = 0; chanIndex < UI_CFG_MAX_CHAN_COUNT; ++chanIndex)
		{
			sprintf(tempText, "Frame%dChannel%d", frameIndex, chanIndex);
			char registChannel[COMMON_CLIENT_MSG_LEN_64];
			ini.GetString("Channel", tempText,	"", registChannel,	COMMON_CLIENT_MSG_LEN_16);
			channelFrameIni.GetString("Channel", tempText,	registChannel, registChannel, COMMON_CLIENT_MSG_LEN_16);
			if(registChannel[0] == 0)
			{
				continue;
			}
			frameCfg.channelName.push_back(string(registChannel));
		}
		_chanData.framesCfg.push_back(frameCfg);
	}
	
	//聊天非官方字符串
	ini.GetString("Channel", "PersonalText",		"", _chanData.personalText,			COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Channel", "PersonalTextColor",	"", _chanData.personalTextColor,	COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Channel", "PersonalTextFont",	"", _chanData.personalTextFont,		COMMON_CLIENT_MSG_LEN_16);
	
	ini.GetString("Channel", "OfficialText",		"", _chanData.officialText,			COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Channel", "OfficialTextColor",	"", _chanData.officialTextColor,	COMMON_CLIENT_MSG_LEN_16);
	ini.GetString("Channel", "OfficialTextFont",	"", _chanData.officialTextFont,		COMMON_CLIENT_MSG_LEN_16);


	//位置链接配置
	ini.GetString("PositionLink",		"LinkFont",		"", _posCfg.font,						COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("PositionLink",		"LinkColor",	"", _posCfg.color,						COMMON_CLIENT_MSG_LEN_32);

	// 左右键默认技能设置
	for ( int nDefaultSkillIdx = 0; nDefaultSkillIdx < 3; ++nDefaultSkillIdx )
	{
		char szSection[COMMON_CLIENT_MSG_LEN_64];
		sprintf( szSection, "DefaultLRSkill%d", nDefaultSkillIdx );
		ini.GetInteger( szSection,		"LSkill",			0,	&_defaultLRSkill.lrSkill[nDefaultSkillIdx].lSkillID);
		ini.GetInteger( szSection,		"RSkill",			0,	&_defaultLRSkill.lrSkill[nDefaultSkillIdx].rSkillID);
	}

	//Tip配置
	ini.GetInteger("Tip",		"WindowWidth",			0,	&_tipData.windowWidth);
	ini.GetInteger("Tip",		"TopMargin",			0,	&_tipData.topMargin);
	ini.GetInteger("Tip",		"BottomMargin",			0,	&_tipData.bottomMargin);
	ini.GetInteger("Tip",		"LeftMargin",			0,	&_tipData.leftMargin);
	ini.GetInteger("Tip",		"RightMargin",			0,	&_tipData.RightMargin);

	//网络状况配置
	ini.GetInteger("NetInfo", "MaxWidth",				0, &_netInfoData.maxWidth);
	ini.GetString("NetInfo" , "NetInfoFont",			"", _netInfoData.NetInfoFont,			COMMON_CLIENT_MSG_LEN_128);
	ini.GetString("NetInfo" , "NetInfoColor",			"", _netInfoData.NetInfoColor,			COMMON_CLIENT_MSG_LEN_128);

	//升级信息配置	
	ini.GetInteger("LevelUpInfo", "MaxWidth",			0, &_levelUpData.maxWidth);
	ini.GetString("LevelUpInfo", "TitleFont",			"", _levelUpData.TitleFont,				COMMON_CLIENT_MSG_LEN_128);
	ini.GetString("LevelUpInfo" , "TitleColor",			"", _levelUpData.TitleColor,			COMMON_CLIENT_MSG_LEN_128);
	ini.GetString("LevelUpInfo" , "NormalFont",			"", _levelUpData.NormalFont,			COMMON_CLIENT_MSG_LEN_128);
	ini.GetString("LevelUpInfo" , "NormalColor",		"", _levelUpData.NormalColor,			COMMON_CLIENT_MSG_LEN_128);
	ini.GetString("LevelUpInfo" , "TipFont",			"", _levelUpData.TipFont,				COMMON_CLIENT_MSG_LEN_128);
	ini.GetString("LevelUpInfo" , "TipColor",			"", _levelUpData.TipColor,				COMMON_CLIENT_MSG_LEN_128);
	ini.GetString("LevelUpInfo" , "SpecialFont",		"", _levelUpData.SpecialFont,			COMMON_CLIENT_MSG_LEN_128);
	ini.GetString("LevelUpInfo" , "SpecialColor",		"", _levelUpData.SpecialColor,			COMMON_CLIENT_MSG_LEN_128);

	//附近聊天气泡
	ini.GetInteger("ChatBubble",	"WindowWidth",		0,	&_chatBubbleData.windowWidth);
	ini.GetInteger("ChatBubble",	"TopMargin",		0,	&_chatBubbleData.topMargin);
	ini.GetInteger("ChatBubble",	"BottomMargin",		0,	&_chatBubbleData.bottomMargin);
	ini.GetInteger("ChatBubble",	"LeftMargin",		0,	&_chatBubbleData.leftMargin);
	ini.GetInteger("ChatBubble",	"RightMargin",		0,	&_chatBubbleData.RightMargin);
	
	//quest配置
	ini.GetString("Quest",	"DescriptionTitleText",		"", _questData.descriptionTitleText,	COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Quest",	"DescriptionTitleFont",		"", _questData.descriptionTitleFont,	COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Quest",	"DescriptionTitleColor",	"", _questData.descriptionTitleColor,	COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Quest",	"DescriptionFont",			"", _questData.descriptionFont,			COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Quest",	"DescriptionColor",			"", _questData.descriptionColor,		COMMON_CLIENT_MSG_LEN_32);
	
	ini.GetString("Quest",	"AimTitleText",				"", _questData.aimTitleText,			COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Quest",	"AimTitleFont",				"", _questData.aimTitleFont,			COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Quest",	"AimTitleColor",			"", _questData.aimTitleColor,			COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Quest",	"AimFont",					"", _questData.aimFont,					COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Quest",	"AimColor",					"", _questData.aimColor,				COMMON_CLIENT_MSG_LEN_32);

	ini.GetString("Quest",	"RequestFont",				"", _questData.requestFont,				COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Quest",	"RequestNormalColor",		"", _questData.requestNormalColor,		COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Quest",	"RequestCompleteColor",		"",	_questData.requestCompleteColor,	COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Quest",	"ExpText",					"",	_questData.expText,					COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("Quest",	"ExpFont",					"",	_questData.expFont,					COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Quest",	"ExpColor",					"",	_questData.expColor,				COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Quest",	"MoneyText",				"",	_questData.moneyText,				COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("Quest",	"MoneyFont",				"",	_questData.moneyFont,				COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Quest",	"MoneyColor",				"",	_questData.moneyColor,				COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Quest",	"AimTip",					"",	_questData.aimTip,					COMMON_CLIENT_MSG_LEN_1024);
	ini.GetInteger("Quest",	"WordExtSpace",				0,	&_questData.wordExtSpace);
	ini.GetInteger("Quest",	"LineExtSpace",				0,	&_questData.lineExtSpace);
	
	//questtrack配置
	ini.GetInteger("QuestTrack",	"WindowWidth",		0,	&_questTrackCfg.windowWidth);
	ini.GetString("QuestTrack",	"QuestNameColor",		"", _questTrackCfg.questNameColor,		COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("QuestTrack",	"QuestNameFont",		"", _questTrackCfg.questNameFont,		COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("QuestTrack",	"IncompleteColor",		"", _questTrackCfg.IncompleteColor,		COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("QuestTrack",	"IncompleteFont",		"", _questTrackCfg.IncompleteFont,		COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("QuestTrack",	"CompleteColor",		"", _questTrackCfg.CompleteColor,		COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("QuestTrack",	"CompleteFont",			"", _questTrackCfg.CompleteFont,		COMMON_CLIENT_MSG_LEN_32);


	//NPC对话相关配置
	ini.GetString("NpcMsg",	"NpcMsgFont",				"",	_npcMsgData.npcMsgFont,				COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("NpcMsg",	"NpcMsgColor",				"",	_npcMsgData.npcMsgColor,			COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("NpcMsg",	"Commit_NotSelectErrMsg",	"",	_npcMsgData.commitQuestNotSelectErrMsg,	COMMON_CLIENT_MSG_LEN_64);

	
	//NPC列表中的图片
	_commImage.clear();
	int commImageCount = 0;
	ini.GetInteger("CommonImage", "Count", 0, &commImageCount);
	
	char imagePath[COMMON_CLIENT_MSG_LEN_128];
	for(int k = 0; k < commImageCount; ++k)
	{
		sprintf(index, "%d", k);
		ini.GetString("CommonImage" , index, "", imagePath, COMMON_CLIENT_MSG_LEN_64);

		_commImage[k] = string(imagePath);
	}

	// BUFF剩余时间相关设置
	ini.GetString("Buff", "BuffFont", "stzhongs-9", _buffRestTime.buffFont, COMMON_CLIENT_MSG_LEN_32);
	ini.GetInteger("Buff", "TopPos", 0, &_buffRestTime.topOffset);
	ini.GetInteger("Buff", "WarningTime", 5, &_buffRestTime.warningTime);

	// 场景更换界面相关
	ini.GetInteger("ChangeMapParam", "LoadingTime", 5, (int*)&_changeMapParam.loadingTime);
	ini.GetInteger("ChangeMapParam", "TipCount", 1, &_changeMapParam.tipCount);
	ini.GetInteger("ChangeMapParam", "MapCount", 1, &_changeMapParam.mapCount);
	for ( int nMapIdx = 0; nMapIdx < _changeMapParam.mapCount; ++nMapIdx )
	{
		_image_set mapImage;
		
		char szBuff[COMMON_CLIENT_MSG_LEN_256];
		char szString[COMMON_CLIENT_MSG_LEN_256];
		sprintf(szBuff, "800ChangeMap_%d", nMapIdx );
		ini.GetString("ChangeMapParam", szBuff, "", szString, COMMON_CLIENT_MSG_LEN_256);
		sscanf( szString, "imageset:%s image:%s", mapImage.imageSet, mapImage.image );
		_changeMapParam.mapList800.push_back( mapImage );

		sprintf(szBuff, "1024ChangeMap_%d", nMapIdx );
		ini.GetString("ChangeMapParam", szBuff, "", szString, COMMON_CLIENT_MSG_LEN_256);
		sscanf( szString, "imageset:%s image:%s", mapImage.imageSet, mapImage.image );
		_changeMapParam.mapList1024.push_back( mapImage );
	}

	//场景地图
	ini.GetString("SceneMapCfg",	"TipFont",				"", _sceneMapCfg.tipFont,				COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("SceneMapCfg",	"TipColor",				"", _sceneMapCfg.tipColor,				COMMON_CLIENT_MSG_LEN_32);
	ini.GetInteger("SceneMapCfg",	"TopMargin",			0,	&_sceneMapCfg.topMargin);
	ini.GetInteger("SceneMapCfg",	"BottomMargin",			0,	&_sceneMapCfg.bottomMargin);
	ini.GetInteger("SceneMapCfg",	"LeftMargin",			0,	&_sceneMapCfg.leftMargin);
	ini.GetInteger("SceneMapCfg",	"RightMargin",			0,	&_sceneMapCfg.RightMargin);
	int mapCount = 0;
	ini.GetInteger("SceneMapCfg",	"MapCount",				0,	&mapCount);
	for(int mapIndex = 0; mapIndex < mapCount; ++mapIndex)
	{
		MapInfo mapInfo;
		
		sprintf(tempText, "Map%dName", mapIndex + 1);
		ini.GetString("SceneMapCfg",	tempText,	"", mapInfo.name,			COMMON_CLIENT_MSG_LEN_16);
		
		sprintf(tempText, "Map%dText", mapIndex + 1);
		ini.GetString("SceneMapCfg",	tempText,	"", mapInfo.text,			COMMON_CLIENT_MSG_LEN_16);
		_sceneMapCfg.mapList.push_back(mapInfo);
	}

	//大地图
	int partCount;
	_bigMapCfg.part.clear();
	ini.GetInteger("BigMap",		"PartCount",			0,	&partCount);
	for(int partIndex = 0; partIndex < partCount; ++partIndex)
	{
		BigMapPartCfg partCfg;
		partCfg.index = partIndex + 1;
		sprintf(tempText, "Part%dName", partIndex + 1);
		ini.GetString("BigMap",	tempText,	"", partCfg.name,			COMMON_CLIENT_MSG_LEN_16);
		sprintf(tempText, "Part%dNormalImage", partIndex + 1);
		ini.GetString("BigMap",	tempText,	"", partCfg.normalImage,	COMMON_CLIENT_MSG_LEN_128);
		sprintf(tempText, "Part%dHoverImage", partIndex + 1);
		ini.GetString("BigMap",	tempText,	"", partCfg.hoverImage,		COMMON_CLIENT_MSG_LEN_128);	
		sprintf(tempText, "Part%dTip", partIndex + 1);
		ini.GetString("BigMap",	tempText,	"", partCfg.tipText,		COMMON_CLIENT_MSG_LEN_1024);
		_bigMapCfg.part.push_back(partCfg);
	}
	
	ini.GetInteger("BigMap", "TipFidInTime", 1000, &_bigMapCfg.tipFidInTime);

	//好友列表地图
	ini.GetString("FriendListInfo",	"OffLineNormalColor",	"", _friendListCfg.offLineNormalColor,	COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("FriendListInfo",	"OffLinePushedColor",	"", _friendListCfg.offLinePushedColor,	COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("FriendListInfo",	"OffLineHoverColor",	"", _friendListCfg.offLineHoverColor,	COMMON_CLIENT_MSG_LEN_32);

	//聊天室相关
	ini.GetString("ChatRoom",	"P2PSelfNameColor",			"", _chatRoomCfg.p2pSelfNameColor,		COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("ChatRoom",	"P2PTargetNameColor",		"", _chatRoomCfg.p2pTargetNameColor,	COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("ChatRoom",	"P2PSelfMsgColor",			"", _chatRoomCfg.p2pSelfChatColor,		COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("ChatRoom",	"P2PTargetMsgColor",		"", _chatRoomCfg.p2pTargetChatColor,	COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("ChatRoom",	"P2RNameColor",				"", _chatRoomCfg.p2rNameColor,			COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("ChatRoom",	"P2RMsgColor",				"", _chatRoomCfg.p2rMsgColor,			COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("ChatRoom",	"SendColor",				"", _chatRoomCfg.SendColor,				COMMON_CLIENT_MSG_LEN_32);

	ini.GetString("ChatRoom",	"P2PSelfNameFont",			"", _chatRoomCfg.p2pSelfNameFont,		COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("ChatRoom",	"P2PTargetNameFont",		"", _chatRoomCfg.p2pTargetNameFont,		COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("ChatRoom",	"P2PSelfMsgFont",			"", _chatRoomCfg.p2pSelfChatFont,		COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("ChatRoom",	"P2PTargetMsgFont",			"", _chatRoomCfg.p2pTargetChatFont,		COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("ChatRoom",	"P2RNameFont",				"", _chatRoomCfg.p2rNameFont,			COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("ChatRoom",	"P2RMsgFont",				"", _chatRoomCfg.p2rMsgFont,			COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("ChatRoom",	"SendFont",					"", _chatRoomCfg.SendFont,				COMMON_CLIENT_MSG_LEN_32);

	//traffic light 相关
	_trafficLightCfg.clear();
	int trafficLightCount = 0;
	ini.GetInteger("Face", "Count", 0, &trafficLightCount);
	char propertyName[COMMON_CLIENT_MSG_LEN_128];
	for(int l = 0; l < trafficLightCount; ++l)
	{
		TrafficLight aNewTL;
		sprintf(propertyName, "TrafL%d_Type", l);
		ini.GetString("TrafficLight",	propertyName,	"", aNewTL.type,		COMMON_CLIENT_MSG_LEN_32);

		sprintf(propertyName, "TrafL%d_PosX", l);
		ini.GetInteger("TrafficLight",	propertyName, 0, &aNewTL.pos.x);

		sprintf(propertyName, "TrafL%d_PosY", l);
		ini.GetInteger("TrafficLight",	propertyName, 0, &aNewTL.pos.y);

		sprintf(propertyName, "TrafL%d_Pic", l);
		ini.GetString("TrafficLight",	propertyName, 	"", aNewTL.imagePath,	COMMON_CLIENT_MSG_LEN_256);

		sprintf(propertyName, "TrafL%d_Cyc", l);
		ini.GetString("TrafficLight",	propertyName, 	"", tempText,			COMMON_CLIENT_MSG_LEN_32);
		if(strcmp(tempText, "Yes") == 0)
		{
			aNewTL.imageCyc = true;
		}
		else
		{
			aNewTL.imageCyc = false;
		}

		sprintf(propertyName, "TrafL%d_Tip", l);
		ini.GetString("TrafficLight",	propertyName,	"", aNewTL.tipText,	COMMON_CLIENT_MSG_LEN_256);

		sprintf(propertyName, "TrafL%d_TipPos", l);
		ini.GetString("TrafficLight",	propertyName, 	"", tempText,		COMMON_CLIENT_MSG_LEN_32);

        aNewTL.tipPos = KUiItemTip::textToTipPos(tempText);

		_trafficLightCfg.push_back(aNewTL);
	}

	//职业配置
	for (int series = 0; series < 3; series++)
	{
		for (int skillSeries = -1; skillSeries < 2; skillSeries++)
		{
			char professionProperty[COMMON_CLIENT_MSG_LEN_32] = { 0 };

			sprintf(professionProperty, "Name_%d_%d", series, skillSeries);
			ini.GetString("Profession", professionProperty, "", _professionCfg.m_Name[series][skillSeries + 1],			COMMON_CLIENT_MSG_LEN_32);
			
			sprintf(professionProperty, "Color_%d_%d", series, skillSeries);
			ini.GetString("Profession", professionProperty, "", _professionCfg.m_Color[series][skillSeries + 1],		COMMON_CLIENT_MSG_LEN_32);

			sprintf(professionProperty, "ImageSet_%d_%d", series, skillSeries);
			ini.GetString("Profession", professionProperty, "", _professionCfg.m_ImageSet[series][skillSeries + 1],		COMMON_CLIENT_MSG_LEN_32);

			sprintf(professionProperty, "ImageName_%d_%d", series, skillSeries);
			ini.GetString("Profession", professionProperty, "", _professionCfg.m_ImageName[series][skillSeries + 1],	COMMON_CLIENT_MSG_LEN_32);
		}
	}

	//交易
	ini.GetString("Trade",	"BeginTradeMsg",	"", _tradeCfg.beginTradeMsg,			COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("Trade",	"CancelTradeMsg",	"", _tradeCfg.cancelTradeMsg,			COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("Trade",	"CompleteTradeMsg",	"", _tradeCfg.completeTradeMsg,			COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("Trade",	"OppoDropMsg",		"", _tradeCfg.oppositeDropMsg,			COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("Trade",	"OppoPickMsg",		"", _tradeCfg.oppositePickMsg,			COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("Trade",	"OppoRefuseMsg",	"", _tradeCfg.oppositeRefuseMsg,		COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("Trade",	"TradeForbidMsg",	"", _tradeCfg.tradeForbiddenMsg,		COMMON_CLIENT_MSG_LEN_128);

	//音效
	ini.GetString("SoundEffect",	"RecvNewCoze",	"", _soundEffectCfg.recvNewCoze,	COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("SoundEffect",	"Repair",		"", _soundEffectCfg.repair,			COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("SoundEffect",	"CostMoney",	"", _soundEffectCfg.costMoney,		COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("SoundEffect",	"Pickup",		"", _soundEffectCfg.pickup,			COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("SoundEffect",	"Dropdown",		"", _soundEffectCfg.dropdown,		COMMON_CLIENT_MSG_LEN_64);

	//团队
	ini.GetString("Raid",	"XingtianHeadImg",		"", _raidCfg.xingtianHeadImg,		COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("Raid",	"XuanfengHeadImg",		"", _raidCfg.xuanfengHeadImg,		COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("Raid",	"TianshiHeadImg",		"", _raidCfg.tianshiHeadImg,		COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("Raid",	"ZhenrenHeadImg",		"", _raidCfg.zhenrenHeadImg,		COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("Raid",	"YishiHeadImg",			"", _raidCfg.yishiHeadImg,			COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("Raid",	"ShoushiHeadImg",		"", _raidCfg.shoushiHeadImg,		COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("Raid",	"NormalColor",			"", _raidCfg.normalColor,			COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Raid",	"HoverColor",			"", _raidCfg.hoverColor,			COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Raid",	"TopMemberNormalColor",	"", _raidCfg.topMemberNormalColor,	COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("Raid",	"TopMemberHoverColor",	"", _raidCfg.topMemberHoverColor,	COMMON_CLIENT_MSG_LEN_32);
	ini.GetInteger("Raid", "DragAlpha",			0,	&_raidCfg.dragAlpha);
	ini.GetString("Raid",	"ShowLife",				"", tempText,						COMMON_CLIENT_MSG_LEN_32);
	_raidCfg.showLife = strcmp("Yes", tempText) == 0 ? true: false;
	ini.GetInteger("Raid", "ColCount",				0,	&_raidCfg.colCount);
	if(_raidCfg.colCount <= 0)
	{
		_raidCfg.colCount = 2;
	}
	
	//快捷键
	ini.GetString("SKSetting",	"SuccessMsg",		"", _skCfg.successMsg,				COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("SKSetting",	"FailMsg",			"", _skCfg.failMsg,					COMMON_CLIENT_MSG_LEN_64);
	ini.GetString("SKSetting",	"KeyUnbindMsg",		"", _skCfg.keyUnbindMsg,			COMMON_CLIENT_MSG_LEN_64);
	
	//电影过场界面
	ini.GetString("MovieScene",	"TextColor",		"", _movieSceneCfg.textColor,		COMMON_CLIENT_MSG_LEN_32);
	ini.GetString("MovieScene",	"TextFont",			"", _movieSceneCfg.textFont,		COMMON_CLIENT_MSG_LEN_32);
    
	//城市Image
	for (int iCity=0;iCity<4;iCity++)
	{
		char szKey[64];
		sprintf(szKey,"CityMapID%d",iCity);
		ini.GetInteger("CityImage",szKey,0,&(_cityImageCfg.CityMapID[iCity]));
		
		sprintf(szKey,"ICon%d",iCity);
		ini.GetString("CityImage",szKey,0,_cityImageCfg.CityImage[iCity],sizeof(_cityImageCfg.CityImage[iCity]));
	}

	//城市资源Res
	for (int iCityRes=0;iCityRes<4;iCityRes++)
	{
		char szKey[64];
		sprintf(szKey,"CityRes%d",iCityRes);
		ini.GetInteger("CityRes",szKey,0,&(_cityResCfg.CityMapID[iCityRes]));
		
		sprintf(szKey,"CityResB%d",iCityRes);
		ini.GetInteger("CityRes",szKey,0,&(_cityResCfg.CityResB[iCityRes]));

		sprintf(szKey,"CityResF%d",iCityRes);
		ini.GetInteger("CityRes",szKey,0,&(_cityResCfg.CityResF[iCityRes]));

		sprintf(szKey,"CityResW%d",iCityRes);
		ini.GetInteger("CityRes",szKey,0,&(_cityResCfg.CityResW[iCityRes]));
		
		sprintf(szKey,"CityDesc%d",iCityRes);
	    ini.GetString("CityRes",szKey,"",_cityResCfg.CityDesc[iCityRes],sizeof(_cityResCfg.CityDesc[iCityRes]));
	}
	int iUseRes = 0;
	ini.GetInteger( "CityRes", "UseRes", 0, &iUseRes);
	_cityResCfg.UseRes = ( iUseRes != 0 );

	ini.GetString("CityImage","IConSet","",_cityImageCfg.CityIConSet,sizeof(_cityImageCfg.CityIConSet));
	//氏族建立提示界面
	ini.GetString("TongCondition","ShizuCond","",_tongConditionCfg.szShizuCond,sizeof(_tongConditionCfg.szShizuCond));
	ini.GetString("TongCondition","ZhuhouCond","",_tongConditionCfg.szZhuhouCond,sizeof(_tongConditionCfg.szZhuhouCond));


	//法宝
	ini.GetString("Talisman",	"ActiveHoleImageset",	"",	_talismanCfg.activeHoleIms,		sizeof(_talismanCfg.activeHoleIms));
	ini.GetString("Talisman",	"ActiveHoleImage",		"",	_talismanCfg.activeHoleImage,	sizeof(_talismanCfg.activeHoleImage));
	ini.GetString("Talisman",	"DeactiveHoleImageset",	"",	_talismanCfg.deactiveHoleIms,	sizeof(_talismanCfg.deactiveHoleIms));
	ini.GetString("Talisman",	"DeactiveHoleImage",	"",	_talismanCfg.deactiveHoleImage,	sizeof(_talismanCfg.deactiveHoleImage));

	//耐久提示
	ini.GetString("DuraAlert",	"TipLayout",		"",	_duraAlertCfg.tipLayout,		sizeof(_duraAlertCfg.tipLayout));
	ini.GetString("DuraAlert",	"RedTipSeg",		"",	_duraAlertCfg.redTipSeg,		sizeof(_duraAlertCfg.redTipSeg));
	ini.GetString("DuraAlert",	"YellowTipSeg",		"",	_duraAlertCfg.yellowTipSeg,		sizeof(_duraAlertCfg.yellowTipSeg));
	ini.GetString("DuraAlert",	"NormalTipSeg",		"",	_duraAlertCfg.normalTipSeg,		sizeof(_duraAlertCfg.normalTipSeg));
	ini.GetString("DuraAlert",	"ShowNormal",		"",	tempText,		sizeof(tempText));
	if(!strcmp(tempText, "Yes"))
	{
		_duraAlertCfg.showNormal = true;
	}
	else
	{
		_duraAlertCfg.showNormal = false;
	}

	//氏族国家小图标
	ini.GetString("TongSmallImage" , "shizu_online" , "",_SmallTongImage.GensImagePathOnline, sizeof(_SmallTongImage.GensImagePathOnline) );
    ini.GetString("TongSmallImage" , "shizu_offline" , "",_SmallTongImage.GensImagePathOffline, sizeof(_SmallTongImage.GensImagePathOffline) );
    ini.GetString("TongSmallImage" , "zhuhou_online" , "",_SmallTongImage.TongImagePathOnline, sizeof(_SmallTongImage.TongImagePathOnline) );
    ini.GetString("TongSmallImage" , "zhuhou_offline" , "",_SmallTongImage.TongImagePathOffline, sizeof(_SmallTongImage.TongImagePathOffline) );
    ini.GetString("TongSmallImage" , "jinyan_online" , "",_SmallTongImage.ForbidChatImagePathOnline, sizeof(_SmallTongImage.ForbidChatImagePathOnline) );
    ini.GetString("TongSmallImage" , "jinyan_offline" , "",_SmallTongImage.ForbidChatPathOffline, sizeof(_SmallTongImage.ForbidChatPathOffline) );
   	
	//装备
	//地挂激活特效
	ini.GetString("Equipment" , "Gua_DiHeng" ,		"", _equipmentCfg.guaEffect[GT_Di][GED_Heng],		sizeof(_equipmentCfg.guaEffect[GT_Di][GED_Heng]));
	ini.GetString("Equipment" , "Gua_DiShu" ,		"", _equipmentCfg.guaEffect[GT_Di][GED_Shu],		sizeof(_equipmentCfg.guaEffect[GT_Di][GED_Shu]));
	ini.GetString("Equipment" , "Gua_DiZuoxie" ,	"", _equipmentCfg.guaEffect[GT_Di][GED_Zuoxie],		sizeof(_equipmentCfg.guaEffect[GT_Di][GED_Zuoxie]));
	ini.GetString("Equipment" , "Gua_DiYouxie" ,	"", _equipmentCfg.guaEffect[GT_Di][GED_Youxie],		sizeof(_equipmentCfg.guaEffect[GT_Di][GED_Youxie]));
	//风挂激活特效
	ini.GetString("Equipment" , "Gua_FengHeng" ,	"", _equipmentCfg.guaEffect[GT_Feng][GED_Heng],		sizeof(_equipmentCfg.guaEffect[GT_Feng][GED_Heng]));
	ini.GetString("Equipment" , "Gua_FengShu" ,		"", _equipmentCfg.guaEffect[GT_Feng][GED_Shu],		sizeof(_equipmentCfg.guaEffect[GT_Feng][GED_Shu]));
	ini.GetString("Equipment" , "Gua_FengZuoxie" ,	"", _equipmentCfg.guaEffect[GT_Feng][GED_Zuoxie],	sizeof(_equipmentCfg.guaEffect[GT_Feng][GED_Zuoxie]));
	ini.GetString("Equipment" , "Gua_FengYouxie" ,	"", _equipmentCfg.guaEffect[GT_Feng][GED_Youxie],	sizeof(_equipmentCfg.guaEffect[GT_Feng][GED_Youxie]));

	//火挂激活特效
	ini.GetString("Equipment" , "Gua_HuoHeng" ,		"", _equipmentCfg.guaEffect[GT_Huo][GED_Heng],		sizeof(_equipmentCfg.guaEffect[GT_Huo][GED_Heng]));
	ini.GetString("Equipment" , "Gua_HuoShu" ,		"", _equipmentCfg.guaEffect[GT_Huo][GED_Shu],		sizeof(_equipmentCfg.guaEffect[GT_Huo][GED_Shu]));
	ini.GetString("Equipment" , "Gua_HuoZuoxie" ,	"", _equipmentCfg.guaEffect[GT_Huo][GED_Zuoxie],	sizeof(_equipmentCfg.guaEffect[GT_Huo][GED_Zuoxie]));
	ini.GetString("Equipment" , "Gua_HuoYouxie" ,	"", _equipmentCfg.guaEffect[GT_Huo][GED_Youxie],	sizeof(_equipmentCfg.guaEffect[GT_Huo][GED_Youxie]));

	//雷挂激活特效
	ini.GetString("Equipment" , "Gua_LeiHeng" ,		"", _equipmentCfg.guaEffect[GT_Lei][GED_Heng],		sizeof(_equipmentCfg.guaEffect[GT_Lei][GED_Heng]));
	ini.GetString("Equipment" , "Gua_LeiShu" ,		"", _equipmentCfg.guaEffect[GT_Lei][GED_Shu],		sizeof(_equipmentCfg.guaEffect[GT_Lei][GED_Shu]));
	ini.GetString("Equipment" , "Gua_LeiZuoxie" ,	"", _equipmentCfg.guaEffect[GT_Lei][GED_Zuoxie],	sizeof(_equipmentCfg.guaEffect[GT_Lei][GED_Zuoxie]));
	ini.GetString("Equipment" , "Gua_LeiYouxie" ,	"", _equipmentCfg.guaEffect[GT_Lei][GED_Youxie],	sizeof(_equipmentCfg.guaEffect[GT_Lei][GED_Youxie]));

	//山挂激活特效
	ini.GetString("Equipment" , "Gua_ShanHeng" ,	"", _equipmentCfg.guaEffect[GT_Shan][GED_Heng],		sizeof(_equipmentCfg.guaEffect[GT_Shan][GED_Heng]));
	ini.GetString("Equipment" , "Gua_ShanShu" ,		"", _equipmentCfg.guaEffect[GT_Shan][GED_Shu],		sizeof(_equipmentCfg.guaEffect[GT_Shan][GED_Shu]));
	ini.GetString("Equipment" , "Gua_ShanZuoxie" ,	"", _equipmentCfg.guaEffect[GT_Shan][GED_Zuoxie],	sizeof(_equipmentCfg.guaEffect[GT_Shan][GED_Zuoxie]));
	ini.GetString("Equipment" , "Gua_ShanYouxie" ,	"", _equipmentCfg.guaEffect[GT_Shan][GED_Youxie],	sizeof(_equipmentCfg.guaEffect[GT_Shan][GED_Youxie]));

	//水挂激活特效
	ini.GetString("Equipment" , "Gua_ShuiHeng" ,	"", _equipmentCfg.guaEffect[GT_Shui][GED_Heng],		sizeof(_equipmentCfg.guaEffect[GT_Shui][GED_Heng]));
	ini.GetString("Equipment" , "Gua_ShuiShu" ,		"", _equipmentCfg.guaEffect[GT_Shui][GED_Shu],		sizeof(_equipmentCfg.guaEffect[GT_Shui][GED_Shu]));
	ini.GetString("Equipment" , "Gua_ShuiZuoxie" ,	"", _equipmentCfg.guaEffect[GT_Shui][GED_Zuoxie],	sizeof(_equipmentCfg.guaEffect[GT_Shui][GED_Zuoxie]));
	ini.GetString("Equipment" , "Gua_ShuiYouxie" ,	"", _equipmentCfg.guaEffect[GT_Shui][GED_Youxie],	sizeof(_equipmentCfg.guaEffect[GT_Shui][GED_Youxie]));

	//天挂激活特效
	ini.GetString("Equipment" , "Gua_TianHeng" ,	"", _equipmentCfg.guaEffect[GT_Tian][GED_Heng],		sizeof(_equipmentCfg.guaEffect[GT_Tian][GED_Heng]));
	ini.GetString("Equipment" , "Gua_TianShu" ,		"", _equipmentCfg.guaEffect[GT_Tian][GED_Shu],		sizeof(_equipmentCfg.guaEffect[GT_Tian][GED_Shu]));
	ini.GetString("Equipment" , "Gua_TianZuoxie" ,	"", _equipmentCfg.guaEffect[GT_Tian][GED_Zuoxie],	sizeof(_equipmentCfg.guaEffect[GT_Tian][GED_Zuoxie]));
	ini.GetString("Equipment" , "Gua_TianYouxie" ,	"", _equipmentCfg.guaEffect[GT_Tian][GED_Youxie],	sizeof(_equipmentCfg.guaEffect[GT_Tian][GED_Youxie]));

	//泽挂激活特效
	ini.GetString("Equipment" , "Gua_ZeHeng" ,		"", _equipmentCfg.guaEffect[GT_Ze][GED_Heng],		sizeof(_equipmentCfg.guaEffect[GT_Ze][GED_Heng]));
	ini.GetString("Equipment" , "Gua_ZeShu" ,		"", _equipmentCfg.guaEffect[GT_Ze][GED_Shu],		sizeof(_equipmentCfg.guaEffect[GT_Ze][GED_Shu]));
	ini.GetString("Equipment" , "Gua_ZeZuoxie" ,	"", _equipmentCfg.guaEffect[GT_Ze][GED_Zuoxie],		sizeof(_equipmentCfg.guaEffect[GT_Ze][GED_Zuoxie]));
	ini.GetString("Equipment" , "Gua_ZeYouxie" ,	"", _equipmentCfg.guaEffect[GT_Ze][GED_Youxie],		sizeof(_equipmentCfg.guaEffect[GT_Ze][GED_Youxie]));

	for(int guaIndex = 0; guaIndex < gua_pos_count; ++guaIndex)
	{
		sprintf(propertyName, "Gua%d", guaIndex + 1);
		ini.GetString("Equipment" , propertyName ,		"", tempText,		sizeof(tempText));
		sscanf(tempText, "%d,%d", &_equipmentCfg.pos[guaIndex].first, &_equipmentCfg.pos[guaIndex].second);
	}

	ini.GetString( "Equipment" , "NextLevelColor",	"", _equipmentCfg.nextLevelColor,	sizeof( _equipmentCfg.nextLevelColor	) );
	ini.GetString( "Equipment" , "NotGrantColor",	"", _equipmentCfg.notGrantColor,	sizeof( _equipmentCfg.notGrantColor		) );
	ini.GetString( "Equipment" , "GrantColor",		"", _equipmentCfg.grantColor,		sizeof( _equipmentCfg.grantColor		) );

	//宠物头像
	ini.GetString("PetFrames" , "SelfPetExtendName" ,		"", _petCfg.selfPetExtendName,		sizeof(_petCfg.selfPetExtendName));
	ini.GetString("PetFrames" , "SeflPetDefaultHeadImage" ,	"", _petCfg.selfPetHeadImage,	sizeof(_petCfg.selfPetHeadImage));
	ini.GetString("PetFrames" , "SelfPetBloodSize" ,		"", tempText,		sizeof(tempText));
	sscanf(tempText	  ,	"%d,%d", &_petCfg.selfPetBloodSize.first, &_petCfg.selfPetBloodSize.second);

	ini.GetString("PetFrames" , "SelfPetPos" ,	"", tempText,		sizeof(tempText));
	sscanf(tempText	  ,	"%d,%d", &_petCfg.selfPetPos.first, &_petCfg.selfPetPos.second);

	//氏族banner
// 	ini.GetString("ShizuBanner" , "Size" ,	"", tempText,		sizeof(tempText));
// 	_ASSERT(sscanf(tempText		, "%d,%d",	&_shizuBannerCfg.size.first, &_shizuBannerCfg.size.second) == 2);
// 	ini.GetString("ShizuBanner" , "Pos",	"", tempText,		sizeof(tempText));
// 	_ASSERT(sscanf(tempText		, "%d,%d",	&_shizuBannerCfg.pos.first, &_shizuBannerCfg.pos.second) == 2);

	//拍卖行配置数据
	ini.GetInteger("Auction", "TimeVeryLong",		2880,	&_auctionCfg.timeVeryLong);	
	ini.GetInteger("Auction", "TimeLong",			1440,	&_auctionCfg.timeLong);
	ini.GetInteger("Auction", "TimeMiddle",			240,	&_auctionCfg.timeMiddle);
	ini.GetInteger("Auction", "TimeShort",			60,		&_auctionCfg.timeShort);
	ini.GetInteger("Auction", "TimeVeryShort",		10,		&_auctionCfg.timeVeryShort);
	for (int nKind = 0; nKind < enAuction_Kind_Count; ++nKind)
	{
		char	typeName[COMMON_CLIENT_MSG_LEN_16];
		ZeroMemory(typeName, COMMON_CLIENT_MSG_LEN_16);
		sprintf(typeName, "Type%d", nKind);
		ini.GetString("Auction", typeName, "-1,-1,-1,-1", _auctionCfg.type[nKind], sizeof(_auctionCfg.type[nKind]));
	}

	_ascIIFontName.vecFontName.clear();
	ini.GetInteger("DefaultAscIIFontName", "FontCount", 0, &_ascIIFontName.count);
	for (int fontNum = 0; fontNum < _ascIIFontName.count; fontNum++)
	{
		char keyName[COMMON_CLIENT_MSG_LEN_16];
		char tempFontName[COMMON_CLIENT_MSG_LEN_32];
		sprintf(keyName, "Font%d", fontNum);
		ini.GetString("DefaultAscIIFontName", keyName, "",	tempFontName, sizeof(tempFontName));
		_ascIIFontName.vecFontName.push_back(tempFontName);
	}
	
	//滚动信息
	int fontCount;
	ini.GetInteger("TopMessage", "FontCount",		0,	&fontCount);
	for(int fontIndex = 0; fontIndex < fontCount; ++fontIndex)
	{
		char fontText[COMMON_CLIENT_MSG_LEN_64];
		char fontIndexText[COMMON_CLIENT_MSG_LEN_64];
		sprintf(fontIndexText, "Font_%d", fontIndex + 1);
			
		ini.GetString("TopMessage", fontIndexText, "", fontText, sizeof(fontText));
		_topMsgCfg.fonts.push_back(fontText);
	}
	
	int msgCount;
	ini.GetInteger("TopMessage", "MsgCount",		0,	&msgCount);
	for(int msgIndex = 0; msgIndex < msgCount; ++msgIndex)
	{
		char msgText[COMMON_CLIENT_MSG_LEN_1024];
		char msgIndexText[COMMON_CLIENT_MSG_LEN_64];
		sprintf(msgIndexText, "Msg_%d", msgIndex + 1);
			
		ini.GetString("TopMessage", msgIndexText, "", msgText, sizeof(msgText));
		_topMsgCfg.msgs.push_back(msgText);
	}

	//小精灵配置信息
	ini.GetInteger("Elf", "ElfChangeInterval", 10000, (int *)&_elfCfg.elfChangeInterval);
	ini.GetInteger("Elf", "RandomHelpInterval", 10000, (int *)&_elfCfg.randomHelpInterval);

	//IB商店的配置
	ini.GetInteger("IBShop", "JinshanbiRate", 1, reinterpret_cast<int*>(&_ibshopCfg.jinshanbiRate));
	ini.GetInteger("IBShop", "CreditRate", 1, reinterpret_cast<int*>(&_ibshopCfg.creditRate));
	ini.GetInteger("IBShop", "IsUseIE", 1, reinterpret_cast<int *>(&_ibshopCfg.isUseIE));

	//推荐系统相关
	ini.GetString("Recommend" , "StudentReportText" ,	"", _recommendCfg.studentReportText,	sizeof(_recommendCfg.studentReportText));
	ini.GetString("Recommend" , "OkText" ,				"", _recommendCfg.okText,				sizeof(_recommendCfg.okText));
	ini.GetString("Recommend" , "CancelText" ,			"", _recommendCfg.cancelText,			sizeof(_recommendCfg.cancelText));
	
	//ServerList的界面配置
	ini.GetInteger( "ServerList",	"RecentFileVersion",	0,	&_serverListCfg.RecentFileVersion );
	ini.GetInteger( "ServerList",	"MaxRecentServerCount",	5,	&_serverListCfg.MaxRecentServerCount );
	
	//邮件相关配置
	ini.GetInteger("Mail", "MailNpcIndex_Chaoge", 0, &_mailCfg.MailNpcIndex_Chaoge);
	ini.GetInteger("Mail", "MailNpcIndex_Beihai", 0, &_mailCfg.MailNpcIndex_Beihai);
	ini.GetInteger("Mail", "MailNpcIndex_Jiuli", 0, &_mailCfg.MailNpcIndex_Jiuli);
	ini.GetInteger("Mail", "MailNpcIndex_Kunlun", 0, &_mailCfg.MailNpcIndex_Kunlun);

	ini.GetInteger("Mail", "Red_MailCount", 180, &_mailCfg.Red_MailCount);
	ini.GetInteger("Mail", "Max_MailCount", 200, &_mailCfg.Max_MailCount);
	ini.GetInteger("Mail", "Green_Day", 15, &_mailCfg.Green_Day);
	ini.GetInteger("Mail", "Red_Day", 5, &_mailCfg.Red_Day);

	//信息条相关配置
	ini.GetInteger( "InfoBar", "LagStateSpace", 400, &_infoBarCfg.LagStateSpace );
	ini.GetInteger( "InfoBar", "TimeFlipInterval", 2000, &_infoBarCfg.TimeFlipInterval );

	//提问窗口相关配置
	ini.GetInteger( "QuestionWindow", "TotalTime", 30, &_questionWindowCfg.TotalTime );

	char szQuestionFilePath[COMMON_CLIENT_MSG_LEN_32];
	ini.GetString( "QuestionWindow", "QuestionFilePath", "", szQuestionFilePath, sizeof( szQuestionFilePath ) );
	_questionWindowCfg.QuestionFilePath = szQuestionFilePath;

	ini.GetInteger( "QuestionWindow", "DisableInput", 0, &_questionWindowCfg.DisableInput );
	ini.GetInteger( "PalyTime",	"num", 1000, &_playTime);
	ini.GetInteger( "PalyTime", "breathTrequency", 5, &_breathTrequency );

	int toggle = 0;
	ini.GetInteger( "PalyTime", "open", 0, &toggle );
	if( toggle == 0)
		_isOpenAnimationForSkill = false;
	else if ( toggle == 1)
		_isOpenAnimationForSkill = true;
	//实力排行榜配置
	int itemNum = 0;
	ini.GetInteger( "StrengeRanking", "ItemNum", 0, &itemNum );
	if( itemNum )
	{
		
		char num[5];
		//遍历所有的功能排行项
		for ( int ii = 0; ii < itemNum; ++ii)
		{
			RankingCfg temp;
			ZeroMemory( num, 5 );
			int titleNum = 0;
			itoa( ii + 1, num, 10 );
			string path( "StrengeRanking" );
			path += num;
			//获得功能排行需要的title数目
			char charBuffer[256];
			ini.GetInteger( path.c_str(), "ItemTitleNum", 0, &titleNum );
			temp.titleNum = titleNum;
			
			
			//获得功能排行的名字
			ZeroMemory( charBuffer, 256 );
			ini.GetString( path.c_str(), "ItemName", "", charBuffer, 256 );
			temp.itemName=( charBuffer );
			
			//获得功能排行参数
			
			int param = 0;
			ini.GetInteger( path.c_str(), "Param", 0, &param );
			temp.param = param;
			char ctitleNum[5];
			//对于每个功能排行项遍历出所有的titleName
			for (int t = 0; t < titleNum; ++t )
			{
				ZeroMemory( ctitleNum, 5 );
				itoa( t+1, ctitleNum, 10 );
				string titlePath("TitleName");
				titlePath += ctitleNum;
				ZeroMemory( charBuffer, 256 );
				ini.GetString( path.c_str(), titlePath.c_str(), "", charBuffer, 256 );
				temp.titleName.push_back( string( ( charBuffer ) ) );			
				
			}
			_strengeItemContainer.push_back( temp );
			
		}
	}

	//功能排行榜配置
	itemNum = 0;
	ini.GetInteger( "FunctionRanking", "ItemNum", 0, &itemNum );
	if( itemNum )
	{
		char num[5];
		//遍历所有的功能排行项
		for ( int ii = 0; ii < itemNum; ++ii)
		{
			RankingCfg temp;
			ZeroMemory( num, 5 );
			int titleNum = 0;
			itoa( ii+1, num, 10 );
			string path( "FunctionRanking" );
			path += num;
			//获得功能排行需要的title数目
			char charBuffer[256];
			ini.GetInteger( path.c_str(), "ItemTitleNum", 0, &titleNum );
			temp.titleNum = titleNum;
			//获得功能排行的名字
			ZeroMemory( charBuffer, 256 );
			ini.GetString( path.c_str(), "ItemName", "", charBuffer, 256 );
			temp.itemName = ( charBuffer );
			//获得功能排行参数
			int param = 0;
			ini.GetInteger( path.c_str(), "Param", 0, &param );
			temp.param = param;
			char ctitleNum[5];
			//对于每个功能排行项遍历出所有的titleName
			for ( int t = 0; t < titleNum; ++t )
			{
				ZeroMemory( ctitleNum, 5 );
				itoa( t+1, ctitleNum, 10 );
				string titlePath( "TitleName" );
				titlePath += ctitleNum;
				ZeroMemory( charBuffer, 256 );
				ini.GetString( path.c_str(), titlePath.c_str(), "", charBuffer, 256 );
				temp.titleName.push_back( string( ( charBuffer ) ) );			
			}
			_functionItemContainer.push_back(temp);
		}
	}
	char colorBuf[20];
	ZeroMemory( colorBuf, sizeof( colorBuf ) );
	ini.GetString( "TextColor", "Color", "", colorBuf, sizeof( colorBuf ) );
	float red	= 0;
	float green = 0;
	float blue	= 0;
	if( 0 != strcmp( colorBuf, "") )
	{
		sscanf(colorBuf, "%f,%f,%f", &red, &green, &blue);
		red		/= 255;
		green	/= 255;
		blue	/= 255;	
	}	
	_RankingTextColor.set( red, green, blue);
	ini.GetInteger( "PalyTime", "uncheckSkill", 0, &_uncheckSkill );
}

char* KUiCfgLoader::getJinImagePath()
{
	return _commData.jinImage;
}

char* KUiCfgLoader::getYinImagePath()
{
	return _commData.yinImage;
}

char* KUiCfgLoader::getTongImagePath()
{
	return _commData.tongImage;
}

char* KUiCfgLoader::getYunhunText()
{
	return _smithData.yunhunText;
}

char* KUiCfgLoader::getMoneyText()
{
	return _smithData.moneyText;
}

char* KUiCfgLoader::getYaoRateText()
{
	return _smithData.yaoRateText;
}

char* KUiCfgLoader::getNormalRateText()
{
	return _smithData.normalRateText;
}


int	KUiCfgLoader::getManPortraitCount( void )
{
	return _manCount;	
}

int	KUiCfgLoader::getWomanPortraitCount( void )
{
	return _womanCount;
}


void	KUiCfgLoader::getManPortraitPath( int nIdx, std::string& imageset, std::string& image )
{
	if ( nIdx >= 0 && nIdx <  _portraitManData.faceList.size() )
	{
		char szImageSet[128];
		char szImage[128];
		sscanf( _portraitManData.faceList[nIdx].image, "set:%s image:%s", szImageSet, szImage );
		imageset = szImageSet;
		image = szImage;
	}
	
}

void	KUiCfgLoader::getWomanPortraitPath( int nIdx, std::string& imageset, std::string& image )
{	
	if ( nIdx >= 0 && nIdx <  _portraitWomanData.faceList.size() )
	{
		char szImageSet[128];
		char szImage[128];
		sscanf( _portraitWomanData.faceList[nIdx].image, "set:%s image:%s", szImageSet, szImage );
		imageset = szImageSet;
		image = szImage;
	}
}

void	KUiCfgLoader::getMidManPortraitPath( int nIdx, std::string& imageset, std::string& image )
{
	if ( nIdx >= 0 && nIdx <  _portraitManData.faceList.size() )
	{
		char szImageSet[128];
		char szImage[128];
		sscanf( _portraitMidManData.faceList[nIdx].image, "set:%s image:%s", szImageSet, szImage );
		imageset = szImageSet;
		image = szImage;
	}
	
}

void	KUiCfgLoader::getMidWomanPortraitPath( int nIdx, std::string& imageset, std::string& image )
{	
	if ( nIdx >= 0 && nIdx <  _portraitWomanData.faceList.size() )
	{
		char szImageSet[128];
		char szImage[128];
		sscanf( _portraitMidWomanData.faceList[nIdx].image, "set:%s image:%s", szImageSet, szImage );
		imageset = szImageSet;
		image = szImage;
	}
}
void	KUiCfgLoader::getMinManPortraitPath( int nIdx, std::string& imageset, std::string& image )
{
	if ( nIdx >= 0 && nIdx <  _portraitManData.faceList.size() )
	{
		char szImageSet[128];
		char szImage[128];
		sscanf( _portraitMinManData.faceList[nIdx].image, "set:%s image:%s", szImageSet, szImage );
		imageset = szImageSet;
		image = szImage;
	}
	
}

void	KUiCfgLoader::getMinWomanPortraitPath( int nIdx, std::string& imageset, std::string& image )
{	
	if ( nIdx >= 0 && nIdx <  _portraitWomanData.faceList.size() )
	{
		char szImageSet[128];
		char szImage[128];
		sscanf( _portraitMinWomanData.faceList[nIdx].image, "set:%s image:%s", szImageSet, szImage );
		imageset = szImageSet;
		image = szImage;
	}
}

void KUiCfgLoader::save()
{
	KIniFile channelFrameIni;
	if(channelFrameIni.Load(CONFIG_INI) == false)
	{
		return;
	}

	channelFrameIni.EraseSection("Channel");
	
	char tempText[COMMON_CLIENT_MSG_LEN_64];
	int frameCount = _chanData.framesCfg.size();

	for(int i = 0; i < frameCount; ++i)
	{
		if(i != 5)
		{
			continue;
		}
		const ChatFrameCfg& frameCfg = _chanData.framesCfg[i];

		const string frameName = frameCfg.frameName;
		sprintf(tempText, "Frame%dName", i);
		channelFrameIni.WriteString("Channel", tempText, frameName.c_str());

		for(int j = 0; j < frameCfg.channelName.size(); ++j)
		{
			const string channelName = frameCfg.channelName[j];
			sprintf(tempText, "Frame%dChannel%d", i, j);
			channelFrameIni.WriteString("Channel", tempText, channelName.c_str());
		}
	}
	channelFrameIni.Save(CONFIG_INI);
}