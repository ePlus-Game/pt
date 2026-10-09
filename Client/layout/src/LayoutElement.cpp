/////////////////////////////////////////////////////////////////////////////
//  FileName    :   LayoutElement.cpp
//  Creator     :   xiehong
//  Date        :   2006-15-18 16:00
//  Comment     :   element declare
//	Changes		:	
/////////////////////////////////////////////////////////////////////////////

#include "LayoutElement.h"
#include "layoutdef.h"
#include <crtdbg.h>


using namespace std;
LayoutElement::LayoutElement( )
{
	_elemType = LO_TEXT;
	_vAlign = LO_VA_BOTTOM;
	
	_isShowDes = false;

	_alpha = 1.0f;
	_inheritsColor = true;

	_lineExtHeight = 0;
	_inheritsLineExtheight = true;
	
	_inheritsWordExtSpace = true;

	_isHaveBackImage = false;
	_isHaveFrontImage = false;

	_underLine = false;
}

LayoutElement::LayoutElement( const LayoutElement& elem )
{
	_area			= elem._area;

	_elemType		= elem._elemType;
	_vAlign			= elem._vAlign;
	
	_font			= elem._font;
	_color			= elem._color;
	_inheritsColor	= elem._inheritsColor;
	
	_gameObj		= elem._gameObj;
	_imageInfo		= elem._imageInfo;

	_isShowDes		= elem._isShowDes;

	_alpha			= elem._alpha;

	_content		= elem._content;
	_description	= elem._description;

	_lineExtHeight	= elem._lineExtHeight;
	_inheritsLineExtheight = elem._inheritsLineExtheight;
	
	_inheritsWordExtSpace = elem._inheritsWordExtSpace;

	_backImagePath		= elem._backImagePath;
	_backImage			= elem._backImage;
	_isHaveBackImage	= elem._isHaveBackImage;

	_frontImagePath		= elem._frontImagePath;
	_frontImage			= elem._frontImage;
	_isHaveFrontImage	= elem._isHaveFrontImage;

	_underLine			= elem._underLine;
}

LayoutElement::~LayoutElement( )
{

}

void LayoutElement::extentContent(wchar_t* newContent, int startIndex)
{
	_content.extentContent(newContent, startIndex);
}

void LayoutElement::setContent(const char* newContent)
{
	_content.set(newContent);
}

void LayoutElement::setContent(const wchar_t* newContent)
{
	_content.set(newContent);
}

void LayoutElement::setDescription(const char* newDescription)
{
	_description.set(newDescription);
}

void LayoutElement::setDescription(const wchar_t* newDescription)
{
	_description.set(newDescription);
}

void LayoutElement::setProperty( char* property )
{
	if(NULL == property)
	{
		_ASSERT(0);
		return;
	}

	char propertyName[16]  = {0};
	char propertyValue[512] = {0};
	
	sscanf( property, PORPERTY_PARSER_TEXT, propertyName, propertyValue);

	if(strlen(propertyName) == 0 || strlen(propertyValue) == 0)
	{
		return;
	}

	//颜色
	if(strcmp(propertyName, Color) == 0 || strcmp(propertyName, ColorS) == 0)
	{
		if(sscanf(propertyValue, "%f,%f,%f", &_color.red, &_color.green, &_color.blue) == 3)
		{
			_color.red		/= 255;
			_color.green	/= 255;
			_color.blue		/= 255;
			_inheritsColor = false;
		}
		else
		{
			_ASSERT(0);
		}
	}
	else if(strcmp(propertyName, ColorNum) == 0 || strcmp(propertyName, ColorNumS) == 0)
	{
		unsigned int color;
		if(sscanf(propertyValue, "%x", &color))
		{
			_color.red		= (color & 0x00ff0000) >> 16;
			_color.green	= (color & 0x0000ff00) >> 8;
			_color.blue		= (color & 0x000000ff);
			_color.red		/= 255;
			_color.green	/= 255;
			_color.blue		/= 255;
			_inheritsColor	= false;
		}
		else
		{
			_ASSERT(0);
		}
	}
	else if(strcmp(propertyName, ElemType) == 0 || strcmp(propertyName, ElemTypeS) == 0)
	{
		if(strcmp(propertyValue, "text") == 0)
			_elemType = LO_TEXT;
		else if(strcmp(propertyValue, "pic") == 0)
			_elemType = LO_IMAGE;
	}
	else if(strcmp(propertyName, FontFamily) == 0 || strcmp(propertyName, FontFamilyS) == 0)
	{
		strcpy(_font.family, propertyValue);
	}
	else if(strcmp(propertyName, FontStyle) == 0 || strcmp(propertyName, FontStyleS) == 0)
	{
		if(strcmp(propertyValue, "normal") == 0)
			_font.style = NORMAL;
		else if(strcmp(propertyValue, "italic") == 0)
			_font.style = ITALIC;
		else if(strcmp(propertyValue, "oblique") == 0)
			_font.style = OBLIQUE;
	}
	else if(strcmp(propertyName, TextVAlign) == 0 || strcmp(propertyName, TextVAlignS) == 0)
	{
		if(strcmp(propertyValue, "top") == 0)
			_vAlign = LO_VA_TOP;
		else if(strcmp(propertyValue, "bottom") == 0)
			_vAlign = LO_VA_BOTTOM;
		else if(strcmp(propertyValue, "center") == 0)
			_vAlign = LO_VA_CENTER;
	}
	else if(strcmp(propertyName, GameObjectType) == 0 || strcmp(propertyName, GameObjectTypeS) == 0)
	{
		if(strcmp(propertyValue, "player") == 0)
			_gameObj._objType = LO_GO_PLAYER;
		else if(strcmp(propertyValue, "item") == 0)
			_gameObj._objType = LO_GO_ITEM;
		else if(strcmp(propertyValue, "face") == 0)
			_gameObj._objType = LO_GO_FACE;
		else if(strcmp(propertyValue, "chan") == 0)
			_gameObj._objType = LO_GO_CHANNEL;
		else if(strcmp(propertyValue, "pos") == 0)
			_gameObj._objType = LO_GO_POSITION;
		else if(strcmp(propertyValue, "portrait") == 0)
			_gameObj._objType = LO_GO_PORTRAIT;
		else if(strcmp(propertyValue, "map") == 0)
			_gameObj._objType = LO_GO_MAP;
		else if(strcmp(propertyValue, "skill") == 0)
			_gameObj._objType = LO_GO_SKILL;
		else if(strcmp(propertyValue, "quest") == 0)
			_gameObj._objType = LO_GO_TASK;
		else if(strcmp(propertyValue, "npc") == 0)
			_gameObj._objType = LO_GO_NPC;
		else if(strcmp(propertyValue, "social")==0)
			_gameObj._objType = LO_GO_SOCIAL_OWNER;
		else
			_gameObj._objType = LO_GO_NOTHING;
	}
	else if(strcmp(propertyName, GameObjectId) == 0 || strcmp(propertyName, GameObjectIdS) == 0)
	{
		sscanf(propertyValue, "%d", &_gameObj._objId[0]);
	}
	else if(strcmp(propertyName, GameObjectId1) == 0 || strcmp(propertyName, GameObjectId1S) == 0)
	{
		sscanf(propertyValue, "%d", &_gameObj._objId[1]);
	}
	else if(strcmp(propertyName, GameObjectId2) == 0 || strcmp(propertyName, GameObjectId2S) == 0)
	{
		sscanf(propertyValue, "%d", &_gameObj._objId[2]);
	}
	else if(strcmp(propertyName, GameObjectId3) == 0 || strcmp(propertyName, GameObjectId3S) == 0)
	{
		sscanf(propertyValue, "%d", &_gameObj._objId[3]);
	}
	else if(strcmp(propertyName, GameObjectId4) == 0 || strcmp(propertyName, GameObjectId4S) == 0)
	{
		sscanf(propertyValue, "%d", &_gameObj._objId[4]);
	}
	else if(strcmp(propertyName, Description) == 0 || strcmp(propertyName, DescriptionS) == 0)
	{
		setDescription(propertyValue);
	}
	else if(strcmp(propertyName, ShowDescription) == 0 || strcmp(propertyName, ShowDescriptionS) == 0)
	{
		if(strcmp(propertyValue, "true") == 0)
			_isShowDes = true;
		else
			_isShowDes = false;
	}
	else if(strcmp(propertyName, LineExtHeight) == 0 || strcmp(propertyName, LineExtHeightS) == 0)
	{
		_inheritsLineExtheight = false;
		sscanf(propertyValue, "%d", &_lineExtHeight);
		_lineExtHeight = _lineExtHeight >= 0 ? _lineExtHeight : 0;
	}
	else if(strcmp(propertyName, WordExtSpace) == 0 || strcmp(propertyName, WordExtSpaceS) == 0)
	{
		_inheritsWordExtSpace = false;
		sscanf(propertyValue, "%d", &_font.wordExtSpace);
		_font.wordExtSpace = _font.wordExtSpace >= 0 ? _font.wordExtSpace : 0;
	}
	else if(strcmp(propertyName, BackImagePath) == 0 || strcmp(propertyName, BackImagePathS) == 0)
	{
		_isHaveBackImage = true;
		_backImagePath.set(propertyValue);
	}
	else if(strcmp(propertyName, FrontImagePath) == 0 || strcmp(propertyName, FrontImagePathS) == 0)
	{
		_isHaveFrontImage = true;
		_frontImagePath.set(propertyValue);
	}
	else if(strcmp(propertyName, UnderLine) == 0 || strcmp(propertyName, UnderLineS) == 0)
	{
		if(strcmp(propertyValue, "true") == 0)
			_underLine = true;
		else
			_underLine = false;
	}
}

void LayoutElement::setElem(const LOElemInfo& elemInfo)
{
	_area			= elemInfo.area;
	
	_elemType		= elemInfo.elemType;
	_vAlign			= elemInfo.vAlign;
	
	_font			= elemInfo.font;
	_color			= elemInfo.color;
	_inheritsColor	= elemInfo.inheritsColor;

	_imageInfo		= elemInfo.imageInfo;
	_gameObj		= elemInfo.gameObj;

	_isShowDes		= elemInfo.isShowDes;
	
	_content		= elemInfo.content;
	_description	= elemInfo.description;

	_lineExtHeight	= elemInfo.lineExtHeight;
	_inheritsLineExtheight = elemInfo.inheritslineExtHeight;

	_inheritsWordExtSpace = elemInfo.inheritslineExtHeight;

	
	_backImagePath		= elemInfo.backImagePath;
	_backImage			= elemInfo.backImage;
	_isHaveBackImage	= elemInfo.isHaveBackImage;

	_frontImagePath		= elemInfo.frontImagePath;
	_frontImage			= elemInfo.frontImage;
	_isHaveFrontImage	= elemInfo.isHaveFrontImage;

	_underLine			= elemInfo.underLine;
}

void LayoutElement::getElem(LOElemInfo& elemInfo)
{
	elemInfo.area			= _area;

	elemInfo.elemType		= _elemType;
	elemInfo.vAlign			= _vAlign;
	
	elemInfo.font			= _font;
	elemInfo.color			= _color;
	
	elemInfo.imageInfo		= _imageInfo;
	elemInfo.gameObj		= _gameObj;
	
	elemInfo.isShowDes		= _isShowDes;
	
	elemInfo.content		= _content;
	elemInfo.description	= _description;

	elemInfo.lineExtHeight	= _lineExtHeight;
	elemInfo.inheritslineExtHeight = _inheritsLineExtheight;

	elemInfo.inheritsWordExtSpace = _inheritsWordExtSpace;

	elemInfo.backImagePath		= _backImagePath;	 
	elemInfo.backImage			= _backImage;		 
	elemInfo.isHaveBackImage	= _isHaveBackImage;

	elemInfo.frontImagePath		= _frontImagePath;
	elemInfo.frontImage			= _frontImage;		 
	elemInfo.isHaveFrontImage	= _isHaveFrontImage;

	elemInfo.underLine			= _underLine;
}

int LayoutElement::getWordCount()
{
	if(true == _isShowDes || _elemType == LO_IMAGE)
	{
		return 1;
	}
	else
	{
		return wcslen(getContent());
	}
}
///////////////////////elem部分(end)///////////////////////////////////////////////////

///////////////////////seg部分(begin)///////////////////////////////////////////////////

LayoutSeg::LayoutSeg()
{
	_hAlign = LO_HA_LEFT;
	_float = FLOAT_NONE;
	_elemList.clear();
	
	_lineExtHeight = 0;
	
	_wordExtSpace = 0;
}

LayoutSeg::LayoutSeg(const LayoutSeg& other)
{
	_area				= other._area;
	
	_float				= other._float;
	_hAlign				= other._hAlign;
	
	_backgroundColor	= other._backgroundColor;
	_frameColor			= other._frameColor;

	_elemList.clear();
	_elemList.reserve(other._elemList.size());
	for(int i = 0; i < other._elemList.size(); ++i)
	{
		_elemList.push_back(other._elemList[i]);
	}

	_segTextFont = other._segTextFont;
	_segTextColor = other._segTextColor;

	_lineExtHeight = other._lineExtHeight;
	
	_wordExtSpace = other._wordExtSpace;
}

LayoutSeg::~LayoutSeg()
{
	_elemList.clear();
}

void LayoutSeg::addElem(const LayoutElement& newElem)
{
	_elemList.push_back(newElem);
}

void LayoutSeg::setProperty(char* property)
{	
	if(NULL == property)
	{
		_ASSERT(0);
		return;
	}

	char propertyName[16]  = {0};
	char propertyValue[512] = {0};
	
	sscanf( property, PORPERTY_PARSER_TEXT, propertyName, propertyValue);

	if(strlen(propertyName) == 0 || strlen(propertyValue) == 0)
	{
		return;
	}

	//对齐方式
	if(strcmp(propertyName, TextHAlign) == 0 || strcmp(propertyName, TextHAlignS) == 0)
	{
		if(strcmp(propertyValue, "left") == 0)
			_hAlign = LO_HA_LEFT;
		else if(strcmp(propertyValue, "right") == 0)
			_hAlign = LO_HA_RIGHT;
		else if(strcmp(propertyValue, "center") == 0)
			_hAlign = LO_HA_CENTER;
		else if(strcmp(propertyValue, "justify") == 0)
			_hAlign = LO_HA_CENTER;
	}
	else if(strcmp(propertyName, LAYOUT_FLOAT) == 0 || strcmp(propertyName, LAYOUT_FLOATS) == 0)
	{
		if(strcmp(propertyValue, "none") == 0)
			_float = FLOAT_NONE;
		else if(strcmp(propertyValue, "left") == 0)
			_float = FLOAT_LEFT;
		else if(strcmp(propertyValue, "right") == 0)
			_float = FLOAT_RIGHT;
		else if(strcmp(propertyValue, "wrap") == 0)
			_float = FLOAT_WRAP;
	}
	else if(strcmp(propertyName, SEG_WIDTH) == 0 || strcmp(propertyName, SEG_WIDTHS) == 0)
	{
		int segWidth;
		sscanf(propertyValue, "%d", &segWidth);
		segWidth = segWidth >= 0 ? segWidth : 0;
		_area.setWidth(segWidth);
	}
	else if(strcmp(propertyName, SEG_COLOR) == 0 || strcmp(propertyName, SEG_COLORS) == 0)
	{
		sscanf(propertyValue, "%f,%f,%f", &_segTextColor.red, &_segTextColor.green, &_segTextColor.blue);
		_segTextColor.red		/= 255;
		_segTextColor.green		/= 255;
		_segTextColor.blue		/= 255;
	}
	else if(strcmp(propertyName, FrameColor) == 0 || strcmp(propertyName, FrameColorS) == 0)
	{
		sscanf(propertyValue, "%f,%f,%f", &_frameColor.red, &_frameColor.green, &_frameColor.blue);
		_frameColor.red		/= 255;
		_frameColor.green	/= 255;
		_frameColor.blue	/= 255;
	}	
	else if(strcmp(propertyName, FontFamily) == 0 || strcmp(propertyName, FontFamilyS) == 0)
	{
		strcpy(_segTextFont.family, propertyValue);
	}
	else if(strcmp(propertyName, Color) == 0 || strcmp(propertyName, ColorS) == 0)
	{
		//颜色
		if(sscanf(propertyValue, "%f,%f,%f", &_segTextColor.red, &_segTextColor.green, &_segTextColor.blue) == 3)
		{
			_segTextColor.red		/= 255;
			_segTextColor.green		/= 255;
			_segTextColor.blue		/= 255;
		}
		else
		{
			_ASSERT(0);
		}
	}
	else if(strcmp(propertyName, ColorNum) == 0 || strcmp(propertyName, ColorNumS) == 0)
	{
		//颜色
		unsigned int color;
		if(sscanf(propertyValue, "%x", &color))
		{
			_segTextColor.red		= (color & 0x00ff0000) >> 16;
			_segTextColor.green		= (color & 0x0000ff00) >> 8;
			_segTextColor.blue		= (color & 0x000000ff);
			_segTextColor.red		/= 255;
			_segTextColor.green		/= 255;
			_segTextColor.blue		/= 255;
		}
		else
		{
			_ASSERT(0);
		}
	}
	else if(strcmp(propertyName, LineExtHeight) == 0 || strcmp(propertyName, LineExtHeightS) == 0)
	{
		sscanf(propertyValue, "%d", &_lineExtHeight);
		_lineExtHeight = _lineExtHeight >= 0 ? _lineExtHeight : 0;
	}
	else if(strcmp(propertyName, WordExtSpace) == 0 || strcmp(propertyName, WordExtSpaceS) == 0)
	{
		sscanf(propertyValue, "%d", &_wordExtSpace);
		_wordExtSpace = _wordExtSpace >= 0 ? _wordExtSpace : 0;
	}
}

int LayoutSeg::getWordCount()
{
	int wordCount = 0;
	for(int i = 0; i < _elemList.size(); ++i)
	{
		wordCount += _elemList[i].getWordCount();
	}
	return wordCount;
}

void LayoutSeg::inheritsColor()
{
	for(int i = 0; i < _elemList.size(); ++i)
	{
		LayoutElement& elem = _elemList[i];
		if(elem.getInheritsColor())
		{
			elem.setColor(_segTextColor);
		}
	}
}

void LayoutSeg::inheritsFont()
{
	if(0 == _segTextFont.family[0])
	{
		return;
	}

	for(int i = 0; i < _elemList.size(); ++i)
	{
		LayoutElement& elem = _elemList[i];
		if(0 == elem.getFont().family[0])
		{
			elem.setFont(_segTextFont);
		}
	}
}

void LayoutSeg::inheritsLineExtHeight()
{
	for(int i = 0; i < _elemList.size(); ++i)
	{
		LayoutElement& elem = _elemList[i];
		if(elem.getInheritsLineExtheight())
		{
			elem.setLineExtHeight(_lineExtHeight);
		}
	}
}

void LayoutSeg::inheritsWordExtSpace()
{
	for(int i = 0; i < _elemList.size(); ++i)
	{
		LayoutElement& elem = _elemList[i];
		if(elem.getInheritsWordExtSpace())
		{
			elem.setWordExtSpace(_wordExtSpace);
		}
	}
}
///////////////////////seg部分(end)///////////////////////////////////////////////////

///////////////////////data部分(begin)///////////////////////////////////////////////////

LayoutData::LayoutData()
{
	clear();
}

LayoutData::~LayoutData()
{
	_segList.clear();
}

void LayoutData::clear()
{
	_marginLeft		= 0;
	_marginRight	= 0;
	_marginTop		= 0;
	_marginBottom	= 0;
	_width			= DefaultWndWidth;
	_height			= DefaultWndHeight;
	_segList.clear();
};

void LayoutData::addASeg(LayoutSeg& seg)
{
	_segList.push_back(seg);
}

void LayoutData::setHeadProperty(char* headProperty)
{	
	if(NULL == headProperty)
	{
		_ASSERT(0);
		return;
	}

	char propertyName[16]  = {0};
	char propertyValue[16] = {0};
	
	sscanf(headProperty, PORPERTY_PARSER_TEXT, propertyName, propertyValue);

	if(strlen(propertyName) == 0 || strlen(propertyValue) == 0)
	{
		return;
	}

	if(strcmp(propertyName, WndWidth) == 0 || strcmp(propertyName, WndWidthS) == 0)
	{
		_width = 0;
		sscanf(propertyValue, "%d", &_width);
		_width = _width >= 0 ? _width : 0;
	}
	else if(strcmp(propertyName, WndHeight) == 0 || strcmp(propertyName, WndHeightS) == 0)
	{
		sscanf(propertyValue, "%d", &_height);
		_height = _height >= 0 ? _height : 0;
	}
	else if(strcmp(propertyName, WndMarginLeft) == 0 || strcmp(propertyName, WndMarginLeftS) == 0)
	{
		sscanf(propertyValue, "%d", &_marginLeft);
		_marginLeft = _marginLeft >= 0 ? _marginLeft : 0;
	}
	else if(strcmp(propertyName, WndMarginRight) == 0 || strcmp(propertyName, WndMarginRightS) == 0)
	{
		sscanf(propertyValue, "%d", &_marginRight);
		_marginRight = _marginRight >= 0 ? _marginRight : 0;
	}
	else if(strcmp(propertyName, WndMarginTop) == 0 || strcmp(propertyName, WndMarginTopS) == 0)
	{
		sscanf(propertyValue, "%d", &_marginTop);
		_marginTop = _marginTop >= 0 ? _marginTop : 0;
	}
	else if(strcmp(propertyName, WndMarginBottom) == 0 || strcmp(propertyName, WndMarginBottomS) == 0)
	{
		sscanf(propertyValue, "%d", &_marginBottom);
		_marginBottom = _marginBottom >= 0 ? _marginBottom : 0;
	}
}

int LayoutData::getActRenderHeight()
{
	int height = 0;
	for(int i = 0; i< _segList.size(); ++i)
	{
		int segBottom = _segList[i].getArea().getBottom();
		if(segBottom > height)
			height = segBottom;
	}
	return height - _marginTop;	
};

int	LayoutData::getActRenderWidth(bool adjWidth)
{
	if(false == adjWidth)
	{
		return  _width - _marginLeft - _marginRight;
	}
	
	int width = 0;
	for(int i = 0; i< _segList.size(); ++i)
	{
		int setRight = _segList[i].getArea().getRight();
		if(setRight > width)
			width = setRight;
	}
	return width - _marginLeft;	
}

void LayoutData::inheritsProperty()
{
	for(int i = 0; i< _segList.size(); ++i)
	{
		LayoutSeg& seg = _segList[i];
		seg.inheritsColor();
		seg.inheritsFont();
		seg.inheritsLineExtHeight();
		seg.inheritsWordExtSpace();
	}
}