//³èÎïÍ·Ïñ xiehong-2007-11-27

#ifndef UI_PET_FRAME
#define UI_PET_FRAME

#define UI_PET_FRAME_INVALID_ID -1
#include "CEGUI.h"
using namespace CEGUI;
class KUiSelfPetFrame
{
	int		_frameIndex;

	int		_npcIndex;

	void	connectToSelfFrame();
	void	loadStyle();
	bool	clickFrame(const EventArgs& args);
public:
	KUiSelfPetFrame();
	~KUiSelfPetFrame();
	
	static KUiSelfPetFrame& getSingleton();
	
	void	show();
	void	show(char* name, int petNpcIndex);
	void	hide();

	int		getNpcIndex();

	void	update(int curBlood, int maxBlood);
};

#endif