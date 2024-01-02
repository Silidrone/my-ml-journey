//
// Created by silidrone on 30.12.2023.
//

#ifndef CONSTANTSTEPSIZEVALUEESTIMATOR_H
#define CONSTANTSTEPSIZEVALUEESTIMATOR_H

#include "ValueEstimator.h"

class ConstantStepSizeValueEstimator : public ValueEstimator{
protected:
    double m_step_size;
public:
    explicit ConstantStepSizeValueEstimator(double step_size, double initial_estimation = 0.0) : ValueEstimator(initial_estimation), m_step_size(step_size) {}

    void feed(int action, double reward, int times_chosen, int time_step) override {
        m_values[action] = m_values[action] + m_step_size * (reward - m_values[action]);
    }
};

#endif //CONSTANTSTEPSIZEVALUEESTIMATOR_H
