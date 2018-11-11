#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct PixelDatas {
	int iWidth;
	int iHeight;
	int iBpp;
	int iLineBytes;
    int iTotalBytes;
	unsigned char *aucPixelDatas;
}T_PixelDatas, *PT_PixelDatas;

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
    pdwSrcXTable = (unsigned long*)malloc(400);
    if(pdwSrcXTable == NULL ) {
        printf("pdwSrcXTable malloc err\n");
        return -1;
    }
    printf("pdwSrcXTable add:%x\n",pdwSrcXTable);
    free(pdwSrcXTable);
    pdwSrcXTable = NULL;

    if (ptOriginPic->iBpp != ptZoomPic->iBpp)
    {
        return -1;
    }

    return 0;
}

int main()
{
    T_PixelDatas tOriginIconPixelDatas;
    T_PixelDatas tIconPixelDatas;
    
    tIconPixelDatas.iBpp        = 16;
    tIconPixelDatas.iWidth      = 108;
    tIconPixelDatas.iHeight     = 64;
    tIconPixelDatas.iLineBytes  = 108 * 16 / 8;
    tIconPixelDatas.iTotalBytes = tIconPixelDatas.iLineBytes * tIconPixelDatas.iHeight;
    tIconPixelDatas.aucPixelDatas = malloc(tIconPixelDatas.iTotalBytes);
    if (tIconPixelDatas.aucPixelDatas == NULL)
    {
        free(tIconPixelDatas.aucPixelDatas);
        return;
    }
    
    tOriginIconPixelDatas.iBpp        = 16;
    tOriginIconPixelDatas.iWidth      = 256;
    tOriginIconPixelDatas.iHeight     = 64;
    tOriginIconPixelDatas.iLineBytes  = 256 * 16 / 8;
    tOriginIconPixelDatas.iTotalBytes = tOriginIconPixelDatas.iLineBytes * tOriginIconPixelDatas.iHeight;
    tOriginIconPixelDatas.aucPixelDatas = malloc(tOriginIconPixelDatas.iTotalBytes);
    if (tOriginIconPixelDatas.aucPixelDatas == NULL)
    {
        free(tOriginIconPixelDatas.aucPixelDatas);
        return;
    }
    
    PicZoom(&tOriginIconPixelDatas, &tIconPixelDatas);
    
}