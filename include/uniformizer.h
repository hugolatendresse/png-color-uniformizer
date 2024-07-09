#ifndef UNIFORMIZER_H
#define UNIFORMIZER_H

#include <png.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct rgba_pixel_ {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    } RGBA_Pixel;

    // TODO if this is never used, can delete, and RGBA_Pixel_Pos_Double can steal its name
    typedef struct rgba_pixel_pos_ {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
        unsigned int pos;
    } RGBA_Pixel_Pos;

    typedef struct rgba_pixel_pos_double_ {
        double r;
        double g;
        double b;
        double a;
        unsigned int pos;
    } RGBA_Pixel_Pos_Double;


    // Width of RGBA pixel and RGBA pixel with position
    #define RGBA_LEN 4
    #define RGBA_POS_LEN 5
    #define RGBA_POS_DOUBLE_LEN 5

    int display_image(int height, int width, unsigned char *data);
    int transform(png_bytep buffer, png_uint_32 height, png_uint_32 width, png_int_32 row_stride, int format,
        unsigned int k);

#ifdef __cplusplus
}
#endif

#endif
