//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   SourceDef.h
//  Version     :   1.0
//  Creater     :   
//  Date        :   2002-12-19 16:54:27
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _UPDATE_SOURCE_DEFINE_H_
#define _UPDATE_SOURCE_DEFINE_H_

#if ( !defined FS_LANG || FS_LANG == 0 )
#define defIDS_FILE_ERROR_FORMAT        "文件 %s 不存在或被破坏！"
#define defIDS_ERROR_INDEX              "所选择的升级文件夹中的信息文件（index.dat）被破坏了，请通过Internet升级下载正确的文件，或者通知网络管理员通过Internet升级下载正确的文件。"
#define defIDS_INDEX_ERROR              "分析下载的升级信息文件时出现错误, 请选择其他的升级服务器重新升级. "
#define defIDS_UPDATE_SELF_SUCCESS      "升级程序优先更新自身成功！"
#define defIDS_UPDATE_SELF_FAILED       "升级程序优先更新自身失败！请先确认存放下载文件的目录是否可写，或者该目录下的UpdateSelf.DAT是否可被修改，再重新升级！"
#define defIDS_UPDATE_FAILED            "更新 %s 到 %s 失败!请重新升级!"
#define defIDS_NEED_RESET               "更新 %s 到 %s 成功，需要重新启动"
#define defIDS_UPDATE_SUCCESS           "更新 %s 到 %s 成功"
#define defIDS_COPY_TEMPFILE_FAILED     "更新 %s 到 %s 失败：拷贝临时文件失败"
#define defIDS_PROCESS_SHARING_FAILED   "更新 %s 到 %s 失败：处理冲突文件失败"
#define defIDS_UPDATE_SUCCESS_NEED_RESET "更新 %s 到 %s 成功：文件冲突，需要重新启动"
#define defIDS_UPDATE_SHARING_SUCCESS   "更新 %s 到 %s 成功：文件冲突，需要重新启动后才能完成更新"
#define defIDS_UPDATE_FINISH_FAILED     "更新文件失败，请重新升级！"
#define defIDS_UPDATE_FINISH_SUCCESS    "更新文件成功，恭喜恭喜！"
#define defIDS_NOTIFY_REUPDATE          "为了保证程序能正常运行，请重新升级成功后再运行。\r\n可以选择从局域网或者本地升级，选择刚才文件下载所保存的目录进行升级，无需重新下载文件。"
#define defIDS_NEED_REBOOT              "为了更新正在使用的文件,请重新启动电脑。"
#define defIDS_DOWNLOAD_FILE_FAILED     "下载文件 %s 到 %s 失败"
#define defIDS_DOWNLOAD_FILE_SUCCESSFUL "下载文件 %s 到 %s 成功"
#define defIDS_INVALID_USERCHECK        "非常抱歉，序列号没有通过服务器验证，可能您的序列号不完整或因为网络传输原因出错，请稍后再升级！"
#define defIDS_PIRATIC_SERIAL_NUMBER    "非常抱歉，您的序列号可能正在被其他用户非法使用因此无法通过升级验证。\r\n如果您的产品的确为正版产品，我们将为您提供新序列号！\r\n电话服务：010-86243222\r\n网站服务：http://www.duba.net/user/invalid.html\r\nEmail:antivirus@kingsoft.net"
#define defIDS_INHIBITIVE_SERIAL_NUMBER "非常抱歉，您的序列号属于已经被停用，如果您已经购买了我们的产品请您与我们联系获取新序列号。\r\n电话服务：010-86243222\r\n网站服务：http://www.duba.net/user/invalid.html\r\nEmail：antivirus@kingsoft.net"
#define defIDS_CONNECT_SERVER_FAILED    "非常抱歉，由于服务器忙或者正在更新，请稍后再升级！"
#define defIDS_CHECK_SERIAL_NUMBER_ERROR    "非常抱歉，由于网络繁忙，服务器无法连接，请稍后再升级！"
#define defIDS_HTTP_CHECK_FAILED        "用HTTP验证用户身份失败！"
#define defIDS_USE_HTTP_CHECK           "用UDP验证失败，使用HTTP验证。"
#define defIDS_HTTP_CHECK_SUCCESSFUL    "用HTTP验证用户身份成功！"
#define defIDS_UDP_CHECK_SUCCESSFUL     "用UDP验证用户身份成功！"
#define defIDS_INDEX_ERRORANALYSIS      "分析下载的升级信息文件时出现错误, 请选择其他的升级服务器重新升级. "
#elif ( FS_LANG == 1 )
#define defIDS_FILE_ERROR_FORMAT        "ゅン %s ぃ┪砆瘆胊"
#define defIDS_ERROR_INDEX              "┮匡拒どゅンЖい獺ゅンindex.dat砆瘆胊叫硄筁Internetど更タ絋ゅン┪硄呼蹈恨瞶硄筁Internetど更タ絋ゅン"
#define defIDS_INDEX_ERROR              "だ猂更ど獺ゅン瞷岿粇, 叫匡拒ㄤど狝叭竟穝ど. "
#define defIDS_UPDATE_SELF_SUCCESS      "ど祘纔穝ōΘ\xA5\x5C"
#define defIDS_UPDATE_SELF_FAILED       "ど祘纔穝ōア毖叫絋粄更ゅンヘ魁琌糶┪赣ヘ魁UpdateSelf.DAT琌砆э穝ど"
#define defIDS_UPDATE_FAILED            "穝 %s  %s ア毖!叫穝ど!"
#define defIDS_NEED_RESET               "穝 %s  %s Θ\xA5\x5C惠璶穝币笆"
#define defIDS_UPDATE_SUCCESS           "穝 %s  %s Θ\xA5\x5C"
#define defIDS_COPY_TEMPFILE_FAILED     "穝 %s  %s ア毖ī羬ゅンア毖"
#define defIDS_PROCESS_SHARING_FAILED   "穝 %s  %s ア毖矪瞶侥ゅンア毖"
#define defIDS_UPDATE_SUCCESS_NEED_RESET "穝 %s  %s Θ\xA5\x5Cゅン侥惠璶穝币笆"
#define defIDS_UPDATE_SHARING_SUCCESS   "穝 %s  %s Θ\xA5\x5Cゅン侥惠璶穝币笆ЧΘ穝"
#define defIDS_UPDATE_FINISH_FAILED     "穝ゅンア毖叫穝ど"
#define defIDS_UPDATE_FINISH_SUCCESS    "穝ゅンΘ\xA5\x5C尺尺"
#define defIDS_NOTIFY_REUPDATE          "玂靡祘タ盽笲︽叫穝どΘ\xA5\x5C笲︽\r\n匡拒眖Ы办呼┪セど匡拒ゅン更┮玂ヘ魁秈︽ど礚惠穝更ゅン"
#define defIDS_NEED_REBOOT              "穝タㄏノゅン,叫穝币笆筿福"
#define defIDS_DOWNLOAD_FILE_FAILED     "更ゅン %s  %s ア毖"
#define defIDS_DOWNLOAD_FILE_SUCCESSFUL "更ゅン %s  %s Θ\xA5\x5C"
#define defIDS_INVALID_USERCHECK        "獶盽╆簆腹⊿Τ硄筁狝叭竟喷靡眤腹ぃЧ俱┪呼蹈肚块岿叫祔ど"
#define defIDS_PIRATIC_SERIAL_NUMBER    "獶盽╆簆眤腹タ砆ㄤノめ獶猭ㄏノ礚猭硄筁ど喷靡\r\n狦眤玻珇絋タ玻珇и盢眤矗ㄑ穝腹\r\n筿杠狝叭010-86243222\r\n呼狝叭http://www.duba.net/user/invalid.html\r\nEmail:antivirus@kingsoft.net"
#define defIDS_INHIBITIVE_SERIAL_NUMBER "獶盽╆簆眤腹妮竒砆氨ノ狦眤竒潦禦и玻珇叫眤籔и羛么莉穝腹\r\n筿杠狝叭010-86243222\r\n呼狝叭http://www.duba.net/user/invalid.html\r\nEmailantivirus@kingsoft.net"
#define defIDS_CONNECT_SERVER_FAILED    "獶盽╆簆パ狝叭竟Γ┪タ穝叫祔ど"
#define defIDS_CHECK_SERIAL_NUMBER_ERROR    "獶盽╆簆パ呼蹈羉Γ狝叭竟礚猭硈钡叫祔ど"
#define defIDS_HTTP_CHECK_FAILED        "ノHTTP喷靡ノめōア毖"
#define defIDS_USE_HTTP_CHECK           "ノUDP喷靡ア毖ㄏノHTTP喷靡"
#define defIDS_HTTP_CHECK_SUCCESSFUL    "ノHTTP喷靡ノめōΘ\xA5\x5C"
#define defIDS_UDP_CHECK_SUCCESSFUL     "ノUDP喷靡ノめōΘ\xA5\x5C"
#define defIDS_INDEX_ERRORANALYSIS      "だ猂更ど獺ゅン瞷岿粇, 叫匡拒ㄤど狝叭竟穝ど. "
#elif ( FS_LANG == 2 )
#define defIDS_FILE_ERROR_FORMAT        "文件 %s 不存在或被破坏！"
#define defIDS_ERROR_INDEX              "所选择的升级文件夹中的信息文件（index.dat）被破坏了，请通过Internet升级下载正确的文件，或者通知网络管理员通过Internet升级下载正确的文件。"
#define defIDS_INDEX_ERROR              "分析下载的升级信息文件时出现错误, 请选择其他的升级服务器重新升级. "
#define defIDS_UPDATE_SELF_SUCCESS      "升级程序优先更新自身成功！"
#define defIDS_UPDATE_SELF_FAILED       "升级程序优先更新自身失败！请先确认存放下载文件的目录是否可写，或者该目录下的UpdateSelf.DAT是否可被修改，再重新升级！"
#define defIDS_UPDATE_FAILED            "更新 %s 到 %s 失败!请重新升级!"
#define defIDS_NEED_RESET               "更新 %s 到 %s 成功，需要重新启动"
#define defIDS_UPDATE_SUCCESS           "更新 %s 到 %s 成功"
#define defIDS_COPY_TEMPFILE_FAILED     "更新 %s 到 %s 失败：拷贝临时文件失败"
#define defIDS_PROCESS_SHARING_FAILED   "更新 %s 到 %s 失败：处理冲突文件失败"
#define defIDS_UPDATE_SUCCESS_NEED_RESET "更新 %s 到 %s 成功：文件冲突，需要重新启动"
#define defIDS_UPDATE_SHARING_SUCCESS   "更新 %s 到 %s 成功：文件冲突，需要重新启动后才能完成更新"
#define defIDS_UPDATE_FINISH_FAILED     "更新文件失败，请重新升级！"
#define defIDS_UPDATE_FINISH_SUCCESS    "更新文件成功，恭喜恭喜！"
#define defIDS_NOTIFY_REUPDATE          "为了保证程序能正常运行，请重新升级成功后再运行。\r\n可以选择从局域网或者本地升级，选择刚才文件下载所保存的目录进行升级，无需重新下载文件。"
#define defIDS_NEED_REBOOT              "为了更新正在使用的文件,请重新启动电脑。"
#define defIDS_DOWNLOAD_FILE_FAILED     "下载文件 %s 到 %s 失败"
#define defIDS_DOWNLOAD_FILE_SUCCESSFUL "下载文件 %s 到 %s 成功"
#define defIDS_INVALID_USERCHECK        "非常抱歉，序列号没有通过服务器验证，可能您的序列号不完整或因为网络传输原因出错，请稍后再升级！"
#define defIDS_PIRATIC_SERIAL_NUMBER    "非常抱歉，您的序列号可能正在被其他用户非法使用因此无法通过升级验证。\r\n如果您的产品的确为正版产品，我们将为您提供新序列号！\r\n电话服务：010-86243222\r\n网站服务：http://www.duba.net/user/invalid.html\r\nEmail:antivirus@kingsoft.net"
#define defIDS_INHIBITIVE_SERIAL_NUMBER "非常抱歉，您的序列号属于已经被停用，如果您已经购买了我们的产品请您与我们联系获取新序列号。\r\n电话服务：010-86243222\r\n网站服务：http://www.duba.net/user/invalid.html\r\nEmail：antivirus@kingsoft.net"
#define defIDS_CONNECT_SERVER_FAILED    "非常抱歉，由于服务器忙或者正在更新，请稍后再升级！"
#define defIDS_CHECK_SERIAL_NUMBER_ERROR    "非常抱歉，由于网络繁忙，服务器无法连接，请稍后再升级！"
#define defIDS_HTTP_CHECK_FAILED        "用HTTP验证用户身份失败！"
#define defIDS_USE_HTTP_CHECK           "用UDP验证失败，使用HTTP验证。"
#define defIDS_HTTP_CHECK_SUCCESSFUL    "用HTTP验证用户身份成功！"
#define defIDS_UDP_CHECK_SUCCESSFUL     "用UDP验证用户身份成功！"
#define defIDS_INDEX_ERRORANALYSIS      "分析下载的升级信息文件时出现错误, 请选择其他的升级服务器重新升级. "
#endif


#endif  //_UPDATE_SOURCE_DEFINE_H_