#include "page_manager.h"
#include <disp_manager.h>
#include <config.h>
#include <pic_operation.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

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

static ShowMainPage(PT_Layout atLayout)
{
    int iFdBmp;
    unsigned char * pucBMPmem;
    T_PixelDatas tIconPixelDatas;
    T_PixelDatas t_PixelDateFB;
    int iXres, iYres, iBpp;
    struct stat tStatTmp;
    int ret;
    PT_DispOpr ptDispOpr;
    
    GetDispResolution(&iXres, &iYres, &iBpp);
    
    // while(atLayout->strIconName)
    // {
        /* open bmp file */
        iFdBmp = open(atLayout->strIconName,O_RDWR);
        if(iFdBmp == -1)
        {
            DBG_PRINTF("cannot open %s\n",atLayout->strIconName);
        }
        
        /* use fstat fun to get fiel size*/
        fstat(iFdBmp,&tStatTmp);    
        /* 文件或对象映射到内存 */
        pucBMPmem = (unsigned char *)mmap(NULL,tStatTmp.st_size,PROT_READ | PROT_WRITE, MAP_SHARED, iFdBmp, 0);
        if (pucBMPmem == (unsigned char *)-1)
        {
            DBG_PRINTF("mmap error!\n");
            return -1;
        }
        
        ret = g_tBmpFileParser.isSupport(pucBMPmem);

        if(ret != 0)
        {
            DBG_PRINTF("file is not bmp format \n");
        }
        
        tIconPixelDatas.iBpp = iBpp;
        g_tBmpFileParser.GetPixelDatas(pucBMPmem,&tIconPixelDatas);
        DBG_PRINTF("iwith is %d\n",tIconPixelDatas.iWidth);
        DBG_PRINTF("iHeight is %d\n",tIconPixelDatas.iHeight);
        DBG_PRINTF("iBpp is %d\n",tIconPixelDatas.iBpp);
        DBG_PRINTF("iLineBytes is %d\n",tIconPixelDatas.iLineBytes);
        
        ptDispOpr = GetDispOpr("fb");
        t_PixelDateFB.iWidth        = ptDispOpr->iXres;
        t_PixelDateFB.iHeight       = ptDispOpr->iYres;
        t_PixelDateFB.iBpp          = ptDispOpr->iBpp;
        t_PixelDateFB.iLineBytes    = ptDispOpr->iXres * ptDispOpr->iBpp/8;
        t_PixelDateFB.aucPixelDatas = ptDispOpr->pucDispMem;
        
        PicMerge(0,0,&tIconPixelDatas,&t_PixelDateFB);
    // }
    
}

static void MainPageRun(void)
{
    /* 1. 显示页面 */
    ShowMainPage(g_atMainPageLayout);
}
static int MainPageGetInputEvent(PT_Layout atLayout, PT_InputEvent ptInputEvent)
{
    ;
}


int MainPageInit(void)
{
    return RegisterPageAction(&g_tMainPageAction);
}