//
// Created by silidrone on 30.12.2023.
//

#ifndef RANDOMACTIONCHOOSER_H
#define RANDOMACTIONCHOOSER_H

#include "ActionChooser.h"
#include <random>

class RandomActionChooser : public ActionChooser{
private:
    std::random_device m_rd;
    std::mt19937 m_gen;

public:
    RandomActionChooser() {
        m_gen = std::mt19937(m_rd());
    }

    int choose(std::vector<double> values, std::vector<int> times_chosen, int time_step) override {
        return std::uniform_int_distribution<int>(0, values.size() - 1)(m_gen);
    }
};

#endif //RANDOMACTIONCHOOSER_H
