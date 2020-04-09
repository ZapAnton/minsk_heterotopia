#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <cstdint>
#include <vector>

class FrameBuffer {
  public:
    size_t width;
    size_t height;
    std::vector<uint32_t> image;

    void clear(const uint32_t color);
    void set_pixel(const size_t x, const size_t y, const uint32_t color);
    void draw_rectangle(const size_t rect_x, const size_t rect_y, const size_t rect_w,
                        const size_t rect_h, const uint32_t color);
};

#endif
