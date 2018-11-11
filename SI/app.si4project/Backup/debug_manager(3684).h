#ifndef __DEBUG_MANAGER_H__
#define __DEBUG_MANAGER_H__

typedef struct DebugOpr {
    char *name;
    int canUsed;
    int (*DebugOprInit)(void);
    int (*DebugOprExit)(void);
    int (*Debug_PRINT)(char* dbg_buf);
    struct DebugOpr *ptNext; 
} T_DebugOpr,*PT_DebugOpr;

int InitDebugChanel(void);
int DebugPrint(const char *pcFormat, ...);
void Set_Dbg_Level(int lel);
void Set_Dbg_Channel(char * dbg_cnl);
int StdoutInit(void);
int NetPrintInit(void);
int RegisterDebugOpr(PT_DebugOpr ptDebugOpr);

#endif  // #ifndef __DEBUG_MANAGER_H__
