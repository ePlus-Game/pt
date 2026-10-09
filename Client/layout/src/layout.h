/////////////////////////////////////////////////////////////////////////////
//  FileName    :   layout.cpp
//  Creator     :   zuolizhi
//  Date        :   2006-12-13 9:54:00
//  Comment     :   Interface Declare
//	Changes		:	
/////////////////////////////////////////////////////////////////////////////

#ifndef _LAYOUT_H_
#define _LAYOUT_H_

#include "layoutdef.h"
#include "layoutinterface.h"
#include "LayoutElement.h"
#include "list"
#include "LayoutArea.h"
#include <vector>

/////////////////////////////////////////////////////////////////////////////
//
//              Class Define
//
/////////////////////////////////////////////////////////////////////////////

class Layout : public ILayout
{
	int					_layoutId;

	ILayoutRender*		_render;		//render由外部模块来实现

	LayoutData			_dataList;		//绘制数据

	LayoutArea			_area;			//排版区域

	bool				_needRelayout;	//是否从新排版

	LORect				_clipper;		//裁减区域
	
	int					_selectionStart;//选择区域开始
	int					_selectionEnd;	//选择区域结尾
	
	bool				_showCarat;		//是否显示光标
	
	LayoutElement		_carat;

	int					_wordBorderMode;
protected:
	void	processLayout();
	LORect	processASeg(LayoutSeg& seg);
	int		processAline(LayoutSeg& seg, int startElemIndex, const LORect& freeArea, LORect& usedArea);
	void	insertElem(LayoutElement& elem);

	void	splitText(const wchar_t* text, std::vector<wchar_t*>& outTexts, wchar_t spliter);
	void	getSubstr(const wchar_t* text, int spliteIndex, wchar_t*& subText1, wchar_t*& subText2);
	void	getSubstr(const wchar_t* text, int spliteIndex, int len, wchar_t*& subText);

	bool	canElemsCombination(LayoutElement& elem1, LayoutElement& elem2);
	LayoutElement* getNextElem(int segIndex, int elemIndex);
	void	omitSubStr(wchar_t* text, int startIndex, int endIndex);
	void	getPosition(int pos, int& segIndex, int& elemIndex, int& wordIndex);
public:
	Layout(ILayoutRender* render);
	~Layout();
	void	Release();

	void	SetText(char* szText);
	LORect	getRenderArea(bool adjWidth = false);

	void	Render(int x, int y, float z);

	int		wordIndexAtPixel(int x, int y);
	void	setClipper(LORect& clipper);
	bool	pickupElem(int x, int y, LOElemInfo& elemInfo);
	void	setSelection(int stratIndex, int endIndex);
	void	getSelection(int& stratIndex, int& endIndex);
	void	eraseSelection();
	void	clearLayout();
	LOPoint	getPosAtWordIndex(int wordIndex);
	void	showCarat(bool carat, bool select);
	bool	isHaveContent();
	void	setColor(LOColor newColor, float alpha = 1.0f);
	void	setAlpha(float alpha);
	void	insertElem(LOElemInfo& newElem);
	void	flashLayout();
	int		getElemList(LOElemInfo*& elems);
	int		getWordCount();
	void	formatText(char* text);
	void	setBorderMode(int mode);
};

#endif