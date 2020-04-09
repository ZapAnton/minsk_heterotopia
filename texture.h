#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vector>

class Texture {
  public:
    size_t img_w;
    size_t img_h;
    size_t count;
    size_t size;
    std::vector<uint32_t> img;

    Texture(const std::string filename);
    uint32_t &get(const size_t i, const size_t j, const size_t idx);
    std::vector<uint32_t> get_scaled_column(const size_t texture_id, const size_t tex_coord,
                                            const size_t column_height);
};

#endif
