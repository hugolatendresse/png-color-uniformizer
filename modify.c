#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "png.h"

// TODO try printing the buffer with SDL2

int main(int argc, const char **argv) {
    if (argc != 3) {
        fprintf(stderr, "expects two arguments: input file path and output file path\n");
        exit(EXIT_FAILURE);
    }

    const char *file_in = argv[1];
    const char *file_out = argv[2];
    png_image image;
    int res = 0;

    // Only the image structure version number needs to be set
    memset(&image, 0, sizeof image);
    image.version = PNG_IMAGE_VERSION;
    image.format = PNG_FORMAT_RGBA;
    // TODO allow other formats
    //  For a colormap format, a colormap will need to be supplied.

    // Read header
    res = png_image_begin_read_from_file(&image, file_in);
    if (!res) {
        fprintf(stderr, "pngtopng: %s: %s\n", argv[1], image.message);
        exit(EXIT_FAILURE);
    }

    // Create buffer for image data
    png_bytep buffer;
    buffer = malloc(PNG_IMAGE_SIZE(image));
    if (buffer == NULL) {
        fprintf(stderr, "Couldn't allocate %lu bytes for buffer\n",
                (unsigned long) PNG_IMAGE_SIZE(image));
        png_image_free(&image); // No need to free elsewhere since ligpng does cleanup on error and success
        exit(EXIT_FAILURE);
    }

    // Read image data
    png_const_colorp background = NULL;
    png_int_32 row_stride = 0;
    void *colormap = NULL;
    res = png_image_finish_read(&image, background, buffer, row_stride, colormap);
    if (!res) {
        fprintf(stderr, "Couldn't read %s: %s\n", argv[1], image.message);
        free(buffer);
        exit(EXIT_FAILURE);
    }

    // Modify image
    // TODO modify buffer here

    // Write image back
    int convert_to_8bit = 0;
    res = png_image_write_to_file(&image, file_out, convert_to_8bit, buffer, row_stride, colormap);
    if (!res) {
        fprintf(stderr, "Couldn't write %s\n", image.message);
        free(buffer);
        exit(EXIT_FAILURE);
    }
    free(buffer);
    return 0;
}
