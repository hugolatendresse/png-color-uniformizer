/* file: transform.c
*
 * @brief
 * TODO
 *
 */

#include "png.h"
#include "uniformizer.h"

#define RGBA_LEN 4

static int stride;

static int get_RGBA_stride(int width, int additional_stride) {
    return width * 4 + additional_stride;
}

static void get_pixel(unsigned char *buffer, int row, int col, RGBA_Pixel *pixel) {
    pixel->r = buffer[row * stride + col * RGBA_LEN + offsetof(RGBA_Pixel, r)];
    pixel->g = buffer[row * stride + col * RGBA_LEN + offsetof(RGBA_Pixel, g)];
    pixel->b = buffer[row * stride + col * RGBA_LEN + offsetof(RGBA_Pixel, b)];
    pixel->a = buffer[row * stride + col * RGBA_LEN + offsetof(RGBA_Pixel, a)];
}

static void set_pixel(unsigned char *buffer, int row, int col, RGBA_Pixel *pixel) {
    buffer[row * stride + col * RGBA_LEN + offsetof(RGBA_Pixel, r)] = pixel->r;
    buffer[row * stride + col * RGBA_LEN + offsetof(RGBA_Pixel, g)] = pixel->g;
    buffer[row * stride + col * RGBA_LEN + offsetof(RGBA_Pixel, b)] = pixel->b;
    buffer[row * stride + col * RGBA_LEN + offsetof(RGBA_Pixel, a)] = pixel->a;
}

int transform(png_bytep buffer, png_uint_32 height, png_uint_32 width, png_int_32 row_stride, int format) {
    stride = get_RGBA_stride(width, row_stride);

    if (format != PNG_FORMAT_RGBA) {
        fprintf(stderr, "Only RGBA format is supported right now.\n");
        return 0;
    }

    RGBA_Pixel pixel = {255, 0, 0, 255};

    set_pixel(buffer, 0, 0, &pixel); // Modify
    set_pixel(buffer, 0, 1, &pixel); // Modify
    set_pixel(buffer, 0, 2, &pixel); // Modify
    set_pixel(buffer, 0, 3, &pixel); // Modify
    set_pixel(buffer, 0, 4, &pixel); // Modify
    set_pixel(buffer, 1, 0, &pixel); // Modify
    set_pixel(buffer, 1, 1, &pixel); // Modify
    set_pixel(buffer, 1, 2, &pixel); // Modify
    set_pixel(buffer, 1, 3, &pixel); // Modify
    set_pixel(buffer, 1, 4, &pixel); // Modify

    return 1;
}

