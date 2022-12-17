#include <iostream>
#include <type_traits>
#include <unordered_map>

template <typename T>
static void CheckType(T&& t) {
    if constexpr (std::is_rvalue_reference_v<decltype(t)>) {
        std::cout << "  rvalue reference" << std::endl;
    }
    else if constexpr (std::is_lvalue_reference_v<decltype(t)>) {
        std::cout << "  lvalue reference" << std::endl;
    }
}

// for only r-value reference
static void foo(int&& a)
{
    std::cout << "      int&&" << std::endl;
}

// for only l-value reference
static void foo(int& a)
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

void LambdaTest() {

    auto foo_lambda = [](auto&& arg)
    {
        CheckType(std::forward<decltype(arg)>(arg));

        foo(arg); // call by lvalue
        foo(std::forward<decltype(arg)>(arg));  // call by what type it is
    };

    int x = 1;

    std::cout << "arg type is rvalue" << std::endl;
    foo_lambda(2);

    std::cout << "arg type is lvalue" << std::endl;
    foo_lambda(x);
}



template <typename T>
class FooClass {
private:
    template <typename KeyType, typename ValueType>
    void foo_member_internal(KeyType&& key, ValueType&& value) {
        CheckType(std::forward<KeyType>(key));
        CheckType(std::forward<ValueType>(value));
    }

public:
    explicit FooClass() = default;

    /* key is lvalue const reference (accept lvalue and also rvalue if there's no rvalue reference overloading)
     * value is forwarding reference
     */
    template <typename ValueType>
    void foo_member(const T& key, ValueType&& value) {
        foo_member_internal(key, value); // call by lvalue
        foo_member_internal(key, std::forward<ValueType>(value)); // call by what type it is
    }

    /* key type is rvalue reference (accept only rvalue)
     * value type is forwarding reference
     */
    template <typename ValueType>
    void foo_member(T&& key, ValueType&& value) {
        foo_member_internal(key, value); // call by lvalue
        foo_member_internal(std::move(key), std::forward<ValueType>(value)); // call by what type it is
    }

};


int main()
{
    int x = 1;

    FooClass<int> fooClass;

    std::cout << "key type is rvalue, value type is rvalue" << std::endl;
    fooClass.foo_member(2, 2);

    std::cout << "key type is rvalue, value type is lvalue" << std::endl;
    fooClass.foo_member(2, x);

    std::cout << "key type is lvalue, value type is rvalue" << std::endl;
    fooClass.foo_member(x, 2);

    std::cout << "key type is lvalue, value type is lvalue" << std::endl;
    fooClass.foo_member(x, x);

    LambdaTest();

    return 0;
}