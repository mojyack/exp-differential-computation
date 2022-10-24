#include <array>

#include "util.hpp"

using Float = float;

struct Planet {
    Float m;
    Float px;
    Float py;
    Float vx;
    Float vy;
};

auto main() -> int {
    constexpr auto g = 4.0;

    const auto time_limit = read_stdin<double>("time limit? ");
    const auto n          = read_stdin<size_t>("n? ");

    auto       dt    = Float(time_limit / n);
    auto       o1    = Planet{.m = 1.0, .px = 1.0, .py = 0.0, .vx = 0.0, .vy = 1.0};
    auto       o2    = Planet{.m = 1.0, .px = -1.0, .py = 0.0, .vx = 0.0, .vy = -1.0};
    const auto force = [&o1, &o2]() -> std::array<Float, 2> {
        const auto dx  = (o1.px - o2.px);
        const auto dy  = (o1.py - o2.py);
        const auto r   = std::sqrt(dx * dx + dy * dy);
        const auto mmg = o1.m * o2.m * g;
        const auto fx  = -mmg * dx / (r * r * r);
        const auto fy  = -mmg * dy / (r * r * r);
        return {Float(fx), Float(fy)};
    };

    for(auto i = 0; i < n; i += 1) {
        const auto [fx, fy] = force();

        o1.vx += fx * dt / o1.m;
        o1.vy += fy * dt / o1.m;
        o1.px += o1.vx * dt;
        o1.py += o1.vy * dt;

        o2.vx += -fx * dt / o2.m;
        o2.vy += -fy * dt / o2.m;
        o2.px += o2.vx * dt;
        o2.py += o2.vy * dt;

        fprintf(stderr, "%f %f %f %f\n", o1.px, o1.py, o2.px, o2.py);
    }

    return 0;
}
