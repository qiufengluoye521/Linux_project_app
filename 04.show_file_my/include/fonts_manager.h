
#ifndef _FONTS_MANAGER_H
#define _FONTS_MANAGER_H

typedef struct FontBitMap {
	int iXLeft;             /*���������ߵ�����*/
	int iYTop;              /*��������ϱߵ�����*/
	int iXMax;              /*x�������ֵ*/
	int iYMax;              /*y�������ֵ*/
	int iBpp;               /*1��������һ��bit����һ��byte��ʾ*/
	int iPitch;             /* ���ڵ�ɫλͼ, ��������֮��Ŀ��(�����ֽ�) */
	int iCurOriginX;        /*i��ǰԭ���Xλ��*/
	int iCurOriginY;        /*i��ǰԭ���Yλ��*/
	int iNextOriginX;       /*��һ��ԭ���Xλ��*/
	int iNextOriginY;       /*��һ��ԭ���Yλ��*/
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

