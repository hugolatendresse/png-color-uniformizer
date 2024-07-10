/* file: pngpixel.c
 *
 * @brief
 * This file allows to read one or all pixels from a PNG file.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <stdbool.h>
#include "png.h"
#include "../include/pcu.h"

/* Return component 'c' of pixel 'x' from the given row. */
static unsigned int
component(png_const_bytep row, png_uint_32 x, unsigned int c,
          unsigned int bit_depth, unsigned int channels) {
    /* PNG images can be up to 2^31 pixels wide, but this means they can be up to
     * 2^37 bits wide (for a 64-bit pixel - the largest possible) and hence 2^34
     * bytes wide.  Since the row fitted into memory, however, the following must
     * work:
     */
    png_uint_32 bit_offset_hi = bit_depth * ((x >> 6) * channels);
    png_uint_32 bit_offset_lo = bit_depth * ((x & 0x3f) * channels + c);

    row = (png_const_bytep) (((const png_byte (*)[8]) row) + bit_offset_hi);
    row += bit_offset_lo >> 3;
    bit_offset_lo &= 0x07;

    /* PNG pixels are packed into bytes to put the first pixel in the highest
     * bits of the byte and into two bytes for 16-bit values with the high 8 bits
     * first, so:
     */
    switch (bit_depth) {
        case 1: return (row[0] >> (7 - bit_offset_lo)) & 0x01;
        case 2: return (row[0] >> (6 - bit_offset_lo)) & 0x03;
        case 4: return (row[0] >> (4 - bit_offset_lo)) & 0x0f;
        case 8: return row[0];
        case 16: return (row[0] << 8) + row[1];
        default:
            /* This should never happen; it indicates a bug in this program or in
             * libpng itself:
             */
            fprintf(stderr, "pngpixel: invalid bit depth %u\n", bit_depth);
            exit(1);
    }
}

/* Print a pixel from a row returned by libpng; determine the row format, find
 * the pixel, and print the relevant information to stdout.
 */
static void
print_and_write_pixel(png_structp png_ptr, png_infop info_ptr, png_const_bytep row,
                      png_uint_32 x, RGBA_Pixel *out, bool printflag) {
    unsigned int bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    switch (png_get_color_type(png_ptr, info_ptr)) {
        case PNG_COLOR_TYPE_GRAY:
            printf("GRAY %u\n", component(row, x, 0, bit_depth, 1));
            return;

        /* The palette case is slightly more difficult - the palette and, if
         * present, the tRNS ('transparency', though the values are really
         * opacity) data must be read to give the full picture:
         */
        case PNG_COLOR_TYPE_PALETTE: {
            int index = component(row, x, 0, bit_depth, 1);
            png_colorp palette = NULL;
            int num_palette = 0;

            if ((png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette) &
                 PNG_INFO_PLTE) && num_palette > 0 && palette != NULL) {
                png_bytep trans_alpha = NULL;
                int num_trans = 0;
                if ((png_get_tRNS(png_ptr, info_ptr, &trans_alpha, &num_trans,
                                  NULL) & PNG_INFO_tRNS) && num_trans > 0 &&
                    trans_alpha != NULL)
                    printf("INDEXED %u = %d %d %d %d\n", index,
                           palette[index].red, palette[index].green,
                           palette[index].blue,
                           index < num_trans ? trans_alpha[index] : 255);

                else /* no transparency */
                    printf("INDEXED %u = %d %d %d\n", index,
                           palette[index].red, palette[index].green,
                           palette[index].blue);
            } else
                printf("INDEXED %u = invalid index\n", index);
        }
            return;

        case PNG_COLOR_TYPE_RGB:
            printf("RGB %u %u %u\n", component(row, x, 0, bit_depth, 3),
                   component(row, x, 1, bit_depth, 3),
                   component(row, x, 2, bit_depth, 3));
            return;

        case PNG_COLOR_TYPE_GRAY_ALPHA:
            printf("GRAY+ALPHA %u %u\n", component(row, x, 0, bit_depth, 2),
                   component(row, x, 1, bit_depth, 2));
            return;

        case PNG_COLOR_TYPE_RGB_ALPHA:
            out->r = component(row, x, 0, bit_depth, 4);
            out->g = component(row, x, 1, bit_depth, 4);
            out->b = component(row, x, 2, bit_depth, 4);
            out->a = component(row, x, 3, bit_depth, 4);

            if (printflag) {
                printf("RGBA %u %u %u %u\n", component(row, x, 0, bit_depth, 4),
                       component(row, x, 1, bit_depth, 4),
                       component(row, x, 2, bit_depth, 4),
                       component(row, x, 3, bit_depth, 4));
            }
            return;

        default:
            png_error(png_ptr, "pngpixel: invalid color type");
    }
}

int get_one_pixel(long pixelrow, long pixelcol, char *filename, RGBA_Pixel *out, bool printflag) {
    /* This program uses the default, <setjmp.h> based, libpng error handling
     * mechanism, therefore any local variable that exists before the call to
     * setjmp and is changed after the call to setjmp returns successfully must
     * be declared with 'volatile' to ensure that their values don't get
     * destroyed by longjmp:
     */
    volatile int result = 1/*fail*/;

    FILE *f = fopen(filename, "rb");
    volatile png_bytep row = NULL;

    if (f != NULL) {
        /* libpng requires a callback function for handling errors; this
         * callback must not return.  The default callback function uses a
         * stored <setjmp.h> style jmp_buf which is held in a png_struct and
         * writes error messages to stderr.  Creating the png_struct is a
         * little tricky; just copy the following code.
         */
        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                     NULL, NULL, NULL);

        if (png_ptr != NULL) {
            png_infop info_ptr = png_create_info_struct(png_ptr);

            if (info_ptr != NULL) {
                /* Declare stack variables to hold pointers to locally allocated
                 * data.
                 */

                /* Initialize the error control buffer: */
                if (setjmp(png_jmpbuf(png_ptr)) == 0) {
                    png_uint_32 width, height;
                    int bit_depth, color_type, interlace_method,
                            compression_method, filter_method;
                    png_bytep row_tmp;

                    /* Now associate the recently opened (FILE*) with the default
                     * libpng initialization functions.  Sometimes libpng is
                     * compiled without stdio support (it can be difficult to do
                     * in some environments); in that case you will have to write
                     * your own read callback to read data from the (FILE*).
                     */
                    png_init_io(png_ptr, f);

                    /* And read the first part of the PNG file - the header and
                     * all the information up to the first pixel.
                     */
                    png_read_info(png_ptr, info_ptr);

                    /* This fills in enough information to tell us the width of
                     * each row in bytes, allocate the appropriate amount of
                     * space.  In this case png_malloc is used - it will not
                     * return if memory isn't available.
                     */
                    row = png_malloc(png_ptr, png_get_rowbytes(png_ptr,
                                                               info_ptr));

                    /* To avoid the overhead of using a volatile auto copy row_tmp
                     * to a local here - just use row for the png_free below.
                     */
                    row_tmp = row;

                    /* All the information we need is in the header is returned by
                     * png_get_IHDR, if this fails we can now use 'png_error' to
                     * signal the error and return control to the setjmp above.
                     */
                    if (png_get_IHDR(png_ptr, info_ptr, &width, &height,
                                     &bit_depth, &color_type, &interlace_method,
                                     &compression_method, &filter_method)) {
                        int passes, pass;

                        /* png_set_interlace_handling returns the number of
                         * passes required as well as turning on libpng's
                         * handling, but since we do it ourselves this is
                         * necessary:
                         */
                        switch (interlace_method) {
                            case PNG_INTERLACE_NONE:
                                passes = 1;
                                break;

                            case PNG_INTERLACE_ADAM7:
                                passes = PNG_INTERLACE_ADAM7_PASSES;
                                break;

                            default:
                                png_error(png_ptr, "pngpixel: unknown interlace");
                        }

                        /* Now read the pixels, pass-by-pass, row-by-row: */
                        png_start_read_image(png_ptr);

                        for (pass = 0; pass < passes; ++pass) {
                            png_uint_32 ystart, xstart, ystep, xstep;
                            png_uint_32 py;

                            if (interlace_method == PNG_INTERLACE_ADAM7) {
                                /* Sometimes the whole pass is empty because the
                                 * image is too narrow or too short.  libpng
                                 * expects to be called for each row that is
                                 * present in the pass, so it may be necessary to
                                 * skip the loop below (over py) if the image is
                                 * too narrow.
                                 */
                                if (PNG_PASS_COLS(width, pass) == 0)
                                    continue;

                                /* We need the starting pixel and the offset
                                 * between each pixel in this pass; use the macros
                                 * in png.h:
                                 */
                                xstart = PNG_PASS_START_COL(pass);
                                ystart = PNG_PASS_START_ROW(pass);
                                xstep = PNG_PASS_COL_OFFSET(pass);
                                ystep = PNG_PASS_ROW_OFFSET(pass);
                            } else {
                                ystart = xstart = 0;
                                ystep = xstep = 1;
                            }

                            /* To find the pixel, loop over 'py' for each pass
                             * reading a row and then checking to see if it
                             * contains the pixel.
                             */
                            for (py = ystart; py < height; py += ystep) {
                                png_uint_32 px, ppx;

                                /* png_read_row takes two pointers.  When libpng
                                 * handles the interlace the first is filled in
                                 * pixel-by-pixel, and the second receives the same
                                 * pixels but they are replicated across the
                                 * unwritten pixels so far for each pass.  When we
                                 * do the interlace, however, they just contain
                                 * the pixels from the interlace pass - giving
                                 * both is wasteful and pointless, so we pass a
                                 * NULL pointer.
                                 */
                                png_read_row(png_ptr, row_tmp, NULL);

                                /* Now find the pixel if it is in this row; there
                                 * are, of course, much better ways of doing this
                                 * than using a for loop:
                                 */
                                if (pixelrow == py)
                                    for (px = xstart, ppx = 0;
                                         px < width; px += xstep, ++ppx)
                                        if (pixelcol == px) {
                                            /* 'ppx' is the index of the pixel in the row
                                             * buffer.
                                             */
                                            print_and_write_pixel(png_ptr, info_ptr, row_tmp, ppx, out, printflag);

                                            /* Now terminate the loops early - we have
                                             * found and handled the required data.
                                             */
                                            goto pass_loop_end;
                                        } /* x loop */
                            } /* y loop */
                        } /* pass loop */

                        /* Finally free the temporary buffer: */
                    pass_loop_end:
                        row = NULL;
                        png_free(png_ptr, row_tmp);
                    } else
                        png_error(png_ptr, "pngpixel: png_get_IHDR failed");
                } else {
                    /* Else libpng has raised an error.  An error message has
                     * already been output, so it is only necessary to clean up
                     * locally allocated data:
                     */
                    if (row != NULL) {
                        /* The default implementation of png_free never errors out
                         * (it just crashes if something goes wrong), but the safe
                         * way of using it is still to clear 'row' before calling
                         * png_free:
                         */
                        png_bytep row_tmp = row;
                        row = NULL;
                        png_free(png_ptr, row_tmp);
                    }
                }

                png_destroy_info_struct(png_ptr, &info_ptr);
            } else {
                fprintf(stderr, "pngpixel: out of memory allocating png_info\n");
            }
            png_destroy_read_struct(&png_ptr, NULL, NULL);
        } else {
            fprintf(stderr, "pngpixel: out of memory allocating png_struct\n");
        }
    } else {
        fprintf(stderr, "pngpixel: %s: could not open file\n", filename);
    }
    return result;
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        // Save all pixels to csv
        char *filename = argv[1];
        long width = 128; // TODO parametrize
        long height = 128; // TODO parametrize

        FILE *outfile = fopen("output.csv", "w");

        for (int i=0; i<128; i++) {
            fprintf(outfile, "%d,",i);
        }

        for (long row = 0; row < height; row++) {
            for (long col = 0; col < width; col++) {
                RGBA_Pixel *out = malloc(sizeof(RGBA_Pixel));
                bool printflag = false;
                // TODO: reading file every time right now. Decompose get_one_pixel and read file once
                get_one_pixel(row, col, filename, out, printflag);
                // fprintf(outfile, "%d,%d,%d,%d\n", out->r, out->g, out->b, out->a);
                fprintf(outfile, "%hhu,", out->a);
            }
            fprintf(outfile, "\n");
        }
    }
    else if (argc == 4) {
        char *filename = argv[1];
        long pixelrow = atol(argv[2]);
        long pixelcol = atol(argv[3]);
        RGBA_Pixel *out = malloc(sizeof(RGBA_Pixel));
        bool printflag = true;
        get_one_pixel(pixelrow, pixelcol, filename, out, printflag);
    } else {
        fprintf(stderr, "pngpixel: usage: pngpixel png-file row col\n");
    }

    return 0;
}
