#ifndef CHAT_RESOURCE_H
#define CHAT_RESOURCE_H
#include <vector>
using std::vector;
#define BITMAP_NAME_LENGTH 64
#define BITMAP_COLOR_BPP_32  1
#define BITMAP_COLOR_BPP_DYNAMIC 2
struct resourceBitmap
{
	HBITMAP hBitmap;
	char    bitmapName[BITMAP_NAME_LENGTH+1];
	int attr;
	resourceBitmap()
	{
		hBitmap = 0;
		memset(&bitmapName,0,sizeof(bitmapName));
	}
	~resourceBitmap()
	{
		if(hBitmap)
			DeleteObject(hBitmap);
	}
	void   Reset();///÷ÿ–¬º”‘ÿÕº∆¨£ª
};
#define ALL_BUTTON_STATE_NORMAL_ID             1
#define ALL_BUTTON_STATE_HOVER_ID              2
#define ALL_BUTTON_STATE_DOWN_ID               3
#define ALL_BUTTON_STATE_DISABLE_ID            4
#define TIP_BUTTON_STATE_NORMAL_ID             5
#define TIP_BUTTON_STATE_HOVE_ID               6
#define TIP_BUTTON_STATE_DIABLE_ID             7


///////////////////////
#define CHAT_INI_FILE_PATH "ui/imagesets/image/"
class ChatResource
{
public:
	ChatResource();
	~ChatResource();
	bool LoadSrc();
	void Reset();
	resourceBitmap* GetResource(int id);
	static ChatResource& GetSingle() ;
private:

	vector<resourceBitmap*> resourceList;
};
#endif