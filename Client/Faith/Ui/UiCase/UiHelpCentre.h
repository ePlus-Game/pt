

#ifndef KUISTATE_H 
#define KUISTATE_H

#include "../UiCommon.h"
#include "CEGUI.h"
#include "TLGameObject.h"
class KUiHelpCentre : public KUiWndSingleton<KUiHelpCentre>
{
public:
	enum UiState
	{
	};
public:
	KUiHelpCentre		 ( );
	virtual ~KUiHelpCentre();

public:
	static	void	Show( void );
	static	void	SetDragObject( CEGUI::TLGameObject::GameObject& rGO );
	static	void	GetDragObject( const CEGUI::TLGameObject::GameObject& rGO );

	inline UiState	GetState( void )
	{
		return m_State;
	}
	inline void	SetState( UiState eState )
	{
		m_State = eState;
	}

private:
	CEGUI::TLGameObject* m_pClientHand;
	UiState				 m_State;	
};

#endif 