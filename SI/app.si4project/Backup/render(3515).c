#include <render.h>
#include <stdio.h>
#include <file.h>
#include <config.h>

extern T_PicFileParser g_tBmpFileParser;

int GetPixelDatasForIcon(char *strFileName, PT_PixelDatas ptPixelDatas)
{
    T_FileMap tFileMap;
    int iError = 0;
    int iXres, iYres, iBpp;
    
    snprintf(tFileMap.strFileName,128,"%s%s",ICON_PATH,strFileName);
    tFileMap.strFileName[127] = '\0';
    
    iError = MapFile(&tFileMap);
    if(iError)
    {
        DBG_PRINTF("MapFile %s error!\n", strFileName);
        return -1;
    }
    iError = g_tBmpFileParser.isSupport(tFileMap.pucFileMapMem);
    if (iError != 0)
    {
        DBG_PRINTF("%s is not bmp file\n", strFileName);
        return -1;
    }

    GetDispResolution(&iXres, &iYres, &iBpp);
    ptPixelDatas->iBpp = iBpp;
    iError = g_tBmpFileParser.GetPixelDatas(tFileMap.pucFileMapMem, ptPixelDatas);
    if (iError)
    {
        DBG_PRINTF("GetPixelDatas for %s error!\n", strFileName);
        return -1;
    }
    
    return 0;
}

int GetFontPixel()
{
}

int GetPicPixel()
{
}


int DrawPixel()
{
}

