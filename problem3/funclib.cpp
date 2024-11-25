#include <string>
#include <memory>
#include <stdexcept>
#include <format>
#include <cmath>
#include <vector>

// #include <algorithm>
// #include <random>
// #include <ranges>
// #include <chrono>

namespace funcs {

class TFunction;
using TFunctionPtr = std::shared_ptr<TFunction>;


class TFunction : public std::enable_shared_from_this<TFunction> {
protected:
    TFunctionPtr Left;
    TFunctionPtr Right;
    char Operation;
    TFunction(TFunctionPtr t_left, char t_operation, TFunctionPtr t_right)
        : Left(t_left)
        , Right(t_right)
        , Operation(t_operation)
    {}
public:

    friend TFunctionPtr operator+(TFunctionPtr lhs, TFunctionPtr rhs) {
        return TFunctionPtr(new TFunction(lhs, '+', rhs));
    }

    friend TFunctionPtr operator-(TFunctionPtr lhs, TFunctionPtr rhs) {
        return TFunctionPtr(new TFunction(lhs, '-', rhs));
    }

    friend TFunctionPtr operator*(TFunctionPtr lhs, TFunctionPtr rhs) {
        return TFunctionPtr(new TFunction(lhs, '*', rhs));
    }

    friend TFunctionPtr operator/(TFunctionPtr lhs, TFunctionPtr rhs) {
        return TFunctionPtr(new TFunction(lhs, '/', rhs));
    }

    virtual std::string ToString() {
        if (Operation == '=') {
            return Left->ToString();
        } else if (Operation == '+' || Operation == '-' ||
                Operation == '*' || Operation == '/') {
            return std::format("({} {} {})", Left->ToString(), Operation, Right->ToString());
        } else {
            // TODO
            throw std::logic_error("Unexpected operation");
        }
    }
    virtual double operator()(double x) {
        if (Operation == '+') {
            return (*Left)(x) + (*Right)(x);
        } else if (Operation == '-') {
            return (*Left)(x) - (*Right)(x);
        } else if (Operation == '*') {
            return (*Left)(x) * (*Right)(x);
        } else if (Operation == '/') {
            return (*Left)(x) / (*Right)(x);
        } else {
            // TODO
            throw std::logic_error("Unexpected operation");
        }
    }
};


class TFactory {
public:
    template<typename T>
    TFunctionPtr Create(std::string type, T value);
};


class TIdent : public TFunction {
    template<typename T>
    friend TFunctionPtr TFactory::Create(std::string type, T value);

protected:
    explicit TIdent() : TFunction(nullptr, '=', nullptr) {}

public:
    std::string ToString() override {
        return "x";
    }
    double operator()(double x) override {
        return x;
    }
};


class TConst : public TFunction {
    double Value;
    template<typename T>
    friend TFunctionPtr TFactory::Create(std::string type, T value);

protected:
    explicit TConst(double t_value) : TFunction(nullptr, '=', nullptr), Value(t_value) {}

public:
    std::string ToString() override {
        return std::to_string(Value);
    }
    double operator()(double) override {
        return Value;
    }
};


class TPower : public TFunction {
    double Power;
    template<typename T>
    friend TFunctionPtr TFactory::Create(std::string type, T value);
protected:
    explicit TPower(double t_power) : TFunction(nullptr, '=', nullptr), Power(t_power) {}

public:
    std::string ToString() override {
        return std::format("x^{}", Power);
    }
    double operator()(double x) override {
        return std::pow(x, Power);
    }

};


class TExp : public TFunction {
    template<typename T>
    friend TFunctionPtr TFactory::Create(std::string type, T value);

protected:
    explicit TExp() : TFunction(nullptr, '=', nullptr) {}

public:
    std::string ToString() override {
        return "e^x";
    }
    double operator()(double x) override {
        return std::exp(x);
    }
};


class TPolynomial : public TFunction {
    std::vector<double> Coeffs;
    template<typename T>
    friend TFunctionPtr TFactory::Create(std::string type, T value);

protected:
    explicit TPolynomial(std::vector<double> t_coeffs)
        : TFunction(nullptr, '=', nullptr)
        , Coeffs(t_coeffs)
    {}

public:
    std::string ToString() override {
        std::string res{};
        for (size_t i = 0; i < Coeffs.size(); i++) {
            if (Coeffs[i] == 0) {
                continue;
            }
            res += std::format("{}*x^{}", Coeffs[i], i);
        }
        if (res == "") {
            res = "0";
        }
        return res;
    }
    double operator()(double x) override {
        double res{};
        for (size_t i = 0; i < Coeffs.size(); i++) {
            res += Coeffs[i] * std::pow(x, i);
        }
        return res;
    }
};


template<typename T>
TFunctionPtr TFactory::Create(std::string type, T value) {
    if (type == "ident") {
        return std::shared_ptr<TIdent>(new TIdent());
    } else if (type == "const") {
        return std::shared_ptr<TConst>(new TConst(value));
    } else {
        return nullptr;
    }
}
} // end namespace funcs
