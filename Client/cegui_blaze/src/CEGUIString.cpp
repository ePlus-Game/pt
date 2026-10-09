/***********************************************************************
	filename: 	CEGUIString.cpp
	created:	26/2/2004
	author:		Paul D Turner
	
	purpose:	Implements string class
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2006 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#include <windows.h>
#include "CEGUIString.h"
#include <iostream>
#include <vector>

// Start of CEGUI namespace section
namespace CEGUI
{
#define ReserveSize 128
std::vector<wchar_t>	s_wchar_buf(ReserveSize);
std::vector<utf8>		s_utf8_buf(ReserveSize);
std::vector<char>		s_ansi_buf(ReserveSize);
// definition of 'no position' value
const String::size_type String::npos = (String::size_type)(-1);


//////////////////////////////////////////////////////////////////////////
// Destructor
//////////////////////////////////////////////////////////////////////////
String::~String(void)
{
	if (d_reserve > STR_QUICKBUFF_SIZE)
	{
		delete[] d_buffer;
	}
		if (d_encodedbufflen > 0)
	{
		delete[] d_encodedbuff;
	}
}

bool String::grow(size_type new_size)
{
    // check for too big
    if (max_size() <= new_size)
        std::length_error("Resulting CEGUI::String would be too big");

    // increase, as we always null-terminate the buffer.
    ++new_size;

    if (new_size > d_reserve)
    {
        utf32* temp = new utf32[new_size];

        if (d_reserve > STR_QUICKBUFF_SIZE)
        {
            memcpy(temp, d_buffer, (d_cplength + 1) * sizeof(utf32));
            delete[] d_buffer;
        }
        else
        {
            memcpy(temp, d_quickbuff, (d_cplength + 1) * sizeof(utf32));
        }

        d_buffer = temp;
        d_reserve = new_size;

        return true;
    }

    return false;
}

// perform re-allocation to remove wasted space.
void String::trim(void)
{
    size_type min_size = d_cplength + 1;

    // only re-allocate when not using quick-buffer, and when size can be trimmed
    if ((d_reserve > STR_QUICKBUFF_SIZE) && (d_reserve > min_size))
    {
            // see if we can trim to quick-buffer
        if (min_size <= STR_QUICKBUFF_SIZE)
        {
            memcpy(d_quickbuff, d_buffer, min_size * sizeof(utf32));
            delete[] d_buffer;
            d_reserve = STR_QUICKBUFF_SIZE;
        }
        // re-allocate buffer
        else
        {
            utf32* temp = new utf32[min_size];
            memcpy(temp, d_buffer, min_size * sizeof(utf32));
            delete[] d_buffer;
            d_buffer = temp;
            d_reserve = min_size;
        }

    }

}

// build an internal buffer with the string encoded as utf8 (remains valid until string is modified).
utf8* String::build_utf8_buff(void) const
{
    size_type buffsize = encoded_size(ptr(), d_cplength) + 1;

    if (buffsize > d_encodedbufflen) {

        if (d_encodedbufflen > 0)
        {
            delete[] d_encodedbuff;
        }

        d_encodedbuff = new utf8[buffsize];
        d_encodedbufflen = buffsize;
    }

    encode(ptr(), d_encodedbuff, buffsize, d_cplength);

    // always add a null at end
    d_encodedbuff[buffsize-1] = ((utf8)0);
    d_encodeddatalen = buffsize;

    return d_encodedbuff;
}



//////////////////////////////////////////////////////////////////////////
// Comparison operators
//////////////////////////////////////////////////////////////////////////
bool	operator==(const String& str1, const String& str2)
{
	return (str1.compare(str2) == 0);
}

bool	operator==(const String& str, const std::string& std_str)
{
	return (str.compare(std_str) == 0);
}

bool	operator==(const std::string& std_str, const String& str)
{
	return (str.compare(std_str) == 0);
}

bool	operator==(const String& str, const utf8* utf8_str)
{
	return (str.compare(utf8_str) == 0);
}

bool	operator==(const utf8* utf8_str, const String& str)
{
	return (str.compare(utf8_str) == 0);
}


bool	operator!=(const String& str1, const String& str2)
{
	return (str1.compare(str2) != 0);
}

bool	operator!=(const String& str, const std::string& std_str)
{
	return (str.compare(std_str) != 0);
}

bool	operator!=(const std::string& std_str, const String& str)
{
	return (str.compare(std_str) != 0);
}

bool	operator!=(const String& str, const utf8* utf8_str)
{
	return (str.compare(utf8_str) != 0);
}

bool	operator!=(const utf8* utf8_str, const String& str)
{
	return (str.compare(utf8_str) != 0);
}


bool	operator<(const String& str1, const String& str2)
{
	return (str1.compare(str2) < 0);
}

bool	operator<(const String& str, const std::string& std_str)
{
	return (str.compare(std_str) < 0);
}

bool	operator<(const std::string& std_str, const String& str)
{
	return (str.compare(std_str) >= 0);
}

bool	operator<(const String& str, const utf8* utf8_str)
{
	return (str.compare(utf8_str) < 0);
}

bool	operator<(const utf8* utf8_str, const String& str)
{
	return (str.compare(utf8_str) >= 0);
}


bool	operator>(const String& str1, const String& str2)
{
	return (str1.compare(str2) > 0);
}

bool	operator>(const String& str, const std::string& std_str)
{
	return (str.compare(std_str) > 0);
}

bool	operator>(const std::string& std_str, const String& str)
{
	return (str.compare(std_str) <= 0);
}

bool	operator>(const String& str, const utf8* utf8_str)
{
	return (str.compare(utf8_str) > 0);
}

bool	operator>(const utf8* utf8_str, const String& str)
{
	return (str.compare(utf8_str) <= 0);
}


bool	operator<=(const String& str1, const String& str2)
{
	return (str1.compare(str2) <= 0);
}

bool	operator<=(const String& str, const std::string& std_str)
{
	return (str.compare(std_str) <= 0);
}

bool	operator<=(const std::string& std_str, const String& str)
{
	return (str.compare(std_str) >= 0);
}

bool	operator<=(const String& str, const utf8* utf8_str)
{
	return (str.compare(utf8_str) <= 0);
}

bool	operator<=(const utf8* utf8_str, const String& str)
{
	return (str.compare(utf8_str) >= 0);
}


bool	operator>=(const String& str1, const String& str2)
{
	return (str1.compare(str2) >= 0);
}

bool	operator>=(const String& str, const std::string& std_str)
{
	return (str.compare(std_str) >= 0);
}

bool	operator>=(const std::string& std_str, const String& str)
{
	return (str.compare(std_str) <= 0);
}

bool	operator>=(const String& str, const utf8* utf8_str)
{
	return (str.compare(utf8_str) >= 0);
}

bool	operator>=(const utf8* utf8_str, const String& str)
{
	return (str.compare(utf8_str) <= 0);
}

//////////////////////////////////////////////////////////////////////////
// c-string operators
//////////////////////////////////////////////////////////////////////////
bool operator==(const String& str, const char* c_str)
{
	return (str.compare(c_str) == 0);
}

bool operator==(const char* c_str, const String& str)
{
	return (str.compare(c_str) == 0);
}

bool operator!=(const String& str, const char* c_str)
{
	return (str.compare(c_str) != 0);
}

bool operator!=(const char* c_str, const String& str)
{
	return (str.compare(c_str) != 0);
}

bool operator<(const String& str, const char* c_str)
{
	return (str.compare(c_str) < 0);
}

bool operator<(const char* c_str, const String& str)
{
	return (str.compare(c_str) >= 0);
}

bool operator>(const String& str, const char* c_str)
{
	return (str.compare(c_str) > 0);
}

bool operator>(const char* c_str, const String& str)
{
	return (str.compare(c_str) <= 0);
}

bool operator<=(const String& str, const char* c_str)
{
	return (str.compare(c_str) <= 0);
}

bool operator<=(const char* c_str, const String& str)
{
	return (str.compare(c_str) >= 0);
}

bool operator>=(const String& str, const char* c_str)
{
	return (str.compare(c_str) >= 0);
}

bool operator>=(const char* c_str, const String& str)
{
	return (str.compare(c_str) <= 0);
}

//////////////////////////////////////////////////////////////////////////
// Concatenation operator functions
//////////////////////////////////////////////////////////////////////////
String	operator+(const String& str1, const String& str2)
{
	String temp(str1);
	temp.append(str2);
	return temp;
}

String	operator+(const String& str, const std::string& std_str)
{
	String temp(str);
	temp.append(std_str);
	return temp;
}

String	operator+(const std::string& std_str, const String& str)
{
	String temp(std_str);
	temp.append(str);
	return temp;
}

String	operator+(const String& str, const utf8* utf8_str)
{
	String temp(str);
	temp.append(utf8_str);
	return temp;
}

String	operator+(const utf8* utf8_str, const String& str)
{
	String temp(utf8_str);
	temp.append(str);
	return temp;
}

String	operator+(const String& str, utf32 code_point)
{
	String temp(str);
	temp.append(1, code_point);
	return temp;
}

String	operator+(utf32 code_point, const String& str)
{
	String temp(1, code_point);
	temp.append(str);
	return temp;
}

String operator+(const String& str, const char* c_str)
{
	String tmp(str);
	tmp.append(c_str);
	return tmp;
}

String operator+(const char* c_str, const String& str)
{
	String tmp(c_str);
	tmp.append(str);
	return tmp;
}

//////////////////////////////////////////////////////////////////////////
// Output (stream) functions
//////////////////////////////////////////////////////////////////////////
OutStream& operator<<(OutStream& s, const String& str)
{
	return s << str.c_str();
}

//////////////////////////////////////////////////////////////////////////
// Modifying operations
//////////////////////////////////////////////////////////////////////////
// swap the contents of str1 and str2
void	swap(String& str1, String& str2)
{
	str1.swap(str2);
}

utf8* AnsiToUtf8(char const *szAnsi)
{
	// ANSI => WCHAR
	size_t lengthAnsi = strlen(szAnsi);
	size_t lengthUnicode = MultiByteToWideChar(CP_ACP, 0, szAnsi, lengthAnsi, NULL, 0);
	if (s_wchar_buf.size() < lengthUnicode + 1)
	{
		s_wchar_buf.resize(lengthUnicode * 2);
	}
	wchar_t *szUnicode = &s_wchar_buf[0];
	::MultiByteToWideChar(CP_ACP, 0, szAnsi, lengthAnsi, szUnicode, lengthUnicode);
	szUnicode[lengthUnicode] = 0;

	// WCHAR => UTF8
	size_t lengthUtf8 = ::WideCharToMultiByte(CP_UTF8, 0, szUnicode, lengthUnicode, NULL, 0, NULL, NULL);
	if (s_utf8_buf.size() < lengthUtf8 + 1)
	{
		s_utf8_buf.resize(lengthUtf8 * 2);
	}
	utf8 *szUtf8String = &s_utf8_buf[0];
	::WideCharToMultiByte(CP_UTF8, 0, szUnicode, lengthUnicode, (char *)szUtf8String, lengthUtf8, NULL, NULL); 
	szUtf8String[lengthUtf8] = 0;

	return (utf8 *)szUtf8String;
} 

char*  Utf8ToAnsi( String u8 )
{
	//UTF8 => WCHAR
	size_t lengthUnicode = ::MultiByteToWideChar(CP_UTF8, 0, u8.c_str(), u8.utf8_stream_len(), NULL, NULL);
	if (s_wchar_buf.size() < lengthUnicode + 1)
	{
		s_wchar_buf.resize(lengthUnicode * 2);
	}
	wchar_t *szUnicode = &s_wchar_buf[0];
	::MultiByteToWideChar(CP_UTF8, 0, u8.c_str(), u8.utf8_stream_len(),szUnicode, lengthUnicode); 
	szUnicode[lengthUnicode] = 0;

	//WCHAR => ANSI
	int lengthAnsi = ::WideCharToMultiByte(CP_ACP, 0, szUnicode, lengthUnicode, NULL, NULL, NULL, NULL );
	if (s_ansi_buf.size() < lengthAnsi + 1)
	{
		s_ansi_buf.resize( lengthAnsi * 2 ) ;
	}
	char* szAnsi = &s_ansi_buf[0];
	::WideCharToMultiByte(CP_ACP, 0, szUnicode, lengthUnicode, szAnsi, lengthAnsi, NULL, NULL );
	szAnsi[lengthAnsi] = 0;
	return szAnsi;
} 

char* _Utf8ToAnsi( char* u8, int lenthUtf8  )
{
	//UTF8 => WCHAR
	size_t lengthUnicode = ::MultiByteToWideChar(CP_UTF8, 0, u8, lenthUtf8, NULL, NULL);
	if (s_wchar_buf.size() < lengthUnicode + 1)
	{
		s_wchar_buf.resize(lengthUnicode * 2);
	}
	wchar_t *szUnicode = &s_wchar_buf[0];
	::MultiByteToWideChar(CP_UTF8, 0, u8, lenthUtf8,szUnicode, lengthUnicode); 
	szUnicode[lengthUnicode] = 0;

	//WCHAR => ANSI
	int lengthAnsi = ::WideCharToMultiByte(CP_ACP, 0, szUnicode, lengthUnicode, NULL, NULL, NULL, NULL );
	if (s_ansi_buf.size() < lengthAnsi + 1)
	{
		s_ansi_buf.resize( lengthAnsi * 2 ) ;
	}
	char* szAnsi = &s_ansi_buf[0];
	::WideCharToMultiByte(CP_ACP, 0, szUnicode, lengthUnicode, szAnsi, lengthAnsi, NULL, NULL );
	szAnsi[lengthAnsi] = 0;
	return szAnsi;
} 


} // End of  CEGUI namespace section
