//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-1-30
//      File_base        : npc_save
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : NPC存盘
//
//////////////////////////////////////////////////////////////////////

#ifndef _NPC_SAVE_H_
#define _NPC_SAVE_H_

//NPC存盘数据库操作
enum enumNpcSaveDbOperation
{
	npc_save_db_op_none = -1,

	npc_save_db_op_load,
	npc_save_db_op_save,
	npc_save_db_op_delete,

	npc_save_db_op_count,
};

//NPC存盘
class NpcSave
{
public:
	static bool IsBusy();//是否正忙（工作队列满了）

	static bool LoadNpc(int npcIndex);//载入NPC
	static bool SaveNpc(int npcIndex);//保存NPC
	static bool DeleteNpc(int npcIndex);//删除NPC存盘数据
	static void DbOpComplete( int dbOpResult, IProcRet* pRet );//数据库操作完成

	static bool LoadGlobalNpcInfo();
	static void LoadGlobalNpcInfoRet(int nDBOpeRst, int nDataSize, char *pData);
	static void SaveAllNpc();//保存所有的存盘NPC

private:
	static void AddTask();//添加工作
	static void RemoveTask();//减少工作

	static void LoadNpcComplete(int dbOpResult, char* pPassBy, IProcRet* pRet );//载入NPC存盘数据完成
	static void SaveNpcComplete(int dbOpResult, char* pPassBy, IProcRet* pRet );//保存NPC存盘数据完成
	static void DeleteNpcComplete(int dbOpResult, char* pPassBy, IProcRet* pRet );//删除NPC存盘数据完成


	static bool SaveNpcGlobalInfo(int npcIndex);
	static bool DeleteNpcGlobalInfo(int npcIndex);

	static int GetEmptyNpcItemPos();
	static int GetNpcItemPos(int npcIndex);
	static bool IsHaveSaved(int npcIndex);

	static int s_ProcessingNpcCount;

private:
	struct GlobalNpcSaveItem
	{
		int	npcIndexInfo;
		int subworldId;
		int xPos;
		int yPos;
	};

	struct DBGlobalNpcSaveInfo
	{
		int count;
		GlobalNpcSaveItem item[1];
	};

	struct MemGlobalNpcSaveInfo
	{
		bool isValid;
		GlobalNpcSaveItem saveItem;

		MemGlobalNpcSaveInfo()
		{
			isValid = false;
		}
	};

	enum { __max_globalnpcitem_num = 64, };

	// 注意: 这个全局存盘是为了解决如下问题:
	//       当通过buff加出一个存盘npc后，此时服务器如果当机，则现有的存盘机制无法
	//		 在服务器重起后把该npc加载进来
	// 
	// 有如下限制:
	//     1. 此npc一个地图上只能出现1个
	//     2. 此npc不能从一个地图跑到另一个地图
	//     3. 只有npc被加出来或者删除的时候才会去更新数据库，npc状态改变的时候
	//        不会更新数据库
	static MemGlobalNpcSaveInfo m_GlobalNpcItem[__max_globalnpcitem_num];

	static bool m_IsGlobalNpcInfoLoaded;
};

#endif// _NPC_SAVE_H_