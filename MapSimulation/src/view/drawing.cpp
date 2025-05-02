#include "drawing.hpp"

#include "../controller/image.hpp"

drawing::drawing(data &data, double &progress) : data_(data), progress_(progress) {
}

bool drawing::init_sprites(const image &map_image, const std::vector<unsigned char> &bytes) {
    if (!texture_.loadFromImage(
        sf::Image(sf::Vector2u(map_image.get_width(), map_image.get_height()), bytes.data()))) {
        return false;
    }
    map_sprite_one_.setTexture(texture_);
    map_sprite_one_.setTextureRect(sf::IntRect(sf::Vector2(0, 0),
                                               sf::Vector2(map_image.get_width(), map_image.get_height())));
    map_sprite_two_.setTexture(texture_);
    map_sprite_two_.setTextureRect(sf::IntRect(sf::Vector2(0, 0),
                                               sf::Vector2(map_image.get_width(), map_image.get_height())));
    return true;
}

void drawing::recalculate_sprite_coords(const std::array<int, 2> offset, const double zoom,
                                        const unsigned int map_width) {
    const auto zoom_f = static_cast<float>(zoom);
    map_sprite_one_.setPosition(sf::Vector2f(static_cast<float>(offset[0]), static_cast<float>(offset[1])));
    map_sprite_one_.setScale(sf::Vector2f(zoom_f, zoom_f));
    map_sprite_two_.setPosition(sf::Vector2f(static_cast<float>(offset[0] + static_cast<int>(map_width * zoom)),
                                             static_cast<float>(offset[1])));
    map_sprite_two_.setScale(sf::Vector2f(zoom_f, zoom_f));
}

void drawing::draw_loading_screen(sf::RenderWindow &window) const {
    sf::RectangleShape progress_bar(sf::Vector2f(800, 50));
    progress_bar.setFillColor(sf::Color::Green);
    progress_bar.setPosition(sf::Vector2f(0, 275));
    progress_bar.setSize(sf::Vector2f(800 * static_cast<float>(progress_), 50));

    window.draw(progress_bar);
}

void drawing::draw_map(sf::RenderWindow &window) const {
    window.draw(map_sprite_one_);
    window.draw(map_sprite_two_);
}

void drawing::update_map_texture(const unsigned char *bytes) {
    texture_.update(bytes);
}

void drawing::update_map_texture(const unsigned char *bytes, const sf::Vector2u &dimensions,
                                 const sf::Vector2u &position) {
    texture_.update(bytes, dimensions, position);
}
