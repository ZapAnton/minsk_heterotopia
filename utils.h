#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
#include <string>
#include <vector>

uint32_t pack_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255);
void unpack_color(const uint32_t &color, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a);
void generate_image(const std::string filename, const std::vector<uint32_t> &image_buffer,
                    const size_t width, const size_t height);

#endif
