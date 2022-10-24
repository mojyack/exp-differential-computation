#include "util.hpp"

using Float = float;

auto main() -> int {
    // dx/dt=ax
    constexpr auto a         = Float(-1.0);
    constexpr auto initial_x = Float(1.0);

    const auto n  = read_stdin<size_t>("n? ");
    const auto dt = 1.0 / n;
    auto       x  = initial_x;
    auto       t  = Float(0.0);
    printf("n=%lu, dt=%f\n", n, dt);
    for(auto i = 0; i < n; i += 1) {
        const auto v = a * x * dt;
        x += v;
        t += dt;
    }
    fprintf(stderr, "%lu %f\n", n, x);
    return 0;
}
