#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/fb.h>

struct fb_var_screeninfo var;       /* Current var */
struct fb_fix_screeninfo fix;       /* Current fix */
unsigned int line_width;
unsigned int pixel_width;
unsigned int screen_size;
unsigned char * fbmem;


void delay(void)
{
    unsigned int i,j;
    for(i=0;i<10000;i++) {
        for(j=0;j<10000;j++) {
            ;
        }
    }
}

int main(int argc, char ** argv)
{
    int fb;

    fb  = open("/dev/fb0",O_RDWR);
    if(fb < 0)
    {
        printf("cannot open dev fb0\n");
        return -1;
    }

    if(ioctl(fb,FBIOGET_VSCREENINFO,&var)) {
        printf("cannot get var\n");
        return -1;
    }

    if(ioctl(fb,FBIOGET_FSCREENINFO,&fix)) {
        printf("cannot get fix\n");
        return -1;
    }
    
    line_width      = var.xres * var.bits_per_pixel;    /* x how many bits */
    pixel_width     = var.bits_per_pixel / 8;           /* x how many bytes */
    screen_size     = var.xres * var.yres * var.bits_per_pixel / 8;

    fbmem = (unsigned char *)mmap(NULL,screen_size, PROT_READ | PROT_WRITE, MAP_SHARED,fb,0);

    /* fbmem 指向的地址，就是写LCD屏幕的地址，对fbmem地址对应写值  就是写LCD */
    memset(fbmem,0,screen_size);

    show_

}





