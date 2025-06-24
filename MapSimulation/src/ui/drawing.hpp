#pragma once

#include <SFML/Graphics.hpp>

struct data;
class image;
class simulation;

class drawing {
    data &data_;
    const double &progress_;
    simulation &simulation_;

    sf::Texture texture_{};
    sf::Sprite map_sprite_one_{texture_};
    sf::Sprite map_sprite_two_{texture_};

public:
    explicit drawing(data &data, const double &progress, simulation * simulation);

    bool init_sprites(const image &map_image, const std::vector<unsigned char> &bytes);

    void recalculate_sprite_coords(std::array<int, 2> offset, double zoom, unsigned int map_width);

    void draw_loading_screen(sf::RenderWindow &window) const;

    void draw_map(sf::RenderWindow &window) const;

    void update_map_texture(const unsigned char *bytes);

    void update_map_texture(const unsigned char *bytes, const sf::Vector2u &dimensions,
                            const sf::Vector2u &position);

    void draw_selected_province_gui(sf::RenderWindow &window) const;

    void draw_map_mode_selector(sf::RenderWindow &window) const;
};
