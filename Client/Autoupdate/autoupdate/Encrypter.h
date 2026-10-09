#ifndef K_ENCRYPTER_H
#define K_ENCRYPTER_H

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 2007-04-13 02:43
//      File_base        : Encrypter.h
//      File_ext         : .h
//      Author           : Brianyao(yaojie)
//      Description      : 加密解密接口
//
//////////////////////////////////////////////////////////////////////

struct IEncrypter
{
	virtual ~IEncrypter(){}
	virtual void Encrypt(const char * szSourceBuffer,char * szDestBuffer,const unsigned long dwSize)=0;
	virtual BOOL Encrypt(const char * szSourceFileName,char * szDestFileName)=0;
	virtual void Unencrypt(const char * szSourceBuffer,char * szDestBuffer,const unsigned long dwSize)=0;
	virtual BOOL Unencrypt(const char * szSourceFileName,char * szDestFileName)=0;
};

IEncrypter * GetXOREncrypter(void);

#endif