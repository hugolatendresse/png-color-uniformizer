#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "png.h"
#include "uniformizer.h"

// TODO try printing the buffer with SDL2


int transform(png_bytep buffer, png_uint_32 height, png_uint_32 width, png_int_32 row_stride, int format) {
    // TODO implement transformation
    return 1;
}


int main(int argc, const char **argv) {
    if (argc != 3) {
        fprintf(stderr, "expects two arguments: input file path and output file path\n");
        exit(EXIT_FAILURE);
    }

    const char *file_in = argv[1];
    const char *file_out = argv[2];
    png_image image;

    // Only the image structure version number needs to be set
    memset(&image, 0, sizeof image);
    image.version = PNG_IMAGE_VERSION;
    image.format = PNG_FORMAT_RGBA;
    // TODO allow other formats
    //  For a colormap format, a colormap will need to be supplied.

    // Read header
    int res = png_image_begin_read_from_file(&image, file_in);
    if (!res) {
        fprintf(stderr, "pngtopng: %s: %s\n", argv[1], image.message);
        exit(EXIT_FAILURE);
    }

    // Create buffer for image data
    png_bytep idat_data;
    idat_data = malloc(PNG_IMAGE_SIZE(image)); // Pointer to uncompressed IDAT data
    if (idat_data == NULL) {
        fprintf(stderr, "Couldn't allocate %lu bytes for buffer\n",
                (unsigned long) PNG_IMAGE_SIZE(image));
        png_image_free(&image); // No need to free elsewhere since ligpng does cleanup on error and success
        exit(EXIT_FAILURE);
    }

    // Read image data
    png_const_colorp background = NULL;
    png_int_32 row_stride = 0;
    void *colormap = NULL;
    res = png_image_finish_read(&image, background, idat_data, row_stride, colormap);
    if (!res) {
        fprintf(stderr, "Couldn't read %s: %s\n", argv[1], image.message);
        free(idat_data);
        exit(EXIT_FAILURE);
    }

    // Modify image
    res = transform(idat_data, image.height, image.width,  row_stride, image.format);
    if (!res) {
        fprintf(stderr, "Error while transforming image\n");
        free(idat_data);
        exit(EXIT_FAILURE);
    }

    // Print the image
    display_image(image.height, image.width, idat_data);

    // Write image back
    int convert_to_8bit = 0;
    res = png_image_write_to_file(&image, file_out, convert_to_8bit, idat_data, row_stride, colormap);
    if (!res) {
        fprintf(stderr, "Couldn't write %s\n", image.message);
        free(idat_data);
        exit(EXIT_FAILURE);
    }
    free(idat_data);
    return 0;
}
