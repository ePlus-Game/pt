///////////////////////////////////////////////////////////////
//	File        : Timer.h
//	Author      : chenshanglin
//	Create Time : 2006-6-1
//	Project     : Common
//	Platform    : win32 & Linux 
//	Remark      :
//	History     : 
///////////////////////////////////////////////////////////////
#ifndef _Timer_h
#define _Timer_h

#include <vector>

using std::vector;

class Timer
{
private:
	typedef		void (*TIMER_CALLBACK)(void*);

	typedef		struct _CallBackTimer
	{
		_CallBackTimer() : pCallBack(NULL)
		{
		}

		bool		 bPeriod;
		unsigned int registerFrame;
		unsigned int timeoutFrame;
		TIMER_CALLBACK	pCallBack;
		void		 *arguments;

	} CallBackTimer, *PCallBackTimer;

	typedef		vector<CallBackTimer>	TimerContainer;

public:
	void	Start();

	int		RegisterCallBack(TIMER_CALLBACK pCallBack, void *pArg, int nHunderdMs, bool bPeriod = false);
	void	CancelCallBack(int nId);
	void	SetTime(int nId, int nHunderdMs);
	void	ProcessTimers();

private:
	const static int	MILLISECS_PER_TIMESLICE;
	static int	m_MilliSecPerFrame;
	
	unsigned int	m_Frame;
	TimerContainer	m_TimerContainer;
};

#endif