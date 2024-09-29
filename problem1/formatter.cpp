// Based on
// https://stackoverflow.com/questions/70905100/c-concepts-for-numeric-types-and-preventing-instantiation-on-pointers
// https://www.geeksforgeeks.org/how-to-iterate-over-the-elements-of-an-stdtuple-in-c/
#include <iostream>
#include <sstream>
#include <limits>
#include <type_traits>
#include <string>
#include <set>
#include <vector>
#include <utility>
#include <tuple>


template <typename T>
concept IntegerType = requires(T param) {
    requires std::is_integral_v<T>;
};

struct TPrettyPrinter {

    TPrettyPrinter() : str{} {}

    std::string Str() const {
        return str;
    }

    template <IntegerType T>
    TPrettyPrinter& Format(const T& value) {
        str += std::to_string(value);
        return *this;
    }

    TPrettyPrinter& Format(const std::string& value) {
        str += value;
        return *this;
    }

    template<typename T>
    TPrettyPrinter& Format(const std::vector<T>& value) {
        return format_container(value, "[", "]");
    }

    template<typename T>
    TPrettyPrinter& Format(const std::set<T>& value) {
        return format_container(value, "{", "}");
    }

    template<typename... Args>
    TPrettyPrinter& Format(const std::tuple<Args...>& value) {
        size_t length = sizeof...(Args);
        std::stringstream ss{};
        std::apply(
            [length, &ss](auto const&... ps) {
                ss << "(";
                size_t k = 0;
                ((ss << TPrettyPrinter().Format(ps).Str() << (++k == length ? "" : ", ")), ...);
                ss << ")";
            },
            value);
        str += ss.str();
        return *this;
    }

    template<typename T1, typename T2>
    TPrettyPrinter& Format(const std::pair<T1, T2>& value) {
        str += "<" + TPrettyPrinter().Format(value.first).Format(", ").Format(value.second).Str() + ">";
        return *this;
    }

private:
    std::string str;

    template<typename C>
    TPrettyPrinter& format_container(
            const C& value,
            std::string open_bracket,
            std::string close_bracket) {
        // str += open_bracket + TPrettyPrinter().Format(*(value.begin())).Str();
        str += open_bracket;
        for (auto it = value.begin(); it != value.end(); it++) {
            str += TPrettyPrinter().Format(*it).Str() + ", ";
        }
        str = str.substr(0, str.size() - 2);
        str += close_bracket;
        return *this;
    }
};

template<typename T>
std::string Format(const T& t) {
    return TPrettyPrinter().Format(t).Str();
}
