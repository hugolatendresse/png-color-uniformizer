//
// Created by hugo on 5/20/2024.
//

#include <cstdio>
#include <algorithm>
#include <windows.h>
#include <iostream>

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
    unsigned int length;
    char type[5];
} ChunkHeader;

typedef struct Chunk_ {
    ChunkHeader *header;
    unsigned char *data;
    unsigned int crc;
} Chunk;



int main(int argc, char *argv[]) {

    const char *path;
    if (argc >= 2) {
        path = argv[1];
    } else {
        path = "icon.png";
    }

    // TODO opening the file twice is inefficient. Move all to a class and do evertyhing at once?

    FILE *file = std::fopen(path, "rb");
    if (file == nullptr) {
        std::perror("Could not open file");
        return false;
    }

    // Read signature
    unsigned char signature[SIGNATURE_LENGTH];
    std::fread(signature, 1, SIGNATURE_LENGTH, file);
    std::cout << "It is a png: " << (is_png(signature)) << std::endl;




    ChunkHeader *chunk = static_cast<ChunkHeader *>(malloc(sizeof(ChunkHeader)));

    std::fread(chunk, sizeof(ChunkHeader), 1, file);
    chunk->type[4] = '\0'; // null-terminate the string

    std::cout << "Chunk length: " << chunk->length << std::endl; // type-safe AND can work with
    // user_defined types
    std::cout << "Chunk type: " << chunk->type << std::endl;

    free(chunk);

    return 0;
}