#include <iostream>
#include <type_traits>

// for only r-value reference
static void foo(int &&a)
{
    std::cout << "      int&&" << std::endl;
}

// for only l-value reference
static void foo(int &a)
{
    std::cout << "      int&" << std::endl;
}

// for both l-value and r-value reference
static void foo(const int& a)
{
    std::cout << "      const int&" << std::endl;
}

// for both l-value and r-value reference (extra one copy)
//static void test(int a)
//{
//    std::cout << "int" << std::endl;
//}

template <typename T>
class FooClass {
public:
    explicit FooClass() = default;

    /* t is rvalue reference
     * y is forwarding reference
     */
    template <typename Y>
    void foo_member(T&& t, Y&& y) {
        if constexpr (std::is_rvalue_reference_v<decltype(y)>) {
            std::cout << "  forwarding reference deduced to rvalue reference" << std::endl;
        }
        else if constexpr (std::is_lvalue_reference_v<decltype(y)>) {
            std::cout << "  forwarding reference deduced to lvalue reference" << std::endl;
        }
        else {
            std::cout << "  forwarding reference deduced to ???" << std::endl;
        }

        std::cout << "  Test on rvalue reference" << std::endl;
        std::cout << "  -> call by lvalue" << std::endl;
        foo(t); // call lvalue overload
        std::cout << "  -> call by std::move" << std::endl;
        foo(std::move(t)); // call rvalue overload

        std::cout << "  Test on forwarding reference" << std::endl;
        std::cout << "  -> call by lvalue" << std::endl;
        foo(y); // call lvalue overload
        std::cout << "  -> call by std::forward" << std::endl;
        foo(std::forward<Y>(y));  // can call lvalue or rvalue overload, decided by deduced type of Y
    }
};

int main()
{
    auto foo_lambda = [](auto&& arg)
    {
        if constexpr (std::is_rvalue_reference_v<decltype(arg)>) {
            std::cout << "  forwarding reference deduced to rvalue reference" << std::endl;
        }
        else if constexpr (std::is_lvalue_reference_v<decltype(arg)>) {
            std::cout << "  forwarding reference deduced to lvalue reference" << std::endl;
        }
        else {
            std::cout << "  forwarding reference deduced to ???" << std::endl;
        }

        std::cout << "  Test on forwarding reference" << std::endl;
        std::cout << "  -> call by lvalue" << std::endl;
        foo(arg); // call lvalue overload
        std::cout << "  -> call by std::forward" << std::endl;
        foo(std::forward<decltype(arg)>(arg));  // can call lvalue or rvalue overload, decided by deduced type of Y
    };

    int x = 1;

    FooClass<int> fooClass;
    std::cout << "[Class] Test pass rvalue to forwarding reference" << std::endl;
    fooClass.foo_member(2, 2);
    std::cout << "[Class] Test pass lvalue to forwarding reference" << std::endl;
    fooClass.foo_member(2, x);

    std::cout << "[Lambda] Test pass rvalue to forwarding reference" << std::endl;
    foo_lambda(2);
    std::cout << "[Lambda] Test pass lvalue to forwarding reference" << std::endl;
    foo_lambda(x);

    return 0;
}