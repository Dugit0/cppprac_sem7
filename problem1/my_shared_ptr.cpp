// Based on https://codereview.stackexchange.com/questions/254279/c-shared-ptr-implementation
#include <iostream>
/*
Реализовать свою версию умного указателя, функциональность которого должна
напоминать shared_ptr. Класс реализации должен быть шаблонным, должны быть
реализованы конструкторы, деструкторы, присваивание, разыменование (в 2 видах),
reset, swap, get, операции сравнения.
*/
template<class T>
class Shared_pointer {
    T* stored_ptr;
    unsigned long long* counter;
public:
    Shared_pointer(T* ptr) {
        stored_ptr = ptr;
        counter = new unsigned long long{0};
        if (stored_ptr) {
            (*counter)++;
        }
    }

    Shared_pointer(const Shared_pointer& ptr) {
        stored_ptr = ptr.stored_ptr;
        counter = ptr.counter;
        (*counter)++;
    }
    
    Shared_pointer operator=(const Shared_pointer& ptr) {
        reset();
        stored_ptr = ptr.stored_ptr;
        counter = ptr.counter;
        (*counter)++;
        return *this;
    }

    T& operator*() const {
        return *stored_ptr;
    }

    T* operator->() const {
        return stored_ptr;
    }

    auto operator==(const Shared_pointer& other) const {
        return stored_ptr == other.stored_ptr;
    }

    auto operator<=>(const Shared_pointer& other) const {
        return stored_ptr <=> other.stored_ptr;
    }

    friend std::ostream& operator<<(std::ostream& os, const Shared_pointer& ptr) {
        os << ptr.stored_ptr;
        return os;
    }
    
    T* get() const {
        return stored_ptr;
    }

    void reset() {
        (*counter)--;
        if (*counter == 0) {
            delete counter;
            delete stored_ptr;
        }
        stored_ptr = nullptr;
        counter = nullptr;
    }

    void swap(Shared_pointer& ptr) {
        std::swap(counter, ptr.counter);
        std::swap(stored_ptr, ptr.stored_ptr);
    }
    
    ~Shared_pointer() {
        if (stored_ptr) {
            reset();
        }
    }
};


template<typename T, typename... Args>
Shared_pointer<T> make_shared_pointer(Args&&... args) {
    return Shared_pointer<T>(new T{std::forward<Args>(args)...});
}


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
