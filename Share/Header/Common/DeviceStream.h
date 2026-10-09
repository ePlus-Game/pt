#ifndef CHAOS_DEVICE_STREAM_H
#define CHAOS_DEVICE_STREAM_H

// Extern streambuf for GUI application use.
#include <streambuf>
#include <algorithm>
#include "Conc.h"

using Chaos::RecMutex;

namespace std
{

//
// Device interface callback by stream.
// Return >0 means success, else return <=0 means failed.
//
typedef int (*nchar_to_device)(void* device, const char* c, int n);

//
// N char buffered device stream.
//
template <class _E, class _Tr = char_traits<_E>, size_t bufSize = 256 >
class nchar_outbuf : public basic_streambuf<_E, _Tr>
{
public:
	explicit nchar_outbuf(void* device = 0,  nchar_to_device toDevice = 0);	
	virtual ~nchar_outbuf();	
	void register_device(void* device, nchar_to_device toDevice);	

protected:
	virtual int		   sync();
	virtual int_type   overflow(int_type c = traits_type::eof());
	virtual streamsize xsputn(const char_type* s, streamsize n);
private:	
	int	buffer_out();

	void*			  _device;
	nchar_to_device   _toDevice;

	// Protect my buffer.
	RecMutex		  _mutex;
	char_type*		  _buf;		

	// No copy.
	nchar_outbuf(const nchar_outbuf&);
	const nchar_outbuf& operator=(const nchar_outbuf&);
};

template <class _E, class _Tr, size_t bufSize> inline
nchar_outbuf<_E, _Tr, bufSize>::nchar_outbuf(void* device /*= 0*/,  nchar_to_device toDevice /*= 0*/)
: _device(device),
  _toDevice(toDevice)  
{
	// Allocate buffer.
	_buf = new char[bufSize];
	setp(_buf, _buf + bufSize);
}

template <class _E, class _Tr, size_t bufSize> inline
nchar_outbuf<_E, _Tr, bufSize>::~nchar_outbuf()
{
	sync();

	// Make sure NOT use device any longer.
	_device = 0;
	_toDevice = 0;

	// Delete buffer.
	delete _buf;
	_buf = 0;
}

template <class _E, class _Tr, size_t bufSize> inline
int
nchar_outbuf<_E, _Tr, bufSize>::sync()
{
	RecMutex::LockIt lock(_mutex);
	return buffer_out();
}


template <class _E, class _Tr, size_t bufSize> inline
int
nchar_outbuf<_E, _Tr, bufSize>::buffer_out()
{
	int count = pptr() - pbase();
	int retval = 0;
	if (_device == 0 || _toDevice == 0 || (retval = _toDevice(_device, _buf, count)) <=0)
	{
		return -1;
	}

	pbump(-count);
	
	return retval;
}


template <class _E, class _Tr, size_t bufSize> inline
void
nchar_outbuf<_E, _Tr, bufSize>::register_device(void* device, nchar_to_device toDevice)
{
	_device = device;
	_toDevice = toDevice;
}

template<class _E, class _Tr, size_t bufSize> inline
typename nchar_outbuf<_E, _Tr, bufSize>::int_type   
nchar_outbuf<_E, _Tr, bufSize>::overflow(int_type c /* = traits_type::eof()*/)
{
	RecMutex::LockIt lock(_mutex);
	if (buffer_out() < 0)
	{
		return traits_type::eof();
	}
	else
	{
		if(!traits_type::eq_int_type(c, traits_type::eof()))
		{
			return sputc(c);
		}
		else
		{	// c eq to eof(), No operation, just not_eof() means success.
			return traits_type::not_eof(c);
		}		
	}
}

template<class _E, class _Tr, size_t bufSize> inline
streamsize  
nchar_outbuf<_E, _Tr, bufSize>::xsputn(const char_type* s, streamsize n)
{
	RecMutex::LockIt lock(_mutex);
	streamsize max, putted;

	for( putted = 0; 0 < n; )
	{
		if( pptr() != 0 && 0 < (max = epptr() - pptr()) ) 
		{
			if( n < max )
				max = n;
			traits_type::copy( pptr(), s, max );
			
			s += max, putted += max, n -= max, pbump(max);
			
			if(traits_type::find(s, max, traits_type::to_char_type('\n')) != 0)
			{
				sync();
			}				
		}
		else if (traits_type::eq_int_type(traits_type::eof(), overflow(traits_type::to_int_type(*s))))
			break;
		else
			++s, ++putted, --n;
	}
	
	return putted; 
}
} // std namespace.

#endif // CHAOS_DEVICE_STREAM_H
