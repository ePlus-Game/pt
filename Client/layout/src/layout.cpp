/////////////////////////////////////////////////////////////////////////////
//  FileName    :   layout.cpp
//  Creator     :   zuolizhi
//  Date        :   2006-12-13 9:54:00
//  Comment     :   Interface Declare
//	Changes		:	
/////////////////////////////////////////////////////////////////////////////
#include <crtdbg.h>

#include <windows.h>

#include "layout.h"
#include "layoutparser.h"

using namespace std;

wchar_t layoutTempText1[LAYOUT_TEXT_MAX_LEN];
wchar_t layoutTempText2[LAYOUT_TEXT_MAX_LEN];

void resetTempText()
{
	layoutTempText1[0] = 0;
	layoutTempText2[0] = 0;
}

Layout::Layout(ILayoutRender* render)
{
// 	static int layoutId = 0;
// 	_layoutId = layoutId++;
	
	_render = render;
	
	_dataList.clear();
		
	_needRelayout = false;

	_selectionStart = 0;
	_selectionEnd	= 0;
	
	_clipper = LORect(0, 0, 1024, 100000);

	_showCarat = false;

	//光标
// 	_carat.setProperty("type=pic");
// 	_carat.setProperty("vertical-align=center");
// 	_carat.setProperty("color=255,255,255");
// 	_carat.setContent("set:TaharezLook image:EditBoxCarat");
	_carat.setProperty("type=text");
	_carat.setContent("_");
	_carat.setArea(_render->getWordSize('_', _carat.getFont()));

	_wordBorderMode = true;
}

Layout::~Layout()
{
	Release();
}

void Layout::SetText(char* layoutText )
{
//      	layoutText =	"<Layout width=200>"
//       					"<Seg t-a=center f=left l-e=10 w-e=10>"
//       						"<Obj c=ff00ff f-f=LiBian-16 ul=true>协作等级描述了</Obj>"
//       						"<Obj c=ff0000 f-f=SongTi-10>DirectDraw</Obj>"
//       						"<Obj c=0000ff>如何同显示交互</Obj>"
//       					"</Seg>"
//       					"<Seg t-a=center f=wrap l-e=10 w-e=1>"
//       						"<Obj c=00ff00 v-a=bottom>因为显示硬件的不同，并不是所有的设备都支持所有的显示模式</Obj>"
//       						"<Obj type=pic v-a=bottom>set:bagua image:bagua1_normal</Obj>"
//       						"<Obj c=0f0f0f v-a=top>因为显示硬件的不同，并不是所有的设备都支持所有的显示模式</Obj>"
//       					"</Seg>"
//       					"<Seg t-a=right f=none>"
//       						"<Obj type=pic>set:bagua image:bagua1_normal</Obj>"
//       						"<Obj c=f0f0f0>你可以调用IDirectDraw2::SetDisplayMode方法来设置显示模式</Obj>"
//       					"</Seg>"
//       				"</Layout>";
	_dataList.clear();

	LayoutParser& parser = LayoutParser::Singleton();
	parser.ParseText( layoutText, _dataList );

	_dataList.inheritsProperty();
	
	_needRelayout = true;
}

void Layout::clearLayout()
{
	_dataList.clear();
		
	_needRelayout = false;

	_selectionStart = 0;
	_selectionEnd	= 0;
	
	_showCarat = false;
	
	_area.setArea(LORect(0, 0, _dataList.getWidth(), _dataList.getHeight()));
	
	_carat.setArea(_render->getWordSize('_', _carat.getFont()));
}

void Layout::splitText(const wchar_t* text, vector<wchar_t*>& outTexts, wchar_t spliter)
{
	int textLen = wcslen(text);
	int endIndex = 0;
	int startIndex = 0;
	while(endIndex < textLen)
	{
		if(text[endIndex] == spliter || endIndex == textLen - 1)
		{
			int subTextLen = endIndex - startIndex + 1;
			wchar_t* subText = new wchar_t[subTextLen + 1];
			for(int i = 0; i < subTextLen; ++i)
			{
				subText[i] = text[i + startIndex];
			}
			subText[subTextLen] = 0;
			outTexts.push_back(subText);
						
			startIndex = endIndex + 1;
		}
		++endIndex;
	}
}

void Layout::getSubstr(const wchar_t* text, int spliteIndex, wchar_t*& subText1, wchar_t*& subText2)
{
	resetTempText();
	subText1 = layoutTempText1;
	subText2 = layoutTempText2;
	
	int textLen = wcslen(text);
	
	if(textLen >= LAYOUT_TEXT_MAX_LEN)
	{
		_ASSERT(0);
		return;
	}

	if(textLen < spliteIndex)
	{
		_ASSERT(0);
		return;
	}

	for(int i = 0; i < textLen; ++i)
	{
		if(i < spliteIndex)
		{
			subText1[i] = text[i];
		}
		else
		{
			subText2[i - spliteIndex] = text[i];
		}
	}
	subText1[spliteIndex] = 0;
	subText2[textLen - spliteIndex] = 0;
}

void Layout::getSubstr(const wchar_t* text, int spliteIndex, int len, wchar_t*& subText)
{
	resetTempText();
	subText = layoutTempText1;

	int textLen = wcslen(text);
	if(textLen >= LAYOUT_TEXT_MAX_LEN)
	{
		_ASSERT(0);
		return;
	}

	if(textLen < spliteIndex + len)
	{
		_ASSERT(0);
		return;
	}

	for(int i = 0; i < len; ++i)
	{
		subText[i] = text[i + spliteIndex];
	}
	subText[len] = 0;
}

void Layout::omitSubStr(wchar_t* text, int startIndex, int endIndex)
{
	if(startIndex >= endIndex || startIndex < 0 || endIndex < 0)
	{
		_ASSERT(0);
		return;
	}

	int textLen = wcslen(text);
	for(int i = endIndex; i <= textLen; ++i)
	{
		text[startIndex++] = text[i];
	}
}

void Layout::eraseSelection()
{
	if(_selectionStart == _selectionEnd)
		return;

	int selectionStart = _selectionStart < _selectionEnd ? _selectionStart : _selectionEnd;
	int selectionEnd = _selectionEnd > _selectionStart ? _selectionEnd : _selectionStart;
	int curWordIndex = 0;

	vector<LayoutSeg>& segs = _dataList.getSegs();
	for(int segIndex = 0; segIndex < segs.size(); ++segIndex)
	{
		vector<LayoutElement>& elems = segs[segIndex].getElems();

		for(int elemIndex = 0; elemIndex < elems.size(); ++elemIndex)
		{
			if(selectionStart >= selectionEnd)
				break;

			LayoutElement& elem = elems[elemIndex];
			int thisElemWordCount = elem.getWordCount();

			if(selectionStart >= curWordIndex && selectionStart < curWordIndex + thisElemWordCount)
			{
				int start = selectionStart - curWordIndex;
				int end = thisElemWordCount;
				if(selectionEnd > curWordIndex && selectionEnd <= curWordIndex + thisElemWordCount)
				{
					end = selectionEnd - curWordIndex;
				}
				if(end - start == thisElemWordCount)
				{
					elems.erase(elems.begin() + elemIndex);
					elemIndex--;
				}
				else
				{
					omitSubStr(const_cast<wchar_t*>(elem.getContent()), start, end);
				}
				selectionStart = curWordIndex + thisElemWordCount;
			}
			curWordIndex += thisElemWordCount;
		}
	}

	if(_selectionEnd > _selectionStart)
		_selectionEnd = _selectionStart;
	else
		_selectionStart = _selectionEnd;
		
	setSelection(_selectionStart, _selectionEnd);

	_needRelayout = true;
}

void Layout::formatText(char* text)
{
	if(NULL == text)
	{
		return;
	}

	int textLen = strlen(text);

	int index = 0;
	for(int i = 0; i < textLen; ++i)
	{
		if('\\' == text[i] && 'n' == text[i + 1])
		{
			text[index] = '\n';
			i += 1;
		}
		else if(index != i)
		{
			text[index] = text[i];
		}
		++index;
	}
	text[index] = 0;
}

void Layout::getPosition(int pos, int& segIndex, int& elemIndex, int& wordIndex)
{
	if(pos < 0)
	{
		pos = 0;
	}
	vector<LayoutSeg>& segs = _dataList.getSegs();

	int curIndex = 0;
	for(int i = 0; i < segs.size(); ++i)
	{
		vector<LayoutElement>& elems = segs[i].getElems();
		for(int j = 0; j < elems.size(); ++j)
		{
			LayoutElement& elem = elems[j];
			int wordCount = elem.getWordCount();
			//注：有时候一个pos既可以是一个elem的结尾也可以是下一个elem的开头
			//该总查找方法，除非是第一个elem，其他找到的wordIndex一定是elem结尾先于下一个elem开头找到
			if(pos >= curIndex && pos <= curIndex + wordCount)
			{
				segIndex = i;
				elemIndex = j;
				wordIndex = pos - curIndex;
				return;
			}
			curIndex += wordCount;
		}
	}

	//如果超出范围则选择最后一个seg的最后一个elem的最后一个word的后一个位置
	segIndex = i - 1;
	vector<LayoutElement>& elems = segs[segIndex].getElems();
	elemIndex = elems.size() - 1;
	LayoutElement& elem = elems[elems.size() - 1];
	wordIndex = elem.getWordCount() - 1;
}

bool Layout::canElemsCombination(LayoutElement& elem1, LayoutElement& elem2)
{
	if(elem1.getElemType() != LO_TEXT
		|| elem2.getElemType() != LO_TEXT
		|| elem1.getShowDes()
		|| elem2.getShowDes())
		return false;

	if(strcmp(elem1.getFont().family, elem2.getFont().family) != 0)
		return false;

	return true;
}

LayoutElement* Layout::getNextElem(int segIndex, int elemIndex)
{
	vector<LayoutSeg>& segs = _dataList.getSegs();
	for(int i = segIndex; i < segs.size(); ++i)
	{
		vector<LayoutElement>& elems = segs[i].getElems();
		for(int j = elemIndex + 1; j < elems.size(); ++j)
		{
			return &elems[j];
		}
	}

	return NULL;
}

void Layout::insertElem(LayoutElement& newElem)
{
	//当前必须有一个seg
	if(_dataList.getSegs().size() == 0)
	{
		return;
	}

	//当可以正常插入时，先清空选中区域
	eraseSelection();

	//如果当前没有elem，则直接插入
	if(_dataList.getSegs()[0].getElems().size() == 0)
	{
		_dataList.getSegs()[0].getElems().push_back(newElem);
	}
	else
	{
		int caratPos = _selectionEnd;
		
		int segIndex = 0;
		int elemIndex = 0;
		int wordIndex = 0;
		
		getPosition(caratPos, segIndex, elemIndex, wordIndex);
		
		LayoutSeg& curSeg = _dataList.getSegs()[segIndex];
		LayoutElement& curElem = curSeg.getElems()[elemIndex];
		
		
		bool combin = false;
		if(canElemsCombination(curElem, newElem))
		{
			curElem.extentContent(const_cast<wchar_t*>(newElem.getContent()), wordIndex);
			combin = true;
		}
		else if(wordIndex == curElem.getWordCount())
		{
			LayoutElement* nextElem = getNextElem(segIndex, elemIndex);
			if(nextElem != NULL && canElemsCombination(*nextElem, newElem))
			{
				nextElem->extentContent(const_cast<wchar_t*>(newElem.getContent()), 0);
				combin = true;
			}
		}
		
		if(false == combin)
		{
			if(0 == wordIndex)
			{
				curSeg.getElems().insert(curSeg.getElems().begin() + elemIndex, newElem);
			}
			else if(wordIndex == curElem.getWordCount())
			{
				curSeg.getElems().insert(curSeg.getElems().begin() + elemIndex + 1, newElem);
			}
			else
			{
				wchar_t* subText1 = NULL;
				wchar_t* subText2 = NULL;
				getSubstr(curElem.getContent(), wordIndex, subText1, subText2);
				
				curElem.setContent(subText1);
				LayoutElement splitElem = curElem;
				splitElem.setContent(subText2);
				curSeg.getElems().insert(curSeg.getElems().begin() + elemIndex + 1, splitElem);
				curSeg.getElems().insert(curSeg.getElems().begin() + elemIndex + 1, newElem);
			}
		}
	}
	_selectionEnd += newElem.getWordCount();
	setSelection(_selectionEnd, _selectionEnd);
	
	_needRelayout = true;
}

void Layout::insertElem(LOElemInfo& newElem)
{
	LayoutElement elem;
	elem.setElem(newElem);
	insertElem(elem);
}

void Layout::processLayout()
{
	_area.setArea(LORect(0, 0, _dataList.getWidth(), _dataList.getHeight()));
	
	//处理margin
	LORect marginArea;
	//左
	_area.useArea(LORect(0, 0, _dataList.getMarginL(), _dataList.getHeight()));
	//右
	_area.useArea(LORect(_dataList.getWidth() - _dataList.getMarginR(), 0, _dataList.getMarginR(), _dataList.getHeight()));
	//上
	_area.useArea(LORect(0, 0, _dataList.getWidth(), _dataList.getMarginT()));

	LORect curRect;
	
	vector<LayoutSeg>& segs = _dataList.getSegs();
	
	for(int i = 0; i < segs.size(); ++i)
	{
		LORect maskArea = processASeg(segs[i]);
		if(segs[i].getFLoat() == FLOAT_LEFT)
		{
			maskArea.setWidth(_dataList.getWidth());
		}
		else if(segs[i].getFLoat() == FLOAT_RIGHT)
		{
			int width = maskArea.getRight();
			maskArea.setPos(0, maskArea.getTop());
			maskArea.setWidth(width);
		}
		else if(segs[i].getFLoat() == FLOAT_WRAP)
		{
			maskArea.setPos(0, maskArea.getTop());
			maskArea.setWidth(_dataList.getWidth());
		}
		_area.useArea(maskArea);
	}

	_needRelayout = false;
	
	int maxHeight = _dataList.getHeight();
	int actHeight = _dataList.getActRenderHeight();

	if(actHeight <= maxHeight)
	{
		return;
	}
}

LORect Layout::processASeg(LayoutSeg& seg)
{
	LayoutArea segArea = _area;

	vector<LayoutElement>& elemList = seg.getElems();

	for(int i = 0; i < elemList.size(); ++i)
	{		
		if(elemList[i].getElemType() == LO_TEXT)
		{
			const wchar_t* contentText = elemList[i].getContent();
			vector<wchar_t*> subTexts;
			splitText(contentText, subTexts, '\n');

			if(subTexts.size() > 1)
			{
				for(int j = 0; j < subTexts.size(); ++j)
				{
					LayoutElement newElem = elemList[i];
					newElem.setContent(subTexts[j]);
					elemList.insert(elemList.begin() + i++, newElem);
				}

				elemList.erase(elemList.begin() + i);
				i--;
			}
			
			for(int k = 0; k < subTexts.size(); ++k)
			{
				wchar_t* subText = subTexts[k];
				delete[] subText;
				subText = NULL;
			}
			subTexts.clear();
		}
		else if(elemList[i].getElemType() == LO_IMAGE)
		{
			elemList[i].setImageInfo(_render->getImageInfo(elemList[i].getContent()));
		}

		if(elemList[i].isHaveBackImage())
		{
			elemList[i].setBackImage(_render->getImageInfo(elemList[i].getBackImagePath()));
		}

		if(elemList[i].isHaveFrontImage())
		{
			elemList[i].setFrontImage(_render->getImageInfo(elemList[i].getFrontImagePath()));
		}
	}
	
	int lineTop = 0;
	int lineBottom = 0;
	
	int startIndex = 0;
	int endIndex = 0;

	while(endIndex < elemList.size())
	{
		if(segArea.peek() == false)
		{
			//排版区域已经分配完，后面的不显示了
			break;
		}
		const LORect freeAreaH = segArea.getAUnusedAreaH();
		const LORect freeAreaV = segArea.getAUnusedAreaV();

		_ASSERT(freeAreaV.getPosition().x == freeAreaH.getPosition().x);
		_ASSERT(freeAreaV.getPosition().y == freeAreaH.getPosition().y);

		//先判断当前排版区域是否满足要求
		if(freeAreaH.getTop() > lineTop && freeAreaH.getTop() < lineBottom)
		{
			LORect invlidArea = getIntersection(freeAreaH, freeAreaV);
			invlidArea.setHeight(lineBottom - invlidArea.getTop());
			segArea.useArea(invlidArea);
			continue;
		}

		//经过了检查之后，应该是同一行且至少满足该行之前要求的排版区域，或者是新的一行的排版区域
		if(freeAreaH.getTop() >= lineBottom)
		{
			lineTop = freeAreaH.getTop();
			lineBottom = lineTop;
			LORect invalidArea(0, 0, 10000, lineTop);
			segArea.useArea(invalidArea);
		}
		
		LORect usedArea;
		LORect freeArea;

		endIndex = processAline(seg, startIndex, freeAreaH, usedArea);
		if(startIndex == endIndex)
		{
			endIndex = processAline(seg, startIndex, freeAreaV, usedArea);
			if(startIndex == endIndex)
			{
				//两个区域都不行，则从新回到循环开头计算申请新的区域，并把这两个区域的交接处废弃
				LORect invlidArea = getIntersection(freeAreaH, freeAreaV);
				segArea.useArea(invlidArea);
				continue;
			}
			else
			{
				freeArea = freeAreaV;
			}
		}
		else
		{
			freeArea = freeAreaH;
		}

		freeArea.setHeight(usedArea.getHeight());
		lineBottom = lineBottom > freeArea.getBottom() ? lineBottom : freeArea.getBottom();

		int xOff = 0;
		//根据横向对齐方式调整该行所占用的区域
		if(seg.getHAlign() == LO_HA_RIGHT)
		{
			xOff = freeArea.getWidth() - usedArea.getWidth();
		}
		else if(seg.getHAlign() == LO_HA_CENTER)
		{
			xOff = (freeArea.getWidth() - usedArea.getWidth()) / 2;
		}

		//在确定该行区域后，调整改行中每个elem的具体位置和区域
		int yOff = 0;
		for(int i = startIndex; i < endIndex; ++i)
		{
			LayoutElement& elem = elemList[i];
			LORect& elemArea = elem.getArea();
			if(elem.getVAlign() == LO_VA_BOTTOM)
			{
				yOff = usedArea.getHeight() - elemArea.getHeight();
			}
			else if(elem.getVAlign() == LO_VA_CENTER)
			{
				yOff = (usedArea.getHeight() - elemArea.getHeight()) / 2;
			}
			elemArea.setPos(elemArea.getLeft() + xOff, elemArea.getTop() + yOff);
			elem.setArea(elemArea);
		}

		//对以换行结束的文字，特殊处理——强制下一个elem从下一行开始
		LayoutElement& lastElem = elemList[endIndex - 1];
		if(lastElem.getElemType() == LO_TEXT)
		{
			const wchar_t* elemWords = lastElem.getContent();
			if(elemWords[wcslen(elemWords) - 1] == '\n')
			{
				freeArea.setWidth(100000);
				freeArea.setPos(0, freeArea.getTop());
			}
		}
		segArea.useArea(freeArea);
		startIndex = endIndex;
	}

	if(elemList.size() > 0)
	{
		LayoutElement& lastElem = elemList[elemList.size() - 1];
		if(lastElem.getElemType() == LO_TEXT)
		{
			const wchar_t* elemWords = lastElem.getContent();
			if(elemWords[wcslen(elemWords) - 1] == '\n')
			{
				LORect& area = lastElem.getArea();
				area.setHeight(area.getHeight() + _render->getLineHeight(lastElem.getFont()));
			}
		}
	}
	
	LORect segRect;
	for(int j = 0; j < elemList.size(); ++j)
	{
		LayoutElement& elem = elemList[j];
		if(j == 0)
		{
			segRect = elem.getArea();
		}
		else
		{
			segRect.add(elem.getArea());
		}
	}
	
	seg.setArea(segRect);
	return segRect;
}


int	Layout::processAline(LayoutSeg& seg, int startElemIndex, const LORect& freeArea, LORect& usedArea)
{
	vector<LayoutElement>& elemList = seg.getElems();
	
	int curElemIndex = 0;

	usedArea.setPos(freeArea.getLeft(), freeArea.getTop());
	usedArea.setWidth(0);
	usedArea.setHeight(0);

	bool isEnd = false;
	for(int i = startElemIndex; i < elemList.size() && isEnd == false; ++i)
	{
		curElemIndex = i;
		LayoutElement& elem = elemList[i];
		LORect elemArea;
		
		const wchar_t* contentText = elem.getContent();

		if(elemList[i].getShowDes() == true)
		{
			elemArea.setHeight(_render->getLineHeight(elem.getFont()) + elem.getLineExtHeight());
			elemArea.setWidth(_render->getTextExtent(elemList[i].getDescription(), elem.getFont()));

			const wchar_t* d = elemList[i].getDescription();
			if(elemArea.getHeight() > freeArea.getHeight()
				|| usedArea.getWidth() + elemArea.getWidth() > freeArea.getWidth())
			{
				curElemIndex--;
				break;
			}
		}
		else if(elem.getElemType() == LO_TEXT)
		{
			//得到字符行高
			int lineHeight = _render->getLineHeight(elemList[i].getFont()) + elem.getLineExtHeight();
			if(lineHeight > freeArea.getHeight())
			{
				curElemIndex--;
				break;
			}

			int extent = _render->getTextExtent(contentText, elemList[i].getFont());
			int wordCount = wcslen(contentText);

			if(usedArea.getWidth() + extent > freeArea.getWidth())
			{
				int endIndex = _render->getCharAtPixel(contentText, 0, freeArea.getWidth() - usedArea.getWidth(), elemList[i].getFont());
				
				_ASSERT(wordCount > endIndex);

				if(endIndex == 0 || endIndex == -1)
				{
					curElemIndex--;
					break;
				}
				wchar_t* subText1 = NULL;
				wchar_t* subText2 = NULL;
				getSubstr(contentText, endIndex, subText1, subText2);
				
				elemList[i].setContent(subText1);
				LayoutElement newElem = elemList[i];
				newElem.setContent(subText2);
				
				elemList.insert(elemList.begin() + i + 1, newElem);
				--i;
				//从新开始计算新生成的elem
				--curElemIndex;
				continue;
			}
			else if(contentText[wordCount - 1] == '\n')
			{
				//如果该行是强制换行，则结束循环
				isEnd = true;
			}
			
			elemArea.setWidth(extent);
			elemArea.setHeight(lineHeight);
		}
		else if(elemList[i].getElemType() == LO_IMAGE)
		{
			elemArea = _render->getImageArea(elemList[i].getContent());
			elemArea.setHeight(elemArea.getHeight() + elem.getLineExtHeight());
			if(elemArea.getHeight() > freeArea.getHeight()
				|| usedArea.getWidth() + elemArea.getWidth() > freeArea.getWidth())
			{
				//对图片的特例——如果是新的一行，并且排不下的原因是因为图片的宽度超过了整个排版区域的宽度，则把图片超出的部分直接裁剪掉
				if(usedArea.getWidth() == 0 
					&& freeArea.getWidth() == _dataList.getWidth()
					&& elemArea.getHeight() <= freeArea.getHeight())
				{
					elemArea.setWidth(freeArea.getWidth());
				}
				else
				{
					curElemIndex--;
					break;
				}
			}
		}

		elemArea.setPos(usedArea.getRight(), usedArea.getTop());

		//设置该elem的区域
		elem.setArea(elemArea);
		
		//更新已使用范围
		usedArea.setWidth(usedArea.getWidth() + elemArea.getWidth());
		usedArea.setHeight(elemArea.getHeight() > usedArea.getHeight() ? elemArea.getHeight() : usedArea.getHeight());
	}
	
	return curElemIndex + 1;
}

void Layout::Render(int x, int y, float z)
{
	if(_needRelayout)
	{
 		processLayout();
 	}

	vector<LayoutSeg>& drawList = _dataList.getSegs();
	
	LOPoint actPos(x, y);
	
	if(_selectionStart == _selectionEnd)
	{
		for(int i = 0; i < drawList.size(); ++i)
		{
			vector<LayoutElement>& elems = drawList[i].getElems();
			
			for(int j = 0; j < elems.size(); ++j)
			{
				LayoutElement& elem = elems[j];
				LORect& elemArea = elem.getArea();
				if(elemArea.getRight() + x < _clipper.getLeft()
					|| elemArea.getLeft() + x > _clipper.getRight()
					|| elemArea.getBottom() + y > _clipper.getBottom()
					|| elemArea.getTop() + y <_clipper.getTop())
				{
					continue;
				}
				
				//elem背景图
				if(elem.isHaveBackImage())
				{
					_render->drawImage(elem.getBackImagePath(), elem.getArea().offset(actPos), elem.getColor(), elem.getAlpha(), elem.getBackImage(), elem.getArea().offset(actPos), z);
				}

				if(elem.getShowDes())
				{
					_render->drawText(elem.getDescription(), elem.getFont(), elem.getArea().offset(actPos), elem.getColor(), elem.getAlpha(), _clipper, z, _wordBorderMode, elem.isDrawUnderLine());	
				}
				else if(elem.getElemType() == LO_TEXT)
				{
					if(elem.getContent()[0] != 0)
						_render->drawText(elem.getContent(), elem.getFont(), elem.getArea().offset(actPos), elem.getColor(), elem.getAlpha(), _clipper, z, _wordBorderMode, elem.isDrawUnderLine());
				}
				else
				{
					_render->drawImage(elem.getContent(), elem.getArea().offset(actPos), elem.getColor(), elem.getAlpha(), elem.getImageInfo(), _clipper, z);
				}

				//elem前景图
				if(elem.isHaveFrontImage())
				{
					_render->drawImage(elem.getFrontImagePath(), elem.getArea().offset(actPos), elem.getColor(), elem.getAlpha(), elem.getFrontImage(), elem.getArea().offset(actPos), z);
				}
			}
		}
	}
	else
	{
		int curWordIndex = 0;
		int startIndex = _selectionStart < _selectionEnd ? _selectionStart : _selectionEnd;
		int endIndex = _selectionEnd > _selectionStart ? _selectionEnd : _selectionStart;

		int subTextLen1 = 0;
		int subTextLen2 = 0;
		int subTextLen3 = 0;
		LOPoint off;

		for(int i = 0; i < drawList.size(); ++i)
		{
			vector<LayoutElement>& elems = drawList[i].getElems();

			for(int j = 0; j < elems.size(); ++j)
			{
				LayoutElement& elem = elems[j];

				int elemWordCount = elem.getWordCount();
				LORect& elemArea = elem.getArea();
				if(elemArea.getRight() + x < _clipper.getLeft()
					|| elemArea.getLeft() + x > _clipper.getRight()
					|| elemArea.getBottom() + y > _clipper.getBottom()
					|| elemArea.getTop() + y <_clipper.getTop())
				{
					curWordIndex += elemWordCount;
					continue;
				}
				
				//elem背景图
				if(elem.isHaveBackImage())
				{
					_render->drawImage(elem.getBackImagePath(), elem.getArea().offset(actPos), elem.getColor(), elem.getAlpha(), elem.getBackImage(), elem.getArea().offset(actPos), z);
				}

				if(startIndex >= curWordIndex + elemWordCount || endIndex <= curWordIndex)
				{
					if(elem.getShowDes())
					{
						_render->drawText(elem.getDescription(), elem.getFont(), elem.getArea().offset(actPos), elem.getColor(), elem.getAlpha(), _clipper, z, _wordBorderMode, elem.isDrawUnderLine());	
					}
					else if(elem.getElemType() == LO_IMAGE)
					{
						_render->drawImage(elem.getContent(), elem.getArea().offset(actPos), elem.getColor(), elem.getAlpha(), elem.getImageInfo(), _clipper, z);
					}
					else
					{
						_render->drawText(elem.getContent(), elem.getFont(), elem.getArea().offset(actPos), elem.getColor(), elem.getAlpha(), _clipper, z, _wordBorderMode, elem.isDrawUnderLine());
					}
					curWordIndex += elemWordCount;
					continue;
				}

				subTextLen1 = elemWordCount;
				subTextLen2 = 0;
				subTextLen3 = 0;
				if(curWordIndex <= startIndex && curWordIndex + elemWordCount > startIndex)
				{
					subTextLen1 = startIndex - curWordIndex;
					subTextLen2 = curWordIndex + elemWordCount - startIndex;
				}
				if(curWordIndex < endIndex && curWordIndex + elemWordCount >= endIndex)
				{
					subTextLen2 -= curWordIndex + elemWordCount - endIndex;
					subTextLen3 = curWordIndex + elemWordCount - endIndex;
				}
				LOColor newcolor;
				newcolor.red = 1 - elem.getColor().red;
				newcolor.green = 1 - elem.getColor().green;
				newcolor.blue = 1 - elem.getColor().blue;
				if(elem.getShowDes())
				{
					_render->drawText(elem.getDescription(), elem.getFont(), elem.getArea().offset(actPos), newcolor, elem.getAlpha(), _clipper, z, _wordBorderMode, elem.isDrawUnderLine());
				}
				else if(elem.getElemType() == LO_IMAGE)
				{
					_render->drawImage(elem.getContent(), elem.getArea().offset(actPos), elem.getColor(), elem.getAlpha(), elem.getImageInfo(), _clipper, z);
				}
				else
				{
					off.x = 0;
					if(subTextLen1 != 0)
					{
						wchar_t* subText = NULL;
						getSubstr(elem.getContent(), 0, subTextLen1, subText);
						_render->drawText(subText, elem.getFont(), elem.getArea().offset(actPos), elem.getColor(), elem.getAlpha(), _clipper, z, _wordBorderMode, elem.isDrawUnderLine());

						off.x = _render->getTextExtent(subText, elem.getFont());
					}
					if(subTextLen2 != 0)
					{
						wchar_t* subText = NULL;
						getSubstr(elem.getContent(), subTextLen1, subTextLen2, subText);
						_render->drawText(subText, elem.getFont(), elem.getArea().offset(actPos).offset(off), newcolor, elem.getAlpha(), _clipper, z, _wordBorderMode, elem.isDrawUnderLine());

						off.x += _render->getTextExtent(subText, elem.getFont());
					}
					if(subTextLen3 != 0)
					{
						wchar_t* subText = NULL;
						getSubstr(elem.getContent(), subTextLen1 + subTextLen2, subTextLen3, subText);
						_render->drawText(subText, elem.getFont(), elem.getArea().offset(actPos).offset(off), elem.getColor(), elem.getAlpha(), _clipper, z, _wordBorderMode, elem.isDrawUnderLine());
					}
				}
				
				if(curWordIndex <= startIndex && curWordIndex + elemWordCount > startIndex)
				{
					startIndex = curWordIndex + elemWordCount;
				}
				curWordIndex += elemWordCount;

				//elem前景图
				if(elem.isHaveFrontImage())
				{
					_render->drawImage(elem.getFrontImagePath(), elem.getArea().offset(actPos), elem.getColor(), elem.getAlpha(), elem.getFrontImage(), elem.getArea().offset(actPos), z);
				}
			}
		}
	}
	//画光标
	if(_showCarat)
	{
//		_render->drawImage(_carat.getContent(), _carat.getArea().offset(actPos), _carat.getColor(), 1.0f, _carat.getImageInfo() , _clipper, z);
		_render->drawText(_carat.getContent(), _carat.getFont(), _carat.getArea().offset(actPos), _carat.getColor(), 1.0f, _clipper, z, _wordBorderMode, false);
		_render->drawText(_carat.getContent(), _carat.getFont(), _carat.getArea().offset(actPos).offset(LOPoint(0, -1)), _carat.getColor(), 1.0f, _clipper, z, _wordBorderMode, false);
	}
}

void Layout::Release()
{
	_dataList.clear();

	if(_render != NULL)
	{
		_render->Release();
		delete _render;
		_render = NULL;
	}
}

void Layout::showCarat(bool carat, bool select)
{
	_showCarat = carat;
}

LORect Layout::getRenderArea(bool adjWidth)
{
	return LORect(0, 0,
		_dataList.getActRenderWidth(adjWidth) + _dataList.getMarginR() + _dataList.getMarginL(), 
		_dataList.getActRenderHeight() + _dataList.getMarginB() + _dataList.getMarginT());
}

void Layout::setClipper(LORect& clipper)
{
	_clipper = clipper;
}

bool Layout::pickupElem(int x, int y, LOElemInfo& elemInfo)
{
	vector<LayoutSeg>& segs = _dataList.getSegs();
	
	for(int i = 0; i < segs.size(); ++i)
	{
		vector<LayoutElement>& elems = segs[i].getElems();
		for(int j = 0; j < elems.size(); ++j)
		{
			LayoutElement& elem = elems[j];
			LORect& elemArea = elem.getArea();
			if(elemArea.getLeft() < x
				&& elemArea.getRight() > x
				&& elemArea.getTop() < y
				&& elemArea.getBottom() > y)
			{
				elem.getElem(elemInfo);
				return true;
			}
		}
	}
	return false;
}


void Layout::setSelection(int startIndex, int endIndex)
{
	if(startIndex != -1)
		_selectionStart = startIndex;
	if(endIndex != -1)
		_selectionEnd = endIndex;
	
	if(_selectionStart < 0)
		_selectionStart = 0;
	if(_selectionEnd < 0)
		_selectionEnd = 0;
	
	int maxWordCount = 0;
	vector<LayoutSeg>& dataList = _dataList.getSegs();
	for(int k = 0; k < dataList.size(); ++k)
	{
		LayoutSeg& seg = dataList[k];
		maxWordCount += seg.getWordCount();
	}

	if(_selectionStart > maxWordCount)
		_selectionStart = maxWordCount;
	if(_selectionEnd > maxWordCount)
		_selectionEnd = maxWordCount;

	LOPoint caratPos = getPosAtWordIndex(_selectionEnd);
	_carat.getArea().setPos(caratPos);
}

void Layout::getSelection(int& startIndex, int& endIndex)
{
	startIndex = _selectionStart;
	endIndex = _selectionEnd;
}

int Layout::wordIndexAtPixel(int x, int y)
{
	std::vector<LayoutSeg>& renderList = _dataList.getSegs();
	
 	int wordIndex = 0;
	for(int i = 0; i < renderList.size(); ++i)
	{
  		LayoutSeg& seg = renderList[i];
// 		LORect& segArea = seg.getArea();
// 		if(segArea.getLeft() >= x
// 			|| segArea.getRight() <= x
// 			|| segArea.getTop() >= y
// 			|| segArea.getBottom() <= y)
// 		{
// 			continue;
// 		}

		std::vector<LayoutElement>& elems = seg.getElems();
		for(int j = 0; j < elems.size(); ++j)
		{
			LayoutElement& elem = elems[j];
			LORect& elemArea = elem.getArea();
			if(elemArea.getLeft() <= x
				&& elemArea.getRight() > x
				&& elemArea.getTop() <= y
				&& elemArea.getBottom() > y)
			{
				int pixel = x - elemArea.getLeft();
				if(elem.getShowDes() || elem.getElemType() == LO_IMAGE)
				{
					wordIndex += elemArea.getWidth() / 2 > pixel ? 0 : 1;
				}
				else
				{
					int tempWidth = 0;
					for(int k = 0; k < elem.getWordCount() - 1; ++k)
					{
						int wordWidth = _render->getWordSize(elem.getContent()[k], elem.getFont()).getWidth();
						if(tempWidth + wordWidth / 2 > pixel)
						{
							break;
						}
						tempWidth += wordWidth;
						++wordIndex;
					}
					//wordIndex += _render->getCharAtPixel(elem.getContent(), 0, x - elemArea.getLeft(), elem.getFont()) - 1;
				}
				return wordIndex;
			}

			wordIndex += elem.getWordCount();
		}
	}
 	return wordIndex;
}

LOPoint Layout::getPosAtWordIndex(int wordIndex)
{
	vector<LayoutSeg>& segs = _dataList.getSegs();
	int curWordIndex = 0;

	for(int i = 0; i < segs.size(); ++i)
	{
		vector<LayoutElement>& elems = segs[i].getElems();
		for(int j = 0; j < elems.size(); ++j)
		{
			LayoutElement& elem = elems[j];
			int elemWordCount = elem.getWordCount();

			if(curWordIndex <= wordIndex && curWordIndex + elemWordCount >= wordIndex)
			{
				int len = wordIndex - curWordIndex;
				int offx = 0;
				if(elem.getElemType() == LO_TEXT && elem.getShowDes() == false)
				{
					wchar_t* subText1 = NULL;
					wchar_t* subText2 = NULL;
					getSubstr(elem.getContent(), len, subText1, subText2);
					offx = _render->getTextExtent(subText1, elem.getFont());
				}
				else
				{
					if(curWordIndex + elemWordCount == wordIndex)
					{
						offx = elem.getArea().getWidth();
					}
				}
				LOPoint caratPos;
				caratPos.x = elem.getArea().getLeft() + offx;
				caratPos.y = elem.getArea().getBottom() - _carat.getArea().getHeight();
				
				return caratPos;
			}
			curWordIndex += elemWordCount;
		}
	}
 	LOPoint nullPoint;
 	nullPoint.x = 0;
 	nullPoint.y = 0;
 	return nullPoint;
}

int Layout::getElemList(LOElemInfo*& elemInfos)
{
	int elemCount = 0;
	vector<LayoutSeg>& segs = _dataList.getSegs();
	for(int segIndex = 0; segIndex < segs.size(); ++segIndex)
	{
		elemCount += segs[segIndex].getElems().size();
	}

	elemInfos = new LOElemInfo[elemCount];
	if(!elemInfos)
	{
		_ASSERT(0);
		return 0;
	}

	int curElemIndex = 0;

	for(int i = 0; i < segs.size(); ++ i)
	{
		vector<LayoutElement>& elems = segs[i].getElems();
		for(int j = 0; j < elems.size(); ++j)
		{
			if(curElemIndex >= elemCount)
			{
				_ASSERT(0);
				continue;
			}
			elems[j].getElem(elemInfos[curElemIndex]);
			++curElemIndex;
		}
	}
	return elemCount;
}

bool Layout::isHaveContent()
{
	vector<LayoutSeg>& segs = _dataList.getSegs();
	int curWordIndex = 0;

	if(segs.size() > 0)
	{
		if(segs[0].getElems().size() > 0)
			return true;
	}
	return false;
}

void Layout::setColor(LOColor color, float alpha)
{
	vector<LayoutSeg>& segs = _dataList.getSegs();
	for(int i = 0; i < segs.size(); ++i)
	{
		vector<LayoutElement>& elems = segs[i].getElems();
		for(int j = 0; j < elems.size(); ++j)
		{
			LayoutElement& elem = elems[j];
			elem.setColor(color);
			elem.setAlpha(alpha);
		}
	}

	LOColor caratColor = color;
	caratColor.red = 1 - caratColor.red;
	caratColor.red = 1 - caratColor.red * caratColor.red;

	caratColor.green = 1 - caratColor.green;
	caratColor.green = 1 - caratColor.green * caratColor.green;
	
	caratColor.blue = 1 - caratColor.blue;
	caratColor.blue = 1 - caratColor.blue * caratColor.blue;
	_carat.setColor(caratColor);
}

void Layout::setAlpha(float alpha)
{
	vector<LayoutSeg>& segs = _dataList.getSegs();
	for(int i = 0; i < segs.size(); ++i)
	{
		vector<LayoutElement>& elems = segs[i].getElems();
		for(int j = 0; j < elems.size(); ++j)
		{
			LayoutElement& elem = elems[j];
			elem.setAlpha(alpha);
		}
	}
}

void Layout::flashLayout()
{
	processLayout();
	setSelection(_selectionStart, _selectionEnd);
}


int	Layout::getWordCount()
{
	int wordCount = 0;
	for(int i = 0; i < _dataList.getSegs().size(); ++i)
	{
		wordCount += _dataList.getSegs()[i].getWordCount();
	}
	return wordCount;
}

void Layout::setBorderMode(int mode)
{
	_wordBorderMode = mode;
}

void CreateLayout( 
	ILayout** pLayout,
	ILayoutRender* pRender )
{
	*pLayout = new Layout(pRender);
}

LOFont::LOFont()
{
	style = NORMAL;
	variant = 0;
	weight = 0;
	size = 0;
	height = 0;
	family[0] = 0;
	wordExtSpace = 0;
}

LOFont::LOFont(const LOFont& newFont)
{
	style = newFont.style;
	variant = newFont.variant;
	weight = newFont.weight;
	size = newFont.size;
	height = newFont.height;
	strcpy(family, newFont.family);
	wordExtSpace = 0;
}

bool LOFont::operator==(const LOFont& other)
{
	if(!strcmp(family, other.family)
		&& height == other.height
		&& size == other.size
		&& style == other.style
		&& variant == other.variant
		&& weight == other.weight
		&& wordExtSpace == other.wordExtSpace)
	{
		return true;
	}
	return false;
}

LORect::LORect()
{
	_width		= 0;
	_height		= 0;
}

LORect::LORect(const LORect& area)
{
	_topLeftPos = area._topLeftPos;
	_width		= area._width;
	_height		= area._height;
}

LORect::LORect(int x, int y, int w, int h)
{
	_topLeftPos.x	= x;
	_topLeftPos.y	= y;
	_width			= w;
	_height			= h;
}

LOGameObject::LOGameObject()
{
	for(int i = 0; i < LAYOUT_GAME_OBJECT_MAX_ID_COUNT; ++i)
	{
		_objId[i] = -1;
	}
	_objType = LO_GO_NOTHING;
}

LOGameObject::LOGameObject(const LOGameObject& other)
{
	for(int i = 0; i < LAYOUT_GAME_OBJECT_MAX_ID_COUNT; ++i)
	{
		_objId[i] = other._objId[i];
	}
	_objType = other._objType;
}


bool LOGameObject::operator==(const LOGameObject& other)
{
	if(_objType != other._objType)
	{
		return false;
	}

	for(int i = 0; i < LAYOUT_GAME_OBJECT_MAX_ID_COUNT; ++i)
	{
		if(_objId[i] != other._objId[i])
		{
			return false;
		}
	}

	return true;
}

LOImageInfo::LOImageInfo()
{
	frameCount		= 0;
	interval		= 0;
	lastDrawTime	= ::GetTickCount();
}

LOImageInfo::LOImageInfo(const LOImageInfo& other)
{
	frameCount		= other.frameCount;
	interval		= other.interval;
	lastDrawTime	= other.lastDrawTime;
}

bool LOImageInfo::operator==(const LOImageInfo& other)
{
	if(frameCount == other.frameCount
		&& interval == other.interval)
	{
		return true;
	}
	return false;
}

LOPoint::LOPoint()
{
	x = 0;
	y = 0;
}

LOPoint::LOPoint(int newx, int newy)
{
	x = newx;
	y = newy;
}

LOPoint::LOPoint(const LOPoint& point)
{
	x = point.x;
	y = point.y;
}

	void LORect::setPos(LOPoint newPos)	{	_topLeftPos = newPos;			}
	void LORect::setPos(int x, int y)	{	_topLeftPos.x = x;	
									_topLeftPos.y = y;				}
	void LORect::setWidth(int width)	{	_width = width;					}
	void LORect::setHeight(int height)	{	_height = height;				}
	int LORect::getLeft()	const		{	return _topLeftPos.x;			}
	int LORect::getTop()	const		{	return _topLeftPos.y;			}
	int LORect::getBottom()	const		{	return _topLeftPos.y + _height;}
	int LORect::getRight()	const		{	return _topLeftPos.x + _width;	}
	int LORect::getWidth()	const		{	return _width;					}
	int LORect::getHeight()	const		{	return _height;					}
	LOPoint LORect::getPosition()const	{	return _topLeftPos;				}

bool LORect::operator>(const LORect& otherRect)
{
	if(getTop() > otherRect.getTop())
	{
		return true;
	}
	else if(getTop() < otherRect.getTop())
	{
		return false;
	}
	else if(getLeft() > otherRect.getLeft())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool LORect::operator<(const LORect& otherRect)
{
	if(getTop() < otherRect.getTop())
	{
		return true;
	}
	else if(getTop() > otherRect.getTop())
	{
		return false;
	}
	else if(getLeft() < otherRect.getLeft())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool LORect::operator==(const LORect& otherRect)
{
	if(*this > otherRect || *this < otherRect)
	{
		return false;
	}
	return true;
}

LORect LORect::offset(LOPoint& off)
{
	LORect retRect(*this);
	retRect._topLeftPos.x = _topLeftPos.x + off.x;
	retRect._topLeftPos.y = _topLeftPos.y + off.y;
	return retRect;
}

void LORect::add(LORect& newRect)
{
	int left	= getLeft()		< newRect.getLeft()		? getLeft() : newRect.getLeft();
	int top		= getTop()		< newRect.getTop()		? getTop()	: newRect.getTop();
	int right	= getRight()	> newRect.getRight()	? getRight() : newRect.getRight();
	int bottom	= getBottom()	> newRect.getBottom()	? getBottom() : newRect.getBottom();
	setPos(left, top);
	setHeight(bottom - top);
	setWidth(right - left);
}


LOColor::LOColor(const char* color)
{
	sscanf(color, "%f,%f,%f", &red, &green, &blue);
	red		/= 255;
	green	/= 255;
	blue	/= 255;
}

LOColor::LOColor()
{
	red = 1.0f;
	green = 1.0f;
	blue = 1.0f;
}

bool LOColor::operator==(const LOColor& other)
{
	if(blue == other.blue
		&& green == other.green
		&& red == other.red)
	{
		return true;
	}
	return false;
}

LOElemInfo::LOElemInfo()
{
	elemType = LO_TEXT;
	isShowDes = false;
	
	inheritsColor = false;

	lineExtHeight = 0;
	inheritslineExtHeight = true;

	inheritsWordExtSpace = true;

	isHaveBackImage = false;
	isHaveFrontImage = false;

	underLine = false;
}

LOElemInfo::LOElemInfo(const LOElemInfo& elem)
{
	area		= elem.area;

	elemType	= elem.elemType;
	vAlign		= elem.vAlign;
	gameObj		= elem.gameObj;

	color		= elem.color;
	inheritsColor = elem.inheritsColor;

	font		= elem.font;

	imageInfo	= elem.imageInfo;
	isShowDes	= elem.isShowDes;
	
	content		= elem.content;
	description = elem.description;
	
	lineExtHeight = elem.lineExtHeight;
	inheritslineExtHeight = elem.inheritslineExtHeight;

	inheritsWordExtSpace = elem.inheritsWordExtSpace;

	isHaveBackImage = elem.isHaveBackImage;
	backImagePath = elem.backImagePath;
	backImage = elem.backImage;

	isHaveFrontImage = elem.isHaveFrontImage;
	frontImagePath = elem.frontImagePath;
	frontImage = elem.frontImage;

	underLine = elem.underLine;
}

bool LOElemInfo::operator==(const LOElemInfo& other)
{
	if(area == other.area
		&& elemType == other.elemType
		&& gameObj == other.gameObj
		&& color == other.color
		&& font == other.font
		&& imageInfo == other.imageInfo
		&& isShowDes == other.isShowDes
		&& content == other.content
		&& description == other.description
		&& lineExtHeight == other.lineExtHeight
		&& inheritsColor == other.inheritsColor
		&& inheritslineExtHeight == other.inheritslineExtHeight
		&& inheritsWordExtSpace == other.inheritsWordExtSpace

		&& isHaveBackImage == other.isHaveBackImage
		&& backImagePath == other.backImagePath
		&& backImage == other.backImage

		&& isHaveFrontImage == other.isHaveFrontImage
		&& frontImagePath == other.frontImagePath
		&& frontImage == other.frontImage
		
		&& underLine == other.underLine)
	{
		return true;
	}

	return false;
}

bool LOElemInfo::operator!=(const LOElemInfo& other)
{
	return !(*this == other);
}

int LOString::set(const char* content)
{
	if(NULL == content)
	{
		_ASSERT(0);
		return 0;
	}

	//得到unicode编码方式的文字
	size_t lengthAnsi = strlen(content);
	size_t lengthUnicode = MultiByteToWideChar(CP_ACP, 0, content, lengthAnsi, NULL, 0);
	
	_string.resize(lengthUnicode + 1);
	
	wchar_t* szUnicode = &_string[0];
	MultiByteToWideChar(CP_ACP, 0, content, lengthAnsi, szUnicode, lengthUnicode);
	szUnicode[lengthUnicode] = 0;

	return lengthUnicode;
}

int LOString::set(const wchar_t* content)
{
	if(NULL == content)
	{
		_ASSERT(0);
		return 0;
	}

	int textLen = wcslen(content);
	
	_string.resize(textLen + 1);

	for(int i = 0; i < textLen; ++i)
	{
		_string[i] = content[i];
	}
	_string[textLen] = 0;

	return textLen;
}


LOString::LOString()
{
	_string.clear();
	_string.push_back(0);
}

LOString::~LOString()
{
}

LOString& LOString::operator=(const LOString& other)
{
	_string.clear();
	_string.resize(other._string.size());

	for(int i = 0; i < other._string.size(); ++i)
	{
		_string[i] = other._string[i];
	}
	return *this;
}

LOString::LOString(const LOString& other)
{
	_string.clear();
	_string.resize(other._string.size());

	for(int i = 0; i < other._string.size(); ++i)
	{
		_string[i] = other._string[i];
	}
}

void LOString::extentContent(const wchar_t* newContent, int startIndex)
{
	int textLen = wcslen(newContent);
	int wordCount = wcslen(get());
	int newTextLen = wordCount + textLen;
	
	if(startIndex < 0 || startIndex > wordCount)
	{
		_ASSERT(0);
		return;
	}

	if(newTextLen + 1 > _string.size())
	{
		_string.resize(newTextLen + 1);
	}

	for(int i = wordCount; i >= startIndex; --i)
	{
		_string[i + textLen] = _string[i];
	}

	for(int j = 0; j < textLen; ++j)
	{
		_string[j + startIndex] = newContent[j];
	}
}

int LOString::len()
{	
	return _string.size() - 1;
}

wchar_t& LOString::operator[](int index)
{
	if(index >= _string.size())
	{
		index = _string.size() - 1;
	}
	else if(index < 0)
	{
		index = 0;
	}
	return _string[index];
}

bool LOString::operator==(const LOString& other)
{
	if(_string.size() != other._string.size())
	{
		return false;
	}

	for(int i = 0; i < _string.size(); ++i)
	{
		if(_string[i] != other._string[i])
		{
			return false;
		}
	}

	return true;
}