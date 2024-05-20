//
// Created by hugo on 5/20/2024.
//

#include <cstdio>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <cstring>

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
    char type[5];
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

        std::cout << std::endl << "Chunk data_size: " << chunk_header->data_size << std::endl;
        std::cout << "Chunk type: " << chunk_header->type << std::endl;


        // Grab data
        // TODO refactor into a select statement
        if (strcmp(chunk_header->type, "IHDR") == 0) {
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
            // I'll have to generalize that for other possible color types
            assert(data_ihdr->color_type == 6);

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
            std::cout << "reached IDAT" << std::endl;
            // TODO
            //        chunk->standard.data_content =static_cast<char *>(calloc(1, chunk_header->data_size));
            //        for(size_t i = 0; i < chunk_header->data_size; i++) {
            //            std:fread(chunk->data_content + i, 1, 1, file);
            //        }
        } else if (strcmp(chunk_header->type, "sRGB") == 0) {
            char *data_content = static_cast<char *>(calloc(1, chunk_header->data_size));
            for(size_t i = 0; i < chunk_header->data_size; i++) {
                std::fread(data_content + i, 1, 1, file);
            }
            free(data_content);
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

        // Grab crc
        char crc[4];
        fread(&crc, sizeof(crc), 1, file);
        switch_endianness(crc, sizeof(crc));

        free(chunk_header);

    }
    return 0;

    // TODO: assert the following: IHDR must appear first and IEND must appear last;
}