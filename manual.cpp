//
// Created by hugo on 5/20/2024.
//

#include <cstdio>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <cstring>
#include <zlib.h>
#include <vector>
#include "uniformizer.h"
#include <SDL.h>

#define SIGNATURE_LENGTH 8

/*Globals*/
char color_type = -1;
char compression_method = -1;
char filtering_method = -1;
char interlacing_method = -1;
std::vector<unsigned char> compressed_idat;

bool is_png(unsigned char *signature) {
    unsigned char expected_signature[] = {137, 80, 78, 71, 13, 10, 26, 10};
    if (!std::equal(signature, signature + SIGNATURE_LENGTH, expected_signature)) {
        perror("This is not a PNG file!");
        return false;
    }
    return true;
}

typedef struct ChunkHeader_ {
    unsigned int data_size;
    char type[4 + 1]; // 4 character type
} ChunkHeader;

typedef struct DataIHDR_ {
    int width;
    int height;
    char bit_depth;
    char color_type;
    char compress_method;
    char filter_method;
    char interlace_method;
} DataIHDR;

void switch_endianness(char *src, char *dest, int byte_cnt) {
    assert(src != dest);
    for (size_t i = 0; i < byte_cnt; i++) {
        dest[i] = src[byte_cnt - 1 - i];
    }
}

void switch_endianness(char *p, int byte_cnt) {
    char buffer[byte_cnt];
    switch_endianness(p, buffer, byte_cnt);
    memcpy(p, buffer, byte_cnt);
}

void print_rgb_info() {
    std::cout << "Pixel is represented by three samples: red (zero = black, max = red) "
            "appears first, then green (zero = black, max = green), then blue "
            "(zero = black, max = blue). The bit depth specifies the size of each "
            "sample, not the total pixel size" << std::endl;
}

void print_alpha_info() {
    std::cout << "An alpha value of zero represents full transparency, and a value of "
            "2^bitdepth represents a fully opaque pixel. " << std::endl;
}

void set_color_type(char color_type_input) {
    color_type = color_type_input;
    std::cout << std::endl << "Color type " << static_cast<int>(color_type) << " info:" <<
            std::endl;
    switch (color_type) {
        case 0:
            std::cout << "Each pixel is a grayscale sample." << std::endl;
            break;
        case 2:
            std::cout << "Each pixel is an R,G,B triple." << std::endl;
            print_rgb_info();
            break;
        case 3:
            std::cout << "Each pixel is a palette index; a PLTE chunk must appear." << std::endl;
            break;
        case 4:
            std::cout << "Each pixel is a grayscale sample, followed by an alpha sample." << std::endl;
            print_alpha_info();
            break;
        case 6:
            std::cout << "Each pixel is an R,G,B triple, followed by an alpha sample." << std::endl;
            print_rgb_info();
            print_alpha_info();
            break;
    }
}

void set_compression_method(char compression_input) {
    compression_method = compression_input;
    std::cout << std::endl << "Compression type " << static_cast<int>(compression_method)
            << " info:" << std::endl;
    switch (compression_method) {
        case 0:
            std::cout << "Deflate/inflate compression with a sliding window of at most 32768 "
                    "bytes" << std::endl;
            break;
        default:
            std::cerr << "Unknown compression type. PNG supports only method 0." << std::endl;
            exit(EXIT_FAILURE);
            break;
    }
}

void set_filtering_method(char filtering_input) {
    filtering_method = filtering_input;
    std::cout << std::endl << "Filtering type " << static_cast<int>(filtering_method) << " info:"
            << std::endl;
    switch (filtering_method) {
        case 0:
            std::cout << "None() filter, the scanline is transmitted unmodified;\n"
                    "It is necessary only to insert a filter-type byte before the data." <<
                    std::endl;
            break;
        default:
            std::cerr << "Filtering method not yet supported by this tool" << std::endl;
            exit(EXIT_FAILURE);
    }
}

void set_interlacing_method(char interlacing_input) {
    interlacing_method = interlacing_input;
    std::cout << std::endl << "Interlacing type " << static_cast<int>(interlacing_method)
            << " info:" << std::endl;
    switch (interlacing_method) {
        case 0:
            std::cout << "With interlace method 0, pixels are stored sequentially from left to "
                    "right, and scanlines sequentially from top to bottom (no interlacing)." <<
                    std::endl;
            break;
        default:
            std::cerr << "Interlacing method not yet supported by this tool" << std::endl;
            exit(EXIT_FAILURE);
    }
}

// Passing compressed_data by reference. Avoids having to copy. Still can't modify the data since
// it's const
std::vector<unsigned char> decompress_idat(const std::vector<unsigned char> &compressed, DataIHDR *ihdr) {
    z_stream strm = {};
    strm.total_in = strm.avail_in = compressed.size();
    strm.next_in = (Bytef *) compressed.data();

    // Initialize the output buffer with a guess of the size of the decompressed data
    std::vector<unsigned char> decompressed(ihdr->height * (ihdr->width * 4 + 1)); // Assumes RGBA image

    // Initialize the zlib decompression stream
    inflateInit(&strm);
    // inflateInit2(&strm, 0);

    int ret;
    do {
        strm.avail_out = decompressed.size() - strm.total_out;
        strm.next_out = (Bytef *) (decompressed.data() + strm.total_out);

        ret = inflate(&strm, Z_NO_FLUSH);
        assert(ret != Z_STREAM_ERROR); // State not clobbered

        switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR; // And fall through
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void) inflateEnd(&strm);
                throw std::runtime_error("Zlib decompression error");
        }

        if (strm.avail_out == 0) {
            // Output buffer was too small, increase the buffer size
            decompressed.resize(decompressed.size() * 2);
        }
    } while (ret != Z_STREAM_END);

    // Clean up and calculate the final size
    inflateEnd(&strm);
    decompressed.resize(strm.total_out);

    return decompressed;
}


int main(int argc, char *argv[]) {
    const char *path;
    if (argc >= 2) {
        path = argv[1];
    } else {
        path = "icon.png";
    }

    FILE *file = std::fopen(path, "rb");
    if (file == nullptr) {
        std::perror("Could not open file");
        return false;
    }

    // Read signature
    unsigned char signature[SIGNATURE_LENGTH];
    std::fread(signature, 1, SIGNATURE_LENGTH, file);
    std::cout << "It is a png: " << (is_png(signature)) << std::endl;

    DataIHDR *ihdr = static_cast<DataIHDR *>(malloc(sizeof(DataIHDR)));

    while (!std::feof(file)) {
        // Grab header of chunk
        ChunkHeader *chunk_header = static_cast<ChunkHeader *>(malloc(sizeof(ChunkHeader)));
        std::fread(&(chunk_header->data_size), sizeof(chunk_header->data_size), 1, file);
        std::fread(&(chunk_header->type), sizeof(chunk_header->type) - 1, 1, file);
        chunk_header->type[4] = '\0';
        switch_endianness((char *) (&(chunk_header->data_size)), (int) sizeof
                          (chunk_header->data_size));

        std::cout << std::endl << "Chunk type: " << chunk_header->type << std::endl;
        std::cout << "Chunk data_size: " << chunk_header->data_size << std::endl;

        // Grab data
        if (strcmp(chunk_header->type, "IHDR") == 0) {
            /*
            The IHDR chunk must appear FIRST. It contains:

            Width:              4 bytes
            Height:             4 bytes
            Bit depth:          1 byte
            Color type:         1 byte
            Compression method: 1 byte
            Filter method:      1 byte
            Interlace method:   1 byte
            */
            std::fread(&(ihdr->width), sizeof(ihdr->width), 1, file);
            switch_endianness((char *) &(ihdr->width), sizeof(ihdr->width));
            std::fread(&(ihdr->height), sizeof(ihdr->height), 1, file);
            switch_endianness((char *) &(ihdr->height), sizeof(ihdr->height));
            std::fread(&(ihdr->bit_depth), sizeof(ihdr->bit_depth), 1, file);
            std::fread(&(ihdr->color_type), sizeof(ihdr->color_type), 1, file);
            std::fread(&(ihdr->compress_method), sizeof(ihdr->compress_method), 1, file);
            std::fread(&(ihdr->filter_method), sizeof(ihdr->filter_method), 1, file);
            std::fread(&(ihdr->interlace_method), sizeof(ihdr->interlace_method), 1, file);
            std::cout << "width: " << ihdr->width << std::endl;
            std::cout << "height: " << ihdr->height << std::endl;
            std::cout << "bit_depth: " << (int) (ihdr->bit_depth) << std::endl;
            std::cout << "color_type: " << (int) (ihdr->color_type) << std::endl;
            std::cout << "compress_method: " << (int) (ihdr->compress_method) << std::endl;
            std::cout << "filter_method: " << (int) (ihdr->filter_method) << std::endl;
            std::cout << "interlace_method: " << (int) (ihdr->interlace_method) << std::endl;

            set_color_type(ihdr->color_type);
            set_compression_method(ihdr->compress_method);
            set_filtering_method(ihdr->filter_method);
            set_interlacing_method(ihdr->interlace_method);

            // In my case, I have color_type == 6, which means Each pixel is an R,G,B triple, followed by
            // an alpha sample.
            // TODO generalize that for other possible color types
            assert(ihdr->color_type == 6);

            // Only compression method currently defined for png is 0 (deflate/inflat compression)
            assert(ihdr->compress_method == 0);

            // Only filter method currently defined for png is 0 (adaptive filtering with five basic filter types)
            assert(ihdr->filter_method == 0);
        } else if (strcmp(chunk_header->type, "PLTE") == 0) {
            std::cout << "reached PLTE" << std::endl;
        } else if (strcmp(chunk_header->type, "IDAT") == 0) {
            /*
            The IDAT chunk contains the actual image data. To create this data:

            Begin with image scanlines represented as described in Image layout; the layout and total
             size of this raw data are determined by the fields of IHDR.
            Filter the image data according to the filtering method specified by the IHDR chunk.
            (Note that with filter method 0, the only one currently defined, this implies
            prepending a filter-type byte to each scanline.)
            Compress the filtered data using the compression method specified by the IHDR chunk.
            The IDAT chunk contains the output datastream of the compression algorithm.

            To read the image data, reverse this process.

            There can be multiple IDAT chunks; if so, they must appear consecutively with no other intervening chunks. The compressed datastream is then the concatenation of the contents of all the IDAT chunks. The encoder can divide the compressed datastream into IDAT chunks however it wishes. (Multiple IDAT chunks are allowed so that encoders can work in a fixed amount of memory; typically the chunk size will correspond to the encoder's buffer size.) It is important to emphasize that IDAT chunk boundaries have no semantic significance and can occur at any point in the compressed datastream. A PNG file in which each IDAT chunk contains only one data byte is valid, though remarkably wasteful of space. (For that matter, zero-length IDAT chunks are valid, though even more wasteful.)
            */

            std::cout << "reached IDAT" << std::endl;
            char *data_content = static_cast<char *>(calloc(1, chunk_header->data_size));
            for (size_t i = 0; i < chunk_header->data_size; i++) {
                std::fread(data_content + i, 1, 1, file);
            }
            compressed_idat.insert(compressed_idat.end(), data_content,
                                   data_content + chunk_header->data_size);
        } else if (strcmp(chunk_header->type, "sRGB") == 0) {
            /*
            The sRGB chunk contains:

            Rendering intent: 1 byte
            The following values are defined for the rendering intent:

            0: Perceptual
            1: Relative colorimetric
            2: Saturation
            3: Absolute colorimetric
            */
            char rendering_intent[1];
            std::fread(rendering_intent, 1, 1, file);
            std::cout << "Rendering Intent: ";
            switch (rendering_intent[0]) {
                case 0:
                    /*
                    Perceptual intent is for images preferring good adaptation to the output device
                     gamut at the expense of colorimetric accuracy, like photographs.
                    */
                    std::cout << "Perceptual" << std::endl;
                    break;
                case 1:
                    /*
                    Relative colorimetric intent is for images requiring color appearance matching
                     (relative to the output device white point), like logos.
                    */
                    std::cout << "Relative colorimetric" << std::endl;
                case 2:
                    /*
                    Saturation intent is for images preferring preservation of saturation at the
                     expense of hue and lightness, like charts and graphs.
                    */
                    std::cout << "Saturation" << std::endl;
                case 3:
                    /*
                    Absolute colorimetric intent is for images requiring preservation of absolute
                     colorimetry, like proofs (previews of images destined for a different output
                     device).
                    */
                    std::cout << "Absolute colorimetric" << std::endl;
                default:
                    std::cerr << "Unsupported Rendering Intent - Exiting" << std::endl;
                    exit(EXIT_FAILURE);
            }
            std::cout << std::endl;
        } else if (strcmp(chunk_header->type, "gAMA") == 0) {
            std::cout << "reached gAMA" << std::endl;
        } else if (strcmp(chunk_header->type, "gAMA") == 0) {
            std::cout << "reached gAMA" << std::endl;
        } else if (strcmp(chunk_header->type, "IEND") == 0) {
            std::cout << "reached IEND" << std::endl;
            fclose(file);
            break;
        } else {
            std::cout << "reached unknown chunk: " << chunk_header->type << std::endl;
        }

        /*
        A 4-byte CRC (Cyclic Redundancy Check) calculated on the preceding bytes in the chunk,
         including the chunk type code and chunk data fields, but not including the length field.
         The CRC is always present, even for chunks containing no data.
        */
        char crc[4];
        fread(&crc, sizeof(crc), 1, file);
        switch_endianness(crc, sizeof(crc));

        free(chunk_header);
    }
    std::cout << "Now need to unpack the IDAT data" << std::endl;

    std::vector<unsigned char> decompressed_idat;
    try {
        decompressed_idat = decompress_idat(compressed_idat, ihdr);
        std::cout << "Decompressed successfully. Size: " << decompressed_idat.size() << " bytes" << std::endl;
    } catch (const std::runtime_error &e) {
        std::cerr << "Decompression failed: " << e.what() << std::endl;
    }

    // Ignore the filtering bytes - but do it pixel by pixel to be able to change pixels
    std::vector<unsigned char> d4;
    for (std::vector<unsigned char>::size_type row = 0; row < ihdr->height; row++) {
        for (std::vector<unsigned char>::size_type col = 1; col < (ihdr->width * 4 + 1); col += 4) {
            int stride = ihdr->width * 4 + 1;
            std::vector<unsigned char> pixel(decompressed_idat.data() + row * stride + col,
                                             decompressed_idat.data() + row * stride + col + 4);
            // R G B Alpha
            pixel[0] = (pixel[0] + 0) % 256;
            pixel[1] = (pixel[1] + 0) % 256;
            pixel[2] = (pixel[2] + 0) % 256;
            pixel[3] = (pixel[3] + 0) % 256;

            // set all to black
            // pixel[0] = 0;
            // pixel[1] = 0;
            // pixel[2] = 0;

            //swap first and last to check for endianness
            // unsigned char temp = pixel[0];
            // pixel[0] = pixel[3];
            // pixel[3] = temp;
            // //
            // unsigned char temp2 = pixel[1];
            // pixel[1] = pixel[2];
            // pixel[2] = temp2;


            // This is used to accentuate all colors
            // if (pixel[3] > 0) {
            //     pixel[3] = 255;
            // }


            //     // Force transparancy
            // if (pixel[3] == 0) {
            //     pixel[0] = UCHAR_MAX;
            //     pixel[1] = UCHAR_MAX;
            //     pixel[2] = UCHAR_MAX;
            //     pixel[3] = UCHAR_MAX;
            // }

            // // Can be used to check who has pixel 0
            // if (pixel[3] == 0) {
            //     pixel[0] = 250;
            //     pixel[1] = 10;
            //     pixel[2] = 0;
            //     pixel[3] = 0;
            // } else {
            //     // pixel[0] = 250;
            //     // pixel[1] = 10;
            //     // pixel[2] = 0;
            //     // pixel[3] = 100;
            // }


            // Invert everything
            // pixel[0] = UCHAR_MAX - pixel[0];
            // pixel[1] = UCHAR_MAX - pixel[1];
            // pixel[2] = UCHAR_MAX - pixel[2];
            // pixel[3] = UCHAR_MAX - pixel[3];

            d4.insert(d4.end(), pixel.begin(), pixel.end());
        }
    }

    std::cout << "stop here";

    display_image(ihdr->height, ihdr->width, d4.data());

    free(ihdr);
    return 0;
}
