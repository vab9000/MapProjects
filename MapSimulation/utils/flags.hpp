#pragma once
#include <map>
#include <optional>
#include <set>

namespace utils {
    /// A concept that checks if a type is an enum class.
    /// @tparam E The type to check.
    template<typename E>
    concept is_enum_class = std::is_enum_v<E> && !std::is_convertible_v<E, int>;

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

    /// A container for enum flags tied to some data.
    template<is_enum_class E>
    class tied_flags {
        std::map<E, void *> flags_;

    public:
        tied_flags() = default;

        /// Add a new flag with associated data
        /// @param flag The flag to add.
        /// @param connection The data to associate with the flag.
        auto add(const E flag, void *connection) -> void { flags_.emplace(flag, connection); }

        /// Check if a flag is present
        /// @param flag The flag to check.
        /// @returns the data or nullptr if it doesn't exist.
        [[nodiscard]] auto at(const E flag) -> void * {
            auto it = flags_.find(flag);
            if (it != flags_.end()) { return it->second; }
            return nullptr;
        }

        /// Remove a flag
        /// @param flag The flag to remove.
        auto remove(const E flag) -> void { flags_.erase(flag); }
    };
}
