##### 条款23: 理解`std::move`和`std::forward`
- `std::move`和`std::forward`都是模板函数，

- 有关`std::move`
  + `std::move`支持隐式实例化
  + `std::move`的实现，在C++14中，`std::move`是constexpr
  + `std::move`的结果是`xvalue expression`
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
毕竟即使再聪明的编译器，使用`insert()`和`push_back()`的拷贝/移动的开销总归无法避免。

- What are rvalues, lvalues, xvalues, glvalues, and prvalues?
  + An lvalue (so-called, historically, because lvalues could appear on the left-hand side of an assignment expression) designates a function or an object. [Example: If E is an expression of pointer type, then *E is an lvalue expression referring to the object or function to which E points. As another example, the result of calling a function whose return type is an lvalue reference is an lvalue.]
  + An xvalue (an “eXpiring” value) also refers to an object, usually near the end of its lifetime (so that its resources may be moved, for example). An xvalue is the result of certain kinds of expressions involving rvalue references. [Example: The result of calling a function whose return type is an rvalue reference is an xvalue.]
  + A glvalue (“generalized” lvalue) is an lvalue or an xvalue.
  + An rvalue (so-called, historically, because rvalues could appear on the right-hand side of an assignment expression) is an xvalue, a temporary object or subobject thereof, or a value that is not associated with an object.
  + A prvalue (“pure” rvalue) is an rvalue that is not an xvalue. [Example: The result of calling a function whose return type is not a reference is a prvalue]
```
        expressions
          /     \
         /       \
        /         \
    glvalues   rvalues
      /  \       /  \
     /    \     /    \
    /      \   /      \
lvalues   xvalues   prvalue
```
- Important rvalue reference properties:
  + For overload resolution, lvalues prefer binding to lvalue references and rvalues prefer binding to rvalue references. Hence why temporaries prefer invoking a move constructor / move assignment operator over a copy constructor / assignment operator.
  + rvalue references will implicitly bind to rvalues and to temporaries that are the result of an implicit conversion. i.e. `float f = 0f; int&& i = f;` is well formed because float is implicitly convertible to int; the reference would be to a temporary that is the result of the conversion.
  + Named rvalue references are lvalues. Unnamed rvalue references are rvalues. This is important to understand why the std::move call is necessary in: `foo&& r = foo(); foo f = std::move(r);`

- 一些极好的链接  
http://stackoverflow.com/questions/3582001/advantages-of-using-forward
http://stackoverflow.com/questions/3106110/what-are-move-semantics
http://stackoverflow.com/questions/5481539/what-does-t-double-ampersand-mean-in-c11
http://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
http://stackoverflow.com/questions/3601602/what-are-rvalues-lvalues-xvalues-glvalues-and-prvalues
