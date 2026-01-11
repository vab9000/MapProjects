#ifndef MAPEDITOR_SAVER_HPP
#define MAPEDITOR_SAVER_HPP
#include <istream>
#include <zstring_view.hpp>

namespace logic {
    class saver {
    public:
        static auto save(zstring_view folder) -> void;

        static auto load(zstring_view folder) -> void;

    private:
        static auto load_properties(std::istream &file) -> void;

        static auto load_locations(std::istream &file) -> void;

        static auto load_image(zstring_view filepath) -> void;
    };
} // namespace logic

#endif
