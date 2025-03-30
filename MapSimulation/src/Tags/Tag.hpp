#pragma once

class tag {
public:
    std::string name;
    unsigned int color;
    std::vector<std::unique_ptr<army> > armies;
    int gold;

    tag() {
        name = "";
        color = 0;
        armies = std::vector<std::unique_ptr<army> >();
        gold = 0;
    }

    tag(const std::string &name, const unsigned int color) {
        this->name = name;
        this->color = color;
        armies = std::vector<std::unique_ptr<army> >();
        gold = 0;
    }

    virtual ~tag() = default;

    [[nodiscard]] virtual bool has_army_access(const province &province) const {
        return true;
    }

    [[nodiscard]] army *new_army() {
        auto n_army = std::make_unique<army>(*this);
        armies.emplace_back(std::move(n_army));
        return armies.back().get();
    }

    void tick() {
    }
};
