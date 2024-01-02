//
// Created by silidrone on 30.12.2023.
//

#ifndef UCBACTIONCHOOSER_H
#define UCBACTIONCHOOSER_H

#include "ActionChooser.h"
#include "GreedyActionChooser.h"
#include <algorithm>
#include <cmath>

class UCBActionChooser : public ActionChooser{
protected:
    GreedyActionChooser greedyActionChooser;
    double m_c;
public:
    explicit UCBActionChooser(double c) : m_c(c) {}

    int choose(std::vector<double> values, std::vector<int> times_chosen, int time_step) override {
        std::vector<double> ucbedValues;
        for(int i = 0; i < values.size(); i++) {
            ucbedValues.push_back(values[i] + m_c * std::sqrt(std::log(time_step) / times_chosen[i]));
        }

        return greedyActionChooser.choose(ucbedValues, times_chosen, time_step);
    }
};

#endif //KARMEDBANDIT_UCBACTIONCHOOSER_H
