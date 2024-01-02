//
// Created by silidrone on 30.12.2023.
//

#ifndef AGENT_H
#define AGENT_H

#include "ValueEstimator.h"
#include "ActionChooser.h"
#include "MAB.h"
#include <string>
#include <utility>
#include <stdexcept>
#include <numeric>
#include <iostream>
#include <tuple>

class Agent {
protected:
    std::string m_name;
    ValueEstimator *m_value_estimator;
    ActionChooser *m_action_chooser;
    MAB *m_machine;
    std::vector<double> m_rewards;
    std::vector<int> m_times_chosen;
    int m_time_steps;
public:
    explicit Agent(std::string name, MAB *machine, ValueEstimator *valueEstimator, ActionChooser *actionChooser) :
            m_name(std::move(name)) {
        setValueEstimator(valueEstimator);
        setActionChooser(actionChooser);
        init(machine);
    }

    virtual ~Agent() {
        delete m_value_estimator;
        delete m_action_chooser;
    }

    void init(MAB *machine) {
        m_time_steps = 1;
        m_times_chosen = std::vector<int>(machine->getArmCount(), 1);
        m_rewards.clear();
        m_machine = machine;
        if(m_value_estimator == nullptr) {
            throw std::runtime_error("Value Estimator must be set before calling init!");
        }
        m_value_estimator->init(m_machine->getArmCount());
    }

    void setValueEstimator(ValueEstimator *valueEstimator) {
        delete m_value_estimator;
        m_value_estimator = valueEstimator;
    }

    void setActionChooser(ActionChooser *actionChooser) {
        delete m_action_chooser;
        m_action_chooser = actionChooser;
    }

    std::tuple<int, double> play() {
        auto currently_estimated_values = m_value_estimator->getValues();
        int action = m_action_chooser->choose(currently_estimated_values, m_times_chosen, m_time_steps);
        double reward = m_machine->roll(action, m_time_steps);
        m_rewards.push_back(reward);
        m_value_estimator->feed(action, reward, m_times_chosen[action], m_time_steps);
        m_times_chosen[action]++;
        m_time_steps++;

        return {action, reward};
    }

    void printRewards() {
        std::cout << m_name << " rewards:" << std::endl;
        int i = 0;
        for(auto reward : m_rewards) {
            std::cout << "[" << i++ << "]: " << reward << std::endl;
        }
    }

    void printState() {
        std::cout << m_name << " - total reward: " << std::accumulate(m_rewards.begin(), m_rewards.end(), 0.0) << std::endl;
    }

    std::vector<double> getRewards() const {
        return m_rewards;
    }

    std::string getName() const {
        return m_name;
    }

    ValueEstimator *getValueEstimator() {
        return m_value_estimator;
    }
};

#endif //AGENT_H
