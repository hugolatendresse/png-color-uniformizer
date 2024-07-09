/* file: transform.c
*
 * @brief
 * TODO
 *
 */

#include <stdlib.h>

#include "png.h"
#include "../include/uniformizer.h"
#include "../include/km.h"


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

int transform(png_bytep buffer, png_uint_32 height, png_uint_32 width, png_int_32 row_stride, int format,
              unsigned int k) {

    // TODO implement options k r g b a as written in helper of main.c

    stride = get_RGBA_stride(width, row_stride);

    if (format != PNG_FORMAT_RGBA) {
        fprintf(stderr, "Only RGBA format is supported right now.\n");
        return 0;
    }

    RGBA_Pixel pixel = {255, 0, 0, 255};

    // set_pixel(buffer, 0, 0, &pixel); // Modify
    // set_pixel(buffer, 0, 1, &pixel); // Modify
    // set_pixel(buffer, 0, 2, &pixel); // Modify
    // set_pixel(buffer, 0, 3, &pixel); // Modify
    // set_pixel(buffer, 0, 4, &pixel); // Modify
    // set_pixel(buffer, 1, 0, &pixel); // Modify
    // set_pixel(buffer, 1, 1, &pixel); // Modify
    // set_pixel(buffer, 1, 2, &pixel); // Modify
    // set_pixel(buffer, 1, 3, &pixel); // Modify
    // set_pixel(buffer, 1, 4, &pixel); // Modify


    char *filename = "Test_9.txt";
    unsigned int pixel_cnt = width * height;
    FILE *fp;

    /* TODO plan
     * have struct of pixels that includes row and col
     * create a list of observation, one struct per pixel
     * create clusters (will need to modify kmeans code to use only n first fields of the struct)
     * recreate picture
     * For k-means, what is MUCH cleaner is that we pass a distance metric (a function that calculates centroids and distances)
    */

    // char *filename = argv[1];
    /* Make sure you update observations_size, vector_size and k
     * accordingly to your needs
     */
    // int observations_size = atoi(argv[2]);
    // int vector_size = atoi(argv[3]);
    // int k = atoi(argv[4]);

    RGBA_Pixel *input_pixels = (RGBA_Pixel *)buffer;

    // Can't use buffer as is since buffer is made of unsigned char and we need double for k-means
    // TODO may not need need pos at all since we can just use the index in buffer and cluster_map
    RGBA_Pixel_Pos_Double *pixels = calloc(pixel_cnt, sizeof(RGBA_Pixel_Pos_Double));
    for (unsigned int i = 0; i < pixel_cnt; i++) {
        // Assign pixel values. Uses implicit casting from unsigned char to double
        pixels[i].r = input_pixels[i].r;
        pixels[i].g = input_pixels[i].g;
        pixels[i].b = input_pixels[i].b;
        pixels[i].a = input_pixels[i].a;
        // Assign position value
        pixels[i].pos = i;
    }

    printf("Observations:\n");
    print_rgba_pixels(pixels, pixel_cnt);  // TODO probably need to pass size of struct rather than vector_size
    printf("\n\n");

    int member_cnt_for_rgba_when_alpha_is_included = 4; // TODO make depend on getopt
    double **centroids = create_centroids_rgba(pixels, k, pixel_cnt, member_cnt_for_rgba_when_alpha_is_included);
    int* cluster_map = km_rgba(centroids, pixels, k, pixel_cnt, member_cnt_for_rgba_when_alpha_is_included); // TODO probably need to pass size of struct rather than vector_size

    free(pixels);


    // printf("Clusters:\n");
    // print_clusters(clusters, k, pixel_cnt, RGBA_POS_DOUBLE_LEN); // TODO probably need to pass size of struct rather than vector_size
    // printf("\n");

    // TODO free everything
    // for (int i=0 ; i<k ; ++i)
    //     free(clusters[i]);
    // free(clusters);

}

static void set_pixel(unsigned char *buffer, int row, int col, RGBA_Pixel *pixel) {
    buffer[row * stride + col * RGBA_LEN + offsetof(RGBA_Pixel, r)] = pixel->r;
    buffer[row * stride + col * RGBA_LEN + offsetof(RGBA_Pixel, g)] = pixel->g;
    buffer[row * stride + col * RGBA_LEN + offsetof(RGBA_Pixel, b)] = pixel->b;
    buffer[row * stride + col * RGBA_LEN + offsetof(RGBA_Pixel, a)] = pixel->a;
}