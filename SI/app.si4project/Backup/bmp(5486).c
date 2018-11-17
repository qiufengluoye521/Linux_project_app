#include "pic_operation.h"
#include "debug_manager.h"
#include "config.h"
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

#pragma pack(push) /* 将当前pack设置压栈保存 */
#pragma pack(1)    /* 必须在结构体定义之前使用 */

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

#pragma pack (pop) /*取消指定对齐，恢复缺省对齐*/

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
    unsigned int dwColor;
    unsigned short *pwDstDatas16bpp = (unsigned short *)pudDstDatas;
    
    //DBG_PRINTF("width:%d srcBpp:%d iDstBpp:%d srcAdd:%d DstAdd:%d\n",iWidth,iSrcBpp,iDstBpp,pudSrcDatas,pudDstDatas);
    
    if(iSrcBpp != 24)
    {
        DBG_PRINTF("iSrcBpp is %d,not supported temporarily\n",iSrcBpp);
        return -1;
    }
    
    if(iDstBpp == 24)
    {
        memcpy(pudDstDatas, pudSrcDatas, iWidth*3);
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

    bmp_header                  = (BITMAPFILEHEADER *)aFileHead;

    file_size                   = bmp_header->bfSize;
    file_OffBits                = bmp_header->bfOffBits;
    DBG_PRINTF("off bits is %d\n",bmp_header->bfOffBits);
        
    bmpinfo_header              = (BITMAPINFOHEADER *) (aFileHead + 14);    //sizeof(BITMAPFILEHEADER));
    iWidth                      = bmpinfo_header->biWidth;
    iHeight                     = bmpinfo_header->biHeight;
    iBmpBpp                     = bmpinfo_header->biBitCount;
    
    if(iBmpBpp != 24)
    {
        DBG_PRINTF("iBmpBpp is %d,not supported temporarily\n",iBmpBpp);
        DBG_PRINTF("bmp header len is %d\n",sizeof(BITMAPFILEHEADER));
        return -1;
    }
    
    ptPixelDatas->iWidth        = iWidth;
    ptPixelDatas->iHeight       = iHeight;
    ptPixelDatas->iLineBytes    = iWidth * ptPixelDatas->iBpp / 8;
    ptPixelDatas->aucPixelDatas = malloc(iWidth * iHeight * ptPixelDatas->iBpp / 8);
    if (NULL == ptPixelDatas->aucPixelDatas)
    {
        DBG_PRINTF("malloc err\n");
        return -1;
    }
    DBG_PRINTF("ptPixelDatas->aucPixelDatas add:%x\n",ptPixelDatas->aucPixelDatas);


    iLineWidthReal              = iWidth * iBmpBpp / 8;
    iLineWidthAlign             = (iLineWidthReal + 3) & ~0x03;     /* 向4取整 */
    
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
                // DBG_PRINTF("%02x ",*(ucSource + i));
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
