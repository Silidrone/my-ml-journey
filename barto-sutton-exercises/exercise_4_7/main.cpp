#include <iostream>
#include <cmath>   // For exp()
#include <cstdlib> // For rand()
#include <unordered_map>
#include <functional>
#include <limits>
#include <matplot/matplot.h>

constexpr double DISCOUNT_RATE = 0.9f;
constexpr double requests_lambda[] = {3, 4};
constexpr double returns_lambda[] = {3, 2};
constexpr int MAX_CARS_COUNT_PER_LOCATION = 20;
constexpr int REQUEST_FULFILLMENT_REWARD = 10;
constexpr int COST_OF_MOVING_A_CAR = 2;
constexpr int NUMBER_OF_LOCATIONS = 2;
constexpr double POLICY_THRESHOLD_EPSILON = 0.01;

using Timestamp = int;                              // day count (i.e. 1, 2, 3, 4 etc.)
using State = std::array<int, NUMBER_OF_LOCATIONS>; // number of cars at each location at the end of the day (s1, s2)
using Action = int;                                 // net numbers of cars moved between the two locations overnight
using Reward = double;
using Return = Reward;

// Custom hash function for State
template <>
struct std::hash<State>
{
    std::size_t operator()(const State &arr) const noexcept {
        const std::size_t hash1 = std::hash<int>()(arr[0]);
        const std::size_t hash2 = std::hash<int>()(arr[1]);
        return hash1 ^ (hash2 << 1); // Combine the hash of both elements
    }
};

// Custom hash function for std::pair with std::array and int
struct state_action_pair_hash
{
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &pair) const
    {
        const std::size_t hash1 = std::hash<T1>()(pair.first);  // Hash for the first element (State)
        const std::size_t hash2 = std::hash<T2>()(pair.second); // Hash for the second element (Action)
        return hash1 ^ (hash2 << 1);                      // Combine the two hash values
    }
};

double pow(const double b, const int p)
{
    if (p == 0)
        return 1;
    if (p == 1)
        return b;

    return b * pow(b, p - 1);
}

double next_poisson(const double lambda)
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

double poisson_probability(const int k, const double lambda)
{
    if (k < 0)
    {
        throw std::invalid_argument("k must be non-negative.");
    }

    return (pow(lambda, k) * exp(-lambda)) / tgamma(k + 1);
}

std::vector<State> state_space_S;                                               // S
std::vector<Action> action_space_A;                                             // A(s)
std::unordered_map<State, std::vector<std::array<std::pair<int, int>, 2>>> rrs; // Requests - responses for each state
std::unordered_map<std::pair<State, Action>, std::vector<std::tuple<State, Reward, double>>, state_action_pair_hash> dynamics_p;

// Make sure we don't cross MAX_CARS_COUNT_PER_LOCATION when moving cars
void generate_action_space()
{
    for (int i = -5; i <= 5; i++)
    {
        action_space_A.push_back(i);
    }
}

// The below is a "hack". If we generate all possible (requests, returns), the complexity is insanely huge. So, I just generate the ones I know are within the optimal policy.
void generate_requests_returns_combinations(State s)
{
    // Generate all (R1, r1) pairs for location 1
    std::vector<std::pair<int, int>> loc1_rr;
    for (int i = 0; i <= 4; i++)
    {
        for (int j = 0; j <= 4; j++)
        {
            loc1_rr.emplace_back(i, j); // (R1, r1) pairs
        }
    }

    // Generate all (R2, r2) pairs for location 2
    std::vector<std::pair<int, int>> loc2_rr;
    for (int i = 0; i <= 5; i++)
    {
        for (int j = 0; j <= 3; j++)
        {
            loc2_rr.emplace_back(i, j); // (R2, r2) pairs
        }
    }

    // Now generate the Cartesian product of loc1_rr and loc2_rr
    for (const auto &rr1 : loc1_rr)
    {
        for (const auto &rr2 : loc2_rr)
        {
            rrs[s].push_back({rr1, rr2}); // Combine each (R1, r1) with each (R2, r2) if probability > 5%
        }
    }
}


void generate_dynamics_p()
{
    for (State s : state_space_S)
    {
        for (Action a : action_space_A)
        {
            auto state_rrs = rrs[s];

            for (int i = 0; i < state_rrs.size(); i++)
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
                    auto rr_pair = state_rrs[i][loc];
                    int requested = rr_pair.first;
                    int returned = rr_pair.second;
                    probability *= poisson_probability(requested, requests_lambda[loc]) *
                                   poisson_probability(returned, returns_lambda[loc]);

                    // rent out the cars
                    int fulfilled_requests = std::min(requested, s_prime[loc]);
                    reward += fulfilled_requests * REQUEST_FULFILLMENT_REWARD;
                    s_prime[loc] -= fulfilled_requests;

                    // return the cars
                    s_prime[loc] += returned;
                }

                dynamics_p[{s, a}].emplace_back(s_prime, reward, probability);
            }
        }
    }
}

void generate_environment()
{
    for (int i = 0; i <= MAX_CARS_COUNT_PER_LOCATION; i++)
    {
        for (int j = 0; j <= MAX_CARS_COUNT_PER_LOCATION; j++)
        {
            State s = {i, j};
            state_space_S.push_back(s);
            generate_requests_returns_combinations(s);
        }
    }

    generate_action_space();
    generate_dynamics_p();
}

std::unordered_map<std::pair<State, Action>, Return, state_action_pair_hash> q; // Action-value q function
std::unordered_map<State, Action> pi;                                           // Policy function

// For easier debugging with unordered_map
// template class std::unordered_map<State, Action>;
// template class std::unordered_map<std::pair<State, Action>, Return, state_action_pair_hash>;
// template class std::unordered_map<State, std::vector<std::array<std::pair<int, int>, 2>>>;
// template class std::unordered_map<std::pair<State, Action>, std::vector<std::tuple<State, Reward, double>>, state_action_pair_hash>;

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

void policy_evaluation()
{
    Return delta;
    do
    {
        delta = 0;
        for (State &s : state_space_S)
        {
            for (Action &a : action_space_A)
            {
                const Return old_value = q_f(s, a);
                Return new_value = 0;
                auto transitions = dynamics_p[{s, a}];

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
        const Return old_value = q_f(s, pi_f(s));
        Return max_return = std::numeric_limits<Return>::lowest(); // in case we decide to use negative rewards in the future
        Action maximizing_action;
        for (Action &a : action_space_A)
        {
            Return candidate_return = q_f(s, a);
            if (candidate_return > max_return)
            {
                max_return = candidate_return;
                maximizing_action = a;
            }
        }

        // maximizing_action should, theoretically, always be initialized
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
    do
    {
        policy_evaluation();
        policy_stable = policy_improvement();
    } while (!policy_stable);
}

void plot_policy()
{
    int grid_size_x = 21;
    int grid_size_y = 21;

    matplot::vector_1d x, y;
    matplot::vector_2d Z;

    Z.resize(grid_size_x, std::vector<double>(grid_size_y, 0));

    for (int i = 0; i < grid_size_x; ++i)
    {
        x.push_back(i);
    }

    for (int j = 0; j < grid_size_y; ++j)
    {
        y.push_back(j);
    }

    auto [X, Y] = matplot::meshgrid(x, y);

    for (const State &s : state_space_S)
    {
        Z[s[0]][s[1]] = static_cast<double>(pi[s]);
    }

    matplot::contour(X, Y, Z);

    matplot::xlabel("Cars at second location");
    matplot::ylabel("Cars at first location");
    matplot::title("Policy Contour");

    matplot::show();
}

int main()
{
    generate_environment();

    policy_iteration();
    plot_policy();
    return 0;
}