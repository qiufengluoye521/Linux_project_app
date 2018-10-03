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

#endif  // #ifndef __DEBUG_MANAGER_H__