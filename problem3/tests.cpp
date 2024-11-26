#include <gtest/gtest.h>
#include <vector>

#include "funclib.cpp"

class BaseTest : public testing::Test {
protected:
    funcs::TFactory factory;
    std::vector<double> test_numbers = {0, 1, -1, 105, 34.5, 123.5, -12, -100, -345.6};
};


TEST_F(BaseTest, Ident) {
    auto func = factory.Create("ident");
    ASSERT_NE(func, nullptr);
    EXPECT_EQ(func->ToString(), "x");
    for (const auto& x : test_numbers) {
        EXPECT_EQ((*func)(x), x);
        EXPECT_EQ(func->calc_derivative(x), 1);
    }
}


TEST_F(BaseTest, Const) {
    for (const auto& number : test_numbers) {
        auto func = factory.Create("const", number);
        ASSERT_NE(func, nullptr);
        EXPECT_EQ(func->ToString(), std::to_string(number));
        for (const auto& x : test_numbers) {
            EXPECT_EQ((*func)(x), number);
            EXPECT_EQ(func->calc_derivative(x), 0);
        }
    }
}


TEST_F(BaseTest, Power) {
    std::vector<std::vector<double>> test_matrix = {
        // num,    num^-1,                num^0.5,            num^4
        {1.000000, 1.000000,              1.000000,           1.000000},
        {105.0000, 0.0095238095238095247, 10.246950765959598, 121550625.000000},
        {34.53000, 0.028960324355632783,  5.8762232769015847, 1421629.1285768102},
        {123.4560, 0.0081000518403317786, 11.111075555498667, 232299784.28455886},
    };
    std::vector<std::vector<double>> test_deriv_matrix = {
        // 1, -1*(num^-2),               0.5*(num^-0.5),       4*num^3
        {1,   -1.000000,                0.500000,              4.000000},
        {1,   -0.000090702947845804991, 0.048795003647426657,  4630500.000000},
        {1,   -0.00083870038678345732,  0.08508866604259463,   164683.362708},
        {1,   -0.000065610839816062223, 0.045000144000691203,  7526561.1808112646},
    };
    std::vector<funcs::TFunctionPtr> test_functions = {
        factory.Create("power", 1),
        factory.Create("power", -1),
        factory.Create("power", 0.5),
        factory.Create("power", 4),
    };

    for (unsigned j = 0; j < test_matrix.size(); j++) {
        ASSERT_NE(test_functions[j], nullptr);
    }

    EXPECT_EQ(test_functions[0]->ToString(), "x^1");
    EXPECT_EQ(test_functions[1]->ToString(), "x^-1");
    EXPECT_EQ(test_functions[2]->ToString(), "x^0.5");
    EXPECT_EQ(test_functions[3]->ToString(), "x^4");

    for (unsigned i = 0; i < test_matrix.size(); i++) {
        for (unsigned j = 0; j < test_functions.size(); j++) {
            EXPECT_DOUBLE_EQ((*test_functions[j])(test_matrix[i][0]), test_matrix[i][j]);
        }
    }

    std::vector<double> test_nums = {1., 105, 34.53, 123.456};
    for (unsigned i = 0; i < test_nums.size(); i++) {
        double x = test_nums[i];
        for (unsigned j = 0; j < test_functions.size(); j++) {
            EXPECT_DOUBLE_EQ(test_functions[j]->calc_derivative(x), test_deriv_matrix[i][j]);
        }
    }


}


TEST_F(BaseTest, Exp) {
    auto func = factory.Create("exp");
    ASSERT_NE(func, nullptr);
    EXPECT_EQ(func->ToString(), "e^x");
    for (const auto& x : test_numbers) {
        EXPECT_DOUBLE_EQ((*func)(x), std::exp(x));
        EXPECT_DOUBLE_EQ(func->calc_derivative(x), std::exp(x));
    }
}

TEST_F(BaseTest, Polynomial) {
    std::vector<funcs::TFunctionPtr> polys = {
        factory.Create("polynomial", {0, 0, 0}),
        factory.Create("polynomial", {5, 0, 0}),
        factory.Create("polynomial", {5, 0, 4, 2}),
    };
    for (const auto& poly : polys) {
        ASSERT_NE(poly, nullptr);
    }

    EXPECT_EQ(polys[0]->ToString(), "0");
    EXPECT_EQ(polys[1]->ToString(), "5*x^0");
    EXPECT_EQ(polys[2]->ToString(), "5*x^0 + 4*x^2 + 2*x^3");

    for (const auto& x : test_numbers) {
        EXPECT_EQ((*polys[0])(x), 0);
        EXPECT_EQ((*polys[1])(x), 5);
    }

    EXPECT_DOUBLE_EQ((*polys[2])(23), 26455);
    EXPECT_DOUBLE_EQ((*polys[2])(42), 155237);
    EXPECT_DOUBLE_EQ((*polys[2])(6.22), 641.037296);

    EXPECT_DOUBLE_EQ(polys[2]->calc_derivative(23), 3358);
    EXPECT_DOUBLE_EQ(polys[2]->calc_derivative(42), 10920);
    EXPECT_DOUBLE_EQ(polys[2]->calc_derivative(6.22), 281.89039999999994);
}

TEST_F(BaseTest, Arithmetic) {
    auto f1 = factory.Create("const", 10);
    auto f2 = factory.Create("polynomial", {1, 1, 2, 1});
    auto f3 = factory.Create("exp");
    auto f4 = factory.Create("ident");
    auto f5 = factory.Create("power", 3);
    auto func = f3 / (f4 + f5) * (f2 - f1);
    ASSERT_NE(func, nullptr);
    EXPECT_EQ(func->ToString(), "((((e^x) / (((x) + (x^3))))) * (((1*x^0 + 1*x^1 + 2*x^2 + 1*x^3) - (10.000000))))");
    EXPECT_DOUBLE_EQ((*func)(1), -6.7957045711476125);
    EXPECT_DOUBLE_EQ((*func)(10.5), 42890.765437737842);
    EXPECT_DOUBLE_EQ((*func)(3.456), 42.236606276507715);
    // f'(x) = (e**x * (x**6 + 2*x**5 - 7*x**3 + 30*x**2 - 9*x + 9))/((x + x**3)**2)
    EXPECT_DOUBLE_EQ(func->calc_derivative(1), 17.668831884983792);
    EXPECT_DOUBLE_EQ(func->calc_derivative(10.5), 42329.10267949718);
    EXPECT_DOUBLE_EQ(func->calc_derivative(3.456), 43.34604940830808);
}

TEST_F(BaseTest, LogicError) {
    EXPECT_THROW(factory.Create("") + factory.Create("ident"), std::logic_error);
    EXPECT_THROW(factory.Create("const", 4) * factory.Create("abc"), std::logic_error);
    EXPECT_THROW(factory.Create("polynomial", {2, 3, 4}) / factory.Create("ident", 4), std::logic_error);
}



int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
