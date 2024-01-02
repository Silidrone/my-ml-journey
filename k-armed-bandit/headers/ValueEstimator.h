//
// Created by silidrone on 30.12.2023.
//

#ifndef VALUEESTIMATOR_H
#define VALUEESTIMATOR_H

#include <vector>

class ValueEstimator {
protected:
    std::vector<double> m_values;
    double m_initial_estimation;
public:
    explicit ValueEstimator(double initial_estimation = 0.0) {
        m_initial_estimation = initial_estimation;
    };
    virtual ~ValueEstimator() = default;
    virtual void feed(int action, double reward, int times_chosen, int time_step) = 0;
    virtual void init(int arm_count) {
        m_values = std::vector<double>(arm_count, m_initial_estimation);
    }
    std::vector<double> getValues() const {
        return m_values;
    }
};

#endif //VALUEESTIMATOR_H
