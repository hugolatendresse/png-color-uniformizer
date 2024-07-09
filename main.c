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
#include "include/uniformizer.h"
#include "include/km.h"
#include <getopt.h>

void helper() {
    const char *m =
            "Usage: ./pcu file_in file_out -r <r> -g <g> -b <b> -a <a> [-k <k>] [--kmeans]\n"
            "       ./pcu file_in file_out -k <k> [--kmodes]\n"
            "\n"
            "   file_in Path to input file (the PNG image to change)\n"
            "   file_out Path to output file (where to save)\n"
            "   -k Number of groups for k-modes algorithm\n"
            "   -r RGBA value for Red\n"
            "   -g RGBA value for Green\n"
            "   -b RGBA value for Blue\n"
            "   -a RGBA value for Alpha\n"
            "   --kmeans will use k-means instead of k-modes"
            "   -h Display this message. Other arguments are ignored\n"
            "\n"
            "If nothing is supplied, all pixels will be changed to the most common color\n" //TODO
            "Example:\n"
            "./pcu icon.png icon_new.png\n"
            "\n"
            "If only RGBA value are supplied, all pixels will be changed to that color\n" //TODO
            "Example:\n"
            "./pcu icon.png icon_new.png -r 255 -g 100 -b 0 -a 255\n"
            "\n"
            "If only a value for k is supplied, k-modes algorithms will be used to restrict the "
            "colors \n" //TODO
            "Example:\n"
            "./pcu icon.png icon_new.png -k 3 0\n"
            "\n"
            "If both a value for k and RGBA value are supplied, the RGBA color will be used as "
            "well as k-1 colors coming from k-modes algorithm\n" //TODO
            "Example:\n"
            "./pcu icon.png icon_new.png -k 4 -r 255 -g 100 -b 0 -a 255\n";
    printf("%s\n", m);
}






void test_kmeans() {
    char *filename = "Test_9.txt";
    int observations_size = 9;
    int vector_size = 2;
    int k = 4;
    FILE *fp;

    /* TODO plan
     * have struct of pixels that includes row and col
     * create a list of observation, one struct per pixel
     * create clusters (will need to modify kmeans code to use only n first fields of the struct)
     * recreate picture
    */



    // char *filename = argv[1];
    /* Make sure you update observations_size, vector_size and k
     * accordingly to your needs
     */
    // int observations_size = atoi(argv[2]);
    // int vector_size = atoi(argv[3]);
    // int k = atoi(argv[4]);
    double **observations;
    double ***clusters;

    observations = (double **) malloc(sizeof(double *) * observations_size);
    for (int i = 0; i < observations_size; i++)
        observations[i] = (double *) malloc(sizeof(double) * vector_size);

    if ((fp = fopen(filename, "r+")) == NULL) {
        printf("No such file or directory\n");
        for (int i=0 ; i<observations_size ; ++i)
            free(observations[i]);
        free(observations);
        exit(1);
    }

    for (int i = 0; i < observations_size; i++) {
        for (int j = 0; j < vector_size; j++)
            fscanf(fp, "%lf", &observations[i][j]);
    }

    printf("Observations:\n");
    print_observations(observations, observations_size, vector_size);
    printf("\n\n");

    clusters = km(observations, k, observations_size, vector_size);
    printf("Clusters:\n");
    print_clusters(clusters, k, observations_size, vector_size);
    printf("\n");

    for (int i=0 ; i<k ; ++i)
        free(clusters[i]);
    free(clusters);

    for (int i=0 ; i<observations_size ; ++i)
        free(observations[i]);
    free(observations);
    fclose(fp);

}


int main(int argc, const char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Expects at least two arguments: input file path and output file path\n");
        exit(EXIT_FAILURE);
    }

    const char *file_in = argv[1];
    const char *file_out = argv[2];

    unsigned int k;
    unsigned char r, g, b, a;
    bool k_set = false;
    bool r_set = false;
    bool g_set = false;
    bool b_set = false;
    bool a_set = false;
    bool kmeans = false;

    // Define the long options
    static struct option long_options[] = {
            {"kmeans", no_argument, 0, 0},
            {0, 0, 0, 0} // Terminate the array with zeros
    };

    // Read all arguments and update variables declared above accordingly
    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, (char * const*)argv, "hk:r:g:b:a:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 0:
                if (strcmp(long_options[option_index].name, "kmeans") == 0) {
                    kmeans = true;
                    test_kmeans(); // TODO remove
                    exit(EXIT_SUCCESS); // TODO remove this line
                }
                break;
            case 'h':
                helper();
                exit(EXIT_SUCCESS);
                break;
            case 'k':
                k_set = true;
                k = atol(optarg);
                break;
            case 'r':
                r_set = true;
                r = atol(optarg);
                break;
            case 'g':
                g_set = true;
                g = atol(optarg);
                break;
            case 'b':
                b_set = true;
                b = atol(optarg);
                break;
            case 'a':
                a_set = true;
                a = atol(optarg);
                break;
            default:
                printf("%s\n", "Error while passing arguments");
                helper();
                exit(EXIT_FAILURE);
        }
    }

    // TODO return error if some of RGBA are set but not all

    png_image image;

    // Only the image structure version number needs to be set
    memset(&image, 0, sizeof image);
    image.version = PNG_IMAGE_VERSION;
    image.format = PNG_FORMAT_RGBA;
    // TODO allow other formats. For a colormap format, a colormap will need to be supplied.

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
    // TODO implement options k r g b a as written in helper
    res = transform(idat_data, image.height, image.width,  row_stride, image.format,
        k);
    if (!res) {
        fprintf(stderr, "Error while transforming image\n");
        free(idat_data);
        exit(EXIT_FAILURE);
    }

    // Print the image
    // TODO create an option -d to display
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
