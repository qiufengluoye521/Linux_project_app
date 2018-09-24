#include "input_manager.h"
#include <stddef.h>

static PT_InputOpr ptInputOprHead;

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

int AllInputDevicesInit(void)
{
    int iError  = 0;
    PT_InputOpr ptInputOprTmp;
    
    ptInputOprTmp   = ptInputOprHead;
    while(ptInputOprTmp) {
        iError += ptInputOprTmp->DeviceInit();
        ptInputOprTmp = ptInputOprTmp->ptNext;
    }

    return iError;
}

int GetInputEvent(PT_InputEvent ptInputEvent)
{
    PT_InputOpr ptInputOprTmp;

    ptInputOprTmp   = ptInputOprHead;
    
    while(ptInputOprTmp) {
        if(0 == ptInputOprTmp->GetInputEvent(ptInputEvent)) {
            return 0;
        } else {
            ptInputOprTmp = ptInputOprTmp->ptNext;
        }
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



