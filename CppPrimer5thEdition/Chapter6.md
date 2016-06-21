##### 函数（Function）
- 函数体形成了一个作用域(scope)。

- C++是静态强类型语言，每次函数调用，编译器都会进行参数检查（类型和个数），因此函数必须先被声明才能使用。

- 函数的形参表称为函数的符号特征(signature)，可用来区分函数的不同实例(重载函数)。
  有了函数名和符号特征，就可以唯一的标识函数了。
 + 对于C++，在编译器层面上，函数名+形参表 -> name mangling -> 生成编译器用于区分函数的符号；
 + 对于C，在编译器层面上，函数名 -> 生成编译器用于区分函数的符号。

##### 形参（Parameter）
- 位于函数声明和定义的形参表中。

- 生命期始于函数调用直到函数返回，作用域在函数体内。

- 形参必须在命名后才能被使用。

- 形参由实参初始化或者是缺省值初始化
```
void foo(); 
// C++中等价于 void foo(void)
// C 中可以接受任何参数，必须显式声明为void foo(void)
```

- 省略号（ellipsis）挂起了类型检查机制，它的出现告诉编译器，当函数调用发生时，可以有0个或者多个实参，而实参的类型未知。
- 省略号参与重载决议。
```
void foo(parm_list...);
void foo(); // 并不等价于 void foo(...);

void foo() {}
void foo(...) {}

foo(); // error: call of overloaded 'foo()' is ambiguous

// Use command "nm -a"
void foo(int, ...) {}   // 08048404 T _Z3fooiz
void foo(int) {}   //  0804840a T _Z3fooi
void foo(...) {}   //  08048410 T _Z3fooz
```

##### 实参（Argument）
- 函数调用发生时，实参作为初始值初始化形参。

- 尽管实参和形参存在对应关系，但是并没有规定实参的求值顺序，编译器能以任意可行的顺序对实参求值。

- 实参必须和形参类型相同，或者可以隐式转换为形参类型。

##### 参数传递
- 形参初始化的机理和变量初始化一样

- 当形参是是引用类型时，形参是它对应的实参的别名，称为按引用传递（passed by reference）
- 当实参的值被拷贝给形参时，形参和实参是两个互相独立的对象，称为按值传递（passed by value）

###### 非引用类型的形参初始化（按值传递）
- **非指针类型**
 + 顶层const被忽略
 + 可用const对象初始化非const对象，反之亦可
 + 在C/C++中，按值传递时，const形参和非const形参的函数无区别，因为形参的顶层const被忽略
```
extern void foo1(const int x);
extern void foo2(int x);

int x = 100;
const int y = 90;
foo1(x);  // Okay
foo1(x);  // Okay
foo2(y);  // Okay
foo2(y);  // Okay
```

```
void foo(const int x);
void foo(int x); // error: redefinition of 'void foo(int)'

> nm -a a.out | grep foo
08048404 T _Z3fooi  // void foo(const int x);
```

- 指针类型
 + 底层const不可忽略
 + 指针形参指向的是const对象还是非const对象，将影响函数调用时可以使用的实参，因为形参的底层const不可被忽略
 + 可以将指向const对象的指针初始化为指向非const对象，但是不能让指向非const对象的指针指向const对象。
```
int x = 100;
const int* p = &x; // Okay

const int x = 100;
int* p1 = &x; // Wrong: invalid conversion from 'const int*' to 'int*'
const int* p2 = &x; // Okay

extern void foo1(const int* p);
extern void foo2(int* p);

int x = 100;
const int y = 90;

foo1(&x); // Okay
foo2(&x); // Okay

foo1(&y); // Okay
foo2(&y); // error: invalid conversion from 'const int*' to 'int*'

void foo(const int* p) {}
void foo(int* p) {} // Okay, overloaded function

void foo(int* const p) {}
void foo(int* p) {} // error: redefinition of 'void foo(int*)'
```

##### 引用类型的形参初始化(按引用传递)
- 引用类型的形参是对应对象的别名，在初始化时直接绑定到对应的实参对象。
- 利用引用形参修改实参的值：
`void swap(int& x, int& y);`
- 使用引用形参传回额外的信息：
`int foo(int& ret);`
- 节省传递复杂庞大对象的开销：
`void foo(const string& s);`
- 在有效地实现重载操作符的同时，还能保证用法的直观性：
 `Matrix operator+(const Matrix& m1, const Matrix& m2);   // m = m1+m2;`
- 引用类型的形参必须指向一个对象，因此如果一个参数可能在函数中指向不同的对象，或者不指向任何对象，那么就不能定义为引用类型。
- const引用，应该将不需要修改的引用形参声明为const引用。
- 非const引用形参只能绑定到相同类型的非const对象实参。因此，非const引用形参在使用时不太灵活，这样的形参既不能用const对象初始化，也不能用字面值或产生右值的表达式实参初始化。
```
int incr(int &val) { return ++val; }
int main()
{
    short v1 = 0;
    const int v2 = 42;
    int v3 = incr(v1);   // error: v1 is not an int
    v3 = incr(v2);       // error: v2 is const
    v3 = incr(0);        // error: literals are not l-values
    v3 = incr(v1 + v2);  // error: addition doesn't yield an l-value
    int v4 = incr(v3);   // ok: v3 is a non const object type int
}
```

```
void foo1(const string& s) {}
void foo2(string& s) {}
string s1;
const string s2;
foo1("hello world"); // Okay
foo1(s1); // Okay
foo1(s2); // Okay
foo2(s1); // Okay
foo2(s2); // Error
``` 

##### 数组形参
- 数组永远不会按值传递，数组长度不是参数类型的一部分（多维数组除第一维之外）
`void foo(int arr[100]);  // 等价于void foo(int* arr);`
- 如果形参是数组类型的引用，数组长度则称为参数的一部分
`void foo(int (&arr)[10]);  // 必须接收 int [10] 类型的实参`
- 数组形参的类型是指向数组元素类型的指针类型
```
extern void foo(int*);
extern void foo(int []);
extern void foo(int [10]);  // Three equivalent definitions of foo
```
- 多维数组实质是数组的数组，因此，对于多维数组参数，必须显式指明除了第一维之外所以维的长度。
`void foo(int array[][10]);   // Okay,  等价于 void foo(int (*array)[10]);`

##### 默认实参
- 默认实参有利于函数接口的设计细节。调用函数时，可以省略有默认值的实参，编译器会为省略的实参提供默认值。
- 默认实参必须从函数形参表的尾部向前指定。
- 既可以在函数定义也可以在函数声明中指定默认实参，但是，在一个编译单元中，只能为一个形参指定默认实参一次。
```
// ff.h
int foo(int = 0);
// ff.cpp
#include "ff.h"
int foo(int i = 0) {} // error

// ff.h
int foo(int, int, int = 9);

// ff.cpp
int foo(int x, int y = 8, int z) {} // Ok
int foo(int x, int y = 8, int z = 9) {} // error
```

- 默认实参只对包含了默认值的文件有效，因此通常在函数声明中指定默认实参。

- 默认实参可以是任意形式的表达式，即可以是运行时才可决定的结果：
```
int foo(int value = calc()) {}
```

##### 返回类型和return语句

- 返回非引用类型：返回值用于初始化函数调用处的对象（可能是临时对象）
- 返回引用类型：返回的是左值，我们可以为返回类型是非const引用的函数的结果赋值
```
char& get_val(string &str, string::size_type ix) { return str[ix]; }
get_val(s, 0) = 'A'; // ok

const string& shorterString(const string &s1, const string &s2);
shorterString("hi", "bye") = "X"; // error
```

##### 列表初始化返回值
- C++11规定，函数可以返回花括号包围的值的列表，用于对表示函数返回的临时量进行初始化
- 如果列表为空，临时量执行值初始化，否则，返回的值由函数的返回类型决定
```
vector<string> process()
{
    if (expected.empty())
        return {}; // 返回一个空vector列表
    else if (expected == actual)
        return {"functionX", "Okay"};
    else
        return {"functionX", expected, actual};
}
```

##### 主函数main()的返回值
我们允许main()函数没有return语句直接返回，此时编译器会隐式地插入一条返回0的return语句

##### 返回数组指针
```
typedef int arrT[10];
using arrT = int[10]; // 等价于typedef int arrT[10]
arrT* func(int i);
int (*func(int i))[10]; // 等价于arrT* func(int i)
```

##### 使用尾置返回类型（Trailing return type）
C++11引入了尾置返回类型，任意函数的定义都可以使用尾置返回类型，但是这种形式对于返回类型复杂的函数最为有效
```
auto func(int i) -> int (*)[10];
```
 
##### 使用decltype
```
int array[10];
decltype(array) *foo(); // 返回值类型是int(*)[10]，数组作为decltype关键字的参数不发生退化
```

##### 函数指针
- 函数的类型由函数的返回值类型和形参类型决定，省略号是函数类型的一部分。
```
int printf(const char*, ...);
int strlen(const char*);  // 和printf不是同一类型
```
- 当一个函数名没有被调用操作符修饰时，会被解释为指向该类型函数的指针。
- 取址运算符作用在函数名上也会产生指向该函数类型的指针。
```
int foo(int);
foo; // int (*)(int)
&foo; // int (*)(int)
```
- 在函数指针之间不存在隐式类型转换
```
typedef int (*PF1)(int);
typedef void (*PF2)(void);

PF1 pf1;
PF2 pf2;

pf1 = pf2; // error: invalid conversion
pf1 = (PF1)pf2; // ok: explicit cast
pf1 = static_cast<PF1>(pf2); // error
pf1 = reinterpret_cast<PF1>(pf2); // ok
```

- 通过函数指针调用函数，有如下两种形式：
```
(*pf)(); // 显式
pf(); // 缩写
```

- 使用typedef或using可以简化函数指针类型的声明和定义。
```
typedef int (*PF)(const string&, const string&);
using PF = int (*)(const string&, const string&); // 等价于typedef
PF pfs[2] = {foo1, foo2}; // 函数指针数组
PF (*pf)[2] = &pfs; // 指向函数指针数组的指针
pfs[0](s1, s2);
(*pf)[0](s1, s2); // 显式调用
```

- 函数指针也可作为函数的返回值类型
```
typedef int (*PF)(int*, int);
PF ff(int); // 等价于 int (*ff(int))(int*, int)
```

- 将auto和decltype用于函数指针类型
```
string::size_type sumLength(const string&, const string&);
string::size_type largerLength(const string&, const string&);
decltype(sumLength) *getFcn(const string&);
// 当用函数名作为decltype的参数时，返回的是函数类型而非指针，因此需要显式加上*
```

- 指向C链接属性的函数指针 和 指向C++链接属性的函数指针类型不同
```
extern "C" int (*PF1)(int); // C链接属性
int (*PF2)(int); // C++链接属性
```

- 链接指示符作用在函数及其形参上
```
extern "C" int (*PF)(int (*)(int)); // 函数参数也具有C链接属性

extern "C" int (*PF)(int);
int foo(PF pf); // pf具有C链接属性，foo具有C++链接属性
```

##### 内联函数（in-line function）
- inline 函数可以避免函数调用的开销，它被编译器在函数调用点上内联的展开
- inline 仅仅是对编译器的建议，编译器可以选择忽略它
- inline 函数定义必须对编译器可见，以便编译器在函数调用点上将其展开

##### constexpr函数
- constexpr函数是指能用于常量表达式的函数
- constexpr函数的返回类型及所有形参的类型都必须是字面值类型(literal type)，而且函数体中必须有且只有一条return语句
```
constexpr int new_sz() { return 100; }
constexpr int foo = new_sz(); // ok, foo是一个常量表达式``
 
class T {};
constexpr T* foo() { return nullptr; }
constexpr T* p = foo();
```
- 执行初始化任务时，编译器把对constexpr函数的调用替换为其结果值，为了能在编译过程中随时展开，constexpr函数被隐式地指定为内联函数
- constexpr函数体内也可以包含其他语句，只要这些语句在运行时不执行任何操作就可，例如，空语句，using声明和类型别名。
- constexpr函数的返回值可以不是一个常量
```
constexpr size_t scale(size_t cnt) { return new_sz()*cnt; } // 如果arg是常量，则scale(arg)也是常量表达式
int arr[scale(2)]; // ok, scale(2)是常量表达式

int i = 2;
int arr2[scale(i)]; // error, i不是常量，因此scale(i)不是常量表达式
```

- 把内联函数和constexpr函数放在头文件中, 和其它函数不同，内联函数和constexpr函数可以被多次定义，通常将它们的定义置于头文件中

##### assert预处理宏
- assert宏定义在cassert头文件中，用于检查“不能发生”的条件

##### 重载函数（Overloaded Function）
- 在同一作用域中的同名函数，但是形参表不同。
- 函数重载將在同一作用域中为函数取名的词汇复杂性中解放出来，如果不同的函数名对于程序的理解性可读性更有益处的话，则无需使用重载函数。
- main()函数不能重载
- 当一个函数名在一个作用域中被声明多次的时候，编译器按照如下步骤解释后续的声明：
 + 如果函数形参表不同，则是函数重载
 + 如果形参表和返回类型精确匹配(不包括形参名)，则是重复声明（Okay）
 + 如果形参表完全匹配，返回类型不同，则标识后续声明为重复声明（Error）
 + 如果只有缺省实参不同，则为重复声明。
```
int foo(int, double) { return 0; }
int foo(int, double); // ok, declaration
void foo(int, double); // error, redefintion
void foo(int); // ok, overloaded
```

- 省略号(...)参与函数重载决议
- bool类型参与函数重载决议

- const和volatile限定符作用于形参：
 + 顶层const不参与重载决议, 
 + 底层const参与重载决议
 + cv-qualifier修饰引用类型(底层const)，参与重载决议：
```
int foo(int&);
int foo(volatile int&); // ok
int foo(const int&); // ok
```
 + 修饰指针指向的类型（底层const），参与重载决议：
```
int foo(int *);
int foo(volatile int*); // ok
int foo(const int*); // ok
```
 + 修饰指针本身(顶层const)，不参与重载决议：
```
int foo(int *);
int foo(int *volatile); // error，redefintion
int foo(int *const); // error，redefinition
```
 + 修饰值传递类型(顶层const)，不参与重载：
```
int foo(int);
int foo(volatile int); // error, redefintion
int foo(const int);  // error, redefinition
```

##### 函数重载和作用域
- 函数重载是针对一个作用域而言的，using指示符会对函数重载产生影响
- using指示符引入的函数声明会参与对应作用域中的函数重载决议。
```
using namespace xxx;
int foo(int); // xxx名字域中的foo函数参与当前域中的函数重载
```

- C++中的名字查找发生在类型检查之前
```
void print(const string &);
void print(double); // overloaded
void foo()
{
    void print(int); // 新作用域，隐藏了外部作用域的print函数
    print("hello"); // error，外部作用域的print(const string&)函数被隐藏
    print(3.14); // ok，调用的是print(int)，外部作用域的print(double)函数被隐藏
    print(100); // ok，调用的是print(int)
}
```
 
- C链接修饰符（extern "C"）只能修饰重载函数集中的一个（C/C++链接性会决定编译器内部生成的名字）
```
extern "C" void foo(void); // foo
extern "C" int foo(int); // error, redefintion, 还是 foo
```

```
extern "C" void foo(void); // foo
int foo(int); // ok，overloaded
```

- 给指向重载函数的指针初始化或赋值时，将会精确匹配返回值类型和形参表
```
int foo(void);
int foo(int);
int (*FP)(int) = &foo; // 精确匹配
```
 
- const_cast和重载函数
```
const string& shorterString(const string &s1, const string &s2)
{
    return s1.size() <= s2.size() ? s1 : s2;
}

string& shorterString(string &s1, string &s2)
{
    auto &r = shorterString(const_cast<const string&>(s1), const_cast<const string&>(s2));
    return const_cast<string&>(r);
}
```
