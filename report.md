## 実験環境について
C/C++言語では、浮動小数点数をIEEE 754で定められた形式でメモリに格納する。  
`float`、`double`、`long double`型をそれぞれIEEE 754のどの形式で表すかは実装依存であるが、本レポートでは、実験に使用した`clang 14.0.6 x86_64-pc-linux-gnu`に合わせ、`float`がIEEE 754の`binary32`、`double`がIEEE 754の`binary64`であるような環境を想定する。

## 浮動小数点型について
IEEE 754では、浮動小数点型を符号部、仮数部、指数部の3つの部分に分けて表現することを規定している。
`binary32`型は、最下位ビットから、仮数部23ビット、指数部8ビット、符号部1ビットの順に並ぶ。
`binary64`型は、最下位ビットから、仮数部52ビット、指数部11ビット、符号部1ビットの順に並ぶ。
仮数部を`f`、指数部を`e`、符号部を`s`とすると、格納されている値`v`は、以下のように計算できる。
`v = (-1)^s * (1.f) * 2^(e-m)`
ただし`m`はbinary32なら127、binary64なら1023である。

以上を踏まえて、以下の実験では、以下のプログラム片(図1)を用いて、浮動小数点型の内部表現の出力を行っている箇所がある。

*図1 浮動小数点型変数の内部表現を得るためのクラス*
```C++
/* float.hpp */
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
```
`Float::to_str()`で作られる文字列は基本的に2進数表記だが、最後の指数部だけ10進数表記であることに注意してほしい。

## 実験 情報落ち1
### 目的
浮動小数点数が大きくなると、情報落ちにより誤差が大きくなることを確認する。
### 方法
ある変数xと、それに1を足したものを比較する。これは一致しないはずだが、xが大きくなると情報落ちにより一致するようになる。これを以下のプログラム(図2)で観測する。
*図2 joho1.cpp*
```C++
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
```
### 結果
以下の出力、図3が得られた。  
*図3 joho1の出力*
```
method1 float
10^6 + 1 = 1e+06
1 = +1.00000000000000000000000*10^0
a = +1.11101000010010000000000*10^19
b = +1.11101000010010000010000*10^19
10^7 + 1 = 1e+07
1 = +1.00000000000000000000000*10^0
a = +1.00110001001011010000000*10^23
b = +1.00110001001011010000001*10^23
10^8 + 1 = 1e+08 (error)
1 = +1.00000000000000000000000*10^0
a = +1.01111101011110000100000*10^26
b = +1.01111101011110000100000*10^26
10^9 + 1 = 1e+09 (error)
1 = +1.00000000000000000000000*10^0
a = +1.11011100110101100101000*10^29
b = +1.11011100110101100101000*10^29
method2 double
10^15 + 1 = 1e+15
1 = +1.0000000000000000000000000000000000000000000000000000*10^0
a = +1.1100011010111111010100100110001101000000000000000000*10^49
b = +1.1100011010111111010100100110001101000000000000001000*10^49
10^16 + 1 = 1e+16 (error)
1 = +1.0000000000000000000000000000000000000000000000000000*10^0
a = +1.0001110000110111100100110111111000001000000000000000*10^53
b = +1.0001110000110111100100110111111000001000000000000000*10^53
10^17 + 1 = 1e+17 (error)
1 = +1.0000000000000000000000000000000000000000000000000000*10^0
a = +1.0110001101000101011110000101110110001010000000000000*10^56
b = +1.0110001101000101011110000101110110001010000000000000*10^56
```
floatを使った場合10\^8 から、doubleを使った場合10\^16 からエラーとなった。
### 考察
内部表現を見ながら、何が起こったのかを考察する。  例として、ここではmethod1を挙げる。
`10^6 + 1`の場合を見てみると、これは`+1.11101000010010000000000*10^19 + +1.00000000000000000000000*10^0`である。指数を19に揃えると、1.0は`+0.00000000000000000010000*10^19`なので、仮数部は23桁に収まっている。  
一方で`10^8 + 1 `の場合、これは`+1.01111101011110000100000*10^26 + +1.00000000000000000000000*10^0`である。指数を26に合わせると、1.0は`+0.00000000000000000000000001*10^26`なので、仮数部は23桁に収まっていない。よって、1.0を足したことで立ったビットは切り捨てられ、誤差が生じる。

## 実験 情報落ち2
### 目的
浮動小数点型を変えたり、計算方法を工夫することで誤差を減らすことができることを確認する
### 方法
`a * x`を、積ではなく、`a`を`n`回加算することで計算し、生じる誤差を観察する。1) floatを使う 2) floatを使うが、計算を工夫する 3) doubleを使う の3つの方法(それぞれをmethod1、2、3と呼ぶことにする)で計算する。  
以下のコード(図4)を用いた。  
*図4 joho2.cpp*
```C++
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
```

### 結果
図5は、`a=1.001`、`n=100000`で実行したときの出力である。  
*図5 joho2 を a=1.001、n=100000 で実行した結果*
```
a?: 1.001
n?: 100000
a=1.00100005 n=00100000 n*a= 100100.00781250
method1:  100047.93750000(    -52.07031250)
method2:  100099.99218750(     -0.01562500)
method3:  100100.00467300(     -0.00313950)
```
doubleを使うmethod3が最も誤差が少なく、次いでmethod2、method1の順という結果になった。
### 考察
method3の精度が良いのは予想通りだが、同じfloatを使うmethod1,2でも誤差はかなりの差がある。method2で行われた誤差を減らす工夫を考察する。  
まず、具体的にイメージしやすくするため、method2関数にデバッグ用の出力を加えた、method2d関数(図6)を先程と同じ入力を与えて実行する。図7はその出力の抜粋である。  
*図6 method2d関数*
```C++
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
```
*図7 joho2のmethod2d関数の出力から、100000回目のループ部分抜粋*
```
<i = 100000>
a         = +1.00000000010000011000101*10^0
w         = +1.10000110110011111111000*10^16
s(before) = +1.10000110110011101111000*10^16
t(before) = +1.10100000011001111011011*10^5
tu        = +1.00000000000000000000000*10^0
tl        = +1.00000110001010000000000*10^-10
s(after)  = +1.10000110110011111111000*10^16
t(after)  = +1.10100000011010011100001*10^5
```
また、method2の計算部分を再度抜き出して掲載する(図8)。  
*図8 method2の計算部分*
```
const auto w  = s + a;
const auto tu = w - s;
const auto tl = a - tu;
s += tu;
t += tl;
```
図8の一行目、`s + a`では、図7からもわかるように、ループを繰り返すにつれ`s`も大きくなり、生じる情報落ちが激しくなっていく。つまり、`w`のうち`a`の要素は下位ビットが切り捨てられた状態になっている。よって、次の行で誤差を含む`w`から`s`を引き、`a`のうち上位ビット部分のみを抜き出している(`tu`)。さらに次の行では、`a`から`a`の上位ビット(`tu`)を引き、`a`の下位ビット部分を抜き出している(`tl`)。  
こうして、`a`を上位と下位に分け、 `s`には`a`の上位ビットを、`t`には`a`の下位ビットを加算していくことにより、精度を向上するというのが、method2の工夫である。

## 実験 桁落ち
### 目的
近い値の2つの浮動小数点数の差をとると相対誤差が大きくなる、桁落ちを確認する。
### 方法
`a=0.780`、`b=0.563`、`c=0.913`、`d=0.659`、`e=0.217`、`f=0.254`の定数を定義する。これらに対して、`de`、`bf`、`de-bf`、`ad`、`bc`、`ad-bc`、`(de-bf)/(ad-bc)`(以後`x`とする)、`(e-ax)/b`(以後`y`とする)をそれぞれ手計算した場合とプログラミングで計算した場合の値を比較し、相対誤差を求める。  
プログラムは図9のものを用いた。  
*図9 keta.cpp*
```C++
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
```
### 結果
図9のプログラムの出力が図10である。  
*図10 keta.cpp の出力*
```
d*e     =       0.14300299(+1.00100100110111101100000*10^-3)
b*f     =       0.14300200(+1.00100100110111100011110*10^-3)
d*e-b*f =       0.00000098(+1.00001000000000000000000*10^-20)
a*d     =       0.51401997(+1.00000111001011011010000*10^-1)
b*c     =       0.51401901(+1.00000111001011011000000*10^-1)
a*d-b*c =       0.00000095(+1.00000000000000000000000*10^-20)
x       =       1.03125000(+1.00001000000000000000000*10^0)
y       =      -1.04329479(-1.00001011000101010101111*10^0)
```
この結果から、手計算と数値計算、及びそれらの誤差と相対誤差を図11にまとめた。  
*図11 手計算と数値計算の誤差*  

|  | 手計算 | 数値計算 | 誤差 | 相対誤差 |  
|--|--|--|--|--|  
| de | 0.143003 | 0.14300299 | 0.000000009999999995 | 0.00000006992860286  
| bf | 0.143002 | 0.14300200 | 0 | 0 |  
| de-bf | 0.000001 | 0.00000098 | 0.00000002 | 0.02 |  
| ad | 0.514020 | 0.51401997 | 0.00000003000000004 | 0.00000005836348788 |  
| bc | 0.514019 | 0.51401901 | 0.00000001000000005 | 0.00000001945453388 |  
| ad-bc | 0.000001 | 0.00000095 | 0.00000005 | 0.05 |  
| x | 1 | 1.03125000 | 0.03125 | 0.03125 |  
| y | -1 | -1.04329479 | 0.04329479 | 0.04329479 |

### 考察
相対誤差を見ると、1%以上の誤差が生じたのは`de-bf`、`ad-bc`、`x`、`y`だった。`de-bf`、`ad-bc`のように近い値で差をとった場合や、`x`、`y`のように桁落ちした値を使って計算したものは誤差が大きくなることが確認できた。

## 実験 丸め誤差
### 目的
浮動小数点数の丸め誤差が蓄積していく様子を確認する
### 方法
以下のプログラム(図12)を実行する。  
*図12 marume.cpp*
```C++
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
```
### 結果
図12のプログラムの出力が図13である。  
*図13 marume.cpp の出力*
```
[000]:    1.000000e+00    1.000000e+00 +1.00000000000000000000000*10^  0 +1.00000000000000000000000*10^  0
[001]:   -6.180340e-01   -6.180340e-01 -1.00111100011011101111010*10^ -1 -1.00111100011011101111010*10^ -1
[002]:    3.819660e-01    3.819660e-01 +1.10000111001000100001101*10^ -2 +1.10000111001000100001100*10^ -2
[003]:   -2.360680e-01   -2.360680e-01 -1.11100011011101111001110*10^ -3 -1.11100011011101111010000*10^ -3
[004]:    1.458980e-01    1.458980e-01 +1.00101010110011001001011*10^ -3 +1.00101010110011001001000*10^ -3
[005]:   -9.016994e-02   -9.017003e-02 -1.01110001010101100000101*10^ -4 -1.01110001010101100010000*10^ -4
[006]:    5.572809e-02    5.572796e-02 +1.11001000100001100100011*10^ -5 +1.11001000100001100000000*10^ -5
[007]:   -3.444185e-02   -3.444207e-02 -1.00011010001001011100111*10^ -5 -1.00011010001001100100000*10^ -5
[008]:    2.128624e-02    2.128589e-02 +1.01011100110000001111001*10^ -6 +1.01011100101111111000000*10^ -6
[009]:   -1.315562e-02   -1.315618e-02 -1.10101111000101010101001*10^ -7 -1.10101111000110100000000*10^ -7
[010]:    8.130619e-03    8.129716e-03 +1.00001010011011001001010*10^ -7 +1.00001010011001010000000*10^ -7
[011]:   -5.024999e-03   -5.026460e-03 -1.01001001010100010111111*10^ -8 -1.01001001011010100000000*10^ -8
[012]:    3.105620e-03    3.103256e-03 +1.10010111000011110101001*10^ -9 +1.10010110110000000000000*10^ -9
[013]:   -1.919379e-03   -1.923203e-03 -1.11110111001001110101010*10^-10 -1.11111000001010000000000*10^-10
[014]:    1.186241e-03    1.180053e-03 +1.00110110111101110100111*10^-10 +1.00110101010110000000000*10^-10
[015]:   -7.331374e-04   -7.431507e-04 -1.10000000011000000000101*10^-11 -1.10000101101000000000000*10^-11
[016]:    4.531039e-04    4.369020e-04 +1.11011011000111010010010*10^-12 +1.11001010001000000000000*10^-12
[017]:   -2.800336e-04   -3.062487e-04 -1.00100101101000101111001*10^-12 -1.01000001001000000000000*10^-12
[018]:    1.730703e-04    1.306534e-04 +1.01101010111101000110011*10^-13 +1.00010010000000000000000*10^-13
[019]:   -1.069633e-04   -1.755953e-04 -1.11000000101000101111110*10^-14 -1.01110000010000000000000*10^-13
[020]:    6.610696e-05   -4.494190e-05 +1.00010101010001011101000*10^-14 -1.01111001000000000000000*10^-15
[021]:   -4.085635e-05   -2.205372e-04 -1.01010110101110100101100*10^-15 -1.11001110100000000000000*10^-13
[022]:    2.525061e-05   -2.654791e-04 +1.10100111101000101000111*10^-16 -1.00010110011000000000000*10^-12
[023]:   -1.560574e-05   -4.860163e-04 -1.00000101110100100010000*10^-16 -1.11111101101000000000000*10^-12
[024]:    9.644875e-06   -7.514954e-04 +1.01000011101000001101100*10^-17 -1.10001010000000000000000*10^-11
[025]:   -5.960861e-06   -1.237512e-03 -1.10010000000001101100111*10^-18 -1.01000100011010000000000*10^-10
[026]:    3.684014e-06   -1.989007e-03 +1.11101110011101011100011*10^-19 -1.00000100101101000000000*10^ -9
[027]:   -2.276846e-06   -3.226519e-03 -1.00110001100101111101011*10^-19 -1.10100110111010000000000*10^ -9
[028]:    1.407168e-06   -5.215526e-03 +1.01111001101110111101111*10^-20 -1.01010101110011100000000*10^ -8
[029]:   -8.696778e-07   -8.442044e-03 -1.11010010111001111001110*10^-21 -1.00010100101000010000000*10^ -7
[030]:    5.374905e-07   -1.365757e-02 +1.00100000100100000010000*10^-21 -1.10111111100010000000000*10^ -7
[031]:   -3.321874e-07   -2.209961e-02 -1.01100100101011101111100*10^-22 -1.01101010000101001000000*10^ -6
[032]:    2.053031e-07   -3.575718e-02 +1.10111000111000101001000*10^-23 -1.00100100111011000100000*10^ -5
[033]:   -1.268843e-07   -5.785680e-02 -1.00010000011110110110000*10^-23 -1.11011001111101101000000*10^ -5
[034]:    7.841880e-08   -9.361398e-02 +1.01010000110011100110001*10^-24 -1.01111111011100010110000*10^ -4
[035]:   -4.846548e-08   -1.514708e-01 -1.10100000010100001011101*10^-25 -1.00110110001101100101000*10^ -3
[036]:    2.995332e-08   -2.450848e-01 +1.00000001010011000000101*10^-25 -1.11110101111011110000000*10^ -3
[037]:   -1.851217e-08   -3.965555e-01 -1.00111110000010010110000*10^-26 -1.10010110000100101010100*10^ -2
[038]:    1.144115e-08   -6.416403e-01 +1.10001001000111010110100*10^-27 -1.01001000100001010001010*10^ -1
[039]:   -7.071018e-09   -1.038196e+00 -1.11100101111010101010110*10^-28 -1.00001001110001110011010*10^  0
[040]:    4.370130e-09   -1.679836e+00 +1.00101100010100000010001*10^-28 -1.10101110000010011011111*10^  0
[041]:   -2.700889e-09   -2.718032e+00 -1.01110011001101010001001*10^-29 -1.01011011111010000111100*10^  1
[042]:    1.669241e-09   -4.397868e+00 +1.11001010110101100110010*10^-30 -1.00011001011101101010110*10^  2
[043]:   -1.031648e-09   -7.115900e+00 -1.00011011100100111100000*10^-30 -1.11000111011010101110100*10^  2
[044]:    6.375933e-10   -1.151377e+01 +1.01011110100001010100100*10^-31 -1.01110000011100001100101*10^  3
[045]:   -3.940543e-10   -1.862967e+01 -1.10110001010001000111000*10^-32 -1.00101010000100110010000*10^  4
[046]:    2.435390e-10   -3.014344e+01 +1.00001011110001100010000*10^-32 -1.11100010010010111000010*10^  4
[047]:   -1.505154e-10   -4.877311e+01 -1.01001010111111001001111*10^-33 -1.10000110001011110101001*10^  5
[048]:    9.302361e-11   -7.891654e+01 +1.10011001000111110100011*10^-34 -1.00111011101010101000101*10^  6
[049]:   -5.749175e-11   -1.276897e+02 -1.11111001101100111110101*10^-35 -1.11111110110000100011010*10^  6
```
計算結果の符号がループごとに入れ替わるのが正しい結果だが、`g`の方は20回目から負数のままになっており、誤差が蓄積していることがわかる。
### 考察
まず、このプログラムは、初項が1、二項目が`b`(bは`{1-sqrt(5)}/2`)のフィボナッチ数列を計算するものである。配列`g`は、フィボナッチ数列の定義どおり、`g[n]=g[n-1]+g[n-2]`と計算している。配列`f`は、`b`がフィボナッチ数列の特性方程式の一つであることを利用し、上の式を`f[n]=b*f[n-1]`と変形して計算している。 
この実験は丸め誤差を確かめるものであった。この丸め誤差は`b`の定義にあり、`b`の真の値は無理数であることから、これを`float`型に代入することで発生している。しかし、`g`の誤差はこれの蓄積によるものではない。 
`g`の誤差が広がっているのは、`g`は正の数と負の数の和を計算しており、これが桁落ちを発生させているためである。これを確かめるため、図12の`b`の定義を、特性方程式のもう一つの解である`{1+sqrt(5)}/2`にしてプログラムを実行してみた(図14)。  
*図14 marume.cpp の出力(b > 0の場合)*
```
[000]:    1.000000e+00    1.000000e+00 +1.00000000000000000000000*10^  0 +1.00000000000000000000000*10^  0
[001]:    1.618034e+00    1.618034e+00 +1.10011110001101110111101*10^  0 +1.10011110001101110111101*10^  0
[002]:    2.618034e+00    2.618034e+00 +1.01001111000110111011111*10^  1 +1.01001111000110111011110*10^  1
[003]:    4.236068e+00    4.236068e+00 +1.00001111000110111011111*10^  2 +1.00001111000110111011110*10^  2
[004]:    6.854103e+00    6.854102e+00 +1.10110110101010011001111*10^  2 +1.10110110101010011001101*10^  2
[005]:    1.109017e+01    1.109017e+01 +1.01100010111000101010111*10^  3 +1.01100010111000101010110*10^  3
[006]:    1.794427e+01    1.794427e+01 +1.00011111000110111011111*10^  4 +1.00011111000110111011110*10^  4
[007]:    2.903444e+01    2.903444e+01 +1.11010000100011010001010*10^  4 +1.11010000100011010001001*10^  4
[008]:    4.697871e+01    4.697871e+01 +1.01110111110101000110100*10^  5 +1.01110111110101000110100*10^  5
[009]:    7.601315e+01    7.601315e+01 +1.00110000000011010111100*10^  6 +1.00110000000011010111100*10^  6
[010]:    1.229919e+02    1.229919e+02 +1.11101011111101111010110*10^  6 +1.11101011111101111010110*10^  6
[011]:    1.990050e+02    1.990050e+02 +1.10001110000000101001001*10^  7 +1.10001110000000101001001*10^  7
[012]:    3.219969e+02    3.219969e+02 +1.01000001111111110011010*10^  8 +1.01000001111111110011010*10^  8
[013]:    5.210019e+02    5.210019e+02 +1.00000100100000000011111*10^  9 +1.00000100100000000011111*10^  9
[014]:    8.429988e+02    8.429988e+02 +1.10100101011111111101100*10^  9 +1.10100101011111111101100*10^  9
[015]:    1.364001e+03    1.364001e+03 +1.01010101000000000000110*10^ 10 +1.01010101000000000000110*10^ 10
[016]:    2.207000e+03    2.207000e+03 +1.00010011110111111111110*10^ 11 +1.00010011110111111111110*10^ 11
[017]:    3.571000e+03    3.571000e+03 +1.10111110011000000000001*10^ 11 +1.10111110011000000000001*10^ 11
[018]:    5.778000e+03    5.778000e+03 +1.01101001001000000000000*10^ 12 +1.01101001001000000000000*10^ 12
[019]:    9.349000e+03    9.349000e+03 +1.00100100001010000000000*10^ 13 +1.00100100001010000000000*10^ 13
[020]:    1.512700e+04    1.512700e+04 +1.11011000101110000000000*10^ 13 +1.11011000101110000000000*10^ 13
[021]:    2.447600e+04    2.447600e+04 +1.01111110011100000000000*10^ 14 +1.01111110011100000000000*10^ 14
[022]:    3.960300e+04    3.960300e+04 +1.00110101011001100000000*10^ 15 +1.00110101011001100000000*10^ 15
[023]:    6.407900e+04    6.407900e+04 +1.11110100100111100000000*10^ 15 +1.11110100100111100000000*10^ 15
[024]:    1.036820e+05    1.036820e+05 +1.10010101000000100000000*10^ 16 +1.10010101000000100000000*10^ 16
[025]:    1.677610e+05    1.677610e+05 +1.01000111101010001000000*10^ 17 +1.01000111101010001000000*10^ 17
[026]:    2.714430e+05    2.714430e+05 +1.00001001000101001100000*10^ 18 +1.00001001000101001100000*10^ 18
[027]:    4.392040e+05    4.392040e+05 +1.10101100111010010000000*10^ 18 +1.10101100111010010000000*10^ 18
[028]:    7.106470e+05    7.106470e+05 +1.01011010111111101110000*10^ 19 +1.01011010111111101110000*10^ 19
[029]:    1.149851e+06    1.149851e+06 +1.00011000101110011011000*10^ 20 +1.00011000101110011011000*10^ 20
[030]:    1.860498e+06    1.860498e+06 +1.11000110001110010010000*10^ 20 +1.11000110001110010010000*10^ 20
[031]:    3.010349e+06    3.010349e+06 +1.01101111011110010110100*10^ 21 +1.01101111011110010110100*10^ 21
[032]:    4.870847e+06    4.870847e+06 +1.00101001010010101111110*10^ 22 +1.00101001010010101111110*10^ 22
[033]:    7.881196e+06    7.881196e+06 +1.11100001000001111011000*10^ 22 +1.11100001000001111011000*10^ 22
[034]:    1.275204e+07    1.275204e+07 +1.10000101001010010101011*10^ 23 +1.10000101001010010101011*10^ 23
[035]:    2.063324e+07    2.063324e+07 +1.00111010110101101001100*10^ 24 +1.00111010110101101001100*10^ 24
[036]:    3.338528e+07    3.338528e+07 +1.11111101011010110100010*10^ 24 +1.11111101011010110100010*10^ 24
[037]:    5.401852e+07    5.401852e+07 +1.10011100001000001110111*10^ 25 +1.10011100001000001110111*10^ 25
[038]:    8.740381e+07    8.740381e+07 +1.01001101011010110100100*10^ 26 +1.01001101011010110100100*10^ 26
[039]:    1.414223e+08    1.414223e+08 +1.00001101101111011110000*10^ 27 +1.00001101101111011110000*10^ 27
[040]:    2.288261e+08    2.288261e+08 +1.10110100011100111000010*10^ 27 +1.10110100011100111000010*10^ 27
[041]:    3.702485e+08    3.702485e+08 +1.01100001000110001011001*10^ 28 +1.01100001000110001011001*10^ 28
[042]:    5.990746e+08    5.990746e+08 +1.00011101101010010011101*10^ 29 +1.00011101101010010011101*10^ 29
[043]:    9.693231e+08    9.693231e+08 +1.11001110001101011001001*10^ 29 +1.11001110001101011001010*10^ 29
[044]:    1.568398e+09    1.568398e+09 +1.01110101111011110110011*10^ 30 +1.01110101111011110110100*10^ 30
[045]:    2.537721e+09    2.537721e+09 +1.00101110100001010001100*10^ 31 +1.00101110100001010001100*10^ 31
[046]:    4.106119e+09    4.106119e+09 +1.11101001011111001100110*10^ 31 +1.11101001011111001100110*10^ 31
[047]:    6.643839e+09    6.643839e+09 +1.10001100000000001111001*10^ 32 +1.10001100000000001111001*10^ 32
[048]:    1.074996e+10    1.074996e+10 +1.01000000010111111010110*10^ 33 +1.01000000010111111010110*10^ 33
[049]:    1.739380e+10    1.739380e+10 +1.00000011001100000001001*10^ 34 +1.00000011001100000001001*10^ 34
```
このように、先ほどと同じく`b`で丸め誤差が発生しており、同じ計算式を用いたにもかかわらず、図13で見られた`g`の誤差は発生しなくなった。よって、`b`の丸め誤差は`g`の誤差と無関係であることがわかる。  
丸め誤差の蓄積を見るためには演算を繰り返す必要があるが、その演算でも誤差が生じる。純粋な丸め誤差を観測するには、さらなる手法の工夫が必要だと感じた。
## 参考文献
Webページはすべて2022年10月23日閲覧  

オープンエデュケーション by 東海林 智也
https://tmytokai.github.io/open-ed/activity/fpoint/

IEEE-754 Floating Point Converter  
https://www.h-schmidt.net/FloatConverter/IEEE754.html

江添亮のC++入門  
https://cpp.rainy.me/012-floating-point.html

フィボナッチ数列について
https://www.nli-research.co.jp/report/detail/id=66771
