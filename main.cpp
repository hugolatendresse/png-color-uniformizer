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


typedef union Chunk_ {
    ChunkHeader *header;
    struct {
        char *data_content;
    } standard;
    struct {
        int width;
        int height;
        char bit_depth;
        char color_type;
        char compress_method;
        char filter_method;
        char interlace_method;
    } ihdr;
    unsigned int crc;
} Chunk;


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

    // Read first chunk_header
    ChunkHeader *chunk_header = static_cast<ChunkHeader *>(malloc(sizeof(ChunkHeader)));
    std::fread(chunk_header, sizeof(ChunkHeader), 1, file);
    chunk_header->type[4] = '\0';
    switch_endianness((char *)(&(chunk_header->data_size)), (int)sizeof
    (chunk_header->data_size));

    std::cout << "Chunk data_size: " << chunk_header->data_size << std::endl;
    std::cout << "Chunk type: " << chunk_header->type << std::endl;

    // Create first chunk
    Chunk *chunk = static_cast<Chunk *>(malloc(sizeof(Chunk)));
    chunk->header = chunk_header;
    if (strcmp(chunk->header->type, "IHDR") == 0) {
        std::fread(&(chunk->ihdr), sizeof(chunk->ihdr), 1, file);
        std::cout << "width: " << chunk->ihdr.width << std::endl;
        std::cout << "height: " << chunk->ihdr.height << std::endl;
        std::cout << "bit_depth: " << chunk->ihdr.bit_depth << std::endl;
        std::cout << "color_type: " << chunk->ihdr.color_type << std::endl;
        std::cout << "compress_method: " << chunk->ihdr.compress_method << std::endl;
        std::cout << "filter_method: " << chunk->ihdr.filter_method << std::endl;
        std::cout << "interlace_method: " << chunk->ihdr.interlace_method << std::endl;
    } else if (strcmp(chunk->header->type, "IDAT") == 0) {
        // TODO
    } else if (strcmp(chunk->header->type, "IEND") == 0) {
        // TODO
    }



    chunk->data_content =static_cast<char *>(calloc(1, chunk->header->data_size));

    for(size_t i = 0; i < chunk->header->data_size; i++) {
        std:fread(chunk->data_content + i, 1, 1, file);
    }




    free(chunk_header);
    free(chunk);
    printf("the end");
    return 0;

    // TODO: assert the following: IHDR must appear first and IEND must appear last;
}