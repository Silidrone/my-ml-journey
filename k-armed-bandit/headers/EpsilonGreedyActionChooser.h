//
// Created by silidrone on 30.12.2023.
//

#ifndef EPSILONGREEDYACTIONCHOOSER_H
#define EPSILONGREEDYACTIONCHOOSER_H

#include "ActionChooser.h"
#include "GreedyActionChooser.h"
#include "RandomActionChooser.h"
#include <random>

class EpsilonGreedyActionChooser : public ActionChooser {
protected:
    double m_epsilon;
    GreedyActionChooser m_greedy_action_chooser;
    RandomActionChooser m_random_action_chooser;
public:
    explicit EpsilonGreedyActionChooser(double epsilon) : m_epsilon(epsilon) {

    }

    int choose(std::vector<double> values, std::vector<int> times_chosen, int time_step) override {
        std::random_device rd;
        std::mt19937 gen(rd());

        return std::uniform_real_distribution<>(0.0, 1.0)(gen) < m_epsilon ? m_random_action_chooser.choose(values,
                                                                                                             times_chosen,
                                                                                                             time_step)
                                                                           : m_greedy_action_chooser.choose(values,
                                                                                                             times_chosen,
                                                                                                             time_step);
    }
};

#endif //EPSILONGREEDYACTIONCHOOSER_H
