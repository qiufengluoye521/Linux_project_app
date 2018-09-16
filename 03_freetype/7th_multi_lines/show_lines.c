#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/fb.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <wchar.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H




struct fb_var_screeninfo var;       /* Current var */
struct fb_fix_screeninfo fix;       /* Current fix */
unsigned int line_width;
unsigned int pixel_width;
unsigned int screen_size;
unsigned char * fbmem;

int fd_hzk16;
struct stat hzk_stat;
unsigned char *hzkmem;


#define FONTDATAMAX 4096



/* color : 0x00RRGGBB */
void lcd_put_pixel(int x, int y, unsigned int color)
{
    unsigned char *pen_8 = fbmem+y*line_width+x*pixel_width;
    unsigned short *pen_16;    
    unsigned int *pen_32;    

    unsigned int red, green, blue;    

    pen_16 = (unsigned short *)pen_8;
    pen_32 = (unsigned int *)pen_8;

    switch (var.bits_per_pixel)
    {
        case 8:
        {
            *pen_8 = color;
            break;
        }
        case 16:
        {
            /* 565 */
            red   = (color >> 16) & 0xff;
            green = (color >> 8) & 0xff;
            blue  = (color >> 0) & 0xff;
            color = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
            *pen_16 = color;
            break;
        }
        case 32:
        {
            *pen_32 = color;
            break;
        }
        default:
        {
            printf("can't surport %dbpp\n", var.bits_per_pixel);
            break;
        }
    }
}



void draw_bitmap( FT_Bitmap*  bitmap,
                  FT_Int      x,
                  FT_Int      y)
{
    FT_Int  i, j, p, q;
    FT_Int  x_max = x + bitmap->width;
    FT_Int  y_max = y + bitmap->rows;

    //printf("x = %d, y = %d\n", x, y);

    for ( i = x, p = 0; i < x_max; i++, p++ )
    {
        for ( j = y, q = 0; j < y_max; j++, q++ )
        {
              if ( i < 0      || j < 0       ||
                   i >= var.xres || j >= var.yres )
                continue;

              //image[j][i] |= bitmap->buffer[q * bitmap->width + p];
              lcd_put_pixel(i, j, bitmap->buffer[q * bitmap->width + p]);
        }
    }
}



int main(int argc, char ** argv)
{
    int fb;
    int i;
    wchar_t *wstr1 = L"爱电电子idea!!!";
    wchar_t *wstr2 = L"www.idea.com";

    FT_Library    library;
    FT_Face       face;
    int error;
    FT_Vector     pen;
    FT_GlyphSlot  slot;

    if (argc != 2)
    {
        printf("Usage : %s <font_file>\n", argv[0]);
        return -1;
    }



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
    
    line_width      = var.xres * var.bits_per_pixel / 8;            /* x how many bits */
    pixel_width     = var.bits_per_pixel / 8;                       /* x how many bytes */
    screen_size     = var.xres * var.yres * var.bits_per_pixel / 8;

    fbmem = (unsigned char *)mmap(NULL,screen_size, PROT_READ | PROT_WRITE, MAP_SHARED,fb,0);
    printf("fbmem add is 0x%x \n",fbmem);
    printf("line_width is %d, pixel_width is %d, screen_size is %d\n",line_width,pixel_width,screen_size);

    /* fbmem 指向的地址，就是写LCD屏幕的地址，对fbmem地址对应写值  就是写LCD */
    memset(fbmem,0,screen_size);
    
    /* 显示矢量字体 */
    error = FT_Init_FreeType( &library );			   /* initialize library */

    /* error handling omitted */
    error = FT_New_Face( library, argv[1], 0, &face ); /* create face object */
    /* error handling omitted */
    slot = face->glyph;
    FT_Set_Pixel_Sizes(face, 24, 0);

    /* 确定座标:
     * lcd_x = 0
     * lcd_y = 24
     * 笛卡尔座标系:
     * x = lcd_x = 0
     * y = var.yres - 24
     */

    pen.x = 0*64;
    pen.y = (var.yres - 24) * 64;

    
    
    for(i=0;i<wcslen(wstr1);i++)
    {
        /* set transformation */
        FT_Set_Transform( face, 0, &pen);
        /* load glyph image into the slot (erase previous one) */
        error = FT_Load_Char( face, wstr1[i], FT_LOAD_RENDER );
        if (error)
        {
            printf("FT_Load_Char error\n");
            return -1;
        }
        
        draw_bitmap( &slot->bitmap,
                     slot->bitmap_left,
                     var.yres - slot->bitmap_top);
                     
        pen.x += slot->advance.x;
    }

    /* 确定座标:
     * lcd_x = 0
     * lcd_y = 60
     * 笛卡尔座标系:
     * x = lcd_x = 0
     * y = var.yres - 60
     */

    pen.x = 0*64;
    pen.y = (var.yres - 60) * 64;

    
    
    for(i=0;i<wcslen(wstr2);i++)
    {
        /* set transformation */
        FT_Set_Transform( face, 0, &pen);
        /* load glyph image into the slot (erase previous one) */
        error = FT_Load_Char( face, wstr2[i], FT_LOAD_RENDER );
        if (error)
        {
            printf("FT_Load_Char error\n");
            return -1;
        }
        
        draw_bitmap( &slot->bitmap,
                     slot->bitmap_left,
                     var.yres - slot->bitmap_top);
                     
        pen.x += slot->advance.x;
    }

}





