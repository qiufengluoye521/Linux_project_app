#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <config.h>
#include <draw.h>
#include <encoding_manager.h>
#include <fonts_manager.h>
#include <disp_manager.h>
#include <input_manager.h>
#include <pic_operation.h>
#include <render.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

extern T_PicFileParser g_tBmpFileParser;

int main(int argc,char ** argv)
{
    PT_DispOpr ptDispOpr;
    int iFdBmp;
    struct stat tStatTmp;
    unsigned char *pucBMPmem;
    int ret;
    T_PixelDatas t_pixelDatas;
    T_PixelDatas t_PixelDateFB;
    
    int i;
    char chrTmp;
    
    if(argc != 2)
    {
        printf("Usage:\n");
        printf("%s <bmp_fiel>\n",argv[0]);
        return -1;
    }
    
    DisplayInit();
    
    ptDispOpr = GetDispOpr("fb");
    ptDispOpr->DeviceInit();
    ptDispOpr->CleanScreen(0);
    
    printf("iXres:%d\n",ptDispOpr->iXres);
    printf("iYres:%d\n",ptDispOpr->iYres);
    
    /* open bmp file */
    iFdBmp = open(argv[1],O_RDWR);
    if(iFdBmp == -1)
    {
        printf("cannot open %s\n",argv[1]);
    }
    
    /* use fstat fun to get fiel size*/
    fstat(iFdBmp,&tStatTmp);    
    /* 文件或对象映射到内存 */
    pucBMPmem = (unsigned char *)mmap(NULL,tStatTmp.st_size,PROT_READ | PROT_WRITE, MAP_SHARED, iFdBmp, 0);
    if (pucBMPmem == (unsigned char *)-1)
    {
        printf("mmap error!\n");
        return -1;
    }
    
    ret = g_tBmpFileParser.isSupport(pucBMPmem);

    if(ret != 0)
    {
        printf("file is not bmp format \n");
    }
    
    t_pixelDatas.iBpp = ptDispOpr->iBpp;
    g_tBmpFileParser.GetPixelDatas(pucBMPmem,&t_pixelDatas);
    // printf("iwith is %d\n",t_pixelDatas.iWidth);
    // printf("iHeight is %d\n",t_pixelDatas.iHeight);
    // printf("iBpp is %d\n",t_pixelDatas.iBpp);
    // printf("iLineBytes is %d\n",t_pixelDatas.iLineBytes);
    
    t_PixelDateFB.iWidth        = ptDispOpr->iXres;
    t_PixelDateFB.iHeight       = ptDispOpr->iYres;
    t_PixelDateFB.iBpp          = ptDispOpr->iBpp;
    t_PixelDateFB.iLineBytes    = ptDispOpr->iXres * ptDispOpr->iBpp/8;
    t_PixelDateFB.aucPixelDatas = ptDispOpr->pucDispMem;
    
    PicMerge(0,0,&t_pixelDatas,&t_PixelDateFB);
    
    return 0;
}






