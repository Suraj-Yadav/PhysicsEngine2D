
#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>

// clang-format off
using std::cout;
using std::cin;
#define newline std::endl
inline void print() {}
template <typename T> void print(T t) { cout<<t; }
template <typename T, typename... Args> void print(T t, Args... args) { cout << t << " "; print(args...); }
template <typename... Args> void printLn(Args... args) { print(args...); cout<<newline; }
inline void printF(const char *&format) { cout << format; }
template <typename T, typename... Args> void printF(const char *format, T t, Args... args) {
while (*format != '%' && *format) {cout.put(*format++);} if (*format++ == '\0') {return;} cout << t, printF( format, args...);}
template <typename T> inline void printC(T t) { for (auto &elem : t) print(elem, ""); printLn(); }
#define NORMAL_IO_SPEEDUP std::ios_base::sync_with_stdio(false),std::cin.tie(NULL);
#define debug(x) #x, x
// clang-format on

#endif  // UTIL_HPP
