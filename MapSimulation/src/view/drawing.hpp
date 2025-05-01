#pragma once

#include <SFML/Graphics.hpp>
#include "../controller/data.hpp"

class drawing {
    data *data_;

    sf::Texture texture_{};
    sf::Sprite map_sprite_one_{texture_};
    sf::Sprite map_sprite_two_{texture_};

    double *progress_;

public:
    explicit drawing(data *data, double *progress) : data_(data), progress_(progress) {
    }

    bool init_sprites(const image &map_image, const std::vector<unsigned char> &bytes) {
        if (!texture_.loadFromImage(sf::Image(sf::Vector2u(map_image.width, map_image.height), bytes.data()))) {
            return false;
        }
        map_sprite_one_.setTexture(texture_);
        map_sprite_one_.setTextureRect(sf::IntRect(sf::Vector2(0, 0), sf::Vector2(map_image.width, map_image.height)));
        map_sprite_two_.setTexture(texture_);
        map_sprite_two_.setTextureRect(sf::IntRect(sf::Vector2(0, 0), sf::Vector2(map_image.width, map_image.height)));
        return true;
    }

    void recalculate_sprite_coords(const std::array<int, 2> offset, const double zoom, const unsigned int map_width) {
        map_sprite_one_.setPosition(sf::Vector2f(offset[0], offset[1]));
        map_sprite_one_.setScale(sf::Vector2f(zoom, zoom));
        map_sprite_two_.setPosition(sf::Vector2f(offset[0] + static_cast<int>(map_width * zoom), offset[1]));
        map_sprite_two_.setScale(sf::Vector2f(zoom, zoom));
    }

    void draw_loading_screen(sf::RenderWindow &window) const {
        sf::RectangleShape progress_bar(sf::Vector2f(800, 50));
        progress_bar.setFillColor(sf::Color::Green);
        progress_bar.setPosition(sf::Vector2f(0, 275));
        progress_bar.setSize(sf::Vector2f(800 * *progress_, 50));

        window.draw(progress_bar);
    }

    void draw_map(sf::RenderWindow &window) const {
        window.draw(map_sprite_one_);
        window.draw(map_sprite_two_);
    }

    void update_map_texture(const unsigned char *bytes) {
        texture_.update(bytes);
    }

    void update_map_texture(const unsigned char *bytes, const sf::Vector2u &dimensions,
                            const sf::Vector2u &position) {
        texture_.update(bytes, dimensions, position);
    }
};
