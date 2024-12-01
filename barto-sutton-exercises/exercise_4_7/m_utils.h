#ifndef M_UTILS_H
#define M_UTILS_H

#include <cmath>
#include <vector>

double pow(double b, int p);  // Declare the pow function

double next_poisson(double lambda);
double poisson_probability(int k, double lambda);

template <typename T>
struct vector_hash {
    std::size_t operator()(const std::vector<T>& vec) const {
        std::size_t seed = 0;
        for (const auto& elem : vec) {
            seed ^= std::hash<T>()(elem) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

#endif // M_UTILS_H
