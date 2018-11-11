#include <config.h>
#include <page_manager.h>


static PT_PageAction g_ptPageActionHead;

PT_PageAction Page(char *pcName)
{
    PT_PageAction ptTmp = g_ptPageActionHead;
    
    while(ptTmp)
    {
        if(strcmp(ptTmp->name,pcName) == 0)
        {
            return ptTmp;
        }
        ptTmp = ptTmp->ptNext;
    }
    
    return NULL;
}

int RegisterPageAction(PT_PageAction ptPageAction)
{
    PT_PageAction ptTmp;

    if (!g_ptPageActionHead)
    {
        g_ptPageActionHead   = ptPageAction;
        ptPageAction->ptNext = NULL;
    }
    else
    {
        ptTmp = g_ptPageActionHead;
        while (ptTmp->ptNext)
        {
            ptTmp = ptTmp->ptNext;
        }
        ptTmp->ptNext           = ptPageAction;
        ptPageAction->ptNext   = NULL;
    }

    return 0;
}


int PagesInit(void)
{
    int iError;

    iError = MainPageInit();
    if (iError)
    {
        DBG_PRINTF("MainPageInit error!\n");
        return -1;
    }

    return 0;
}

