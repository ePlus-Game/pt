//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 2008-6-3
//      File_base        : question
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 问答系统
//
//////////////////////////////////////////////////////////////////////

#ifndef _QUESTION_H_
#define _QUESTION_H_

#include "fontinterface.h"

#define MAX_QUESTION_POOL_SIZE 50			//最大问题池尺寸
#define MAX_QUESTION_TEXT_SIZE 16			//最大问题答案文本字符个数
#define MAX_QUESTION_SIZE (1024 * 5)		//最大问题数据尺寸
#define MAX_CHARACTER_SET 5					//最多5个字符集
#define MAX_CHARACTER_COUNT 1000			//每个字符集最多1000个字符
#define MAX_FONT_NAME_SIZE 16				//最大字体名称尺寸
#define MAX_FONT_PATH_SIZE 128				//最大字体路径尺寸
#define MAX_FONT_COUNT 10					//最大字体种类

typedef utf16 Character;
typedef std::vector<Character> CharacterArray;

//问题实例
struct QuestionInstance
{
	QuestionInstance()
	{
		Id = 0;
		IsQuestionCompressed = false;
		QuestionLength = 0;
		memset(Question, 0, sizeof(Question));
		memset(Answer, 0, sizeof(Answer));
		AnswerSize = 0;
	}

	DWORD Id;
	bool IsQuestionCompressed;
	DWORD QuestionLength;
	char Question[MAX_QUESTION_SIZE];
	Character Answer[MAX_QUESTION_TEXT_SIZE];
	int AnswerSize;
};

//字体配置
struct FontConfig 
{
	FontConfig()
	{
		memset(Name, 0, sizeof(Name));
		memset(Path, 0, sizeof(Path));
	};

	char Name[MAX_FONT_NAME_SIZE];
	char Path[MAX_FONT_PATH_SIZE];
};

//字体类型
enum enumFontType
{
	font_type_normal = 0,
	font_type_noise,
	
	font_type_count
};

//问答管理器
class QuestionManager
{
public:
	QuestionManager();
	~QuestionManager();

	static QuestionManager& Singleton();

	void LoadFonts();//重新载入字体
	void ReloadAllSettings();//重新载入所有配置
	void ProcessLoadQuestionSettings(IProcRet* pRet, int systemVar);//处理重新载入问题参数

	void Active();//活动一次
	bool GetRandomQuestion(QuestionInstance& questionInstance);//随机得到一个问题
	int GetKeepTime() const;//回答问题正确保持时间
	int GetForbidQuestionTime() const;//禁止请求提问的时间
	int GetTimeout() const;//默认回答问题超时
	bool IsEnabled() const;//是否启用
	DWORD GetBadAnswerClearInterval() const;
	DWORD GetBadAnswerMaxCount() const;
	DWORD GetLongTermBadAnswerClearInterval() const;
	DWORD GetLongTermBadAnswerMaxCount() const;
	DWORD GetBadAnswerStage1KeepTime() const;
	DWORD GetBadAnswerStage2KeepTime() const;

private:
	bool CreateQuestion(QuestionInstance& instance);//生成问题
	void RefreshQuestionPool();//刷新问题池
	bool RandomNumText(int charCount, Character* pTextBuff, int buffSize, Character* pNumTextBuff, int numBuffSize);
	bool RandomFont(char* szFontName, int buffSize, enumFontType fontType = font_type_normal);

	bool CreateQuestionType1(QuestionInstance& instance);
	bool CreateQuestionType2(QuestionInstance& instance);

	int m_QuestionTypeCount;
	QuestionInstance m_QuestionPool[MAX_QUESTION_POOL_SIZE];
	int m_NextRefreshQuestion;
	DWORD m_NextRefreshQuestionTime;
	DWORD m_NextQuestionInstanceId;
	IFontManager* m_pFontManager;
	char m_QuestionTemplate[MAX_QUESTION_SIZE];
	bool m_QuestionTemplateLoaded;
	FontConfig m_FontConfigs[MAX_FONT_COUNT][font_type_count];	
	int m_FontConfigCount[font_type_count];
	bool m_IsEnabled;
	DWORD m_BadAnswerClearInterval;
	DWORD m_BadAnswerMaxCount;
	DWORD m_LongTermBadAnswerClearInterval;
	DWORD m_LongTermBadAnswerMaxCount;
	DWORD m_BadAnswerStage1KeepTime;
	DWORD m_BadAnswerStage2KeepTime;

	//图形文字识别参数
	int m_PoolSize;//当前问题池的大小
	int m_RefreshInterval;//问题池更新一个问题的间隔
	bool m_Compress;//是否采用压缩
	int m_KeepTime;//回答问题正确保持时间（避免过多的向玩家提问）
	int m_ForbidQuestionTime;//禁止请求提问的时间（防止过快请求提问）
	int m_Timeout;//单个问答的超时
	int m_CharCountMin;//最小文字个数
	int m_CharCountMax;//最大问题个数
	int m_FontSizeMin;//字体尺寸
	int m_FontSizeMax;//字体尺寸
	int m_OverlapMin;//交叠尺寸
	int m_OverlapMax;//交叠尺寸
	int m_PlusPercent;//杂点比例
	int m_ColorType;//色彩类型
	int m_ForeColor;//前景色
	int m_BgColor;//背景色
	int m_AngleMin;//旋转角度
	int m_AngleMax;//旋转角度
	int m_XTransMin;//X错位
	int m_XTransMax;//X错位
	int m_YTransMin;//Y错位
	int m_YTransMax;//Y错位
	int m_XScaleMin;//X缩放
	int m_XScaleMax;//X缩放
	int m_YScaleMin;//Y缩放
	int m_YScaleMax;//Y缩放
	int m_ImageHeight;//图片高度
	int m_NoiseScaleMin;//干扰缩放
	int m_NoiseScaleMax;//干扰缩放
	int m_NoiseCharMin;//干扰字符数
	int m_NoiseCharMax;//干扰字符数
};

//回调脚本参数
struct CallbackScriptParam
{
	CallbackScriptParam()
	{
		ScriptId = 0;
		memset(FuncName, 0, sizeof(FuncName));
		NpcIndex = 0;
		NpcSettingIdx = 0;
		NpcId = 0;
	}

	DWORD ScriptId;
	char FuncName[128];
	int NpcIndex;
	int NpcSettingIdx;
	DWORD NpcId;
};

//问答结果
enum enumQuestionResult
{
	question_result_right_answer = 0,
	question_result_wrong_answer,
	question_result_timeout,
};

//回答错误状态
enum enumBadAnswerState
{
	bad_answer_normal = 0,
	bad_answer_stage1,
	bad_answer_stage2,
};

//问答回调函数
typedef void (*PQUESTION_CALLBACK)(int playerIdx, enumQuestionResult questionResult);

//问答状态
class QuestionState
{
public:
	QuestionState();
	~QuestionState();

	void Init(int playerIndex);//初始化
	void Active();//活动一次
	bool NewQuestion(int keepCount, int forbidTime, int timeout, CallbackScriptParam* pCallbackParam, PQUESTION_CALLBACK pCallbackFunc);//新一次提问
	bool IsHuman() const;//是否真人（非外挂玩家）
	void OnAnswer(const void* pAnswer, int answerSize);//收到问题答案
	void SetTempParam(CallbackScriptParam& param);
	void GetTempParam(CallbackScriptParam& param);
	void UseKeepCount();//消耗保持计数
	bool HasQuestion() const;
	void OnTimeout();//回答超时

	void GetState(DWORD& startTime, DWORD& count, DWORD& state, DWORD& stageStartTime, DWORD& longTermStartTime, DWORD& longTermCount);
	void SetState(DWORD startTime, DWORD count, DWORD state, DWORD stageStartTime, DWORD longTermStartTime, DWORD longTermCount);
	void ResetBadAnswerState();
	
private:
	bool IsTimeout() const;//回答是否超时
	void Reset();//重置
	bool TooSoonToQuestion() const;//是否提问太快
	void RememberPos();//记录当前位置
	bool CheckPos() const;//检查位置
	void BadAnswer();//回答错误/超时
	void ResetBadAnswerCount();	
	DWORD GetBadAnswerStageEndTime() const;
	void ResetLongTermBadAnswerCount();

	DWORD m_QuestionId;
	Character m_Answer[MAX_QUESTION_TEXT_SIZE];
	int m_AnswerSize;
	DWORD m_Timeout;
	int m_PlayerIndex;
//	DWORD m_NextCheckTime;
	DWORD m_NextCanQuestionTime;
	int m_TempKeepCount;
	int m_KeepCount;
	CallbackScriptParam m_CallbackScripParam;
	CallbackScriptParam m_TempCallbackScripParam;
	PQUESTION_CALLBACK m_CallbackFunc;
	int m_Subworld;
	int m_MapPosX;
	int m_MapPosY;

	DWORD m_BadAnswerStartTime;
	DWORD m_BadAnswerCount;
	DWORD m_LongTermBadAnswerStartTime;
	DWORD m_LongTermBadAnswerCount;
	enumBadAnswerState m_BadAnswerState;
	DWORD m_BadAnswerStageStartTime;
};

#endif// _QUESTION_H_