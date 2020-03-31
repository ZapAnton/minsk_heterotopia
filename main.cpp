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

int main() {
    const size_t win_w = 512;
    const size_t win_h = 512;
    std::vector<uint32_t> framebuffer(win_w * win_h, 255);
    for (size_t j = 0; j < win_h; ++j) {
        for (size_t i = 0; i < win_w; ++i) {
            auto r = 225 * j / float(win_h);
            auto g = 225 * i / float(win_w);
            auto b = 0;
            auto color_index = i + j * win_w;
            framebuffer[color_index] = pack_color(r, g, b);
        }
    }
    generate_image("out.ppm", framebuffer, win_w, win_h);
    return 0;
}
