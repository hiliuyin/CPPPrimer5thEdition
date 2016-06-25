#####条款23: 理解`std::move`和`std::forward`
- `std::move`和`std::forward`都是模板函数，
- `std::move`支持隐式实例化
- `std::forward`必须显式实例化

- `std::move`的实现，在C++14中，`std::move`是constexpr
```
// TEMPLATE FUNCTION move
template<class _Ty> inline
constexpr
typename remove_reference<_Ty>::type&&
move(_Ty&& _Arg) _NOEXCEPT
{	// forward _Arg as movable
   return (static_cast<typename remove_reference<_Ty>::type&&>(_Arg));
}
```
- `std::forward`的实现，`std::forward`有两种重载形式，在C++14中，`std::forward`是constexpr
```
// TEMPLATE FUNCTION forward
template<class _Ty> inline
constexpr
_Ty&& forward(
typename remove_reference<_Ty>::type& _Arg) _NOEXCEPT
{	// forward an lvalue as either an lvalue or an rvalue
    return (static_cast<_Ty&&>(_Arg));
}

template<class _Ty> inline
constexpr
_Ty&& forward(
typename remove_reference<_Ty>::type&& _Arg) _NOEXCEPT
{	// forward an rvalue as an rvalue
    static_assert(!is_lvalue_reference<_Ty>::value, "bad forward call");
    return (static_cast<_Ty&&>(_Arg));
}
```
- `std::move`执行无条件的右值转换
- `std::forward`必须显式实例化，它的返回值是static_cast<T&&>(arg)，因此T绑定到什么类型决定了`std::forward`的返回值类型
 + 如果T绑定到左值引用类型，那么返回值类型是左值引用
 + 如果T绑定到非引用类型或者右值引用类型，那么返回值类型是右值引用
```
class A
{
public:
    A() = default;
    A(const A&) { std::cout << "lvalue overload" << std::endl; }
    A(A&&) { std::cout << "rvalue overload" << std::endl; }
};

class Widget
{
public:
    Widget() = default;
    ~Widget() = default;
    Widget(const Widget& rhs) : data_(rhs.data_) {}
    Widget(Widget&& rhs) : data_(std::forward<A>(rhs.data_)) {}
    
private:
    A data_;
};

Widget w1;
Widget w2(std::forward<Widget>(w1));  // r-value overload
Widget w3(std::forward<Widget&>(w2)); // l-value overload
```
