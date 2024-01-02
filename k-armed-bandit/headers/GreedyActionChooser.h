//
// Created by silidrone on 30.12.2023.
//

#ifndef GREEDYACTIONCHOOSER_H
#define GREEDYACTIONCHOOSER_H

#include "ActionChooser.h"
#include <algorithm>

class GreedyActionChooser : public ActionChooser{
public:
    GreedyActionChooser() = default;

    int choose(std::vector<double> values, std::vector<int> times_chosen, int time_step) override {
        return std::distance(values.begin(), std::max_element(values.begin(), values.end()));
    }
};

#endif //GREEDYACTIONCHOOSER_H
