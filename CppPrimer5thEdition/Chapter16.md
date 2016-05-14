- 所谓泛型编程就是以独立于任何特定类型的方式编写代码（代码与对象类型彼此独立）
- 模板是泛型编程的基础
- 泛型编程（Generic Programming）：**编译时多态**
- 面向对象（OO）：**运行时多态**

###### 函数模板
- 模板定义以template开始，后接模板形参表（template parameter list）
- 模板形参表是以 <> 括住的一个或多个模板形参的列表，模板形参表不能为空
- 模板形参可以是表示类型的类型形参（type parameter），也可以是表示常量表达式的非类型形参
- 类型形参在关键字class或typename之后，class 和 typename 在此处具有相同的含义
- 非类型形参在类型说明符之后，非类型模板形参的模板实参必须是常量表达式
```
template <typename T, int x>  // T为类型形参，x为非类型形参
inline void foo() // 函数模板可以声明为inline
{
    T array[x]; // x为常量表达式
}
foo<int, 100>();
```
- 使用类模板时，必须**显式**指定实参
`std::vector<int> vi;`
- 使用函数模板时，可以显式指定实参，也可以让编译器自己推断
- 模板形参的作用域从声明开始一直到模板声明或定义的结束
- 模板形参遵循常规的名字屏蔽（hide）规则，与外层作用域中声明的对象，函数或类型同名的模板形参会屏蔽外层作用域中的名字。
```
typedef double T;
template <typename T>
void foo()
{
    T a; // T为模板形参
}
```
 
- 用作模板形参的名字不能在模板内部重用。
```
template <typename T>
void foo(T a)
{
    typedef double T; // error: redefinition of formal parameter
}
```
 
- 同一模板的声明和定义中，模板形参的名字可以不同。
```
template <typename T> void foo();
template <typename U> void foo(); // 相同的函数声明
```

###### typename关键字
- 在模板定义的内部指定类型，通过在成员名前加上关键字typename来显式地告诉编译器将成员当做类型。
```
template <typename T, typename U>
T foo(T *array, U value)
{
    T::sizetype *p; // error，编译器无法确定究竟是*运算还是指针的声明
    typename T::sizetype *p; // ok
}

template <typename T>
typename T::value_type bop(const T& c)
{
    if (!c.empty())
        return c.back();
    else
        return typename T::value_type(); // 值初始化的元素
}
```

- typename不得在base class lists或member initialization list内以它做为base class的修饰符。

###### 编写独立于类型的代码（writing type-independent code）
- 在模板内部完成的操作限制了可用于实例化该模板的类型，因此，应该保证用作函数实参的类型实际上支持所有的操作。
- 编写模板代码时，对实参类型的要求尽可能少是有益的，两个重要的原则：
 + 模板形参是const引用 `template <typename T> void foo(const T&);`
 + 函数体中的测试只用 `<` 比较

###### 实例化（instantiation）
- 模板本身并不是函数或类，编译器用模板产生指定的函数或类的特定类型版本。
- "实例化"这个术语反映了创建函数模板和类模板的新"实例"的概念。
- 当编译器遇到一个模板定义时，它并不生成代码。
- 类模版在引用实际的类模板类型的时候被实例化，必须显式指定类模板实参
```
Queue<int> qi;
Queue<string> qs; // 类模板每次实例化都会产生独立的类类型，qs 和 qi 没有关系
```
- 函数模板在调用它或用它对函数指针进行初始化或赋值的时候 被实例化，可以显式的指定函数模版实参或者让编译器推断
```
template <typename T> int compare(const T& v1, const T& v2);
compare(10, 15); // 函数模板形参被绑定到int类型，编译器推断
compare(2.14, 3.15); // 函数模板形参被绑定到double类型，编译器推断
compare<int>(10, 15); // 显式指定实参类型，显式指定
```

- C++ Primer 5th Exercise 16.27
```
template <typename T> class Stack 
{
    // could use the following idiom to check whether instantiation is happened.
    typedef typename T::ThisDoesntExist StaticAssert; // T::NotExisting doesn't exist at all!
};
void f1(Stack<char>); // No instantiation, compiles
class Exercise {
    Stack<double> &rsd; 
    // No instantiation, compiles (references don't need instantiation, are similar to pointers in this)
    
    Stack<int> si; // Instantiation! Doesn't compile!!
};
int main(){
    Stack<char> *sc; // No Instantiation, this compiles successfully since a pointer doesn't need instantiation
    f1(*sc); // Instantiation of Stack<char>! Doesn't compile!!
    int iObj = sizeof(Stack<std::string>); // Instantiation of Stack<std::string>, doesn't compile!!
}
```

```
template<class T> struct Z {
    void f();
    void g();
};

void h() 
{
    Z<int> a;     // instantiation of class Z<int> required
    Z<char>* p;   // instantiation of class Z<char> not required
    Z<double>* q; // instantiation of class Z<double> not required
    a.f();        // instantiation of Z<int>::f() required
    p->g();       // instantiation of class Z<char> required, and instantiation of Z<char>::g() required
}
```

- 控制实例化
当模板被使用时才会进行实例化这一特性意味着，相同的实例可能出现在多个编译生成的对象文件中。
当多个独立编译的源文件使用了相同的模板，并提供了相同的模版参数时，每个文件中都会有该模板的一个实例。
在大系统中，在多个文件中实例化相同模板的额外开销可能非常严重。
在C++11中，我们可以通过显式实例化（explicit instantiation）来避免这种开销。
```
extern template declaration; // 实例化声明
template declaration; // 实例化定义
```

- 类模板的实例化定义会实例化所有成员
因此，在一个类模板的实例化定义中，所用类型必须能用于模板的所有成员函数。
```
class NoDefault
{
    NoDefault() = delete;
};
template vector<NoDefault> v; // error
```

###### 模板实参推断（Template Argument Deduction）
- 从函数调用的实参确定模板实参的类型和值的过程叫 模板实参推断。
- 与函数模板不同之处是，编译器不能为类模板推断模板参数类型。
- 多个类型形参的实参必须完全匹配
```
template <typename T> int compare(const T& v1, const T& v2);
short si;
compare(si, 1024);
// error, cannot instantiate compare(short, int), must be compare(short, short) or compare(int, int)
```

- 如果一个函数形参的类型使用了模板类型参数，那么它采用特殊的初始化规则。只有很有限的几种类型转换会自动应用于这些实参。
编译器通常不是对实参进行类型转换，而是生成一个新的模板实例。
 + 顶层const无论在实参还是形参中，都会被忽略
 + 其它类型转换，例如 算术转换，派生类向基类的转换 以及 用户定义的转换 都不能应用于函数模板
 + const转换:可以将一个非const对象的引用(或指针)传递给一个const的引用(或指针)形参
 + 数组或函数到指针的转换: 如果函数形参不是引用类型，则对数组或函数类型的实参执行到指针的转换
```
template <typename T>
void foo(const T&, const T&) {}

string s;
foo(s, s); // foo(const string&, const string&)，const转换
foo(10, 100); // foo(const int&, const int&)

short si = 100;
foo(si, si); // foo(const short&, const short&)，const转换

const int x = 1000;
foo(x, x); // 同一个foo(const int&, const int&)实例

const string cs;
foo(cs, s); // 同一个foo(const string&, const string&)实例

// 从nm可以看出生成了三个函数实例
nm -a a.out
0000000100000e50 T __Z3fooINSt3__112basic_stringIcNS0_11char_traitsIcEENS0_9allocatorIcEEEEEvRKT_S9_
0000000100000e60 T __Z3fooIiEvRKT_S2_
0000000100000e70 T __Z3fooIsEvRKT_S2_
```

```
template <typename T> T fobj(T, T);
template <typename T> T fref(const T&, const T&);

string s1;
const string s2;
fobj(s1, s2); // ok, calls fobj(string, string), 顶层const忽略
fref(s1, s2); // ok, s1 -> const string，const转换

int a[10], b[42];
fobj(a, b); // ok，f(int*, int*), 数组向指针的转换
fref(a, b); // error, 函数模板形参是引用
```

```
template <class T> int compare(const T&, const T&);
compare("hi", "world"); // error
compare("bye", "dada"); // ok, const char[4] -> compare(const char[4]&, const char[4]&);
```

- 函数模板的显式实参
在某些情况下，不可能推断出模板实参的类型，例如：当函数的返回值类型必须与形参表中所有的类型都不同时。
因此，在返回类型中使用类型形参，或者为了消除二义性，可以显式指定实参。
```
template <typename T1, typename T2, typename T3>
T1 sum(T2, T3);
auto val3 = sum<long long>(i, lng);
```

- 显式函数模板实参按照从左至右的顺序与对应的模板参数匹配
```
template <typename T1, typename T2, typename T3>
T3 alternative_sum(T2, T1);
auto val3 = alternative_sum<long long>(i, lng); // error
auto val2 = alternative_sum<long long, int, long>(i, lng); // ok
```

- 函数指针和实参推断
当用一个函数模板初始化一个函数指针或者为一个函数指针赋值时，编译器使用指针的类型来推断模板实参
```
template <typename T> int compare(const T&, const T&);
int (*pf1)(const int&, const int&) = compare; // pf1指向实例 int compare(const int&, const int&);
```

- 有时候需要显式模板实参来指出究竟实例化哪个版本
```
template <typename T> int compare(const T&, const T&);
void func(int(*)(const int&, const int&));
void func(int(*)(const string&, const string&));

func(compare); // error, 二异性，不知道实例化哪个版本
func(compare<int>); // 实例化compare(const int&, const int&);
```

- 模板默认实参
在C++11中，我们可以为函数和类模板提供默认实参
```
template <typename T, typename F = less<T>>
int compare(const T &v1, const T &v2, F f = F())
{
    if (f(v1, v2)) return -1;
    if (f(v2, v1)) return 1;
    return 0;
}
```
- 无论何时使用一个类模板，都必须在模板名之后接上尖括号
```
template <class T = int>
class Numbers
{
public:
    Numbers(T v = 0) : val(v) {}
private:
    T val;
};
Numbers<long double> lots_of_precision;
Numbers<> average_precision; // 空<>表示我们希望使用默认实参
```
 
###### 类模板成员
- 类模板成员函数的形式
```
template <typename T>
void A<T>::member_name()
{
}
```
- 类成员模板不能是虚函数。
- 类模板的成员函数只有在被使用的时候才进行实例化。
```
Queue<int> qi; // 实例化 class Queue<int>
short s = 42;
int i = 42;
qi.push(s); // 实例化Queue<int>::push(const int&)，允许常规转换
qi.push(i); // 使用Queue<int>::push(const int&)
```
- 因为对象的模板形参可以确定其成员函数的模板形参，
所以调用类模板成员函数相比函数模板, 用模板形参定义的函数形参的实参允许进行常规转换。

###### 模板特化（Template Specializations）
- 模板特化是这样一个定义，该定义中一个或多个模板形参的实际类型或实际值是指定的
- 函数模板的特化
```
template <typename T>
int compare(const T&, const T&);

template <>
int compare<const char*>(const char* const &v1, const char* const &v2)
// special version of compare to handle C-style character strings
{
    return strcmp(v1, v2);
}

const char* cp1 = "world", *cp2 = "hi";
char* p1, *p2;
compare(cp1, cp2); // 特化版本
compare(p1, p2); // 泛型版本
```
- 函数模板特化可以只声明而无定义，特化声明和定义很像，只是忽略了函数体。
- 不能总是检测到重复定义，因此和其它函数声明一样，应在一个头文件中包含模板特化的声明，然后使用该特化版本的每个源文件包含该文件。       
- 如果程序由多个文件构成，模板特化的声明必须在使用该特化的每个文件中出现。
- 不能在一些文件中从泛型模板定义实例化一个函数模板，而在其他文件中为同一模板实参集合特化该函数模板。

###### 类模板的特化(Specializing a class template)
- 类模板特化可以定义与模板本身完全不同的成员。
- 在类模板特化外部定义成员时，成员之前不能加 template<> 标志。
- 类模板可以部分特化
- 类模板特化可以特化成员而不特化类
```
template <typename T>
class FOO
{
public:
    void B(T t) {}
    void B(int i) {}
    void B(int i) const {}
};

template <>
class FOO<bool>
{
public:
    void B(int i) {}
    void B(int i) const {}
};

int main()
{
    FOO<bool> f;
    const FOO<bool> g = f;
    g.B(10);
    return 0;
}
```

###### 重载和函数模版
- 函数模板可以重载，可以定义在相同名字但形参数目或类型不同的多个函数模板，也可以定义与函数模板有相同名字的普通非模板函数。
- 建立候选函数集合
 + 同名的普通函数
 + 相匹配的函数模板实例化（模板实参推演）
- 确定可行函数
 + 模板实例都是可行的
- 如果需要转换，则根据转换的种类排列可行函数（调用函数模板所允许的转换是有限的）
 + 如果仅有一个函数，则调用该函数
 + 如果有二义性，从可行函数中去掉所有函数模板实例
- 重新排列去掉了模板实例的可行函数
 + 只有一个函数，调用它
 + 存在二义性
```
template <typename T> int compare(const T&, const T&); // (1)
template <> int compare<const char*>(const char * const&, const char * const&); // (2)
int compare(const char*, const char*); // (3)

char *p1, *p2;
const char *cp1, *cp2;
compare(cp1, cp2); // (3)
compare(p1, p2); // (3)

// 如果没有(3)声明呢：
compare(cp1, cp2); // (2)
compare(p1, p2);  // (1)
```
 
###### 模板实参推断 和 引用
- 从左值引用函数参数推断类型
当一个函数参数是模板类型参数的一个左值引用时（形如T&），绑定规则告诉我们：只能传递给它一个左值。
实参可以是const类型，也可以不是。如果实参是const的，则T被推断为const类型
```
template <typename T> void f1(T&); // 实参必须是一个左值
f1(i); // i是int；T是int
f1(ci); // ci是const int；T是const int
f1(5); // error：实参不是左值
```

当一个函数参数的类型是const T&，绑定规则告诉我们：可以绑定到左值或右值。
当函数实参本身是const时，T的类型推断不会是一个const类型，因为const已经是函数参数类型的一部分，它不会也是模板参数类型的一部分。
```
template <typename T> void f2(const T&); // 可以接收左值或右值
// f2中的参数是const的，实参中的const是无关的
f2(i); // i是一个int；T被推断为int
f2(ci); // ci是const int；但是T被推断为int
f2(5); // 绑定到右值；T被推断为int
```

- 从右值引用函数参数推断类型
当一个函数参数是一个右值引用时（形如T&&），绑定规则告诉我们可以传递给它一个右值。
```
template <typename T> void f3(T&&);
f3(42); // 实参是一个int类型的右值；模板参数T是int
```

###### 引用折叠和右值引用参数
－ 通常我们不能定义一个引用的引用，但是，通过类型别名 或 通过模板类型参数 间接定义是可以的。

typedef int& INT_REF;

int i = 10;

INT_REF &r = i; // ok

 

引用折叠只能应用于间接创建的引用的引用，如类型别名或模板参数。
Exercise 16.45: Given the following template, explain what happens if we call g on a literal value such as 42. What if we call g on a variable of type int?

template <typename T>

void g(T&& val)

{

    std::vector<T> v;

}

 

int main()

{

    g(42); // compiles

 

    int i;

    g(i); // error

}

 

The first of the remaining two rules for rvalue references affects old-style lvalue references as well. Recall that in pre-11 C++, it was not allowed to take a reference to a reference: something like A& & would cause a compile error. C++11, by contrast, introduces the following reference collapsing rules:

 

A& & becomes A&

A& && becomes A&

A&& & becomes A&

A&& && becomes A&&

Secondly, there is a special template argument deduction rule for function templates that take an argument by rvalue reference to a template argument:

 

template<typename T>

void foo(T&&);

Here, the following apply:

 

When foo is called on an lvalue of type A, then T resolves to A& and hence, by the reference collapsing rules above, the argument type effectively becomes A&.

When foo is called on an rvalue of type A, then T resolves to A, and hence the argument type becomes A&&.

So case 1, when passing 42, you are calling g with a rvalue, so T is resolved to int thus g's parameter is int&& and std::vector is legal.

 

In case 2, when passing i, you are calling g with a lvalue, so T is resolved to int& thus g's parameter is int& and std::vector<int&> is NOT legal.

 

Remove the line with the vector and it will work fine in both cases.

 
```
using T1 = int&;
using T2 = int&&;
using T3 = const int&;
using T4 = const int&&;
int x = 100;

T1 &t1 = x;
T2 &t2 = x;

T1 &&t3 = x;
T2 &&t4 = std::move(x);
    
T3 &t5 = x;
T3 &&t6 = x;
    
T4 &&t7 = std::move(x);
```
  
