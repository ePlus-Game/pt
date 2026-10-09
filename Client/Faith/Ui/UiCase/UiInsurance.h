//xiehong 2008-4-24 ±£œ’ΩÁ√Ê

#ifndef UI_INSURANCE_H
#define UI_INSURANCE_H

#define UI_INSURANCE_WINDOW_PATH	  "uisettings/layouts/In.ls"
#define UI_INSURANCE_WINDOW_PATH_1024 "uisettings/layouts1024/GMCommunication.ls"

#include "CEGUI.h"
#include "TLStatic.h"

class KUiInsurance
{
	Window*	_thisWindow;
	bool	_loaded;

private:
	bool	loadUi();
	
protected:

public:
	KUiInsurance();
	~KUiInsurance();
	static KUiInsurance& getSingleton();

	void	show();
	void	hide();
	void	toggle();
	bool	isVisible();
};

#endif