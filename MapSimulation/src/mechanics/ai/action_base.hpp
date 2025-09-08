#pragma once

namespace mechanics {
    class action_base {
    public:
        virtual ~action_base() = default;

        virtual explicit operator bool() const = 0;

        [[nodiscard]] virtual auto operator*() const -> int = 0;

        virtual auto operator()() const -> void = 0;
    };
}
