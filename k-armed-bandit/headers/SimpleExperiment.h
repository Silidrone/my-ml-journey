//
// Created by silidrone on 30.12.2023.
//

#ifndef SIMPLEEXPERIMENT_H
#define SIMPLEEXPERIMENT_H

#include "Experiment.h"
#include "Agent.h"
#include "GreedyActionChooser.h"
#include "SampleAvgValueEstimator.h"

class SimpleExperiment : Experiment {
protected:
    Agent *m_agent;

    void createChart() override {
        plt::figure_size(1200, 780);  // Set the size of the figure

        plt::hist(m_agent->getRewards(), 20);  // Create a histogram with 20 bins

        plt::title("Reward Distribution");
        plt::xlabel("Reward");
        plt::ylabel("Frequency");

        plt::show();
    }

    void perform(int roll_count) override {
        m_machine->reset();
        m_agent->setValueEstimator(new SampleAvgValueEstimator());
        m_agent->setActionChooser(new GreedyActionChooser());
        m_agent->init(m_machine);

        for(int i = 0; i < roll_count; i++) {
            m_agent->play();
        }
    }
public:
    SimpleExperiment(MAB *machine, Agent *agent) : Experiment(machine), m_agent(agent) {}
};

#endif //SIMPLEEXPERIMENT_H
