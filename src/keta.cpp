#include "float.hpp"

auto main(const int argc, const char* const argv[]) -> int {
    constexpr auto a = 0.780f;
    constexpr auto b = 0.563f;
    constexpr auto c = 0.913f;
    constexpr auto d = 0.659f;
    constexpr auto e = 0.217f;
    constexpr auto f = 0.254f;

    const auto dump = [](const char(&expr)[8], const float value) -> void {
        printf("%s = %16.8f(%s)\n", expr, value, Float{.data = value}.to_str().data());
    };

    dump("d*e    ", d * e);
    dump("b*f    ", b * f);
    dump("d*e-b*f", d * e - b * f);
    dump("a*d    ", a * d);
    dump("b*c    ", b * c);
    dump("a*d-b*c", a * d - b * c);

    const auto x = (d * e - b * f) / (a * d - b * c);
    const auto y = (e - a * x) / b;

    dump("x      ", x);
    dump("y      ", y);
    return 0;
}
