##### 条款1: 深刻理解模板类型推断(template type deduction)
```
template <typename T> void f(ParamType param);
f(expr); // 推断T和ParamType的类型
```
- ParamType类型为引用或者指针，但不是Universal Reference
 + 如果expr的类型是引用，那么忽略掉引用，而使用引用对象的类型
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
 + 如果expr的类型是引用，那么忽略掉引用，而使用引用对象的类型
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

f(ptr); // T is const char*, ParamType is const char*
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
auto&& uref3 = 27; // uref3 is int
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

- 使用列表初始化会使auto类型推断和template类型推断变的不同
```
auto x1 = 27; // 拷贝初始化，x1 is int
auto x2(27);  // 直接初始化, x2 is int
auto x3 = {27}; // x3 is std::initializer_list<int>
auto x4{27};    // x4 is std::initializer_list<int>

auto x = {11, 23, 9}; // x is std::initializer_list<int>

template <typename T> void f(T param);
f({11, 23, 9}); // error, cannot deduce

template <typename T> void f(std::initializer_list<T> param);
f({11, 23, 9}); // T is int, param is std::initializer_list<int>
```

- C++14新引入了auto的用法，如果auto用在函数返回类型 或 lambda表达式的形参列表中，那么推断是按照template类型推断规则来处理的
```
auto createInitList() { return {1, 2, 3}; } // error

std::vector<int> v;
auto resetV = [&](const auto& newValue) { v = newValue; };
resetV({1, 2, 3}); // error
```

####条款3: 理解decltype
