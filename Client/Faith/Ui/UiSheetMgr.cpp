
#include "UiSheetMgr.h"

using namespace std;
KUiSheetMgr::KUiSheetMgr()
{
	
}

KUiSheetMgr::~KUiSheetMgr()
{

}

Window* KUiSheetMgr::createNewSheet(char* name)
{
	if(find(name) != NULL)
	{
		return NULL;
	}
	
	Window* sheet = WindowManager::getSingleton().createWindow(DefaultWindow::WidgetTypeName, name);
	_sheetList.push_back(sheet);
	return sheet;
}

Window* KUiSheetMgr::find(char *name)
{
	for(int i = 0; i < _sheetList.size(); ++i)
	{
		Window* sheet = _sheetList[i];
		if(!strcmp(sheet->getName().c_str(), name))
		{
			return sheet;
		}
	}
	return NULL;
}

Window* KUiSheetMgr::find(int index)
{
	if(index < 0 || index > _sheetList.size())
	{
		return NULL;
	}

	return _sheetList[index];
}

int KUiSheetMgr::getSheetCount()
{
	return _sheetList.size();
}

void KUiSheetMgr::redrawAllWindow()
{
	Window* pWindow = KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT);
	if(pWindow)
	{
		uint child_count = pWindow->getChildCount();
		
		for (uint i = 0; i < child_count; ++i)
		{
			Window* panelWindow = pWindow->getChildAtIdx(i);
			if(!panelWindow->getRenderMode())
				panelWindow->requestRedraw();
		}
	}
}
Window* KUiSheetMgr::switchSheet(char* name)
{
	Window* activeSheet = find(name);
	if(!activeSheet)
	{
		return NULL;
	}

	System::getSingleton().setGUISheet(activeSheet);
	return activeSheet;
}



void KUiSheetRefCounter::addRef(string name)
{
	if(_refList.find(name) != _refList.end())
	{
		++_refList[name];
	}
	else
	{
		_refList[name] = 1;
	}
}

bool KUiSheetRefCounter::removeRef(string name)
{
	if(_refList.find(name) == _refList.end())
	{
		return true;
	}
	
	if(_refList[name] > 0)
	{
		--_refList[name];
	}
	if(0 == _refList[name])
	{
		return true;
	}

	return false;
}