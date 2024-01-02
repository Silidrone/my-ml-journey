//
// Created by silidrone on 30.12.2023.
//

#ifndef COMPARISONEXPERIMENT_H
#define COMPARISONEXPERIMENT_H

#include <iostream>
#include <vector>
#include "Agent.h"
#include "Experiment.h"
#include <tuple>

class ComparisonExperiment : public Experiment {
protected:
    std::vector<Agent *> m_agents;
    std::unordered_map<std::string, std::vector<double>> m_mean_rewards;
    int m_run_count;

    void perform(int roll_count) override {
        for (Agent *agent: m_agents) {
            m_mean_rewards[agent->getName()] = std::vector<double>(roll_count, 0);
        }
        for (int i = 1; i <= m_run_count; i++) {
            m_machine->reset();

//            std::cout << "For run #" << i << " the q* values of the machine are: " << std::endl;
//            m_machine->printQStar();

            for (Agent *agent: m_agents)
                agent->init(m_machine);

            for (int j = 0; j < roll_count; j++) {
                for (Agent *agent: m_agents) {
                    auto [action, reward] = agent->play();
                    m_mean_rewards[agent->getName()][j] += (1.0 / m_run_count) * reward;
//                    std::cout << "Player " << agent->getName() << " played: {action:  " << action << ", reward: " << reward << "}" << std::endl;
                }
            }
        }

//        for (Agent *agent: m_agents) {
//            agent->printState();
//            agent->printRewards();
//        }
    }

    void createChart() override {
        plt::title("Rewards versus time step");
        plt::xlabel("Step");
        plt::ylabel("Reward");
        for (Agent *agent: m_agents) {
            auto agent_name = agent->getName();
            plt::plot(m_mean_rewards[agent_name], {{"label", agent_name}});
            plt::legend();
        }
    }

public:
    ComparisonExperiment(MAB *machine, int run_count) : Experiment(machine), m_run_count(run_count) {

    }

    ~ComparisonExperiment() override {
        for (Agent *agent: m_agents) {
            delete agent;
        }
    }

    void addAgent(Agent *agent) {
        m_agents.push_back(agent);
    }
};

#endif //COMPARISONEXPERIMENT_H
