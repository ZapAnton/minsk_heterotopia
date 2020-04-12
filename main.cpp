#define _USE_MATH_DEFINES
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "framebuffer.h"
#include "map.h"
#include "player.h"
#include "sprite.h"
#include "texture.h"
#include "utils.h"

int wall_x_texcoord(const float hitx, const float hity, Texture &tex_walls) {
    float x = hitx - floor(hitx + .5);
    float y = hity - floor(hity + .5);
    int tex = x * tex_walls.size;
    if (std::abs(y) > std::abs(x)) {
        tex = y * tex_walls.size;
    }
    if (tex < 0) {
        tex += tex_walls.size;
    }
    return tex;
}

void map_show_sprite(Sprite &sprite, FrameBuffer &fb, Map &map) {
    const size_t rect_w = fb.width / (map.width * 2);
    const size_t rect_h = fb.height / map.height;
    fb.draw_rectangle(sprite.x * rect_w - 3, sprite.y * rect_h - 3, 6, 6, pack_color(255, 0, 0));
}

void draw_sprite(Sprite &sprite, FrameBuffer &fb, Player &player, Texture &tex_sprites) {
    float sprite_dir = atan2(sprite.y - player.y, sprite.x - player.x);
    while (sprite_dir - player.a > M_PI) {
        sprite_dir -= 2 * M_PI;
    }
    while (sprite_dir - player.a < -M_PI) {
        sprite_dir += 2 * M_PI;
    }
    const float sprite_dist = std::sqrt(pow(player.x - sprite.x, 2) + pow(player.y - sprite.y, 2));
    const size_t sprite_scree_size = std::min(1000, static_cast<int>(fb.height / sprite_dist));
    const int h_offset = (sprite_dir - player.a) / player.fov * (fb.width / 2) +
                         (fb.width / 2) / 2 - tex_sprites.size / 2;
    const int v_offset = fb.height / 2 - sprite_scree_size / 2;
    for (size_t i = 0; i < sprite_scree_size; ++i) {
        if (h_offset + i < 0 || h_offset + i >= fb.width / 2) {
            continue;
        }
        for (size_t j = 0; j < sprite_scree_size; ++j) {
            if (v_offset + j < 0 || v_offset + j > fb.height) {
                continue;
            }
            fb.set_pixel(fb.width / 2 + h_offset + i, v_offset + j, pack_color(0, 0, 0));
        }
    }
}

void render(FrameBuffer &fb, Map &map, Player &player, std::vector<Sprite> &sprites,
            Texture &tex_walls, Texture &tex_monst) {
    fb.clear(pack_color(255, 255, 255));
    const size_t rect_w = fb.width / (map.width * 2); // size of one map cell on the screen
    const size_t rect_h = fb.height / map.height;
    for (size_t j = 0; j < map.height; j++) { // draw the map
        for (size_t i = 0; i < map.width; i++) {
            if (map.is_empty(i, j))
                continue; // skip empty spaces
            size_t rect_x = i * rect_w;
            size_t rect_y = j * rect_h;
            size_t texid = map.get(i, j);
            fb.draw_rectangle(
                rect_x, rect_y, rect_w, rect_h,
                tex_walls.get(
                    0, 0,
                    texid)); // the color is taken from the upper left pixel of the texture #texid
        }
    }
    for (size_t i = 0; i < fb.width / 2; i++) { // draw the visibility cone AND the "3D" view
        float angle = player.a - player.fov / 2 + player.fov * i / float(fb.width / 2);
        for (float t = 0; t < 20; t += .01) { // ray marching loop
            float x = player.x + t * cos(angle);
            float y = player.y + t * sin(angle);
            fb.set_pixel(x * rect_w, y * rect_h,
                         pack_color(160, 160, 160)); // this draws the visibility cone

            if (map.is_empty(x, y))
                continue;

            size_t texid = map.get(x, y); // our ray touches a wall, so draw the vertical column to
                                          // create an illusion of 3D
            const float dist = t * cos(angle - player.a);
            size_t column_height = fb.height / dist;
            int x_texcoord = wall_x_texcoord(x, y, tex_walls);
            std::vector<uint32_t> column =
                tex_walls.get_scaled_column(texid, x_texcoord, column_height);
            int pix_x =
                i +
                fb.width / 2; // we are drawing at the right half of the screen, thus +fb.width/2
            for (size_t j = 0; j < column_height;
                 j++) { // copy the texture column to the framebuffer
                int pix_y = j + fb.height / 2 - column_height / 2;
                if (pix_y >= 0 && pix_y < (int)fb.height) {
                    fb.set_pixel(pix_x, pix_y, column[j]);
                }
            }
            break;
        } // ray marching loop
    }
    for (size_t i = 0; i < sprites.size(); ++i) {
        map_show_sprite(sprites[i], fb, map);
        draw_sprite(sprites[i], fb, player, tex_monst);
    }
}

int main() {
    FrameBuffer fb{1024, 512, std::vector<uint32_t>(1024 * 512, pack_color(255, 255, 255))};
    Player player{3.456, 2.345, 1.523, M_PI / 3.};
    Map map;
    Texture tex_walls("walltext.png");
    Texture tex_monst("monsters.png");
    if (!(tex_walls.count && tex_monst.count)) {
        std::cerr << "Failed to load textures" << std::endl;
        return -1;
    }
    std::vector<Sprite> sprites{{1.834, 8.765, 0}, {5.323, 5.365, 1}, {4.123, 10.265, 1}};
    render(fb, map, player, sprites, tex_walls, tex_monst);
    generate_image("out.ppm", fb.image, fb.width, fb.height);
    return 0;
}
