#include <string>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <format>

#include <stdio.h>
#include <algorithm>
#include <random>
#include <vector>
#include <ranges>
#include <chrono>

// #include <cstdio>
// #include <assert.h>
// #include <cmath>

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

// class TPower : public TFunction {
// };

// class TExp : public TFunction {
// };

// class TPolynomial : public TFunction {
// };


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


int main(void) {
    auto factory = funcs::TFactory();
    // auto i1 = factory.Create("ident", nullptr);
    auto i1 = factory.Create("ident", 0);
    auto i2 = factory.Create("const", 2.5);
    auto i3 = i1 + i2;
    // auto i3 = funcs::TIdent(3.);
    std::cout << (*i3)(10) << std::endl;
    std::cout << i3->ToString() << std::endl;
    // std::cout << (*i1)(10) << std::endl;

    return 0;
}
