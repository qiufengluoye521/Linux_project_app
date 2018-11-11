#include <pic_operation.h>
#include <string.h>

int PicMerge(int x_start,int y_start,T_PixelDatas * t_pixel_smlData,T_PixelDatas * t_pixel_bigData)
{
    unsigned char * puc_sourceData;
    unsigned char * puc_dstData;
    int i;
    
    puc_sourceData  = t_pixel_smlData->aucPixelDatas;
    puc_dstData     = t_pixel_bigData->aucPixelDatas + y_start * t_pixel_bigData->iLineBytes + x_start * t_pixel_bigData->iBpp/8;
    
    for(i = 0;i < t_pixel_smlData->iHeight;i++)
    {
        memcpy(puc_dstData,puc_sourceData,t_pixel_smlData->iLineBytes);
        puc_dstData     += t_pixel_bigData->iLineBytes;
        puc_sourceData  += t_pixel_smlData->iLineBytes;
    }

    return 0;
}
