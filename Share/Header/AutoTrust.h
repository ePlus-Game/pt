#ifndef  AUTO_TRUST_DATA_H
#define AUTO_TRUST_DATA_H
/* render添加*/
/* 为自动打怪设置标志*/
#define AUTO_ATTACK_FLAG_NORNAL             0
#define AUTO_ATTACK_FLAG_ENEMY_LEVEL_HIGHER 1
#define AUTO_ATTACK_FLAG_ENEMY_LEVEL_LOWER  2
#define AUTO_ATTACK_FLAG_GO_BACK            4
#define AUTO_ATTACK_FLAG_SELL_CHEAP         8
#define AUTO_ATTACK_FLAG_RANDOM_GO          16

#define AUTO_ATTACK_HIGHER_LEVEL   5
#define AUTO_ATTACK_LOWER_LEVEL   10

/////自动拾取///////////////////
#define AUTO_PICKUP_ITEM_ALL           0
#define AUTO_PICKUP_ITEM_DEAR_SET_FIRST 1
#define AUTO_PICKUP_ITEM_DEATH          2
///////////////////////////////////////
#define AUTO_PICKUP_ITEM_NOT_WHITE            4
#define AUTO_PICKUP_ITEM_ONLY_GREEN           8      


#define AUTO_PICKUP_POS_WIDTH 300
#define AUTO_PICKUP_POS_HEIGHT 300
//////////自动喝药/////////////////////
#define AUTO_USE_HP_MEDICINE_NORMAL     0
#define AUTO_USE_HP_MEDICINE_LIFE_1     1
#define AUTO_USE_HP_MEDICINE_LIFE_2     2
#define AUTO_USE_HP_MEDICINE_LIFE_ONWER 4
#define AUTO_USE_MP_MEDICINE_1          8
#define AUTO_USE_MP_MEDICINE_2			16

//托机处理状态位///
#define ENSTRUST_NORMAL_MODE   0
#define ENSTRUST_AUTO_ATTACK_MODE 1
#define ENSTRUST_AUTO_PICK_UP_MODE 2
#define ENSTRUST_AUTO_USEITEM_MODE 4

//////////////////////////////

#define AUTO_ITEM_NAME_LENGTH     256
#define AUTO_MEDICINE_NUMBERS      8

#define AUTO_ITEM_HP_SMALL_IDX       0
#define AUTO_ITEM_HP_NORMAL_IDX      1
#define AUTO_ITEM_HP_POWER_IDX       2
#define AUTO_ITEM_MP_SMALL_IDX       3
#define AUTO_ITEM_MP_NORMAL_IDX      4
#define AUTO_ITEM_MP_POWER_IDX       5
#define AUTO_ITEM_HP_MP_SMALL_IDX    6
#define AUTO_ITEM_HP_MP_POWER_IDX    7
///////////////////////////////////////////
//////////////////自动回城/////////////////////
#define AUTO_GO_NONE            0
#define AUTO_GO_PREPAER         1
#define AUTO_GO_HOME_AT_CAN_SELL_MAP 2
#define AUTO_GO_HOME_SELLING         4
#define AUTO_GO_ENABLE               8
#define AUTO_GO_DOING                16
#define AUTO_GO_HOME                 32
#define AUTO_GO_BACK                 64
#define AUTO_GO_NET_DELAY            128
#define AUTO_INI_FILE_PATH    "settings\\entrustcpu.ini"
#define AUTO_TAB_INI_FILE_PATH "uisettings\\AutoPickupList.ini"
#define AUTO_ITEM_HP_SMALL_NAME "medicineHpsmall"
#define AUTO_ITEM_HP_NORMAL_NAME "medicineHpNormal"
#define AUTO_ITEM_HP_POWER_NAME  "medicineHpPower"
#define AUTO_ITEM_MP_SMALL_NAME "medicineMpsmall"
#define AUTO_ITEM_MP_NORMAL_NAME "medicineMpNormal"
#define AUTO_ITEM_MP_POWER_NAME   "medicineMpPower"
#define AUTO_ITEM_HP_MP_SMALL_NAME "medicineHpMpSmall"
#define AUTO_ITEM_HP_MP_POWER_NAME "medicineHpMpPower"
#endif