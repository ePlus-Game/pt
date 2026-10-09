//xiehong 2008-3-14 自动寻路结束后自动打开NPC

#ifndef _AutoDialogNpc_h
#define _AutoDialogNpc_h
#ifdef _AUTO_ROBOT

#define AUTO_DIALOG_NPC_INVALID_DIALOG_NPC_ID -1

class AutoDialogNpc
{
	//自动对话的Npc模板id
	int _dialogNpcTemplateId;
 public:
	 AutoDialogNpc();
	 ~AutoDialogNpc();
	 
	 
	 static AutoDialogNpc& getSingleton();
	 
	 void openDialogNpc();
	 void setTargetNpc(int templateId){	_dialogNpcTemplateId = templateId;	};
};

#endif	// #ifdef _AUTO_ROBOT
#endif	// #ifndef _AutoDialogNpc_h