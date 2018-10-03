#include "debug_manager.h"

static int StdOutDebugPrint(char* dbg_buf)
{
    printf("%s",dbg_buf);
    
    return strlen(dbg_buf);
    
}

static T_DebugOpr g_tStdoutDebugOpr = {
    .name               = "stdout",
    .canUsed            = 1,
    .Debug_PRINT        = StdOutDebugPrint,
        
};

int StdoutInit(void)
{
    return RegisterDebugOpr(&g_tStdoutDebugOpr);
}