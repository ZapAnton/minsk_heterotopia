#ifndef MAP_H
#define MAP_H

#include <cstdlib>

class Map {
  public:
    size_t width;
    size_t height;
    Map();
    int get(const size_t i, const size_t j);
    bool is_empty(const size_t i, const size_t j);
};

#endif
