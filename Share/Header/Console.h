#ifndef DEBUG_CONSOLE_H
#define DEBUG_CONSOLE_H

#include <io.h>
#include <fcntl.h>
#include <stdio.h>

class Console
{
public:
	Console();
	~Console();
	
	void Top();

private:
	HWND  _window;	
	FILE* _last;
};

inline
Console::Console()
: _window(0)
{
   int crthandle;
   FILE *crtfile;

   AllocConsole();
   crthandle = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
   crtfile = _fdopen(crthandle, "w");
   _last = stdout;
   *stdout = *crtfile;
   setvbuf(stdout, NULL, _IONBF, 0); 

   HMODULE kernel = ::LoadLibrary("Kernel32.dll");
   _window = (HWND)(::GetProcAddress(kernel, "GetConsoleWindow")());
}

inline
Console::~Console()
{
	*stdout = *_last;
}

inline void 
Console::Top()
{
	if (_window != NULL)
	{
		::BringWindowToTop(_window);
	}	
}

#endif // DEBUG_CONSOLE_H