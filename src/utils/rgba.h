#pragma once

#include <cstdint>

struct RGBA {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a = 255;
};

struct Texture {
    int width = 0;
    int height = 0;
    RGBA* colorPointer = nullptr;
};
