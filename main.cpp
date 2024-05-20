//
// Created by hugol on 5/20/2024.
//

#include <cstdio>
#include <algorithm>

int main(int argc, char *argv[]) {

    unsigned char expected_signature[] = {137, 80, 78, 71, 13, 10, 26, 10};

    const char *path;
    if (argc >= 2) {
        path = argv[1];
    } else {
        path = "../icon.png";
    }

    FILE *file = fopen("icondebug.png", "rb");
    if (file == nullptr) {
        std::perror("Could not open file");
        return 1;
    }

    size_t signature_len = sizeof(expected_signature) / sizeof(expected_signature[0]);

    unsigned char buffer[signature_len];
    std::fread(buffer, 1, 8, file);

    if (!std::equal(buffer, buffer + signature_len, expected_signature)){
        perror("This is not a PNG file!");
    }

    for (int i = 0; i < 8; i++) {
        std::printf("%d ", buffer[i]);
    }
    return 0;
}