#ifndef SPRITE_H
#define SPRITE_H

#include <cstddef>

class Sprite {
  public:
    float x, y;
    size_t tex_id;
    float player_dist;
    bool operator<(const Sprite &s) const;
};

#endif
