#include <iostream>

#include "framebuffer.h"
#include "utils.h"

void FrameBuffer::set_pixel(const size_t x, const size_t y, const uint32_t color) {
    this->image[x + y * this->width] = color;
}

void FrameBuffer::clear(const uint32_t color) {
    this->image = std::vector<uint32_t>(this->width * this->height, color);
}

void FrameBuffer::draw_rectangle(const size_t rect_x, const size_t rect_y, const size_t rect_w,
                                 const size_t rect_h, const uint32_t color) {
    for (size_t i = 0; i < rect_w; ++i) {
        for (size_t j = 0; j < rect_h; ++j) {
            const size_t cx = rect_x + i;
            const size_t cy = rect_y + j;
            if (cx < this->width && cy < this->height) {
                this->set_pixel(cx, cy, color);
            }
        }
    }
}
