//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-6-12 10:22
//      File_base        : AccountDef
//      File_ext         : h
//      Author           : 
//      Description      : 
//////////////////////////////////////////////////////////////////////

#ifndef __ACCOUNTLOGINDEF_H__
#define __ACCOUNTLOGINDEF_H__

#pragma once

#include "KWin32.h"


#pragma	pack(push, 1)

//----------------------------------------------------------------

#define LOGIN_USER_ACCOUNT_MIN_LEN			4
#define LOGIN_USER_ACCOUNT_MAX_LEN			32
#define LOGIN_USER_PASSWORD_MIN_LEN			6
#define LOGIN_USER_PASSWORD_MAX_LEN			64
#define LOGIN_USER_PRESENT_CODE_MAX_LEN     32

#define LOGIN_USER_TOCKEN_PASSWORD_LEN      10
#define LOGIN_USER_MATRIX_POS_LEN           9
#define LOGIN_USER_MATRIX_PASSWORD_LEN      9

//login action return value
#define ACTION_SUCCESS					1
#define ACTION_FAILED					2
#define E_ACCOUNT_OR_PASSWORD			3	//用户密码错
#define E_ACCOUNT_NODEPOSIT				5	//点卡余额为零、或无点卡
#define E_ACCOUNT_FREEZE				8	//被冻结
#define E_ACCOUNT_SMS_LOCK				12	//短信冻结
#define E_ACCOUNT_NOT_ACTIVE			13	//帐号未激活
#define E_ACCOUNT_IN_GATEWAY			15  //帐号已在别的网管登录且未下线

#define E_CDKEY							70	//错误的激活码
#define E_ACTIVE						71	//帐号需要激活


#define E_IB_NO_ENOUGH_COIN				1100 //错误代码，表示玩家的金币数（通宝数）余额不足。
#define E_IB_ITEM_NOT_EXIST				1101 //错误代码，表示物品不存在。
#define E_IB_ITEM_HAS_BEEN_USED			1102 //错误代码，物品存在，但是物品已被消耗。
#define E_IB_ITEM_EXPIRED				1103 //错误代码，物品存在，但是该物品已过期。

#define E_PARAM_ERROR					1009
#define E_ZONE_ACCOUNT_ID_NOT_EXIST		1015

#define	S_IB_ITEM_NOT_IN_SAME_GATEWAY	1200

#define E_ACCOUNT_OR_PASSWORD_1			4001
#define E_ACCOUNT_OR_PASSWORD_2			4002
#define E_ACCOUNT_OR_PASSWORD_3			4003
#define E_ACCOUNT_OR_PASSWORD_4			4004

//【密宝协议修改】 以下错误代码用于密宝 
#define S_PASSPOD_SUCCESS                           5000 //密保验证成功
#define E_PASSPOD_SYSTEM                            5001 //PASSPOD系统错误
#define E_PASSPOD_USED                              5002 //令牌已使用
#define E_PASSPOD_FAILED                            5003 //验证失败
#define E_PASSPOD_EXPIRED                           5004 //令牌过期
#define E_PASSPOD_NOTFOUND                          5005 //令牌绑定未找到
#define E_PASSPOD_DISABLE                           5006 //令牌已经禁用（挂失）

//其余错误码可以直接打印，也可以添加，
//----

#define ACCOUNT_CURRENT_VERSION			0x1
#define CHANGE_EXT_POINT_SILVER         0x1

#ifdef WIN32
#define PGUID __int64
#else	
#define PGUID long long
#endif
//----------------------------------------------------------------

enum l2p_PROTOCOL
{	
	l2p_accountlogout=35,//玩家离开游戏通知paysys回包 35
	l2p_gatewayverify=36,// lord登陆的paysys包 36
	l2p_gatewayverifyagain=37,//用于lord重连后的认证包 37
	l2p_gatewayclose=39,//lord关闭向paysys通知 39
	l2p_accountlogin=62,//玩家登陆Paysys回包以及二级密码 62
	l2p_account_change_extpoint=40,//lord向paysys扩展点操作包 40
	
	//--------------------------->
	l2p_use_spreader_cdkey = 50,//新手推广员
	l2p_ib_buy_item = 60,//ib物品购买包
	l2p_ib_use_item = 61,//ib物品使用包
	l2p_activate_present = 69, //激活礼品卡
	l2p_ping = 112, //ping


	//--------------------------->
	l2p_end
};

enum p2l_PROTOCOL
{
	p2l_accountlogout=35,//玩家离开游戏通知paysys 35
	p2l_gatewayverify=36,//lord登陆的paysys回包 36
	p2l_accountlogin=62,//玩家登陆Paysys以及二级密码 62
	p2l_account_change_extpoint=40,//lord向paysys扩展点操作返回包 40

	//--------------------------->
	p2l_use_spreader_cdkey = 50,//新手推广员返回包
	p2l_ib_buy_item = 60,//ib物品购买回包
	p2l_ib_use_item = 61,//ib物品使用包回包
	p2l_activate_present = 69, //激活礼品卡
	p2l_ping = 130, //ping返回包


	//--------------------------->
	p2l_end
};

enum
{
	AccountUser = 1,
	AccountUserLoginInfo,
	ServerAccountUserLoginInfo,
	AccountUserReturn,
	AccountUserTimeInfo,
	ServerOptionInfo,
	ServerInfo,
	AccountUserVerify,
	AccountUserReturnEx,
	AccountUserLogout,
    AccountChangeExtPoint,
	AccountUserGetIp,				//RelayServer使用
	
	AccountSpreaderCDKEY = 22,		//传递新手推广CDKEY给PaySys
	AccountSpreaderCDKEYRet = 23,	//返回新手推广CDKEY使用结果
	AccountIB_ItemBuy = 30,			//传递购物申请给PaySys
	AccountIB_ItemBuyRet = 31,		//返回传递购物结果
	AccountIB_ItemUse = 32,			//传递物品使用申请给PaySys
	AccountIB_ItemUseRet = 33,		//返回物品使用结果
	AccountCommon    = 48,   //通用协议
	AccountComonRet  = 49,   //通用协议结果

};

enum 
{
	PaySys_AddFreeDay	=	0,
	PaySys_AddFreePoint,
	PaySys_NewPlayer_SN,
	PaySys_SilverCount,
	PaySys_WorldMoney,
	PaySys_GetSecPW
};

//----------------------------------------------------------------

struct KAccountHead
{
	WORD	Size;		// size of the struct
	WORD	Version;	// ACCOUNT_CURRENT_VERSION
	WORD	Type;		// such as AccountUser .etc
	DWORD	Operate;	// lord used
};

struct KAccountUser : public KAccountHead
{
	char Account[LOGIN_USER_ACCOUNT_MAX_LEN];	//account
};

struct KAccountUserLoginInfo : public KAccountUser
{
	char Password[LOGIN_USER_PASSWORD_MAX_LEN];	//password
    DWORD UserIP;
};

struct KServerAccountUserLoginInfo : public KAccountUserLoginInfo
{
	DWORD dwIP;
	BYTE MacAddress[6];
	DWORD nLastTime;
};

struct KAccountUserLoginInfoExt : public KAccountUserLoginInfo
{
	int					nUserPort;        /* port */
	BYTE                byMachineID[16];  /*【密宝协议修改】相同的MachineID的连接被认为是同一台电脑多开*/
	unsigned char		nLogout;          /* 1: 表示login时要先logout0: 表示正常登录*/
	char				szActiveCode[LOGIN_USER_ACCOUNT_MAX_LEN];
	//---------【密宝协议修改】--------------------------------------------------
	BYTE                byPasspodVersion;                                  // 密宝版本
    BYTE                byPasspodMode;                                     // 密宝认证方式
    char                szTokenPassword[LOGIN_USER_TOCKEN_PASSWORD_LEN];   // 令牌密码
    char                szMatrixPosition[LOGIN_USER_MATRIX_POS_LEN];       // 需要用户输入的矩阵密码的位置
    char                szMatrixPassword[LOGIN_USER_MATRIX_PASSWORD_LEN];  // 对应矩阵位置的密码
	char				szReserve[33];
	//-----------------------------------------------------------------------
};

// Return Package

struct KAccountUserReturn : public KAccountUser
{
	int nReturn;
};

//用户登录Paysys返回包
struct KAccountUserReturnExt : public KAccountUserReturn
{
	DWORD nExtPoint;        //可用的附送点
	DWORD nExtPoint1;        //可用的附送点1
	DWORD nExtPoint2;        //可用的附送点2
	DWORD nExtPoint3;        //可用的附送点3
	DWORD nExtPoint4;        //可用的附送点4
	DWORD nExtPoint5;        //可用的附送点5
	DWORD nExtPoint6;        //可用的附送点6
	DWORD nExtPoint7;        //可用的附送点7
	DWORD nLeftTime;        //剩余时间,现在以秒为单位
	DWORD nLeftTimeOfPoint; //点数对应的剩余时间,现在以秒为单位，nLeftTime-nLeftTimeOfTime可以得到包月时间
	DWORD dwLastLoginTime;	//最後一次登录时间 
	DWORD dwLastLoginIP;	//最後一次登录ip	
	DWORD dwLeftMoney;		//剩余金币数
	DWORD dwOnlineSecond;		//
	DWORD dwLimitPlayTimeFlag;	//防沉迷标识	
								//0：表示该用户不纳入防沉迷管辖，并且没有实名信息（老用户）
								//1：表示该用户纳入防沉迷管辖，并且没有实名信息（老用户）
								//2：表示该用户不纳入防沉迷管辖，并且有实名信息
								//3：表示该用户纳入防沉迷管辖，并且有实名信息
	                            //255：表示防沉迷系统没有开启
	DWORD dwLimitOnlineSecond;	//防沉迷在线累计
	DWORD dwLimitOfflineSecond;	//防沉迷离线累计
	//---------【密宝协议修改】--------------------------------------------------
	int           nChargeFlag;           
	DWORD         uAccountState; 
	char          szPhoneNumber[20];     
	DWORD         dwGatewayID;            
	BYTE          byPasspodVersion;    //密宝版本
	BYTE          byPasspodMode;       //密宝认证方式 
    char          szMatrixPosition[9]; //需要用户输入的矩阵密码的位置
	char          Reserve[9];
	//-----------------------------------------------------------------------
};

//服务器登出paysys
struct KServerAccountUserReturnExt : public KAccountUserReturn
{
	DWORD		dwVerifyTime;         /*网关的当前时间*/ 	
};

//用户登出paysys
struct KAccountUserLogout : public KAccountUser
{
	WORD nExtPoint; //将要扣除的附送点
};

struct KAccountUserLogoutRet : public KAccountUser
{
	int nReturn;
};

//扩展点包
struct KAccountUserChangeExtPoint : public KAccountUser
{
    union
    {
    unsigned uExtPointIndex;    // 将要改变的附加点的索引
    unsigned uSilverType;       // 银票的类型，高16位（0：表示大银票，1：表示小银票）
                                //             低16位（0：转为点数，：转为包（周）月）
    };
    int      nChangeValue;      // 附加点被修改的值，可正可负，或者银票的数目

    int      nPlayerIndex;      // 用来将返回协议到达GameServer时，表示原始的玩家是谁

    unsigned int uFlag;             // 如果是0表示附加点的变化，如果是CHANGE_EXT_POINT_SILVER : 表示银票的处理
};

struct KAccountUserChangeExtPointRet : public KAccountUserReturn
{
    unsigned int uFlag;
    int      nPlayerIndex;      // 用来将返回协议到达GameServer时，表示原始的玩家是谁
};
//扩展点包

//IB包
//购买
struct KAccountBuyIBItem : public KAccountUser
{
	int nPlayerDataIndex;		//玩家在游戏世界的编号
	int nGoodsIndex;			//商品在游戏世界中的编号
	int nItemTypeID;			//物品类型
	int nItemLevel;				//物品级别
	int nUseType;				//使用类型
	int nPrice;					//物品价格
	DWORD dwOverdueTime;		//过期时间（差值），即是可以使用的秒数
};

struct KAccountBuyIBItemRet : public KAccountUser
{
	int nPlayerDataIndex;		//玩家在游戏世界的编号
	int nGoodsIndex;			//商品在游戏世界中的编号
	int nItemTypeID;			//物品类型
	int nItemLevel;				//物品级别
	int nPrice;					//物品价格
	PGUID	GUID;				//GUID
	int nResult;				//返回结果
};

struct KAccountUseIBItem : public KAccountUser
{
	int nPlayerDataIndex;		//玩家在游戏世界的编号
	int nItemTypeID;			//物品类型
	int nItemLevel;				//物品级别
	PGUID	GUID;				//GUID
};

struct KAccountUseIBItemRet : public KAccountUser
{
	int nPlayerDataIndex;		//玩家在游戏世界的编号
	int nItemTypeID;			//物品类型
	int nItemLevel;				//物品级别
	PGUID	GUID;				//GUID
	int nResult;				//返回结果
};

//礼品卡激活

struct KGameworldPaysysCommon : public KAccountHead //通用协议 协议头
{
    unsigned    uDataSize;
    BYTE        byData[1];
};

struct KAccountActivePresentCode 
{
	BYTE        ProtocolType;
    char        Account[LOGIN_USER_ACCOUNT_MAX_LEN];
    char        PresentCode[LOGIN_USER_ACCOUNT_MAX_LEN]; //礼品卡号
    DWORD       dwActiveIP;                              //激活IP
};
struct KAccountActivePresentCodeRet 
{
	BYTE        ProtocolType;
    char        Account[LOGIN_USER_ACCOUNT_MAX_LEN];
    char        PresentCode[LOGIN_USER_ACCOUNT_MAX_LEN]; //礼品卡号
    DWORD       dwActiveIP;                              //激活IP
    DWORD       dwPresentType;                           //礼品卡类型，用于游戏世界决定用的礼品
    
    int         nResult;
};

//----------------------------------------------------------------

#pragma	pack(pop)

#endif