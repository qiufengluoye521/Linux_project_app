#include "input_manager.h"
#include <stddef.h>
#include <config.h>

static PT_InputOpr ptInputOprHead;
static T_InputEvent gtInputEvent;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condvar = PTHREAD_COND_INITIALIZER;


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
		DebugPrint("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}

void *threadfunc(void *pvoid)
{
    T_InputEvent tInputEventTmp;
    /* 定义函数指针 */
    int (*PGetInputEvent)(PT_InputEvent ptInputEvent);
    PGetInputEvent = (int (*)(PT_InputEvent))pvoid;
    
    while(1)
    {
        if(0 == PGetInputEvent(&tInputEventTmp)) {
            DebugPrint("get input event\n");
            /* 读到输入状态值，唤醒主线程 */
            /* 访问临界资源前，先获得互斥量 */
            pthread_mutex_lock(&mutex);
            gtInputEvent = tInputEventTmp;
            /* 唤醒主线程 */
            pthread_cond_signal(&condvar);
            /* 释放互斥量 */
            pthread_mutex_unlock(&mutex);
            DebugPrint("pthread_mutex_unlock\n");
        } 
    }
        
}


int AllInputDevicesInit(void)
{
    int iError  = 0;
    PT_InputOpr ptInputOprTmp;
    
    ptInputOprTmp   = ptInputOprHead;
    while(ptInputOprTmp) {
        iError += ptInputOprTmp->DeviceInit();
        pthread_create(&ptInputOprTmp->tid,NULL,&threadfunc,ptInputOprTmp->GetInputEvent);
        ptInputOprTmp = ptInputOprTmp->ptNext;
    }

    return iError;
}

int GetInputEvent(PT_InputEvent ptInputEvent)
{
    DebugPrint("enter GetInputEvent fun\r\n");
    /* 访问临界资源前，先获得互斥量 */
    pthread_mutex_lock(&mutex);
    /* 休眠，等待 pthread_cond_signal(&g_tConVar)函数来唤醒 */
    DebugPrint("pthread_cond_wait\r\n");
    pthread_cond_wait(&condvar, &mutex);

    *ptInputEvent = gtInputEvent;
    DebugPrint("ptInputEvent = gtInputEvent will return\r\n");
    pthread_mutex_unlock(&mutex);
    
    return 0;
}

int InputInit(void)
{
    int iError = 0;

    iError |= StdinInit();
    iError |= TouchScreenInit();

    return iError;
}



