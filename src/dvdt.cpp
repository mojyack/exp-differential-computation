#include "util.hpp"

using Float = float;

auto main() -> int {
    // (d^2x)/(dt^2)=ax
    constexpr auto a         = Float(-1.0);
    constexpr auto initial_x = Float(1.0);
    constexpr auto initial_v = Float(0.0);

    const auto time_limit = read_stdin<double>("time limit? ");
    const auto n          = read_stdin<size_t>("n? ");
    const auto dt         = time_limit / n;
    auto       x          = initial_x;
    auto       v          = initial_v;
    auto       t          = Float(0.0);
    printf("n=%lu, dt=%f\n", n, dt);
    for(auto i = 0; i < n; i += 1) {
        v += a * x * dt;
        x += v * dt;
        t += dt;
        fprintf(stderr, "%f %f\n", t, x);
    }
    return 0;
}
