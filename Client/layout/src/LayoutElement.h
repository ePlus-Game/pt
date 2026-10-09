/////////////////////////////////////////////////////////////////////////////
//  FileName    :   LayoutElement.h
//  Creator     :   xiehong
//  Date        :   2006-15-18 16:00
//  Comment     :   element declare
//	Changes		:	
/////////////////////////////////////////////////////////////////////////////

#ifndef _LAYOUT_ELEMENT_
#define _LAYOUT_ELEMENT_

#include <vector>
#include "layoutdef.h"

/////////////////////////////////////////////////////////////////////////////
//
//              Class Define
//
/////////////////////////////////////////////////////////////////////////////

class LayoutElement
{
	LORect			_area;
	
	LOElementType	_elemType;
	LOVerticalAlign	_vAlign;
	
	LOFont			_font;
	LOColor			_color;
	bool			_inheritsColor;		//是否是通过属性设置过的color

	float			_alpha;
	
	LOImageInfo		_imageInfo;
	LOGameObject	_gameObj;

	bool			_isShowDes;


	LOString		_content;
	LOString		_description;

	unsigned char	_lineExtHeight;		//每行的扩展高度
	bool			_inheritsLineExtheight;

	bool			_inheritsWordExtSpace;
	
	bool			_isHaveBackImage;
	LOString		_backImagePath;
	LOImageInfo		_backImage;

	bool			_isHaveFrontImage;
	LOString		_frontImagePath;
	LOImageInfo		_frontImage;

	bool			_underLine;
public:
	LayoutElement();
	LayoutElement(const LayoutElement& elem);
	~LayoutElement();

	//content
	void			setContent(const char* content);
	void			setContent(const wchar_t* content);
	const wchar_t*	getContent()const {	return _content.get();	};
	void			extentContent(wchar_t* newContetn, int startIndex);
	
	//description
	void			setDescription(const char* newDescription);
	void			setDescription(const wchar_t* newDescription);
	const wchar_t*	getDescription()const {	return _description.get();	};

	LORect&	getArea()											{	return _area;					};
	void			setArea(const LORect& area)					{	_area = area;					};

	const LOElementType&	getElemType() const					{	return _elemType;				};
	void			setElemType(const LOElementType& elemType)	{	_elemType = elemType;			};

	LOVerticalAlign&getVAlign()									{	return _vAlign;					};
	void			setVAlign(const LOVerticalAlign& vAlign)	{	_vAlign = vAlign;				};

	const LOFont&	getFont()									{	return _font;					};
	void			setFont(const LOFont& font)					{	_font = font;					};

	const LOColor&	getColor()									{	return _color;					};
	void			setColor(const LOColor& color)				{	_color = color;					};
	bool			getInheritsColor()							{	return _inheritsColor;			};

	float			getAlpha()									{	return _alpha;					};
	void			setAlpha(float alpha)						{	_alpha = alpha;					};

	LOImageInfo&	getImageInfo()								{	return _imageInfo;				};
	void			setImageInfo(const LOImageInfo& imageInfo)	{	_imageInfo = imageInfo;			};

	const LOGameObject&	getGameObject()							{	return _gameObj;				};
	void			setGameObject(const LOGameObject& go)		{	_gameObj = go;					};

	bool			getShowDes()								{	return _isShowDes;				};
	void			setShowDes(bool isShowDes)					{	_isShowDes = isShowDes;			};

	unsigned char	getLineExtHeight()							{	return _lineExtHeight;			};
	void			setLineExtHeight(unsigned char extHeght)	{	_lineExtHeight = extHeght;		};
	bool			getInheritsLineExtheight()					{	return _inheritsLineExtheight;	};

	unsigned char	getWordExtSpace()							{	return _font.wordExtSpace;		};
	void			setWordExtSpace(unsigned char wordExtSpace)	{	_font.wordExtSpace = wordExtSpace;	};
	bool			getInheritsWordExtSpace()					{	return _inheritsWordExtSpace;	};

	LOImageInfo&	getBackImage()								{	return _backImage;				};
	LOImageInfo&	getFrontImage()								{	return _frontImage;				};
	void			setBackImage(const LOImageInfo& imageInfo)	{	_backImage = imageInfo;			};
	void			setFrontImage(const LOImageInfo& imageInfo)	{	_frontImage = imageInfo;		};
	
	const wchar_t*	getBackImagePath()							{	return _backImagePath.get();	};
	const wchar_t*	getFrontImagePath()							{	return _frontImagePath.get();	};

	bool			isHaveBackImage()							{	return _isHaveBackImage;		};
	bool			isHaveFrontImage()							{	return _isHaveFrontImage;		};

	bool			isDrawUnderLine()							{	return _underLine;				};
	

	void			setProperty(char* property);
	
	int				getWordCount();

	void			setElem(const LOElemInfo& elemInfo);
	void			getElem(LOElemInfo& elem);
};

class LayoutSeg
{
	LORect			_area;

	LOHAlign		_hAlign;
	LOFloat			_float;
	
	LOColor			_backgroundColor;
	LOColor			_frameColor;

	LOColor			_segTextColor;
	LOFont			_segTextFont;
	
	unsigned char	_lineExtHeight;	//每行的扩展高度
	unsigned char	_wordExtSpace;	//字间距

	std::vector<LayoutElement> _elemList;
	LOImageInfo		_back;
	LOImageInfo		_front;
public:
	LayoutSeg();
	LayoutSeg(const LayoutSeg& other);
	~LayoutSeg();
	LORect&			getArea()					{	return _area;			};
	void			setArea(LORect newArea)		{	_area = newArea;		};

	LOHAlign		getHAlign()	const			{	return _hAlign;			};
	LOFloat			getFLoat()	const			{	return _float;			};

	void			setProperty(char* property);

	void			addElem(const LayoutElement& newElem);

	int				getWordCount();
	inline std::vector<LayoutElement>& getElems(){	return _elemList;		};

	void			inheritsColor();
	void			inheritsFont();
	void			inheritsLineExtHeight();
	void			inheritsWordExtSpace();
};

class LayoutData
{
	int _width;
	int _height;

	int _marginLeft;
	int _marginRight;
	int _marginTop;
	int _marginBottom;

	std::vector<LayoutSeg> _segList;

public:
	LayoutData();
	~LayoutData();

	void	addASeg(LayoutSeg& seg);
	
	void	clear();

	int		getWidth(){		return _width;	};
	
	void	setHeight(int newHeight){	_height = newHeight;	};
	int		getHeight(){	return _height;	};

	int		getActRenderHeight();
	int		getActRenderWidth(bool adjWidth);
	
	void	setHeadProperty(char* headProperty);
	
	int		getMarginL(){	return _marginLeft;		}
	int		getMarginR(){	return _marginRight;	}
	int		getMarginT(){	return _marginTop;		}
	int		getMarginB(){	return _marginBottom;	}
	
	void	inheritsProperty();
	inline std::vector<LayoutSeg>& getSegs()
	{	
		return _segList;
	};
};

#endif