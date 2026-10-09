//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-9-6 20:35
//      File_base        : MagicDef
//      File_ext         : h
//      Author           : chenshanglin
//      Description      : 
//
//      <Change_list>
//      {
//      Change_datetime  : 
//      Change_by        : 
//      Change_purpose   : 
//      }
//////////////////////////////////////////////////////////////////////
#ifndef _MagicDef_h
#define _MagicDef_h

#define	INVALID_ATTRIB -1

typedef struct _tagMagicData
{
	int		nMagicNo;
	int		nVal;
	bool	bBroadCast;

	_tagMagicData()
	{
		bBroadCast = false;
	}

} MagicData, *PMagicData;

#endif