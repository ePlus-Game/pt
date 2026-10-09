/////////////////////////////////////////////////////////////////////////////
//  FileName    :   layoutinterface.h
//  Creator     :   zuolizhi
//  Date        :   2006-12-13 9:54:00
//  Comment     :   Interface Declare
//	Changes		:	
/////////////////////////////////////////////////////////////////////////////
#ifndef _LAYOUT_INTERFACE_H_
#define _LAYOUT_INTERFACE_H_

#include <vector>
#define interface struct

/////////////////////////////////////////////////////////////////////////////
//
//              Interface Declare
//
/////////////////////////////////////////////////////////////////////////////

#define LAYOUT_SHORT_TEXT_LEN	32
#define MAX_TEXT_LEN	1024 * 5
#define LAYOUT_ONEK		1024
#define LAYOUT_TEXT_MAX_LEN (LAYOUT_ONEK * 50)
#define LAYOUT_GAME_OBJECT_MAX_ID_COUNT 5
//排版内部全部为UTF8
//所以接口为char*
//接口用到的结构…………begin
struct LOColor
{
	float red;
	float green;
	float blue;

	LOColor();
	LOColor(const char* color);
	bool operator==(const LOColor& other);
};

enum LOGameObjType
{
	LO_GO_NOTHING = 0,
	LO_GO_PLAYER,
	LO_GO_ITEM,
	LO_GO_FACE,
	LO_GO_CHANNEL,
	LO_GO_POSITION,
	LO_GO_PORTRAIT,
	LO_GO_MAP,
	LO_GO_SKILL,
	LO_GO_TASK,
	LO_GO_NPC,
	LO_GO_SOCIAL_OWNER,
};

struct LOGameObject
{
	LOGameObjType	_objType;
	int				_objId[LAYOUT_GAME_OBJECT_MAX_ID_COUNT];
	LOGameObject();
	LOGameObject(const LOGameObject& other);
	bool operator==(const LOGameObject& other);
};

enum LOFontStyle
{
	NORMAL,
	ITALIC,
	OBLIQUE,
};

struct LOFont
{
	LOFontStyle style;
	int variant;
	int weight;
	int size;
	int height;
	unsigned char wordExtSpace;
	char family[LAYOUT_SHORT_TEXT_LEN];
public:
	LOFont();
	LOFont(const LOFont& newFont);
	bool operator==(const LOFont& other);
};

struct LOImageInfo
{
	int frameCount;
	int	interval;
	long lastDrawTime;
	LOImageInfo();
	LOImageInfo(const LOImageInfo& other);
	bool operator==(const LOImageInfo& other);
};

struct LOPoint
{
	int x;
	int y;
	LOPoint();
	LOPoint(int newx, int newy);
	LOPoint(const LOPoint& point);
};

class LORect
{
	LOPoint _topLeftPos;
	int _width;
	int _height;
	
public:
	LORect();
	
	LORect(const LORect& area);
	LORect(int x, int y, int w, int h);
	void setPos(LOPoint newPos);
	void setPos(int x, int y);

	void setWidth(int width);
	void setHeight(int height);
	int getLeft()	const	;
	int getTop()	const	;
	int getBottom()	const	;
	int getRight()	const	;
	int getWidth()	const	;
	int getHeight()	const	;
	LOPoint getPosition()const;

	bool operator<(const LORect& otherRect);

	bool operator>(const LORect& otherRect);

	bool operator==(const LORect& otherRect);

	LORect offset(LOPoint& off);

	void add(LORect& newRect);	
};

enum LOHAlign
{
	LO_HA_LEFT,
	LO_HA_RIGHT,
	LO_HA_CENTER,
};

enum LOElementType
{
	LO_IMAGE,
	LO_TEXT,
};

enum LOFloat
{
	FLOAT_NONE,
	FLOAT_LEFT,
	FLOAT_RIGHT,
	FLOAT_WRAP,
};

enum LOVerticalAlign
{
	LO_VA_TOP,
	LO_VA_BOTTOM,
	LO_VA_CENTER,
};

class LOString
{
	std::vector<wchar_t>	_string;

public:
	LOString();
	~LOString();
	LOString(const LOString& other);
	LOString(const char* content)	{	set(content);	};
	LOString(const wchar_t* content){	set(content);	};

	int		set(const char* content);
	int		set(const wchar_t* content);
	void	extentContent(const wchar_t* newContetn, int startIndex);
	const wchar_t*	get()const {	return &_string[0];	};
	int		len();
	wchar_t& operator[](int index);
	bool operator==(const LOString& other);
	LOString& operator=(const LOString& other);
};

struct LOElemInfo
{
public:
	LOString		content;
	LOString		description;

	LORect			area;
	
	LOElementType	elemType;
	LOVerticalAlign	vAlign;
	
	LOFont			font;
	LOColor			color;
	bool			inheritsColor;
	
	LOImageInfo		imageInfo;
	LOGameObject	gameObj;

	bool			isShowDes;

	unsigned char	lineExtHeight;
	bool			inheritslineExtHeight;

	bool			inheritsWordExtSpace;

	bool			isHaveBackImage;
	LOString		backImagePath;
	LOImageInfo		backImage;

	bool			isHaveFrontImage;
	LOString		frontImagePath;
	LOImageInfo		frontImage;

	bool			underLine;

	LOElemInfo();
	LOElemInfo(const LOElemInfo& elem);
	bool operator==(const LOElemInfo& other);
	bool operator!=(const LOElemInfo& other);
};


//接口用到的结构…………end

//接口…………begin
interface ILayoutBase
{
	virtual void Release( )	=	0;
};

interface ILayoutRender : ILayoutBase
{
	virtual LORect	getWordSize(
		unsigned short codePoint,
		const LOFont& font)			= 0;
	
	virtual int		getLineHeight(
		const LOFont& font)			= 0;

	virtual int		getTextExtent(
		const unsigned short* text,
		const LOFont& font)			= 0;

	//得到一个字符串上指定坐标的字符
	virtual int		getCharAtPixel(
		const unsigned short* text, 
		int startCharIndex,  
		int pixel,
		const LOFont& font)			= 0;

	virtual void	drawText(
		const unsigned short* text,
		const LOFont& font,
		const LORect& destArea,
		const LOColor& color,
		float alpha,
		const LORect& clipper,
		float zPos,
		int borderMode,
		bool underLine)					= 0;

	
	virtual LORect	getImageArea(
		const unsigned short* imageName)		= 0;

	
	virtual LOImageInfo getImageInfo(
		const unsigned short* imageName)		= 0;

	//在指定位置画一幅图片
	virtual void	drawImage(
		const unsigned short* imageName, 
		LORect& destArea,
		const LOColor& color, 
		float alpha,
		LOImageInfo& imageInfo, 
		LORect& clipper, 
		float zPos)					= 0;

	virtual ~ILayoutRender(){}
};

interface ILayout : ILayoutBase
{
	virtual void SetText(
		char* szText )		= 0;

	virtual void Render(
		int x,
		int y,
		float z)			= 0;

	virtual LORect getRenderArea(
		bool adjWidth = true
		)					= 0;

	virtual void setClipper(
		LORect& clipper)	=0;

	virtual bool pickupElem(
		int x, 
		int y, 
		LOElemInfo& elemInfo) = 0;

	virtual void setSelection(
		int stratIndex,
		int endIndex)		=0;

	virtual void getSelection(
		int& stratIndex,
		int& endIndex)		=0;

	virtual void eraseSelection(
						)	=0;

	virtual void clearLayout(
							)=0;

	virtual LOPoint getPosAtWordIndex(
		int wordIndex		)=0;

	virtual void showCarat(
		bool show,
		bool select)		=0;
	
	virtual bool isHaveContent(
		)					= 0;

	virtual void setColor(
		LOColor color,
		float alpha = 1.0f
		)					= 0;

	virtual void setAlpha(
		float alpha
		)					= 0;

	virtual void insertElem(
		LOElemInfo& 
		)					=0;

	virtual void flashLayout(
		)					= 0;

	virtual int getElemList(
		LOElemInfo*& elems
		)					= 0;
	
	virtual int wordIndexAtPixel(
		int x, 
		int y)				= 0;
	
	virtual int	getWordCount(
		)					= 0;

	virtual void formatText(
		char* text
		)					= 0;

	virtual void setBorderMode(
		int mode
		)					= 0;

	virtual ~ILayout(){}
};

void CreateLayout( 
		ILayout** pLayout,
		ILayoutRender* pRender );

//接口…………end

#endif