/********************************************************************
	created:	2003/06/18
	file base:	Cipher
	file ext:	h
	author:		liupeng
	
	purpose:	
*********************************************************************/
#ifndef __INCLUDE_CIPHER_H__
#define __INCLUDE_CIPHER_H__

/*
 * ProtocolType same as s2c_accountbegin in the protocol.h file
 */
#define CIPHER_PROTOCOL_TYPE	0x20

#ifndef OLD_ENCRYPT
#define OLD_ENCRYPT
#endif

#pragma pack(push, 1)

#ifndef OLD_ENCRYPT
typedef struct 
{
	unsigned char	ProtocolType;
    unsigned char	Mode;
    unsigned char	Reserve1[3];
    unsigned		ServerKey;
    unsigned char	Reserve3[8];
    unsigned		ClientKey;
    unsigned char	Reserve2[21];
} ACCOUNT_BEGIN;
#define NE_MAKEKEY(key)		(~((((key) & 0xFF000000) >> 24) | (((key) & 0x000000FF) << 24) | ((key) & 0x00FFFF00)))
#define NE_GETKEY(key)			((((~(key)) & 0xFF000000) >> 24) | ((~(key) & 0x000000FF) << 24) | (~(key) & 0x00FFFF00))
#else
typedef struct 
{
	unsigned char	ProtocolType;
    unsigned char	Mode;
    unsigned char	Reserve1[6];
    unsigned        ServerKey;
    unsigned        ClientKey;
    unsigned char	Reserve2[16];
} ACCOUNT_BEGIN;
#define NE_MAKEKEY(key)		(~(key))
#define NE_GETKEY(key)		(~(key))
#endif

#pragma pack(pop)

#endif // __INCLUDE_CIPHER_H__