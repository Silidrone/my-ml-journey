//
// Created by silidrone on 30.12.2023.
//

#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "MAB.h"
#include <string>
#include "../matplotlibcpp.h"

namespace plt = matplotlibcpp;

class Experiment{
protected:
    MAB *m_machine;
    virtual void init(int roll_count) {}

    virtual void perform(int roll_count) = 0;

    virtual void createChart() = 0;
public:
    explicit Experiment(MAB *machine) : m_machine(machine) {}
    virtual ~Experiment() {
        delete m_machine;
    }

    void run(int roll_count, std::string path) {
        init(roll_count);
        perform(roll_count);

        createChart();
        plt::save(path);
    }
};

#endif //EXPERIMENT_H
