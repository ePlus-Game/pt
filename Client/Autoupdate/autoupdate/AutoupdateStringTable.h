#ifndef _AUTOUPDATE_STRING_TABLE_
#define _AUTOUPDATE_STRING_TABLE_

#define K_KINGSOFT_URL					"http://www.xoyo.com"
#define K_BLAZE_URL						"http://fs.xoyo.com"

#ifndef TRADITIONAL_CHINESE
//--简体中文版--
	#define K_HOME_PAGE					"http://fs.xoyo.com"
	#define K_URL_COMPANY				K_KINGSOFT_URL
	#define K_JXONLINE_FILE_NAME_0		"FS2Run"
	#define K_JXONLINE_FILE_NAME_1		"FS2Run.exe"
	#define K_GAME_FILE_NAME_0			"FSOnline2"
	#define K_GAME_FILE_NAME_1			"FSOnline2.exe"

#if ( !defined FS_LANG || FS_LANG == 0 )
#pragma message("[FS_LANG=0 or not defined FS_LANG]")
	#define U_TOTAL_PROGRESS			"总体更新进度"
	#define U_CURRENT_FILE_PROGRESS		"当前更新文件"
	#define U_CANCEL_UPDATE				"取消升级..."
	#define U_CONNECTING_SERVER			"正在连接服务器..."
	#define U_VERIFY_ACCOUNT			"正在进行用户校验..."
	#define U_LOADING_UPDATE_INFO		"正在下载更新信息..."
	#define U_DOWNLOADING_FILE			"正在下载文件..."
	#define U_FILE_DOWNLOADING_STATUS	"%s --- %d KB(%d KB)"
	#define U_UPDATE_SYSTEM				"正在更新系统..."
	#define U_UPDATE_FINISH				"升级完成！"
	#define U_NEEDLESS_UPDATE			"已经是最新版本，不需要升级。"
	#define U_LOADICON_ERR				"LoadIcon(IDR_MAINFRAME) 出错"
	#define U_LOAD_DLL					"调入动态库'"
	#define U_GET_FUN_ADDRESS			"取得函数地址'"
	#define U_RELEASE_MODULE			"释放模块'"
	#define U_TAIL_ERROR				"'出错"
	#define U_SELECT_FOLDER				"浏览文件夹"
	#define U_AUTOUPDATE_SITE			"自动升级站点%d"
	#define U_ANALYSE_ADDRESS_ERR		"地址解析出错！"
	#define U_ANALYSE_ADDRESS_ERR_2		"下载服务状态文件时，解析地址出错！"
	#define U_DOWNLOAD_FOLDER_LIST_FILE	"正在下载目录列表文件..."
	#define U_DOWNLOAD_SVR_STAT_SUCC	"下载服务器状态成功！"
	#define U_DOWNLOAD_SVR_STAT_FAIL	"下载服务器状态失败！"
	#define U_CONNECT_DOWNLOAD_SVR		"正在连接下载服务器"
	#define U_TRY_UPDATE_SERVER			"正在尝试在服务器[%i]进行下载，请稍等..."
	#define U_TO_RESTART_SELF			"下载升级包成功，即将重新启动升级程序..."
	#define U_NO_AVAILABLE_SERVER		"无可用的更新服务器"
	#define U_FAILT_BUT_CAN_TRY			"更新失败，您可以尝试进入游戏，但客户端可能不是最新。"
	#define U_FAILT_BUT_CAN_TRY_2		"更新服务器不可用,点击这里进行手动更新下载."

	#define	U_DETAILHEADER_FILENAME			"文件"
	#define	U_DETAILHEADER_PROCESS			"进度"
	#define U_DETAILHEADER_STATUS			"状态"
	#define U_DETAILHEADER_STATUS_COMPLETE	"完成"
	#define U_DETAILHEADER_STATUS_DOWNING	"更新中"

	#define L_CONNECT_SERVER_SUCC		"连接下载服务器成功"
	#define L_CANT_READ_DATA			"读取不到数据"
	#define L_CANCEL_DOWNLOAD			"用户取消下载"
	#define L_DOWN_AUTOUPDATE_SUCC		"下载升级程序成功"
	#define L_UPDAE_SUCC				"升级成功完成"

	// Dialog Message when ftp server isn't usable
	#define	CANTUPDATE_DLGMSG_1			"更新未能完成，但您可以尝试进入游戏。为了能正常游戏，请到"
	#define	CANTUPDATE_DLGMSG_2			"http://fs2.xoyo.com"
	#define	CANTUPDATE_DLGMSG_3			"上查询或者下载手动更新包。您确定要进入游戏吗？"
	//

	// Dialog Message when enter game on updating
	#define ENTERGAMEONUPDATE_DLGMSG_1	"正在更新中，您可以进入游戏，系统会在后台更新，下次进入"
	#define ENTERGAMEONUPDATE_DLGMSG_2	"游戏时完成此次更新，"
	#define	ENTERGAMEONUPDATE_DLGMSG_3	"建议您更新完成再进入游戏，您确定要进入游戏吗？"
	//

	#define	MSG_UPDATE_NOTFORCE			"非强制更新，可以进入游戏"
	#define	MSG_UPDATE_FORCE			"强制更新，更新完成前无法进入游戏，请耐心等待"

#elif ( FS_LANG == 1 )
#pragma message("[FS_LANG=1]")
	#define U_TOTAL_PROGRESS			"羆砰穝秈"
	#define U_CURRENT_FILE_PROGRESS		"讽玡穝ゅン"
	#define U_CANCEL_UPDATE				"ど..."
	#define U_CONNECTING_SERVER			"タ硈钡狝叭竟..."
	#define U_VERIFY_ACCOUNT			"タ秈︽ノめ喷..."
	#define U_LOADING_UPDATE_INFO		"タ更穝獺..."
	#define U_DOWNLOADING_FILE			"タ更ゅン..."
	#define U_FILE_DOWNLOADING_STATUS	"%s --- %d KB(%d KB)"
	#define U_UPDATE_SYSTEM				"タ穝╰参..."
	#define U_UPDATE_FINISH				"どЧΘ"
	#define U_NEEDLESS_UPDATE			"竒琌程穝セぃ惠璶ど"
	#define U_LOADICON_ERR				"LoadIcon(IDR_MAINFRAME) 岿"
	#define U_LOAD_DLL					"秸笆篈畐'"
	#define U_GET_FUN_ADDRESS			"眔ㄧ计'"
	#define U_RELEASE_MODULE			"睦家遏'"
	#define U_TAIL_ERROR				"'岿"
	#define U_SELECT_FOLDER				"聅凝ゅンЖ"
	#define U_AUTOUPDATE_SITE			"笆ど翴%d"
	#define U_ANALYSE_ADDRESS_ERR		"秆猂岿"
	#define U_ANALYSE_ADDRESS_ERR_2		"更狝叭篈ゅン秆猂岿"
	#define U_DOWNLOAD_FOLDER_LIST_FILE	"タ更ヘ魁ゅン..."
	#define U_DOWNLOAD_SVR_STAT_SUCC	"更狝叭竟篈Θ\xA5\x5C"
	#define U_DOWNLOAD_SVR_STAT_FAIL	"更狝叭竟篈ア毖"
	#define U_CONNECT_DOWNLOAD_SVR		"タ硈钡更狝叭竟"
	#define U_TRY_UPDATE_SERVER			"タ沽刚狝叭竟[%i]秈︽更叫祔单..."
	#define U_TO_RESTART_SELF			"更どΘ\xA5\x5C盢穝币笆ど祘..."
	#define U_NO_AVAILABLE_SERVER		"礚ノ穝狝叭竟"
	#define U_FAILT_BUT_CAN_TRY			"穝ア毖眤沽刚秈笴栏め狠ぃ琌程穝"
	#define U_FAILT_BUT_CAN_TRY_2		"穝狝叭竟ぃノ眤沽刚秈笴栏"

	#define	U_DETAILHEADER_FILENAME			"ゅン"
	#define	U_DETAILHEADER_PROCESS			"秈"
	#define U_DETAILHEADER_STATUS			"篈"
	#define U_DETAILHEADER_STATUS_COMPLETE	"ЧΘ"
	#define U_DETAILHEADER_STATUS_DOWNING	"穝い"

	#define L_CONNECT_SERVER_SUCC		"硈钡更狝叭竟Θ\xA5\x5C"
	#define L_CANT_READ_DATA			"弄ぃ计沮"
	#define L_CANCEL_DOWNLOAD			"ノめ更"
	#define L_DOWN_AUTOUPDATE_SUCC		"更ど祘Θ\xA5\x5C"
	#define L_UPDAE_SUCC				"どΘ\xA5\x5CЧΘ"
#elif ( FS_LANG == 2 )
#pragma message("[FS_LANG=2]")
	#define U_TOTAL_PROGRESS			"总体更新进度"
	#define U_CURRENT_FILE_PROGRESS		"当前更新文件"
	#define U_CANCEL_UPDATE				"取消升级..."
	#define U_CONNECTING_SERVER			"正在连接服务器..."
	#define U_VERIFY_ACCOUNT			"正在进行用户校验..."
	#define U_LOADING_UPDATE_INFO		"正在下载更新信息..."
	#define U_DOWNLOADING_FILE			"正在下载文件..."
	#define U_FILE_DOWNLOADING_STATUS	"%s --- %d KB(%d KB)"
	#define U_UPDATE_SYSTEM				"正在更新系统..."
	#define U_UPDATE_FINISH				"升级完成！"
	#define U_NEEDLESS_UPDATE			"已经是最新版本，不需要升级。"
	#define U_LOADICON_ERR				"LoadIcon(IDR_MAINFRAME) 出错"
	#define U_LOAD_DLL					"调入动态库'"
	#define U_GET_FUN_ADDRESS			"取得函数地址'"
	#define U_RELEASE_MODULE			"释放模块'"
	#define U_TAIL_ERROR				"'出错"
	#define U_SELECT_FOLDER				"浏览文件夹"
	#define U_AUTOUPDATE_SITE			"自动升级站点%d"
	#define U_ANALYSE_ADDRESS_ERR		"地址解析出错！"
	#define U_ANALYSE_ADDRESS_ERR_2		"下载服务状态文件时，解析地址出错！"
	#define U_DOWNLOAD_FOLDER_LIST_FILE	"正在下载目录列表文件..."
	#define U_DOWNLOAD_SVR_STAT_SUCC	"下载服务器状态成功！"
	#define U_DOWNLOAD_SVR_STAT_FAIL	"下载服务器状态失败！"
	#define U_CONNECT_DOWNLOAD_SVR		"正在连接下载服务器"
	#define U_TRY_UPDATE_SERVER			"正在尝试在服务器[%i]进行下载，请稍等..."
	#define U_TO_RESTART_SELF			"下载升级包成功，即将重新启动升级程序..."
	#define U_NO_AVAILABLE_SERVER		"无可用的更新服务器"
	#define U_FAILT_BUT_CAN_TRY			"更新失败，您可以尝试进入游戏，但客户端可能不是最新。"
	#define U_FAILT_BUT_CAN_TRY_2		"更新服务器不可用，您可以尝试进入游戏。"

	#define	U_DETAILHEADER_FILENAME			"文件"
	#define	U_DETAILHEADER_PROCESS			"进度"
	#define U_DETAILHEADER_STATUS			"状态"
	#define U_DETAILHEADER_STATUS_COMPLETE	"完成"
	#define U_DETAILHEADER_STATUS_DOWNING	"更新中"

	#define L_CONNECT_SERVER_SUCC		"连接下载服务器成功"
	#define L_CANT_READ_DATA			"读取不到数据"
	#define L_CANCEL_DOWNLOAD			"用户取消下载"
	#define L_DOWN_AUTOUPDATE_SUCC		"下载升级程序成功"
	#define L_UPDAE_SUCC				"升级成功完成"
#endif

	
#else
	#include "AutoupdateStringTable_T.h"	//--繁体中文版--

#endif

#endif //_AUTOUPDATE_K_STRING_TABLE_
