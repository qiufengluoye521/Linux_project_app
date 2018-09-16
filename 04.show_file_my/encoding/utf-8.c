#include <config.h>
#include <encoding_manager.h>
#include <string.h>


static int isUtf8Coding(unsigned char *pucBufHead);
static int Utf8GetCodeFrBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode);



static T_EncodingOpr g_tUtf8EncodingOpr = {
    .name          = "utf8",
    .iHeadLen      = 3,
    .isSupport     = isUtf8Coding,
    .GetCodeFrmBuf = Utf8GetCodeFrBuf,

};

static int isUtf8Coding(unsigned char *pucBufHead)
{
    const unsigned char utf8head[] = {0xEF,0xBB,0xBF,0x00};
    if(strncmp((const char *)pucBufHead, utf8head, 3) == 0) {
        // this is utf8 coding
        return 1;
    } else {
        return 0;
    }
}

/* 获得前导为1的位的个数
 * 比如二进制数 11001111 的前导1有2位
 *              11100001 的前导1有3位
 */
static int GetPreOneBits(unsigned char ucVal)
{
    int i;
    int j = 0;

    for (i = 7; i >= 0; i--)
    {
        if (!(ucVal & (1<<i)))
            break;
        else
            j++;
    }
    return j;

}


static int Utf8GetCodeFrBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode) 
{
    unsigned int uiNum = 0;
    unsigned char ucVal;
    unsigned int udwCode;
    int i = 0;
    
    if(pucBufStart >= pucBufEnd) {
        // 文件结束
        return 0;
    }

    uiNum = GetPreOneBits(pucBufStart[0]);
    
    if(pucBufStart + uiNum >= pucBufEnd) {
        // 文件结束
        return 0;
    }

    // 1的个数为0个，表示为ASCII码
    if(0 == uiNum) {
        *pdwCode = pucBufStart[0];
        return 1;
    } else {

        ucVal   = pucBufStart[0];
        ucVal   <<= uiNum;
        ucVal   >>= uiNum;
        udwCode = ucVal;
        for(i = 1; i < uiNum; i++) {
            ucVal = pucBufStart[i] & 0x3F;
            udwCode <<= 6;
            udwCode |= ucVal;
        }
        *pdwCode = udwCode;
        return uiNum;
    }
}

int  Utf8EncodingInit(void) 
{
    g_tUtf8EncodingOpr.aptFontOprSupported[0] = GetFontOpr("freetype");
    g_tUtf8EncodingOpr.aptFontOprSupported[1] = NULL;
    return RegisterEncodingOpr(&g_tUtf8EncodingOpr);
}




