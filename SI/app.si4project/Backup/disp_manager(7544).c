
#include <config.h>
#include <disp_manager.h>
#include <string.h>

static PT_DispOpr g_ptDispOprHead;
static PT_DispOpr g_ptDefaultDispOpr;
static PT_VideoMem g_ptVideoMemHead;


int RegisterDispOpr(PT_DispOpr ptDispOpr)
{
	PT_DispOpr ptTmp;

	if (!g_ptDispOprHead)
	{
		g_ptDispOprHead   = ptDispOpr;
		ptDispOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptDispOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptDispOpr;
		ptDispOpr->ptNext = NULL;
	}

	return 0;
}


void ShowDispOpr(void)
{
	int i = 0;
	PT_DispOpr ptTmp = g_ptDispOprHead;

	while (ptTmp)
	{
		DBG_PRINTF("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}

PT_DispOpr GetDispOpr(char *pcName)
{
	PT_DispOpr ptTmp = g_ptDispOprHead;
	
	while (ptTmp)
	{
		if (strcmp(ptTmp->name, pcName) == 0)
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	return NULL;
}

void SelectAndInitDefaultDispDev(char *name)
{
	g_ptDefaultDispOpr = GetDispOpr(name);
	if (g_ptDefaultDispOpr)
	{
		g_ptDefaultDispOpr->DeviceInit();
		g_ptDefaultDispOpr->CleanScreen(0);
	}
}

PT_DispOpr GetDefaultDispDev(void)
{
	return g_ptDefaultDispOpr;
}
int GetDispResolution(int *piXres, int *piYres,int *piBPP)
{
	if (g_ptDefaultDispOpr)
	{
		*piXres = g_ptDefaultDispOpr->iXres;
		*piYres = g_ptDefaultDispOpr->iYres;
        *piBPP  = g_ptDefaultDispOpr->iBpp;
		return 0;
	}
	else
	{
		return -1;
	}
}

/*  */
int AllocVideoMem(int iNum)
{
    int i;
    int iXres   = 0;
    int iYres   = 0;
    int iBpp    = 0;
    int iVMSize = 0;
    
    PT_VideoMem pt_new;
    
    GetDispResolution(&iXres, &iYres, &iBpp);
    
    pt_new = malloc(sizeof(T_VideoMem));
    if(pt_new == NULL)
    {
        return -1;
    }
    DBG_PRINTF("pt_new add:%x\n",pt_new);
    
    iVMSize = iXres * iYres * iBpp / 8;
    
    pt_new->iID                         = 0;
    pt_new->bDevFrameBuffer             = 1;
    pt_new->eVideoMemState              = VMS_FREE;
    pt_new->ePicState                   = PS_BLANK;
    pt_new->tPixelDatas.iWidth          = iXres;
    pt_new->tPixelDatas.iHeight         = iYres;
    pt_new->tPixelDatas.iBpp            = iBpp;
    pt_new->tPixelDatas.iLineBytes      = iXres * iBpp / 8;
    pt_new->tPixelDatas.iTotalBytes     = iXres * iYres * iBpp / 8;
    pt_new->tPixelDatas.aucPixelDatas   = g_ptDefaultDispOpr->pucDispMem;
    
    if(iNum != 0)
    {
        pt_new->eVideoMemState  = VMS_USED_FOR_CUR;
    }
    
    /*放入链表*/
    pt_new->ptNext      = g_ptVideoMemHead;
    g_ptVideoMemHead    = pt_new;
    
    for(i=0;i<iNum;i++)
    {
        pt_new = malloc(sizeof(T_VideoMem) + iVMSize);
        if(pt_new == NULL)
        {
            return -1;
        }
        DBG_PRINTF("pt_new%d add:%x\n",i,pt_new);
        
        pt_new->tPixelDatas.aucPixelDatas   = (unsigned char *)(pt_new + 1);
        pt_new->iID                         = 0;
        pt_new->bDevFrameBuffer             = 1;
        pt_new->eVideoMemState              = VMS_FREE;
        pt_new->ePicState                   = PS_BLANK;
        pt_new->tPixelDatas.iWidth          = iXres;
        pt_new->tPixelDatas.iHeight         = iYres;
        pt_new->tPixelDatas.iBpp            = iBpp;
        pt_new->tPixelDatas.iLineBytes      = iXres * iBpp / 8;
        pt_new->tPixelDatas.iTotalBytes     = iXres * iYres * iBpp / 8;
        
        /*放入链表*/
        pt_new->ptNext      = g_ptVideoMemHead;
        g_ptVideoMemHead    = pt_new; 
    }
    
    return 0;    
}

PT_VideoMem GetVideoMem(int iID, int bCur)
{
    PT_VideoMem ptTmp = g_ptVideoMemHead;

    /* 1. 优先: 取出空闲的、ID相同的videomem */
    while (ptTmp)
    {
        if ((ptTmp->eVideoMemState == VMS_FREE) && (ptTmp->iID == iID))
        {
            ptTmp->eVideoMemState = bCur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
            return ptTmp;
        }
        ptTmp = ptTmp->ptNext;
    }

    /* 2. 优先: 取出任意一个空闲videomem */
    ptTmp = g_ptVideoMemHead;
    while (ptTmp)
    {
        if (ptTmp->eVideoMemState == VMS_FREE)
        {
            ptTmp->iID = iID;
            ptTmp->ePicState = PS_BLANK;
            ptTmp->eVideoMemState = bCur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
            return ptTmp;
        }
        ptTmp = ptTmp->ptNext;
    }

    return NULL;
}

int ID(char *strName)
{
    return (int)(strName[0]) + (int)(strName[1]) + (int)(strName[2]) + (int)(strName[3]);
}

int DisplayInit(void)
{
	int iError;
	
	iError = FBInit();

	return iError;
}

