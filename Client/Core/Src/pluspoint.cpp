//  [7/1/2008 zhangjianyu]
#include "KCore.h"
#include "pluspoint.h"
#include "KTabFile.h"
#define DEFAULT_MAX 4000000000
#define DEFAULT_NAME ""

PlusPointTable& PlusPointTable::Singleton()
{
	static PlusPointTable pluspointtable;
	return pluspointtable;
}

bool PlusPointTable::IsOkPlusPointIdx( 
						int pluspointIdx )
{
	if  ( pluspointIdx >= 0 && pluspointIdx < MAX_PLUS_POINT_COUNT)
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool PlusPointTable::Load( 
						const std::string& filepath )
{
	KTabFile tabFile;
	if ( tabFile.Load( filepath.c_str() ) == TRUE )
	{
		int height = tabFile.GetHeight();
		height = height < MAX_PLUS_POINT_COUNT ? height : MAX_PLUS_POINT_COUNT;
		for ( int nIdx = 0; nIdx < height; ++nIdx )
		{
			char szName[COMMON_CLIENT_MSG_LEN_16];
			tabFile.GetString( nIdx + 2, "Name", DEFAULT_NAME, szName, sizeof(szName) );
			szName[COMMON_CLIENT_MSG_LEN_16 - 1] = 0;
			d_pluspointTemplate[nIdx].name = szName;

			tabFile.GetInteger( nIdx + 2, "MaxPlusPoint", DEFAULT_MAX, (int*)&d_pluspointTemplate[nIdx].maxpluspoint);
			if (d_pluspointTemplate[nIdx].maxpluspoint > DEFAULT_MAX)
			{
				d_pluspointTemplate[nIdx].maxpluspoint = DEFAULT_MAX;
			}
		}

		return true;
	}
	return false;
}

bool PlusPointTable::GetPlusPointName( 
					  int pluspointIdx,
					  std::string& name )
{
	if  (  IsOkPlusPointIdx( pluspointIdx ) )
	{
		name = d_pluspointTemplate[pluspointIdx].name;
		return true;
	}
	else
	{
		name = DEFAULT_NAME;
		return false;
	}
}

bool PlusPointTable::GetPlusPointMax( 
					 int pluspointIdx,
					 DWORD& maxpluspoint )
{
	if  (  IsOkPlusPointIdx( pluspointIdx ) )
	{
		maxpluspoint = d_pluspointTemplate[pluspointIdx].maxpluspoint;
		return true;
	}
	else
	{
		maxpluspoint = DEFAULT_MAX;
		return false;
	}
}

bool PlusPointTable::GetPlusPointParam( 
						int pluspointIdx,
						std::string& name,
						DWORD& maxpluspoint )
{
	if  (  IsOkPlusPointIdx( pluspointIdx ) )
	{
		 name = d_pluspointTemplate[pluspointIdx].name;
		 maxpluspoint = d_pluspointTemplate[pluspointIdx].maxpluspoint;
		return true;
	}
	else
	{
		name = DEFAULT_NAME;
		maxpluspoint = DEFAULT_MAX;
		return false;
	}
}