//
// Created by hugo on 5/20/2024.
//

#include <cstdio>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <cstring>

// TODO ok to iterate twice, but now need to implement the IDAT logic (row 139)


#define SIGNATURE_LENGTH 8

bool is_png(unsigned char *signature) {
    unsigned char expected_signature[] = {137, 80, 78, 71, 13, 10, 26, 10};
    if (!std::equal(signature, signature + SIGNATURE_LENGTH, expected_signature)){
        perror("This is not a PNG file!");
        return false;
    }
    return true;
}


typedef struct ChunkHeader_ {
    unsigned int data_size;
    char type[4 + 1];  // 4 character type
} ChunkHeader;


// TODO can probably delete
typedef struct ChunkStandard_ {
    ChunkHeader *header;
    char *data_content;
    unsigned int crc;
} ChunkStandard;

typedef struct DataIHDR_ {
//    ChunkHeader *header;  // TODO can probably delete
    int width;
    int height;
    char bit_depth;
    char color_type;
    char compress_method;
    char filter_method;
    char interlace_method;
//    unsigned int crc;  // TODO can probably delete
} DataIHDR;



void switch_endianness(char *src, char *dest, int byte_cnt) {
    assert(src != dest);
    for (size_t i = 0; i < byte_cnt; i++) {
        dest[i] = src[byte_cnt - 1 - i];
    }
}

// Todo make this a method in a class
void switch_endianness(char *p, int byte_cnt) {
    char buffer[byte_cnt];
    switch_endianness(p, buffer, byte_cnt);
    memcpy(p, buffer, byte_cnt);
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
        // TODO refactor into a select statement
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
            Width and height give the image dim
            */
            DataIHDR *data_ihdr = static_cast<DataIHDR *>(malloc(sizeof(DataIHDR)));
            std::fread(&(data_ihdr->width), sizeof(data_ihdr->width), 1, file);
            switch_endianness((char *)&(data_ihdr->width), sizeof(data_ihdr->width));
            std::fread(&(data_ihdr->height), sizeof(data_ihdr->height), 1, file);
            switch_endianness((char *) &(data_ihdr->height), sizeof(data_ihdr->height));
            std::fread(&(data_ihdr->bit_depth), sizeof(data_ihdr->bit_depth), 1, file);
            std::fread(&(data_ihdr->color_type), sizeof(data_ihdr->color_type), 1, file);
            std::fread(&(data_ihdr->compress_method), sizeof(data_ihdr->compress_method), 1, file);
            std::fread(&(data_ihdr->filter_method), sizeof(data_ihdr->filter_method), 1, file);
            std::fread(&(data_ihdr->interlace_method), sizeof(data_ihdr->interlace_method), 1, file);
            std::cout << "width: " << data_ihdr->width << std::endl;
            std::cout << "height: " << data_ihdr->height << std::endl;
            std::cout << "bit_depth: " << (int) (data_ihdr->bit_depth) << std::endl;
            std::cout << "color_type: " << (int) (data_ihdr->color_type) << std::endl;
            std::cout << "compress_method: " << (int) (data_ihdr->compress_method) << std::endl;
            std::cout << "filter_method: " << (int) (data_ihdr->filter_method) << std::endl;
            std::cout << "interlace_method: " << (int) (data_ihdr->interlace_method) << std::endl;

            // In my case, I have color_type == 6, which means Each pixel is an R,G,B triple, followed by
            // an alpha sample.
            // TODO generalize that for other possible color types
            assert(data_ihdr->color_type == 6);  // TODO remove for generic tool

            // Only compression method currently defined for png is 0 (deflate/inflat compression)
            assert(data_ihdr->compress_method == 0);

            // Only filter method currently defined for png is 0 (adaptive filtering with five basic filter types)
            assert(data_ihdr->filter_method == 0);

            free(data_ihdr);
        } else if (strcmp(chunk_header->type, "PLTE") == 0) {
            std::cout << "reached PLTE" << std::endl;
            // TODO
            //        chunk->standard.data_content =static_cast<char *>(calloc(1, chunk_header->data_size));
            //        for(size_t i = 0; i < chunk_header->data_size; i++) {
            //            std:fread(chunk->data_content + i, 1, 1, file);
            //        }
        } else if (strcmp(chunk_header->type, "IDAT") == 0) {
            /*
            The IDAT chunk contains the actual image data. To create this data:

            Begin with image scanlines represented as described in Image layout; the layout and total size of this raw data are determined by the fields of IHDR.
                    Filter the image data according to the filtering method specified by the IHDR chunk. (Note that with filter method 0, the only one currently defined, this implies prepending a filter-type byte to each scanline.)
            Compress the filtered data using the compression method specified by the IHDR chunk.
                    The IDAT chunk contains the output datastream of the compression algorithm.

                    To read the image data, reverse this process.

                    There can be multiple IDAT chunks; if so, they must appear consecutively with no other intervening chunks. The compressed datastream is then the concatenation of the contents of all the IDAT chunks. The encoder can divide the compressed datastream into IDAT chunks however it wishes. (Multiple IDAT chunks are allowed so that encoders can work in a fixed amount of memory; typically the chunk size will correspond to the encoder's buffer size.) It is important to emphasize that IDAT chunk boundaries have no semantic significance and can occur at any point in the compressed datastream. A PNG file in which each IDAT chunk contains only one data byte is valid, though remarkably wasteful of space. (For that matter, zero-length IDAT chunks are valid, though even more wasteful.)

            See Filter Algorithms and Deflate/Inflate Compression for details.
            */


            std::cout << "reached IDAT" << std::endl;
            // TODO
            //        chunk->standard.data_content =static_cast<char *>(calloc(1, chunk_header->data_size));
            //        for(size_t i = 0; i < chunk_header->data_size; i++) {
            //            std:fread(chunk->data_content + i, 1, 1, file);
            //        }
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
            // TODO for my benefit, compare different factorization of ElementSize * Count in fread
            std::cout << "Rendering Intent: ";
            switch(rendering_intent[0]) {
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
            // TODO
            //        }
        } else if (strcmp(chunk_header->type, "gAMA") == 0) {
            std::cout << "reached gAMA" << std::endl;
            // TODO
            //        }
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
    return 0;

    // TODO: assert the following: IHDR must appear first and IEND must appear last;
}