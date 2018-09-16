#include <config.h>
#include <disp_manager.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <string.h>

static int FB_DeviceInit(void);
static int FB_ShowPixel(int iPenX, int iPenY, unsigned int dwColor);
static int FB_CleanScreen(void);

static int g_fb;
struct fb_var_screeninfo g_tFBVar;       /* Current var */
struct fb_fix_screeninfo g_tFBFix;       /* Current fix */
static unsigned int g_dwLine_width;
static unsigned int g_dwPixel_width;
static unsigned int g_dwScreen_size;
static unsigned char * g_pucFbmem;





static T_DispOpr g_tFBopr = 
{
    .name           = "fb",
    .DeviceInit     = FB_DeviceInit,
    .ShowPixel      = FB_ShowPixel,
    .CleanScreen    = FB_CleanScreen,
};


static int FB_DeviceInit(void)
{
    g_fb  = open("/dev/fb0",O_RDWR);
    if(g_fb < 0)
    {
        printf("cannot open dev fb0\n");
        return -1;
    }
    
    if(ioctl(g_fb,FBIOGET_VSCREENINFO,&g_tFBVar)) {
        printf("cannot get g_tFBVar\n");
        return -1;
    }

    if(ioctl(g_fb,FBIOGET_FSCREENINFO,&g_tFBFix)) {
        printf("cannot get g_tFBFix\n");
        return -1;
    }

    
    g_dwLine_width      = g_tFBVar.xres * g_tFBVar.bits_per_pixel / 8;            /* x how many bits */
    g_dwPixel_width     = g_tFBVar.bits_per_pixel / 8;                       /* x how many bytes */
    g_dwScreen_size     = g_tFBVar.xres * g_tFBVar.yres * g_tFBVar.bits_per_pixel / 8;

    g_pucFbmem = (unsigned char *)mmap(NULL,g_dwScreen_size, PROT_READ | PROT_WRITE, MAP_SHARED,g_fb,0);
    printf("fbmem add is 0x%x \n",g_pucFbmem);
    printf("line_width is %d, pixel_width is %d, screen_size is %d\n",g_dwLine_width,g_dwPixel_width,g_dwScreen_size);

    g_tFBopr.iXres      = g_tFBVar.xres;
    g_tFBopr.iYres      = g_tFBVar.yres;
    g_tFBopr.iBpp       = g_tFBVar.bits_per_pixel;


    return 0;

}

static int FB_ShowPixel(int iPenX, int iPenY, unsigned int dwColor)
{
    unsigned char * pucFB;
    unsigned short * pwFB16bpp;
    unsigned int * pdwFB32bpp;
    unsigned int dwRed, dwGreen, dwBlue;    

    pucFB       = g_pucFbmem + iPenY*g_dwLine_width + iPenX*g_dwPixel_width;
    pwFB16bpp   = (unsigned short*)pucFB;
    pdwFB32bpp  = (unsigned int*)pucFB;



    switch (g_tFBVar.bits_per_pixel)
    {
        case 8:
        {
            *pucFB = dwColor;
            break;
        }
        case 16:
        {
            /* 565 */
            dwRed   = (dwColor >> 16) & 0xff;
            dwGreen = (dwColor >> 8) & 0xff;
            dwBlue  = (dwColor >> 0) & 0xff;
            dwColor = ((dwRed >> 3) << 11) | ((dwGreen >> 2) << 5) | (dwBlue >> 3);
            *pwFB16bpp = dwColor;
            break;
        }
        case 32:
        {
            *pdwFB32bpp = dwColor;
            break;
        }
        default:
        {
            printf("can't surport %dbpp\n", g_tFBVar.bits_per_pixel);
            break;
        }
    }


    return 0;
}

static int FB_CleanScreen(void)
{
    memset(g_pucFbmem,0,g_dwScreen_size);

    return 0;
}

int FBInit(void)
{
    return RegisterDispOpr(&g_tFBopr);
}





