//
// Created by hugol on 5/20/2024.
//

#include <cstdio>

int main() {

    // Read icon.png byte by byte
    FILE *file = std::fopen("../icon.png", "rb");
    if (file == NULL) {
        std::perror("This is just an error message");
        return 1;
    }

    unsigned char buffer[8];
    std::fread(buffer, 1, 8, file);

    for (int i = 0; i < 8; i++) {
        std::printf("%d ", buffer[i]);
    }
    return 0;
}