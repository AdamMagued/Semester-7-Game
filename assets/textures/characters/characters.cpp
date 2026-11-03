#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


int width, height, channels;

unsigned char* image = stbi_load(
    "assets/player.png",
    &width, &height, &channels, 4
);
