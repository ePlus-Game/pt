/*
** $Id: LuaLib.h,v 1.1 2007/07/11 03:04:53 zuolizhi Exp $
** Lua - An Extensible Extension Language
** Tecgraf: Computer Graphics Technology Group, PUC-Rio, Brazil
** http://www.lua.org	mailto:info@lua.org
** See Copyright Notice at the end of this file
*/


#ifndef lua_h
#define lua_h

#include <stdarg.h>
#include <stddef.h>

#ifdef __linux
	#define LUA_API
	#define	LUALIB_API
#else
	#ifndef LUA_API
	#define LUA_API		extern
	#define	LUALIB_API	extern
	#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif 

#define LUA_VERSION	"Lua 5.0.3"
#define LUA_COPYRIGHT	"Copyright (C) 1994-2006 Tecgraf, PUC-Rio"
#define LUA_AUTHORS 	"R. Ierusalimschy, L. H. de Figueiredo & W. Celes"



/* option for multiple returns in `lua_pcall' and `lua_call' */
#define LUA_MULTRET	(-1)


/*
** pseudo-indices
*/
#define LUA_REGISTRYINDEX	(-10000)
#define LUA_GLOBALSINDEX	(-10001)
#define lua_upvalueindex(i)	(LUA_GLOBALSINDEX-(i))


/* error codes for `lua_load' and `lua_pcall' */
#define LUA_ERRRUN	1
#define LUA_ERRFILE	2
#define LUA_ERRSYNTAX	3
#define LUA_ERRMEM	4
#define LUA_ERRERR	5


typedef struct lua_State lua_State;

typedef int (*lua_CFunction) (lua_State *L);


/*
** functions that read/write blocks when loading/dumping Lua chunks
*/
typedef const char * (*lua_Chunkreader) (lua_State *L, void *ud, size_t *sz);

typedef int (*lua_Chunkwriter) (lua_State *L, const void* p,
                                size_t sz, void* ud);


/*
** basic types
*/
#define LUA_TNONE	(-1)

#define LUA_TNIL	0
#define LUA_TBOOLEAN	1
#define LUA_TLIGHTUSERDATA	2
#define LUA_TNUMBER	3
#define LUA_TSTRING	4
#define LUA_TTABLE	5
#define LUA_TFUNCTION	6
#define LUA_TUSERDATA	7
#define LUA_TTHREAD	8


/* minimum Lua stack available to a C function */
#define LUA_MINSTACK	20


/*
** generic extra include file
*/
#ifdef LUA_USER_H
#include LUA_USER_H
#endif


/* type of numbers in Lua */
#ifndef LUA_NUMBER
typedef double lua_Number;
#else
typedef LUA_NUMBER lua_Number;
#endif


/*
** state manipulation
*/
LUA_API lua_State *lua_open (void);
LUA_API void       lua_close (lua_State *L);
LUA_API lua_State *lua_newthread (lua_State *L);

LUA_API lua_CFunction lua_atpanic (lua_State *L, lua_CFunction panicf);


/*
** basic stack manipulation
*/
LUA_API int   lua_gettop (lua_State *L);
LUA_API void  lua_settop (lua_State *L, int idx);
LUA_API void  lua_pushvalue (lua_State *L, int idx);
LUA_API void  lua_remove (lua_State *L, int idx);
LUA_API void  lua_insert (lua_State *L, int idx);
LUA_API void  lua_replace (lua_State *L, int idx);
LUA_API int   lua_checkstack (lua_State *L, int sz);

LUA_API void  lua_xmove (lua_State *from, lua_State *to, int n);


/*
** access functions (stack -> C)
*/

LUA_API int             lua_isnumber (lua_State *L, int idx);
LUA_API int             lua_isstring (lua_State *L, int idx);
LUA_API int             lua_iscfunction (lua_State *L, int idx);
LUA_API int             lua_isuserdata (lua_State *L, int idx);
LUA_API int             lua_type (lua_State *L, int idx);
LUA_API const char     *lua_typename (lua_State *L, int tp);

LUA_API int            lua_equal (lua_State *L, int idx1, int idx2);
LUA_API int            lua_rawequal (lua_State *L, int idx1, int idx2);
LUA_API int            lua_lessthan (lua_State *L, int idx1, int idx2);

LUA_API lua_Number      lua_tonumber (lua_State *L, int idx);
LUA_API int             lua_toboolean (lua_State *L, int idx);
LUA_API const char     *lua_tostring (lua_State *L, int idx);
LUA_API size_t          lua_strlen (lua_State *L, int idx);
LUA_API lua_CFunction   lua_tocfunction (lua_State *L, int idx);
LUA_API void	       *lua_touserdata (lua_State *L, int idx);
LUA_API lua_State      *lua_tothread (lua_State *L, int idx);
LUA_API const void     *lua_topointer (lua_State *L, int idx);


/*
** push functions (C -> stack)
*/
LUA_API void  lua_pushnil (lua_State *L);
LUA_API void  lua_pushnumber (lua_State *L, lua_Number n);
LUA_API void  lua_pushlstring (lua_State *L, const char *s, size_t l);
LUA_API void  lua_pushstring (lua_State *L, const char *s);
LUA_API const char *lua_pushvfstring (lua_State *L, const char *fmt,
                                                    va_list argp);
LUA_API const char *lua_pushfstring (lua_State *L, const char *fmt, ...);
LUA_API void  lua_pushcclosure (lua_State *L, lua_CFunction fn, int n);
LUA_API void  lua_pushboolean (lua_State *L, int b);
LUA_API void  lua_pushlightuserdata (lua_State *L, void *p);


/*
** get functions (Lua -> stack)
*/
LUA_API void  lua_gettable (lua_State *L, int idx);
LUA_API void  lua_rawget (lua_State *L, int idx);
LUA_API void  lua_rawgeti (lua_State *L, int idx, int n);
LUA_API void  lua_newtable (lua_State *L);
LUA_API void *lua_newuserdata (lua_State *L, size_t sz);
LUA_API int   lua_getmetatable (lua_State *L, int objindex);
LUA_API void  lua_getfenv (lua_State *L, int idx);


/*
** set functions (stack -> Lua)
*/
LUA_API void  lua_settable (lua_State *L, int idx);
LUA_API void  lua_rawset (lua_State *L, int idx);
LUA_API void  lua_rawseti (lua_State *L, int idx, int n);
LUA_API int   lua_setmetatable (lua_State *L, int objindex);
LUA_API int   lua_setfenv (lua_State *L, int idx);


/*
** `load' and `call' functions (load and run Lua code)
*/
LUA_API void  lua_call (lua_State *L, int nargs, int nresults);
LUA_API int   lua_pcall (lua_State *L, int nargs, int nresults, int errfunc);
LUA_API int lua_cpcall (lua_State *L, lua_CFunction func, void *ud);
LUA_API int   lua_load (lua_State *L, lua_Chunkreader reader, void *dt,
                        const char *chunkname);

LUA_API int lua_dump (lua_State *L, lua_Chunkwriter writer, void *data);


/*
** coroutine functions
*/
LUA_API int  lua_yield (lua_State *L, int nresults);
LUA_API int  lua_resume (lua_State *L, int narg);

/*
** garbage-collection functions
*/
LUA_API int   lua_getgcthreshold (lua_State *L);
LUA_API int   lua_getgccount (lua_State *L);
LUA_API void  lua_setgcthreshold (lua_State *L, int newthreshold);

/*
** miscellaneous functions
*/

LUA_API const char *lua_version (void);

LUA_API int   lua_error (lua_State *L);

LUA_API int   lua_next (lua_State *L, int idx);

LUA_API void  lua_concat (lua_State *L, int n);



/* 
** ===============================================================
** some useful macros
** ===============================================================
*/

#define lua_boxpointer(L,u) \
	(*(void **)(lua_newuserdata(L, sizeof(void *))) = (u))

#define lua_unboxpointer(L,i)	(*(void **)(lua_touserdata(L, i)))

#define lua_pop(L,n)		lua_settop(L, -(n)-1)

#define lua_register(L,n,f) \
	(lua_pushstring(L, n), \
	 lua_pushcfunction(L, f), \
	 lua_settable(L, LUA_GLOBALSINDEX))

#define lua_pushcfunction(L,f)	lua_pushcclosure(L, f, 0)

#define lua_isfunction(L,n)	(lua_type(L,n) == LUA_TFUNCTION)
#define lua_istable(L,n)	(lua_type(L,n) == LUA_TTABLE)
#define lua_islightuserdata(L,n)	(lua_type(L,n) == LUA_TLIGHTUSERDATA)
#define lua_isnil(L,n)		(lua_type(L,n) == LUA_TNIL)
#define lua_isboolean(L,n)	(lua_type(L,n) == LUA_TBOOLEAN)
#define lua_isnone(L,n)		(lua_type(L,n) == LUA_TNONE)
#define lua_isnoneornil(L, n)	(lua_type(L,n) <= 0)

#define lua_pushliteral(L, s)	\
	lua_pushlstring(L, "" s, (sizeof(s)/sizeof(char))-1)



/*
** compatibility macros and functions
*/


LUA_API int lua_pushupvalues (lua_State *L);

#define lua_getregistry(L)	lua_pushvalue(L, LUA_REGISTRYINDEX)
#define lua_setglobal(L,s)	\
   (lua_pushstring(L, s), lua_insert(L, -2), lua_settable(L, LUA_GLOBALSINDEX))

#define lua_getglobal(L,s)	\
		(lua_pushstring(L, s), lua_gettable(L, LUA_GLOBALSINDEX))


/* compatibility with ref system */

/* pre-defined references */
#define LUA_NOREF	(-2)
#define LUA_REFNIL	(-1)

#define lua_ref(L,lock)	((lock) ? luaL_ref(L, LUA_REGISTRYINDEX) : \
      (lua_pushstring(L, "unlocked references are obsolete"), lua_error(L), 0))

#define lua_unref(L,ref)	luaL_unref(L, LUA_REGISTRYINDEX, (ref))

#define lua_getref(L,ref)	lua_rawgeti(L, LUA_REGISTRYINDEX, ref)



/*
** {======================================================================
** useful definitions for Lua kernel and libraries
** =======================================================================
*/

/* formats for Lua numbers */
#ifndef LUA_NUMBER_SCAN
#define LUA_NUMBER_SCAN		"%lf"
#endif

#ifndef LUA_NUMBER_FMT
#define LUA_NUMBER_FMT		"%.14g"
#endif

/* }====================================================================== */


/*
** {======================================================================
** Debug API
** =======================================================================
*/


/*
** Event codes
*/
#define LUA_HOOKCALL	0
#define LUA_HOOKRET	1
#define LUA_HOOKLINE	2
#define LUA_HOOKCOUNT	3
#define LUA_HOOKTAILRET 4


/*
** Event masks
*/
#define LUA_MASKCALL	(1 << LUA_HOOKCALL)
#define LUA_MASKRET	(1 << LUA_HOOKRET)
#define LUA_MASKLINE	(1 << LUA_HOOKLINE)
#define LUA_MASKCOUNT	(1 << LUA_HOOKCOUNT)

typedef struct lua_Debug lua_Debug;  /* activation record */

typedef void (*lua_Hook) (lua_State *L, lua_Debug *ar);


LUA_API int lua_getstack (lua_State *L, int level, lua_Debug *ar);
LUA_API int lua_getinfo (lua_State *L, const char *what, lua_Debug *ar);
LUA_API const char *lua_getlocal (lua_State *L, const lua_Debug *ar, int n);
LUA_API const char *lua_setlocal (lua_State *L, const lua_Debug *ar, int n);
LUA_API const char *lua_getupvalue (lua_State *L, int funcindex, int n);
LUA_API const char *lua_setupvalue (lua_State *L, int funcindex, int n);

LUA_API int lua_sethook (lua_State *L, lua_Hook func, int mask, int count);
LUA_API lua_Hook lua_gethook (lua_State *L);
LUA_API int lua_gethookmask (lua_State *L);
LUA_API int lua_gethookcount (lua_State *L);


#define LUA_IDSIZE	60

struct lua_Debug {
  int event;
  const char *name;	/* (n) */
  const char *namewhat;	/* (n) `global', `local', `field', `method' */
  const char *what;	/* (S) `Lua', `C', `main', `tail' */
  const char *source;	/* (S) */
  int currentline;	/* (l) */
  int nups;		/* (u) number of upvalues */
  int linedefined;	/* (S) */
  char short_src[LUA_IDSIZE]; /* (S) */
  /* private part */
  int i_ci;  /* active function */
};

/* }====================================================================== */


#define LUA_COLIBNAME	"coroutine"
LUALIB_API int luaopen_base (lua_State *L);

#define LUA_TABLIBNAME	"table"
LUALIB_API int luaopen_table (lua_State *L);

#define LUA_IOLIBNAME	"io"
#define LUA_OSLIBNAME	"os"
LUALIB_API int luaopen_io (lua_State *L);

#define LUA_STRLIBNAME	"string"
LUALIB_API int luaopen_string (lua_State *L);

#define LUA_MATHLIBNAME	"math"
LUALIB_API int luaopen_math (lua_State *L);

#define LUA_DBLIBNAME	"debug"
LUALIB_API int luaopen_debug (lua_State *L);


LUALIB_API int luaopen_loadlib (lua_State *L);


/* to help testing the libraries */
#ifndef lua_assert
#define lua_assert(c)		/* empty */
#endif


/* compatibility code */
#define lua_baselibopen	luaopen_base
#define lua_tablibopen	luaopen_table
#define lua_iolibopen	luaopen_io
#define lua_strlibopen	luaopen_string
#define lua_mathlibopen	luaopen_math
#define lua_dblibopen	luaopen_debug


typedef struct luaL_reg {
  const char *name;
  lua_CFunction func;
} luaL_reg;


LUALIB_API void luaL_openlib (lua_State *L, const char *libname,
                               const luaL_reg *l, int nup);
LUALIB_API int luaL_getmetafield (lua_State *L, int obj, const char *e);
LUALIB_API int luaL_callmeta (lua_State *L, int obj, const char *e);
LUALIB_API int luaL_typerror (lua_State *L, int narg, const char *tname);
LUALIB_API int luaL_argerror (lua_State *L, int numarg, const char *extramsg);
LUALIB_API const char *luaL_checklstring (lua_State *L, int numArg, size_t *l);
LUALIB_API const char *luaL_optlstring (lua_State *L, int numArg,
                                           const char *def, size_t *l);
LUALIB_API lua_Number luaL_checknumber (lua_State *L, int numArg);
LUALIB_API lua_Number luaL_optnumber (lua_State *L, int nArg, lua_Number def);

LUALIB_API void luaL_checkstack (lua_State *L, int sz, const char *msg);
LUALIB_API void luaL_checktype (lua_State *L, int narg, int t);
LUALIB_API void luaL_checkany (lua_State *L, int narg);

LUALIB_API int   luaL_newmetatable (lua_State *L, const char *tname);
LUALIB_API void  luaL_getmetatable (lua_State *L, const char *tname);
LUALIB_API void *luaL_checkudata (lua_State *L, int ud, const char *tname);

LUALIB_API void luaL_where (lua_State *L, int lvl);
LUALIB_API int luaL_error (lua_State *L, const char *fmt, ...);

LUALIB_API int luaL_findstring (const char *st, const char *const lst[]);

LUALIB_API int luaL_ref (lua_State *L, int t);
LUALIB_API void luaL_unref (lua_State *L, int t, int ref);

LUALIB_API int luaL_getn (lua_State *L, int t);
LUALIB_API void luaL_setn (lua_State *L, int t, int n);

LUALIB_API int luaL_loadfile (lua_State *L, const char *filename);
LUALIB_API int luaL_loadbuffer (lua_State *L, const char *buff, size_t sz,
                                const char *name);



/*
** ===============================================================
** some useful macros
** ===============================================================
*/

#define luaL_argcheck(L, cond,numarg,extramsg) if (!(cond)) \
                                               luaL_argerror(L, numarg,extramsg)
#define luaL_checkstring(L,n)	(luaL_checklstring(L, (n), NULL))
#define luaL_optstring(L,n,d)	(luaL_optlstring(L, (n), (d), NULL))
#define luaL_checkint(L,n)	((int)luaL_checknumber(L, n))
#define luaL_checklong(L,n)	((long)luaL_checknumber(L, n))
#define luaL_optint(L,n,d)	((int)luaL_optnumber(L, n,(lua_Number)(d)))
#define luaL_optlong(L,n,d)	((long)luaL_optnumber(L, n,(lua_Number)(d)))


/*
** {======================================================
** Generic Buffer manipulation
** =======================================================
*/


#ifndef LUAL_BUFFERSIZE
#define LUAL_BUFFERSIZE	  BUFSIZ
#endif


typedef struct luaL_Buffer {
  char *p;			/* current position in buffer */
  int lvl;  /* number of strings in the stack (level) */
  lua_State *L;
  char buffer[LUAL_BUFFERSIZE];
} luaL_Buffer;

#define luaL_putchar(B,c) \
  ((void)((B)->p < ((B)->buffer+LUAL_BUFFERSIZE) || luaL_prepbuffer(B)), \
   (*(B)->p++ = (char)(c)))

#define luaL_addsize(B,n)	((B)->p += (n))

LUALIB_API void luaL_buffinit (lua_State *L, luaL_Buffer *B);
LUALIB_API char *luaL_prepbuffer (luaL_Buffer *B);
LUALIB_API void luaL_addlstring (luaL_Buffer *B, const char *s, size_t l);
LUALIB_API void luaL_addstring (luaL_Buffer *B, const char *s);
LUALIB_API void luaL_addvalue (luaL_Buffer *B);
LUALIB_API void luaL_pushresult (luaL_Buffer *B);


/* }====================================================== */



/*
** Compatibility macros and functions
*/

LUALIB_API int   lua_dofile (lua_State *L, const char *filename);
LUALIB_API int   lua_dostring (lua_State *L, const char *str);
LUALIB_API int   lua_dobuffer (lua_State *L, const char *buff, size_t sz,
                               const char *n);


#define luaL_check_lstr 	luaL_checklstring
#define luaL_opt_lstr 	luaL_optlstring 
#define luaL_check_number 	luaL_checknumber 
#define luaL_opt_number	luaL_optnumber
#define luaL_arg_check	luaL_argcheck
#define luaL_check_string	luaL_checkstring
#define luaL_opt_string	luaL_optstring
#define luaL_check_int	luaL_checkint
#define luaL_check_long	luaL_checklong
#define luaL_opt_int	luaL_optint
#define luaL_opt_long	luaL_optlong

#define Lua_CFunction					lua_CFunction
#define Lua_State						lua_State
#define Lua_Create						lua_open
#define Lua_Release(L)					lua_close(L)
#define Lua_GetTopIndex(L)				lua_gettop(L)
#define Lua_SetTopIndex(L,nIndex)		lua_settop(L,nIndex)
#define Lua_PushValue(L,nIndex)			lua_pushvalue(L,nIndex)
#define Lua_RemoveValue(L,nIndex)		lua_remove(L,nIndex)
#define Lua_InsertValue(L,nIndex)		lua_insert(L,nIndex)
#define Lua_GetStackSpace(L)			lua_stackspace(L)
#define Lua_GetValueType(L,nIndex)		lua_type(L,nIndex)
#define Lua_GetTypeName(L,Tag)			lua_typename(L,Tag)
#define Lua_IsNumber(L,nIndex)			lua_isnumber(L,nIndex)
#define Lua_IsString(L,nIndex)			lua_isstring(L,nIndex)
#define Lua_IsCFunction(L,nIndex)		lua_iscfunction(L,nIndex)
#define Lua_IsTable(L,nIndex)			lua_istable(L,nIndex)
#define Lua_GetValueTag(L,nIndex)		lua_tag(L,nIndex)
#define Lua_IsEqual(L,index1,index2)	lua_equal(L,index1,index2)
#define Lua_IsLessThan(L,index1,index2)	lua_lessthan(L,index1,index2)
#define Lua_ValueToNumber(L,nIndex)		lua_tonumber(L,nIndex)
#define Lua_ValueToString(L,nIndex)		lua_tostring(L,nIndex)
#define Lua_GetStrLen(L,nIndex)			lua_strlen(L,nIndex)
#define Lua_ValueToCFunction(L,nIndex)	lua_tocfunction(L,nIndex)
#define Lua_ValueToUserData(L,nIndex)	lua_touserdata(L,nIndex)
#define Lua_ValueToPoint(L,nIndex)		lua_topointer(L,nIndex)

/*
** push functions (C -> stack)
*/

#define Lua_PushNil(L)					lua_pushnil(L)
#define Lua_PushNumber(L,Number)		lua_pushnumber(L,Number)
#define Lua_PushLString(L,LString,Len)	lua_pushlstring(L,LString,Len)
#define Lua_PushString(L,String)		lua_pushstring(L,String)
#define Lua_PushCClosure(L,Fun,N)		lua_pushcclosure(L,Fun,N)
#define Lua_PushUserTag(L,PVoid,Tag)	lua_pushusertag(L,PVoid,Tag)

#define Lua_GetGlobal(L,Valuename)		lua_getglobal(L,Valuename)
#define Lua_GetTable(L,nIndex)			lua_gettable(L,nIndex)
#define Lua_RawGet(L,nIndex)			lua_rawget(L,nIndex)
#define Lua_RawGetI(L,nIndex,n)			lua_rawgeti(L,nIndex,n)
#define Lua_GetGlobals(L)				lua_getglobals(L)
#define Lua_GetTagMethod(L,iTag,cEvent)	lua_gettagmethod(L,iTag,cEvent)
#define Lua_GetRef(L,iRef)				lua_getref(L,iRef)
#define Lua_NewTable(L)					lua_newtable(L)


#define Lua_SetGlobal(L,cName)			lua_setglobal(L,cName)
#define Lua_SetTable(L,nIndex)			lua_settable(L,nIndex)
#define Lua_RawSet(L,nIndex)			lua_rawset(L,nIndex)
#define Lua_RawSetI(L,nIndex,nNum)		lua_rawseti(L,nIndex,nNum)
#define Lua_SetGlobals(L)				lua_setglobals(L)
#define Lua_SetTagMethod(L,iTag,cEvent)	lua_settagmethod(L,iTag,cEvent)
#define Lua_SetRef(L,nLock)				lua_ref(L,nLock)

#define Lua_Call(L,nArgs,nResults)		lua_pcall(L,nArgs,nResults,0)
#define Lua_RawCall(L,nArgs,nResults)	lua_rawcall(L,nArgs,nResults)
#define	Lua_ExecuteFile(L,cFilename)	lua_dofile(L,cFilename)
#define Lua_ExecuteString(L,cString)	lua_dostring(L,cString)
#define Lua_ExecuteBuffer(L,pBuff,iSize,cname)	lua_dobuffer(L,pBuff,iSize,cname)

#define Lua_GetGCThreshold(L)			lua_getgcthreshold(L)
#define Lua_GetGCCount(L)				lua_getgccount(L)
#define Lua_SetGCThreshold(L,nThreshold)lua_setgcthreshold(L,nThreshold)


/*
** miscellaneous functions
*/

#define Lua_NewTag(L)					lua_newtag(L)
#define Lua_CopyTagMethods(L,nTagTo,nTagFrom)	lua_copytagmethods(L,nTagTo,nTagFrom)
#define Lua_SetTag(L,nTag)				lua_settag(L,nTag)
#define	Lua_Error(L,cError)				lua_error(L,cError)
#define Lua_UnRef(L,nRef)				lua_unref(L,nRef)
#define Lua_Next(L,nIndex)				lua_next(L,nIndex)
#define Lua_GetN(L,nIndex)				lua_getn(L,nIndex)
#define Lua_Concat(L,nNum)				lua_concat(L,nNum)
#define Lua_NewUserData(L,nSize)		lua_newuserdata(L,nSize)


#define Lua_OpenBaseLib(L)				 lua_baselibopen(L)
#define Lua_OpenIOLib(L)				 lua_iolibopen(L)
#define Lua_OpenStrLib(L)				 lua_strlibopen(L)
#define Lua_OpenMathLib(L)				 lua_mathlibopen(L)
#define Lua_OpenDBLib(L)				 lua_dblibopen(L)


/* 
** ===============================================================
** some useful macros
** ===============================================================
*/
#define Lua_Pop(L,nIndex)				lua_pop(L,nIndex)
#define Lua_Register(L,cfname,pFun)		lua_register(L,cfname,pFun)
#define Lua_PushUserData(L,UseData)	lua_pushuserdata(L,UseData)
#define Lua_PushCFunction(L,pFun)	lua_pushcfunction(L,pFun)
#define Lua_CloneTag(L,nTag)		lua_clonetag(L,nTag)
#define Lua_GetRegistry(L)			lua_getregistry(L)


#define Lua_CompileBuffer(L,pBUFF,nBuffSize,cBname)	luaL_loadbuffer(L,pBUFF,nBuffSize,cBname)
#define Lua_CompileFile(L,cFilename)		luaL_loadfile(L,cFilename)
#define Lua_Execute(L)				lua_pcall(L, 0, 0, 0)
#define Lua_SafeBegin(L)		lua_gettop(L)
#define Lua_SafeEnd(L,nIndex)		Lua_SetTopIndex(L,nIndex)


/******************************************************************************
* Copyright (C) 1994-2006 Tecgraf, PUC-Rio.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

#ifdef __cplusplus
}
#endif 

#endif
