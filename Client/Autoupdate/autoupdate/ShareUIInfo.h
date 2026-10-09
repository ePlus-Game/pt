#ifndef FS_SHARE_UI_INFO_H
#define FS_SHARE_UI_INFO_H
/***************************************************************************************************************
 AutoUpdateDlg Infos
 主界面的相关宏配置
 ***************************************************************************************************************/

//浏览器的位置
#define BROWSER_X 37
#define BROWSER_Y 108
#define BROWSER_CX 515
#define BROWSER_CY 340

//下方四个位图按钮的属性
#define UI_BUTTON_Y       542 //启始的Y位置
#define UI_BUTTON_START_X 188 //启始X位置
#define UI_BUTTON_WIDTH   110 //Button的长度
#define UI_BUTTON_HIGHT   28  //Button的高度
#define UI_BUTTON_CX      15  //Button之间的间距
#define UI_BUTTON_TRANS_COLOR RGB(255,0,255) //Button图片的ColorKey
//右上角的最小化和关闭小按扭
#define UI_LITTLE_BUTTON_SIZE {22, 22}     //小按扭的大小
#define UI_LITTLE_POS_MINI_POS {495 + 17, 53 + 23}   //最小化的按扭的位置 
#define UI_LITTLE_POS_CLOSE_POS {515 + 17, 53 + 23}  //关闭按扭的位置
//进度条上的提示文字
#define UI_UPDATEMANNER_TIP_RECT	{60, 460, 43 + 380, 460 + 12}
#define UI_TIP_RECT       {60, 460, 43 + 380, 460 + 12 }
#define UI_TIP_COLOR      RGB(180,173,164)
//进度条相关属性,注意进度条前后景必须等长等宽
#define CHANEL_SATART_X   52              //进度条的初始X位置
#define CHANEL_WIDTH      10                //进度槽的宽
#define CHANEL_LEN        380              //进度槽的长度
#define CHANLE_OVERAL_Y   483 + 12        //总体进度条的y位置
#define CHANLE_FILE_Y     503 + 19         //文件进度条的y位置
//当前的服务器选择
#define CURE_SEL_COLOR    RGB(0,0,0) //文字颜色
// #define CURE_SEL_RECT     {20,45,200,115}  //对应的CTransStatic 的区域RECT
#define CURE_SEL_RECT	{6, 30, 198,158}
//窗口的Caption
#define UI_CAPTION        {209, 97, 209 + 200,97 + 10}  //对应的RECT
#define UI_FONT_COLOR     RGB(255,255,255)

//背景
#define UI_BK_TANS_COLORKEY RGB(255, 0, 255)
/*************************************************************************************
CUpdateTipDlg 强制更新提示窗口相关配置
*************************************************************************************/
//#define UI_UTD_OK_POS      {160,170}
//#define UI_UTD_SIZE        {100, 50}
//#define UI_UTD_COLOR_KEY   RGB(255, 0, 255) //图片对应的Colorkey
/**************************************************************************************
GameOptionPanelDlg 游戏设置提示窗口相关配置
**************************************************************************************/
#define UI_TD_MINICLOSE_POS {377, 5}
#define UI_TD_MINICLOSE_SIZE {22, 22}
#define UI_TD_MINICLOSE_CK   RGB(0, 255, 0) //ColorKey
#define UI_TD_CAPTION        {170, 24, 170 + 20, 20 + 19} //“游戏设置”Caption的区域
#define UI_TD_FUL_CHECK_RECT {236, 42, 234 + 27, 42 + 25} //全屏按扭
#define UI_TD_WIN_CHECK_RECT {313, 42, 311 + 27, 42 + 25} //窗口按扭区域
#define UI_TD_PICTURE_TIP    {18, 215, 18 + 200, 215 + 19} //截图提示语的区域
#define UI_TD_PICTRUE_PATH   {35, 138, 35 + 80, 138 + 20}  //路径标题区域
#define UI_TD_PICTRUE_EDIT   {95, 183, 95 + 222, 183 + 16} //路径编辑控件的区域
#define UI_TD_BTN_DEFAULT_RC {30, 239, 30 + 90, 239 + 30}   //默认按扭的区域
#define UI_TD_BTN_OK         {140, 239, 140 + 85, 239 + 30} //确定按扭的区域
#define UI_TD_CANCEL         {250, 239, 250 + 85, 239 + 30} //取消按扭的区域 
#define UI_TD_MUSIC_RC       {94, 119, 94 + 230, 119 + 7}    //音乐滑动条的区域
#define UI_TD_SOUND_RC       {94, 144, 94 + 230, 144+ 7}     //音效滑动的区域
#define UI_TD_FULLS			 {58, 82, 58 + 10, 82 + 10}
#define UI_TD_WINDOWS		 {58, 58, 58 + 10, 58 + 10}
#define UI_TD_800            {233, 59, 233 + 10, 59 + 10}
#define UI_TD_1024			 {233, 83, 233 + 10, 83 + 10}

//EXVERSION
#define EX_SER_NORMAL {107, 290}
#define EX_SER_EXMODE {223+UI_BUTTON_WIDTH, 290}
#define EX_SER_CLOSE  {473, 157}

//Tip:Bitmap对话框的大小可以随图片大小变化而自动变化，形状可以通过ColorKey来改变
//    所有图片注意保留其ColorKey
#endif
