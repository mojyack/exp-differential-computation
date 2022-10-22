#include "float.hpp"
#include "util.hpp"

auto method1(const float a, const int n) -> float {
    auto s = float(0);
    for(auto i = 1; i <= n; i += 1) {
        s += a;
    }
    return s;
}

auto method2(const float a, const int n) -> float {
    auto s = float(0);
    auto t = float(0);

    for(auto i = 1; i <= n; i += 1) {
        const auto w  = s + a;
        const auto tu = w - s;
        const auto tl = a - tu;
        s += tu;
        t += tl;
    }

    return s + t;
}

auto method2d(const float a, const int n) -> float {
    auto s = float(0);
    auto t = float(0);

    for(auto i = 1; i <= n; i += 1) {
        const auto w  = s + a;
        const auto tu = w - s;
        const auto tl = a - tu;

        if(i % 100 == 0) {
            print("<i = ", i, ">");
            print("a         = ", Float{.data = a}.to_str());
            print("w         = ", Float{.data = w}.to_str());
            print("s(before) = ", Float{.data = s}.to_str());
            print("t(before) = ", Float{.data = t}.to_str());
            print("tu        = ", Float{.data = tu}.to_str());
            print("tl        = ", Float{.data = tl}.to_str());
        }

        s += tu;
        t += tl;

        if(i % 100 == 0) {
            print("s(after)  = ", Float{.data = s}.to_str());
            print("t(after)  = ", Float{.data = t}.to_str());
        }
    }

    return s + t;
}

auto method3(const double a, const int n) -> double {
    auto s = double(0);
    for(auto i = 1; i <= n; i += 1) {
        s += a;
    }
    return s;
}

auto main(const int argc, const char* const argv[]) -> int {
loop:
    const auto a = float(read_stdin<double>("a?: "));
    const auto n = read_stdin<int>("n?: ");
    if(n == 0) {
        return 0;
    }
    printf("a=%10.8f n=%08d n*a=%16.8f\n", a, n, a * n);

    const auto result_1 = method1(a, n);
    const auto result_2 = method2(a, n);
    const auto result_3 = method3(a, n);
    printf("method1: %16.8f(%16.8f)\n", result_1, result_1 - a * n);
    printf("method2: %16.8f(%16.8f)\n", result_2, result_2 - a * n);
    printf("method3: %16.8f(%16.8f)\n", result_3, result_3 - a * n);
    printf("method2: %16.8f\n", method2d(a, n));

    goto loop;
}
