#ifndef CHAOS_DB_CONCURRENCY_H
#define CHAOS_DB_CONCURRENCY_H

#include "ExceptionNew.h"

namespace Chaos
{

//
// For auto release mutex.
//
template<typename T>
class LockT
{   
public:
	LockT(const T& mutex)
		: _mutex(mutex)
	{
		_mutex.Lock();
		_acquired = true;
	}


protected: // For TryLockT
	LockT(const T& mutex, void*)
		: _mutex(mutex)
	{
		_acquired = _mutex.TryLock();
	}

public:
	~LockT()
	{
		if (_acquired)
		{
			_mutex.UnLock();
		}
	}

	// Test whether acquired.
	bool Acquired() const
	{
		return _acquired;
	}

	void Acquire() const
	{
		if (_acquired)
		{
			throw ThreadLockException(__FILE__, __LINE__);
		}

		_mutex.Lock();
		_acquired = true;
	}

	void Release() const
	{
		if (!_acquired)
		{
			throw ThreadLockException(__FILE__, __LINE__);
		}

		_mutex.UnLock();
		_acquired = false;
	}

	bool TryAcquire() const
	{
		if (_acquired)
		{
			throw ThreadLockException(__FILE__, __LINE__);
		}

		_acquired = _mutex.TryLock();

		return _acquired;
	}

private:
	// Un accessable.
	LockT(const LockT&);
	const LockT& operator=(const LockT&);

	const	T&		_mutex;  // Ref to constant.
	mutable bool	_acquired; // Already locked.
};

template<typename T>
class TryLockT : public LockT<T>
{
public:
	TryLockT(const T& mutex)
	: LockT<T>(mutex, (void*)0)
	{
	}
};

//
// Mutex.
//
class Mutex
{
public:
	typedef LockT<Mutex> LockIt;
	typedef TryLockT<Mutex> TryLockIt;

	Mutex();
	~Mutex();

	void Lock() const;
	void TryLock() const;
	void UnLock() const;
	bool WillUnLock() const;

private:					
	mutable CRITICAL_SECTION _mutex;

    Mutex(const Mutex&);
	const Mutex& operator=(const Mutex&);
};

inline
Mutex::Mutex()
{
	InitializeCriticalSection(&_mutex);
}

inline
Mutex::~Mutex()
{
	DeleteCriticalSection(&_mutex);
}

inline void
Mutex::Lock() const
{
	EnterCriticalSection(&_mutex);
}

inline void
Mutex::UnLock() const
{
	LeaveCriticalSection(&_mutex);
}

inline void
Mutex::TryLock() const
{ 
	TryEnterCriticalSection(& _mutex);
}

inline bool
Mutex::WillUnLock() const
{
	return true;
}

//
// RecMutex.
//
class RecMutex
{
public:
	typedef LockT<RecMutex>		LockIt;
	typedef	TryLockT<RecMutex>	TryLockIt;

	RecMutex();
	~RecMutex();
    
	void Lock() const;
	bool TryLock() const;
	void UnLock() const;

	//
	// Returns true if the mutex will unlock when calling unlock()
	// (false otherwise). For non-recursive mutex, this will always
	// return true. 
	// This function is used by the Monitor implementation to know whether 
	// the Mutex has been locked for the first time, or unlocked for the 
	// last time (that is another thread is able to acquire the mutex).
	// Pre-condition: the mutex must be locked.
	//
	bool WillUnLock() const; 

private:
	mutable int _count;
    mutable CRITICAL_SECTION _mutex;

	RecMutex(const RecMutex&);
	const RecMutex operator= (const RecMutex&);
};

inline
RecMutex::RecMutex()
: _count(0)
{
	InitializeCriticalSection(&_mutex);
}

inline
RecMutex::~RecMutex()
{
	DeleteCriticalSection(&_mutex);
}

inline void
RecMutex::Lock() const
{
	EnterCriticalSection(&_mutex); // Windows support recusive enter critical section.
	
	if (++_count > 1)
	{  // Already lock by myself.
		LeaveCriticalSection(&_mutex);
	}    
}

inline bool
RecMutex::TryLock() const
{
	if (TryEnterCriticalSection(&_mutex) == 0)
	{
		return false;
	}

	if (++_count > 1)
	{
		LeaveCriticalSection(&_mutex);
	}           
	return true;
}

inline void
RecMutex::UnLock() const
{
    if (--_count == 0)
	{
		LeaveCriticalSection(&_mutex);
    }
}

inline bool
RecMutex::WillUnLock() const
{
	return _count == 1;
}

//
// POD type for use Mutex as global static object.
//

// Use below initializer init StaticMutex.
#define STATIC_MUTEX_INITIALIZER { false }

class StaticMutex
{
public:
	typedef LockT<StaticMutex>		LockIt;
	typedef TryLockT<StaticMutex>	TryLockIt;
	
	// Must not call direct, use LockIt and TryLockIt instead.
	void Lock() const;
	bool TryLock() const;
	void UnLock() const;

	
	void __Initialize() const;

	mutable bool				_mutexInitialized;
	mutable CRITICAL_SECTION*	_mutex;	
};

inline void
StaticMutex::Lock() const
{
    if (!_mutexInitialized)
    {
		__Initialize();
    }
    
	EnterCriticalSection(_mutex);
//    assert(_mutex->RecursionCount == 1);
}

inline bool
StaticMutex::TryLock() const
{
    if (!_mutexInitialized)
    {
		__Initialize();
    }
    if(!TryEnterCriticalSection(_mutex))
    {
		return false;
    }
    if(_mutex->RecursionCount > 1)
    {
		LeaveCriticalSection(_mutex);
		return false;
    }
    return true;
}

inline void
StaticMutex::UnLock() const
{
//    assert(_mutexInitialized);
//    assert(_mutex->RecursionCount == 1);
    LeaveCriticalSection(_mutex);
}

//
// Semaphore.
//
class Semaphore
{
public:
	Semaphore(long initial = 0);
	~Semaphore();

    void Wait() const;
	bool TimeWait(time_t millisecond) const;

	void Post(int count = 1) const;

private:
  	HANDLE _sem;    
};

inline
Semaphore::Semaphore(long initial)
{
	_sem = CreateSemaphore(0, initial, 0x7fffffff, 0);

	if (_sem == 0)
	{
		throw ThreadSyscallException(__FILE__, __LINE__, GetLastError());
	}
}

inline
Semaphore::~Semaphore()
{
	CloseHandle(_sem);
	_sem = 0;
}

inline void
Semaphore::Wait() const
{
	int rc = WaitForSingleObject(_sem, INFINITE);

	if (rc != WAIT_OBJECT_0)
	{
		throw ThreadSyscallException(__FILE__, __LINE__, GetLastError());	
	}
}

inline bool
Semaphore::TimeWait(time_t millisecond) const
{
	int rc = WaitForSingleObject(_sem, static_cast<DWORD>(millisecond));

	if (rc != WAIT_TIMEOUT && rc != WAIT_OBJECT_0)
	{
		throw ThreadSyscallException(__FILE__, __LINE__, GetLastError());
	}

	return rc != WAIT_TIMEOUT;
}

inline void
Semaphore::Post(int count) const
{
	int rc = ReleaseSemaphore(_sem, count, 0);

	if(rc == 0)
	{
		throw ThreadSyscallException(__FILE__, __LINE__, GetLastError());
	}
}

} // Chaos namespace.

#endif // CHAOS_DB_CONCURRENCY_H