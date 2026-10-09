/////////////////////////////////////////////////////////////////////////////
//  FileName    :   LayoutParser.cpp
//  Creator     :   zuolizhi
//  Date        :   2006-12-13 9:54:00
//  Comment     :   Interface Declare
//	Changes		:	
/////////////////////////////////////////////////////////////////////////////

#include "LayoutParser.h"
#include <boost\regex.hpp>

char* szRegExpFilterList[] =
{
	"(<.*?>.*?</.*?>)",				//split
	"(<.*?>)(.*)(</.*>$)",			//head body tail
	"(\\s.*?\\s*=\\s*[^\\s>]*)",	//property
	"<(.*?)\\s+.*>.*</.*>",			//split header
	"(<%s.*?>.*?</%s>)",
};

enum
{
	parse_step_one,
	parse_step_two,
	parse_step_thr,
	parse_step_header,
	parse_step_cont,
	parse_step_end
};

/////////////////////////////////////////////////////////////////////////////
//
//              Class Define
//
/////////////////////////////////////////////////////////////////////////////

LayoutParser::LayoutParser( )
{

}

LayoutParser::~LayoutParser( )
{

}

void LayoutParser::ParseText( 
	char* szText,
	LayoutData& LD )
{
	boost::cmatch	result;
	boost::regex	expression;
	STRINGLIST		ElementList;
	STRINGLIST		ObjList;
	std::string		content = szText;
	char			szRegExp[MAX_REGEXPLEN];
	
	expression = szRegExpFilterList[parse_step_two];

	if( boost::regex_split( std::back_inserter(ElementList), content, expression ) )
	{
		STRINGLIST::iterator it = ElementList.begin( );
		
		expression = szRegExpFilterList[parse_step_thr];

		if( boost::regex_split( std::back_inserter(ObjList), (*it), expression ) )
		{
			STRINGLIST::iterator sit = ObjList.begin( );

			while ( sit != ObjList.end( ) ) 
			{
				LD.setHeadProperty( (char*)(*sit).c_str( ) );
				++sit;
			}
		}

		ObjList.clear( );

		++it;

		expression = szRegExpFilterList[parse_step_header];
		content = (*it);
		
		if( boost::regex_split( std::back_inserter(ObjList), content, expression ) )
		{
			const char* szHeader = (*ObjList.begin()).c_str();
			sprintf( szRegExp, szRegExpFilterList[parse_step_cont], szHeader, szHeader );
			ObjList.clear( );

			expression = szRegExp;
			
			content = (*it);
			if( boost::regex_split( std::back_inserter(ObjList), content, expression ) )
			{
				STRINGLIST::iterator sit = ObjList.begin( );
				
				while ( sit != ObjList.end( ) ) 
				{
					LayoutSeg LS;
					ParseSeg( (char*)(*sit).c_str(), LS );
					LD.addASeg( LS );
					++sit;
				}
			}
		}

		ElementList.clear( );
		ObjList.clear( );
	}
}

void LayoutParser::ParseSeg(
	char* szText,
	LayoutSeg& LS )
{
	boost::cmatch	result;
	boost::regex	expression;
	STRINGLIST		ElementList;
	STRINGLIST		ObjList;
	std::string		content = szText;

	
	expression = szRegExpFilterList[parse_step_two];
	
	if( boost::regex_split( std::back_inserter(ElementList), content, expression ) )
	{
		STRINGLIST::iterator it = ElementList.begin( );
		
		expression = szRegExpFilterList[parse_step_thr];
		
		if( boost::regex_split( std::back_inserter(ObjList), (*it), expression ) )
		{
			STRINGLIST::iterator sit = ObjList.begin( );
			
			while ( sit != ObjList.end( ) ) 
			{
				LS.setProperty( (char*)(*sit).c_str( ) );
				++sit;
			}
		}
		
		ObjList.clear( );
		
		++it;
		
		expression = szRegExpFilterList[parse_step_one];
		if( boost::regex_split( std::back_inserter(ObjList), (*it), expression ) )
		{
			STRINGLIST::iterator sit = ObjList.begin( );
			
			while ( sit != ObjList.end( ) ) 
			{
				ParseObj( (char*)(*sit).c_str(), LS );
				++sit;
			}
		}
		
		ElementList.clear( );
		ObjList.clear( );
	}
}


void LayoutParser::ParseObj( 
	char* szText,
	LayoutSeg& LS )
{
	boost::cmatch	result;
	boost::regex	expression;
	STRINGLIST		ElementList;
	STRINGLIST		PropertyList;
	std::string		content = szText;

	
	
	expression = szRegExpFilterList[parse_step_two];
	
	if( boost::regex_split( std::back_inserter(ElementList), content, expression ) )
	{
		STRINGLIST::iterator it = ElementList.begin( );
		
		LayoutElement	element;
		expression = szRegExpFilterList[parse_step_thr];
		
		if( boost::regex_split( std::back_inserter(PropertyList), (*it), expression ) )
		{
			STRINGLIST::iterator sit = PropertyList.begin( );
			
			while ( sit != PropertyList.end( ) ) 
			{
				element.setProperty( (char*)(*sit).c_str( ) );
				++sit;
			}
		}

		++it;

		element.setContent( (char*)(*it).c_str() );

		ElementList.clear( );
		PropertyList.clear( );
		LS.addElem( element );
	}
}

LayoutParser& LayoutParser::Singleton( )
{
	static LayoutParser LP;
	
	return LP;
}
