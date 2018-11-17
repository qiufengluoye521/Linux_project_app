
#ifndef _RENDER_H
#define _RENDER_H

#include <pic_operation.h>
#include <disp_manager.h>
#include <page_manager.h>


int PicZoom(PT_PixelDatas ptOriginPic, PT_PixelDatas ptZoomPic);
int PicMerge(int iX, int iY, PT_PixelDatas ptSmallPic, PT_PixelDatas ptBigPic);
int GetPixelDatasForIcon(char *strFileName, PT_PixelDatas ptPixelDatas);


#endif /* _RENDER_H */

