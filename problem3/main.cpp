#include <iostream>

#include "funclib.cpp"

int main(void) {
    auto factory = funcs::TFactory();
    auto i1 = factory.Create("ident", 0);
    auto i2 = factory.Create("const", 2.5);
    auto i3 = i1 + i2;
    std::cout << (*i3)(10) << std::endl;
    std::cout << i3->ToString() << std::endl;

    return 0;
}
