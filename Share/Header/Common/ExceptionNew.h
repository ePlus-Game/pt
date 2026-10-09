#ifndef CHAOS_DB_EXCEPTION_H
#define CHAOS_DB_EXCEPTION_H

#include <iostream>
#include <list>

namespace Chaos
{

class Exception;
// Non-member functions also have virtual characteristic.
std::ostream& operator<<(std::ostream&, const Exception&);

class Exception
{
public:

	Exception();
	Exception(const char*, int);

	virtual ~Exception();
	
	// Get Exception name, We need polymophic.
	virtual const std::string& name() const;

	// Make Non-member << operator¡¡become virtually.
	virtual void  print(std::ostream&) const;

	// virtual constructor.
	virtual Exception* clone() const;

	const char* file() const;
	int			line() const;

private:
    // Exception position.
	const char* _file;
	int		    _line;

	// Exception name.
	static std::string _name;
};

//-----------------------------------------------------------------------------------
// Derefence a null pointer.
class NullException : public Exception
{
public:
	NullException(const char*, int);
	~NullException();

    virtual Exception*		    clone() const;
	virtual const std::string&	 name() const;

private:
	static std::string _name;    
};

//---------------------------------------------------------------------------------
// Berkeley Database Exceptions.

class DatabaseException : public Exception
{
public:
	DatabaseException(const char*, int);
	~DatabaseException();
    
    virtual Exception*			clone() const;
	virtual const std::string&  name() const;
	virtual void  print(std::ostream&) const;

	std::string message;
private:
	static std::string _name;
};

class DeadLockException : public DatabaseException
{
public:
	DeadLockException(const char*, int);
   ~DeadLockException();

   virtual Exception*		   clone() const;
   virtual const std::string&  name() const;
   virtual  void print(std::ostream&) const;

private:
	static std::string _name;
};

class NoSuchElementException : public DatabaseException
{
public:
	NoSuchElementException(const char*, int);
	~NoSuchElementException();

	virtual Exception*		    clone() const;
	virtual const std::string&	name() const;

private:
	static std::string _name;
};

//---------------------------------------------------------------------------------
// Thread Exceptions.
class ThreadSyscallException : public Exception
{
public:
	ThreadSyscallException(const char*, int, int);
	~ThreadSyscallException();

	virtual Exception*		    clone() const;
	virtual const std::string&	name() const;
	virtual int  error() const;
	virtual void print(std::ostream&) const;

private:
	const int _error;
	static std::string _name; 

	const ThreadSyscallException& operator=(const ThreadSyscallException&);
};

class ThreadNotStartedException : public Exception
{
public:
	ThreadNotStartedException(const char*, int);
	~ThreadNotStartedException();

	virtual Exception*		    clone() const;
	virtual const std::string&	name() const;
	
private:
	static std::string _name;
};

class ThreadStartedException : public Exception
{
public:
	ThreadStartedException(const char*, int);
	~ThreadStartedException();

	virtual Exception*		    clone() const;
	virtual const std::string&	name() const;

private:
	static std::string _name;
};

class ThreadLockException : public Exception
{
public:
	ThreadLockException(const char*, int);
	~ThreadLockException();

	virtual Exception*		    clone() const;
	virtual const std::string&	name() const;

private:
	static std::string _name;
};

class UserException : public Exception
{
public:
	UserException(const char*, int);
	~UserException();

	virtual Exception*			clone() const;
	virtual const std::string&  name() const;
	virtual void  print(std::ostream&) const;

	std::string message;
private:
	static std::string _name;
};


} // Chaos namespace.

#endif // CHAOS_DB_EXCEPTION_H