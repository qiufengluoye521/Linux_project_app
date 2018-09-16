
#ifndef _FONTS_MANAGER_H
#define _FONTS_MANAGER_H

typedef struct FontBitMap {
	int iXLeft;             /*字体点阵左边点坐标*/
	int iYTop;              /*字体点阵上边点坐标*/
	int iXMax;              /*x方向最大值*/
	int iYMax;              /*y方向最大值*/
	int iBpp;               /*1个像素用一个bit和是一个byte表示*/
	int iPitch;             /* 对于单色位图, 两行象素之间的跨度(相差几个字节) */
	int iCurOriginX;        /*i当前原点的X位置*/
	int iCurOriginY;        /*i当前原点的Y位置*/
	int iNextOriginX;       /*下一个原点的X位置*/
	int iNextOriginY;       /*下一个原点的Y位置*/
	unsigned char *pucBuffer;
}T_FontBitMap, *PT_FontBitMap;

typedef struct FontOpr {
	char *name;
	int (*FontInit)(char *pcFontFile, unsigned int dwFontSize);
	int (*GetFontBitmap)(unsigned int dwCode, PT_FontBitMap ptFontBitMap);
	struct FontOpr *ptNext;
}T_FontOpr, *PT_FontOpr;


int RegisterFontOpr(PT_FontOpr ptFontOpr);
void ShowFontOpr(void);
int FontsInit(void);
int ASCIIInit(void);
int GBKInit(void);
int FreeTypeInit(void);
PT_FontOpr GetFontOpr(char *pcName);

#endif /* _FONTS_MANAGER_H */

