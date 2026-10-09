///////////////////////////////////////////////////////////////
//	File        : Timer.cpp
//	Author      : chenshanglin
//	Create Time : 2006-6-1
//	Project     : Common
//	Platform    : 
//	Remark      :
//	History     : 
///////////////////////////////////////////////////////////////
#include "KCore.h"
#include "Timer.h"

int Timer::m_MilliSecPerFrame;
const int Timer::MILLISECS_PER_TIMESLICE = 100;

void Timer::Start()
{
	static bool bHasStarted = false;

	if( !bHasStarted )
	{
		m_MilliSecPerFrame = 1000 / GAME_FPS;
		m_TimerContainer.resize(100);
		m_Frame = 1;

		bHasStarted = true;
	}
}

int Timer::RegisterCallBack(TIMER_CALLBACK pCallBack, void *pArg, int nHunderdMs, bool bPeriod /* = false */)
{
	TimerContainer::size_type pos;
	TimerContainer::size_type size = m_TimerContainer.size();

	for(pos = 0; pos < size; ++pos)
	{
		if(NULL == m_TimerContainer[pos].pCallBack)
		{
			m_TimerContainer[pos].registerFrame = m_Frame;
			m_TimerContainer[pos].timeoutFrame = nHunderdMs * MILLISECS_PER_TIMESLICE / m_MilliSecPerFrame;
			m_TimerContainer[pos].pCallBack = pCallBack;
			m_TimerContainer[pos].bPeriod = bPeriod;
			m_TimerContainer[pos].arguments = pArg;
			return pos + 1;
		}
	}

	CallBackTimer t;
	t.pCallBack = pCallBack;
	t.registerFrame = m_Frame;
	t.timeoutFrame = nHunderdMs * MILLISECS_PER_TIMESLICE / m_MilliSecPerFrame;
	t.bPeriod = bPeriod;
	t.arguments = pArg;
	m_TimerContainer.push_back(t);

	return pos;
}

void Timer::CancelCallBack(int nId)
{
	if( nId >= 0 && nId < (int)m_TimerContainer.size() )
	{
		if(m_TimerContainer[nId - 1].pCallBack)
		{
			m_TimerContainer[nId - 1].pCallBack = NULL;
		}
	}
}

void Timer::SetTime(int nId, int nHunderdMs)
{
	if( nId >= 0 && nId < (int)m_TimerContainer.size() )
	{
		if(m_TimerContainer[nId - 1].pCallBack)
		{
			m_TimerContainer[nId - 1].registerFrame = m_Frame;
			m_TimerContainer[nId - 1].timeoutFrame = nHunderdMs * MILLISECS_PER_TIMESLICE / m_MilliSecPerFrame;
		}
	}
}

void Timer::ProcessTimers()
{
	TimerContainer::size_type pos;
	TimerContainer::size_type size = m_TimerContainer.size();

	++m_Frame;

	for(pos = 0; pos < size; pos++)
	{
		if(m_TimerContainer[pos].pCallBack)
		{
			if(m_Frame >= m_TimerContainer[pos].registerFrame + m_TimerContainer[pos].timeoutFrame)
			{
				m_TimerContainer[pos].pCallBack(m_TimerContainer[pos].arguments);

				if(m_TimerContainer[pos].bPeriod)
				{
					m_TimerContainer[pos].registerFrame = m_Frame;
				}
				else
				{
					m_TimerContainer[pos].pCallBack = NULL;
				}
			}
		}
	}
}