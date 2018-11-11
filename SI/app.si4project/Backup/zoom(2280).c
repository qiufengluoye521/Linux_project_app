
#include <pic_operation.h>
#include <stdlib.h>
#include <string.h>
#include <config.h>


int PicZoom(PT_PixelDatas ptOriginPic, PT_PixelDatas ptZoomPic)
{
    unsigned long dwDstWidth = ptZoomPic->iWidth;
    unsigned long* pdwSrcXTable = NULL;
    unsigned long x;
    unsigned long y;
    unsigned long dwSrcY;
    unsigned char *pucDest;
    unsigned char *pucSrc;
    unsigned long dwPixelBytes = ptOriginPic->iBpp/8;
    int width_cnt;
    int long_size;


    long_size = sizeof(unsigned long);
    width_cnt = long_size * dwDstWidth;
    printf("long_size %d\n",long_size);
    printf("dwDstWidth %d\n",dwDstWidth);
    printf("width_cnt %d\n",width_cnt);
    pdwSrcXTable = (unsigned long*)malloc(200);
    if(pdwSrcXTable == NULL ) {
        DBG_PRINTF("pdwSrcXTable malloc err\n");
        return -1;
    }
    free(pdwSrcXTable);
    pdwSrcXTable = NULL;

    if (ptOriginPic->iBpp != ptZoomPic->iBpp)
    {
        return -1;
    }

    // for (x = 0; x < dwDstWidth; x++)//生成表 pdwSrcXTable
    // {
        // pdwSrcXTable[x]=(x*ptOriginPic->iWidth/ptZoomPic->iWidth);
        // printf("table[%d]:%d\n",x,pdwSrcXTable[x]);
    // }
    //free(pdwSrcXTable);

    

    // for (y = 0; y < ptZoomPic->iHeight; y++)
    // {
        // dwSrcY = (y * ptOriginPic->iHeight / ptZoomPic->iHeight);

        // pucDest = ptZoomPic->aucPixelDatas + y*ptZoomPic->iLineBytes;
        // pucSrc  = ptOriginPic->aucPixelDatas + dwSrcY*ptOriginPic->iLineBytes;
        
        // for (x = 0; x <dwDstWidth; x++)
        // {
            // /* 原图座标: pdwSrcXTable[x]，srcy
             // * 缩放座标: x, y
             // */
             // memcpy(pucDest+x*dwPixelBytes, pucSrc+pdwSrcXTable[x]*dwPixelBytes, dwPixelBytes);
        // }
    // }


    //free(pdwSrcXTable);
    return 0;
}