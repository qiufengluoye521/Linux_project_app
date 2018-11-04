#ifndef __PAGE_MANAGER_H__
#define __PAGE_MANAGER_H__

#include "input_manager.h"

typedef struct Layout {
    int iTopLeftX;
    int iTopLeftY;
    int iBotRithtX;
    int iBotRightY;
    char *strIconName;
}T_Layout,*PT_Layout;

typedef struct PageAction {
    char *name;
    void (*Run)(void);
    int (*GetInputEvent)(PT_Layout atLayout, PT_InputEvent ptInputEvent);
    int (*Prepare)(void);
    struct PageAction *ptNext;
}T_PageAction,*PT_PageAction;

PT_PageAction Page(char *pcName);
int RegisterPageAction(PT_PageAction ptPageAction);
int MainPageInit(void);

#endif // __PAGE_MANAGER_H__
