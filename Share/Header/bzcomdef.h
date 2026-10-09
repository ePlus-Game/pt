// bzcomdef.h: Defines for blaze game studio
// Version: 1.0
// by Cooler 2003-11-07 liuyujun@263.net
// Crossing platform (Win32/Linux)
//////////////////////////////////////////////////////////////////////

#ifndef _BZCOMDEF_H_
#define _BZCOMDEF_H_

#include <stdio.h>
#ifdef WIN32
#include "windows.h"
#else /* LINUX */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bzdatatypes.h"
#endif

// Macro define region

	// Limits define
#ifdef WIN32
#define MAX_PATHNAME			_MAX_PATH
#else /* LINUX */
#define MAX_PATHNAME			512
#endif

#define MAX_DATATIMESTR			64
#define MAX_FILEOPENFLAGS		256
#define MAX_DWORDSTR			20
#define MAX_SCANCONDITION		256

	// Symbols define
#define SPACEMARK				0x20
#define BZENTERMARK				"\n"
#define SIZE_BZENTERMARK		1
#define EQUALMARK				"="
#define SIZE_EQUALMARK			1
#define LEFTSQUARE				"["
#define SIZE_LEFTSQUARE			1
#define RIGHTSQUARE				"]"
#define SIZE_RIGHTSQUARE		1

#define WIN32SLIPMARK			"\\"
#define SIZE_WIN32SLIPMARK		1
#define LINUXSLIPMARK			"/"
#define SIZE_LINUXSLIPMARK		1
#ifdef WIN32
#define DIRSLIPMARK				WIN32SLIPMARK
#define SIZE_DIRSLIPMARK		1
#else /* LINUX */
#define DIRSLIPMARK				LINUXSLIPMARK
#define SIZE_DIRSLIPMARK		1
#endif

	// Macros define
#ifndef	max
#define	max(a,b)				(((a) > (b)) ? (a) : (b))
#endif

#ifndef	min
#define	min(a,b)				(((a) < (b)) ? (a) : (b))
#endif

#define TRIMPOINTER_LRSPACE(begin, end) \
		while(begin <= end && begin[0] == SPACEMARK) \
		{ begin++; } \
		while(end >= begin && end[0] == SPACEMARK) \
		{ end--; }

#define TRIMPOINTER_LSPACE(pos, end) \
		while(pos <= end && pos[0] == SPACEMARK) \
		{ pos++; }

#define TRIMPOINTER_RSPACE(pos, begin) \
		while(pos >= begin && pos[0] == SPACEMARK) \
		{ pos--; }

#define TRIMPOINTER_LSPACEENTER(pos, end) \
		while(pos <= end && (pos[0] == SPACEMARK || pos[0] == BZENTERMARK[0])) \
		{ pos++; }

#define TRIMPOINTER_RSPACEENTER(pos, begin) \
		while(pos >= begin && (pos[0] == SPACEMARK || pos[0] == BZENTERMARK[0])) \
		{ pos--; }

#define MOVPOINTER_NEXTLINEHEAD(pos, end) \
		{ LPSTR pMOVPOINTER_NEXTLINEHEAD_c001er = strstr(pos, BZENTERMARK); \
		if(pMOVPOINTER_NEXTLINEHEAD_c001er == NULL) pos = end + 1; \
		else pos = pMOVPOINTER_NEXTLINEHEAD_c001er + SIZE_BZENTERMARK; }

#define SIZEBYPOINTER(begin, end) \
		(end >= begin ? end - begin + 1 : -1)

#endif // _BZCOMDEF_H_
