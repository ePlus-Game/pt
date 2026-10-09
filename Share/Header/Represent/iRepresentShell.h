/*****************************************************************************************
//  表现模块的对外接口。
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-11-11
------------------------------------------------------------------------------------------
*****************************************************************************************/

#ifndef IREPRESENTSHELL_H_HEADER_INCLUDED_C2311ED3
#define IREPRESENTSHELL_H_HEADER_INCLUDED_C2311ED3

struct KRepresentUnit;
struct KImageParam;
#include "FontDef.h"
#include "TextProcessDef.h"
#include "windows.h"
struct KRPosition2;
#define TEXT_IN_SINGLE_PLANE_COORD -32767
#define LIGHTING_GRID_SIZEX			32							// 计算光照信息格子的大小，必须能整除512
#define LIGHTING_GRID_SIZEY			32							// 计算光照信息格子的大小，必须能整除512
//RWPP_AREGION_WIDTH和RWPP_AREGION_HEIGHT要与KScenePlaceRegionC.h中的值保持一致
#define AREGION_WIDTH	512
#define AREGION_HEIGHT	1024
#define LIGHTING_GRID_WIDTH	(AREGION_WIDTH / LIGHTING_GRID_SIZEX * 3)	// 九个焦点区域安格子计算的宽度
#define LIGHTING_GRID_HEIGHT (AREGION_HEIGHT / LIGHTING_GRID_SIZEY * 3)	// 九个焦点区域安格子计算的高度

//屏幕截图文件类型
enum ScreenFileType
{
	SCRFILETYPE_BMP,
	SCRFILETYPE_JPG,
};

enum RepresentOption
{
	DYNAMICLIGHT = 0,
	PERSPECTIVE,
	MOTIONBLUR,
	CAMERADISTANCE,
	CAMERADISTANCE_UP,
	CAMERADISTANCE_DOWN,
	HIGH_QUALITY_PAINTING,
	FRAMECUT_LEVE,
};

//-----数据缓冲区的数据格式枚举------
enum	BITMAP_DATA_BUFFER_FORMAT
{
	BDBF_16BIT_555	= 1,
	BDBF_16BIT_565,
	BDBF_16BIT_4444,
	DBDF_24BIT,
	DBDF_32BIT,
};

struct KBitmapDataBuffInfo
{
	int		nWidth;			//图形宽度
	int		nHeight;		//图形高度
	int		nPitch;			//图形缓冲区每横行数据的起点的间隔
	BITMAP_DATA_BUFFER_FORMAT	eFormat;	//数据格式，取值来自枚举BITMAP_DATA_BUFFER_FORMAT
	void*	pData;			//数据区指针
};

struct IInlinePicEngineSink;

//////////////////////////////////////////////////////////////////////////
// 资源内存驻留参数
// Added By Rocker 2004.3.12
struct MemoryResidentParam
{
	bool bStatic;							// 是否是静态的资源
	unsigned int nValidTime;				// 驻留在内存中的最大时间，
	// 如果在这个时间段里面没有被使用，就完全释放掉
};

//////////////////////////////////////////////////////////////////////////
// 图形聊天室接口
struct iGraphChatRoom
{
	virtual void	Update() = 0;						// 更新聊天场景
	virtual void	Render() = 0;						// 把聊天场景显示出来
	virtual void	RotateCamera(float fDegree) = 0;	// 按Z轴旋转聊天场景
	virtual void	Release() = 0;						// 释放
};

#ifdef _New3DClient
struct KMeshParam
{
	int		nToltalFrame;	
};

struct TerrianOption
{
	bool	bWire;
	bool	bAutoLod;
	bool	bFall;
};

#endif

//## 表现模块的对外接口。
struct iRepresentShell
{
  public:
    //## 释放销毁接口对象
    virtual void Release() = 0;

//---------绘图设备相关---------------

    //## 创建绘图设备，构造绘图环境。
    virtual bool Create(
        //## 设备（窗口）横宽（单位：像素点）
        int nWidth, 
        //## 设备（窗口）纵宽（单位：像素点）
        int nHeight, 
        //## 是否独占全屏幕
        bool bFullScreen) = 0;

    //## 重置绘图设备
    virtual bool Reset(
        //## 设备（窗口）横宽（单位：像素点）
        int nWidth, 
        //## 设备（窗口）纵宽（单位：像素点）
        int nHeight, 
        //## 是否独占全屏幕
        bool bFullScreen) = 0;
	//add by zhangpengcheng 处理DX表面丢失//
	virtual bool ProcessSurfaceLost(int nWidth,int nHeight,bool bFullScreen) = 0;

//---------字体与文字输出---------------

    //## 创建一个字体对象。
    virtual bool CreateAFont(
        //## 字库文件名。
        const char* pszFontFile, 
        //## 字库使用的字符编码集。
        CHARACTER_CODE_SET CharaSet, 
        //## 字体对象id.
        int nId) = 0;

    //## 输出文字。
    virtual void OutputText(
        //## 使用的字体对象id。
        int nFontId, 
        //## 要输出的字符串。
        const char* psText, 
        //## 要输出的字符串的长度(BYTE)。
        //## 当nCount大于等于0时，字符串可以不包括特殊的结束符号来表示输出字符的结束。
        //## 当nCount小于0时，表示此字符串是以'\0'结尾，将根据结束字符来确定输出字符串的长度。
        //## 默认值为-1。
        int nCount = KRF_ZERO_END, 
        //## 字符串显示起点坐标X，如果传入值为KF_FOLLOW，
        //## 则此字符串紧接在上次字符串的输出位置之后。
        //## 默认值为KRF_FOLLOW。
        int nX = KRF_FOLLOW, 
        //## 字符串显示起点坐标Y, 如果传入值为KF_FOLLOW，
        //## 此字符串与前一次输出字符串在同一行的位置。
        //## 默认值为KRF_FOLLOW。
        int nY = KRF_FOLLOW, 
        //## 字符串显示颜色，默认为黑色，用32bit数以ARGB的格
        //## 式表示颜色，每个分量8bit。
        unsigned int Color = 0xFF000000, 
        //## 自动换行的行宽限制，如果其值小于一个全角字符宽度则不做自动换行处理。默认值为0，既不做自动换行处理。
        int nLineWidth = 0,
		int nZ = TEXT_IN_SINGLE_PLANE_COORD,
		//字的边缘颜色
		unsigned int BorderColor = 0) = 0;

    //## 输出文字。
    virtual void OutputTextOnImage(
		const char* szImage,
		unsigned int uImage,
		short& nPosition,	
        //## 使用的字体对象id。
        int nFontId, 
        //## 要输出的字符串。
        const char* psText, 
        //## 要输出的字符串的长度(BYTE)。
        //## 当nCount大于等于0时，字符串可以不包括特殊的结束符号来表示输出字符的结束。
        //## 当nCount小于0时，表示此字符串是以'\0'结尾，将根据结束字符来确定输出字符串的长度。
        //## 默认值为-1。
        int nCount = KRF_ZERO_END, 
        //## 字符串显示起点坐标X，如果传入值为KF_FOLLOW，
        //## 则此字符串紧接在上次字符串的输出位置之后。
        //## 默认值为KRF_FOLLOW。
        int nX = KRF_FOLLOW, 
        //## 字符串显示起点坐标Y, 如果传入值为KF_FOLLOW，
        //## 此字符串与前一次输出字符串在同一行的位置。
        //## 默认值为KRF_FOLLOW。
        int nY = KRF_FOLLOW, 
        //## 字符串显示颜色，默认为黑色，用32bit数以ARGB的格
        //## 式表示颜色，每个分量8bit。
        unsigned int Color = 0xFF000000, 
        //## 自动换行的行宽限制，如果其值小于一个全角字符宽度则不做自动换行处理。默认值为0，既不做自动换行处理。
        int nLineWidth = 0,
		int nZ = TEXT_IN_SINGLE_PLANE_COORD,
		//字的边缘颜色
		unsigned int BorderColor = 0) = 0;
	
	//## 输出文字。
    virtual void OutputTextInRect(
        //## 使用的字体对象id。
        int nFontId, 
        //## 要输出的字符串。
        const char* psText, 
        //## 要输出的字符串的长度(BYTE)。
        //## 当nCount大于等于0时，字符串可以不包括特殊的结束符号来表示输出字符的结束。
        //## 当nCount小于0时，表示此字符串是以'\0'结尾，将根据结束字符来确定输出字符串的长度。
        //## 默认值为-1。
        int nCount = KRF_ZERO_END, 
        //## 字符串显示起点坐标X，如果传入值为KF_FOLLOW，
        //## 则此字符串紧接在上次字符串的输出位置之后。
        //## 默认值为KRF_FOLLOW。
        int nX = KRF_FOLLOW, 
        //## 字符串显示起点坐标Y, 如果传入值为KF_FOLLOW，
        //## 此字符串与前一次输出字符串在同一行的位置。
        //## 默认值为KRF_FOLLOW。
        int nY = KRF_FOLLOW, 
        //## 字符串显示颜色，默认为黑色，用32bit数以ARGB的格
        //## 式表示颜色，每个分量8bit。		
		RECT *pRect = NULL,	//裁减区域
        unsigned int Color = 0xFF000000, 
        //## 自动换行的行宽限制，如果其值小于一个全角字符宽度则不做自动换行处理。默认值为0，既不做自动换行处理。
        int nLineWidth = 0,
		int nZ = TEXT_IN_SINGLE_PLANE_COORD,
		//字的边缘颜色
		unsigned int BorderColor = 0) = 0;

	//## 输出文字。
    virtual int OutputRichTextInRect(
        //## 使用的字体对象id。
        int nFontId, 
		KOutputTextParam* pParam,
        //## 要输出的字符串。
        const char* psText, 
        //## 要输出的字符串的长度(BYTE)。
        //## 当nCount大于等于0时，字符串可以不包括特殊的结束符号来表示输出字符的结束。
        //## 当nCount小于0时，表示此字符串是以'\0'结尾，且中间不存在'\0'的字符。
        //## 默认值为-1。		
        int nCount = KRF_ZERO_END,
		//裁减区域
		RECT *pRect = NULL,
        //##Documentation
        //## 自动换行的行宽限制，如果其值小于一个全角字符宽度则不做自动换行处理。默认值为0，既不做自动换行处理。
        int nLineWidth = 0) = 0;

    //## 输出文字。
    virtual int OutputRichText(
        //## 使用的字体对象id。
        int nFontId, 
		KOutputTextParam* pParam,
        //## 要输出的字符串。
        const char* psText, 
        //## 要输出的字符串的长度(BYTE)。
        //## 当nCount大于等于0时，字符串可以不包括特殊的结束符号来表示输出字符的结束。
        //## 当nCount小于0时，表示此字符串是以'\0'结尾，且中间不存在'\0'的字符。
        //## 默认值为-1。		
        int nCount = KRF_ZERO_END, 
        //##Documentation
        //## 自动换行的行宽限制，如果其值小于一个全角字符宽度则不做自动换行处理。默认值为0，既不做自动换行处理。
        int nLineWidth = 0) = 0;

	//## 输出文字。
    virtual int OutputRichTextOnImage(
        const char* szImage,
		unsigned int uImage,
		short& nPosition,	
		//## 使用的字体对象id。
        int nFontId, 
		KOutputTextParam* pParam,
        //## 要输出的字符串。
        const char* psText, 
        //## 要输出的字符串的长度(BYTE)。
        //## 当nCount大于等于0时，字符串可以不包括特殊的结束符号来表示输出字符的结束。
        //## 当nCount小于0时，表示此字符串是以'\0'结尾，且中间不存在'\0'的字符。
        //## 默认值为-1。		
        int nCount = KRF_ZERO_END, 
        //##Documentation
        //## 自动换行的行宽限制，如果其值小于一个全角字符宽度则不做自动换行处理。默认值为0，既不做自动换行处理。
        int nLineWidth = 0) = 0;
    //## 返回指定坐标在字符串中最近的字符偏移(-1为失败)
    virtual int LocateRichText(
		//## 指定的坐标
		int nX, int nY,

        //## 使用的字体对象id。
        int nFontId, 
		KOutputTextParam* pParam,
        //## 要输出的字符串。
        const char* psText, 
        //## 要输出的字符串的长度(BYTE)。
        //## 当nCount大于等于0时，字符串可以不包括特殊的结束符号来表示输出字符的结束。
        //## 当nCount小于0时，表示此字符串是以'\0'结尾，且中间不存在'\0'的字符。
        //## 默认值为-1。		
        int nCount = KRF_ZERO_END, 
        //##Documentation
        //## 自动换行的行宽限制，如果其值小于一个全角字符宽度则不做自动换行处理。默认值为0，既不做自动换行处理。
        int nLineWidth = 0) = 0;

    //## 释放一个字体对象
    virtual void ReleaseAFont(
        //## 字体对象的id
        int nId) = 0;
	
//---------图资源相关---------------

    //## 如果内存图形对象失败，则返回0，否则返回根据图形名字转换而成的数值id。
    virtual unsigned int CreateImage(
        //## 图形的名字。
        const char* pszName, 
        //## 图形横宽
        int nWidth, 
        //## 图形纵宽
        int nHeight, 
        //## 图形类型
        int nType) = 0;

    //## 指定释放某个图资源。
    virtual void FreeImage(
        //## 图形文件名/图形名。
        const char* pszImage 
        ) = 0;

    //## 释放全部的图形对象。
    virtual void FreeAllImage() = 0;

    //## 获取图形像点数据缓冲区，返回空，则表示失败
    virtual void* GetBitmapDataBuffer(
        //## 图形名
        const char* pszImage,
		//用于获取图形数据缓冲区的相关信息数据，如果传入空指针，则忽略这些信息。
		KBitmapDataBuffInfo* pInfo) = 0;

	//##释放对(通过GetBitmapDataBuffer调用获取得的)图形像点数据缓冲区的控制
	virtual void ReleaseBitmapDataBuffer(
		//## 图形名
		const char* pszImage,
		//通过GetBitmapDataBuffer调用获取得的图形像点数据缓冲区指针
		void* pBuffer) = 0;

    //## 获取某个图形信息。
    virtual bool GetImageParam(
        //##Documentation
        //## 图形的资源文件名/图形名
        const char* pszImage,
        //##Documentation
        //## 图形信息存储结构的指针
        KImageParam* pImageData,
		//##Documentation
        //## 图形类型
        int nType) = 0;

#ifdef _New3DClient
	virtual bool GetMeshParam(
        //##Documentation
        //## 图形的资源文件名/图形名
        const char* pszMesh,
        //##Documentation
        //## 图形信息存储结构的指针
        KMeshParam* pMeshParam) = 0;

	virtual void UpdateMeshSkin(
		// 模型文件名
		const char* pszMesh, 
		// 目标皮肤名称
		const char* pszDestSkin) = 0;
#endif
	
    //## 获取某个图形某帧的信息
	virtual bool GetImageFrameParam(
        //## 指向保存图形资源文件名/图形名的缓冲区
        const char* pszImage,
		//图形帧索引
		int		nFrame,
        //## 帧图形相对于整个图形的偏移
		KRPosition2* pOffset,
		//## 帧图形大小
		KRPosition2* pSize,
        //## 图资源类型
		int nType) = 0;

    //## 获取某个图形某帧上指定像点的alpha值，如果不形不存在，或者象点落在图形范围之外则返回0。
    virtual int GetImagePixelAlpha(
        //## 图形资源文件名/图形名
        const char* pszImage, 
        //## 图形的帧索引。
        int nFrame, 
        //## 像点在图中横坐标
        int nX, 
        //## 像点在图中纵坐标
        int nY,
        //## 图形类型
        int nType) = 0;

    //## 设置图形动态加载平衡参数。
    virtual void SetImageStoreBalanceParam(
        //## 加载图形的数目的平衡值。
        int nNumImage, 
        //## 每多少次引用图形对象后作一次平衡检查。
        unsigned int uCheckPoint = 1000) = 0;

    //## 保存图形到文件。保存文件只对ISI_T_BITMAP16格式的图形有效。返回指表明图形是否成功保存到文件。
    virtual bool SaveImage(
        //## 保存文件的名字。
        const char* pszFile, 
        //## 图形名
        const char* pszImage, 
        //## 目标图形文件的类型
        int nFileType) = 0;


//---------绘制操作---------------

    //## 绘制图元
    virtual void DrawPrimitives(
        //## 绘制的图元的数目
        int nPrimitiveCount,
        //## 描述图元的结构的数组
        KRepresentUnit* pPrimitives, 
        //## Primitive类型，取值来自枚举值REPRESENT_UNIT_TYPE
        unsigned int uGenre, 
        //## 图元绘制操作提供的坐标是否为垂直与视线的单平面坐标。如果不是，则图元绘制操作提供的坐标是三维空间坐标。
        int bSinglePlaneCoord) = 0;

    //## 绘制图元到指定的图形之上
	virtual void DrawPrimitivesOnImage(
        //## 绘制的图元的数目
        int nPrimitiveCount,
        //## 描述图元的结构的数组
        KRepresentUnit* pPrimitives, 
        //## Primitive类型，取值来自枚举值REPRESENT_UNIT_TYPE
        unsigned int uGenre, 
        //## 图形名
        const char* pszImage, 
        //## 目标图形的id
        unsigned int uImage,
		short& nImagePosition)	= 0;

	//## 清除图形数据
	virtual void ClearImageData(
        //## 图形名
        const char* pszImage, 
        //## 目标图形的id
        unsigned int uImage,
		short nImagePosition)	= 0;

    //## 设置视觉观察的目的焦点。
    virtual void LookAt(int nX, int nY, int nZ) = 0;

    //## 复制绘图设备上的图到图形对象。
    //## 目标图形资源对象的类型目前只能是ISI_T_BITMAP16。如果目标图形对象不存在或者复制失败则返回false。
    virtual bool CopyDeviceImageToImage(
        //## 图形的名字。
        const char* pszName, 
        //## 绘图设备图形复制范围的左上角点横坐标
        int nDeviceX, 
        //## 绘图设备图形复制范围的左上角点纵坐标
        int nDeviceY, 
        //## 目的图形复制到范围的左上角点横坐标
        int nImageX, 
        //## 目的图形复制到范围的左上角点纵坐标
        int nImageY, 
        //## 复制范围的横宽
        int nWidth, 
        //## 复制范围的横宽
        int nHeight) = 0;

	//## 开始一轮绘制操作
	virtual bool RepresentBegin(
		//## 是否清除设备上当前的图形。
		int bClear, 
		//## 如果bClear为非0值，则Color指出用什么颜色值来清除设备原来的图形。
		unsigned int Color) = 0;

	//## 结束一轮绘制操作
	virtual void RepresentEnd() = 0;
	virtual void UpdataSceen() =0;

	//视图/绘图设备坐标 转化为空间坐标
	virtual void ViewPortCoordToSpaceCoord(
		int& nX,	//传入：视图/绘图设备坐标的x量，传出：空间坐标的x量
		int& nY,	//传入：视图/绘图设备坐标的y量，传出：空间坐标的y量
		int  nZ		//（期望）得到的空间坐标的z量
		) = 0;
	
	//## 设置光照信息
	virtual void SetLightInfo(
		//## 当前9个区域左上角的x坐标
		int nX,
		//## 当前9个区域左上角的y坐标
		int nY,
		//## 当前9个区域的光照信息，DWORD数组，表示各网格节点上光照亮度值
		//## 如果pLighting为NULL，表示不做光照处理
		unsigned int *pLighting) = 0;
	
	//## 设置表现模块选项
	virtual void SetOption(
		//## 选项类型
		RepresentOption eOption,
		//## 选项是否打开
		int bOn) = 0;

	//## 挂接嵌入文字的图片显示 [wxb 2003-6-20]
	virtual long AdviseRepresent(IInlinePicEngineSink*) = 0;
	virtual long UnAdviseRepresent(IInlinePicEngineSink*) = 0;
	
	virtual bool SaveScreenToFile(const char* pszName, ScreenFileType eType, unsigned int nQuality) = 0;
	
	// nGamma取值在0－100之间
	virtual void SetGamma(int nGamma) = 0;

	//设置偏色列表
	virtual unsigned int SetAdjustColorList(
			unsigned int* puColorList,	//偏色的颜色列表，每个颜色为一个unsigned int数，
										//用32bit数以0RGB的格式表示颜色，每个分量8bit。
			unsigned int uCount			//颜色列表中颜色数的数目
			) = 0;

	//## 预装载图形资源
	virtual bool PreLoadImage(
		const char* pszImageFile,					// 图形资源字符串
		unsigned int nType,							// 图形资源类型ISI_T_BITMAP16, ISI_T_SPR
		MemoryResidentParam* pResidentParam) = 0;	// 图形资源驻留内存的参数
	
    //## 指定ID释放某个图资源。
    virtual void FreeImageByID(
        //## 图形文件名/图形名。
        unsigned int nImageID 
        ) = 0;

	//## 得到图形化聊天室的接口实例
	virtual iGraphChatRoom* GetChatRoom(const char* pszIniFile) = 0;

	// <Add Name = "Ray[LuoLiang]" Date = "2006-9-15">
	 //##ModelId=3DCFED410049
    //##Documentation
    //## 空间坐标到二维(显示面)平面坐标的变换
    virtual void CoordinateTransform(
        //##Documentation
        //## 传入三维坐标x分量，传出二维坐标x(水平)分量
        int& nX, 
        //##Documentation
        //## 传入三维坐标y分量，传出二维坐标y(垂直)分量
        int& nY, 
        //##Documentation
        //## 传入三维坐标z分量
        int nZ) = 0;
	// </Add>
	
	//--> Rocker 2004/08/10 返回是represent
	virtual bool IsRepresent3() = 0;
	//<-- End
	 virtual void BltBackBufferToSurface(void* pSurface,int x,int y,int width,int height) = 0;

#ifdef _New3DClient
	virtual void SetCameraScale(float fScale) = 0;
	virtual void RenderTerrian() = 0;
	virtual void TerrianMove(float fMove, float fTurn, float fRoll) = 0;
	virtual void SetTerrianOption(TerrianOption& Option) = 0;
	virtual void GetTerrianOption(TerrianOption& Option) = 0;
#endif

//#ifdef _GameVideo
//	// add by chenshanglin on 2005-10-17 for game video
//	virtual int SetDrawFlag(int nDrawFlag) = 0;
//	// add end
//#endif
};



#ifndef _REPRESENT_INTERNAL_SIGNATURE_
	//----接口函数的导出----
	//=========创建一个iRepresentShell接口的实例===============
	extern "C" __declspec(dllimport) iRepresentShell* CreateRepresentShell();
	//=========测试模块性能，判断是否推荐使用===============
	//可选存在的接口函数
	//extern "C" __declspec(dllimport) bool RepresentIsModuleRecommended();
#endif

//------模块的接口函数的原型的定义------
typedef iRepresentShell*					(*fnCreateRepresentShell)();
typedef bool								(*fnRepresentIsModuleRecommended)();
//------接口函数的函数名字符串----------
#define	FN_CREATE_REPRESENT_SHELL			"CreateRepresentShell"
#define	FN_REPRESENT_IS_MODULE_RECOMMENDED	"RepresentIsModuleRecommended"


#endif /* IREPRESENTSHELL_H_HEADER_INCLUDED_C2311ED3 */
