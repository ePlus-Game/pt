#ifndef CHAOS_REPORTER_H
#define CHAOS_REPORTER_H

#include <iostream>
#include <fstream>
#include <queue>
#include "Conc.h"
#include "DeviceStream.h"

#define UM_ADDSTRING WM_USER + 500

using namespace Chaos;

class Reporter
{
public:
	Reporter(HWND hWnd)
	: _hWnd(hWnd),
	  _outOld(0),
	  _errOld(0)
	{
		// Redirect std cout cerr to list box.
		_bufGui.register_device(this, Reporter::Report);
		_outOld = std::cout.rdbuf(&_bufGui);
		_errOld = std::cerr.rdbuf(&_bufGui);
	}

	~Reporter()
	{
		std::cout.rdbuf(_outOld);
		std::cerr.rdbuf(_errOld);
	}

	static int Report(void* me, const char* s, int n)
	{
		if (me != 0 && s != 0 && n > 0)
		{
			static_cast<Reporter*>(me)->Output(s,n);
		}	
		
		return 1;
	}	
	
	void Output(const char* s, int n)
	{
		std::string str(s, n);
		
		{
			RecMutex::LockIt lock(_lockQueue);
			_queue.push(str);
		}		

		if ( _hWnd && ::IsWindow(_hWnd) )
		{			
			::PostMessage(_hWnd, UM_ADDSTRING, 0, (LPARAM)this);
		}
	}

	bool GetOneReport(std::string& report)
	{
		RecMutex::LockIt lock(_lockQueue);

		if (_queue.empty())
		{
			return false;
		}
		else
		{
			report = _queue.front();
			_queue.pop();
			
			return true;
		}
	}


private:
	std::nchar_outbuf<char>		_bufGui; // For redirect std stream.
	std::basic_streambuf<char>* _errOld;
	std::basic_streambuf<char>* _outOld;

	RecMutex				_lockQueue;
	std::queue<std::string>	_queue;
	HWND	 _hWnd;
};

#endif // CHAOS_REPORTER_H