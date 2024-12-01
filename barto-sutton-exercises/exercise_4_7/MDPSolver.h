#ifndef MDPSOLVER_H
#define MDPSOLVER_H

#include "MDPCore.h"
#include <stdexcept>

template <typename State, typename Action, typename StateHash = std::hash<State>, typename ActionHash = std::hash<Action>>
class MDPSolver {
public:
    using MDPCoreFullTypeName = MDPCore<State, Action, StateHash, ActionHash>;
    using Return = typename MDPCoreFullTypeName::Return;
    using Policy = std::unordered_map<State, Action, StateHash>;
protected:
    using state_action_pair_hash = typename MDPCoreFullTypeName::state_action_pair_hash;

    Policy m_pi; // Policy pi function
    std::unordered_map<State, typename MDPCoreFullTypeName::Return, StateHash> m_v;  // State-value v function
    std::unordered_map<std::pair<State, Action>, Return, state_action_pair_hash> m_Q; // Action-value Q function

    MDPCoreFullTypeName* m_mdp;

    virtual void initialize_policy() = 0;
    virtual void initialize_value_functions() = 0;

public:
    virtual ~MDPSolver() = default;

    explicit MDPSolver(MDPCore<State, Action, StateHash, ActionHash>* mdp_core)
        : m_mdp(mdp_core) {}

    Action pi(State s)
    {
        auto it = m_pi.find(s);
        if (it == m_pi.end()) throw std::runtime_error("Error: Invalid state provided for the pi policy function.");

        return it->second;
    }

    Return v(State s)
    {
        auto it = m_v.find(s);
        if (it == m_v.end()) throw std::runtime_error("Error: Invalid state provided for the v-value function.");

        return it->second;
    }

    Return Q(State s, Action a)
    {
        auto it = m_Q.find({s, a});
        if (it == m_Q.end()) throw std::runtime_error("Error: Invalid state-action pair provided for the Q-value function.");

        return it->second;
    }

    Policy get_policy() {
        return m_pi;
    }
};

#endif // MDPSOLVER_H
