/********************************************************************
	created:	2004/08/03
	created:	3:8:2004   12:00
	filename: 	KConsole.h
	file path:	
	file base:	KConsole
	file ext:	h
	author:		
	
	purpose:	控制台类，负责注册，管理，执行控制台命令
*********************************************************************/

#define MAX_RETURNBUFFER_LEN	512

struct ConsoleCmd
{
//	char	CmdName[MAX_NAME_AND_CMD_LENGTH];
};

class KConsole
{
public:
	bool			registerCommand();
	int				getCommandNum();
	const char*		execute(char* buf);

private:
	char			m_ReturnBuf[MAX_RETURNBUFFER_LEN];

};

extern KConsole	gConsole;