#include "page_manager.h"
#include <disp_manager.h>
#include <config.h>
#include <pic_operation.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <render.h>

extern T_PicFileParser g_tBmpFileParser;

static T_Layout g_atMainPageLayout[] = {
    {0,0,0,0,"browse_mode.bmp"},
    {0,0,0,0,"continue_mod.bmp"},
    {0,0,0,0,"setting.bmp"},
    {0,0,0,0,NULL},
};

static T_PageLayout g_tMainPageLayout = {
    .iMaxTotalBytes = 0,
    .atLayout       = g_atMainPageIconsLayout,
};

static void MainPageRun(void);
static int MainPageGetInputEvent(PT_Layout atLayout, PT_InputEvent ptInputEvent);

T_PageAction g_tMainPageAction = 
{
    .name           = "main",
    .Run            = MainPageRun,
    .GetInputEvent  = MainPageGetInputEvent,
    //.Prepare        = MainPagePrepare,
    // ptNext
};

static void ShowMainPage(PT_Layout atLayout)
{
    T_PixelDatas tOriginIconPixelDatas;
    T_PixelDatas tIconPixelDatas;
    int iIconWidth;
	int iIconHeight;
	int iIconX;
	int iIconY;
    int iXres, iYres, iBpp;
    PT_VideoMem ptVideoMem;
    int iError;
    
    /* 1. ����Դ� */
    ptVideoMem = GetVideoMem(ID("main"),1);
    if(ptVideoMem == NULL)
    {
        DBG_PRINTF("cannot get video mem for main page\n");
        return ;
    }
    
    /* 2. �軭���� */
    if(ptVideoMem->ePicState != PS_GENERATED)
    {
        GetDispResolution(&iXres, &iYres, &iBpp);
        iIconHeight = iYres * 2 / 10;
        iIconWidth  = iIconHeight * 2;
        
        iIconX = (iXres - iIconWidth)/2;
        iIconY = iYres / 10;

        tIconPixelDatas.iBpp        = iBpp;
        tIconPixelDatas.iWidth      = iIconWidth;
        tIconPixelDatas.iHeight     = iIconHeight;
        tIconPixelDatas.iLineBytes  = iIconWidth * iBpp / 8;
        tIconPixelDatas.iTotalBytes = tIconPixelDatas.iLineBytes * iIconHeight;
        tIconPixelDatas.aucPixelDatas = malloc(tIconPixelDatas.iTotalBytes);
        if (tIconPixelDatas.aucPixelDatas == NULL)
        {
            free(tIconPixelDatas.aucPixelDatas);
            return;
        }
        /* process every icon */
        while(atLayout->strIconName)
        {
            atLayout->iTopLeftX     = iIconX;
            atLayout->iTopLeftY     = iIconY;
            atLayout->iBotRightX    = iIconX + iIconWidth - 1;
            atLayout->iBotRightY    = iIconY + iIconHeight - 1;
                
            
            /* 2.1 get pic datas from icon: store in tOriginIconPixelDatas */
            iError = GetPixelDatasForIcon(atLayout->strIconName, &tOriginIconPixelDatas);
            if (iError)
            {
                DBG_PRINTF("GetPixelDatasForIcon error!\n");
                return;
            }
            
            /* 2.2 pic size zoom to certain size */
            PicZoom(&tOriginIconPixelDatas, &tIconPixelDatas);
            
            /* 2.3 copy the pic datas to vide memery */
            PicMerge(iIconX,iIconY,&tIconPixelDatas,&ptVideoMem->tPixelDatas);
            //FreePixelDatasForIcon(&tOriginIconPixelDatas);
            atLayout ++;
            iIconY +=  iYres * 3 / 10;
        }
        
        /* 2.4 vide memery to lcd */
        FlushVideoMemToDev(ptVideoMem);
    }
    return;
    
}

static void MainPageRun(void)
{
    int iIndex;
    T_InputEvent tInputEvent;
    int bPressed = 0;
    int iIndexPressed = -1;

    
    /* 1. ��ʾҳ�� */
    ShowMainPage(g_atMainPageLayout);

    /* 3. ����GetInputEvent��������¼����������� */
    while (1)
    {
        iIndex = MainPageGetInputEvent(g_atMainPageLayout, &tInputEvent);
        if (tInputEvent.iPressure == 0)
        {
            /* ������ɿ� */
            if (bPressed)
            {
                /* �����а�ť������ */
                ReleaseButton(&g_atMainPageLayout[iIndexPressed]);
                bPressed = 0;
                iIndexPressed = -1;
            }
        }
        else
        {
            /* ����״̬ */
            if (iIndex != -1)
            {
                if (!bPressed)
                {
                    /* δ�����°�ť */
                    bPressed = 1;
                    iIndexPressed = iIndex;
                    PressButton(&g_atMainPageLayout[iIndexPressed]);
                }
            }
        }
    }
}

static int MainPageGetInputEvent(PT_Layout atLayout, PT_InputEvent ptInputEvent)
{
    T_InputEvent tInputEvent;
    int iRet;
    int i = 0;

    /* ���ԭʼ�Ĵ��������� 
     * ���ǵ���input_manager.c�ĺ������˺������õ�ǰ�߷�����
     * ���������̻߳�����ݺ󣬻��������
     */
    iRet = GetInputEvent(&tInputEvent);
    if (iRet)
    {
        return -1;
    }

    if (tInputEvent.iType != INPUT_TYPE_TOUCHSCREEN)
    {
        return -1;
    }

    *ptInputEvent = tInputEvent;

    /* �������� */
    /* ȷ������λ����һ����ť�� */
    while (atLayout[i].strIconName)
    {
        if ((tInputEvent.iX >= atLayout[i].iTopLeftX) && (tInputEvent.iX <= atLayout[i].iBotRightX) && \
             (tInputEvent.iY >= atLayout[i].iTopLeftY) && (tInputEvent.iY <= atLayout[i].iBotRightY))
        {
            /* �ҵ��˱����еİ�ť */
            return i;
        }
        else
        {
            i++;
        }			
    }

    /* ����û�����ڰ�ť�� */
    return -1;
}


int MainPageInit(void)
{
    return RegisterPageAction(&g_tMainPageAction);
}
