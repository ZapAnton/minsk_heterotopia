#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

uint32_t pack_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255) {
    return (a << 24) + (b << 16) + (g << 8) + r;
}

void unpack_color(const uint32_t &color, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a) {
    r = (color >> 0) & 255;
    g = (color >> 8) & 255;
    b = (color >> 16) & 255;
    a = (color >> 24) & 255;
}

void generate_image(const std::string filename, const std::vector<uint32_t> &image_buffer,
                    const size_t width, const size_t height) {
    std::ofstream ofs(filename);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (size_t i = 0; i < image_buffer.size(); ++i) {
        uint8_t r, g, b, a;
        unpack_color(image_buffer[i], r, g, b, a);
        ofs << static_cast<char>(r) << static_cast<char>(g) << static_cast<char>(b);
    }
    ofs.close();
}

void draw_rectangle(std::vector<uint32_t> &image_buffer, const size_t image_width,
                    const size_t image_height, const size_t x, const size_t y, const size_t w,
                    const size_t h, const uint32_t color) {
    for (size_t i = 0; i < w; ++i) {
        for (size_t j = 0; j < h; ++j) {
            const size_t cx = x + i;
            const size_t cy = y + j;
            if (cx >= image_width || cy >= image_height) {
                continue;
            }
            const size_t rect_index = cx + cy * image_width;
            image_buffer[rect_index] = color;
        }
    }
}

bool load_texture(const std::string filename, std::vector<uint32_t> &texture, size_t &texture_size,
                  size_t &texture_count) {
    int nchannels = -1;
    int w;
    int h;
    unsigned char *pixmap = stbi_load(filename.c_str(), &w, &h, &nchannels, 0);
    if (!pixmap) {
        std::cerr << "Error: can not load the textures" << std::endl;
        return false;
    }
    if (nchannels != 4) {
        std::cerr << "Error: the texture must be a 32 bit image" << std::endl;
        stbi_image_free(pixmap);
        return false;
    }
    texture_count = w / h;
    texture_size = w / texture_count;
    if (w != h * int(texture_count)) {
        std::cerr << "Error: the texture file must contain N square textures packed horizontally"
                  << std::endl;
        stbi_image_free(pixmap);
        return false;
    }
    texture = std::vector<uint32_t>(w * h);
    for (int j = 0; j < h; ++j) {
        for (int i = 0; i < w; ++i) {
            uint8_t r = pixmap[(i + j * w) * 4 + 0];
            uint8_t g = pixmap[(i + j * w) * 4 + 1];
            uint8_t b = pixmap[(i + j * w) * 4 + 2];
            uint8_t a = pixmap[(i + j * w) * 4 + 3];
            texture[i + j * w] = pack_color(r, g, b, a);
        }
    }
    stbi_image_free(pixmap);
    return true;
}

int main() {
    const size_t win_w = 1024;
    const size_t win_h = 512;
    const size_t map_w = 16;
    const size_t map_h = 16;
    const size_t rect_w = win_w / (map_w * 2);
    const size_t rect_h = win_h / map_h;
    const char map[] = "0000222222220000"
                       "1              0"
                       "1              0"
                       "1     0        0"
                       "0     0  1110000"
                       "0     3        0"
                       "0   10000      0"
                       "0   0   11100  0"
                       "0   0   0      0"
                       "0   0   1  00000"
                       "0       1      0"
                       "2       1      0"
                       "0       0      0"
                       "0 0000000      0"
                       "0              0"
                       "0002222222200000";
    const float fov = M_PI / 3.0;
    float player_x = 3.456;
    float player_y = 2.345;
    float player_a = 1.523;
    std::vector<uint32_t> framebuffer(win_w * win_h, pack_color(255, 255, 255));
    const size_t ncolors = 10;
    std::vector<uint32_t> colors(ncolors);
    for (size_t i = 0; i < ncolors; ++i) {
        colors[i] = pack_color(rand() % 255, rand() % 255, rand() % 255);
    }
    std::vector<uint32_t> wall_texture;
    size_t wall_texture_size = 0;
    size_t wall_texture_cout = 0;
    if (!load_texture("walltext.png", wall_texture, wall_texture_size, wall_texture_cout)) {
        std::cerr << "Failed to load wall texture" << std::endl;
        return -1;
    }
    for (size_t j = 0; j < map_h; ++j) {
        for (size_t i = 0; i < map_w; ++i) {
            const auto map_index = i + j * map_w;
            if (map[map_index] == ' ') {
                continue;
            }
            const size_t rect_x = i * rect_w;
            const size_t rect_y = j * rect_h;
            const size_t icolor = map[i + j * map_w] - '0';
            draw_rectangle(framebuffer, win_w, win_h, rect_x, rect_y, rect_w, rect_h,
                           colors[icolor]);
        }
    }
    for (size_t i = 0; i < win_w / 2; ++i) {
        const float angle = player_a - fov / 2 + fov * i / float(win_w / 2);
        for (float t = 0; t < 20; t += 0.01) {
            float cx = player_x + t * cos(angle);
            float cy = player_y + t * sin(angle);
            size_t pix_x = cx * rect_w;
            size_t pix_y = cy * rect_h;
            framebuffer[pix_x + pix_y * win_w] = pack_color(160, 160, 160);
            if (map[int(cx) + int(cy) * map_w] != ' ') {
                const size_t column_height = win_h / (t * cos(angle - player_a));
                const size_t icolor = map[int(cx) + int(cy) * map_w] - '0';
                draw_rectangle(framebuffer, win_w, win_h, win_w / 2 + i,
                               win_h / 2 - column_height / 2, 1, column_height, colors[icolor]);
                break;
            }
        }
    }
    const size_t texid = 4;
    for (size_t i = 0; i < wall_texture_size; ++i) {
        for (size_t j = 0; j < wall_texture_size; ++j) {
            framebuffer[i + j * win_w] = wall_texture[i + texid * wall_texture_size +
                                                      j * wall_texture_size * wall_texture_cout];
        }
    }
    generate_image("out.ppm", framebuffer, win_w, win_h);
    return 0;
}
