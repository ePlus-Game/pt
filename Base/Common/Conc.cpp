#include "stdafx.h"
#include "Conc.h"

static CRITICAL_SECTION criticalSection;

typedef std::list<CRITICAL_SECTION*> MutexList;

static MutexList* mutexList;

//
// C++ compiler do NOT sure which global variable init first.
// So static mutex use lazy initialization.
//
namespace Chaos
{
	class Init
	{
	public:
		Init()
		{
			InitializeCriticalSection(&criticalSection);
			mutexList = new MutexList;
		}
		
		~Init()
		{
			for(MutexList::iterator p = mutexList->begin(); 
			p != mutexList->end(); ++p)
			{
				DeleteCriticalSection(*p);
				delete *p;
			}

			delete mutexList;
			DeleteCriticalSection(&criticalSection);
		}

		
	};

	static Init init;

	void 
	StaticMutex::__Initialize() const
	{
		EnterCriticalSection(&criticalSection);
		
		if(!_mutexInitialized)
		{
			_mutex = new CRITICAL_SECTION;
			InitializeCriticalSection(_mutex);
			mutexList->push_back(_mutex);
			
			_mutexInitialized = true;
		}
		
		LeaveCriticalSection(&criticalSection);
	}

} // Chaos namespace.