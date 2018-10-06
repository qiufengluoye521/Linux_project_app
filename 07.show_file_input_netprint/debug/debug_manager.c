#include "debug_manager.h"
#include <string.h>
#include <stdarg.h>
#include "config.h"

#define DEFAULT_DBGLEVEL 4

static int g_iDbgLevelLimit = 8;


static PT_DebugOpr g_ptDebugOprHead;

static PT_DebugOpr find_Opr_by_name(char * chrName)
{
    PT_DebugOpr ptTmp = g_ptDebugOprHead;
    
    while(ptTmp)
    {
        if(strcmp(ptTmp->name,chrName) == 0)
        {
            return ptTmp;
        }
    }
    
    return NULL;
}

void Set_Dbg_Level(int lel)
{    
    g_iDbgLevelLimit = lel;
}

void Set_Dbg_Channel(char * dbg_cnl)
{
    char chr_Name[100];
    char * equal_num = 0;
    PT_DebugOpr ptDebugOprTmp;
    
    equal_num               = strchr(dbg_cnl,'=');
    strcpy(chr_Name,dbg_cnl);
    chr_Name[equal_num - dbg_cnl]     = '\0';
    ptDebugOprTmp           = find_Opr_by_name(chr_Name);
    
    ptDebugOprTmp->canUsed  = dbg_cnl[equal_num - dbg_cnl + 1] - '0';

}


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
    
    /*先打印到strTmpBuf数组*/
    iNum = vsprintf (strTmpBuf, pcFormat, tArg);
    va_end (tArg);
    strTmpBuf[iNum] = '\0';
    
    pcTmp = strTmpBuf;
    
    /* 根据打印级别决定是否打印 */
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
    
    /* >打印级别则不打印，<打印级别，则打印 */
    if (dbglevel > g_iDbgLevelLimit)
	{
		return -1;
	}
    
    while(ptTmp)
    {
        if(ptTmp->canUsed)
        {
            ptTmp->Debug_PRINT(pcTmp);
        }
        ptTmp = ptTmp->ptNext;
    }
    
    return 0;
}


int InitDebugChanel(void)
{
    PT_DebugOpr ptTmp = g_ptDebugOprHead;

    while(ptTmp)
    {
        if(ptTmp->DebugOprInit)
        {
            ptTmp->DebugOprInit();
        }
        ptTmp = ptTmp->ptNext;
    }

    return 0;
    
}

int DebugInit(void)
{
    int iError = 0;
    
    iError += StdoutInit();
    iError += NetPrintInit();
    
    return iError;
}
