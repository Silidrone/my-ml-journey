//
// Created by silidrone on 02.01.2024.
//

#ifndef NONSTATIONARYMAB_H
#define NONSTATIONARYMAB_H

#include "./MAB.h"
#include <random>

class NonStationaryMABDecorator : public MAB{
protected:
    MAB *m_machine;
    int m_dii; // Destabilize interval (in iterations)
public:
    explicit NonStationaryMABDecorator(MAB *machine, int dii) : MAB(machine->getArmCount()), m_machine(machine), m_dii(dii) {}
    ~NonStationaryMABDecorator() override {
        delete m_machine;
    }

    void reset() override { m_machine->reset(); }

    double roll(int action, int timestep) override {
        if (m_dii > 0 && (timestep + 1) % m_dii == 0) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<double> d1(0.0, 2.0);
            std::uniform_int_distribution<std::size_t> d2(0, m_machine->m_qstar_values.size() - 1);

            m_machine->m_qstar_values[d2(gen)] *= d1(gen);
        }
        m_machine->roll(action, timestep);
    }

    void printQStar() override {
        m_machine->printQStar();
    }

    void setDII(int dii) {
        m_dii = dii;
    }
};

#endif //NONSTATIONARYMAB_H
