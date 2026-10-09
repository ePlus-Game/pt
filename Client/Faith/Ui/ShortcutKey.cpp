//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 08/04/2006 16:13
//      File_base        : ShortcutKey
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "KWin32Wnd.h"
#include "KIniFile.h"
#include "CoreUseNameDef.h"
#include "coreshell.h"
#include "iRepresentShell.h"
#include "ShortcutKey.h"
#include "UiCase/UiESCDlg.h"
#include "UiCase/UiChatCentre.h"
#include "UiCase/UiToolsControlBar.h"
#include "UiCase/UiGameSpace.h"
#include "UiCase/UiShortcutWnd.h"
#include "UiCase/UiMapCentre.h"
#include "UiCase/UiESCDlg.h"
#include "UiCase/UiChatWindow.h"
#include "UiCase/UiTargetFace.h"
#include "UiCase/UiEquipment.h"
#include "UiCase/UiItemBox.h"
#include "UiCase/UiStudySkillManage.h"
#include "UiCase/UiVendueWnd.h"
#include "UiCase/UiTongManager.h"
#include "UiCase/UiTalisman.h"
#include "UiCase/UiCompound.h"
#include "UiCase/UiMailCentre.h"
#include "UiCase/UiPlayerMenu.h"
#include "UiCase/UiItemTip.h"
#include "UiCase/UiSystemMessage.h"
#include "UiCase/UiQuestManage.h"
#include "UiCase/UiRoleHead.h"
#include "UiCase/UiFSBible.h"
#include "UiCase/UiNpcNavigation.h"
#include "../Login/Login.h"
#include "UiCase/UiDragItem.h"
#include "UiAdapter.h"
#include "Faith.h"
#include "SkillDef.h"
#include "UiCase/UiPlayerState.h"
#include "UiCase/UiHelpInfo.h"
#include "UiCase/UiTargetMenu.h"
#include "UiCase/UiComMsgBox.h"
#include "UiConfigManager.h"
#include "UiCase/UiBeginHelp.h"
#include "UiCase/UiSplitItemBox.h"
#include "UiCase/UiGameSetting.h"
#include "UiCase/UiErrorMessageBox.h"
#include "UiCase/UiShortcutPlusWnd.h"
#include "KMessageCentre.h"
#include "UiCase/UiTeamList.h"
#include "UiCase/UiTrafficLight.h"
#include "UiCase/UiRaid.h"
#include "UiCase/UiTaisuiWnd.h"
#include "UiCase/UiRoleFace.h"
#include "UiCase/UiTeamViewer.h"
#include "UiCase/UiGMCommunication.h"
#include "UiSheetMgr.h"
#include "UiCase/UiShortcutKeySetting.h"
#include "chatWindow/EntrustComputerDlg.h"
#include "ChatWindow/ChatMainDlg.h"
#include "UiCase/UiIEWindow.h"
#include <strstream>
#include "UiElem/TLIEWindow.h"
#include "Ui/UiCase/UiTradeBox.h"
#include "Ui/UiCase/UiBattleResult.h"
#include "Ui/UiCase/UiEntrustComputer.h"
#include "Ui/UiCase/UiQuestionWindow.h"
#include "Ui/UiCase/UiItemPassword.h"
#include "Ui/UiCase/UiGenPersonalInfo.h"
#include "Ui/UiCase/UiPointListCharts.h"

enum SCREEN_MODE
{
	SCREEN_MODE_1D = 1,
	SCREEN_MODE_2D = 2,
	SCREEN_MODE_3D = 3,
};

#define SCRIPT_SECTION "ScriptAuto"

extern iCoreShell*		g_pCoreShell;
extern iRepresentShell*	g_pRepresentShell;
#include <crtdbg.h>

/************************************************************************/
/*              KShortcutKeyCentre 内部使用的函数                       */
/************************************************************************/
static inline bool __x_isgraph( char c )
{
	return c < 0 || isgraph(c);
}

inline bool __x_memcpy_n( void* d, size_t l, const void* s, size_t n ) 
{ 
    if ( l < n )
	{
		return false; 
	}
    memcpy( d, s, n ); 
    return true; 
} 

namespace hotkey_str
{
	std::string DescHotKey(DWORD hk)
	{
		static const char* modidesc_table[] = {
			//	0		1		2		3		4		5		6		7
			"Shift",	"Ctrl",	"Alt",	"Ext",	"",		"",		"",		""
		};
		static const char* vkeydesc_table[] = {
		//	0			1			2			3			4			5			6			7
		//	8			9			A			B			C			D			E			F
			"",			"LButton",	"RButton",	"Cancel",	"MButton",	"",			"",			"",				//0
			"BackSpace","Tab",		"",			"",			"Clear",	"Enter",	"",			"",
			"",			"",			"",			"Pause",	"CapLock",	"",			"",			"",				//1
			"",			"",			"",			"ESC",		"Convert",	"NonConvert","Accept",	"ModeChange",
			"Space",	"PageUp",	"PageDown",	"End",		"Home",		"Left",		"Up",		"Right",		//2
			"Down",		"Select",	"Print",	"Execute",	"PrintScreen",	"Insert",	"Delete",	"Help",
			"0",		"1",		"2",		"3",		"4",		"5",		"6",		"7",			//3
			"8",		"9",		"",			"",			"",			"",			"",			"",
			"",			"A",		"B",		"C",		"D",		"E",		"F",		"G",			//4
			"H",		"I",		"J",		"K",		"L",		"M",		"N",		"O",
			"P",		"Q",		"R",		"S",		"T",		"U",		"V",		"W",			//5
			"X",		"Y",		"Z",		"Windows",	"",			"Menu",		"",			"",
			"Num0",		"Num1",		"Num2",		"Num3",		"Num4",		"Num5",		"Num6",		"Num7",			//6
			"Num8",		"Num9",		"Num*",		"Num+",		"Separator","Num-",		"Num.",		"Num/",
			"F1",		"F2",		"F3",		"F4",		"F5",		"F6",		"F7",		"F8",			//7
			"F9",		"F10",		"F11",		"F12",		"F13",		"F14",		"F15",		"F16",	
			"F17",		"F18",		"F19",		"F20",		"F21",		"F22",		"F23",		"F24",			//8
			"",			"",			"",			"",			"",			"",			"",			"",
			"NumLock",	"ScrollLock","",		"",			"",			"",			"",			"",				//9
			"",			"",			"",			"",			"",			"",			"",			"",	
			"",			"",			"",			"",			"",			"",			"",			"",				//A
			"",			"",			"",			"",			"",			"",			"",			"",
			"",			"",			"",			"",			"",			"",			"",			"",				//B
			"",			"",			";",		"=",		",",		"-",		".",		"/",
			"`",		"",			"",			"",			"",			"",			"",			"",				//C
			"",			"",			"",			"",			"",			"",			"",			"",
			"",			"",			"",			"",			"",			"",			"",			"",				//D
			"",			"",			"",			"[",		"\\",		"]",		"'",		"",
			"",			"",			"",			"",			"",			"",			"",			"",				//E
			"",			"",			"",			"",			"",			"",			"",			"",	
			"",			"",			"",			"",			"",			"",			"",			"",				//F
			"",			"",			"",			"",			"",			"",			"",			"",
			"LDButton", "RDButton", "MDButton", "",			"",			"",			"",			"",				//10
			"",			"",			"",			"",			"",			"",			"",			"",
		};
		static const size_t count_moditbl = sizeof(modidesc_table) / sizeof(modidesc_table[0]);
		static const size_t count_vkeytbl = sizeof(vkeydesc_table) / sizeof(vkeydesc_table[0]);


		static const char STR_DELIMITER[] = " + ";
		static const size_t LEN_DELIMITER = sizeof(STR_DELIMITER) - sizeof(STR_DELIMITER[0]);


		const WORD modi = HIWORD(hk);
		const WORD vkey = LOWORD(hk);
		if ((modi & 0xFF00) || (vkey >= count_vkeytbl))
			return "";

		const char* szVkDesc = vkeydesc_table[vkey];
		if (!szVkDesc[0])
			return "";


		std::string desc;

		{{
		for (size_t pos = 0; pos <= count_moditbl; pos++)
		{
			const char* szDesc = NULL;

			if (pos < count_moditbl)
			{
				if (!(modi & (0x01 << pos)))
					continue;
				szDesc = modidesc_table[pos];
				if (!szDesc[0])
					return "";
			}
			else
			{
				szDesc = szVkDesc;
			}

			if (!desc.empty())
				desc += STR_DELIMITER;
			desc += szDesc;
		}
		}}

		return desc;
	}

	DWORD ParseHotKey(const std::string& desc)
	{
		static const struct PATTERNMAP
		{
			enum {MASK_VKEY = 0x0000FFFF};

			typedef std::pair<DWORD, DWORD>	HOTKEYPART;
			typedef std::map<std::string, HOTKEYPART, string_iless>	DESC2HKPMAP;
			DESC2HKPMAP theMap;

			PATTERNMAP()
			{
				const struct _PATTERN
				{
					char* desc;
					DWORD mask;
					DWORD value;
				} pattern_table[] = {
					//modifier
					{"Shift", HOTKEYF_SHIFT<<16, HOTKEYF_SHIFT<<16}, {"Control", HOTKEYF_CONTROL<<16, HOTKEYF_CONTROL<<16},
					{"Alt", HOTKEYF_ALT<<16, HOTKEYF_ALT<<16}, {"Ext", HOTKEYF_EXT<<16, HOTKEYF_EXT<<16},

					//vk
					{"LButton", MASK_VKEY, VK_LBUTTON}, {"RButton", MASK_VKEY, VK_RBUTTON},
					{"Cancel", MASK_VKEY, VK_CANCEL}, {"MButton", MASK_VKEY, VK_MBUTTON},
					{"BackSpace", MASK_VKEY, VK_BACK}, {"Tab", MASK_VKEY, VK_TAB}, {"Clear", MASK_VKEY, VK_CLEAR},
					{"Return", MASK_VKEY, VK_RETURN}, {"Pause", MASK_VKEY, VK_PAUSE},
					{"Convert", MASK_VKEY, VK_CONVERT}, {"NonConvert", MASK_VKEY, VK_NONCONVERT},
					{"Accept", MASK_VKEY, VK_ACCEPT}, {"ModeChange", MASK_VKEY, VK_MODECHANGE},
					{"Escape", MASK_VKEY, VK_ESCAPE}, {"Space", MASK_VKEY, VK_SPACE},
					{"Prior", MASK_VKEY, VK_PRIOR}, {"Next", MASK_VKEY, VK_NEXT}, {"End", MASK_VKEY, VK_END}, {"Home", MASK_VKEY, VK_HOME},
					{"Left", MASK_VKEY, VK_LEFT}, {"Up", MASK_VKEY, VK_UP}, {"Right", MASK_VKEY, VK_RIGHT}, {"Down", MASK_VKEY, VK_DOWN},
					{"Insert", MASK_VKEY, VK_INSERT}, {"Delete", MASK_VKEY, VK_DELETE},
					{"Select", MASK_VKEY, VK_SELECT}, {"Print", MASK_VKEY, VK_PRINT}, {"Execute", MASK_VKEY, VK_EXECUTE},
					{"SnapShot", MASK_VKEY, VK_SNAPSHOT}, {"Help", MASK_VKEY, VK_HELP},
					{"0", MASK_VKEY, '0'}, {"1", MASK_VKEY, '1'}, {"2", MASK_VKEY, '2'}, {"3", MASK_VKEY, '3'},
					{"4", MASK_VKEY, '4'}, {"5", MASK_VKEY, '5'}, {"6", MASK_VKEY, '6'}, {"7", MASK_VKEY, '7'},
					{"8", MASK_VKEY, '8'}, {"9", MASK_VKEY, '9'},
					{"A", MASK_VKEY, 'A'}, {"B", MASK_VKEY, 'B'}, {"C", MASK_VKEY, 'C'}, {"D", MASK_VKEY, 'D'},
					{"E", MASK_VKEY, 'E'}, {"F", MASK_VKEY, 'F'}, {"G", MASK_VKEY, 'G'}, {"H", MASK_VKEY, 'H'},
					{"I", MASK_VKEY, 'I'}, {"J", MASK_VKEY, 'J'}, {"K", MASK_VKEY, 'K'}, {"L", MASK_VKEY, 'L'},
					{"M", MASK_VKEY, 'M'}, {"N", MASK_VKEY, 'N'}, {"O", MASK_VKEY, 'O'}, {"P", MASK_VKEY, 'P'},
					{"Q", MASK_VKEY, 'Q'}, {"R", MASK_VKEY, 'R'}, {"S", MASK_VKEY, 'S'}, {"T", MASK_VKEY, 'T'},
					{"U", MASK_VKEY, 'U'}, {"V", MASK_VKEY, 'V'}, {"W", MASK_VKEY, 'W'}, {"X", MASK_VKEY, 'X'},
					{"Y", MASK_VKEY, 'Y'}, {"Z", MASK_VKEY, 'Z'},
					{"Num0", MASK_VKEY, VK_NUMPAD0}, {"Num1", MASK_VKEY, VK_NUMPAD1}, {"Num2", MASK_VKEY, VK_NUMPAD2}, {"Num3", MASK_VKEY, VK_NUMPAD3},
					{"Num4", MASK_VKEY, VK_NUMPAD4}, {"Num5", MASK_VKEY, VK_NUMPAD5}, {"Num6", MASK_VKEY, VK_NUMPAD6}, {"Num7", MASK_VKEY, VK_NUMPAD7},
					{"Num8", MASK_VKEY, VK_NUMPAD8}, {"Num9", MASK_VKEY, VK_NUMPAD9},
					{"Num+", MASK_VKEY, VK_ADD}, {"Num-", MASK_VKEY, VK_SUBTRACT}, {"Num*", MASK_VKEY, VK_MULTIPLY}, {"Num/", MASK_VKEY, VK_DIVIDE},
					{"Separator", MASK_VKEY, VK_SEPARATOR}, {"Num.", MASK_VKEY, VK_DECIMAL},
					{"F1", MASK_VKEY, VK_F1}, {"F2", MASK_VKEY, VK_F2}, {"F3", MASK_VKEY, VK_F3}, {"F4", MASK_VKEY, VK_F4},
					{"F5", MASK_VKEY, VK_F5}, {"F6", MASK_VKEY, VK_F6}, {"F7", MASK_VKEY, VK_F7}, {"F8", MASK_VKEY, VK_F8},
					{"F9", MASK_VKEY, VK_F9}, {"F10", MASK_VKEY, VK_F10}, {"F11", MASK_VKEY, VK_F11}, {"F12", MASK_VKEY, VK_F12},
					{"F13", MASK_VKEY, VK_F13}, {"F14", MASK_VKEY, VK_F14}, {"F15", MASK_VKEY, VK_F15}, {"F16", MASK_VKEY, VK_F16},
					{"F17", MASK_VKEY, VK_F17}, {"F18", MASK_VKEY, VK_F18}, {"F19", MASK_VKEY, VK_F19}, {"F20", MASK_VKEY, VK_F20},
					{"F21", MASK_VKEY, VK_F21}, {"F22", MASK_VKEY, VK_F22}, {"F23", MASK_VKEY, VK_F23}, {"F24", MASK_VKEY, VK_F24},
					{"CapLock", MASK_VKEY, VK_CAPITAL}, {"NumLock", MASK_VKEY, VK_NUMLOCK}, {"ScrollLock", MASK_VKEY, VK_SCROLL},
					{";", MASK_VKEY, 0x00BA},	{"=", MASK_VKEY, 0x00BB},	{",", MASK_VKEY, 0x00BC},	{"-", MASK_VKEY, 0x00BD},
					{".", MASK_VKEY, 0x00BE},	{"/", MASK_VKEY, 0x00BF},	{"`", MASK_VKEY, 0x00C0},
					{"[", MASK_VKEY, 0x00DB},
					{"\\", MASK_VKEY, 0x00DC}, {"]", MASK_VKEY, 0x00DD}, {"'", MASK_VKEY, 0x00DE},


					//modifier alias
					{"Ctrl", HOTKEYF_CONTROL<<16, HOTKEYF_CONTROL<<16}, {"Menu", HOTKEYF_ALT<<16, HOTKEYF_ALT<<16},
					{"Break", MASK_VKEY, VK_PAUSE},

					//vk alias
					{"ESC", MASK_VKEY, VK_ESCAPE}, {"Enter", MASK_VKEY, VK_RETURN},
					{"BACK", MASK_VKEY, VK_BACK},
					{"INS", MASK_VKEY, VK_INSERT}, {"DEL", MASK_VKEY, VK_DELETE},
					{"PageUp", MASK_VKEY, VK_PRIOR}, {"PageDown", MASK_VKEY, VK_NEXT},
					{"ScrlLock", MASK_VKEY, VK_SCROLL},
					{"NumAdd", MASK_VKEY, VK_ADD}, {"NumSub", MASK_VKEY, VK_SUBTRACT}, {"NumMul", MASK_VKEY, VK_MULTIPLY}, {"NumDiv", MASK_VKEY, VK_DIVIDE},
					{"NumDecimal", MASK_VKEY, VK_DECIMAL},
					{"PrintScreen", MASK_VKEY, VK_SNAPSHOT},

					{"LDButton", MASK_VKEY, VK_LDBUTTON}, {"RDButton", MASK_VKEY, VK_RDBUTTON}, {"MDButton", MASK_VKEY, VK_MDBUTTON}, 
				};
				for (size_t i = 0; i < sizeof(pattern_table)/sizeof(pattern_table[0]); i++)
				{
					const _PATTERN& pat = pattern_table[i];
					theMap[std::string(pat.desc)] = std::make_pair(pat.mask, pat.value);
				}
			}
		} s_mapPattern;

		static const char CH_DELIMITER = '+';


		if (desc.empty())
			return 0;

		DWORD hkcode = 0;

		const char* szToken = desc.c_str(), * szLimit = NULL, * szNext = NULL;

		for ( ; *szToken; szToken = szNext)
		{
			for (szNext = NULL, szLimit = szToken; *szLimit; szLimit++)
			{
				if (*szLimit == CH_DELIMITER)
				{
					for (szNext = szLimit + 1; ; szNext++)
					{
						if (!*szNext)
						{
							szLimit ++;
							break;
						}
						if (*szNext == CH_DELIMITER)
							szLimit ++;
						else if (__x_isgraph(*szNext))
							break;
					}
					if (szLimit <= szToken)
						return 0;
					break;
				}
			}


			while (!__x_isgraph(*szToken))
			{
				szToken ++;
				if (szToken >= szLimit)
					return 0;
			}
			const char* pe = szLimit - 1;
			while (!__x_isgraph(*pe))
				pe --;
			size_t toklen = pe - szToken + 1;



			PATTERNMAP::DESC2HKPMAP::const_iterator it = s_mapPattern.theMap.find(std::string(szToken, toklen));
			if (it == s_mapPattern.theMap.end())
				return 0;
			const PATTERNMAP::HOTKEYPART& hkp = (*it).second;

			if (hkcode & hkp.first)
				return 0;
			hkcode |= hkp.second;

			if (szNext == NULL)
				break;
		}

		if (!(hkcode & PATTERNMAP::MASK_VKEY))
			return 0;

		return hkcode;
	}

} //namespace hotkey_str

size_t Compile(const char* src, char* dst, size_t dstlen) 
{ 
        static const char str_param_begin[] = " ( "; 
        static const char str_param_end[] = " ) "; 
        static const char str_param_split[] = ", "; 
 		static const char str_quote_begin[] = "\"";
		static const char str_quote_end[] = "\"";
		static const size_t len_param_begin = sizeof(str_param_begin) - sizeof(str_param_begin[0]); 
        static const size_t len_param_end = sizeof(str_param_end) - sizeof(str_param_end[0]); 
        static const size_t len_param_split = sizeof(str_param_split) - sizeof(str_param_split[0]); 
		static const char len_quote_begin = sizeof(str_quote_begin) - sizeof(str_quote_begin[0]);
		static const char len_quote_end = sizeof(str_quote_end) - sizeof(str_quote_end[0]);



        if (src == NULL || dst == NULL || dstlen <= 0) 
                return 0; 

		SHORTFUNCMAP::iterator iFun = KShortcutKeyCentre::ms_FunsMap.end();
        size_t uselen = 0; 

        size_t cntToken = 0; 
        const char* szToken = NULL; 
        const char* szNext = src - 1; 
        for ( ; ; ) 
        { 
                for (szToken = szNext + 1; *szToken; szToken++) 
                { 
                        if (__x_isgraph(*szToken)) 
                                break; 
                } 
                if (!*szToken) 
                        break; 

                for (szNext = szToken + 1; *szNext; szNext++) 
                { 
                        if (!__x_isgraph(*szNext)) 
                                break; 
                } 

                size_t toklen = szNext - szToken; 

                if (cntToken > 0) 
                {//param 
                        if (cntToken > 1) 
                        {//non first param 
                                if (!__x_memcpy_n(dst + uselen, dstlen - uselen, str_param_split, len_param_split)) 
									return 0; 
                                uselen += len_param_split; 
                        } 
						
						if (!__x_memcpy_n(dst + uselen, dstlen - uselen, str_quote_begin, len_quote_begin))
							return 0;
						uselen += len_quote_begin;
						
						if (!__x_memcpy_n(dst + uselen, dstlen - uselen, szToken, toklen))
							return 0;
						uselen += toklen;
						
						if (!__x_memcpy_n(dst + uselen, dstlen - uselen, str_quote_end, len_quote_end))
							return 0;
						uselen += len_quote_end;
                } 
                else 
                {//func name 
						iFun = KShortcutKeyCentre::ms_FunsMap.find(std::string(szToken, toklen));
						if (iFun == KShortcutKeyCentre::ms_FunsMap.end())
						{
							if (!__x_memcpy_n(dst + uselen, dstlen - uselen, szToken, toklen)) 
								return 0; 
							uselen += toklen; 
						}
						else
						{
							const std::string& name = iFun->second.strName;
							if (!__x_memcpy_n(dst + uselen, dstlen - uselen, name.c_str(), name.size())) 
								return 0; 
							uselen += name.size(); 
						}
                        

                        if (!__x_memcpy_n(dst + uselen, dstlen - uselen, str_param_begin, len_param_begin)) 
                                return 0; 
                        uselen += len_param_begin; 
                } 

                cntToken ++; 


				if (!*szNext)
					break;
        } 

        if (cntToken > 0) 
        {//is func, fill ')' 
			if (iFun != KShortcutKeyCentre::ms_FunsMap.end())
			{
				int nParamCount = cntToken - 1;
				if (iFun->second.nParamNum - nParamCount > 0)
				{	//补默认参数
					_ASSERT(iFun->second.strDefaultParam.size() == iFun->second.nParamNum);
					PARAMLIST::iterator iP = iFun->second.strDefaultParam.begin();
					for (int nSkip = nParamCount; nSkip > 0; nSkip--)
					{
						iP++;
					}

					for (int nDefault = iFun->second.nParamNum - nParamCount; nDefault > 0; nDefault--)
					{
						if (nParamCount > 0)
						{
							if (!__x_memcpy_n(dst + uselen, dstlen - uselen, str_param_split, len_param_split)) 
								return 0;
							uselen += len_param_split;
						}

						const std::string& name = (*iP);
						iP++;
						if (!__x_memcpy_n(dst + uselen, dstlen - uselen, name.c_str(), name.size())) 
							return 0; 
						uselen += name.size();

						nParamCount++;
					}
				}
			}

            if (!__x_memcpy_n(dst + uselen, dstlen - uselen, str_param_end, len_param_end)) 
				return 0;
            uselen += len_param_end; 
        }

        return uselen; 
} 

/************************************************************************/
/*					 快捷键脚本函数的实现实现			                */
/************************************************************************/

int LuaSwitchLife(Lua_State * L)
{
	g_pCoreShell->OperationRequest( GOI_SWITCH_LIFE_ROLE, NULL, NULL );
	return 0;
}

int LuaSwitchName(Lua_State * L)
{
	g_pCoreShell->OperationRequest( GOI_SWITCH_NAME_ROLE, NULL, NULL );
	return 0;
}

int LuaPrintScreen(Lua_State * L)
{
	char szPath[256];
    time_t curr = ::time(0);
    tm* localtm = ::localtime(&curr);
	::GetCurrentDirectory(256, szPath);
	::CreateDirectory( "Screenshots\\", NULL );

    sprintf(szPath,"\\Screenshots\\%d%02d%02d-%02d%02d%02d.bmp",localtm->tm_year+1900,localtm->tm_mon+1,localtm->tm_mday,localtm->tm_hour,localtm->tm_min,localtm->tm_sec);

	g_pRepresentShell->SaveScreenToFile(szPath, SCRFILETYPE_BMP, 100);

	const char* msg = KMessageCentre::GetMessage( common_message, CE_PrintScreen_Ok );
	KUiChannelCentre::GetSingleton().toSysMsg(msg);
	
	return 0;
}

int LuaAddCommand(Lua_State * L)
{
	char * strUKey = (char *)Lua_ValueToString(L, 1);
	char * strName = (char *)Lua_ValueToString(L, 2);
	char * strDo = (char *)Lua_ValueToString(L, 3);

	COMMAND_SETTING cs;
	cs.uKey = hotkey_str::ParseHotKey(strUKey);
	strncpy(cs.szCommand, strName, 31);
	cs.szCommand[31] = 0;
	strncpy(cs.szDo, strDo, 127);
	cs.szDo[127] = 0;
	KShortcutKeyCentre::AddCommand(&cs);

	return 0;
}

int LuaRemoveCommand(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 2)
		return 0;

	char * strUKey = (char *)Lua_ValueToString(L, 1);
	char * strName = (char *)Lua_ValueToString(L, 2);

	COMMAND_SETTING cs;
	cs.uKey = hotkey_str::ParseHotKey(strUKey);
	if (cs.uKey != 0)
		KShortcutKeyCentre::RemoveCommand(KShortcutKeyCentre::FindCommand(cs.uKey));
	else if (strName && strName[0] != 0)
	{
		strncpy(cs.szCommand, strName, 31);
		cs.szCommand[31] = 0;
		KShortcutKeyCentre::RemoveCommand(KShortcutKeyCentre::FindCommand(cs.szCommand));
	}
	else	//清除所有命令
	{
		KShortcutKeyCentre::RemoveCommandAll();
	}

	return 0;
}

int LuaSetLSkill(Lua_State * L)
{
	int nCount = Lua_GetTopIndex(L);
	if (nCount < 1)
		return 0;

	const char* szName = Lua_ValueToString(L, 1);

	KUiLRSkillWnd::GetSingleton().ShowSkills(true);
	KUiLRSkillWnd::Hide();
	KUiLRSkillWnd::GetSingleton().SelectSkill( szName, true );
	return 0;
}

int LuaSetRSkill(Lua_State * L)
{
	int nCount = Lua_GetTopIndex(L);
	if (nCount < 1)
		return 0;

	const char* szName = Lua_ValueToString(L, 1);

	KUiLRSkillWnd::GetSingleton().ShowSkills(false);
	KUiLRSkillWnd::Hide();
	KUiLRSkillWnd::GetSingleton().SelectSkill( szName, false );
	return 0;
}

int LuaRegisterFunctionAlias(Lua_State * L)
{
	int nCount = Lua_GetTopIndex(L);
	if (nCount < 2)
		return 0;

	char * strFunAlias = (char *)Lua_ValueToString(L, 1);
	char * strFun = (char *)Lua_ValueToString(L, 2);
	int nParam = 0;
	if (nCount >= 3)
	{
		nParam = (int)Lua_ValueToNumber(L, 3);
	}

	PARAMLIST List;
	for(int i = 4; i <= nCount; i++)
	{
		char* sDefault = (char *)Lua_ValueToString(L, i);
		if (sDefault == NULL || sDefault[0] == 0)
			List.push_back("\"\"");
		else
			List.push_back(sDefault);
	}

	KShortcutKeyCentre::RegisterFunctionAlias(strFunAlias, strFun, nParam, List);

	return 0;
}

int LuaShortcutUse(Lua_State* L)
{
	int nCount = Lua_GetTopIndex(L);
	if (nCount < 1)
		return 0;
	int nIdx = (int)Lua_ValueToNumber(L, 1);
	//bool bAlt = (bool)Lua_ValueToNumber(L, 2);
	//bool bAlt = KShortcutKeyCentre::ms_bAltPressed;

	if ( nIdx < 10 && nIdx >= 0 )
	{
		KUiShortcutWnd::Intonate( nIdx );//, bAlt );
	}
	else
	{
		KUiShortcutPlusWnd::Intonate( nIdx - 10 );
	}
	
	return 0;
}

int LuaShortcutSelect(Lua_State* L)
{
	int nCount = Lua_GetTopIndex(L);
	if (nCount < 1)
		return 0;
	int nIdx = (int)Lua_ValueToNumber(L, 1);
	bool bAlt = Lua_ValueToNumber(L, 2)  > 0 ? true : false;

	if ( nIdx < 10 && nIdx >= 0 )
	{
		KUiShortcutWnd::SelectSkill( nIdx );
	}
	else
	{
		KUiShortcutPlusWnd::SelectSkill( nIdx - 10 );
	}
	
	return 0;
}

int LuaOpenBigTeamMode(Lua_State* L)
{
	g_pCoreShell->TeamOperation(TEAM_OI_OPEN_BIG_TEAM_MODE, NULL, NULL);
	return 0;
}

int LuaLoopPKMode(Lua_State* L)
{
	int mode = (int)KUiRoleFace::GetData();
	mode++;
	while(pk_guard == mode || pk_monster == mode)
	{
		++mode;
	}
	if(pk_mode_num == mode)
	{
		mode = pk_peace;
	}
	KUiRoleFace::SetData((PK_MODE)mode);
	g_pCoreShell->OperationRequest(GOI_PK_SETTING, (unsigned int)mode, NULL);
	/*static int mode = pk_peace;
	KUiRoleFace::SetData((PK_MODE)mode);
	g_pCoreShell->OperationRequest(GOI_PK_SETTING, (unsigned int)++mode, NULL);
	while(pk_guard == mode || pk_monster == mode)
	{
		++mode;
	}
	if(pk_mode_num == mode)
	{
		mode = pk_peace;
	}//*/
	return 0;
}

int LuaReloadUiCfg(Lua_State* L)
{
	KUiCfgLoader::getSingleton().reload();
	return 0;
}

int LuaSceneMapSwitch(Lua_State* L)
{
	if ( g_LoginLogic.GetStatus() == LL_S_IN_GAME )
	{
		// 游戏中切换地图
		KUiSceneMap::getSinglton().toggle();
		if (g_pCoreShell)
		{
		Size absSize = KUiMiniMap::GetSingleton().GetParent()->getChild("TaharezLook/MiniMap/MiniMapWnd")->getAbsoluteSize();
		g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_SHOWING,
			SCENE_PLACE_MAP_ELEM_PIC | SCENE_PLACE_MAP_ELEM_CHARACTER | SCENE_PLACE_MAP_ELEM_PARTNER,
			( (int)(absSize.d_width)| ((int)(absSize.d_height) << 16)));	
		}
	}

	return 0;
}

int LuaBigMapSwitch(Lua_State* L)
{
	if(KUiBigMap::getSinglton().isVisible())
 	{
		KUiBigMap::getSinglton().hide();
 	}
 	else
 	{
		KUiBigMap::getSinglton().show();
 		KUiAdapter::UiCloseNoNpcDlg();
 	}
	return 0;
}

int LuaSwitchFullScreen(Lua_State * L)
{
	if ( g_IsFullScreen() )
	{
		CEGUI::System::getSingleton().releaseTexture();
		g_pRepresentShell->Reset( g_GetScreenWidth(), g_GetScreenHeight(), false );
		CEGUI::System::getSingleton().reCreateTexture();
//		CEGUI::System::getSingleton().ReDrawAllDxSurfaceWindow();
		KUiSheetMgr::getSinglton().redrawAllWindow();
		g_SetFullScreen( false );
	}
	else
	{
		CEGUI::System::getSingleton().releaseTexture();
		g_pRepresentShell->Reset( g_GetScreenWidth(), g_GetScreenHeight(), true );
		CEGUI::System::getSingleton().reCreateTexture();
	//	CEGUI::System::getSingleton().ReDrawAllDxSurfaceWindow();
		KUiSheetMgr::getSinglton().redrawAllWindow();
		g_SetFullScreen( true );
		ChatMainDlg::MainDlgShowWndChat(FALSE);
		KUiChannelCentre::Show();
		KUiChannelCentre::GetSingleton().showSystemFrame(true);
		KUiMiniMap::Show();
	}
	
	return 0;
}

int LuaAutoAttack(Lua_State * L)
{
/*	int nRet = g_pCoreShell->OperationRequest( GOI_AUTOATTACK_SWITCH, NULL, NULL );
	EntrustComputerDlg::EntrustDlgGetSingleton().autoAttackGroup.CheckedButtonSetAutoAttack(nRet == false ? false : true);
	KUiEntrustComputer::GetSingleton().setAutoAttackButtonState( nRet != 0 );*/
	KUiEntrustComputer::GetSingleton().CtrlAndA();
	return 0;
}

int LuaAutoPickup(Lua_State * L)
{
	g_pCoreShell->OperationRequest( GOI_AUTO_PICKUP, NULL, NULL );
	return 0;
}

void processDeleteItem()
{
	KObjAtContRegion* sourPos = (KObjAtContRegion*)KUiDragItem::GetSingleton().getObj()->getUserData();
	if (sourPos)
	{
		int itemIndex = sourPos->Obj.uId;
		g_pCoreShell->ThrowAwayItem(itemIndex);
		KUiDragItem::GetSingleton().initItem();
	}
}

int LuaMouseForceL(Lua_State * L)
{
	if ( !KShortcutKeyCentre::ms_bMouse )
	{
		return FALSE;
	}

	if ( g_pCoreShell == NULL )
	{
		return FALSE;
	}

	KUiPlayerItem SelectPlayer;
	BOOL findSelectNpc	=  FALSE;
	int nNPCKind		= -1;
	int nRelation		= relation_none;	
	int nLSkillID		= g_pCoreShell->GetGameData( GDI_LEFT_ENABLE_SKILLS, NULL, NULL );
	
	/*
	if ( nLSkillID == RUN_SKILL_ID )
	{
		findSelectNpc =  g_pCoreShell->FindSelectNPC(
											KShortcutKeyCentre::ms_MouseX, 
											KShortcutKeyCentre::ms_MouseY, 
											relation_all, 
											false, 
											&SelectPlayer, 
											nNPCKind, 
											true, 
											false );

	}
	else//*/
	{
		int  relation = relation_all;
		bool bNoPlayer = false;

		if (KUiMiniMap::showPlayer)
		{
			bNoPlayer  = true;
		}
		findSelectNpc =  g_pCoreShell->FindSelectNPC(
											KShortcutKeyCentre::ms_MouseX, 
											KShortcutKeyCentre::ms_MouseY, 
											relation, 
											true, 
											&SelectPlayer, 
											nNPCKind, 
											true, 
											false,
											bNoPlayer);

	}
		
	if (findSelectNpc)
	{		
		int nRelation = g_pCoreShell->GetNPCRelation(SelectPlayer.nIndex);
		if ( nRelation != relation_self )
		{
			if (nRelation == relation_enemy || nRelation == relation_produce )
			{				
				if ( nLSkillID >= 0 && nLSkillID != RUN_SKILL_ID )
				{
					g_pCoreShell->FollowAttack();
					return TRUE;
				}				
			}
			else if (nRelation == relation_dialog)
			{
				g_pCoreShell->FollowDialog();
				return TRUE;
			}
			else 
			{
				//g_pCoreShell->OperationRequest(GOI_FOLLOW_SOMEONE, (unsigned int)SelectPlayer.uId, 0);
				if ( nRelation == relation_ally)
				{
					if (!KUiDragItem::GetSingleton().getObj()->isEmpty())
					{
						if ( KUiDragItem::GetSingleton().getObj()->isSkill() || 
							KUiDragItem::GetSingleton().getObj()->getType() == TLGameObject::shortcut )
						{
							KUiDragItem::GetSingleton().getObj()->clear();
						}
						else
						{
							if (g_pCoreShell)
							{
								KTargetInfo _TargetInfo;
								g_pCoreShell->GetGameData( GDI_PLAYER_TARGET_INFO, (unsigned int)&_TargetInfo, NULL );
								KUiTradeBox::GetSingleton().SetIsItem(true);
								if (!KUiTradeBox::GetSingleton().isVisible())
								{
									g_pCoreShell->TradeApplyStart(_TargetInfo.nId);
								}
								else
								{
									KUiTradeBox::GetSingleton().AddItemByClickPlayer();
								}
							}
						}
					}
					KShortcutKeyCentre::MiniNaviMothedBind();
				}
				else
				{
					KUiMiniNaviation::GetSingleton().setMiniNavMouseStatus(MOUSE_NORMAL_STATUS);
				}
				return TRUE;
			}
		}
		else
		{
			findSelectNpc = FALSE;
		}
	}
	
	int nObjKind	= -1;
	int nObjIndex	= 0;
	BOOL findSelectObject = g_pCoreShell->FindSelectObject(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY, true, nObjIndex, nObjKind );
	if (findSelectObject && !findSelectNpc)
	{
		KUiAdapter::SetMouseRes(MOUSE_CURSOR_PICKCONFIRM);
		g_pCoreShell->PickupObject(nObjIndex);
		return TRUE;
	}

	if ( KUiDragItem::GetSingleton().getObj()->isEmpty() )
	{
		if (g_pCoreShell)
		{
			g_pCoreShell->GotoWhere( KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY, 0);
			g_pCoreShell->DrawMovePosition(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY);
			closeUiWnd(GAMESPACE_CLICKED);
		}
	}
	else
	{
		KObjAtContRegion* sourPos = (KObjAtContRegion*)KUiDragItem::GetSingleton().getObj()->getUserData();
		if ( sourPos )
		{
			if ( KUiDragItem::GetSingleton().getObj()->isSkill() || 
				KUiDragItem::GetSingleton().getObj()->getType() == TLGameObject::shortcut )
			{
				KUiDragItem::GetSingleton().getObj()->clear();
			}
			else
			{
				UIOBJECT_CONTAINER origin = ((KObjAtContRegion*)KUiDragItem::GetSingleton().getObj()->getUserData())->eContainer;
				if ( origin == UOC_ITEM_TAKE_WITH )
				{
					char itemName[COMMON_CLIENT_MSG_LEN_128];
					g_pCoreShell->GetGameData(GDI_GET_ITEM_NAME_WITH_COLOR_BY_INDEX, (UINT)itemName, sourPos->Obj.uId);
					KItemInfo itemInfo;
					memset(&itemInfo, 0, sizeof(itemInfo) );					
					g_pCoreShell->GetGameData(GDI_ITEM_INFO_INDEX, (unsigned int)&itemInfo, sourPos->Obj.uId );

					char deleteItemMsg[COMMON_CLIENT_MSG_LEN_256];
					if ( itemInfo.bDestory )
					{
						sprintf(deleteItemMsg, KUiCfgLoader::getSingleton().getCommonCfg().deleteItemMsg, itemName);
					}
					else
					{
						strncpy( deleteItemMsg, MSG_ITEM_CANNT_DESTORY, COMMON_CLIENT_MSG_LEN_256 );
					}					

					KUiComMsgBox::GetSingleton().setComMsgPosition();
					KUiComMsgBox::Show();
					char yesString[COMMON_CLIENT_MSG_LEN_8];
					char noString[COMMON_CLIENT_MSG_LEN_8];
					strcpy(yesString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().yesString));
					strcpy(noString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().noString));
					KUiComMsgBox::GetSingleton().setBtnName((utf8*)yesString, (utf8*)noString);
					KUiComMsgBox::GetSingleton().setLayoutMsg(deleteItemMsg);
					
					KUiComMsgBox::GetSingleton().setFristBtnCallback(processDeleteItem);
				}
			}
		}
	}
	
	KUiMiniNaviation::GetSingleton().setMiniNavMouseStatus(MOUSE_NORMAL_STATUS);

	return 0;
}

int LuaMouseForceR(Lua_State * L)
{
	if ( !KShortcutKeyCentre::ms_bMouse )
	{
		return FALSE;
	}

	if ( g_pCoreShell == NULL )
	{
		return FALSE;
	}

	KUiPlayerItem SelectPlayer;
	int nNPCKind		= -1;
	BOOL findSelectNpc	= FALSE;
	int nRSkillID		= g_pCoreShell->GetGameData( GDI_RIGHT_ENABLE_SKILLS, NULL, NULL );

	/*
	if ( nRSkillID == RUN_SKILL_ID )
	{
		findSelectNpc = g_pCoreShell->FindSelectNPC(
										KShortcutKeyCentre::ms_MouseX,
										KShortcutKeyCentre::ms_MouseY,
										relation_all,
										false,
										&SelectPlayer,
										nNPCKind,
										true,
										false );

	}
	else//*/
	{		
		int  relation = relation_all;
		bool bNoPlayer = false;

		if (KUiMiniMap::showPlayer)
		{
			bNoPlayer = true;
		}
		findSelectNpc = g_pCoreShell->FindSelectNPC(
										KShortcutKeyCentre::ms_MouseX,
										KShortcutKeyCentre::ms_MouseY,
										relation,
										true,
										&SelectPlayer,
										nNPCKind,
										true,
										false ,
										bNoPlayer);

	}
		
	if (findSelectNpc)
	{		
		int nRelation = g_pCoreShell->GetNPCRelation(SelectPlayer.nIndex);
		if ( nRelation != relation_self )
		{
			if (nRelation == relation_enemy || nRelation == relation_produce || nRelation == relation_ally)
			{
				
				if ( nRSkillID >= 0 )
				{
					if ( nRSkillID == RUN_SKILL_ID )
					{
						if ( KUiDragItem::GetSingleton().getObj()->isEmpty() )
						{
							if (g_pCoreShell)
							{
								g_pCoreShell->GotoWhere( KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY, 0);
								g_pCoreShell->DrawMovePosition(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY);
								closeUiWnd(GAMESPACE_CLICKED);
							}
						}
					}
					else
					{
						g_pCoreShell->NextSkill( nRSkillID );
					}
					
				}				
				return TRUE;
			}
			else if (nRelation == relation_dialog)
			{
				g_pCoreShell->FollowDialog();
				return TRUE;
			}
		}
		else
		{
			findSelectNpc = FALSE;
		}
	}
	else
	{
		int nRSkillID = g_pCoreShell->GetGameData( GDI_RIGHT_ENABLE_SKILLS, NULL, NULL );
		if ( nRSkillID > 0 )
		{
			g_pCoreShell->NextSkill( nRSkillID );
		}				
		return FALSE;
	}	

	return FALSE;
}

int LuaAutoSelect(Lua_State * L)
{	
	KUiPlayerItem SelectPlayer;
	int nNPCKind = -1;

	::POINT point;
	::GetCursorPos( &point );
	::ScreenToClient( g_GetMainHWnd(), &point );

	if ( g_pCoreShell->FindSelectNPC( 
						point.x, 
						point.y, 
						relation_all, 
						false, 
						&SelectPlayer, 
						nNPCKind, 
						true) )
	{
		g_pCoreShell->SelectNPC( SelectPlayer.nIndex );
	}
	else
	{
		g_pCoreShell->AutoSelectNPC( relation_all, point.x, point.y );
	}

	return 0;
}

int LuaTargeMenu(Lua_State *L)
{
	KUiPlayerItem SelectPlayer;
	int nNPCKind = -1;
	if ( g_pCoreShell )
	{
		if (g_pCoreShell->FindSelectNPC(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY, relation_all, true, &SelectPlayer, nNPCKind, true))
		{
			int nRelation = g_pCoreShell->GetNPCRelation(SelectPlayer.nIndex);
			if ( nNPCKind == kind_player )
			{
				KTargetInfo	tagTargetInfo;
				g_pCoreShell->GetGameData( GDI_PLAYER_TARGET_INFO, (unsigned int)&tagTargetInfo, NULL );
	
				KUiPlayerMenu::Hide();
				
				Point pos(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY);
				if ( pos.d_y > g_GetScreenHeight() / 2)
				{
					KUiPlayerMenu::GetSingleton().setPos(Point(pos.d_x, pos.d_y - KUiPlayerMenu::GetSingleton().GetWinHeight()));
				}
				else
				{
					KUiPlayerMenu::GetSingleton().setPos(pos);
				}
				KUiPlayerMenu::GetSingleton().show(tagTargetInfo.strName, tagTargetInfo.nId, tagTargetInfo.nTeamId, tagTargetInfo.nPrivateState > 0);
			}
		}
	}
	return 0;
}

int LuaFirstLogin(Lua_State * L)
{
	/*if ( !KUiBeginHelp::GetSingleton().IsVisible() )
	{
		KUiBeginHelp::GetSingleton().RepeatShow();
	}
	else
	{
		KUiBeginHelp::GetSingleton().Hide();
	}//*/
	return 0;
}

int LuaEsc(Lua_State * L)
{
	if ( !KUiAdapter::EscHideDialog() )
	{
		KUiExit::Show();
	}
	g_pCoreShell->LockSomeoneAction(0);
	g_pCoreShell->Stop();
	
	return 0;
}

int LuaHideUi(Lua_State * L)
{
	KUiAdapter::HideUi();
	return 0;
}

int LuaShowFPS(Lua_State * L)
{
	g_bShowFPS = !g_bShowFPS;
	return 0;
}

int LuaSwitchChannel(Lua_State * L)
{
	if (IsWindowVisible(ChatMainDlg::hMainDlg))
	{
		return 0;
	}

	KUiChatRoom* pRoom = KUiChatCentre::GetSingleton().getActiveChatRoom();
	if ( pRoom && pRoom->IsVisible() )
	{
		return 0;
	}
	else
	{
		if(KUiChatInputWnd::GetSingleton().isCurInput())
		{
		//	KUiChatInputWnd::Hide();
			KUiChatInputWnd::GetSingleton().doSendMessage();
		}
		else
		{
			KUiChatInputWnd::GetSingleton().show();
		}
	}

	return 0;
}

int LuaBeginSpeakToLastSender(Lua_State * L)
{
	//先找到当前焦点窗口
	Window* capWnd = NULL;
	
	Window* modalTarget = System::getSingleton().getModalTarget();
	Window* activeSheet = KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT);
	if (modalTarget == NULL)
	{
		capWnd = activeSheet->getActiveChild();
	}
	else
	{
		capWnd = modalTarget->getActiveChild();
		if (capWnd == NULL)
		{
			capWnd = modalTarget;
		}
	}

	//如果存在且是editbox则不影响其输入
	if(capWnd != NULL && capWnd->getName() == TLEditbox::WidgetTypeName)
	{
		return 0;
	}

	KUiChatInputWnd::GetSingleton().showLastSender();
	return 0;
}

int LuaBeginSpeakToLastReciver(Lua_State * L)
{
	//先找到当前焦点窗口
	Window* capWnd = NULL;
	
	Window* modalTarget = System::getSingleton().getModalTarget();
	Window* activeSheet = KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT);
	if (modalTarget == NULL)
	{
		capWnd = activeSheet->getActiveChild();
	}
	else
	{
		capWnd = modalTarget->getActiveChild();
		if (capWnd == NULL)
		{
			capWnd = modalTarget;
		}
	}

	//如果存在且是editbox则不影响其输入
	if(capWnd != NULL && capWnd->getName() == TLEditbox::WidgetTypeName)
	{
		return 0;
	}

	KUiChatInputWnd::GetSingleton().showLastReciver();
	return 0;
}

int LuaSwitchWindow(Lua_State * L)
{
	int nCount = Lua_GetTopIndex(L);
	if (nCount < 1)
		return 0;
	if(KUiSceneMap::getSinglton().isVisible())
	{
		KUiSceneMap::getSinglton().hide();
	}
	
	const char* name = Lua_ValueToString(L, 1);	
	if(strcmp( name, "TaharezLook/Equipment") == 0 )
	{
		if(KUiEquipment::GetSingleton().IsVisible() == false)
		{
			KUiEquipment::Show();
		}
		else
		{
			KUiEquipment::Hide();
		}
		return 0;//*/
	}
	else if ( strcmp( name, "TaharezLook/IEWindow") == 0 )
	{
		if (KUiIEWindow::IsVisible() == false )
		{
			KUiIEWindow::Show();
		}
		else
		{
			KUiIEWindow::Hide();
		}
		return 0;
	}
	else if(strcmp( name, "TaharezLook/ItemBox") == 0)
	{
		if(KUiItemBox::getSingleton().isVisible() == false)
			KUiItemBox::getSingleton().show();
		else
			KUiItemBox::getSingleton().hide();
		return 0;
	}
	else if(strcmp( name, "TaharezLook/SkillManage") == 0)
	{
		if(KUiStudySkillManage::GetSingleton().IsVisible() == false)
			KUiStudySkillManage::Show();
		else
			KUiStudySkillManage::Hide();
		return 0;
	}
	else if(strcmp( name, "TaharezLook/TongManager") == 0)
	{
		SocietyInfoIndex tagSocietyIdx;
		tagSocietyIdx.TemplateId	= enSUTplId_Tong;
		int nTopLayer = 0;
		g_pCoreShell->GetGameData( GDI_GET_SOCIETY_PLAYER, (unsigned int)&tagSocietyIdx, (int)&nTopLayer );
		if ( nTopLayer > 0 )
		{
			if(KUiTongManager::GetSingleton().IsVisible() == false)
				KUiTongManager::Show();
			else
				KUiTongManager::Hide();
			return 1;
		}
		else
		{
			const char* message = KMessageCentre::GetMessage(tong_operation_message, 24);
			KUiChannelCentre::GetSingleton().toSysMsg(message);
			return 0;
		}
	}
	else if(strcmp( name, "TaharezLook/Talisman") == 0)
	{
		KUiTalisman::getSingleton().toggle();
		return 0;
	}
	else if(strcmp( name, "TaharezLook/MailCentre") == 0)
	{
		if(KUiMailCentre::GetSingleton().IsVisible() == false)
			KUiMailCentre::Show();
		else
			KUiMailCentre::Hide();
		return 0;
	}
	else if(strcmp( name, "TaharezLook/ChatCentre") == 0)
	{
		if(KUiChatCentre::GetSingleton().IsVisible() == false)
			KUiChatCentre::Show();
		else
			KUiChatCentre::Hide();
		return 0;
	}
	else if(strcmp( name, "TaharezLook/Compound") == 0)
	{
		if(KUiCompound::GetSingleton().IsVisible() == false)
			KUiCompound::Show();
		else
			KUiCompound::Hide();
		return 0;
	}
	else if(strcmp( name, "TaharezLook/itemvendueshop") == 0)
	{
		if(KUiVendueWnd::GetSingleton().IsVisible() == false)
			KUiVendueWnd::Show();
		else
			KUiVendueWnd::Hide();
		return 0;
	}
	else if(strcmp( name, "TaharezLook/HelpFrame") == 0)
	{
		//zhangxin080418 KUiHelpInfo窗口已经不在使用，功能合并到封神宝典中
		/*if(KUiHelpInfo::GetSingleton().IsVisible() == false)
			KUiHelpInfo::Show();
		else
			KUiHelpInfo::Hide();*/
		return 0;
	}
	else if(strcmp( name, "TaharezLook/QuestManage") == 0)
	{
		if(KUiQuestManage::GetSingleton().IsVisible() == false)
			KUiQuestManage::GetSingleton().show();
		else
			KUiQuestManage::Hide();
		return 0;
	}
	else if(strcmp( name, "TaharezLook/GameSetting") == 0)
	{
		if(KUiGameSetting::GetSingleton().IsVisible() == false)
			KUiGameSetting::Show();
		else
			KUiGameSetting::Hide();
		return 0;
	}
	else if (strcmp(name,"TaharezLook/TeamList") == 0 )
	{
		if (KUiTeamList::GetCanShow())
		{
			KUiTeamList::SetCanShow(false);
			KUiTeamList::Hide();
		}//endif
		else
		{
			KUiTeamList::SetCanShow(true);
			KUiTeamList::Show();
		}//end else
	}
	else if (strcmp(name,"TaharezLook/ShortcutPlusWnd")==0)
	{
        if (KUiShortcutPlusWnd::IsShowing())
		{
			KUiShortcutPlusWnd::Hide();
		}
		else
			KUiShortcutPlusWnd::Show();
	}
	else if (strcmp(name, "TaharezLook/NpcNavigation") ==0 )
	{
		KUiNpcNavigation::getSingleton().toggle();
	}
	else if (strcmp(name, "TaharezLook/GM") ==0 )
	{
		KUiGMCommunication::getSingleton().toggle();
	}
	else if (strcmp(name, "TaharezLook/FSBible") ==0 )
	{
        KUiFSBible::getSingleton().toggle();
	}
	else if (strcmp(name, "TaharezLook/FSBibleToday") ==0 )
	{
        KUiFSBible::getSingleton().toggleToday();
	}
	else if (strcmp(name, "TaharezLook/FSBibleQuest") ==0 )
	{
        KUiFSBible::getSingleton().toggleQuest();
	}
	else if ( strcmp(name, "TaharezLook/HidePlayer") ==0 )
	{
		EventArgs args;
		KUiMiniMap::GetSingleton().onClickHidePlayer(args);
	}
	else if ( strcmp(name, "TaharezLook//TeamView") ==0 )
	{
		KUiTeamViewer::GetSingleton().ToggleVisibility();
	}
	else if ( strcmp(name, "TaharezLook/EntrustComputer") ==0 )
	{
		KUiEntrustComputer::GetSingleton().toggle();
	}
	else if ( strcmp(name, "TaharezLook/UiQuestionWindow") ==0 )
	{
		KUiQuestionWindow::GetSingleton().Toggle();
	}
	else if ( strcmp(name, "TaharezLook/ItemPassword") ==0 )
	{
		KUiItemPassword::GetSingleton().ToggleVisibility();
	}
	else if ( strcmp(name, "TaharezLook/ItemPassword_Create") ==0 )
	{
		KUiItemPassword_Create::GetSingleton().ToggleVisibility();
	}
	else if ( strcmp(name, "TaharezLook/ItemPassword_Modify") ==0 )
	{
		KUiItemPassword_Modify::GetSingleton().ToggleVisibility();
	}
	else if ( strcmp( name, "TaharezLook/SmallBattleFieldResult" ) ==0 )
	{
		KUiSmallBattleFieldResult::GetSingleton().ToggleVisibility();
	}
	else if ( strcmp( name, "TaharezLook/TongStatueMsg" ) ==0 )
	{
		KUiTongStatueMsg::GetSingleton().ToggleVisibility();
	}
	else if ( strcmp(name, "TaharezLook/GenPersonalInfo") ==0 )
	{
		KUiGenPersonalInfo::GetSingleton().ToggleVisibility();
	}
	//Add by DarkMagic(DuanMu)
	else if ( strcmp(name, "TaharezLook/Charts") ==0 )
	{
		KUiPointListCharts::GetSingleton().ToggleVisibility();
	}

/*	else if (strcmp(name, "TaharezLook/BattleResult") == 0)
	{
		if (KUiBattleResult::GetSingleton().IsVisible())
		{
			KUiBattleResult::Hide();
		}
		else
		{
			KUiBattleResult::Show();
		}
	}*/
	return 0;
}

// int	LuaSetDefaultSkill(Lua_State * L)
// {	
// 	// 空格键设置默认技能
// 	g_pCoreShell->SwitchDefaultSkill();
// 
// 	return 0;
// }

int LuaMove(Lua_State * L)
{
	int nCount = Lua_GetTopIndex(L);
	if (nCount < 1)
		return 0;

	int direction = (int)Lua_ValueToNumber(L,1);
	g_pCoreShell->Move(direction, 50);

	return 1;
}
/*
#ifdef _DEBUG  //TaisuiSys debug brianyao 2007
int LuaWheelTianGan(Lua_State * L)
{
    g_pCoreShell->OperationRequest(GOI_WHEEL_TIAN_GAN,0,0);
	return 0;
}

int LuaWheelDiZhi(Lua_State * L)
{
   g_pCoreShell->OperationRequest(GOI_WHEEL_DI_ZHI,0,0);
   return 0;
}

int LuaDropChance(Lua_State * L)
{
   g_pCoreShell->OperationRequest(GOI_DROP_CHANCE,0,0);
   return 0;
}

#endif
*/

#ifdef _DEBUG
int LuaKeyUpTaisuiDialog(Lua_State * L)
{
	if (!KTaisuiWnd::IsVisible())
	{
		KTaisuiWnd::Show();
	}
	return 0;
}
#endif

int LuaSwithChatBackground(Lua_State * L)
{
	static int i = 1;
	KUiChannelCentre::GetSingleton().showBackground(i++ % 2 == 0 ? false : true);
	
	return 0;
}

int LuaSwithRaidWindow(Lua_State * L)
{
	KUiRaid::getSinglton().toggle();
	
	return 0;
}

int LuaListTeam(Lua_State * L)
{
	g_pCoreShell->OperationRequest( GOI_LIST_TEAM, NULL, NULL );
	return 0;
}

static TLua_Funcs GameScriptFuns[] = 
{
	{"AddCommand",				LuaAddCommand			},	//char* szKey, char* szName, char* szScript
	{"RemoveCommand",			LuaRemoveCommand		},	//char* szKey, char* szName
	{"RegisterFunctionAlias",	LuaRegisterFunctionAlias},	//char * strFunAlias, char * strFun, [int nParam], [Paramlist...]
	{"SetLSkill",				LuaSetLSkill			},
	{"SetRSkill",				LuaSetRSkill			},
	{"ShortcutUse",				LuaShortcutUse			},	
	{"SceneMapSwitch",			LuaSceneMapSwitch		},	
	{"BigMapSwitch",			LuaBigMapSwitch			},	
	{"MouseForceL",				LuaMouseForceL			},	
	{"MouseForceR",				LuaMouseForceR			},	
	{"Esc",						LuaEsc					},	
	{"HideUi",					LuaHideUi				},	
	{"ShowFPS",					LuaShowFPS				},	
	{"SwitchChannel",			LuaSwitchChannel		},
	{"ShowLastSender",			LuaBeginSpeakToLastSender},
	{"ShowLastReciver",			LuaBeginSpeakToLastReciver},	
	{"SwitchWindow",			LuaSwitchWindow			},
	{"AutoSelect",				LuaAutoSelect			},
	{"AutoAttack",				LuaAutoAttack			},
	{"TargetMenu",				LuaTargeMenu			},
	{"FirstLogin",				LuaFirstLogin			},
	{"SwitchLife",				LuaSwitchLife			},
	{"SwitchName",				LuaSwitchName			},
	{"SwitchFullScreen",		LuaSwitchFullScreen		},
	{"PrintScreen",				LuaPrintScreen			},
	{"Move",					LuaMove					},
	{"AutoPickup",				LuaAutoPickup			},
	{"ShortcutSelect",			LuaShortcutSelect		},
	{"SwithChatBackground",		LuaSwithChatBackground	},	
	{"SwithRaidWindow",			LuaSwithRaidWindow		},	
	{"ShortcutSelect",			LuaShortcutSelect		},
	{"OpenBigTeamMode",			LuaOpenBigTeamMode		},
#ifdef _DEBUG
	{"OpenTaisuiDlg",           LuaKeyUpTaisuiDialog     },
#endif
	{"LoopPKMode",				LuaLoopPKMode			},
	{"ReloadUiCfg",				LuaReloadUiCfg			},
	{"ListTeam",				LuaListTeam				},
};

static int g_GetGameScriptFunNum()
{
	return sizeof(GameScriptFuns)  / sizeof(TLua_Funcs);
}


/************************************************************************/
/*                   KShortcutKeyCentre 实现                            */
/************************************************************************/

KLuaScript			KShortcutKeyCentre::ms_Script;
SHORTFUNCMAP		KShortcutKeyCentre::ms_FunsMap;
COMMAND_SETTING*	KShortcutKeyCentre::ms_pCommands			= NULL;
int					KShortcutKeyCentre::ms_nCommands			= 0;
bool				KShortcutKeyCentre::ms_bMouse				= false;
int					KShortcutKeyCentre::ms_MouseX				= 0;
int					KShortcutKeyCentre::ms_MouseY				= 0;
int					KShortcutKeyCentre::ms_nLBtnPressedCounter	= 0;
int					KShortcutKeyCentre::ms_nAutoRunMode			= KShortcutKeyCentre::enNone;
bool				KShortcutKeyCentre::ms_bLBtnPressed			= false;
bool				KShortcutKeyCentre::ms_bAltPressed			= false;
bool				KShortcutKeyCentre::ms_bMouseInWnd			= false;

int	KShortcutKeyCentre::HandleKeyInput( unsigned int uKey, int nModifier )
{
	if ( g_LoginLogic.GetStatus() != LL_S_IN_GAME )
	{
		return false;
	}
	if ( uKey == VK_PROCESSKEY )
	{
		uKey = ImmGetVirtualKey( g_GetMainHWnd() );
	}
	int nIndex = FindCommand( MAKELONG( uKey, nModifier ) );
	if ( nIndex >= 0 )
	{
		return ExecuteScript( ms_pCommands[nIndex].szDo );
	}

	return false;
}

int	KShortcutKeyCentre::HandleMouseInput( unsigned int uKey, int nModifier, int x, int y )
{
	if ( g_LoginLogic.GetStatus() != LL_S_IN_GAME )
	{
		return false;
	}
	if ( ms_bMouse )
	{
		return false;
	}

	ms_bMouse = true;
	ms_MouseX = x;
	ms_MouseY = y;
	int nIndex = FindCommand( MAKELONG( uKey, nModifier ) );
	int nRet = false;
	if ( nIndex >= 0 )
	{
		nRet = ExecuteScript( ms_pCommands[nIndex].szDo );
	}

	ms_bMouse = false;

	return nRet;
}

bool KShortcutKeyCentre::InitScript( void )
{
	if (ms_Script.Init() && ms_Script.RegisterFunctions(GameScriptFuns, g_GetGameScriptFunNum()))
	{
		KUiSKSettingMgr::getSinglton().loadUserSettings();
		KUiSKSettingMgr::getSinglton().save();
		string scriptPath = KUiSKSettingMgr::getSinglton().getCurValidSKPath();
		return LoadScript(const_cast<char*>(scriptPath.c_str()));
	}

	return false;
}

bool KShortcutKeyCentre::LoadScript(char* pFileName)
{
	ClearScript();

	return ms_Script.Load(pFileName) > 0 ? true:false;
}

bool KShortcutKeyCentre::UninitScript( void )
{
	return ClearScript();
}

bool KShortcutKeyCentre::ClearScript( void )
{
	if (ms_pCommands)
		free(ms_pCommands);
	ms_pCommands = NULL;
	ms_nCommands = 0;

	ms_FunsMap.clear();

	return true;
}



bool KShortcutKeyCentre::LoadPrivateSetting(KIniFile* pFile)
{
	return false;
}


bool KShortcutKeyCentre::SavePrivateSetting(KIniFile* pFile)
{
	return false;
}

bool KShortcutKeyCentre::TranslateExcuteScript(const char * ScriptCommand)
{
	if (ScriptCommand && ScriptCommand[0] != 0)
	{
		int nIndex = FindCommand(ScriptCommand);	//首先寻找快捷键带的名称
		if (nIndex >= 0)
		{
			if (ms_pCommands[nIndex].szDo[0] != 0)
				return ExecuteScript(ms_pCommands[nIndex].szDo);
		}
		else//翻译通常语法为严格语法
		{
			char sztrueCommand[512];
			sztrueCommand[0] = 0;
			//语法转换和函数名转换
			int nLen = Compile(ScriptCommand, sztrueCommand, 511);
			sztrueCommand[nLen] = 0;
			return ExecuteScript(sztrueCommand);
		}
	}

	return false;
}

bool KShortcutKeyCentre::ExecuteScript(const char * ScriptCommand)
{
	if (ScriptCommand && ScriptCommand[0] != 0)
	{
		if ((ScriptCommand[0] != '-' || ScriptCommand[1] != '-') &&
			ScriptCommand[0] != '#')
		{
            if (ms_Script.LoadBuffer((PBYTE)ScriptCommand, strlen(ScriptCommand)))
				return ms_Script.ExecuteCode() > 0 ? true : false;
		}
	}

	return false;
}

bool KShortcutKeyCentre::ExcuteHWNDScript(const char * ScriptCommand)
{
	if (ScriptCommand && ScriptCommand[0] != 0)
	{
		if (ms_Script.LoadBuffer((PBYTE)ScriptCommand, strlen(ScriptCommand)))
		{
			return ms_Script.ExecuteCode() > 0 ? true : false;
		}
	}

	return false;
}

int KShortcutKeyCentre::AddCommand(COMMAND_SETTING* pAdd)	//复制Add数据并增加到Commands中，如果uKey!=0则覆盖原uKey，否则如szCommand[0]!=0则覆盖szCommand相同者
{
	int nIndex = -1;
	if (pAdd == NULL)
		return nIndex;
	if (pAdd->uKey != 0)
	{
		nIndex = FindCommand(pAdd->uKey);
	}
	else if (pAdd->szCommand[0] != 0)
	{
		nIndex = FindCommand(pAdd->szCommand);
	}
	else
		return nIndex;

	if (nIndex >= 0)
	{
		ms_pCommands[nIndex] = *pAdd;
	}
	else
	{
		if (ms_pCommands == NULL)
		{
			ms_pCommands = (COMMAND_SETTING*)malloc(sizeof(COMMAND_SETTING));
			ms_pCommands[0] = *pAdd;
			nIndex = 0;
			ms_nCommands = 1;
		}
		else
		{
			ms_pCommands = (COMMAND_SETTING*)realloc(ms_pCommands, sizeof(COMMAND_SETTING) * (ms_nCommands + 1));
			ms_pCommands[ms_nCommands] = *pAdd;
			nIndex = ms_nCommands;
			ms_nCommands++;
		}
	}
	return nIndex;
}

int	KShortcutKeyCentre::RemoveCommand(int nIndex)	//返回剩余Command的总数
{
	if (ms_pCommands && nIndex >= 0 && nIndex < ms_nCommands)
	{
		if (nIndex != ms_nCommands - 1)
		{
			memmove(ms_pCommands + nIndex, ms_pCommands + nIndex + 1, sizeof(COMMAND_SETTING));
		}

		ms_nCommands--;
	}

	return ms_nCommands;
}

void KShortcutKeyCentre::RemoveCommandAll()
{
	ms_nCommands = 0;
}

int	KShortcutKeyCentre::FindCommand( DWORD uKey )
{
	if ( uKey == 0 )
	{
		return -1;
	}
	for ( int i = 0; i < ms_nCommands; ++i )
	{
		if ( ms_pCommands[i].uKey != 0 && ms_pCommands[i].uKey == uKey )
		{
			return i;
		}
	}
	return -1;
}

int	KShortcutKeyCentre::FindCommand( const char* szCommand )
{
	if ( szCommand == NULL || szCommand[0] == 0 )
	{
		return -1;
	}
	for ( int i = 0; i < ms_nCommands; ++i )
	{
		if ( ms_pCommands[i].szCommand[0] != 0 && strcmpi(ms_pCommands[i].szCommand, szCommand) == 0 )
		{
			return i;
		}
	}
	return -1;
}

int	KShortcutKeyCentre::FindCommandByScript( const char* szScript )
{
	if ( szScript == NULL || szScript[0] == 0 )
	{
		return -1;
	}
	for ( int i = 0; i < ms_nCommands; i++ )
	{
		if ( ms_pCommands[i].szDo[0] != 0 && strcmpi(ms_pCommands[i].szDo, szScript) == 0 )
		{
			return i;
		}
	}
	return -1;
}

DWORD KShortcutKeyCentre::GetCommandKey(int nIndex)
{
	if (nIndex >= 0 && nIndex < ms_nCommands)
	{
		return ms_pCommands[nIndex].uKey;
	}
	return 0;
}

const char*	KShortcutKeyCentre::GetKeyName(DWORD Key)
{
	static std::string s_descHK;

	s_descHK = hotkey_str::DescHotKey(Key);
	return s_descHK.c_str();
}

bool KShortcutKeyCentre::RegisterFunctionAlias(const char * strFunAlias, const char * strFun, int nParam, const PARAMLIST& List)
{
	if (strFunAlias && strFunAlias[0] != 0 &&
		strFun && strFun[0] != 0)
	{
		ShortFuncInfo info;
		info.strName = strFun;
		info.nParamNum = nParam;
		info.strDefaultParam = List;
		if (info.nParamNum >= (int)info.strDefaultParam.size())
		{
			for (int i = info.nParamNum - info.strDefaultParam.size(); i > 0; i--)
			{
				info.strDefaultParam.push_back("\"\"");
			}
		}
		else
		{
			for (int i = info.strDefaultParam.size() - info.nParamNum; i > 0; i--)
			{
				info.strDefaultParam.pop_back();
			}
		}
		_ASSERT(info.nParamNum == info.strDefaultParam.size());

		ms_FunsMap[strFunAlias] = info;
		return true;
	}

	return false;
}

int KShortcutKeyCentre::MouseMove( int x, int y, bool bLbutton)
{
	if (g_pCoreShell == NULL)
		return 0;

	int nObjKind = -1;
	int nObjIdx = -1;
	KUiPlayerItem SelectPlayer;
	ZeroMemory( &SelectPlayer, sizeof(KUiPlayerItem) );
	int nNPCKind = -1;

	int  relation      = relation_all;
	bool bNoPickPlayer = false;
	if (KUiMiniMap::showPlayer)
		bNoPickPlayer  = true;

	//如果当前是修理状态
	switch(KUiPlayerState::getSingleton().getState())
	{
	case KUiPlayerState::TRADE_NPC_NORMAL_REPAIR:
		{
			KUiAdapter::SetMouseRes( MOUSE_CURSOR_REPAIR );
		}
		return 0;
	case KUiPlayerState::TRADE_NPC_SPECIAL_REPAIR:
		{
			KUiAdapter::SetMouseRes( MOUSE_CURSOR_REPAIR_PLUS );
		}
		return 0;
	}

	if ( g_pCoreShell->FindSelectObject(x, y, false,nObjIdx, nObjKind ) && !ms_bMouseInWnd )
	{
		KUiAdapter::SetMouseRes( MOUSE_CURSOR_PICK );
	}
	else if ( g_pCoreShell->FindSelectNPC(x, y, relation, false, &SelectPlayer, nNPCKind, false , bNoPickPlayer) &&
			KUiAdapter::GetMouseRes() != MOUSE_CURSOR_TARGETITEM )
	{
		if ( SelectPlayer.nIndex > 0  && !ms_bMouseInWnd)
		{
			int nRelation = g_pCoreShell->GetNPCRelation(SelectPlayer.nIndex);

			switch( nRelation )
			{
			case relation_ally:
				KUiAdapter::SetMouseRes( MOUSE_CURSOR_FRIEND );
				break;
			case relation_dialog:
				KUiAdapter::SetMouseRes( MOUSE_CURSOR_DIALOG );
				break;
			case relation_produce:
				KUiAdapter::SetMouseRes( MOUSE_CURSOR_NORMAL );
				break;
			case relation_enemy:
				KUiAdapter::SetMouseRes( MOUSE_CURSOR_FIGHT );
				break;
			default:
				KUiAdapter::SetMouseRes( MOUSE_CURSOR_NORMAL );
				break;
			}
		}
		else
		{
			if ( KUiAdapter::GetMouseRes() != MOUSE_CURSOR_ITEM_LOCK_BY_DATA && 
				MOUSE_SUPER_LINK_POSITION != KUiAdapter::GetMouseRes() )
			{
				KUiAdapter::SetMouseRes( MOUSE_CURSOR_NORMAL );
			}			
		}
	}
	else
	{
		//likun
		MOUSE_CURRENT_STATUS CurrentStatus = KUiMiniNaviation::GetSingleton().getMiniNavMouseSatatus();
		switch( CurrentStatus )
		{
			case MOUSE_FRIEND_STATUS:
			{
				KUiAdapter::SetMouseRes( MOUSE_CURSOR_ADDFRIEND );
				break;
			}
			case MOUSE_TRADE_STATUS:
			{
				KUiAdapter::SetMouseRes( MOUSE_CURSOR_TRADE );
				break;
			}
			case MOUSE_LOOK_EQUIPMENT:
			{
				KUiAdapter::SetMouseRes( MOUSE_CURSOR_VIEW );
				break;
			}
			case MOUSE_BLACK_LIST:
			{
				KUiAdapter::SetMouseRes( MOUSE_CURSOR_SCREEN );
				break;
			}
			case MOUSE_TEAM_STATUS:
			{
				KUiAdapter::SetMouseRes( MOUSE_CURSOR_TEAM );
				break;
			}
			case MOUSE_CHAT_STATUS:
			{
				KUiAdapter::SetMouseRes( MOUSE_CURSOR_CHAT );
				break;
			}
			case MOUSE_FOLLOW_STATUS:
			{
				KUiAdapter::SetMouseRes( MOUSE_CURSOR_FOLLOW );
				break;
			}
			case MOUSE_EDIT_STATUS:
			{
				KUiAdapter::SetMouseRes( MOUSE_CURSOR_EDIT );
				break;
			}
			default:
			{
				if ( (KUiAdapter::GetMouseRes() < MOUSE_CURSOR_TARGETITEM ||
					KUiAdapter::GetMouseRes() > MOUSE_SUPER_LINK_NPC) && 
					KUiAdapter::GetMouseRes() != MOUSE_CURSOR_ITEM_LOCK_BY_DATA)
				{
					KUiAdapter::SetMouseRes( MOUSE_CURSOR_NORMAL );
				}
				
				break;
			}
		}
	}

	if (bLbutton
		&& KUiDragItem::GetSingleton().getObj()->isEmpty()
		&& !KUiTradeBox::GetSingleton().IsTrading())
	{
		if (g_pCoreShell && !g_pCoreShell->FindSelectNPC(x, y, relation_enemy, false, &SelectPlayer, nNPCKind, false ))
		{
			g_pCoreShell->GotoWhere(x, y, 0,true);
		}
	}

	return 0;
}


void	KShortcutKeyCentre::MiniNaviMothedBind()
{
	KUiPlayerItem SelectPlayer;
	KTargetInfo   PlayerInfo;
	int nNPCKind = -1;
	int nOldTarget = g_pCoreShell->GetTargetNPC();

	if (g_pCoreShell != NULL  &&
		g_pCoreShell->FindSelectNPC(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY, relation_all, true, &SelectPlayer, nNPCKind, true))
	{
		g_pCoreShell->GetGameData(GDI_GET_PLAYER_INFO_BY_NPCID, (UINT)(&PlayerInfo), SelectPlayer.uId);
		MOUSE_CURRENT_STATUS CurrentStatus = KUiMiniNaviation::GetSingleton().getMiniNavMouseSatatus();
		switch( CurrentStatus )
		{
			case MOUSE_FRIEND_STATUS:
			{
				g_pCoreShell->OperationRequest(GOI_CHAT_FRIEND_ADD, (unsigned int)(PlayerInfo.strName), CHAT::GROUPID_NONE );
				break;
			}
			case MOUSE_TRADE_STATUS:
			{
				if ( PlayerInfo.nIndex != -1 )
				{
					g_pCoreShell->TradeApplyStart(PlayerInfo.nId);
				}
				break;
			}
			case MOUSE_LOOK_EQUIPMENT:
			{
				if (  PlayerInfo.nIndex != -1 )
				{
					g_pCoreShell->OperationRequest(GOI_VIEW_PLAYERITEM, (UINT)PlayerInfo.nId, NULL);
				}
				break;
			}
			case MOUSE_BLACK_LIST:
			{
				break;
			}
			case MOUSE_TEAM_STATUS:
			{
				KUiPlayerItem tagPlayer;
				if ( PlayerInfo.strName )
				{
					strncpy( tagPlayer.Name, PlayerInfo.strName, CLIENT_NAME_AND_TITLE_MAX + 1);
				}
				tagPlayer.nData = 0;
				tagPlayer.nIndex = PlayerInfo.nIndex;
				tagPlayer.nParam = 0;
				tagPlayer.uId = PlayerInfo.nId;

				KUiPlayerTeam	TeamInfo;
				TeamInfo.cNumMember = 0;
				g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)&TeamInfo, 0);
				if ( ((int)(TeamInfo.cNumMember)) <= MAX_TEAMMEMBER_COUNT )
				{
					if (TeamInfo.cNumMember == 0)
					{
						g_pCoreShell->TeamOperation(TEAM_OI_CREATE, 0, 0);
					}
					g_pCoreShell->TeamOperation( TEAM_OI_INVITE, (unsigned int)&tagPlayer, NULL );
				}
				else
				{
					const char* msg = KMessageCentre::GetMessage(team_message, 1);
					KUiChannelCentre::GetSingleton().toSysMsg(msg);
				}
				break;
			}
			case MOUSE_CHAT_STATUS:
			{
				KUiChatInputWnd::GetSingleton().clearText();
				KUiChatInputWnd::GetSingleton().write("/");
				KUiChatInputWnd::GetSingleton().write(PlayerInfo.strName);
				KUiChatInputWnd::GetSingleton().write(" ");
				KUiChatInputWnd::GetSingleton().show();	
				break;
			}
			case MOUSE_FOLLOW_STATUS:
			{
				if ( PlayerInfo.nIndex != -1 )
				{
					g_pCoreShell->OperationRequest(GOI_FOLLOW_SOMEONE, (unsigned int)PlayerInfo.nId, NULL );
				}
				break;
			}
			default:
			{
				break;
			}
		}
		KUiAdapter::SetMouseRes(MOUSE_CURSOR_NORMAL);
		KUiMiniNaviation::GetSingleton().setMiniNavMouseStatus(MOUSE_NORMAL_STATUS);
	}
}


//是否鼠标在窗口上
void	KShortcutKeyCentre::SetIsMouseInWnd( bool bInWnd )
{
	ms_bMouseInWnd = bInWnd;
}

void	KShortcutKeyCentre::LoadUpdataShortcutKey( const std::string& path )
{
	KFile selfFile;
	if ( !selfFile.Open((char*)path.c_str()) )
	{
		selfFile.Close();
		return;
	}

	char* buff = new char[selfFile.Size()];
	if ( buff == NULL )
	{
		return;
	}
	selfFile.Read( buff, selfFile.Size() );
	selfFile.Close();
	
	std::strstream dataSys;
	dataSys<<buff;
	if ( buff )
	{
		delete[] buff;
		buff = NULL;
	}

	char aLine[COMMON_CLIENT_MSG_LEN_512];

	std::vector<std::string> selfList;
	while( dataSys.getline(aLine, COMMON_CLIENT_MSG_LEN_512) )
	{

		std::string Key;
		std::string CmdName;
		std::string Cmd;
		std::string Sys;

		aLine[COMMON_CLIENT_MSG_LEN_512 - 1] = 0;
		std::string strLine = aLine;
		
		int startIndex = strLine.find('\"');
		int endIndex = strLine.find('\"', startIndex + 1);
		Key = strLine.substr(startIndex + 1, endIndex - startIndex - 1);

		startIndex = strLine.find('\"', endIndex + 1);
		endIndex = strLine.find('\"', startIndex + 1);
		CmdName = strLine.substr(startIndex + 1, endIndex - startIndex - 1);
		
		startIndex = strLine.find('\"', endIndex + 1);
		endIndex = strLine.find('\"', startIndex + 1);
		Cmd = strLine.substr(startIndex + 1, endIndex - startIndex - 1);

		startIndex = strLine.find('\"', endIndex + 1);
		endIndex = strLine.find('\"', startIndex + 1);
		Sys = strLine.substr(startIndex + 1, endIndex - startIndex - 1);



		if ( !Sys.empty() && 
			( Sys == "true" || 
			Sys == "TRUE" || 
			Sys == "True" ) )
		{
			// do nothings.
		}
		else
		{
			selfList.push_back( strLine );
		}
	}
	//*/

	char szCurDir[256];
	::GetCurrentDirectory(256, szCurDir);
	std::string curPath = szCurDir;
	curPath += "\\";
	curPath += path;

	::DeleteFile( curPath.c_str() );	

	KPakFile sysFile;
	if ( !sysFile.Open( UI_SHORTCUT_KEY_DEFAULT_FILE_NAME ) )
	{
		return;
	}
	buff = new char[sysFile.Size()];
	if ( buff == NULL)
	{
		return;
	}
	sysFile.Read(buff,sysFile.Size());
	sysFile.Close();

	std::strstream sysSelf;
	sysSelf<<buff;
	if ( buff )
	{
		delete[] buff;
		buff = NULL;
	}

	std::map<std::string,std::string> sysList;
	while( sysSelf.getline(aLine, COMMON_CLIENT_MSG_LEN_512) )
	{

		std::string Key;
		std::string CmdName;
		std::string Cmd;
		std::string Sys;

		aLine[COMMON_CLIENT_MSG_LEN_512 - 1] = 0;
		std::string strLine = aLine;
		
		int startIndex = strLine.find('\"');
		int endIndex = strLine.find('\"', startIndex + 1);
		Key = strLine.substr(startIndex + 1, endIndex - startIndex - 1);

		startIndex = strLine.find('\"', endIndex + 1);
		endIndex = strLine.find('\"', startIndex + 1);
		CmdName = strLine.substr(startIndex + 1, endIndex - startIndex - 1);
		
		startIndex = strLine.find('\"', endIndex + 1);
		endIndex = strLine.find('\"', startIndex + 1);
		Cmd = strLine.substr(startIndex + 1, endIndex - startIndex - 1);

		startIndex = strLine.find('\"', endIndex + 1);
		endIndex = strLine.find('\"', startIndex + 1);
		Sys = strLine.substr(startIndex + 1, endIndex - startIndex - 1);

		if ( !Sys.empty() && 
			( Sys == "true" || 
			Sys == "TRUE" || 
			Sys == "True" ) )
		{
			if ( sysList.find(Key) == sysList.end() )
			{
				sysList[Key] = strLine;
			}			
		}
	}
	
	//*/

    HANDLE hFile = ::CreateFile( 
						path.c_str(), 
						GENERIC_WRITE, 
						FILE_SHARE_WRITE, 
						NULL, 
						CREATE_ALWAYS,
						FILE_ATTRIBUTE_NORMAL,
						NULL );

    if (hFile!=INVALID_HANDLE_VALUE)
    {
      ::CloseHandle(hFile);

		std::fstream newFile;
		newFile.open(curPath.c_str(), ios::binary|ios::out);
		if ( newFile.is_open() )
		{
			std::map<std::string,std::string>::iterator it = sysList.begin();
			while ( it != sysList.end() )
			{
				newFile.write( it->second.c_str(), it->second.length() );
				newFile.write( "\n", 1 );
				++it;
			}

			std::vector<std::string>::iterator its = selfList.begin();
			while ( its != selfList.end() )
			{
				newFile.write( its->c_str(), its->length() );
				newFile.write( "\r\n", 2 );
				++its;
			}

			newFile.close();
		}
	}	
}

