#include <iostream>
#include "shared_pointer.cpp"


struct A {
    int value;
    A(int p_value) : value{p_value} {}
    ~A() {
        std::cout << "~A" << std::endl;
    }
    void hello_world() {
        std::cout << "Hello world!" << std::endl;
    }
};

int main(void) {
    {
        std::cout << "---------- TEST 1 ----------" << std::endl;
        Shared_pointer<int> ptr1 = new int{1};
        std::cout << ptr1.get() << " : " << *ptr1.get() << std::endl;
    }
    {
        std::cout << "---------- TEST 2 ----------" << std::endl;
        Shared_pointer<int> ptr1 = new int{1};
        Shared_pointer<int> ptr2(ptr1);
        std::cout << ptr1.get() << " : " << *ptr1.get() << std::endl;
        std::cout << ptr2.get() << " : " << *ptr2.get() << std::endl;
    }
    {
        std::cout << "---------- TEST 3 ----------" << std::endl;
        Shared_pointer<int> ptr1 = new int{1};
        Shared_pointer<int> another_ptr1 = new int{2};
        Shared_pointer<int> ptr2 = ptr1;
        std::cout << ptr2.get() << " : " << *ptr2.get() << std::endl;
        ptr2 = another_ptr1;
        std::cout << ptr1.get() << " : " << *ptr1.get() << std::endl;
        std::cout << ptr2.get() << " : " << *ptr2.get() << std::endl;
    }
    {
        std::cout << "---------- TEST 4 ----------" << std::endl;
        // Move semantic?
        Shared_pointer<int> ptr1 = Shared_pointer<int>{new int{4}};
        std::cout << ptr1.get() << " : " << *ptr1.get() << std::endl;
    }
    {
        std::cout << "---------- TEST 5 ----------" << std::endl;
        Shared_pointer<A> ptr1 = make_shared_pointer<A>(5);
        std::cout << ptr1 << " : " << ptr1->value << " == " << (*ptr1).value << std::endl;
        ptr1->hello_world();
        (*ptr1).hello_world();
    }
    {
        std::cout << "---------- TEST 6 ----------" << std::endl;
        // Swap
        auto ptr1 = make_shared_pointer<A>(1);
        auto ptr2 = make_shared_pointer<A>(2);
        auto ptr3 = ptr2;
        std::cout << ptr1 << " : " << ptr1->value << std::endl;
        std::cout << ptr2 << " : " << ptr2->value << std::endl;
        std::cout << ptr3 << " : " << ptr3->value << std::endl;
        ptr1.swap(ptr2);
        std::cout << ptr1 << " : " << ptr1->value << std::endl;
        std::cout << ptr2 << " : " << ptr2->value << std::endl;
        std::cout << ptr3 << " : " << ptr3->value << std::endl;
    }
    {
        std::cout << "---------- TEST 7 ----------" << std::endl;
        auto ptr1 = make_shared_pointer<int>(5);
        auto ptr2 = make_shared_pointer<int>(5);
        std::cout << ptr1 << " " << ptr2 << std::endl;
        std::cout << *ptr1 << " " << *ptr2 << std::endl;
        std::cout << (ptr1 == ptr2) << std::endl;
        std::cout << (ptr1 != ptr2) << std::endl;
        std::cout << (ptr1 < ptr2) << std::endl;
    }
    // {
    //     std::cout << "---------- TEST N ----------" << std::endl;
    //     std::cout << ptr1.get() << " : " << *ptr1.get() << std::endl;
    //     std::cout << "----------------------------" << std::endl;
    // }
    return 0;
}
