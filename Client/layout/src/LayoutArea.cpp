/////////////////////////////////////////////////////////////////////////////
//  FileName    :   LayoutParser.cpp
//  Creator     :   xiehong
//  Date        :   2006-12-26 21:54
//  Comment     :   
//	Changes		:	
/////////////////////////////////////////////////////////////////////////////

#include "LayoutArea.h"

LORect getIntersection(const LORect& rectA,const LORect& rectB)
{
	LORect retRect;
	retRect.setPos(0, 0);
	retRect.setWidth(0);
	retRect.setHeight(0);

	if(rectB.getLeft() < rectA.getRight() &&
		rectB.getRight() > rectA.getLeft() &&
		rectB.getTop() < rectA.getBottom() &&
		rectB.getBottom() > rectA.getTop())
	{
		int left	= rectA.getLeft()		> rectB.getLeft()	? rectA.getLeft()		: rectB.getLeft();
		int right	= rectA.getRight()		< rectB.getRight()	? rectA.getRight()	: rectB.getRight();
		int top		= rectA.getTop()		> rectB.getTop()	? rectA.getTop()		: rectB.getTop();
		int bottom	= rectA.getBottom()		< rectB.getBottom() ? rectA.getBottom()	: rectB.getBottom();
		
		retRect.setPos(left, top);
		retRect.setWidth(right - left);
		retRect.setHeight(bottom - top);
	}
	return retRect;
}

bool splitRectH(const LORect& oriRect, const LORect& spliter, std::vector<LORect>& newRects)
{
	newRects.clear();
	LORect formatSpliter = getIntersection(oriRect, spliter);
	
	//两个矩形不相交
	if(formatSpliter.getWidth() == 0 && formatSpliter.getHeight() == 0)
	{
		newRects.push_back(oriRect);
		return false;
	}
	
	if(formatSpliter.getTop() > oriRect.getTop())
	{
		LORect topRect;
		topRect.setPos(oriRect.getLeft(), oriRect.getTop());
		topRect.setWidth(oriRect.getWidth());
		topRect.setHeight(formatSpliter.getTop() - oriRect.getTop());
		newRects.push_back(topRect);
	}
	
	if(formatSpliter.getLeft() > oriRect.getLeft())
	{
		LORect leftRect;
		leftRect.setPos(oriRect.getLeft(), formatSpliter.getTop());
		leftRect.setHeight(formatSpliter.getHeight());
		leftRect.setWidth(formatSpliter.getLeft() - oriRect.getLeft());
		newRects.push_back(leftRect);
	}
	
	if(formatSpliter.getRight() < oriRect.getRight())
	{
		LORect rightRect;
		rightRect.setPos(formatSpliter.getRight(), formatSpliter.getTop());
		rightRect.setHeight(formatSpliter.getHeight());
		rightRect.setWidth(oriRect.getRight() - formatSpliter.getRight());
		newRects.push_back(rightRect);
	}
	
	if(formatSpliter.getBottom() < oriRect.getBottom())
	{
		LORect bottomRect;
		bottomRect.setPos(oriRect.getLeft(), formatSpliter.getBottom());
		bottomRect.setHeight(oriRect.getBottom() - formatSpliter.getBottom());
		bottomRect.setWidth(oriRect.getWidth());
		newRects.push_back(bottomRect);
	}

	return true;
}

bool splitRectV(const LORect& oriRect, const LORect& spliter, std::vector<LORect>& newRects)
{
	newRects.clear();
	LORect formatSpliter = getIntersection(oriRect, spliter);
	
	//两个矩形不相交
	if(formatSpliter.getWidth() == 0 && formatSpliter.getHeight() == 0)
	{
		newRects.push_back(oriRect);
		return false;
	}
	
	if(formatSpliter.getTop() > oriRect.getTop())
	{
		LORect topRect;
		topRect.setPos(formatSpliter.getLeft(), oriRect.getTop());
		topRect.setWidth(formatSpliter.getWidth());
		topRect.setHeight(formatSpliter.getTop() - oriRect.getTop());
		newRects.push_back(topRect);
	}
	
	if(formatSpliter.getLeft() > oriRect.getLeft())
	{
		LORect leftRect;
		leftRect.setPos(oriRect.getLeft(), oriRect.getTop());
		leftRect.setHeight(oriRect.getHeight());
		leftRect.setWidth(formatSpliter.getLeft() - oriRect.getLeft());
		newRects.push_back(leftRect);
	}
	
	if(formatSpliter.getRight() < oriRect.getRight())
	{
		LORect rightRect;
		rightRect.setPos(formatSpliter.getRight(), oriRect.getTop());
		rightRect.setHeight(oriRect.getHeight());
		rightRect.setWidth(oriRect.getRight() - formatSpliter.getRight());
		newRects.push_back(rightRect);
	}
	
	if(formatSpliter.getBottom() < oriRect.getBottom())
	{
		LORect bottomRect;
		bottomRect.setPos(formatSpliter.getLeft(), formatSpliter.getBottom());
		bottomRect.setHeight(oriRect.getBottom() - formatSpliter.getBottom());
		bottomRect.setWidth(formatSpliter.getWidth());
		newRects.push_back(bottomRect);
	}

	return true;
}

LayoutArea::LayoutArea()
{
	_areaH.clear();
	_areaV.clear();
}

LayoutArea::LayoutArea(LayoutArea& newArea)
{
	_areaV.clear();
	_areaH.clear();
	for(RectList::iterator itAreaV = newArea._areaV.begin(); itAreaV != newArea._areaV.end(); ++itAreaV)
	{
		_areaV.push_back(*itAreaV);
	}
	for(RectList::iterator itAreaH = newArea._areaH.begin(); itAreaH != newArea._areaH.end(); ++itAreaH)
	{
		_areaH.push_back(*itAreaH);
	}
}

LayoutArea::~LayoutArea()
{
	_areaV.clear();
	_areaH.clear();
}

void LayoutArea::useArea(const LORect& usedArea)
{
	if(usedArea.getWidth() == 0 || usedArea.getHeight() == 0)
	{
		return;
	}

	std::vector<LORect> splitedRectsV;
	RectList::iterator itAreaV = _areaV.begin();
	while(itAreaV != _areaV.end())
	{
		LORect& curArea = *itAreaV;
		if(splitRectV(curArea, usedArea, splitedRectsV) == false)
		{
			itAreaV++;
			continue;
		}

		_areaV.erase(itAreaV++);
		
		for(int i = 0; i < splitedRectsV.size(); ++i)
			_areaV.push_front(splitedRectsV[i]);
	}

	std::vector<LORect> splitedRectsH;
	RectList::iterator itAreaH = _areaH.begin();
	while(itAreaH != _areaH.end())
	{
		LORect& curArea = *itAreaH;
		if(splitRectH(curArea, usedArea, splitedRectsH) == false)
		{
			itAreaH++;
			continue;
		}

		_areaH.erase(itAreaH++);
		
		for(int i = 0; i < splitedRectsH.size(); ++i)
			_areaH.push_front(splitedRectsH[i]);
	}
}

const LORect& LayoutArea::getAUnusedAreaV()
{
	static LORect emptyArea;
	if(_areaV.size() == 0)
		return emptyArea;

	_areaV.sort();
	return *_areaV.begin();
}

const LORect& LayoutArea::getAUnusedAreaH()
{
	static LORect emptyArea;
	if(_areaH.size() == 0)
		return emptyArea;

	_areaH.sort();
	return *_areaH.begin();
}

void LayoutArea::setArea(const LORect& initArea)
{
	if(initArea.getWidth() == 0 || initArea.getHeight() == 0)
	{
		return;
	}

	_areaV.clear();
	_areaV.push_back(initArea);
	
	_areaH.clear();
	_areaH.push_back(initArea);
}

bool LayoutArea::peek()
{
	if(_areaH.size() == 0)
	{
		return false;
	}
	return true;
}