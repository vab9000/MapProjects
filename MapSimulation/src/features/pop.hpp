#pragma once

enum class lifestyle_t {
    tribal,
};

class pop {
    size_t size_;
    lifestyle_t lifestyle_;

public:
    pop();

    // The number of individuals in the pop
    [[nodiscard]] size_t size() const;
};
