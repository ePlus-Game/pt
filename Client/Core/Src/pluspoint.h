//  [7/1/2008 zhangjianyu]

#ifndef _plus_point_h_
#define _plus_point_h_

#include "cfs_fs2_savedef.h"
#include <string>
#define PLUS_POINT_SETTINGS "/settings/pluspoint.txt"

struct _PLUS_POINT_PARAM 
{
	std::string name;
	DWORD		maxpluspoint;
};

class PlusPointTable
{
public:
	PlusPointTable( void ) {};
	~PlusPointTable( void ) {};
public:
	static PlusPointTable& Singleton( void );

	bool IsOkPlusPointIdx( 
			int pluspointIdx );
	
	bool Load( 
		const std::string& filepath );

	bool GetPlusPointName( 
		int pluspointIdx,
		std::string& name );	

	bool GetPlusPointMax( 
		int pluspointIdx,
		DWORD& maxpluspoint );	

	bool GetPlusPointParam( 
		int pluspointIdx,
		std::string& name,
		DWORD& maxpluspoint );
private:
	_PLUS_POINT_PARAM d_pluspointTemplate[MAX_PLUS_POINT_COUNT];
};

#endif

