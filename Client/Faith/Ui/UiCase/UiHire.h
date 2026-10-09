// UiHire.h: interface for the KUiHire class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIHIRE_H__1856945F_2229_4320_AA43_1F9623526F98__INCLUDED_)
#define AFX_UIHIRE_H__1856945F_2229_4320_AA43_1F9623526F98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../uicommon.h"
#include <map>
#include "EmplomentDataDef.h"
#include "TLRadioButton.h"

using namespace std;


class KUiHire : public KUiWndSingleton<KUiHire>
{
public:
	enum HireOpMsg
	{
		HOM_NO_RECORD = 0,						//无记录
		HOM_HIRE_FAILED,						//雇佣失败
		HOM_OPERATE_TOO_FAST,					//操作过快
		HOM_ALREADY_HAS_EMPLOYEE,				//已经雇佣了一个佣兵
		HOM_EMPLOYEE_OUT_OF_EMPLOY_TIME,		//佣兵没有雇用时间了
		HOM_EMPLOYER_OUT_OF_EMPLOY_TIME,		//雇主没有雇用时间了
		HOM_EMPLOYER_OUT_OF_MONEY,				//雇主没有金钱了
		HOM_NO_EMPLOYEE,						//没有佣兵
		HOM_EMPLOYEE_ON_LINE,					//佣兵上线
		HOM_EMPLOYER_NOT_ENOUGH_EMPLOY_TIME,	//雇主没有足够的雇用时间进行一次雇用
		HOM_EMPLOYER_NOT_ENOUGH_MONEY,			//雇主没有足够的金钱进行一次雇用
		HOM_EMPLOYEE_FIGHT_MODE_LEVEL_REQUIRED,	//战斗雇用等级要求不满足
		HOM_SUCCESS,							//雇佣成功
	};
	KUiHire( const CEGUI::String& id_name );
	virtual ~KUiHire();

	static void	Show();
	static void	Hide();
	static void Breathe();
	void		Init();
private:
	bool	btnSelectMetier_MouseClick( const CEGUI::EventArgs& args );
	bool	menuSelectMetier_MouseClick( const CEGUI::EventArgs& args );
	bool	btnPageBar_MouseClick( const CEGUI::EventArgs& args );

	//功能按钮
	bool	btnSearch_MouseClick( const CEGUI::EventArgs& args );
	bool	btnHire_MouseClick( const CEGUI::EventArgs& args );

	bool	btnExpHire_MouseClick( const CEGUI::EventArgs& args) ;
	bool	btnFighterHire_MouseClick( const CEGUI::EventArgs& args) ;

	bool	btnCloseBtn_MouseClick( const CEGUI::EventArgs& args) ;

	void	InitBars( StaticImage& page, const CEGUI::String& layoutFileName, vector<Window *>& barList );
	void	RefreshBars( vector<Window *>& barList, const Window& activeBar/*const CEGUI::String& activeBarName*/);

	void	setExpContent(int index, BYTE sex, char* name, Metier metier, int level, char* shizu, char* zhuhou);
	void	setFighterContent(int index, char* name, Metier profession, int level, DualityNumber attack,
								DualityNumber magic, int hujia, int blood, int j, int y, int t);
	void	clearExpContent();
	void	clearFighterContent();

	bool	btnPrev_MouseClick( const CEGUI::EventArgs& args );
	bool	btnNext_MouseClick( const CEGUI::EventArgs& args );
	void	sendDataReq();

	void	search(int pageIndex, HireType type);
public:
	void	updateFighter(vector<FighterHirer>& fitherData, int recordStart);
	void	updateExp(vector<ExpHirer>& fitherData, int recordStart);
	void	clear();
private:
	const int MaxBarCount;
	const CEGUI::String BarFilename_Exp;
	const CEGUI::String BarFilename_Fighter;

	map<CEGUI::RadioButton*, int>	m_KindMap;
	vector<Window *>	m_vBarList;
	vector<Window *>	m_vFighterBarList;

	StaticImage*	m_pMenuSelectMetier;
	RadioButton*	m_pBtnAll;
	RadioButton*	m_pBtnXuanfeng;
	RadioButton*	m_pBtnXingtian;
	RadioButton*	m_pBtnZhenren;
	RadioButton*	m_pBtnTianshi;
	RadioButton*	m_pBtnShoushi;
	RadioButton*	m_pBtnYishi;
	StaticText*		m_pTxtMetier;

	StaticImage*	m_pPage_Exp;		//经验佣兵页签
	StaticImage*	m_pPage_Fighter;	//战斗佣兵页签
	Window*			m_pPage_ExpBackImg;		//经验佣兵背景
	Window*			m_pPage_FighterBackImg;	//战斗佣兵背景
	
	Window*			m_pEdit_Shizu;
	Window*			m_pEdit_Zhuhou;
	Window*			m_pTxt_Shizu;
	Window*			m_pTxt_Zhuhou;

	int				m_curExpPage;
	int				m_curFightPage;
	bool			m_expPageHaveNext;
	bool			m_fightPageHaveNext;

	int				m_curExpPageItemCount;
	int				m_curFightPageItemCount;

	String			m_name;
	int				m_selTime;
	int				m_selMoney;
	MetierSelection m_selMetier;
};

/********************************************************************
/*						class: KUiHireConfig
*********************************************************************/

class KUiHireConfigExp : public KUiWndSingleton<KUiHireConfigExp>
{
	ExpHireType m_curSelType;
	TLRadioButton* m_expHireTypeBtn[EHT_TYPE_COUNT];
public:
	KUiHireConfigExp( const CEGUI::String& id_name );
	virtual ~KUiHireConfigExp();
	
	void		Init();
	static void	Show();
private:
	bool	btnOK_MouseClick( const CEGUI::EventArgs& args );
	bool	btnCancel_MouseClick( const CEGUI::EventArgs& args );
	bool	btnHireType_MouseClick( const CEGUI::EventArgs& args );
	bool	window_hiden( const CEGUI::EventArgs& args );

	void		showEmpployTime();
};

/********************************************************************
/*						class: KUiHireSalary
*********************************************************************/
class KUiHireConfigSalary : public KUiWndSingleton<KUiHireConfigSalary>
{
public:
	KUiHireConfigSalary( const CEGUI::String& id_name );
	virtual ~KUiHireConfigSalary();

	void		Init();
private:
	bool	btnOK_MouseClick( const CEGUI::EventArgs& args );
	bool	btnCancel_MouseClick( const CEGUI::EventArgs& args );
	bool	input_TextChanged( const CEGUI::EventArgs& args );
	bool	window_hiden( const CEGUI::EventArgs& args );
};

#endif // !defined(AFX_UIHIRE_H__1856945F_2229_4320_AA43_1F9623526F98__INCLUDED_)
