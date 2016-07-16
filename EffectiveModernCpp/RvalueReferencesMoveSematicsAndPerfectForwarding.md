#####条款23: 理解`std::move`和`std::forward`
- `std::move`和`std::forward`都是模板函数，

- 有关`std::move`
 + `std::move`支持隐式实例化
 + `std::move`的实现，在C++14中，`std::move`是constexpr
 + `std::move`执行无条件的右值转换
```
// TEMPLATE FUNCTION move
template<class _Ty> inline
constexpr
typename remove_reference<_Ty>::type&&  // 返回值类型永远是 typename remove_reference<_Ty>::type&&
move(_Ty&& _Arg) _NOEXCEPT
{	// forward _Arg as movable
   return (static_cast<typename remove_reference<_Ty>::type&&>(_Arg));
}
```

- 有关`std::forward`
 + `std::forward`有两种重载形式，因此必须显式实例化
 + `std::forward`的实现，在C++14中，`std::forward`是constexpr
```
// TEMPLATE FUNCTION forward
template<class _Ty> inline
constexpr
_Ty&& forward(
typename remove_reference<_Ty>::type& _Arg) _NOEXCEPT      // 参数是左值类型时，重载决议选择这个
{	  // forward an lvalue as either an lvalue or an rvalue  // 返回值类型是左值引用或右值引用
    return (static_cast<_Ty&&>(_Arg));
}

template<class _Ty> inline
constexpr
_Ty&& forward(
typename remove_reference<_Ty>::type&& _Arg) _NOEXCEPT  // 参数是右值类型时，重载决议选择这个
{	  // forward an rvalue as an rvalue                   // 返回值类型一定是右值引用
    static_assert(!is_lvalue_reference<_Ty>::value, "bad forward call");
    return (static_cast<_Ty&&>(_Arg));
}
```
- `std::forward`必须显式实例化，它的返回值是static_cast<T&&>(arg)，因此T绑定到什么类型决定了`std::forward`的返回值类型
 + 按照`perfect forwarding`的规则来决定返回值类型
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

- 有了`perfect forwarding`，C++11的标准库容器（例如`std::vector`）增加了新的API：`emplace()`和`emplace_back()`
本质上，这些API利用了`perfect forwarding`，能够直接在容器中构造对象，而无需有临时对象和拷贝对象的开销。

- 一些极好的链接  
http://stackoverflow.com/questions/3582001/advantages-of-using-forward
http://stackoverflow.com/questions/3106110/what-are-move-semantics/11540204#11540204
