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
    T_PixelDatas pt_pixelDatas;
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
    
    pt_pixelDatas.iBpp = ptDispOpr->iBpp;
    g_tBmpFileParser.GetPixelDatas(pucBMPmem,&pt_pixelDatas);
    printf("iwith is %d\n",pt_pixelDatas.iWidth);
    printf("iHeight is %d\n",pt_pixelDatas.iHeight);
    printf("iBpp is %d\n",pt_pixelDatas.iBpp);
    printf("iLineBytes is %d\n",pt_pixelDatas.iLineBytes);
    
    for(i = 0;i < pt_pixelDatas.iHeight;i++)
    {
        // 写到LCD去
        // FBShowLine(0, pt_pixelDatas.iWidth, i, pt_pixelDatas.aucPixelDatas);
    }
    
    // for(i=0;i<50;i++)
    // {
        // printf("%02x ",*(pt_pixelDatas.aucPixelDatas + i));
        
    // }
    // printf("\n");
    
    // unsigned char *aucPixelDatas;
    
    
    // g_tBmpFileParser.FreePixelDatas(pt_pixelDatasTmp);
    return 0;
}






