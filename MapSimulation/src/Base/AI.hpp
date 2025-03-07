#pragma once

#include <vector>
#include <random>

// How unlikely the AI is to perform an action
constexpr double INERTIA = 10.0;

class Action {
    int weightCache;
    int numCache = 0;
public:
    void (*action)(void *, void *);
    void *sParam;
    void *oParam;
    int (*weightFunc)(void *, void *);

    Action(void (*action)(void *, void *), void(*sParam), void(*oParam), int (*getWeight)(void *, void *)) {
        this->action = action;
        this->sParam = sParam;
        this->oParam = oParam;
        this->weightFunc = getWeight;
        weightCache = this->getWeight();
    }

    ~Action() = default;

    void perform() const {
        action(sParam, oParam);
    }

    [[nodiscard]] int getWeight() {
        if (numCache != 0) {
            numCache--;
            return weightCache;
        }
        numCache = 30;
        return weightFunc(sParam, oParam);
    }
};

class AI {
    std::vector<std::unique_ptr<Action>> actions;
    std::default_random_engine gen;

public:
    AI() {
        actions = std::vector<std::unique_ptr<Action>>();
        std::random_device rd;
        gen = std::default_random_engine(rd());
    }

    void addAction(std::unique_ptr<Action> &&action) {
        actions.push_back(std::move(action));
    }

    void clearActions() {
        actions.clear();
    }

    void performActions() {
        auto dis = std::uniform_real_distribution(0.0, 1.0);

        const auto random = dis(gen);

        for (const auto &action : actions) {
			if (const int weight = action->getWeight(); weight >= 1) {
                if (const auto chance = 1.0 - (1 / ((weight * weight) / INERTIA + 1)); random < chance) {
                    action->perform();
                }
            }
        }
    }
};

class HasAI {
public:
    AI ai;

    virtual ~HasAI() = default;

    virtual void updateAI() = 0;
};
