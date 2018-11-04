#ifndef __INPUT_MANAGER_H__
#define __INPUT_MANAGER_H__

#include <sys/time.h>
#include <pthread.h>


#define INPUT_TYPE_STDIN            0
#define INPUT_TYPE_TOUCHSCREEN      1

#define INPUT_VALUE_UP              0
#define INPUT_VALUE_DOWN            1
#define INPUT_VALUE_EXIT            2
#define INPUT_VALUE_UNKNOWN         -1


typedef struct InputEvent {
    struct timeval tTime;
    int iType;              /*stdint,touchsceen*/
    int iVal;
}T_InputEvent,*PT_InputEvent;

typedef struct InputOpr {
    char *name;
    pthread_t tid;
    int (*DeviceInit)(void);
    int (*DeviceExit)(void);
    int (*GetInputEvent)(PT_InputEvent ptInputEvent);
    struct InputOpr *ptNext; 
} T_InputOpr,*PT_InputOpr;

int RegesiterInputOpr(PT_InputOpr ptInputOpr);
void ShowInputOpr(void);
int AllInputDevicesInit(void);
int GetInputEvent(PT_InputEvent ptInputEvent);
int InputInit(void);


#endif      //__INPUT_MANAGER_H__
