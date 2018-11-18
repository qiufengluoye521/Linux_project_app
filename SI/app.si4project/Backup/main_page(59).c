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
    {0,0,0,0,"setting.bmp"},
    {0,50,0,0,"setting.bmp"},
    {0,100,0,0,"setting.bmp"},
    {0,150,0,0,NULL},
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
    
    /* 1. 获得显存 */
    ptVideoMem = GetVideoMem(ID("main"),1);
    if(ptVideoMem == NULL)
    {
        DBG_PRINTF("cannot get video mem for main page\n");
        return ;
    }
    
    /* 2. 描画数据 */
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
        
        /* 2.1 get pic datas from icon: store in tOriginIconPixelDatas */
        iError = GetPixelDatasForIcon(atLayout->strIconName, &tOriginIconPixelDatas);
        if (iError)
        {
            DBG_PRINTF("GetPixelDatasForIcon error!\n");
            return;
        }
        
        /* 2.2 pic size zoom to certain size */
        PicZoom(&tOriginIconPixelDatas, &tIconPixelDatas);
        
        // /* 2.3 copy the pic datas to vide memery */
        PicMerge(50,20,&tIconPixelDatas,&ptVideoMem->tPixelDatas);
        //FreePixelDatasForIcon(&tOriginIconPixelDatas);
        
        // /* 2.4 vide memery to lcd */
        DBG_PRINTF("FlushVideoMemToDev\n");
        FlushVideoMemToDev(ptVideoMem);
        DBG_PRINTF("ptVideoMem.iId:%d\n",ptVideoMem->iID);
        DBG_PRINTF("ptVideoMem.bDevFrameBuffer:%d\n",ptVideoMem->bDevFrameBuffer);
        //DBG_PRINTF("ptVideoMem.iId:%d\n",ptVideoMem.iID);
        
        // iFdBmp = open(atLayout->strIconName,O_RDWR);
        // if(iFdBmp == -1)
        // {
            // DBG_PRINTF("cannot open %s\n",atLayout->strIconName);
        // }
        
        // /* use fstat fun to get fiel size*/
        // fstat(iFdBmp,&tStatTmp);    
        // /* 文件或对象映射到内存 */
        
        // pucBMPmem = (unsigned char *)mmap(NULL,tStatTmp.st_size,PROT_READ | PROT_WRITE, MAP_SHARED, iFdBmp, 0);
        // if (pucBMPmem == (unsigned char *)-1)
        // {
            // DBG_PRINTF("mmap error!\n");
            // return ;
        // }
        
        // ret = g_tBmpFileParser.isSupport(pucBMPmem);

        // if(ret != 0)
        // {
            // DBG_PRINTF("file is not bmp format \n");
        // }
        
        // tIconPixelDatas.iBpp = iBpp;
        // g_tBmpFileParser.GetPixelDatas(pucBMPmem,&tIconPixelDatas);
        
        // DBG_PRINTF("iwith is %d\n",tIconPixelDatas.iWidth);
        // DBG_PRINTF("iHeight is %d\n",tIconPixelDatas.iHeight);
        // DBG_PRINTF("iBpp is %d\n",tIconPixelDatas.iBpp);
        // DBG_PRINTF("iLineBytes is %d\n",tIconPixelDatas.iLineBytes);
        
        // ptDispOpr = GetDispOpr("fb");
        // t_PixelDateFB.iWidth        = ptDispOpr->iXres;
        // t_PixelDateFB.iHeight       = ptDispOpr->iYres;
        // t_PixelDateFB.iBpp          = ptDispOpr->iBpp;
        // t_PixelDateFB.iLineBytes    = ptDispOpr->iXres * ptDispOpr->iBpp/8;
        // t_PixelDateFB.aucPixelDatas = ptDispOpr->pucDispMem;
        
        //PicMerge(0,0,&tIconPixelDatas,&t_PixelDateFB);
        // PicMerge(0,0,&tIconPixelDatas,ptVideoMem->tPixelDatas);
    }
    return;
    
    
    
    
    // while(atLayout->strIconName)
    // {
        /* open bmp file */
        // iFdBmp = open(atLayout->strIconName,O_RDWR);
        // if(iFdBmp == -1)
        // {
            // DBG_PRINTF("cannot open %s\n",atLayout->strIconName);
        // }
        
        // /* use fstat fun to get fiel size*/
        // fstat(iFdBmp,&tStatTmp);    
        // /* 文件或对象映射到内存 */
        // pucBMPmem = (unsigned char *)mmap(NULL,tStatTmp.st_size,PROT_READ | PROT_WRITE, MAP_SHARED, iFdBmp, 0);
        // if (pucBMPmem == (unsigned char *)-1)
        // {
            // DBG_PRINTF("mmap error!\n");
            // return -1;
        // }
        
        // ret = g_tBmpFileParser.isSupport(pucBMPmem);

        // if(ret != 0)
        // {
            // DBG_PRINTF("file is not bmp format \n");
        // }
        
        // tIconPixelDatas.iBpp = iBpp;
        // g_tBmpFileParser.GetPixelDatas(pucBMPmem,&tIconPixelDatas);
        // DBG_PRINTF("iwith is %d\n",tIconPixelDatas.iWidth);
        // DBG_PRINTF("iHeight is %d\n",tIconPixelDatas.iHeight);
        // DBG_PRINTF("iBpp is %d\n",tIconPixelDatas.iBpp);
        // DBG_PRINTF("iLineBytes is %d\n",tIconPixelDatas.iLineBytes);
        
        // ptDispOpr = GetDispOpr("fb");
        // t_PixelDateFB.iWidth        = ptDispOpr->iXres;
        // t_PixelDateFB.iHeight       = ptDispOpr->iYres;
        // t_PixelDateFB.iBpp          = ptDispOpr->iBpp;
        // t_PixelDateFB.iLineBytes    = ptDispOpr->iXres * ptDispOpr->iBpp/8;
        // t_PixelDateFB.aucPixelDatas = ptDispOpr->pucDispMem;
        
        // PicMerge(0,0,&tIconPixelDatas,&t_PixelDateFB);
    // }
    
}

static void MainPageRun(void)
{
    /* 1. 显示页面 */
    ShowMainPage(g_atMainPageLayout);
}
static int MainPageGetInputEvent(PT_Layout atLayout, PT_InputEvent ptInputEvent)
{
    return 0;
}


int MainPageInit(void)
{
    return RegisterPageAction(&g_tMainPageAction);
}
