/////////////////////////////////////////////////////////////////////////////
//  FileName    :   LayoutParser.h
//  Creator     :   xiehong
//  Date        :   2006-12-26 21:54
//  Comment     :   
//	Changes		:	
/////////////////////////////////////////////////////////////////////////////

#ifndef _LAYOUTAREA_H_
#define _LAYOUTAREA_H_

#include "layoutdef.h"
#include <list>
#include <vector>


/////////////////////////////////////////////////////////////////////////////
//
//              Class Declare
//
/////////////////////////////////////////////////////////////////////////////

LORect getIntersection(const LORect& rectA,const LORect& rectB);

class LayoutArea
{
	typedef std::list<LORect> RectList;

	RectList _areaV;
	RectList _areaH;

public:

	LayoutArea();

	LayoutArea(LayoutArea& newArea);

	~LayoutArea();

	void setArea(const LORect& initArea);
	
	void useArea(const LORect& delArea);

	const LORect& getAUnusedAreaV();

	const LORect& getAUnusedAreaH();

	bool peek();
};

#endif