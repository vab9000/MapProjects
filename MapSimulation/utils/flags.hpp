#ifndef UTILS_FLAGS
#define UTILS_FLAGS
#include <enums.hpp>
#include <map>
#include <set>

namespace utils {
    /// A container for enum flags.
    /// @tparam E An enum class type.
    template<is_enum_class E>
    class flags {
        std::set<E> flags_;

    public:
        flags() = default;

        /// Add a new flag
        /// @param flag The flag to add.
        auto add(const E flag) -> void { flags_.insert(flag); }

        /// Check if a flag is present
        /// @param flag The flag to check.
        /// @returns True if the flag is present, false otherwise.
        [[nodiscard]] auto has(const E flag) const -> bool { return flags_.contains(flag); }

        /// Remove a flag
        /// @param flag The flag to remove.
        auto remove(const E flag) -> void { flags_.erase(flag); }
    };

    /// A concept that checks if a mapping type provides a type for a given enum value to a pointer type.
    /// @tparam Map The mapping type.
    /// @tparam E The enum class type.
    template<typename Map, typename E>
    concept has_map_type_for = requires {
        typename Map::template type<static_cast<E>(0)>;
    } && std::is_convertible_v<typename Map::template type<static_cast<E>(0)>, void *> && is_enum_class<E>;

    /// A container for enum flags tied to some data.
    /// @tparam E An enum class type.
    /// @tparam Map A mapping type that provides associated data types for each enum value.
    template<is_enum_class E, has_map_type_for<E> Map>
    class tied_flags {
        std::map<E, void *> flags_;

    public:
        tied_flags() = default;

        /// Add a new flag with associated data
        /// @tparam Flag The flag to add.
        /// @param connection The data to associate with the flag.
        template<E Flag>
        auto add(Map::template type<Flag> connection) -> void {
            static_assert(std::is_convertible_v<typename Map::template type<Flag>, void *>);
            flags_.emplace(Flag, static_cast<void *>(connection));
        }

        /// Check if a flag is present
        /// @tparam Flag The flag to check.
        /// @returns the data or nullptr if it doesn't exist.
        template<E Flag>
        [[nodiscard]] auto at() -> Map::template type<Flag> {
            auto it = flags_.find(Flag);
            if (it != flags_.end()) { return static_cast<Map::template type<Flag>>(it->second); }
            return nullptr;
        }

        /// Remove a flag
        /// @param flag The flag to remove.
        auto remove(const E flag) -> void { flags_.erase(flag); }
    };
}
#endif
