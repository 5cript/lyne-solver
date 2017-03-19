#ifndef NEURAL_HELPERS_H_INCLUDED
#define NEURAL_HELPERS_H_INCLUDED

#include <cmath>

namespace internal
{
    constexpr long long factorial (long long index) {
        return (index != 1LL && index != 0LL) ? factorial(index - 1LL) * index : 1LL;
    }

    template <typename T>
    constexpr T euler_constant_estimator(int rbi = 20) {
        return (rbi == 1) ? static_cast <T> (1) :
            euler_constant_estimator <T> (rbi - 1) +
            static_cast <T> (rbi % 2 ? static_cast <T> (1LL) : static_cast <T> (-1LL)) /
            static_cast <T> (factorial(static_cast <long long> (rbi-1)))
        ;
    }

    constexpr long double e = 1.L / euler_constant_estimator <long double>();
    constexpr long double sigmoidal_slope = -4.9L;
}

template <typename T>
T sigmoid (T t) {
    return static_cast <T>(2) / (static_cast <T> (1) + std::pow <long double>(internal::e, internal::sigmoidal_slope * t));
}

#endif // NEURAL_HELPERS_H_INCLUDED
