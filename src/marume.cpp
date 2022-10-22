#include <array>
#include <cmath>

#include "float.hpp"

auto main(const int argc, const char* const argv[]) -> int {
    constexpr auto n = 50;

    auto       f = std::array<float, n>();
    auto       g = std::array<float, n>();
    const auto b = (1.0 - std::sqrt(5.0)) / 2.0;

    auto dump = [&f, &g](const size_t i) -> void {
        printf("[%03lu]: %15.6e %15.6e %s %s\n", i, f[i], g[i], Float{.data = f[i]}.to_str().data(), Float{.data = g[i]}.to_str().data());
    };

    f[0] = 1.0;
    f[1] = b;
    g[0] = 1.0;
    g[1] = b;

    dump(0);
    dump(1);

    for(auto i = 2; i < n; i += 1) {
        f[i] = b * f[i - 1];
        g[i] = g[i - 1] + g[i - 2];
        dump(i);
    }

    return 0;
}
