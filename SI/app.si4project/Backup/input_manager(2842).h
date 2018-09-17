#include <sys/time.h>

#ifndef __INPUT_MANAGER_H__
#define __INPUT_MANAGER_H__

typedef struct InputEvent {
    struct timeval tTime;
    int iType;              /*stdint,touchsceen*/
    int iVal;
}T_InputEvent,*PT_InputEvent;

typedef struct InputOpr {
    char *name;
    int (*DeviceInit)(void);
    int (*DeviceExit)(void);
    int (*GetInputEvent)(PT_InputEvent ptInputEvent);
} T_InputOpr,*PT_InputOpr;

#endif      //__INPUT_MANAGER_H__
