
#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <limits>

#define newline std::endl
inline void write(std::ostream&) {}
template <typename T> inline void write(std::ostream& out, T t) { out << t; }
template <typename T, typename... Args>
inline void write(std::ostream& out, T t, Args... args) {
	out.precision(std::numeric_limits<double>::max_digits10);
	out << t << " ";
	write(out, args...);
}

inline void writeF(std::ostream& out, const char*& format) { out << format; }
template <typename T, typename... Args>
void writeF(std::ostream& out, const char* format, T t, Args... args) {
	while (*format != '%' && *format) {
		out.precision(std::numeric_limits<double>::max_digits10);
		out.put(*format++);
	}
	if (*format++ == '\0') {
		return;
	}
	out << t, writeF(out, format, args...);
}
template <typename T> inline void writeContainer(std::ostream& out, T t) {
	for (auto& elem : t) write(out, elem, "");
}
#define NORMAL_IO_SPEEDUP \
	std::ios_base::sync_with_stdio(false), std::cin.tie(NULL);

#define debug(x) #x, x

#define SRC std::string(__FILE__) + ":" + std::to_string(__LINE__)
// #undef S1
// #undef S2

#define writeC(out, x)      \
	writeLn(out, #x);       \
	writeContainer(out, x); \
	writeLn(out);

#define writeLn(out, ...)      \
	write(out, ##__VA_ARGS__); \
	out << newline;

#define print(...)	 write(std::cout, ##__VA_ARGS__)
#define printLn(...) writeLn(std::cout, ##__VA_ARGS__)

#endif	// UTIL_HPP
