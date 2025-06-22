#ifndef ARGS_PARSER_H
#define ARGS_PARSER_H

#include <string>
#include <string_view>
#include <type_traits>

template<typename T>
concept SignedInteger = std::is_integral_v<T> && std::is_signed_v<T>;

// Parses command-line arguments for an integer value following a given prefix.
// If no such argument is found, the provided fallback value is returned.
template<SignedInteger I>
I parse_int_args(int argc, char* argv[], std::string_view prefix, I fallback)
{
    for (int i = 1; i < argc; ++i) {
        std::string_view arg{argv[i]};
        if (arg.starts_with(prefix)) {
            return static_cast<I>(std::stoll(std::string(arg.substr(prefix.size()))));
        }
    }
    return fallback;
}

#endif // ARGS_PARSER_H
