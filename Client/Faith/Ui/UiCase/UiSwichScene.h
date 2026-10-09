//xiehong 2007-12-17

#ifndef UI_SWITCH_SCENE
#define UI_SWITCH_SCENE

#include "TLStatic.h"

#define UI_SWITCH_SCENE_WINDOW_PATH "uisettings/layouts/SwitchScene.ls"
#define UI_SWITCH_SCENE_WINDOW_NAME "TaharezLook/SwitchScene"

class KUiSwitchSceneAnimation
{
	TLStaticImage* _thisWindow;

	TLStaticImage*	_animation;
private:
	void			loadUi();
	void			playAnimation();
	void			stopAnimation();

	bool			onFadingInEnd(const EventArgs& arg);
	bool			onHide(const EventArgs& arg);
	bool			onClick(const EventArgs& arg);
public:
	void			show();
	void			hide();
	bool			isVisible();

	void			preAnimation();
	void			postAnimation();

	KUiSwitchSceneAnimation();
	~KUiSwitchSceneAnimation();

	static KUiSwitchSceneAnimation& getSingleton();
};

#endif