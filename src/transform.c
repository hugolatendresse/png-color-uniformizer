/* file: transform.c
*
 * @brief
 * TODO
 *
 */

#include <stdlib.h>

#include "png.h"
#include "../include/pcu.h"


static int stride;

static int get_RGBA_stride(int width, int additional_stride) {
    return width * 4 + additional_stride;
}

static void get_pixel(unsigned char *buffer, int row, int col, RGBA_Pixel *pixel) {
    pixel->r = buffer[row * stride + col * RGBA_PIXEL_LEN + offsetof(RGBA_Pixel, r)];
    pixel->g = buffer[row * stride + col * RGBA_PIXEL_LEN + offsetof(RGBA_Pixel, g)];
    pixel->b = buffer[row * stride + col * RGBA_PIXEL_LEN + offsetof(RGBA_Pixel, b)];
    pixel->a = buffer[row * stride + col * RGBA_PIXEL_LEN + offsetof(RGBA_Pixel, a)];
}

int transform(png_bytep buffer, png_uint_32 height, png_uint_32 width, png_int_32 row_stride, int format,
              unsigned int k) {
    stride = get_RGBA_stride(width, row_stride);

    int member_cnt;
    if (format == PNG_FORMAT_RGBA) {
        member_cnt = RGBA_PIXEL_LEN;
    } else if (format == PNG_FORMAT_RGB) {
        member_cnt = RGB_PIXEL_LEN;
    } else {
        fprintf(stderr, "Only RGBA format is supported right now. Try a different PNG file\n");
        return 0;
    }
    unsigned int pixel_cnt = width * height;
    RGBA_Pixel *input_pixels = (RGBA_Pixel *)buffer;

    double **pixels = malloc(pixel_cnt * sizeof(double *));
    for (unsigned int i = 0; i < pixel_cnt; i++) {
        pixels[i] = malloc(4 * sizeof(double));
        pixels[i][0] = input_pixels[i].r;
        pixels[i][1] = input_pixels[i].g;
        pixels[i][2] = input_pixels[i].b;
        pixels[i][3] = input_pixels[i].a;
    }

    double **centroids = create_centroids(pixels, k, pixel_cnt, member_cnt);
    int* cluster_map = km(centroids, pixels, k, pixel_cnt, member_cnt);

    // Apply cluster map
    for (unsigned int i = 0; i < pixel_cnt; i++) {
        input_pixels[i].r = centroids[cluster_map[i]][0];
        input_pixels[i].g = centroids[cluster_map[i]][1];
        input_pixels[i].b = centroids[cluster_map[i]][2];
        // Only change alpha if change_alpha is true or if the pixel is intended to be white
        if (change_alpha || (input_pixels[i].r == 0 && input_pixels[i].g == 0 && input_pixels[i].b == 0)) {
            input_pixels[i].a = centroids[cluster_map[i]][3];
        }
    }

    // printf("Clusters:\n");
    // print_clusters(clusters, k, pixel_cnt, RGBA_POS_DOUBLE_LEN); // TODO probably need to pass size of struct rather than vector_size
    // printf("\n");

    // TODO free everything
    // free(pixels);
    // for (int i=0 ; i<k ; i++)
    //     free(clusters[i]);
    // free(clusters);

}

static void set_pixel(unsigned char *buffer, int row, int col, RGBA_Pixel *pixel) {
    buffer[row * stride + col * RGBA_PIXEL_LEN + offsetof(RGBA_Pixel, r)] = pixel->r;
    buffer[row * stride + col * RGBA_PIXEL_LEN + offsetof(RGBA_Pixel, g)] = pixel->g;
    buffer[row * stride + col * RGBA_PIXEL_LEN + offsetof(RGBA_Pixel, b)] = pixel->b;
    buffer[row * stride + col * RGBA_PIXEL_LEN + offsetof(RGBA_Pixel, a)] = pixel->a;
}