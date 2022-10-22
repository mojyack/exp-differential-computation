#pragma once
#include <iomanip>

#include "util.hpp"

union Float {
    float data;
    struct {
        uint32_t fraction : 23;
        uint32_t exponent : 8;
        uint32_t sign : 1;
    } __attribute__((packed)) bits;

    auto to_str() const -> std::string {
        if(bits.exponent == 0 && bits.fraction == 0) {
            return "(zero)";
        } else if(bits.exponent == 255) {
            if(bits.fraction == 0) {
                return "(inf)";
            } else {
                return "(nan)";
            }
        }

        return build_string(bits.sign ? "-" : "+", "1.", std::bitset<23>(bits.fraction), "*10^", std::setw(3), int32_t(bits.exponent) - 127);
    }
};

union Double {
    double data;
    struct {
        uint64_t fraction : 52;
        uint64_t exponent : 11;
        uint64_t sign : 1;
    } __attribute__((packed)) bits;

    auto to_str() const -> std::string {
        if(bits.exponent == 0 && bits.fraction == 0) {
            return "(zero)";
        } else if(bits.exponent == 2047) {
            if(bits.fraction == 0) {
                return "(inf)";
            } else {
                return "(nan)";
            }
        }

        return build_string(bits.sign ? "-" : "+", "1.", std::bitset<52>(bits.fraction), "*10^", std::setw(3), int32_t(bits.exponent) - 1023);
    }
};
