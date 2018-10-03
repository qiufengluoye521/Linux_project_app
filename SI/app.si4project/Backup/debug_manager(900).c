#include "debug_manager.h"

static PT_DebugOpr g_ptDebugOprHead;

int RegisterDebugOpr(PT_DebugOpr ptDebugOpr)
{
    PT_DebugOpr ptDebugOprTmp;
    
    ptDebugOprTmp = g_ptDebugOprHead;

    if(!g_ptDebugOprHead) {
        g_ptDebugOprHead    = ptDebugOpr;
        ptDebugOpr->ptNext  = NULL;
    } else {
        while(ptDebugOprTmp->ptNext) {
            ptDebugOprTmp = ptDebugOprTmp->ptNext;
        }
        ptDebugOprTmp->ptNext   = ptDebugOpr;
        ptDebugOpr->ptNext      = NULL;
    }

    return 0;
}

int DebugPrint(const char *pcFormat, ...)
{
    char strTmpBuf[1000];
    char *pcTmp;
    va_list tArg;
    int iNum;
    PT_DebugOpr ptTmp = g_ptDebugOprHead;
    int dbglevel = DEFAULT_DBGLEVEL;
    
    va_start (tArg, pcFormat);
    
    /*�ȴ�ӡ��strTmpBuf����*/
    iNum = vsprintf (strTmpBuf, pcFormat, tArg);
    va_end (tArg);
    strTmpBuf[iNum] = '\0';
    
    pcTmp = strTmpBuf;
    
    /* ���ݴ�ӡ��������Ƿ��ӡ */
    if(strTmpBuf[0] == '<' && strTmpBuf[2] == '>')
    {
        dbglevel = strTmpBuf[1] - '0';
        if(dbglevel >= 0 && dbglevel <= 9)
        {
            pcTmp = strTmpBuf + 3;
        }
        else 
        {
            dbglevel = DEFAULT_DBGLEVEL;
        }
    }
    else 
    {
        pcTmp = strTmpBuf;
    }
    
    /* >��ӡ�����򲻴�ӡ��<��ӡ�������ӡ */
    if (dbglevel > g_iDbgLevelLimit)
	{
		return -1;
	}
    
    while(ptTmp)
    {
        if(ptTmp->canUsed)
        {
            ptTmp->StdOutDebugPrint(pcTmp);
        }
        ptTmp = ptTmp->ptNext;
    }
    
    return 0;
}

int DebugInit(void)
{
    int iError = 0;
    
    iError = StdoutInit();
    
    return iError;
}
