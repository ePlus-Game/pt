//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 08/04/2006 16:13
//      File_base        : ShortcutKey
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef SHORTCUTKEY_H
#define SHORTCUTKEY_H

#pragma warning(disable:4786)

#include "string"
#include "map"
#include "list"
#include "tchar.h"
#include "commctrl.h"
#include "KLuaScript.h"

#define VK_LDBUTTON						0x0100
#define VK_RDBUTTON						0x0101
#define VK_MDBUTTON						0x0102

struct COMMAND_SETTING
{
	DWORD uKey;				//第一关键字
	char szCommand[32];		//第二关键字,当第一关键字为0时有效
	char szDo[128];
};


struct string_less
{
	bool operator() ( const std::string& src1, const std::string& src2 ) const
	{
		const size_t len1 = src1.size();
		const size_t len2 = src2.size();
		if (len1 < len2)
		{
			return true;
		}
		if (len1 > len2)
		{
			return false;
		}
		return _tcscmp(src1.c_str(), src2.c_str()) < 0;
	}
};
struct string_iless
{
	bool operator() ( const std::string& src1, const std::string& src2 ) const
	{
		const size_t len1 = src1.size();
		const size_t len2 = src2.size();
		if (len1 < len2)
		{
			return true;
		}
		if (len1 > len2)
		{
			return false;
		}
		return _tcsicmp(src1.c_str(), src2.c_str()) < 0;
	}
};

typedef std::list<std::string> PARAMLIST;

struct ShortFuncInfo
{
	std::string		strName;
	int				nParamNum;
	PARAMLIST		strDefaultParam;
};

typedef std::map<std::string, ShortFuncInfo, string_iless> SHORTFUNCMAP;

class KIniFile;

class KShortcutKeyCentre
{
public:
	friend class KUiGameSpace;
	friend class KUiShortcutPlusWnd;
	friend class KUiShortcutWnd;
	friend size_t Compile(const char* src, char* dst, size_t dstlen);
	friend int LuaMouseForceL(Lua_State * L);
	friend int LuaMouseForceR(Lua_State * L);
	friend int LuaTargeMenu(Lua_State * L);
	friend int LuaAutoSelect(Lua_State * L);

	enum enAutoRunMode
	{
		enNone,
		enPrepare,
		enAutoRun,
	};

public:
	static int			HandleKeyInput			(	unsigned int uKey, int nModifier												);
	static int			HandleMouseInput		(	unsigned int uKey, int nModifier, int x, int y									);
	static int			MouseMove				(	int x, int y, bool bLbutton 													);

	static bool			InitScript				(	void																			);
	static bool			LoadScript				(	char* pFileName																	);
	static bool			ClearScript				(	void																			);
	static bool			UninitScript			(	void																			);

	static bool			LoadPrivateSetting		(	KIniFile* pFile																	);
	static bool			SavePrivateSetting		(	KIniFile* pFile																	);

	static bool			ExecuteScript			(	const char * ScriptCommand														);
	static int			AddCommand				(	COMMAND_SETTING* pAdd															);	
	static int			RemoveCommand			(	int nIndex																		);	
	static int			FindCommand				(	DWORD uKey																		);
	static int			FindCommand				(	const char* szCommand															);
	static int			FindCommandByScript		(	const char* szScript															);
	static DWORD		GetCommandKey			(	int nIndex																		);
	static const char*	GetKeyName				(	DWORD Key																		);
	static bool			TranslateExcuteScript	(	const char* ScriptCommand														);
	static bool			RegisterFunctionAlias	(	const char* strFunAlias, const char * strFun, int nParam, const PARAMLIST& List	);
	static void			RemoveCommandAll		(	void																			);
	static bool			ExcuteHWNDScript		(	const char* ScriptCommand														);
	static void			MiniNaviMothedBind( void );
	static void			SetIsMouseInWnd( bool bInWnd );
	static void			LoadUpdataShortcutKey( const std::string& path );
private:
	
private:
	static int				ms_MouseX;
	static int				ms_MouseY;
	static bool				ms_bMouse;
	static int				ms_nLBtnPressedCounter;
	static int				ms_nAutoRunMode;
	static bool				ms_bLBtnPressed;
	static KLuaScript		ms_Script;
	static SHORTFUNCMAP		ms_FunsMap;
	static COMMAND_SETTING* ms_pCommands;
	static int				ms_nCommands;
	
	static bool				ms_bAltPressed;
	//likun 是否鼠标在窗口上
	static bool				ms_bMouseInWnd;
};

#endif