//xiehong 2007-8-20		根窗口管理器
//用于管理根窗口，以及多个根窗口之间的切换


#ifndef UI_SHEET_MGR
#define UI_SHEET_MGR

#include "uicommon.h"
#include <vector>


class KUiSheetMgr
{
	std::vector<Window*>	_sheetList;
public:
	KUiSheetMgr();
	~KUiSheetMgr();

	Window* createNewSheet(char* name);
	int getSheetCount();
	Window* find(char* name);
	Window* find(int index);
	Window* switchSheet(int index);
	Window* switchSheet(char* name);
	void    redrawAllWindow();

	static KUiSheetMgr& getSinglton()
	{
		static KUiSheetMgr singlton;
		return singlton;
	}
protected:
	
private:
};

class KUiSheetRefCounter
{
	std::map<std::string, int>	_refList;
public:
	void	addRef(std::string name);
	bool	removeRef(std::string name);

	static KUiSheetRefCounter& getSinglton()
	{
		static KUiSheetRefCounter singlton;
		return singlton;
	}
};
#endif