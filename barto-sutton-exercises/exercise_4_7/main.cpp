#include <iostream>
#include <cmath>   // For exp()
#include <cstdlib> // For rand()
#include <unordered_map>
#include <functional>
#include <limits>
#include "matplotlibcpp.h"

const double DISCOUNT_RATE = 0.9f;
const double requests_lambda[] = {3, 4};
const double returns_lambda[] = {3, 2};
const int MAX_CARS_COUNT_PER_LOCATION = 20;
const int REQUEST_FULLFILMENT_REWARD = 10;
const int COST_OF_MOVING_A_CAR = 2;
const int NUMBER_OF_LOCATIONS = 2;
const double POLICY_THRESHOLD_EPSILON = 100;

double pow(double b, int p)
{
    if (p == 0)
        return 1;
    if (p == 1)
        return b;

    return b * pow(b, p - 1);
}

double next_poisson(double lambda)
{
    double L = exp(-lambda); // e^(-λ)
    double p = 1.0;
    int k = 0;

    do
    {
        k++;
        double u = rand() / (RAND_MAX + 1.0); // Generate uniform random number u between 0 and 1
        p *= u;                               // Update probability
    } while (p > L);

    return k - 1; // Subtract 1 because we start from 0 events
}

double poisson_probability(int k, double lambda)
{
    if (k < 0)
    {
        throw std::invalid_argument("k must be non-negative.");
    }

    double probability = (pow(lambda, k) * exp(-lambda)) / tgamma(k + 1);
    return probability;
}

using Timestamp = int;                              // day count (i.e. 1, 2, 3, 4 etc.)
using State = std::array<int, NUMBER_OF_LOCATIONS>; // number of cars at each location at the end of the day (s1, s2)
using Action = int;                                 // net numbers of cars moved between the two locations overnight
using Reward = double;
using Return = Reward;

std::vector<State> state_space_S;

void generate_state_space()
{
    for (int i = 0; i <= MAX_CARS_COUNT_PER_LOCATION; i++)
    {
        for (int j = 0; j <= MAX_CARS_COUNT_PER_LOCATION; j++)
        {
            state_space_S.push_back({i, j});
        }
    }
}

// Make sure we don't cross MAX_CARS_COUNT_PER_LOCATION when moving cars
std::vector<Action> generate_action_space(State s)
{
    std::vector<Action> action_space;
    auto n = s[0];
    auto k = s[1];
    int lower_bound = -std::min(k, MAX_CARS_COUNT_PER_LOCATION - n);
    int upper_bound = std::min(n, MAX_CARS_COUNT_PER_LOCATION - k);
    for (int i = lower_bound; i <= upper_bound; i++)
    {
        action_space.push_back(i);
    }

    return action_space;
}

// Custom hash function for std::pair with std::array and int
struct state_action_pair_hash
{
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &pair) const
    {
        std::size_t hash1 = std::hash<T1>()(pair.first);  // Hash for the first element (State)
        std::size_t hash2 = std::hash<T2>()(pair.second); // Hash for the second element (Action)
        return hash1 ^ (hash2 << 1);                      // Combine the two hash values
    }
};

// Custom hash function for State
template <>
struct std::hash<State>
{
    std::size_t operator()(const State &arr) const
    {
        std::size_t hash1 = std::hash<int>()(arr[0]);
        std::size_t hash2 = std::hash<int>()(arr[1]);
        return hash1 ^ (hash2 << 1); // Combine the hash of both elements
    }
};

std::unordered_map<std::pair<State, Action>, Return, state_action_pair_hash> q; // Action-value q function
std::unordered_map<State, Action> pi;                                           // Policy function

Return q_f(State s, Action a)
{
    auto it = q.find({s, a});
    if (it == q.end())
    {
        return 0.0f;
    }

    return it->second;
}

Action pi_f(State s)
{
    auto it = pi.find(s);
    if (it == pi.end())
    {
        return 0;
    }

    return it->second;
}

std::vector<std::array<std::pair<int, int>, NUMBER_OF_LOCATIONS>> generate_requests_returns_combinations(State s)
{
    std::vector<std::array<std::pair<int, int>, 2>> result;

    // Generate all (R1, r1) pairs for location 1
    std::vector<std::pair<int, int>> loc1_rr;
    for (int i = 0; i <= s[0]; i++)
    {
        for (int j = 0; j <= MAX_CARS_COUNT_PER_LOCATION - s[0]; j++)
        {
            loc1_rr.push_back({i, j}); // (R1, r1) pairs
        }
    }

    // Generate all (R2, r2) pairs for location 2
    std::vector<std::pair<int, int>> loc2_rr;
    for (int i = 0; i <= s[1]; i++)
    {
        for (int j = 0; j <= MAX_CARS_COUNT_PER_LOCATION - s[1]; j++)
        {
            loc2_rr.push_back({i, j}); // (R2, r2) pairs
        }
    }

    // Now generate the Cartesian product of loc1_rr and loc2_rr
    for (const auto &rr1 : loc1_rr)
    {
        for (const auto &rr2 : loc2_rr)
        {
            result.push_back({rr1, rr2}); // Combine each (R1, r1) with each (R2, r2)
        }
    }

    return result;
}

std::vector<std::tuple<State, Reward, double>> dynamics_p(State s, Action a)
{
    std::vector<std::tuple<State, Reward, double>> result;

    auto requests_rr_combinations = generate_requests_returns_combinations(s);

    for (int i = 0; i < requests_rr_combinations.size(); i++)
    {
        State s_prime = s;
        Reward reward = 0;

        // move the cars
        reward -= abs(a * COST_OF_MOVING_A_CAR);
        s_prime[0] -= a;
        s_prime[1] += a;

        double probability = 1;
        for (int loc = 0; loc < s.size(); loc++)
        {
            auto rr_pair = requests_rr_combinations[i][loc];
            int requested = rr_pair.first;
            int returned = rr_pair.second;
            probability *= poisson_probability(requested, requests_lambda[loc]) *
                           poisson_probability(returned, returns_lambda[loc]);

            // rent out the cars
            int fulfilled_requests = std::min(requested, s_prime[loc]);
            reward += fulfilled_requests * REQUEST_FULLFILMENT_REWARD;
            s_prime[loc] -= fulfilled_requests;

            // return the cars
            s_prime[loc] += returned;
        }

        result.push_back({s_prime, reward, probability});
    }

    return result;
}

void policy_evaluation()
{
    Return delta;
    do
    {
        delta = 0;
        for (State &s : state_space_S)
        {
            for (Action &a : generate_action_space(s))
            {
                Return old_value = q_f(s, a);
                Return new_value = 0;
                auto transitions = dynamics_p(s, a);

                for (auto transition : transitions)
                {
                    State s_prime = std::get<0>(transition);
                    Reward r = std::get<1>(transition);
                    double probability = std::get<2>(transition);
                    new_value += probability * (r + DISCOUNT_RATE * q_f(s_prime, pi_f(s_prime)));
                }

                q[{s, a}] = new_value;
                delta = std::max(delta, std::abs(old_value - new_value));
            }
        }
    } while (delta > POLICY_THRESHOLD_EPSILON);
}

bool policy_improvement()
{
    bool policy_stable = true;
    for (State &s : state_space_S)
    {
        Return old_value = q_f(s, pi_f(s));
        Return max_return = std::numeric_limits<Return>::lowest(); // in case we decide to use negative rewards in the future
        Action maximizing_action;
        for (Action &a : generate_action_space(s))
        {
            Return candidate_return = q_f(s, a);
            if (candidate_return > max_return)
            {
                max_return = candidate_return;
                maximizing_action = a;
            }
        }

        pi[s] = maximizing_action;

        if (old_value != max_return)
        {
            policy_stable = false;
        }
    }

    return policy_stable;
}

void policy_iteration()
{
    bool policy_stable;
    do {
        policy_evaluation();
        policy_stable = policy_improvement();
    } while(!policy_stable);
}

namespace plt = matplotlibcpp;

void plot_policy() {
    std::vector<int> X, Y, Z;
    
    for (const State& s : state_space_S) {
        X.push_back(s[0]);
        Y.push_back(s[1]);
        Z.push_back(pi[s]); // Action for the policy
    }

    plt::contour(X, Y, Z);
    plt::xlabel("Cars at Location 1");
    plt::ylabel("Cars at Location 2");
    plt::title("Policy Contour");
    plt::show();
}

int main()
{
    generate_state_space();
    policy_iteration();
    plot_policy();
    return 0;
}