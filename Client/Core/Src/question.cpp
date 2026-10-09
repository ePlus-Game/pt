//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 2008-6-3
//      File_base        : question
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 问答系统
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "question.h"
#include "KLuaScript.h"
#include "LuaFuns.h"
#include "KPlayer.h"
#include "CoreRelated.h"

#define FONT_CONFIG_FILE "/settings/fontconfig.txt"
#define MSG_QUESTION_SUCCESS 11375
#define MSG_QUESTION_WRONG_ANSWER 11376
#define MSG_QUESTION_TIMEOUT 11377
#define MSG_QUESTION_TOO_SOON 11378
#define MSG_QUESTION_TOO_MANY_BAD_ANSWER 11379
#define MSG_QUESTION_BAD_ANSWER_STAGE1 11373
#define MSG_QUESTION_BAD_ANSWER_STAGE2 11374
#define MSG_QUESTION_BAD_ANSWER_BEFORE_STAGE1 11383
#define MSG_QUESTION_BAD_ANSWER_BEFORE_STAGE2 11384
#define MSG_QUESTION_APPEND_DESC_IN_GAME 11385

QuestionManager::QuestionManager()
{
	m_QuestionTypeCount = 1;
	m_NextRefreshQuestion = 0;
	m_NextRefreshQuestionTime = 0;
	m_NextQuestionInstanceId = 1;
	m_IsEnabled = false;
	m_PoolSize = 10;
	m_RefreshInterval = 5;
	m_Compress = true;
	m_QuestionTemplateLoaded = false;
	memset(m_QuestionTemplate, 0, sizeof(m_QuestionTemplate));
	m_KeepTime = 30;
	m_ForbidQuestionTime = 10;
	m_Timeout = 30;
	m_CharCountMin = 3;
	m_CharCountMax = 5;
	m_FontSizeMin = 20;
	m_FontSizeMax = 20;
	m_OverlapMin = 5;
	m_OverlapMax = 10;
	m_PlusPercent = 15;
	m_ColorType = FMT_BIT;
	m_ForeColor = 0xff000000;
	m_BgColor = 0xffffffff;
	m_AngleMin = 0;
	m_AngleMax = 20;
	m_XTransMin = 0;
	m_XTransMax = 5;
	m_YTransMin = 0;
	m_YTransMax = 5;
	m_XScaleMin = 75;
	m_XScaleMax = 125;
	m_YScaleMin = 75;
	m_YScaleMax = 125;
	m_ImageHeight = 42;
	m_NoiseScaleMin = 25;
	m_NoiseScaleMax = 50;
	m_NoiseCharMin = 0;
	m_NoiseCharMax = 0;
	m_BadAnswerClearInterval = 60;
	m_BadAnswerMaxCount = 10;
	m_LongTermBadAnswerClearInterval = 60;
	m_LongTermBadAnswerMaxCount = 50;
	m_BadAnswerStage1KeepTime = 3 * 3600;
	m_BadAnswerStage2KeepTime = 24 * 3600;
	
	memset(m_FontConfigs, 0, sizeof(m_FontConfigs));
	memset(m_FontConfigCount, 0, sizeof(m_FontConfigCount));

	m_pFontManager = NULL;
	CreateFontManager(&m_pFontManager);
}

QuestionManager::~QuestionManager()
{
}

QuestionManager& QuestionManager::Singleton()
{
	static QuestionManager questionManager;
	return questionManager;
}

void QuestionManager::ReloadAllSettings()
{
	GetSystemVar(system_var_question_pool_size, system_var_value_type_int);
	GetSystemVar(system_var_question_refresh_interval, system_var_value_type_int);
	GetSystemVar(system_var_question_compress, system_var_value_type_int);
	GetSystemVar(system_var_question_template, system_var_value_type_blob);
	GetSystemVar(system_var_question_keep_time, system_var_value_type_int);
	GetSystemVar(system_var_question_timeout, system_var_value_type_int);
	GetSystemVar(system_var_question_forbid_time, system_var_value_type_int);	
	GetSystemVar(system_var_question_char_count_min, system_var_value_type_int);
	GetSystemVar(system_var_question_char_count_max, system_var_value_type_int);
	GetSystemVar(system_var_question_font_size_min, system_var_value_type_int);
	GetSystemVar(system_var_question_font_size_max, system_var_value_type_int);
	GetSystemVar(system_var_question_overlap_min, system_var_value_type_int);
	GetSystemVar(system_var_question_overlap_max, system_var_value_type_int);
	GetSystemVar(system_var_question_plus_percent, system_var_value_type_int);
	GetSystemVar(system_var_question_angle_min, system_var_value_type_int);
	GetSystemVar(system_var_question_angle_max, system_var_value_type_int);
	GetSystemVar(system_var_question_xtrans_min, system_var_value_type_int);
	GetSystemVar(system_var_question_xtrans_max, system_var_value_type_int);
	GetSystemVar(system_var_question_ytrans_min, system_var_value_type_int);
	GetSystemVar(system_var_question_ytrans_max, system_var_value_type_int);
	GetSystemVar(system_var_question_xscale_min, system_var_value_type_int);
	GetSystemVar(system_var_question_xscale_max, system_var_value_type_int);
	GetSystemVar(system_var_question_yscale_min, system_var_value_type_int);
	GetSystemVar(system_var_question_yscale_max, system_var_value_type_int);
	GetSystemVar(system_var_question_image_height, system_var_value_type_int);
	GetSystemVar(system_var_question_noise_scale_min, system_var_value_type_int);
	GetSystemVar(system_var_question_noise_scale_max, system_var_value_type_int);
	GetSystemVar(system_var_question_noise_char_min, system_var_value_type_int);
	GetSystemVar(system_var_question_noise_char_max, system_var_value_type_int);
	GetSystemVar(system_var_question_bad_answer_clear_interval, system_var_value_type_int);
	GetSystemVar(system_var_question_bad_answer_max_count, system_var_value_type_int);
	GetSystemVar(system_var_question_long_term_bad_answer_clear_interval, system_var_value_type_int);
	GetSystemVar(system_var_question_long_term_bad_answer_max_count, system_var_value_type_int);
	GetSystemVar(system_var_question_bad_answer_stage1_keep_time, system_var_value_type_int);
	GetSystemVar(system_var_question_bad_answer_stage2_keep_time, system_var_value_type_int);

	GetSystemVar(system_var_question_enabled, system_var_value_type_int);
}

void QuestionManager::ProcessLoadQuestionSettings(IProcRet* pRet, int systemVar)
{
	if (NULL == pRet || 0 == pRet->GetExeRet() || 0 == pRet->GetRet())
		return;

	if (pRet->GetRowCount() != 1)
		return;

	switch (systemVar)
	{
	case system_var_question_enabled:
		{
			int enabled = TRUE;
			pRet->GetData(0, 0, enabled);
			m_IsEnabled = (TRUE == enabled);
		}
		break;
	case system_var_question_pool_size:
		{
			pRet->GetData(0, 0, m_PoolSize);
			if (m_PoolSize > MAX_QUESTION_POOL_SIZE)
				m_PoolSize = MAX_QUESTION_POOL_SIZE;
			else if (m_PoolSize < 0)
				m_PoolSize = 0;
		}
		break;
	case system_var_question_refresh_interval:
		pRet->GetData(0, 0, m_RefreshInterval);
		break;
	case system_var_question_compress:
		{
			int compress = TRUE;
			pRet->GetData(0, 0, compress);
			m_Compress = (TRUE == compress);
		}		
		break;
	case system_var_question_template:
		{
			memset(m_QuestionTemplate, 0 , sizeof(m_QuestionTemplate));			
			pRet->GetData(0, 0, m_QuestionTemplate, sizeof(m_QuestionTemplate));			
			m_QuestionTemplate[sizeof(m_QuestionTemplate) - 1] = 0;
			m_QuestionTemplateLoaded = true;
		}
		break;
	case system_var_question_keep_time:
		pRet->GetData(0, 0, m_KeepTime);
		break;
	case system_var_question_timeout:
		pRet->GetData(0, 0, m_Timeout);
		break;
	case system_var_question_forbid_time:
		pRet->GetData(0, 0, m_ForbidQuestionTime);
		break;
	case system_var_question_char_count_min:
		pRet->GetData(0, 0, m_CharCountMin);
		break;
	case system_var_question_char_count_max:
		pRet->GetData(0, 0, m_CharCountMax);
		break;
	case system_var_question_font_size_min:
		pRet->GetData(0, 0, m_FontSizeMin);
		break;
	case system_var_question_font_size_max:
		pRet->GetData(0, 0, m_FontSizeMax);
		break;	
	case system_var_question_overlap_min:
		pRet->GetData(0, 0, m_OverlapMin);
		break;
	case system_var_question_overlap_max:
		pRet->GetData(0, 0, m_OverlapMax);
		break;
	case system_var_question_plus_percent:
		pRet->GetData(0, 0, m_PlusPercent);
		break;
	case system_var_question_angle_min:
		pRet->GetData(0, 0, m_AngleMin);
		break;
	case system_var_question_angle_max:
		pRet->GetData(0, 0, m_AngleMax);
		break;	
	case system_var_question_xtrans_min:
		pRet->GetData(0, 0, m_XTransMin);
		break;
	case system_var_question_xtrans_max:
		pRet->GetData(0, 0, m_XTransMax);
		break;	
	case system_var_question_ytrans_min:
		pRet->GetData(0, 0, m_YTransMin);
		break;
	case system_var_question_ytrans_max:
		pRet->GetData(0, 0, m_YTransMax);
		break;
	case system_var_question_xscale_min:
		pRet->GetData(0, 0, m_XScaleMin);
		break;
	case system_var_question_xscale_max:
		pRet->GetData(0, 0, m_XScaleMax);
		break;	
	case system_var_question_yscale_min:
		pRet->GetData(0, 0, m_YScaleMin);
		break;
	case system_var_question_yscale_max:
		pRet->GetData(0, 0, m_YScaleMax);
		break;
	case system_var_question_image_height:
		pRet->GetData(0, 0, m_ImageHeight);
		break;
	case system_var_question_noise_scale_min:
		pRet->GetData(0, 0, m_NoiseScaleMin);
		break;
	case system_var_question_noise_scale_max:
		pRet->GetData(0, 0, m_NoiseScaleMax);
		break;
	case system_var_question_noise_char_min:
		pRet->GetData(0, 0, m_NoiseCharMin);
		break;
	case system_var_question_noise_char_max:
		pRet->GetData(0, 0, m_NoiseCharMax);
		break;
	case system_var_question_bad_answer_clear_interval:
		pRet->GetData(0, 0, m_BadAnswerClearInterval);
		break;
	case system_var_question_bad_answer_max_count:
		pRet->GetData(0, 0, m_BadAnswerMaxCount);
		break;
	case system_var_question_long_term_bad_answer_clear_interval:
		pRet->GetData(0, 0, m_LongTermBadAnswerClearInterval);
		break;
	case system_var_question_long_term_bad_answer_max_count:
		pRet->GetData(0, 0, m_LongTermBadAnswerMaxCount);
		break;
	case system_var_question_bad_answer_stage1_keep_time:
		pRet->GetData(0, 0, m_BadAnswerStage1KeepTime);
		break;
	case system_var_question_bad_answer_stage2_keep_time:
		pRet->GetData(0, 0, m_BadAnswerStage2KeepTime);
		break;
	}
}

void QuestionManager::LoadFonts()
{
	if (NULL == m_pFontManager)
		return;

	memset(m_FontConfigCount, 0, sizeof(m_FontConfigCount));
	KTabFile fontConfigFile;
	if (TRUE == fontConfigFile.Load(FONT_CONFIG_FILE))
	{
		int row = 0;
		int recordCount = fontConfigFile.GetHeight() - 1;
		
		for (int record = 0; record < recordCount; ++record)
		{
			row = record + 2;
			int field = 1;

			int type = 0;
			if (FALSE == fontConfigFile.GetInteger(row, field++, 0, &type))
				continue;

			if (type < font_type_normal || type >= font_type_count)
				continue;

			FontConfig& fc = m_FontConfigs[m_FontConfigCount[type]][type];

			if (FALSE == fontConfigFile.GetString(row, field++, "", fc.Name, sizeof(fc.Name)))
				continue;
			if (FALSE == fontConfigFile.GetString(row, field++, "", fc.Path, sizeof(fc.Path)))
				continue;
			
			if (m_pFontManager->IAddFont(fc.Name, fc.Path))
				m_FontConfigCount[type]++;
		}
	}
}

void QuestionManager::Active()
{
	if (!m_IsEnabled)
		return;

	if (m_NextRefreshQuestionTime <= UNIX_TMIE_STAMP)
	{
		m_NextRefreshQuestionTime = UNIX_TMIE_STAMP + m_RefreshInterval;
		RefreshQuestionPool();
	}
}

bool QuestionManager::GetRandomQuestion(QuestionInstance& questionInstance)
{
	int randomIndex = g_Random(m_PoolSize);

	if (randomIndex >= 0 && randomIndex < m_PoolSize)
	{
		if (m_QuestionPool[randomIndex].Id > 0)
		{
			memcpy(&questionInstance, &(m_QuestionPool[randomIndex]), sizeof(questionInstance));
			return true;
		}
	}

	return false;
}

void QuestionManager::RefreshQuestionPool()
{
	if (m_PoolSize <= 0 || m_PoolSize > MAX_QUESTION_POOL_SIZE)
		return;

	if (m_NextRefreshQuestion < 0 || m_NextRefreshQuestion >= m_PoolSize)
		m_NextRefreshQuestion = 0;

	if (CreateQuestion(m_QuestionPool[m_NextRefreshQuestion]))
	{
		m_NextRefreshQuestion++;
	}
}

bool QuestionManager::CreateQuestion(QuestionInstance& instance)
{
	if (m_QuestionTypeCount <= 0)
		return false;

	memset(&instance, 0, sizeof(instance));
	
	//---------------------------------------------------------------------------------
	//根据类型创建问题
	bool createResult = false;
	int randomQuestionType = g_Random(m_QuestionTypeCount) + 1;
	switch(randomQuestionType)
	{
	case 1:
		createResult = CreateQuestionType1(instance);
		break;
	case 2:
		createResult = CreateQuestionType2(instance);
		break;
	}

	if (!createResult)
		return false;
	//---------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------
	//压缩问题
	if (m_Compress)
	{
		char compressBuff[COMMON_QUESTION_BUFF_SIZE];
		memset(compressBuff, 0, sizeof(compressBuff));
		
		BYTE* pCompressBuff = (BYTE*)compressBuff;
		unsigned int compressBuffLength = COMMON_QUESTION_BUFF_SIZE;
		BYTE* pCompressSrc = (BYTE*)instance.Question;
		unsigned int compressSrcLength = instance.QuestionLength;
		lzo1x_1_compress(
			pCompressSrc,
			compressSrcLength,
			pCompressBuff,
			&compressBuffLength,
			wrkmem);
		
		if (compressBuffLength > 0 && compressBuffLength < sizeof(instance.Question))
		{
			memcpy(instance.Question, pCompressBuff, compressBuffLength);
			instance.QuestionLength = compressBuffLength;
		}
		else
		{
			//压缩失败
			return false;
		}
	}
	instance.IsQuestionCompressed = m_Compress;
	//---------------------------------------------------------------------------------
	
	instance.Id = m_NextQuestionInstanceId++;

	return true;
}

bool QuestionManager::CreateQuestionType1(QuestionInstance& instance)
{
	if (!m_QuestionTemplateLoaded)
		return false;

	//---------------------------------------------------------------------------------
	//创建随机字符串
	int charCount = g_Random(m_CharCountMax - m_CharCountMin + 1) + m_CharCountMin;

	Character randomText[MAX_QUESTION_TEXT_SIZE];
	memset(randomText, 0, sizeof(randomText));		
	
	if (!RandomNumText(charCount, randomText, sizeof(randomText), instance.Answer, sizeof(instance.Answer)))
		return false;

	instance.AnswerSize = charCount;

	char imgDataBuff[RGB565_BUFF_MAX] = { 0 };
	int imgDataLength = sizeof(imgDataBuff);
	//---------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------
	//生成图片		
	if (NULL == m_pFontManager)
		return false;
	
	bool hasGap = false;
	int fontSize = g_Random(abs(m_FontSizeMax - m_FontSizeMin)) + m_FontSizeMin;
	int fontWidth = 0;
	int fontHeight = 0;
	int currentXPos = 0;
	m_pFontManager->IGetFontSize(fontSize, fontWidth, fontHeight);
	TextureFont fontArray[MAX_QUESTION_TEXT_SIZE];
	for (int i = 0; i < charCount; i++)
	{
		TextureFont& font = fontArray[i];
		font.d_colour		= m_ForeColor;
		font.d_angle		= g_Random(abs(m_AngleMax - m_AngleMin)) + m_AngleMin;
		font.d_xTrans		= g_Random(abs(m_XTransMax - m_XTransMin)) + m_XTransMin;
		font.d_yTrans		= g_Random(abs(m_YTransMax - m_YTransMin)) + m_YTransMin;
		font.d_xScalc		= g_Random(abs(m_XScaleMax - m_XScaleMin)) + m_XScaleMin;
		font.d_yScalc		= g_Random(abs(m_YScaleMax - m_YScaleMin)) + m_YScaleMin;
		font.d_space		= g_Random(abs(m_OverlapMax - m_OverlapMin)) + m_OverlapMin;
		font.d_x			= currentXPos;
		font.d_y			= g_Random(m_ImageHeight - fontHeight * 3 / 4);
		char fontName[MAX_FONT_NAME_SIZE] = { 0 };
		if (RandomFont(fontName, sizeof(fontName)))
		{
			font.d_fontName = fontName;			
		}
		else
		{
			return false;
		}

		currentXPos += fontWidth;
		if (i + 1 < charCount)
		{
			if (hasGap || g_Random(charCount - 1))
			{
				currentXPos -= g_Random(abs(m_OverlapMax - m_OverlapMin)) + m_OverlapMin;
			}
			else
			{
				hasGap = true;
				currentXPos += g_Random(abs(m_OverlapMax - m_OverlapMin)) + m_OverlapMin;
			}			
		}
	}

	char noiseFontName[MAX_FONT_NAME_SIZE] = { 0 };
	int noiseCharCount = 0;
	int noiseScale = 100;
	if (RandomFont(noiseFontName, sizeof(noiseFontName), font_type_noise))
	{
		noiseFontName[sizeof(noiseFontName) - 1] = 0;

		noiseCharCount = g_Random(abs(m_NoiseCharMax - m_NoiseCharMin)) + m_NoiseCharMin;
		noiseScale = g_Random(abs(m_NoiseScaleMax - m_NoiseScaleMin)) + m_NoiseScaleMin;
	}	
	
	m_pFontManager->IFontGetTextBmpPosition(
		currentXPos,
		m_ImageHeight,
		(BYTE*)imgDataBuff,
		imgDataLength,
		fontSize,
		(FONTCOLOURTYPE)m_ColorType,
		m_BgColor,
		m_ForeColor,
		m_PlusPercent,
		noiseFontName,
		noiseCharCount,
		noiseScale,
		randomText,
		fontArray);

	if (imgDataLength > MAX_QUESTION_SIZE)
		imgDataLength = MAX_QUESTION_SIZE;
	//---------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------
	//拼接问题数据
	char* pQuestionData = instance.Question;
	int leftQuestionBuffSize = sizeof(instance.Question);

	if (leftQuestionBuffSize < sizeof(WORD))
		return false;
	WORD* pImgLength = (WORD*)(pQuestionData);
	*pImgLength = imgDataLength;
	pQuestionData += sizeof(WORD);
	leftQuestionBuffSize -= sizeof(WORD);

	if (leftQuestionBuffSize < imgDataLength)
		return false;
	memcpy(pQuestionData, imgDataBuff, imgDataLength);
	pQuestionData += imgDataLength;
	leftQuestionBuffSize -= imgDataLength;

	int templateSize = strlen(m_QuestionTemplate);

	if (leftQuestionBuffSize < sizeof(WORD))
		return false;
	WORD* pQuestionTextLength = (WORD*)pQuestionData;
	*pQuestionTextLength = templateSize;
	pQuestionData += sizeof(WORD);
	leftQuestionBuffSize -= sizeof(WORD);

	if (leftQuestionBuffSize < templateSize)
		return false;
	memcpy(pQuestionData, m_QuestionTemplate, templateSize);
	pQuestionData += templateSize;
	leftQuestionBuffSize -= templateSize;

	instance.QuestionLength = pQuestionData - instance.Question;
	//---------------------------------------------------------------------------------

	return true;
}

bool QuestionManager::CreateQuestionType2(QuestionInstance& instance)
{
	return false;
}

bool QuestionManager::RandomNumText(int charCount, Character* pTextBuff, int buffSize, Character* pNumTextBuff, int numBuffSize)
{
	if (charCount <= 0
		|| NULL == pTextBuff
		|| (charCount * sizeof(Character)) > buffSize
		|| 	NULL == pNumTextBuff
		|| (charCount * sizeof(Character)) > numBuffSize)
	{
		return false;
	}

	const int NumberCount = 9;
	static Character numberCharArray[NumberCount] = { 0x58F9, 0x8D30, 0x53C1, 0x8086, 0x4F0D, 0x9646, 0x67D2, 0x634C, 0x7396 };//壹贰叁肆伍陆柒捌玖
	static Character numberArray[NumberCount] = { 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039 };//123456789

	for (int i = 0; i < charCount; i++)
	{
		int randomIndex = g_Random(NumberCount);
		if (randomIndex < 0 || randomIndex >= NumberCount)
			randomIndex = 0;

		pTextBuff[i] = numberCharArray[randomIndex];
		pNumTextBuff[i] = numberArray[randomIndex];
	}

	return true;
}

int QuestionManager::GetKeepTime() const
{
	return m_KeepTime;
}

int QuestionManager::GetForbidQuestionTime() const
{
	return m_ForbidQuestionTime;
}

int QuestionManager::GetTimeout() const
{
	return m_Timeout;
}

bool QuestionManager::IsEnabled() const
{
	return m_IsEnabled;
}

bool QuestionManager::RandomFont(char* szFontName, int buffSize, enumFontType fontType)
{
	if (NULL == szFontName || buffSize <= 0)
		return false;

	if (fontType < font_type_normal || fontType >= font_type_count)
		return false;

	int fontCount = m_FontConfigCount[fontType];

	if (fontCount <= 0 || fontCount > MAX_FONT_COUNT)
		return false;

	int randomFontIndex = g_Random(fontCount);
	if (randomFontIndex < 0 || randomFontIndex >= fontCount)
		randomFontIndex = 0;

	int size = buffSize < MAX_FONT_NAME_SIZE ? buffSize : MAX_FONT_NAME_SIZE;
	strncpy(szFontName, m_FontConfigs[randomFontIndex][fontType].Name, size);
	szFontName[buffSize - 1] = 0;

	return true;
}

DWORD QuestionManager::GetBadAnswerClearInterval() const
{
	return m_BadAnswerClearInterval;
}

DWORD QuestionManager::GetBadAnswerMaxCount() const
{
	return m_BadAnswerMaxCount;
}

DWORD QuestionManager::GetLongTermBadAnswerClearInterval() const
{
	return m_LongTermBadAnswerClearInterval;
}

DWORD QuestionManager::GetLongTermBadAnswerMaxCount() const
{
	return m_LongTermBadAnswerMaxCount;
}

DWORD QuestionManager::GetBadAnswerStage1KeepTime() const
{
	return m_BadAnswerStage1KeepTime;
}

DWORD QuestionManager::GetBadAnswerStage2KeepTime() const
{
	return m_BadAnswerStage2KeepTime;
}

QuestionState::QuestionState()
{
	Init(0);
}

QuestionState::~QuestionState()
{
}

void QuestionState::Init(int playerIndex)
{
	m_QuestionId = 0;
	memset(m_Answer, 0, sizeof(m_Answer));
	m_AnswerSize = 0;
	m_Timeout = 0;
	m_PlayerIndex = playerIndex;
//	m_NextCheckTime = 0;
	m_NextCanQuestionTime = 0;
	m_TempKeepCount = 0;
	m_KeepCount = 0;

	ResetBadAnswerCount();
	ResetLongTermBadAnswerCount();
	m_BadAnswerState = bad_answer_normal;
	m_BadAnswerStageStartTime = 0;
}

void QuestionState::Active()
{
	if (m_BadAnswerStartTime + QuestionManager::Singleton().GetBadAnswerClearInterval() < UNIX_TMIE_STAMP)
	{
		ResetBadAnswerCount();
	}

	if (m_LongTermBadAnswerStartTime + QuestionManager::Singleton().GetLongTermBadAnswerClearInterval() < UNIX_TMIE_STAMP)
	{
		ResetLongTermBadAnswerCount();
	}

	DWORD endTime = GetBadAnswerStageEndTime();
	if (endTime > 0 && endTime < UNIX_TMIE_STAMP)
	{
		if (IsValidPlayer(m_PlayerIndex))
		{
			//取消惩罚状态
			Player[m_PlayerIndex].m_AntiEnthrall.SetEnforceState(AntiEnthrall::enAntiEnthrall_InValid, m_PlayerIndex);
		}
		
		m_BadAnswerState = bad_answer_normal;
		m_BadAnswerStageStartTime = 0;
	}

	if (0 == m_QuestionId)
		return;

	if (IsTimeout())
	{
		OnTimeout();
	}
}

void QuestionState::OnTimeout()
{
	//超时
	BadAnswer();

//	m_NextCheckTime = 0;

	if (m_CallbackFunc != NULL)
	{
		(*m_CallbackFunc)(m_PlayerIndex, question_result_timeout);
	}

	Player[m_PlayerIndex].ShowPredefinedMsg(MSG_QUESTION_TIMEOUT);
	Reset();
}

void QuestionState::OnAnswer(const void* pAnswer, int answerSize)
{
	//是否有问题正在进行中
	if (m_QuestionId <= 0)
		return;

	if (NULL == pAnswer)
		return;

	if (answerSize / sizeof(Character) == m_AnswerSize)
	{
		if (memcmp(m_Answer, pAnswer, sizeof(Character) * m_AnswerSize) == 0)//回答正确
		{
			//m_NextCheckTime = UNIX_TMIE_STAMP + QuestionManager::Singleton().GetKeepTime();
			m_KeepCount = m_TempKeepCount;

			//回调脚本
			if (m_CallbackScripParam.ScriptId > 0 && CheckPos())
			{
				//判断NPC是否是同一个
				if (IsValidNpc(m_CallbackScripParam.NpcIndex) && Npc[m_CallbackScripParam.NpcIndex].GetId() == m_CallbackScripParam.NpcId)
				{
					Player[m_PlayerIndex].ExecuteScript2Param(
						m_CallbackScripParam.ScriptId,
						m_CallbackScripParam.FuncName,
						0,
						m_CallbackScripParam.NpcIndex,
						m_CallbackScripParam.NpcSettingIdx);
				}
			}

			if (m_CallbackFunc != NULL)
			{
				(*m_CallbackFunc)(m_PlayerIndex, question_result_right_answer);
			}

			Player[m_PlayerIndex].ShowPredefinedMsg(MSG_QUESTION_SUCCESS);
			Reset();
			return;
		}
	}

	//回答错误
	BadAnswer();

//	m_NextCheckTime = 0;

	if (m_CallbackFunc != NULL)
	{
		(*m_CallbackFunc)(m_PlayerIndex, question_result_wrong_answer);
	}

	Player[m_PlayerIndex].ShowPredefinedMsg(MSG_QUESTION_WRONG_ANSWER);
	Reset();
}

bool QuestionState::NewQuestion(int keepCount, int forbidTime, int timeout, CallbackScriptParam* pCallbackParam, PQUESTION_CALLBACK pCallbackFunc)
{
	//已经有问答正在进行中
	if (m_QuestionId > 0)
		return false;

	if (bad_answer_stage2 == m_BadAnswerState)
	{
		Player[m_PlayerIndex].ShowPredefinedMsg(MSG_QUESTION_TOO_MANY_BAD_ANSWER);
		return false;
	}

	if (TooSoonToQuestion())
	{
		Player[m_PlayerIndex].ShowPredefinedMsg(MSG_QUESTION_TOO_SOON);
		return false;
	}

	QuestionInstance question;

	//从问题池中随机选取一个问题
	if (!QuestionManager::Singleton().GetRandomQuestion(question))
		return false;
	
	//记录问题信息（答案，超时等）
	memcpy(m_Answer, question.Answer, sizeof(m_Answer));
	m_AnswerSize = question.AnswerSize;
	m_QuestionId = question.Id;

	if (timeout < 0)
		timeout = QuestionManager::Singleton().GetTimeout();

	//设置超时
	if (timeout == 0)//不超时
		m_Timeout = 0;
	else
		m_Timeout = UNIX_TMIE_STAMP + timeout;

	//发送问题到客户端
	int maxQuestionDataSize = COMMON_QUESTION_BUFF_SIZE - sizeof(QUESTION);
	char sendBuff[COMMON_QUESTION_BUFF_SIZE];
	memset(sendBuff, 0, sizeof(sendBuff));
	QUESTION* pSendQuestion = (QUESTION*)sendBuff;
	pSendQuestion->Protocol = s2c_byte_extend;
	pSendQuestion->ProtocolExtend = s2c_ex_protocol_question;
	pSendQuestion->wProtocolSize = sizeof(QUESTION) - sizeof(pSendQuestion->QuestionData) - 1;
	pSendQuestion->Timeout = timeout;
	pSendQuestion->AppendDescStrId = MSG_QUESTION_APPEND_DESC_IN_GAME;
	pSendQuestion->IsCompressed = question.IsQuestionCompressed ? TRUE : FALSE;
	memcpy(pSendQuestion->QuestionData, question.Question, question.QuestionLength);
	pSendQuestion->wProtocolSize += question.QuestionLength;
		
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[m_PlayerIndex].GetNetConnectIdx(), sendBuff, pSendQuestion->wProtocolSize + 1);

	if (forbidTime < 0)
		forbidTime = QuestionManager::Singleton().GetForbidQuestionTime();
	m_NextCanQuestionTime = UNIX_TMIE_STAMP + forbidTime;

	m_TempKeepCount = keepCount;

	if (NULL == pCallbackParam)
	{
		m_CallbackScripParam.ScriptId = 0;
	}
	else
	{
		m_CallbackScripParam = *pCallbackParam;
	}
	
	m_CallbackFunc = pCallbackFunc;

	RememberPos();

	return true;
}

void QuestionState::Reset()
{
	m_QuestionId = 0;
	memset(m_Answer, 0, sizeof(m_Answer));
	m_Timeout = 0;
}

bool QuestionState::IsTimeout() const
{
	return (m_Timeout > 0 && m_Timeout < UNIX_TMIE_STAMP);
}

bool QuestionState::IsHuman() const
{
	//return (m_NextCheckTime > UNIX_TMIE_STAMP) || (!QuestionManager::Singleton().IsEnabled());
	return (m_KeepCount > 0) || (!QuestionManager::Singleton().IsEnabled());
}

bool QuestionState::TooSoonToQuestion() const
{
	return (m_NextCanQuestionTime > UNIX_TMIE_STAMP);
}

void QuestionState::SetTempParam(CallbackScriptParam& param)
{
	m_TempCallbackScripParam = param;
}

void QuestionState::GetTempParam(CallbackScriptParam& param)
{
	param = m_TempCallbackScripParam;
}

void QuestionState::RememberPos()
{
	m_Subworld = 0;
	m_MapPosX = 0;
	m_MapPosY = 0;

	int npcIndex = Player[m_PlayerIndex].GetNpcIndex();
	if (!IsValidNpc(npcIndex))
		return;

	m_Subworld = Npc[npcIndex].GetSubWorldIndex();
	Npc[npcIndex].GetMpsPos(&m_MapPosX, &m_MapPosY);
}

#define MAX_MOVE_OFFSET 200

bool QuestionState::CheckPos() const
{
	int npcIndex = Player[m_PlayerIndex].GetNpcIndex();
	if (!IsValidNpc(npcIndex))
		return false;

	int currentSubworld = Npc[npcIndex].GetSubWorldIndex();
	
	if (m_Subworld != currentSubworld)
		return false;

	int currentPosX = 0;
	int currentPosY = 0;
	Npc[npcIndex].GetMpsPos(&currentPosX, &currentPosY);

	if (abs(currentPosX - m_MapPosX) > MAX_MOVE_OFFSET)
		return false;

	if (abs(currentPosY - m_MapPosY) > MAX_MOVE_OFFSET)
		return false;

	return true;
}

void QuestionState::UseKeepCount()
{
	m_KeepCount--;
}

void QuestionState::BadAnswer()
{
	if (!IsValidPlayer(m_PlayerIndex))
		return;

	m_BadAnswerCount++;
	m_LongTermBadAnswerCount++;

	if (m_BadAnswerCount >= QuestionManager::Singleton().GetBadAnswerMaxCount())
	{
		ResetBadAnswerCount();

		switch(m_BadAnswerState)
		{
		case bad_answer_normal:
			{
				Player[m_PlayerIndex].ShowPredefinedMsg(MSG_QUESTION_BAD_ANSWER_STAGE1);

				//进入惩罚状态1
				Player[m_PlayerIndex].m_AntiEnthrall.SetEnforceState(AntiEnthrall::enAntiEnthrall_Weariness, m_PlayerIndex);

				//提升为第一阶段
				m_BadAnswerState = bad_answer_stage1;
				m_BadAnswerStageStartTime = UNIX_TMIE_STAMP;
			}
			break;
		case bad_answer_stage1:
			{
				Player[m_PlayerIndex].ShowPredefinedMsg(MSG_QUESTION_BAD_ANSWER_STAGE2);

				//进入惩罚状态2
				Player[m_PlayerIndex].m_AntiEnthrall.SetEnforceState(AntiEnthrall::enAntiEnthrall_Insalubrity, m_PlayerIndex);

				//提升为第二阶段
				m_BadAnswerState = bad_answer_stage2;
				m_BadAnswerStageStartTime = UNIX_TMIE_STAMP;
			}
			break;
		case bad_answer_stage2:
			{
				//保持当前状态
				m_BadAnswerStageStartTime = UNIX_TMIE_STAMP;
			}
			break;
		}
	}
	else
	{
		switch(m_BadAnswerState)
		{
		case bad_answer_normal:
			{
				Player[m_PlayerIndex].ShowPredefinedMsg(MSG_QUESTION_BAD_ANSWER_BEFORE_STAGE1);
			}
			break;
		case bad_answer_stage1:
			{
				Player[m_PlayerIndex].ShowPredefinedMsg(MSG_QUESTION_BAD_ANSWER_BEFORE_STAGE2);
			}
			break;
		}
	}

	if (m_LongTermBadAnswerCount >= QuestionManager::Singleton().GetLongTermBadAnswerMaxCount())
	{
		ResetLongTermBadAnswerCount();
		
		switch(m_BadAnswerState)
		{
		case bad_answer_normal:
		case bad_answer_stage1:
			{
				Player[m_PlayerIndex].ShowPredefinedMsg(MSG_QUESTION_BAD_ANSWER_STAGE2);
				
				//进入惩罚状态2
				Player[m_PlayerIndex].m_AntiEnthrall.SetEnforceState(AntiEnthrall::enAntiEnthrall_Insalubrity, m_PlayerIndex);
				
				//提升为第二阶段
				m_BadAnswerState = bad_answer_stage2;
				m_BadAnswerStageStartTime = UNIX_TMIE_STAMP;
			}
			break;
		case bad_answer_stage2:
			{
				//保持当前状态
				m_BadAnswerStageStartTime = UNIX_TMIE_STAMP;
			}
			break;
		}
	}
}

void QuestionState::ResetBadAnswerCount()
{
	m_BadAnswerCount = 0;
	m_BadAnswerStartTime = UNIX_TMIE_STAMP;
}

void QuestionState::ResetLongTermBadAnswerCount()
{
	m_LongTermBadAnswerCount = 0;
	m_LongTermBadAnswerStartTime = UNIX_TMIE_STAMP;
}

DWORD QuestionState::GetBadAnswerStageEndTime() const
{
	switch(m_BadAnswerState)
	{
	case bad_answer_stage1:
		return m_BadAnswerStageStartTime + QuestionManager::Singleton().GetBadAnswerStage1KeepTime();
	case bad_answer_stage2:
		return m_BadAnswerStageStartTime + QuestionManager::Singleton().GetBadAnswerStage2KeepTime();
	}

	return 0;
}

void QuestionState::GetState(DWORD& startTime, DWORD& count, DWORD& state, DWORD& stageStartTime, DWORD& longTermStartTime, DWORD& longTermCount)
{
	startTime = m_BadAnswerStartTime;
	count = m_BadAnswerCount;
	state = (DWORD)m_BadAnswerState;
	stageStartTime = m_BadAnswerStageStartTime;
	longTermStartTime = m_LongTermBadAnswerStartTime;
	longTermCount = m_LongTermBadAnswerCount;
}

void QuestionState::SetState(DWORD startTime, DWORD count, DWORD state, DWORD stageStartTime, DWORD longTermStartTime, DWORD longTermCount)
{
	m_BadAnswerStartTime = startTime;
	m_BadAnswerCount = count;
	m_BadAnswerState = (enumBadAnswerState)state;
	m_BadAnswerStageStartTime = stageStartTime;
	m_LongTermBadAnswerStartTime = longTermStartTime;
	m_LongTermBadAnswerCount = longTermCount;

	if (!IsValidPlayer(m_PlayerIndex))
		return;
	
	switch(m_BadAnswerState)
	{
	case bad_answer_stage1:
		{
			Player[m_PlayerIndex].ShowPredefinedMsg(MSG_QUESTION_BAD_ANSWER_STAGE1);

			//进入惩罚状态1
			Player[m_PlayerIndex].m_AntiEnthrall.SetEnforceState(AntiEnthrall::enAntiEnthrall_Weariness, m_PlayerIndex);
		}
		break;
	case bad_answer_stage2:
		{
			Player[m_PlayerIndex].ShowPredefinedMsg(MSG_QUESTION_BAD_ANSWER_STAGE2);

			//进入惩罚状态2
			Player[m_PlayerIndex].m_AntiEnthrall.SetEnforceState(AntiEnthrall::enAntiEnthrall_Insalubrity, m_PlayerIndex);
		}
		break;
	}
}

void QuestionState::ResetBadAnswerState()
{
	m_BadAnswerStartTime = 0;
	m_BadAnswerCount = 0;
	m_LongTermBadAnswerStartTime = 0;
	m_LongTermBadAnswerCount = 0;
	m_BadAnswerState = bad_answer_normal;
	m_BadAnswerStageStartTime = 0;
	
	if (IsValidPlayer(m_PlayerIndex))
	{
		Player[m_PlayerIndex].m_AntiEnthrall.SetEnforceState(AntiEnthrall::enAntiEnthrall_InValid, m_PlayerIndex);
	}
}

bool QuestionState::HasQuestion() const
{
	return (m_QuestionId > 0);
}
