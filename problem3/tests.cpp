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
    }
}


TEST_F(BaseTest, Const) {
    for (const auto& number : test_numbers) {
        auto func = factory.Create("const", number);
        ASSERT_NE(func, nullptr);
        EXPECT_EQ(func->ToString(), std::to_string(number));
        for (const auto& x : test_numbers) {
            EXPECT_EQ((*func)(x), number);
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
        for (unsigned j = 0; j < test_matrix.size(); j++) {
            EXPECT_DOUBLE_EQ((*test_functions[j])(test_matrix[i][0]), test_matrix[i][j]);
        }
    }
}


TEST_F(BaseTest, Exp) {
    auto func = factory.Create("exp");
    ASSERT_NE(func, nullptr);
    EXPECT_EQ(func->ToString(), "e^x");
    for (const auto& x : test_numbers) {
        EXPECT_EQ((*func)(x), std::exp(x));
    }
}

TEST_F(BaseTest, Polynomial) {
    std::vector<funcs::TFunctionPtr> polys = {
        factory.Create("polynomial", {0, 0, 0}),
        factory.Create("polynomial", {5, 0, 0}),
        factory.Create("polynomial", {5, 2, 4, 2}),
    };
    for (const auto& poly : polys) {
        ASSERT_NE(poly, nullptr);
    }
}



int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
