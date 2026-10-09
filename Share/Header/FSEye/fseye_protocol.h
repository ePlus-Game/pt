#ifndef _FSEYE_PROTOCOL_H
#define _FSEYE_PROTOCOL_H
#pragma pack(push, 1)
struct g2e_ping {
	unsigned short Protocol;
};

struct e2g_ping {
	unsigned short Protocol;
};

struct e2g_openfile {
	unsigned short Protocol;
	unsigned char bFlag;
	unsigned char bFullPath;
	char szFileName[256];
};

struct g2e_openfile {
	unsigned short Protocol;
	unsigned int nFileLen;
	int nRetCode;
};

struct e2g_readfile {
	unsigned short Protocol;
	unsigned short nDataLen;
};

struct g2e_readfile {
	unsigned short Protocol;
	unsigned int nReadLen;
	unsigned char szBuf[4096];
	int nRetCode;
};

struct e2g_writefile {
	unsigned short Protocol;
	unsigned short nDataLen;
	unsigned char szBuf[4096];
};

struct g2e_writefile {
	unsigned short Protocol;
	unsigned int nWritedLen;
	int nRetCode;
};

struct e2g_seekfile {
	unsigned short Protocol;
	unsigned char bKeep;
	unsigned int nOffset;
};

struct g2e_seekfile {
	unsigned short Protocol;
	int nRetCode;
};

struct e2g_closefile {
	unsigned short Protocol;
};

struct g2e_closefile {
	unsigned short Protocol;
	int nRetCode;
};

struct e2g_loadplug {
	unsigned short Protocol;
};

struct g2e_loadplug {
	unsigned short Protocol;
	int nRetCode;
};

struct e2g_unloadplug {
	unsigned short Protocol;
};

struct g2e_unloadplug {
	unsigned short Protocol;
	int nRetCode;
};

struct e2g_getcpubaseinfo {
	unsigned short Protocol;
};

struct cpubaseinfo {
	char szCPUName[100];
	char szCPUVendor[100];
	char szCPUDesc[100];
	int nCPULoad;
};

struct g2e_getcpubaseinfo {
	unsigned short Protocol;
	unsigned short nCPUCount;
	cpubaseinfo Processor[10];
};

struct e2g_getcpuload {
	unsigned short Protocol;
};

struct g2e_getcpuload {
	unsigned short Protocol;
	unsigned short nCPUCount;
	unsigned short nLoad[10];
};

struct e2g_getmeminfo {
	unsigned short Protocol;
};

struct g2e_getmeminfo {
	unsigned short Protocol;
	unsigned int nTotalMem;
	unsigned int nFreeMem;
};

struct e2g_getdiskinfo {
	unsigned short Protocol;
};

struct diskbaseinfo {
	char szDiskDesc[100];
	unsigned int nTotalSize;
	unsigned int nFreeSize;
};

struct g2e_getdiskinfo {
	unsigned short Protocol;
	unsigned short nDiskCount;
	diskbaseinfo Disk[10];
};

struct e2g_getnetbaseinfo {
	unsigned short Protocol;
};

struct netcardinfo {
	char szCardDesc[100];
	char szIP[20];
	char szMask[20];
	char szMac[20];
};

struct g2e_getnetinfo {
	unsigned short Protocol;
	unsigned short nCardCount;
	netcardinfo Card[10];
	char szSystemName[100];
	char szHostName[100];
};

struct e2g_getcardload {
	unsigned short Protocol;
};

struct netcardload {
	unsigned int nTXSize;
	unsigned int nRXSize;
	unsigned int nTXRate;
	unsigned int nRXRate;
};

struct g2e_getcardload {
	unsigned short Protocol;
	unsigned short nCardCount;
	netcardload Card[10];
};

struct e2g_getprocinfo {
	unsigned short Protocol;
};

struct procinfo {
	char szProcName[50];
	unsigned int nPID;
	unsigned int nMemUse;
	unsigned int nVMSize;
	unsigned int nCPUTime;
	unsigned int nThreadCount;
};

struct g2e_getprocinfo {
	unsigned short Protocol;
	unsigned short nProcCount;
	procinfo Proc[80];
};

struct pluginfo {
	char szPath[50];
	char szModHAndModE[50];
	char szGUID[50];
	char szAuthor[50];
	char szVersion[50];
	char szDesc[100];
};

struct e2g_getpluginfo {
	unsigned short Protocol;
};

struct g2e_getpluginfo {
	unsigned short Protocol;
	int PlugCount;
	pluginfo PlugInfo[5];
};

struct e2g_exesql {
	unsigned short Protocol;
	unsigned int nSessionID;
	char szDBName[48];
	unsigned char szSQL[4096];
	unsigned int nLen;
};

struct g2e_exesql {
	unsigned short Protocol;
	unsigned int nSessionID;
	unsigned int nRetCode;
	unsigned char szResult[4096];
	unsigned int nLen;
};

struct e2g_exesyscmd {
	unsigned short Protocol;
	char Command[1024];
	char InputBuff[256];
};

struct g2e_exesyscmd {
	unsigned short Protocol;
	int ReturnCode;
	char OutputBuff[256];
};

struct e2l_header {
	unsigned short Protocol;
};

struct l2e_header {
	unsigned short Protocol;
};

struct e2l_SayToWorld {
	e2l_header Header;
	unsigned short Protocol;
	char Message[256];
};

struct e2l_GetBasicInfo {
	e2l_header Header;
	unsigned short Protocol;
};

struct l2e_GetBasicInfo {
	e2l_header Header;
	unsigned short Protocol;
	unsigned short PlayerCount;
	unsigned short UpTime;
};

struct e2l_ExeGMCmd {
	e2l_header Header;
	unsigned short Protocol;
	char PlayerName[32];
	char Command[1024];
};

struct l2e_ExeGMCmd {
	e2l_header Header;
	unsigned short Protocol;
	int ReturnCode;
};

struct e2g_Authentication {
	unsigned short Protocol;
	int X;
};

struct g2e_Authentication {
	unsigned short Protocol;
	char Y[64];
};

struct e2l_PlayerCount {
	e2l_header Header;
	unsigned short Protocol;
};

struct l2e_PlayerCount {
	l2e_header Header;
	unsigned short Protocol;
	unsigned short PlayerCount;
};

struct e2g_GetGuardDir {
	unsigned short Protocol;
};

struct g2e_GetGuardDir {
	unsigned short Protocol;
	char GuardDir[256];
};

struct e2g_UpdateGuard {
	unsigned short Protocol;
};

struct e2l_Who {
	e2l_header Header;
	unsigned short Protocol;
	unsigned short Offset;
};

struct l2e_Who_PlayerInfo {
	char Name[32];
};

struct l2e_Who {
	l2e_header Header;
	unsigned short Protocol;
	unsigned short PlayerCount;
	l2e_Who_PlayerInfo PlayerList[20];
};

struct e2l_GetGlobalVariable {
	e2l_header Header;
	unsigned short Protocol;
	unsigned short VariableIndex;
};

struct l2e_GetGlobalVariable {
	l2e_header Header;
	unsigned short Protocol;
	unsigned short VariableIndex;
	int VariableValue;
};

struct e2l_SetGlobalVariable {
	e2l_header Header;
	unsigned short Protocol;
	unsigned short VariableIndex;
	int VariableValue;
};

struct e2g_Key {
	unsigned char Data[128];
};

struct e2g_ConfigInfo {
	int X;
	char Y[64];
	unsigned short ConfigFileDataLength;
	char ConfigFileData[4096];
};

struct e2g_config {
	unsigned short Protocol;
	e2g_Key Key;
	unsigned char ConfigInfo[4166];
};

struct g2e_config {
	unsigned short Protocol;
};

struct e2g_switchmode {
	unsigned short Protocol;
	unsigned short Mode;
};

struct g2e_switchmode {
	unsigned short Protocol;
};

struct e2l_GetGameStartTime {
	e2l_header Header;
	unsigned short Protocol;
};

struct l2e_GetGameStartTime {
	l2e_header Header;
	unsigned short Protocol;
	char GameStartTime[32];
};

struct l2e_ReportError {
	l2e_header Header;
	unsigned short Protocol;
	int Module;
	int ErrorCode;
};

struct e2g_DeliverKey {
	unsigned short Protocol;
	e2g_Key Key;
};

struct e2g_exesyscmd_large {
	unsigned short Protocol;
	char Command[6144];
	char InputBuff[256];
};

struct l2e_info {
	l2e_header Header;
	unsigned short Protocol;
	char Info[1024];
};

struct l2e_info_large {
	l2e_header Header;
	unsigned short Protocol;
	char InfoLarge[4096];
};

enum ProtocolDef {
	g2e_ping_def = 0,
	e2g_ping_def,
	e2g_openfile_def,
	g2e_openfile_def,
	e2g_readfile_def,
	g2e_readfile_def,
	e2g_writefile_def,
	g2e_writefile_def,
	e2g_seekfile_def,
	g2e_seekfile_def,
	e2g_closefile_def,
	g2e_closefile_def,
	e2g_loadplug_def,
	g2e_loadplug_def,
	e2g_unloadplug_def,
	g2e_unloadplug_def,
	e2g_getcpubaseinfo_def,
	g2e_getcpubaseinfo_def,
	e2g_getcpuload_def,
	g2e_getcpuload_def,
	e2g_getmeminfo_def,
	g2e_getmeminfo_def,
	e2g_getdiskinfo_def,
	g2e_getdiskinfo_def,
	e2g_getnetbaseinfo_def,
	g2e_getnetinfo_def,
	e2g_getcardload_def,
	g2e_getcardload_def,
	e2g_getprocinfo_def,
	g2e_getprocinfo_def,
	e2g_getpluginfo_def,
	g2e_getpluginfo_def,
	e2g_exesql_def,
	g2e_exesql_def,
	e2g_exesyscmd_def,
	g2e_exesyscmd_def,
	e2l_header_def,
	l2e_header_def,
	e2l_SayToWorld_def,
	e2l_GetBasicInfo_def,
	l2e_GetBasicInfo_def,
	e2l_ExeGMCmd_def,
	l2e_ExeGMCmd_def,
	e2g_Authentication_def,
	g2e_Authentication_def,
	e2l_PlayerCount_def,
	l2e_PlayerCount_def,
	e2g_GetGuardDir_def,
	g2e_GetGuardDir_def,
	e2g_UpdateGuard_def,
	e2l_Who_def,
	l2e_Who_def,
	e2l_GetGlobalVariable_def,
	l2e_GetGlobalVariable_def,
	e2l_SetGlobalVariable_def,
	e2g_config_def,
	g2e_config_def,
	e2g_switchmode_def,
	g2e_switchmode_def,
	e2l_GetGameStartTime_def,
	l2e_GetGameStartTime_def,
	l2e_ReportError_def,
	e2g_DeliverKey_def,
	e2g_exesyscmd_large_def,
	l2e_info_def,
	l2e_info_large_def,

	fseye_protocol_count
};

enum FSEyeResult {
	fseye_success = 0,
	guard_err,
	guard_create_client_err,
	guard_startup_client_err,
	guard_client_invalidhadle,
	guard_client_send_err,
	plug_opendll_err,
	plug_getproc_err,
	plug_creat_err,
	filetran_opening_err,
	filetran_app_err,
	filetran_cre_err,
	filetran_seek_err,
	filetran_close_err,
	mydb_err_opendb,
	mydb_err_query,
	mydb_err_dbuncon,
	servicestate_stopped,
	servicestate_starting,
	servicestate_running,
	servicestate_stopping,
	servicestate_restarting,
	db_err,
	db_transaction_started_err,
	db_transaction_not_started_err,
	db_rebuild_table_err,
	db_delete_table_err,
	db_table_not_exist_err,
	db_table_exist_err,
	db_begin_transaction_err,
	db_commit_transaction_err,
	db_rollback_transaction_err,
	db_get_table_data_err,
	db_adapter_not_init_err,
	db_add_table_data_err,
	db_update_table_data_err,
	as_err,
	as_bad_argument_err,
	as_user_not_exist_err,
	as_user_already_login_err,
	as_fm_task_complete,
	sec_err,
	sec_allow,
	sec_deny,
	sec_unknown,
	sec_not_found_in_cache,
	sec_not_enough_privilege_err,
	sec_ace_already_exist_err,
	sec_ace_not_exist_err,
	sec_user_already_login_err,
	sec_user_not_exist_err,
	sec_user_not_login_err,
	l2e_ExeGMCmd_err,
	l2e_ExeGMCmd_player_not_found_err,
	g2e_ExeSysCmd_done,
	g2e_ExeSysCmd_busy,
	g2e_ExeSysCmd_result,
	mydb_more_result,
};

#pragma pack(pop)
#endif
