/************************************************************************
	filename: 	CEGUIEventSet.cpp
	created:	21/2/2004
	author:		Paul D Turner
	
	purpose:	Implements the EventSet class
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://www.cegui.org.uk)
    Copyright (C)2004 - 2005 Paul D Turner (paul@cegui.org.uk)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/
#include "CEGUIEventSet.h"
#include "CEGUIExceptions.h"
#include "CEGUIGlobalEventSet.h"
#include "CEGUIScriptModule.h"
#include <fstream.h>
#include <time.h>

#include <map>

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constructor
*************************************************************************/
EventSet::EventSet() :
	d_muted(false)
{
}


/*************************************************************************
	Destructor
*************************************************************************/
EventSet::~EventSet(void)
{
	removeAllEvents();
}


/*************************************************************************
	Add a new event to the EventSet
*************************************************************************/
void EventSet::addEvent(const int id)
{
	if (isEventPresent(id))
	{
		throw AlreadyExistsException("An event already exists in the EventSet.");
	}

	d_events[id] = new Event(id);
}


/*************************************************************************
	Remove an event from the EventSet
*************************************************************************/
void EventSet::removeEvent(const int id)
{
	EventMap::iterator pos = d_events.find(id);

	if (pos != d_events.end())
	{
		delete pos->second;
		d_events.erase(pos);
	}

}


/*************************************************************************
	Remove all events from the EventSet
*************************************************************************/
void EventSet::removeAllEvents(void)
{
	EventMap::iterator pos = d_events.begin();
	EventMap::iterator end = d_events.end()	;

	for (; pos != end; ++pos)
	{
		delete pos->second;
	}

	d_events.clear();
}


/*************************************************************************
	Check to see if an event is available
*************************************************************************/
bool EventSet::isEventPresent(const int id)
{
	return (d_events.find(id) != d_events.end());
}


/*************************************************************************
	Subscribe to an event (no group)
*************************************************************************/
Event::Connection EventSet::subscribeEvent(const int id, Event::Subscriber subscriber)
{
	EventMap::iterator pos = d_events.find(id);

	if (pos == d_events.end())
	{
		throw UnknownObjectException("No event is defined for this EventSet");	
	}

	return pos->second->subscribe(subscriber);
}


/*************************************************************************
	Subscribe to an event group
*************************************************************************/
Event::Connection EventSet::subscribeEvent(const int id, Event::Group group, Event::Subscriber subscriber)
{
	EventMap::iterator pos = d_events.find(id);

	if (pos == d_events.end())
	{
		throw UnknownObjectException("No event is defined for this EventSet");	
	}

	return pos->second->subscribe(group, subscriber);
}

/*************************************************************************
	Fire / Trigger an event
*************************************************************************/
void EventSet::fireEvent(int id, EventArgs& args)
{
    // handle global events
    GlobalEventSet::getSingleton().fireEvent(id, args);

    EventMap::iterator pos = d_events.find(id);

	if (pos == d_events.end())
	{
		throw UnknownObjectException("No event is defined for this EventSet");	
	}

	// fire the event
	if (!d_muted)
	{
		(*pos->second)(args);
	}

// 	static bool printEvent;
// 	
// 	static std::map<int, int> typeCounter;
// 	fstream fs;
// 	fs.open("D:\\event.txt", ios::binary | ios::trunc );
// 
// 	
// 	//看看是否有此类型的窗体，没有就加入，有就累加计数器
// 	if(typeCounter.find(id) == typeCounter.end())
// 	{
// 		typeCounter[id] = 1;
// 	}
// 	else
// 	{
// 		typeCounter[id]++;
// 	}
// 	const time_t curTime = time(NULL);
// 	tm* dt = localtime(&curTime);
// 			
// 	fs<<id<<'\t'<<typeCounter[id]<<'\t'<<dt->tm_min<<dt->tm_sec<<endl;
// 	
// 
// 	fs.close();
}


/*************************************************************************
	Return whether the EventSet is muted or not.	
*************************************************************************/
bool EventSet::isMuted(void) const
{
	return d_muted;
}


/*************************************************************************
	Set the mute state for this EventSet.	
*************************************************************************/
void EventSet::setMutedState(bool setting)
{
	d_muted = setting;
}


/*************************************************************************
	Return a EventSet::EventIterator object to iterate over the available
	events.
*************************************************************************/
EventSet::EventIterator EventSet::getIterator(void) const
{
	return EventIterator(d_events.begin(), d_events.end());
}


} // End of  CEGUI namespace section
