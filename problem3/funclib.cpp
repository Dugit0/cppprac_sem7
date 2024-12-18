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
    explicit TFunction(TFunctionPtr t_left, char t_operation, TFunctionPtr t_right)
        : Left(t_left)
        , Right(t_right)
        , Operation(t_operation)
    {
        if (Operation != '=') {
            if (!Left) {
                throw std::logic_error("Invalid left operand");
            } else if (!Right) {
                throw std::logic_error("Invalid right operand");
            }
        }
    }
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
            return std::format("(({}) {} ({}))", Left->ToString(), Operation, Right->ToString());
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

    virtual double calc_derivative(double x) {
        if (Operation == '+') {
            return Left->calc_derivative(x) + Right->calc_derivative(x);
        } else if (Operation == '-') {
            return Left->calc_derivative(x) - Right->calc_derivative(x);
        } else if (Operation == '*') {
            return Left->calc_derivative(x) * (*Right)(x) + (*Left)(x) * Right->calc_derivative(x);
        } else if (Operation == '/') {
            return (Left->calc_derivative(x) * (*Right)(x) - (*Left)(x) * Right->calc_derivative(x)) / ((*Right)(x) * (*Right)(x));
        } else {
            // TODO
            throw std::logic_error("Unexpected operation");
        }
    }
};


class TFactory {
public:
    TFunctionPtr Create(std::string type);
    TFunctionPtr Create(std::string type, double value);
    TFunctionPtr Create(std::string type, std::vector<double> value);
};


class TIdent : public TFunction {
    friend TFunctionPtr TFactory::Create(std::string type);

protected:
    explicit TIdent() : TFunction(nullptr, '=', nullptr) {}

public:
    std::string ToString() override {
        return "x";
    }
    double operator()(double x) override {
        return x;
    }
    double calc_derivative(double) override {
        return 1.;
    }
};


class TConst : public TFunction {
    double Value;
    friend TFunctionPtr TFactory::Create(std::string type, double value);

protected:
    explicit TConst(double t_value) : TFunction(nullptr, '=', nullptr), Value(t_value) {}

public:
    std::string ToString() override {
        return std::to_string(Value);
    }
    double operator()(double) override {
        return Value;
    }
    double calc_derivative(double) override {
        return 0.;
    }
};


class TPower : public TFunction {
    double Power;
    friend TFunctionPtr TFactory::Create(std::string type, double value);
protected:
    explicit TPower(double t_power) : TFunction(nullptr, '=', nullptr), Power(t_power) {}

public:
    std::string ToString() override {
        return std::format("x^{}", Power);
    }
    double operator()(double x) override {
        return std::pow(x, Power);
    }
    double calc_derivative(double x) override {
        if (Power == 0) {
            return 0.;
        }
        return Power * std::pow(x, Power - 1);
    }
};


class TExp : public TFunction {
    friend TFunctionPtr TFactory::Create(std::string type);

protected:
    explicit TExp() : TFunction(nullptr, '=', nullptr) {}

public:
    std::string ToString() override {
        return "e^x";
    }
    double operator()(double x) override {
        return std::exp(x);
    }
    double calc_derivative(double x) override {
        return std::exp(x);
    }
};


class TPolynomial : public TFunction {
    std::vector<double> Coeffs;
    friend TFunctionPtr TFactory::Create(std::string type, std::vector<double> value);

protected:
    explicit TPolynomial(std::vector<double> t_coeffs)
        : TFunction(nullptr, '=', nullptr)
        , Coeffs(t_coeffs)
    {}

public:
    std::string ToString() override {
        bool first_not_null = true;
        std::string res{};
        for (size_t i = 0; i < Coeffs.size(); i++) {
            if (Coeffs[i] == 0) {
                continue;
            }
            if (first_not_null) {
                first_not_null = false;
                res += std::format("{}*x^{}", Coeffs[i], i);
            } else {
                res += std::format(" + {}*x^{}", Coeffs[i], i);
            }
        }
        if (res == "") {
            res = "0";
        }
        return res;
    }
    double operator()(double x) override {
        double res = 0.;
        for (size_t i = 0; i < Coeffs.size(); i++) {
            res += Coeffs[i] * std::pow(x, i);
        }
        return res;
    }
    double calc_derivative(double x) override {
        double res = 0.;
        for (size_t i = 1; i < Coeffs.size(); i++) {
            res += Coeffs[i] * i * std::pow(x, i - 1);
        }
        return res;
    }
};



TFunctionPtr TFactory::Create(std::string type) {
    if (type == "ident") {
        return std::shared_ptr<TIdent>(new TIdent());
    } else if (type == "exp") {
        return std::shared_ptr<TExp>(new TExp());
    } else {
        return nullptr;
    }
}

TFunctionPtr TFactory::Create(std::string type, double value) {
    if (type == "const") {
        return std::shared_ptr<TConst>(new TConst(value));
    } else if (type == "power") {
        return std::shared_ptr<TPower>(new TPower(value));
    } else {
        return nullptr;
    }
}

TFunctionPtr TFactory::Create(std::string type, std::vector<double> value) {
    if (type == "polynomial") {
        return std::shared_ptr<TPolynomial>(new TPolynomial(value));
    } else {
        return nullptr;
    }
}

} // end namespace funcs
