#ifndef POLICYITERATION_H
#define POLICYITERATION_H

#include "MDPSolver.h"

template <typename State, typename Action, typename StateHash = std::hash<State>, typename ActionHash = std::hash<Action>>
class PolicyIteration : public MDPSolver<State, Action, StateHash, ActionHash> {
protected:
    double m_discount_rate;
    double m_policy_evaluation_threshold;

    virtual void policy_evaluation() = 0;
    virtual bool policy_improvement() = 0;
public:
    using MDPSolver<State, Action, StateHash, ActionHash>::MDPSolver;  // Inherit constructor

    PolicyIteration(MDPCore<State, Action, StateHash, ActionHash>* mdp_core, const double discount_rate, const double policy_evaluation_threshold)
    : MDPSolver<State, Action, StateHash, ActionHash>(mdp_core), m_discount_rate(discount_rate), m_policy_evaluation_threshold(policy_evaluation_threshold) {
    }

    void policy_iteration()
    {
        bool policy_stable;
        do
        {
            policy_evaluation();
            policy_stable = policy_improvement();
        } while (!policy_stable);
    }
};

#endif // POLICYITERATION_H
