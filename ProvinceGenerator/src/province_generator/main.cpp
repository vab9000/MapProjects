#include "map_generator.hpp"

int main() {
    const image img("images/map.png");

    const image write_img("images/write_map.png", img.width(), img.height());

    generate_provinces(write_img, img);

    write_img.write();
}
