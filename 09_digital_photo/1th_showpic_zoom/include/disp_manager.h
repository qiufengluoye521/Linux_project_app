#ifndef _DISP_MANAGER_H
#define _DISP_MANAGER_H

#include <pic_operation.h>

typedef enum {
    VMS_FREE = 0,
    VMS_USED_FOR_PREPARE,
    VMS_USED_FOR_CUR,
}E_VideoMemState;

typedef enum {
    PS_BLANK = 0,
    PS_GENERATING,
    PS_GENERATED,
}E_PicState;

typedef struct VideoMem {
    int iID;
    int bDevFrameBuffer;
    E_VideoMemState eVideoMemState;
    E_PicState ePicState;
    T_PixelDatas tPixelDatas;
    struct VideoMem *ptNext; 
}T_VideoMem,*PT_VideoMem;

typedef struct DispOpr {
	char *name;
	int iXres;
	int iYres;
	int iBpp;
	unsigned char *pucDispMem;
	int (*DeviceInit)(void);
	int (*ShowPixel)(int iPenX, int iPenY, unsigned int dwColor);
	int (*CleanScreen)(unsigned int dwBackColor);
	struct DispOpr *ptNext;
}T_DispOpr, *PT_DispOpr;

int RegisterDispOpr(PT_DispOpr ptDispOpr);
void ShowDispOpr(void);
int DisplayInit(void);
void SelectAndInitDefaultDispDev(char *name);
int GetDispResolution(int *piXres, int *piYres,int *iBPP);
int ID(char *strName);

int FBInit(void);

#endif /* _DISP_MANAGER_H */

