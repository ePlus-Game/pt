/////////////////////////////////////////////////////////////////////////////
//  FileName    :   LayoutParser.h
//  Creator     :   zuolizhi
//  Date        :   2006-12-13 9:54:00
//  Comment     :   Interface Declare
//	Changes		:	
/////////////////////////////////////////////////////////////////////////////

#ifndef _LAYOUTPARSER_H_
#define _LAYOUTPARSER_H_

#include "layoutdef.h"
#include "layoutelement.h"
#include <list>
/////////////////////////////////////////////////////////////////////////////
//
//              Struct Declare
//
/////////////////////////////////////////////////////////////////////////////

#define MAX_REGEXPLEN	256
typedef std::list<std::string> STRINGLIST;

/////////////////////////////////////////////////////////////////////////////
//
//              Class Declare
//
/////////////////////////////////////////////////////////////////////////////

class LayoutParser
{
public:

	void ParseText( 
		char* szText,
		LayoutData& LD );

	void ParseSeg(
		char* szText,
		LayoutSeg& LS );

	void ParseObj( 
		char* szText,
		LayoutSeg& LS );
	static LayoutParser& Singleton( );
public:

	LayoutParser( );
	~LayoutParser( );

protected:


private:

};

#endif