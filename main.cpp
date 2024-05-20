//
// Created by hugo on 5/20/2024.
//

#include <cstdio>
#include <algorithm>
#include <windows.h>
#include <iostream>

bool is_png(const char *path) {
    FILE *file = std::fopen(path, "rb");
    if (file == nullptr) {
        std::perror("Could not open file");
        return false;
    }
    unsigned char expected_signature[] = {137, 80, 78, 71, 13, 10, 26, 10};

    size_t signature_len = sizeof(expected_signature) / sizeof(expected_signature[0]);

    unsigned char buffer[signature_len];
    std::fread(buffer, 1, 8, file);

    if (!std::equal(buffer, buffer + signature_len, expected_signature)){
        perror("This is not a PNG file!");
        return false;
    }

//    for (int i = 0; i < 8; i++) {
//        std::printf("%d ", buffer[i]);
//    }

    std::fclose(file);
    return true;

}



int main(int argc, char *argv[]) {

    const char *path;
    if (argc >= 2) {
        path = argv[1];
    } else {
        path = "icon.png";
    }

    std::cout << "It is a png: " << (is_png(path));
    return 0;
}