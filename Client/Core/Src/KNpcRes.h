
#pragma once

#ifndef _SERVER

#include "KWavSound.h"
#include "KList.h"
#include "KNpcResNode.h"
#include "KNpcResList.h"
#include "KSprControl.h"
#include "KRepresentUnit.h"
#include "UiImage.h"
#include "iRepresentshell.h"

extern iRepresentShell * g_pRepresentShell;

class KCacheNode;

class KStateSpr
{

public:
	int				m_nID;						// 在表格文件中的位置（从 1 开始，0为空）
	int				m_nType;					// 类型：头顶、脚底、身上
	int				m_nPlayType;				// 播放类型
	int				m_nBackStart;				// 身上类型 背后开始帧
	int				m_nBackEnd;					// 身上类型 背后结束帧
	int				m_nCopyNum;					// 图分几瓣
	KSprControl		m_SprContrul;				// spr 控制
public:
	KStateSpr();
	void			Release();
};

#define		MAX_BLUR_FRAME		7
#define		START_BLUR_ALPHA	128
#define		BLUR_ALPHA_CHANGE	16
#define		MAX_INLAY_STATE_PART_NUM 4 

class KNpcBlur
{
public:
	int				m_nActive;							// 当前残影处理是否处于激活状态
	int				m_nCurNo;							// 当前帧指针
	DWORD			m_dwTimer;							// 时间计数器
	DWORD			m_dwInterval;						// 多少帧取一次残影
	int				m_nMapXpos[MAX_BLUR_FRAME];			// 对应的地图坐标 x
	int				m_nMapYpos[MAX_BLUR_FRAME];			// 对应的地图坐标 y
	int				m_nMapZpos[MAX_BLUR_FRAME];			// 对应的地图坐标 z
	unsigned int	m_SceneIDNpcIdx[MAX_BLUR_FRAME];
	unsigned int	m_SceneID[MAX_BLUR_FRAME];			// 
	KRUImage		m_Blur[MAX_BLUR_FRAME][MAX_PART];	// 残影绘制列表
public:
	KNpcBlur();
	~KNpcBlur();
	BOOL			Init();
	void			Remove();
	void			SetNextNo();
	void			SetMapPos(int x, int y, int z, int nNpcIdx);
	void			ChangeAlpha();
	void			ClearCurNo();
	void			SetFile(int nNo, char *lpszFileName, int nSprID, int nFrameNo, unsigned int uPal, int nXpos, int nYpos, int nZpos);
	void			Draw(int nIdx);
	BOOL			NowGetBlur();
	void			AddObj();
	void			RemoveObj();
};

class KNpcRes
{
	friend class KNpc;
	enum
	{
		SHADOW_BEGIN	= 0,
		STATE_BEGIN		= 1,
		PART_BEGIN		= 1 + 6,
		SPEC_BEGIN		= 1 + MAX_PART + 6,
		MENUSTATE_BEGIN = 1 + MAX_PART + 6 + 1,
	};
	
public:
	int				m_nNpcResPart;
	unsigned int	m_SceneID;							// 在场景中的ID
	unsigned int	m_uRoleType;						// 角色类型
	int				m_nXpos;							// 坐标 x
	int				m_nYpos;							// 坐标 y
	int				m_nZpos;							// 坐标 z
	int				m_nXposNew;							// 坐标 x
	int				m_nYposNew;							// 坐标 y
	int				m_nZposNew;							// 坐标 z
	int				m_nXposOld;							// 坐标 x
	int				m_nYposOld;							// 坐标 y
	int				m_nZposOld;							// 坐标 z
private:
	bool            changeRes;                          //NPC资源是不是改变了
	int				m_nDoing;							// Npc的动作
	int				m_nAction;							// Npc的实际动作（与武器、骑马有关）
	int				m_nNpcKind;							// 特殊 普通
	unsigned int 	m_SceneID_NPCIdx;                   // 在场景中的ID 对应的NPCidx
	int				m_nPart[BODY_PART_MAX];
	int				m_nPartPal[BODY_PART_MAX];			//装备偏色
	BOOL			m_bRideHorse;						// 当前是否骑马
	int				m_nBlurState;
	char			m_szSoundName[80];					// 当前音效文件名
	KCacheNode		*m_pSoundNode;						// 声效指针
	KWavSound		*m_pWave;							// 声效wav指针
public:
	enum
	{
		adjustcolor_physics = 0,		// 物理伤害
		adjustcolor_poison, 
		adjustcolor_freeze,			// 火焰伤害
		adjustcolor_burn,			// 冰冻伤害
		adjustcolor_confuse,		// 闪电伤害
		adjustcolor_stun,			// 毒素伤害
	};
	KSprControl		m_cNpcImage[MAX_PART];				// 所有动作的所有spr文件名
	bool            m_cNpcChanged[MAX_PART];
	int             m_nLastFrame;
	int             m_nLastDir;
    bool            m_bNeedSort;
	KSprControl		m_cNpcShadow;						// npc阴影
	KStateSpr		m_cStateSpr[MAX_STATE_PART_NUM];	// 状态特效，0 1 为头顶 2 3 为脚底 4 5 为身上 6 为套装特效
	KStateSpr		m_cInlayStateSpr[MAX_INLAY_STATE_PART_NUM];	// 镶嵌人物特效
	int				m_cStateID[MAX_STATE_PART_NUM];
	KSprControl		m_cSpecialSpr;						// 特殊的只播放一遍的随身spr文件
	unsigned int	m_ulAdjustColorId;
	
	
	KSprControl		m_cMenuStateSpr;
	
	int				m_nMenuState;
	int				m_nBackMenuState;
	int				m_nSleepState;
	char			m_szSentence[MAX_SENTENCE_LENGTH];
	//char			m_szBackSentence[MAX_SENTENCE_LENGTH];
	
	int				m_nSortTable[MAX_PART];				// 排序表

	KRUImage		m_cDrawFile[MAX_NPC_IMAGE_NUM];// 绘制列表 身体部件 + 阴影 + 魔法状态 + 特殊动画 + 头顶状态
	KRUImage		m_cInlayDrawFile[MAX_NPC_IMAGE_NUM];
	KRUImage		m_cShadowFile;// 绘制列表 身体部件 + 阴影 + 魔法状态 + 特殊动画 + 头顶状态
	KRUImage		m_cFootFile[2];// 绘制列表 身体部件 + 阴影 + 魔法状态 + 特殊动画 + 头顶状态
	int				m_nFootNum;

	KRUImage		m_cBodyFile[2];// 绘制列表 身体部件 + 阴影 + 魔法状态 + 特殊动画 + 头顶状态
	int				m_nBodyFrontNum;
	int				m_nBodyBackNum;

	KRUImage		m_cHeadFile[2];// 绘制列表 身体部件 + 阴影 + 魔法状态 + 特殊动画 + 头顶状态
	int				m_nHeadNum;

	KRUImage		m_cOnlyFile;

	KNpcBlur		m_cNpcBlur;							// npc 残影

	const KNpcResNode *m_pcResNode;						// npc 资源
	//Lucifer~yu(zhangjianyu) 07/28/2007 Modify 血条
	//Begin-------------------------------------------------------------------
	static BOOL	InitBloodTemplate();
	enum{ MAX_NPC_LIFE_BAR_TYPE = 16 };
	enum{ MAX_QUEST_ICON = 6 };
	static KUiImagePartRef		m_imgNpcBkgnd[MAX_NPC_LIFE_BAR_TYPE];
	static KUiImagePartRef		m_imgNpcLifeBar[MAX_NPC_LIFE_BAR_TYPE];
	static int					m_bNpcLeftBarTyp[MAX_NPC_LIFE_BAR_TYPE];
	static KUiImagePartRef		m_imgNumber;
	static KUiImagePartRef		m_imgExceed;
	static POINT				m_ptNpcLifeBarPos[MAX_NPC_LIFE_BAR_TYPE];
	static POINT				m_ptNpcLevelTextStartPos[MAX_NPC_LIFE_BAR_TYPE];
	static SIZE					m_sizeNpcLevelTxtRect[MAX_NPC_LIFE_BAR_TYPE];
	//End---------------------------------------------------------------------


	static KUiImageRef			selImageEnemy;
	static KUiImageRef			selImageNormal;
	static KUiImageRef			QuestImage[MAX_QUEST_ICON];
	
	static KUiImageRef			m_HoverImage;
	static KUiImageRef			m_SelectImage;
	static KUiImageRef			m_RideHoverImage;
	static KUiImageRef			m_RideSelectImage;
	static KUiImageRef			m_BloodImage[8];

	void GetSpr(std::string& spr );
	
private:
	// 由一个图像资源文件名得到他的阴影图像文件名
	void			GetSoundName();						// 获得当前动作的音效文件名
	void			PlaySound(int nX, int nY);			// 播放当前动作的音效
	void			SetMenuStateSpr(int nMenuState);					// set menu state spr
	void			UpDateUiImage(void);
	void			UpDateImage(void);
	
public:
	KNpcRes();
	~KNpcRes();
	BOOL			Init(char *lpszNpcName, KNpcResList *pNpcResList, int nNpcTemplateIdx, bool binitPos = true );	// 初始化
	void			Remove(int nNpcIdx, bool bRemoveFromScene);								// 清除
	//Modified by [Ray]  2005-6-21
	void			Draw(int nNpcIdx, int nDir, int nAllFrame, int nCurFrame, 
		BOOL bInMenu = FALSE, BOOL bDrawSelected = FALSE, int nSelectedType = 0, int nHover = false, int nDrawType = 0, int nBeginFrame = 0, int nEndFrame = 0 );		// 绘制
	//-------> Ray [Luoliang] 2005-7-11
	void			DrawUI(int x, int y, int nNpcIdx, int nDir, int nAllFrame, int nCurFrame);		// 绘制
	//<------- End [Ray]
	void			DrawBorder(int nDir, int nAllFrame, int nCurFrame);
	int				DrawMenuState(int nHeightOffset);
	BOOL			SetPart(unsigned int uPart,int nType, int nPal, bool bUi = false);
	BOOL			SetAction(int nDoing);								// 设定动作类型
	int				GetCurTotalFrame( int nDoing );
// 设定套装特效 lixuewu 
	BOOL			SetGreenItemEffect(unsigned int uGreenID, int nSetType, int nRoleType);
	void			RemoveGreenItemEffect(void) 
	{
		KStateSpr& aStateSpr = m_cStateSpr[6];
		aStateSpr.Release();		
	}
// 设定套装特效 lixuewu 
	BOOL			SetRideHorse(BOOL bRideHorse, bool bUi = false);						// 设定是否骑马
	void			SetPos(int nNpcIdx, int x, int y, int z = 0, BOOL bFocus = FALSE, BOOL bMenu = FALSE);// 设定 npc 位置
	void			UpDateScene(int nNpcIdx, BOOL bFocus);
	void			AddState( unsigned long ulState, int nID );	// 设定状态特效
	void			AddHState( unsigned long ulState, int nID );	// 设定状态特效
	void			AddBState( unsigned long ulState, int nID );	// 设定状态特效
	void			AddFState( unsigned long ulState, int nID );	// 设定状态特效
	void			ClearState( int nID );
	void			ClearAllState( void );

	void			AddInlayState( int nIdx, int nID );	// 设定状态特效
	void			ClearInlayState( int nIdx  );	// 设定状态特效
	void			ClearAllInlayState( void );	// 设定状态特效

	void			SetSpecialSpr(char *lpszSprName);					// 设定特殊的只播放一遍的随身spr文件
	void			SetBlur(BOOL bBlur);								// 残影打开关闭
	void			SetAdjustColorId(unsigned long ulColorId){m_ulAdjustColorId = ulColorId;};			// 设置偏色情况，如果为0表示不偏色.
	int				GetAction(){return m_nAction;};
	void			SetMenuState(int nState, char *lpszSentence = NULL, int nSentenceLength = 0);	// 设定头顶状态
	int				GetMenuState();						// 获得头顶状态
	//-------> Ray [Luoliang] 2005-7-19
	int				GetMenuSentence(char *szBuf);
	//<------- End [Ray]
	void			SetSleepState(BOOL bFlag);			// 设定睡眠状态
	BOOL			GetSleepState();						// 获得睡眠状态
	void			StopSound();
	static void		GetShadowName(char *lpszShadow, unsigned int nNo);
	int				GetNormalNpcStandDir(int nFrame);	// 动画帧数转换成逻辑方向(0 - 63)
	unsigned int	GetQuestIcon() const {return m_uQuestIcon; }
	void			SetQuestIcon(unsigned int uIcon) { m_uQuestIcon = uIcon; }

public:
	unsigned int	m_uHue;
private:
	unsigned int	m_uQuestIcon;
};




#endif
