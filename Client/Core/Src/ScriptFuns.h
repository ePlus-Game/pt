#ifndef SCIRPT_FUNS_H
#define SCIRPT_FUNS_H

void ScriptSetPlayerIndex(int nIdx);
void ScriptSetSubWorldIndex(int nIdx);
void ScriptSetObjIndex(int nIdx);
void ScriptSetItemIndex(int nIdx);
#ifdef _SERVER
int ExecuteScript(DWORD scriptId, char* functionName, int nParam, int worldIndex);
int ExecuteScript(char* scriptFileName, char* functionName, int nParam, int worldIndex);
#endif

#endif // SCIRPT_FUNS_H