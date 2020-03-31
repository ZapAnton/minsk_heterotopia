#include <fstream>
#include <iostream>
#include <vector>

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
                    const size_t image_heigh, const size_t x, const size_t y, const size_t w,
                    const size_t h, const uint32_t color) {
    for (size_t i = 0; i < w; ++i) {
        for (size_t j = 0; j < h; ++j) {
            const size_t cx = x + i;
            const size_t cy = y + j;
            const size_t rect_index = cx + cy * image_width;
            image_buffer[rect_index] = color;
        }
    }
}

int main() {
    const size_t win_w = 512;
    const size_t win_h = 512;
    const size_t map_w = 16;
    const size_t map_h = 16;
    const size_t rect_w = win_w / map_w;
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
    std::vector<uint32_t> framebuffer(win_w * win_h, 255);
    for (size_t j = 0; j < win_h; ++j) {
        for (size_t i = 0; i < win_w; ++i) {
            const auto r = 225 * j / float(win_h);
            const auto g = 225 * i / float(win_w);
            const auto b = 0;
            const auto color_index = i + j * win_w;
            framebuffer[color_index] = pack_color(r, g, b);
        }
    }
    for (size_t j = 0; j < map_h; ++j) {
        for (size_t i = 0; i < map_w; ++i) {
            const auto map_index = i + j * map_w;
            if (map[map_index] == ' ') {
                continue;
            }
            const size_t rect_x = i * rect_w;
            const size_t rect_y = j * rect_h;
            const uint32_t rect_color = pack_color(0, 255, 255);
            draw_rectangle(framebuffer, win_w, win_h, rect_x, rect_y, rect_w, rect_h, rect_color);
        }
    }
    generate_image("out.ppm", framebuffer, win_w, win_h);
    return 0;
}
