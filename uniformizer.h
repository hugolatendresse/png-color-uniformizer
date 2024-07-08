#ifdef __cplusplus
extern "C" {
#endif

    typedef struct rgba_pixel_ {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    } RGBA_Pixel;

    int display_image(int height, int width, unsigned char *data);
    int transform(png_bytep buffer, png_uint_32 height, png_uint_32 width, png_int_32 row_stride, int format);

#ifdef __cplusplus
}
#endif