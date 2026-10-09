// class Exception cpp file.
#include "stdafx.h"
#include "ExceptionNew.h"
#include <sstream>

namespace Chaos
{

std::ostream& 
operator<<(std::ostream &out, const Exception &e)
{
	e.print(out);
	return out;
}

//-----------------------------------------------------------------------------------
Exception::Exception() 
: _file(0),
  _line(0)
{
}

Exception::Exception(const char* file, int line)
: _file(file),
  _line(line)
{
}

Exception::~Exception()
{
}

Exception* 
Exception::clone() const
{
	return new Exception(*this);  // Use default copy semantic.
}

const char* 
Exception::file() const
{
	return _file;
}

int 
Exception::line() const
{
	return _line;
}

std::string Exception::_name = "Exception";

const std::string & 
Exception::name() const
{
	return _name;
}

void 
Exception::print(std::ostream &out) const
{
    if (_file && _line > 0)
	{
		out << _file << ":" << _line << ":";
	}

	out << name();
}

//-----------------------------------------------------------------------------------
NullException::NullException(const char*file, int line)
: Exception(file, line)
{
}

NullException::~NullException()
{
}

Exception* 
NullException::clone() const
{
	return new NullException(*this);
}

std::string 
NullException::_name = "Null Exception";

const std::string&
NullException::name() const
{
	return _name;
}

//-----------------------------------------------------------------------------------
DatabaseException::DatabaseException(const char* file, int line)
: Exception(file, line)
{
}

DatabaseException::~DatabaseException()
{
}

Exception* 
DatabaseException::clone() const
{
	return new DatabaseException(*this);
}

std::string 
DatabaseException::_name = "DataBase Exception";

const std::string& 
DatabaseException::name() const
{
    return _name;
}

void  
DatabaseException::print(std::ostream &out) const
{
	Exception::print(out);
	out << ":\n" << message;
}


//-----------------------------------------------------------------------------------
DeadLockException::DeadLockException(const char* file, int line)
: DatabaseException(file, line)
{
}

DeadLockException::~DeadLockException()
{
}

Exception*
DeadLockException::clone() const
{
	return new DeadLockException(*this);
}

std::string 
DeadLockException::_name = "DeadLockException";

const std::string& 
DeadLockException::name() const
{
	return _name;
}

void
DeadLockException::print(std::ostream &out) const
{
	Exception::print(out);
	out << ":\n" << message;
}

//-----------------------------------------------------------------------------------
NoSuchElementException::NoSuchElementException(const char* file, int line) 
: DatabaseException(file, line)
{
}

NoSuchElementException::~NoSuchElementException()
{
}

std::string 
NoSuchElementException::_name = "NoSuchElementException";

const std::string&
NoSuchElementException::name() const
{
	return _name;
}

Exception*
NoSuchElementException::clone() const
{
	return new NoSuchElementException(*this);
}

//-----------------------------------------------------------------------------------
ThreadSyscallException::ThreadSyscallException(const char* file, int line, int error)
: Exception(file, line),
  _error(error)
{
}

ThreadSyscallException::~ThreadSyscallException()
{
}

std::string 
ThreadSyscallException::_name = "ThreadSyscallException";

const std::string&
ThreadSyscallException::name() const
{
	return _name;
}

ThreadSyscallException::error() const
{
	return _error;
}

Exception*
ThreadSyscallException::clone() const
{
	return new ThreadSyscallException(*this);
}

void
ThreadSyscallException::print(std::ostream& out) const
{
	Exception::print(out);

	if (_error != 0)
	{
		out << ":\nthread syscall exception: ";
		LPVOID lpMsgBuf = 0;
		DWORD ok = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			_error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR)&lpMsgBuf,
			0,
			NULL);

		if(ok)
		{
			LPCTSTR msg = (LPCTSTR)lpMsgBuf;
//			assert(msg && strlen((char*)msg) > 0);
			out << msg;
			LocalFree(lpMsgBuf);
		}
		else
		{
			out << "unknown thread error";
		}
	}
}

//-----------------------------------------------------------------------------------
ThreadStartedException::ThreadStartedException(const char* file, int line)
: Exception(file, line)
{
}

ThreadStartedException::~ThreadStartedException()
{
}

std::string 
ThreadStartedException::_name = "ThreadStartedException";

const std::string&
ThreadStartedException::name() const
{
	return _name;
}

Exception*
ThreadStartedException::clone() const
{
	return new ThreadStartedException(*this);
}

//-----------------------------------------------------------------------------------
ThreadNotStartedException::ThreadNotStartedException(const char* file, int line)
: Exception(file, line)
{
}

ThreadNotStartedException::~ThreadNotStartedException()
{
}

std::string 
ThreadNotStartedException::_name = "ThreadNotStartedException";

const std::string&
ThreadNotStartedException::name() const
{
	return _name;
}

Exception*
ThreadNotStartedException::clone() const
{
	return new ThreadNotStartedException(*this);
}    

//-----------------------------------------------------------------------------------
ThreadLockException::ThreadLockException(const char* file, int line)
: Exception(file, line)
{
}

ThreadLockException::~ThreadLockException()
{
}

std::string 
ThreadLockException::_name = "ThreadLockException";

const std::string&
ThreadLockException::name() const
{
	return _name;
}

Exception*
ThreadLockException::clone() const
{
	return new ThreadLockException(*this);
}    

//-----------------------------------------------------------------------------------
UserException::UserException(const char* file, int line)
: Exception(file, line)
{
}

UserException::~UserException()
{
}

Exception* 
UserException::clone() const
{
	return new UserException(*this);
}

std::string 
UserException::_name = "User Exception";

const std::string& 
UserException::name() const
{
	return _name;
}

void  
UserException::print(std::ostream &out) const
{
	Exception::print(out);
	out << ":\n" << message;
}  

} // Chaos namespace.