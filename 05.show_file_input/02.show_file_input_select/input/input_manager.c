#include "input_manager.h"
#include <stddef.h>
#include <config.h>

static PT_InputOpr ptInputOprHead;
fd_set gtRfds;
int g_iMaxFd = 0;


int RegesiterInputOpr(PT_InputOpr ptInputOpr)
{
    PT_InputOpr ptInputOprTmp;
    
    ptInputOprTmp = ptInputOprHead;

    if(!ptInputOprHead) {
        ptInputOprHead      = ptInputOpr;
        ptInputOpr->ptNext  = NULL;
    } else {
        while(ptInputOprTmp->ptNext) {
            ptInputOprTmp = ptInputOprTmp->ptNext;
        }
        ptInputOprTmp->ptNext   = ptInputOpr;
        ptInputOpr->ptNext      = NULL;
    }

    return 0;
}

void ShowInputOpr(void)
{
	int i = 0;
	PT_InputOpr ptTmp = ptInputOprHead;

	while (ptTmp)
	{
		printf("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}
int AllInputDevicesInit(void)
{
    int iError  = 0;
    PT_InputOpr ptInputOprTmp;
    

    FD_ZERO(&gtRfds);
    
    ptInputOprTmp   = ptInputOprHead;
    while(ptInputOprTmp) {
        iError += ptInputOprTmp->DeviceInit();
        DBG_PRINTF("iFd is %d\r\n",ptInputOprTmp->iFd);
        FD_SET(ptInputOprTmp->iFd, &gtRfds);
        if(g_iMaxFd < ptInputOprTmp->iFd) {
            g_iMaxFd = ptInputOprTmp->iFd;
            DBG_PRINTF("g_iMaxFd is %d\r\n",g_iMaxFd);
        }
        ptInputOprTmp = ptInputOprTmp->ptNext;
    }

    g_iMaxFd ++;
    DBG_PRINTF("g_iMaxFd after ++ is %d\r\n",g_iMaxFd);

    return iError;
}

int GetInputEvent(PT_InputEvent ptInputEvent)
{
    PT_InputOpr ptInputOprTmp;
    fd_set tRFds;
    int retval;

    tRFds = gtRfds;

//    DBG_PRINTF("enter GetInputEvent fun\r\n");
    ptInputOprTmp   = ptInputOprHead;

    retval = select(g_iMaxFd, &tRFds, NULL, NULL, NULL);

    if(retval > 0 ) {
//        DBG_PRINTF("retval is %d\r\n",retval);
        while(ptInputOprTmp) {
//            DBG_PRINTF("FD_ISSET iFd is %d\r\n",ptInputOprTmp->iFd);
            if(FD_ISSET(ptInputOprTmp->iFd, &tRFds)) {
                if(0 == ptInputOprTmp->GetInputEvent(ptInputEvent)) {
                    return 0;
                } 
            }
            
            ptInputOprTmp = ptInputOprTmp->ptNext;
            
        }
    }
    else {
        DBG_PRINTF("retval is %d\r\n",retval);
    }
    return -1;
}

int InputInit(void)
{
    int iError = 0;

    iError |= StdinInit();
    iError |= TouchScreenInit();

    return iError;
}



