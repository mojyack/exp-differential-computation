#pragma once
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

// concat multiple values into a string
// example:
//  build_strings("1 + 2 = ", 1 + 2) // "1 + 2 = 3"
template <class... Args>
auto build_string(Args... args) -> std::string {
    auto ss = std::stringstream();
    (ss << ... << args);
    return ss.str();
}

// die with a message
template <class... Args>
[[noreturn]] auto panic(Args... args) -> void {
    auto ss = std::stringstream();
    (ss << ... << args) << std::endl;
    throw std::runtime_error(ss.str());
}

// if !cond, panic
template <class... Args>
auto dynamic_assert(const bool cond, Args... args) -> void {
    if(!cond) {
        panic(args...);
    }
}

// print multiple values to stdout
// use print<false>(...) to prevent writing '\n'
template <bool nl = true, class... Args>
auto print(Args&&... args) -> void {
    if constexpr (nl) {
        (std::cout << ... << args) << std::endl;
    } else {
        (std::cout << ... << args);
    }
}

// read string from stdin
// prompt message can be displayed
// example:
//  auto arg = read_line("what's your name?")
inline auto read_line(const std::optional<std::string_view> prompt = std::nullopt) -> std::string {
    if(prompt) {
        print<false>(*prompt);
    }
    auto line = std::string();
    std::getline(std::cin, line);
    return line;
}

template <class T>
constexpr auto false_v = false;

// read number from stdin
// currently supports 3 types as a template parameter: int, size_t, double
// example:
//  auto age     = read_stdin<int>("age?");
//  auto size    = read_stdin<size_t>("size?");
//  auto length  = read_stdin<double>();
template <class T>
inline auto read_stdin(const std::optional<std::string_view> prompt = std::nullopt) -> T {
    while(true) {
        const auto line = read_line(prompt);
        try {
            if constexpr(std::is_same_v<T, int>) {
                return std::stoi(line);
            } else if constexpr(std::is_same_v<T, double>) {
                return std::stod(line);
            } else if constexpr(std::is_same_v<T, size_t>) {
                return std::stoull(line);
            } else {
                static_assert(false_v<T>, "unsupported type");
            }
        } catch(const std::invalid_argument&) {
            print("invalid input");
            continue;
        }
    }
}
