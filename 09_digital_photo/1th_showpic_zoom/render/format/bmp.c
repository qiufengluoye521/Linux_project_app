#include "pic_operation.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int isFileBmpFormat(unsigned char *aFileHead);
int GetPixelDatasFrmBmp(unsigned char *aFileHead, PT_PixelDatas ptPixelDatas);
int FreeBmpPixelDatas(PT_PixelDatas ptPixelDatas);

T_PicFileParser g_tBmpFileParser = {
    .name           = "bmp",
    .isSupport      = isFileBmpFormat,
    .GetPixelDatas  = GetPixelDatasFrmBmp,
    .FreePixelDatas = FreeBmpPixelDatas,
};

#pragma pack(push) /* ����ǰpack����ѹջ���� */
#pragma pack(1)    /* �����ڽṹ�嶨��֮ǰʹ�� */

typedef struct tagBITMAPFILEHEADER { /* bmfh */ 
    unsigned short bfType;  
    unsigned int bfSize; 
    unsigned short bfReserved1; 
    unsigned short bfReserved2; 
    unsigned int bfOffBits;
} BITMAPFILEHEADER; 

typedef struct tagBITMAPINFOHEADER { /* bmih */ 
    unsigned int biSize; 
    unsigned int biWidth; 
    unsigned int biHeight; 
    unsigned short biPlanes; 
    unsigned short biBitCount; 
    unsigned int biCompression; 
    unsigned int biSizeImage; 
    unsigned int biXPelsPerMeter; 
    unsigned int biYPelsPerMeter; 
    unsigned int biClrUsed; 
    unsigned int biClrImportant;
} BITMAPINFOHEADER;

#pragma pack (pop) /*ȡ��ָ�����룬�ָ�ȱʡ����*/

int isFileBmpFormat(unsigned char *aFileHead)
{
    if((aFileHead[0] == 0x42) && (aFileHead[1] == 0x4D))
    {
        return 0;
    }
    else{
        return -1;
    }
}

/* convert one line raw data from pudSrcDatas  to pudDstDatas */
static int CovertOneLine(unsigned int iWidth, unsigned short iSrcBpp, int iDstBpp, unsigned char *pudSrcDatas, unsigned char *pudDstDatas)
{
    int i;
    unsigned int redTmp;
    unsigned int greenTmp;
    unsigned int blueTmp;
    int pos = 0;
    int des_pos = 0;
    unsigned int dwColor;
    unsigned short *pwDstDatas16bpp = (unsigned short *)pudDstDatas;
    unsigned int   *pwDstDatas32bpp = (unsigned int *)pudDstDatas;
    
    //printf("width:%d srcBpp:%d iDstBpp:%d srcAdd:%d DstAdd:%d\n",iWidth,iSrcBpp,iDstBpp,pudSrcDatas,pudDstDatas);
    
    if(iSrcBpp != 24)
    {
        printf("iSrcBpp is %d,not supported temporarily\n",iSrcBpp);
        return -1;
    }
    
    if(iDstBpp == 24)
    {
        strncpy(pudDstDatas, pudSrcDatas, iWidth * iDstBpp / 8);
    } else
    {
        for(i = 0;i < iWidth;i++)
        {
            blueTmp     = pudSrcDatas[pos++];
            greenTmp    = pudSrcDatas[pos++];
            redTmp      = pudSrcDatas[pos++];
            if(iDstBpp == 16)
            {
                /* 565 */
                redTmp   = redTmp >> 3;
                greenTmp = greenTmp >> 2;
                blueTmp  = blueTmp >> 3;
                dwColor  = (redTmp << 11) | (greenTmp << 5) | (blueTmp);
                // *(pudDstDatas + des_pos++) = (dwColor >> 8) || 0xFF;
                // *(pudDstDatas + des_pos++) = (dwColor >> 0) || 0xFF;
                *pwDstDatas16bpp = dwColor;
                pwDstDatas16bpp++;
            }
        }
    }
    
    return 0;
}

/* get pixel datas from aFileHead file,result store in ptPixelDatas struct
*  raw data is ptPixelDatas->aucPixelDatas
*/
int GetPixelDatasFrmBmp(unsigned char *aFileHead, PT_PixelDatas ptPixelDatas)
{
    BITMAPFILEHEADER *bmp_header;
    BITMAPINFOHEADER *bmpinfo_header;
    unsigned int file_size;
    unsigned int file_OffBits;
    unsigned int iWidth;
    unsigned int iHeight;
    unsigned short iBmpBpp;
    int iLineWidthAlign;
    int iLineWidthReal;
    unsigned char *ucSource;
    unsigned char *ucDest;
    int y;
    int i;

    bmp_header                  = (BITMAPFILEHEADER *)aFileHead;

    file_size                   = bmp_header->bfSize;
    file_OffBits                = bmp_header->bfOffBits;
    printf("off bits is %d\n",bmp_header->bfOffBits);
        
    bmpinfo_header              = (BITMAPINFOHEADER *) (aFileHead + 14);    //sizeof(BITMAPFILEHEADER));
    iWidth                      = bmpinfo_header->biWidth;
    iHeight                     = bmpinfo_header->biHeight;
    iBmpBpp                     = bmpinfo_header->biBitCount;
    
    if(iBmpBpp != 24)
    {
        printf("iBmpBpp is %d,not supported temporarily\n",iBmpBpp);
        printf("bmp header len is %d\n",sizeof(BITMAPFILEHEADER));
        return -1;
    }
    
    ptPixelDatas->iWidth        = iWidth;
    ptPixelDatas->iHeight       = iHeight;
    ptPixelDatas->iLineBytes    = iWidth * iBmpBpp / 8;
    ptPixelDatas->aucPixelDatas = malloc(iWidth * iHeight * ptPixelDatas->iBpp / 8);
    if (NULL == ptPixelDatas->aucPixelDatas)
    {
        printf("malloc err\n");
        return -1;
    }


    iLineWidthReal              = iWidth * iBmpBpp / 8;
    iLineWidthAlign             = (iLineWidthReal + 3) & ~0x03;     /* ��4ȡ�� */
    
    ucSource                    = aFileHead + bmp_header->bfOffBits;
    ucSource                   += (bmpinfo_header->biHeight - 1) * iLineWidthAlign;
    ucDest                      = ptPixelDatas->aucPixelDatas;
    for(y = 0;y < iHeight;y++)
    {
        CovertOneLine(iWidth, iBmpBpp, ptPixelDatas->iBpp, ucSource, ucDest);
        // if(y == iHeight - 1)
        // {
            // for(i=0;i<iWidth;i++)
            // {
                // printf("%02x ",*(ucSource + i));
            // }
            
        // }
        ucSource   -= iLineWidthAlign;
        ucDest     += ptPixelDatas->iLineBytes;
    }
    
    return 0;
}

int FreeBmpPixelDatas(PT_PixelDatas ptPixelDatas)
{
    free(ptPixelDatas);
    
    return 0;
}
