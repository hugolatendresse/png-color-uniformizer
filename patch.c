/* file: main.c
 *
 * @brief
 * Reads, modifies, and writes a PNG
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "png.h"
#include "include/pcu.h"
#include <getopt.h>
#include <stdatomic.h>
#include <time.h>

// Will use k-modes by default and switch to k-means if --kmeans is passed
bool kmodes = true;
bool change_alpha = false;
unsigned int seed;
RGBAPixelDouble *forced_pixel = NULL;

void helper() {
    const char *m =
            "Usage: ./pcu file_in.png file_out.png [-c <c>] [--kmeans]\n"
            "       ./pcu file_in.png file_out.png -r <r> -g <g> -b <b> -a <a> [-c <c>] [--kmeans]\n"
            "\n"
            "   file_in Path to input file (the PNG image to change)\n"
            "   file_out Path to output file (where to save)\n"
            "   -c Number of groups for k-modes algorithm. Default is 2\n"
            "   -r RGBA value for Red\n"
            "   -g RGBA value for Green\n"
            "   -b RGBA value for Blue\n"
            "   -a RGBA value for Alpha\n"
            "   -d To display image once its transformed\n"
            "   -s set the seed. Picked at random if not provided\n"
            "   --kmeans will use k-means instead of k-modes"
            "   --alpha will treat alpha like other color components and change it. The default behavior is to keep alpha the same for all pixels.\n"
            "   -h Display this message. Other arguments are ignored\n"
            "\n"
            "If nothing is supplied, all pixels will be changed to the most common color\n" //TODO
            "Example:\n"
            "./pcu icon.png icon_new.png\n"
            "\n"
            "If only RGBA value are supplied, all pixels will be changed to that color\n" //TODO
            "Example, k-means with 2 colors:\n"
            "./pcu icon.png icon_new.png -c 2 --kmeans\n"
            "Example, k-modes (the default), specifying of the three colors to use:\n"
            "./pcu icon.png icon_new.png -c 3 -r 255 -g 100 -b 0 -a 255\n"
            "\n"
            "If only a value for c is supplied, k-modes algorithms will be used to restrict the "
            "colors \n" //TODO
            "Example:\n"
            "./pcu icon.png icon_new.png -c 3 0\n"
            "\n"
            "If both a value for c and RGBA value are supplied, the RGBA color will be used as "
            "well as c-1 colors coming from k-modes algorithm\n" //TODO
            "Example:\n"
            "./pcu icon.png icon_new.png -c 4 -r 255 -g 100 -b 0 -a 255\n";
    printf("%s\n", m);
}

int main(int argc, const char **argv) {
    const char *file_in = argv[1];
    const char *file_out = argv[2];

    // Only the image structure version number needs to be set
    png_image image;
    memset(&image, 0, sizeof image);
    image.version = PNG_IMAGE_VERSION;
    // TODO allow other formats. For a colormap format, a colormap will need to be supplied.

    // Read header
    int res = png_image_begin_read_from_file(&image, file_in);
    if (!res) {
        fprintf(stderr, "%s: %s\n", argv[1], image.message);
        exit(EXIT_FAILURE);
    } else {
        printf("Reading %s\n", file_in);
    }

    if (kmodes) {
        dbg_printf("Using kmodes algorithm\n");
    } else {
        dbg_printf("Using kmeans algorithm\n");
    }

    if (change_alpha) {
        dbg_printf("Will change alpha, like other colors\n");
    } else {
        dbg_printf("Changing only RGB (not alpha)\n");
    }

    // Create buffer for image data
    png_bytep idat_data;
    image.format = PNG_FORMAT_RGBA;
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
    res = patch(idat_data, image.height, image.width, row_stride, image.format);
    if (!res) {
        fprintf(stderr, "Error while transforming image\n");
        free(idat_data);
        exit(EXIT_FAILURE);
    }

    // Show the image on screen
    if (true) {
        display_image(image.height, image.width, idat_data);
    }

    // Write resulting image to file
    int convert_to_8bit = 0;
    res = png_image_write_to_file(&image, file_out, convert_to_8bit, idat_data, row_stride, colormap);
    if (!res) {
        fprintf(stderr, "Couldn't write %s\n", image.message);
        free(idat_data);
        exit(EXIT_FAILURE);
    } else {
        printf("Saved %s\n", file_out);
    }
    free(idat_data);
    free(forced_pixel);
    return 0;
}
