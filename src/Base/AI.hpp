//
// Created by varun on 1/18/2025.
//

#ifndef AI_HPP
#define AI_HPP

#include <vector>
#include <random>

#include "Action.hpp"

// How unlikely the AI is to perform an action
constexpr double INERTIA = 10.0;

class AI {
    std::vector<Action*> *actions;
    std::default_random_engine *gen;

public:
    AI() {
        actions = new std::vector<Action*>();
        std::random_device rd;
        gen = new std::default_random_engine(rd());
    }

    ~AI() {
        delete actions;
        delete gen;
    }

    void addAction(Action *action) const {
        actions->emplace_back(action);
    }

    void addActions(const std::vector<Action*> &actionsVector) const {
        for (const auto action : actionsVector) {
            addAction(action);
        }
    }

    void clearActions() const {
        actions->clear();
    }

    void performActions() const {
        auto dis = std::uniform_real_distribution(0.0, 1.0);

        const auto random = dis(*gen);

        for (const auto &action : *actions) {
			if (const int weight = action->getWeight(action->sParam, action->oParam); weight >= 1) {
                if (const auto chance = 1.0 - (1 / ((weight * weight) / INERTIA + 1)); random < chance) {
                    action->action(action->sParam, action->oParam);
                }
            }
        }
    }
};


#endif //AI_HPP
