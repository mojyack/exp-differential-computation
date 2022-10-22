#include <cmath>

#include "float.hpp"
#include "util.hpp"

template <class T, int begin, int end>
auto method() -> void {
    using Type = std::conditional_t<std::is_same_v<T, double>, Double, Float>;

    auto a = T(std::pow(T(10), begin));
    auto b = T();
    for(auto n = begin + 1; n <= end; n += 1) {
        a *= 10;
        b = a + 1;
        print<false>("10^", n, " + 1 = ", b, " ");

        if(a == b) {
            print("(error)");
        } else {
            print();
        }
        print("1 = ", Type{.data = 1}.to_str());
        print("a = ", Type{.data = a}.to_str());
        print("b = ", Type{.data = b}.to_str());
    }
}

auto main(const int argc, const char* const argv[]) -> int {
    print("method1 float");
    method<float, 5, 9>();

    print("method2 double");
    method<double, 14, 17>();

    return 0;
}
