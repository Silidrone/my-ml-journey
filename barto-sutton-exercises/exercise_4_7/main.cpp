#include "CarRentalEnvironment.h"
#include "VValuePolicyIteration.h"
#include <matplot/matplot.h>

template <typename State, typename Action, typename StateHash = std::hash<State>, typename ActionHash = std::hash<Action>>
void plot_policy(PolicyIteration<State, Action, StateHash, ActionHash> &policy_iteration)
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

    for (const State &s : policy_iteration.m_mdp.S())
    {
        Z[s[0]][s[1]] = static_cast<double>(policy_iteration.pi(s));
    }

    matplot::contour(X, Y, Z);

    matplot::xlabel("Cars at second location");
    matplot::ylabel("Cars at first location");
    matplot::title("Policy Contour");

    matplot::show();
}


int main() {
    CarRentalEnvironment car_rental_environment;
    VValuePolicyIteration value_policy_iteration(&car_rental_environment);

    value_policy_iteration.policy_iteration();
    plot_policy();

}
