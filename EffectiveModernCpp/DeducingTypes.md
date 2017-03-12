##### 条款1: 深刻理解模板类型推断(template type deduction)
```
template <typename T> void f(ParamType param);
f(expr); // 推断T和ParamType的类型
```
- ParamType类型为引用或者指针，但不是Universal Reference
 + 如果expr的类型是引用，那么忽略掉引用，而使用被引用对象的类型
```
template <typename T> void f1(T& param);
template <typename T> void f2(const T& param);
template <typename T> void f3(T* param);

int x = 27;
const int cx = x;
const int& rx = x;

f1(x);   // T is int, ParamType is int&
f1(cx);  // T is const int, ParamType is const int&
f1(rx);  // ignore &, T is const int, ParamType is const int&

f2(x);   // T is int, ParamType is const int&
f2(cx);  // T is int(no longer a need for const to be deduced as part of T), ParamType is const int&
f2(rx);  // ignore &, T is int(no longer a need for const to be deduced as part of T), ParamType is const int&

const int *px = &x;
f3(&x);  // T is int, ParamType is int*
f3(px);  // T is const int, ParamType is const int*
```

- ParamType类型是Universal Reference
 + Universal Reference 在标准中已经重新命名为 Forwarding Reference
 + 在函数模板类型参数列表中，T&&表示Universal Reference
 + 引用折叠的规则:  
 T& &   -> T&  
 T&& &  -> T&  
 T& &&  -> T&  
 T&& && -> T&&
```
 template <typename T> void f(T&& param);
 
 int x = 27;
 const int cx = x;
 const int& rx = x;
 
 f(x);  // T is int&, ParamType is int&
 f(cx); // T is const int&, ParamType is const int&
 f(rx); // T is const int&, ParamType is const int&
 f(27); // T is int, ParamType is int&&
```
 
- ParamType 既不是指针也不是引用
 + 如果expr的类型是引用，那么忽略掉引用，而使用被引用对象的类型
 + 忽略top-level cv-qualifier
```
template <typename T> void f(T param);

int x = 27;
const int cx = x;
const int& rx = x;

f(x); // T is int, ParamType is int
f(cx); // ignore top-level const, T is int, ParamType is int
f(rx); // ignore &, then ignore top-level const, T is int, ParamType is int
```

```
template <typename T> void f(T param);

const char* const ptr = "hello world";
f(ptr); // ignore top-level const, T is const char*, ParamType is const char*
```

- 如果模板类型实参是数组类型
 + 如果模板类型参数在函数参数列表中是按值传递，那么数组退化为指针
 + 如果模板类型参数在函数参数列表中是按引用传递，那么保持数组类型
```
const char name[] = "hello world";

template <typename T> void f(T param);
f(name); // T is const char*, ParamType is const char*

template <typename T> void f(T& param);
f(name); // T is const char[12], ParamType is const char (&)[12];
```

- 如果模板类型实参是函数
 + 如果模板类型参数在函数参数列表中是按值传递，那么函数退化为函数指针
 + 如果模板类型参数在函数参数列表中是按引用传递，那么保持函数类型
```
void someFunc(int, double);

template <typename T> void f1(T param);
template <typename T> void f2(T& param);

f1(someFunc); // T and ParamType is void(*)(int, double)
f2(someFunc); // T is void (int, double), and ParamType is void (&)(int, double);
```
- Things to Remember
 + During template type deduction, arguments that are references are treated as non-references, i.e., their reference-ness is ignored.
 + When deducing types for universal reference parameters, lvalue arguments get special treatment.
 + When deducing types for by-value parameters, const and/or volatile arguments are treated as non-const and non-volatile.
 + During template type deduction, arguments that are array or function names decay to pointers, unless they're used to initialize references.

####条款2: 理解auto类型推断
- 通常情况下，template类型推断的规则可以应用于auto类型推断：可以把auto看作T，auto对象的类型指示符(type specifier)看作ParamType
```
template <typename T>
void f(ParamType param);
f(expr);

auto x = 27; // template <typename T> void f(T param); -> x is int
const auto cx = x;  // template <typename T> void f(const T param); -> x is const int
const auto& rx = x; // template <typename T> void f(const T& param); -> x is const int&
```
- 对于auto类型推断，type specifier(即ParamType)存在和template函数一致的三种形式，对应的推断规则也是一致的
 1. 指针或者引用，但是不是Universal Reference
 2. Universal Reference
 3. 既不是指针，也不是引用
```
auto x = 27;
const auto cx = x;
const auto& rx = x;

auto&& uref1 = x; // uref1 is int&
auto&& uref2 = cx; // uref2 is const int&
auto&& uref3 = 27; // uref3 is int&&
```

- 数组和函数退化为指针的规则依旧适用于auto类型推断
```
const char name[] = "hello world";
auto arr1 = name;  // arr1 is const char*
auto& arr2 = name; // arr2 is const char[12]

void f(int, double);
auto func1 = f;  // func1 is void(*)(int, double)
auto& func2 = f; // func2 is void(&)(int, double)
```

- 使用列表初始化会使auto类型推断和template类型推断的规则变的有所不同
```
auto x1 = 27; // 拷贝初始化，x1 is int
auto x2(27);  // 直接初始化, x2 is int
auto x3 = {27}; // x3 is std::initializer_list<int>
auto x4{27};    // x4 is std::initializer_list<int>

auto x = {11, 23, 9}; // x is std::initializer_list<int>

template <typename T> void f(T param);
f({11, 23, 9}); // error, cannot deduce

auto x = {11, 23, 9}; // x is std::initializer_list<int>
f(x); // ok, T is deduced to std::initializer_list<int>

template <typename T> void f(std::initializer_list<T> param);
f({11, 23, 9}); // T is int, param is std::initializer_list<int>
```

- 永远不要试图返回一个`std::initialzier_list`类型，不安全，因为`std::intializer_list`指向的存储空间有可能在栈上（如果列表的值不全是常量，一定在栈上）。跟不能返回局部变量的引用一样。 

- C++14新引入了auto的用法，如果auto用在函数返回类型 或 lambda表达式的形参列表中，那么推断是按照template类型推断规则来处理的
```
auto createInitList() { return {1, 2, 3}; } // error, auto作为函数返回类型时，永远不要直接返回braced-init-list

std::vector<int> v;
auto resetV = [&](const auto& newValue) { v = newValue; };
resetV({1, 2, 3}); // error
```
- 一个代码中的例子：
```
struct Data { 
    int value; 
    Data() {value=1;} 
}; 
  
class MyClass { 
    Data data; 
public: 
    Data& getData() {return data;}         
}; 
  
int main(int argc, char** argv) { 
    MyClass c; 
    auto data = c.getData(); // auto& data = c.getData();
    data.value = 2; 
    return 0; 
} 
```
- Things to Remember
 + auto type deduction is usually the same as template type deduction, but auto type deduction assumes that a braced initializer represents a std::initializer_list, and template type deduction doesn't.
 + auto in a function return type or a lambda parameter implies template type deduction, not auto type deduction.

####条款3: 理解decltype
- 相较于auto类型推断和template类型推断，decltype的类型推断规则要简化清晰很多；通常情况下，decltype类型推断不会改变用于推断的类型
```
const int i = 0; // decltype(i) is const int
bool f(const Widget& w); // decltype(w) is const Widget&;  decltype(f) is bool(const Widget&)

struct Point {int x; int y;}; // decltype(Point.x) is int

Widget w; // decltype(w) is Widget

if (f(w)) ... // decltype(f(w)) is bool

vector<int> v; // decltype(v) is vector<int>
if (v[0] == 0)... // decltype(v[0]) is int&
```
- 对于vector的bool类型的特化版本，operator[]返回的并不是bool&
- 如果decltype使用的表达式不是一个变量，如果表达式是一个左值表达式，那么推断的结果是左值引用
```
int x = 10;
int y = 20;
decltype(x = y) z = x; // x = y是左值表达式，z是int&
```
- 如果decltype的表达式是加上了括号的变量，结果将是引用
```
int i;
decltype((i)) d1 = i; // d1是int&
decltype(i) d2; // d2是int
```

- 一些有关`decltype`的细节
 + 一个对象的类型包括：声明时的类型(declared type) 和 使用它时候的类型(effective type)
 + `decltype`可以用来区分出 上述两种类型
 + `decltype`用在变量名时，关注的是declared type
 + `decltype`用在表达式时，关注的是effective type; 如果effective type是左值，那么返回的是左值引用
```
struct S { int i = 0; };
const S s;
const S* ps;
decltype(s->i) x;     // x is const int; using declared type
decltype(ps->i) y;    // y is int; using declared type
decltype((ps->i)) z;  // z is const int&; using effective type

const int f();
decltype(f()) x = 0;  // x is const int
decltype((f()) y = 0; // y is const int; 因为 f() 返回的是右值
```
- Things to Remember
 + decltype almost always yields the type of a variable or expression without any modifications.
 + For lvalue expressions of type T other than names, decltype always reports a type of T&.
 
