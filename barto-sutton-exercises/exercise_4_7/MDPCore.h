#ifndef MDPCORE_H
#define MDPCORE_H

#include <vector>
#include <unordered_map>
#include <tuple>

template <typename State, typename Action, typename StateHash = std::hash<State>, typename ActionHash = std::hash<Action>>
class MDPCore {
public:
    struct state_action_pair_hash {
        std::size_t operator()(const std::pair<State, Action>& pair) const {
            const std::size_t hash1 = StateHash()(pair.first);
            const std::size_t hash2 = ActionHash()(pair.second);
            return hash1 ^ (hash2 << 1); // Combine the two hashes
        }
    };

    using TimeStep = int;
    using Reward = double;
    using Return = Reward;
    using Transition = std::tuple<State, Reward, double>;
    using Dynamics = std::unordered_map<std::pair<State, Action>, std::vector<Transition>, state_action_pair_hash>;
protected:
    std::vector<State> m_S;   // state space: S
    std::vector<Action> m_A;  // action space: A
    Dynamics m_dynamics;      // Dynamics p function
    TimeStep m_T;

    virtual void initialize_state_space() {};
    virtual void initialize_action_space() {};
    virtual void initialize_dynamics() {};
public:
    virtual ~MDPCore() = default;

    MDPCore() : m_T(0) {}

    virtual void initialize() {
        initialize_state_space();
        initialize_action_space();
        initialize_dynamics();
    }

    void increment_time_step() {
        ++m_T;
    }

    [[nodiscard]] TimeStep get_time_step() const {
        return m_T;
    }

    std::vector<Transition> p(State s, Action a) {
        return m_dynamics[{s, a}];
    }

    Dynamics dynamics() {
        return m_dynamics;
    }

    std::vector<State> S() {
        return m_S;
    }

    std::vector<Action> A() {
        return m_A;
    }
};

#endif // MDPCORE_H
