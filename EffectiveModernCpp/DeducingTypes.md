##### **条款1**: 深刻理解模板类型推断(template type deduction)
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

#### **条款2**: 理解auto类型推断

