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


// TEST_F(BaseTest, Power) {
//     std::vector<std::vector<double>> test_matrix = {
//         // num, num^-1, num^0.5, num^4
//         {1.000000, 1.000000, 1.000000, 1.000000},
//         {105.000000, 0.009524, 10.246951, 121550625.000000},
//         {34.530000, 0.028960, 5.876223, 1421629.128577},
//         {123.456000, 0.008100, 11.111076, 232299784.284559},
//     };
//     std::vector<funcs::TFunctionPtr> test_functions = {
//         factory.Create("power", 1),
//         factory.Create("power", -1),
//         factory.Create("power", 0.5),
//         factory.Create("power", 4),
//     };

//     // for (unsigned i = 0; i < test_matrix.size(); i++) {
//     //     for (unsigned j = 0; j < test_matrix.size(); j++) {
//     //         EXPECT_EQ(test_functions[j], test_matrix[i][j]);
//     //     }
//     // }
// }





int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
