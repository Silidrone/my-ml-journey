//
// Created by silidrone on 29.12.2023.
//

#ifndef GAUSSIANMAB_H
#define GAUSSIANMAB_H

#include <iostream>
#include <random>
#include <vector>
#include "./MAB.h"

class GaussianMAB : public MAB {
private:
    std::random_device m_rd;
    std::mt19937 m_gen;
    std::normal_distribution<double> m_gauss_dis;

    void init() {
        m_gen = std::mt19937(m_rd());
        m_gauss_dis = std::normal_distribution<double>(0.0, 1.0);
        m_qstar_values.resize(m_arm_count);
        for(int i = 0; i < m_arm_count; i++) {
            m_qstar_values[i] = m_gauss_dis(m_gen);
        }
    }
public:
    explicit GaussianMAB(int arm_count) : MAB(arm_count) {
        init();
    };

    double roll(int action, int timestamp) override {
        return m_qstar_values[action] + m_gauss_dis(m_gen);
    }

    void reset() override {
        init();
    }
};


#endif //GAUSSIANMAB_H
