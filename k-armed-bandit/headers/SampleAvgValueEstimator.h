//
// Created by silidrone on 30.12.2023.
//

#ifndef SAMPLEAVGVALUEESTIMATOR_H
#define SAMPLEAVGVALUEESTIMATOR_H

#include "ValueEstimator.h"
#include <stdexcept>

class SampleAvgValueEstimator : public ValueEstimator {
public:
    explicit SampleAvgValueEstimator(double initial_estimation = 0.0) : ValueEstimator(initial_estimation) {}

    void feed(int action, double reward, int times_chosen, int time_step) override {
        if(times_chosen <= 0) throw std::runtime_error("When using SampleAvgValueEstimator, you have to provide times_chosen parameter that is > 0");
        m_values[action] = m_values[action] + (1.0 / times_chosen) * (reward - m_values[action]);
    }
};

#endif //SAMPLEAVGVALUEESTIMATOR_H
