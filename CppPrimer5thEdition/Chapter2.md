在计算机存储器中，数据以位序列的形式存在着。在位(Bit)这一级上，存储器是没有结构和意义的。  
如何对位序列进行有意义的解释？  
为此，我们在语言层面上引入了类型抽象，能够对位(Bit)序列进行有意义的解释。  
C++ 是一门静态类型（statically typed）语言，类型检查（type checking）发生在编译时。

###基本内置类型（Primitive Built-in Types）
####算术类型
* 整型（integral type，包括字符和布尔类型在内）  
bool类型: C99中也引入了bool类型，定义在stdbool.h头文件中；在C++中，bool类型可以参与重载决议。  
字符类型: char，wchar_t，char16_t，char32_t  
int类型: short，int，long，long long

* 浮点数
float，double，long double

* 整型类型可以按照有无符号区分（unsigned, signed）
char类型包括三种类型：unsigned char，signed char 和 char  
标准仅仅规定了它们的宽度是一致的，但是char究竟是unsigned还是signed则是implementation-defined，我们不能做任何假设。

* 切勿混用有符号类型和无符号类型

####基本内置类型的宽度
基本内置类型的宽度是implementation-defined的，C++标准只规定了一个范围，  
所以除了标准中保证的宽度，我们不能对内建型别的数据宽度做任何假设，否则会引入问题，同时代码也不具备可移植性。

C++标准保证：
对char类型或值为char类型的表达式做sizeof()操作保证得1;  
wchar_t为两个字节宽;
short <= int <= long, int的宽度为机器字长，在32位机上，典型情况下，short为2字节，int为4字节，long为4字节;
float < double，在32位机器上，典型情况，float为4字节，double为8字节;

在C++标准中，bool类型的宽度只需要能容纳下0和1即可，很多编译器的实现选择了1个字节，但是标准不保证sizeof(bool) == 1

在limits.h和climits头文件中，编译器按照标准定义了整数类型的大小（宏定义）

下面是一道很经典的题目，有关整数类型的宽度:  
Write a program to determine the ranges of char, short, int, and long variables,  
both signed and unsigned, by printing appropriate values from standard headers and by direct computation.
```
const int BITS_PER_BYTE = 8;

std::cout << "signed char: " << SCHAR_MIN << " to " << SCHAR_MAX << std::endl;
std::cout << "signed char: " << (int)(signed char)(1<<(sizeof(signed char)*BITS_PER_BYTE - 1))
          << " to " << (int)(signed char)(~(1<<(sizeof(signed char)*BITS_PER_BYTE - 1))) << std::endl;
    
std::cout << "unsigned char: " << 0 << " to " << UCHAR_MAX << std::endl;
std::cout << "unsigned char: " << 0 << " to " << (int)(unsigned char)(~0) << std::endl;
    
std::cout << "int : " << INT_MIN << " to " << INT_MAX << std::endl;
std::cout << "int : " << (1<<(sizeof(int)*BITS_PER_BYTE - 1)) << " to " << ~(1<<(sizeof(int)*BITS_PER_BYTE - 1)) << std::endl;
    
std::cout << "unsigned int: " << 0 << " to " << UINT_MAX << std::endl;
std::cout << "unsigned int: " << 0 << " to " << ~(unsigned int)0 << std::endl;
        
std::cout << "long: " << LONG_MIN << " to " << LONG_MAX << std::endl;
std::cout << "long: " << ((long)1 << (sizeof(long)*BITS_PER_BYTE - 1))
          << " to " << ~((long)1 << (sizeof(long)*BITS_PER_BYTE - 1)) << std::endl;
    
std::cout << "long long: " << LONG_LONG_MIN << " to " << LONG_LONG_MAX << std::endl;
std::cout << "long long: " << ((long long)1 << (sizeof(long long)*BITS_PER_BYTE - 1))
          << " to " << ~((long long)1 << (sizeof(long long)*BITS_PER_BYTE - 1)) << std::endl;
```

在stdint.h头文件中，按照c99标准7.18节定义了一系列具有特定宽度的整数类型（typedef），例如：

```
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
```

在格式化输入/输出这些特定宽度的整数类型时，应使用特定的宏操作以确保代码的正确和移植性，具体参考文件inttypes.h

如果在C++中包含inttypes.h头文件，则必须在包含头文件前确保宏定义 __STDC_FORMAT_MACROS 的存在:

```
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#endif
```

对于这些内建型别，需要注意编译器的隐式转换行为，注意如下代码:

```
#define VALUE_1 ((char)0xAA)
#define VALUE_2 ((char)0xAB)
int main()
{
    uint8_t x = VALUE_1;    // x = 0xAA
    if (x == VALUE_1)       // VALUE_1 和 x 被隐式整型提升为 int，VALUE_1 -> 0xFFFFAA
        printf(“VALUE_1\n”);
    else
        printf(“VALUE_2\n”);
    return 0;
}
```

C++标准中还定义了如下类型：

    size_t: implementation-defined unsigned integer type
    ssize_t: implementation-defined signed integer type
    ptrdiff_t:  implementation-defined signed integer type of the result of subtracting two pointers.
    std::size_type:
    std::difference_type:

####字面值常量(Literal Constant)
字面值常量: 称之为字面值是因为只能用它的值称呼它，称之为常量是因为它的值不能修改。

每个字面值都有相应的类型，只有内置类型存在字面值，可以通过增加前缀和后缀改变字面值常量的默认类型。

* 整型常量：默认为int或long，实际类型取决于其字面值，可有U后缀和L后缀；
* 浮点数常量：默认为double，可有F后缀和L后缀；
* 字符常量：可通过L前缀表示宽字符常量，如 L'a';
* 字符串常量：亦可通过L前缀表示宽字符串常量, 如 L"hello";
* bool常量：true和false
* 指针字面值：nullptr

字面值常量具有存储区域，但是是不可寻址的，可以作为右值。

某些字面值常量的类型在C和C++中是不同的，例如:  
在C中，字符常量的类型是int：sizeof('a') == 4  
在C++中，字符常量的类型是char：sizeof('a') == 1

#### **变量（Variable）**
变量具有右值属性（r-value）和左值属性（l-value），是可寻址的。

在此处需要强调一下对象和变量这两个概念：  
所谓对象，在C++中指的是执行环境中的一块存储区域，该存储区域中的内容则代表了该对象的值；  
所谓变量，指的是一种声明，通过声明，我们将一个名字和一个对象对应起来，通过名字可以操作对象，简而言之，即命名的存储区域。

注意区分：临时对象 和 临时变量。

内建型别变量的初始化:  

1. 显式：使用赋值操作符  

        int a = 100

2. 隐式：使用括号  

        int a(100)

3. 特殊的初始化为零的方式：使用括号  

        int a = int()

4. 列表初始化：使用花括号  

        int a = {0}

如果使用列表初始化并且初始值存在丢失信息的风险，编译器将报错： 
 
    int x;
    float f{x};  // error

变量定义后即立刻可见，因此如下语句在语法上是合法的：  

    int x = x;

注意区分初始化和赋值这两个概念：

    int x = 1;  // copy-initialization
    int x(1);   // direct-initialization
    x = 10;     // assignment

#### **复合类型（Compound type）**
复合类型的声明语句由一个基本数据类型（basic type）和紧随其后的一个声明符（declarator）列表组成。

##### **指针（Pointer）**
* 指针用于指向一个对象，从而我们可以通过指针去间接地操作该对象

* 指针的值是另外一个对象的地址，而指针的类型则用于如何去解析该地址

* 当我们仅关心地址值的时候（不带有指向对象的类型信息）, C/C++提供了void*类型供我们使用  
void*可被除了函数指针以外所有类型的指针赋值  
void*类型指针可用于传递地址值（函数参数，函数返回值，被赋值）以及比较地址值  

        using PF = void (*)(); // 等价于 typedef void (*PF)();
        PF pf;
        void *p1 = pf; // error
        void *p2 = (void*)pf; // ok
        void *p3 = static_cast<void*>(pf); // error
        void *p4 = reinterpret_cast<void*>(pf); // ok

* 指针可用作右值或左值

* 对指针施加解引用操作符（Dereference Operator, *）会返回指针指向对象的左值，通过这种方式我们可以修改指针指向的对象值

        int *p = 10;
        *p = 90; // *p是左值

* 对指针施加取址操作符（Addressing Operator, &）返回的是指向指针的指针，其值为当前指针所在的地址值

        int *p; // &p类型为int**

* 取址操作符作用于对象不会访址，只返回对象的地址：
  
        struct Point3D
        {
            int x;
            int y;
            int z;
        };

        int main()
        {
            Point3D* pPoint = NULL;
            int offset = (int)(&(pPoint->z));  // 计算z的偏移，&(pPoint->z)不会发生访存操作
            printf("%d", offset);
            return 0;
        }

* 指针与const限定符: 
 
        const int *ptr;
        int const *ptr; // 和a一致，指针指向的对象为const的
        int *const ptr; // 指针本身是const的，const指针必须被初始化
        const int *const ptr;
        int const *const ptr; // 和d一致，指针本身以及指向的对象都是const的

* const指针必须被初始化:

        int x = 100;
        int *const p = &x;

* 指向非const类型的指针不能被const对象的地址初始化和赋值:

        const int x = 100;
        int *ptr = &x; // error, invalid conversion from 'const int*' to 'int*'
        const int *ptr = &x; // ok
        const int *const ptr = &x; // ok

* 指向const对象的指针可以被非const对象的地址或者const对象地址初始化和赋值  
因此应该理解为"自以为指向const对象的指针"  

        int x = 100;
        const int * p = &x; // ok, 自以为指向const对象的指针
        x = 200; // ok

##### **引用 (Reference)**
* 引用相当于对象的别名（alias），通过引用我们可以间接地去操纵该对象

* 此处涉及的引用是对左值的引用（l-value reference）, 在C++11中引入了右值引用(r-value reference)

* 引用不是对象，没有地址，指针不能指向引用，引用可以绑定到指针  

        int i = 42;
        int *p;      // p is a pointer to int
        int* &r = p; // r is a reference to the pointer p
        r = &i;      // r refers to a pointer; assigning &i to r makes p point to i
        *r = 0;      // dereferencing r yields i, the object to which p points; changes i to 0

* 非const引用必须被同类型的对象初始化，const对象不能绑定到非const引用上

* const引用则可以绑定到不同但相关的类型的对象或者右值上，const引用可能会引入临时对象  

        double d = 9.0;
        const int &rx = d; // 编译器会隐式地产生一个int类型的临时对象(左值)，用于rx的初始化，
                           // 此处rx并非为d的引用，而是用d值初始化的int类型的临时对象的引用。

* const引用可以被非const对象初始化，所以应该理解为”自以为是对const对象的引用”  

        const int &ry = 100; // 该引用实际绑定到用100作为右值初始化的临时对象（左值）上
        int x = 10;
        const int &rx = x; // rx是x的引用，两者为同一对象
        x = 20; // Ok
        const int x = 10;
        const int &rx = x; // rx是x的引用，两者为同一对象

* 从汇编层面上，引用的实现:  

        int x = 99;   movl $99, -24(%ebp)
        int &rx = x;  leal -24(%ebp), %eax // 实际就是通过被引用对象的地址进行操作
                      movl %eax, -16(%ebp)

#####const限定符 （const qualifier）
* const限定符修饰的对象是左值，但是不是可修改的左值

* const对象必须初始化  

        const int i = get_size(); // ok: initialized at run time
        const int j = 42; // ok: initialized at compile time
        const int k; // error: k is uninitialized const

* const对象默认为文件的局部变量，除非特别说明，在全局作用域声明的const 变量是定义该对象的文件的局部变量。
此变量只存在于那个文件中，不能被其他文件访问。通过指定 const 变更为 extern，就可以在整个程序中访问 const 对象。  

        const int x = 10; // 内部链接属性，仅当前文件可见
        extern const int y = 10; // 外部链接属性，全局可见

* 常量折叠（const folding），编译器在可能的情况下，会将const对象由其值替代。

        const int size = 100; // defined in test.h
        int array[size];  // int array[100]

* 顶层const(top-level const)和 底层const(low-level const)

> A top level const qualifier affects the object itself. Others are only relevant with pointers and references. They do not make the object const, and only prevent modification through a path using the pointer or reference. Thus:

> ```
char x;  char const* p = &x;
> ```

> This is not a top level const, and none of the objects are immutable. The expression \*p cannot be used to modify x, but other expressions can be; x is not const. For that matter `\*const_cast<char\*>( p ) =
'x'` is legal and well defined.
But

> ```
char const x = 'x';  char const* p = &x;
> ```

> This time, there is a top level const on x, so x is immutable. No expression is allowed to change it (even if const_cast is used). The compiler may put x in read only memory, and it may assume that the value of x never changes, regardless of what other code may do.
To give the pointer top level const, you'd write:

> ```
char x = 'x';  char *const p = &x;
> ```

> In this case, p will point to x forever; any attempt to change this is undefined behavior (and the compiler may put p in read-only memory, or assume that \*p refers to x, regardless of any other code).

* 非引用类型的const对象初始化:const对象可以由非const对象初始化，反之亦可。

        int x = 100;
        const int y = x; // Okay
        const int x = 100;
        int y = x; // Okay

* 普通的指针不能被const对象的地址初始化和赋值。

        int x = 100;
        const int *p = &x; // Okay
        const int x = 100;
        int *p = &x; // error, invalid conversion from 'const int*' to 'int*'

* const引用类型对象初始化：  
const引用可以被非const对象初始化；const引用可以被绑定到相关类型（存在隐式转换）或者右值上，会引入临时对象。

        double dval = 3.14;
        const int &ri = dval;
        const int temp = dval; // create a temporary const int from the double
        const int &ri = temp; // bind ri to that temporary object

#####constexpr和常量表达式
* 字面值属于常量表达式，用常量表达式初始化的const对象也是常量表达式。

        const int max_files = 20;
        const int limit = max_files+1;
 
* constexpr变量  
C++11规定，允许将变量声明为constexpr类型以便由编译器来验证变量的值是否是一个常量表达式。
声明为constexpr的变量**一定**是一个常量，而且**必须用常量表达式初始化**。

        constexpr int mf = 20;
        constexpr int limit = mf+1;
        constexpr int sz = size(); // 只有当size是一个constexpr函数时才是合法的声明语句

* 字面值类型（literal type）  
常量表达式的值需要在编译时就得到计算，因此对声明constexpr时用到的类型必须有所限制，这类类型称为**字面值类型**  
算术类型，引用和指针都属于**字面值类型**  
指针和引用都能定义成constexpr，但它们的初始值却受到严格限制  
一个constexpr指针的初始值必须是nullptr或者0，或者是存储于某个固定地址中的对象

        constexpr int *p = nullptr; // ok
        int x = 100; // global variable
        constexpr int *p = &x; // ok
        constexpr int &y = x; // ok

在constexpr声明中如果定义了一个指针，限定符constexpr仅对指针本身有效，与指针所指的对象无关

        const int *p = nullptr; // p is a pointer to const int
        constexpr int *q = nullptr; // q is a const pointer to int

*** 
##### Difference between `constexpr` and `const`
> * **Basic meaning and syntax**

> Both keywords can be used in the declaration of objects as well as functions. The basic difference when applied to objects is this:

> - `const` declares an object as constant. This implies a guarantee that, once initialized, the value of that object won't change, and the compiler can make use of this fact for optimizations. It also helps prevent the programmer from writing code that modifies objects that were not meant to be modified after initialization.

> - `constexpr` declares an object as fit for use in what the Standard calls **constant expressions**.  
But note that `constexpr` is not the only way to do this.

> When applied to functions the basic difference is this:

> - `const` can only be used for non-static member functions, not functions in general. It gives a guarantee that the member function does not modify any of the non-static data members.

> - `constexpr` can be used with both member and non-member functions, as well as constructors. It declares the function fit for use in constant expressions. The compiler will only accept it if the function meets certain criteria (7.1.5/3,4), most importantly (†):

>  + The function body must be non-virtual and extremely simple: Apart from typedefs and static asserts, only a single return statement is allowed. In the case of a constructor, only an initialization list, typedefs and static assert are allowed. (= default and = delete are allowed, too, though.)

>  + The arguments and the return type must be literal types (i.e., generally speaking, very simple types, typically scalars or aggregates)

> * **Constant expressions**

> As said above, `constexpr` declares both objects as well as functions as fit for use in constant expressions. A constant expression is more than merely constant:

> - It can be used in places that require compile-time evaluation, for example, template parameters and array-size specifiers:

> ```
> template<int N>  
class fixed_size_list  
{ /*...*/ };
> ```

> ```fixed_size_list<X> mylist;  // X must be an integer constant expression```

> ```int numbers[X];  // X must be an integer constant expression```

> But note:

> - Declaring something as constexpr does not necessarily guarantee that it will be evaluated at compile time. It can be used for such, but it can be used in other places that are evaluated at run-time, as well.

> - An object may be fit for use in constant expressions without being declared constexpr. Example:

> ```
int main()  
{
  const int N = 3;
  int numbers[N] = {1, 2, 3};  // N is constant expression
  return 0;
}
> ```

> This is possible because N, being constant and initialized at declaration time with a literal, satisfies the criteria for a constant expression, even if it isn't declared constexpr.

> * So when do I actually have to use `constexpr`?

> - An object like N above can be used as constant expression without being declared constexpr. This is true for all objects that are:

>  + const
>  + of integral or enumeration type and
>  + initialized at declaration time with an expression that is itself a constant expression 

> [This is due to §5.19/2: A constant expression must not include a subexpressions that involves "an lvalue-to-rvalue modification unless […] a glvalue of integral or enumeration type […]" Thanks to Richard Smith for correcting my earlier claim that this was true for all literal types.]

> - For a function to be fit for use in constant expressions, it must be explicitly declared constexpr; it is not sufficient for it merely to satisfy the criteria for constant-expression functions. Example:

> ```
> template<int N>
class list
{ };
> ```

> ```
> constexpr int sqr1(int arg)
{ return arg * arg; }
> ```

> ```
> int sqr2(int arg)
{ return arg * arg; }
> ```

> ```
> int main()
{
  const int X = 2;
  list<sqr1(X)> mylist1;  // OK: sqr1 is constexpr
  list<sqr2(X)> mylist2;  // wrong: sqr2 is not constexpr
  return 0;
}
> ```

> * **When can I / should I use both, `const` and `constexpr` together?**

> - A. In object declarations. This is never necessary when both keywords refer to the same object to be declared. constexpr implies const.

> `constexpr const int N = 5;`

> is the same as

> `constexpr int N = 5;`

> However, note that there may be situations when the keywords each refer to different parts of the declaration:

> ```
> static constexpr int N = 3;
int main()
{
  constexpr const int *NP = &N;
  return 0;
}
> ```

> Here, NP is declared as an address constant-expression, i.e. an pointer that is itself a constant expression. (This is possible when the address is generated by applying the address operator to a static/global constant expression.) Here, both constexpr and const are required: constexpr always refers to the expression being declared (here NP), while const refers to int (it declares a pointer-to-const). Removing the const would render the expression illegal (because (a) a pointer to a non-const object cannot be a constant expression, and (b) &N is in-fact a pointer-to-constant).

> - B. In member function declarations. In C++11, constexpr implies const also for member functions. However, this is likely to change in C++14. According to the current drafts, constexpr will imply const only for objects, not for member functions, due to a proposed change to §7.1.5/8. Hence, a member function declared under C++11 as

> `constexpr void f();`

> will have to be declared as

> `constexpr void f() const;`

> under C++14 in order to still be usable as a const function. Best mark your constexpr member functions as const even now so as to avoid having to change a lot of code later on.

> (†) The conditions for acceptable constexpr functions will probably be relaxed for C++14. A proposal by Richard Smith has recently been adopted into the C++14 draft.

***

##### **数组和指针**
- 数组是分配了一块连续的存储区域，数组名可代表整个数组，可以用sizeof取得数组的真实大小；
  指针则只分配了指针大小的内存，并且可以指向某个有效的存储区域。
 
- 数组名非左值，在此层面上，可以理解为常量；
  指针可作为左值。

- 数组名在多数情况下会退化为指向数组第一个元素的指针（右值）。
数组不发生退化的地方有几个场合，
 + 声明时
 + 用作sizeof的操作数时
 + 用作&取址运算的操作数时 
```
int a[3] = {1,2,3};
int b[3];
b = a;       // error: b converted to int* (rvalue): array degration.
int *p = a;  // array degration: a converted to an rvalue of int*
sizeof(a);   // no degration.
&a;          // no degration.
```

- C++中，数组还有其他场合不发生退化：
 + 比如作为引用的initializer；
 + 作为typeid/typeinfo的操作数和模板推导时；
 + 作为decltype的参数时

- 解引用操作符可作用于指针和数组名：
 + 对指针和一维数组解引用，会发生访存操作
 + 对多维数组解引用，只是类型改变。
```
   int x = 100;
   int *p = &x;
   *p = 200;  // x -> 200
   int array[2] = { 10, 20 };
   *array = 30;  // { 30, 20 }
```
```
   int array[3][4];  // *array的类型是 int[4]
```
- 取址操作符可作用于数组名和指针：
 + 对指针取址，取得的是指针所在的地址，即指向指针的指针；
 + 对数组名取址，只是类型改变，得到的还是该数组首元素的地址。
```
    int array[10];   // sizeof(array) = 10*sizeof(int)
    printf(“0x%x\n”, &array);  // -> printf(“0x%x\n”, array);
                               // &array的类型: int (*)[10]
```

- 下标运算符[] 可作用于数组名和指针。

- 当指向同一数组中的两个元素的指针相减时，结果是这两个元素的下标差值。
   标准引入了`ptrdiff_t`类型，定义在stddef.h(cstddef)头文件中，其类型为signed integer，用于表示这种差值。

- 多维数组实际上是数组的数组，首元素的类型就是数组类型。
```
int array[3][4];                leal    -64(%ebp), %eax
int (*p1)[4] = array;           movl    %eax, -16(%ebp)
// array的类型是 三个int[4]数组作为元素的数组 
int (*p2)[4] = &array[2];
leal   -64(%ebp), %eax
addl   $32, %eax
movl   %eax, -12(%ebp)
// array[2]的类型是int[4]
int *p3 = array[2];
leal      -64(%ebp), %eax
addl     $16, %eax
movl    %eax, -8(%ebp)
```
 
- C++11引入了名为begin()和end()的函数，这两个函数定义在iterator头文件
  begin()函数返回指向数组首元素的地址，end()函数返回指向数组尾元素的下一位置的指针

- 标准库类型限定使用的下标必须是无符号类型，而内置数组的下标运算无此要求
```
int *p = &ia[2];
int k = p[-2]; // p[-2]是ia[0]表示的那个元素
```

- 使用数组初始化vector对象
```
int int_arr[] = { 0, 1, 2, 3, 4, 5 };
vector<int> ivec(begin(int_arr), end(int_arr));
vector<int> subVec(int_arr+1, int_arr+4);
```

- C++11, auto和数组类型
```
int ia[10];
auto ia2(ia); // 数组名发生退化，ia2类型是int*，指向ia的首元素
```

- C++11, decltype()和数组类型
``` 
int ia[10];
decltype(ia) ia3; // ia3是数组
```

- 使用范围for语句处理多维数组
```
int ia[3][4];
size_t cnt = 0;
for (auto &row : ia)  // 外层循环必须是reference，否则会退化为指针
    for (auto &col : row)
        col = cnt++;

for (const auto &row : ia) // 外层循环的控制变量必须是reference
    for (auto col : row)
        cout << col << endl;

vector<vector<int>> v; // vector和数组的区别
for (auto row : v)
    for (auto col : row)
        cout << col << endl;
```

#####枚举(Enumerations)

- 枚举成员是常量，必须用常量表达式初始化，枚举成员值可以不唯一（值可以重复）。

```
 constexpr int foo() { return 100; }

 enum E {
 E1 = 99,
 E2 = 99,  // Okay
 E3 = sizeof(int),  // Okay
 E4 = foo() // Okay, foo() is constant expression
 };
```

- 枚举类型的对象的初始化和赋值只能通过其枚举成员或同一枚举类型的其它对象来进行。

- 枚举类型的宽度是implementation-defined的，只要规定了其宽度能容纳下最大的枚举成员值:
 + C中char和integer类型都可以，选择权在于实现；
 + C++中则是int->unsigned int->long->unsigned long，依次选择。

* **typedef**

- 标识符或类型名并没有引入新的类型，而只是现有数据类型的同义词

- 为了隐藏特定类型的实现，强调使用类型的目的；

`typedef unsigned int uint32_t;`

- 简化复杂的类型定义，使其更易理解；
```
// 函数指针数组的不同写法
typedef void (*PF)(int); 
PF pf_array[5]; // 等价于  void (*pf_array[5])(int); 

// 返回函数指针的函数
typedef void (*PF)(int);
PF foo(int); // 等价于void (*foo(int))(int);

// 指向函数指针数组的指针
typedef void (*PF)(int);
PF (*pf_array_p)[5]; // 等价于 void (*(*pf_array_p)[5])(int);
```

- 允许一种类型用于多个目的，同时使得每次使用该类型的目的明确

- 容易犯错的typedef用法
```
typedef char *cstring;
extern const cstring cstr; // cstr的类型：char *const cstr;
```

当链接指示符应用在一个声明上时，所有被它声明的函数都将受到链接指示符的影响
```
extern "C" void f1(void (*pfParm)(int)); // f1 和 pfParm 都具有C链接属性
extern "C" typedef void FC(int);
void f2(FC *pfParm); // f2：C++链接属性；pfParm：C链接属性
```

##### **`auto`类型说明符**

- C++11引入了`auto`类型说明符

- `auto`让编译器通过初始值来推断变量的类型，因此`auto`定义的变量必须有初始值
```
auto item = val1 + val2;`
```
- auto也能在一条语句中声明多个变量
```
auto i = 0, *p = &i;
```
- 复合类型和auto
编译器推断出来的auto类型有时候和初始值的类型并不完全一样

当引用被作为初始值的时候，编译器以引用对象的类型作为auto的类型：
```
int i = 0, &r = i;
auto a = r; // a is int
```

auto一般会忽略掉top-level const，同时low-level const则会保留下来
```
int i = 0;
const int ci = i, &cr = ci;
auto b = ci; // b is int
auto c = cr; // c is int
auto d = &i; // d is a pointer to int
auto e = &ci; // e is a pointer to const int
```

如果希望推断出的auto类型是一个顶层const，需要明确指出：
```
const auto f = ci; // f is const int
```

可以将引用类型设为auto，顶层const不会被忽略，除此之外原来的初始化规则仍然适用，
```
auto &g ＝ ci; // ok, g is a reference to const int
auto &h = 42; // error, int& 不能绑定到整型常量
const auto &j = 42; // ok, const int& 可以绑定到整型常量
```

##### `decltype`类型指示符

- C++11 引入了`decltype`，它的作用是选择并返回操作数的数据类型，
在此过程中，编译器分析表达式并得到它的类型，但并不实际计算表达式的值
```
decltype(f()) sum = x;
```

- decltype处理顶层const和引用的方式与auto不同，如果decltype使用的表达式是一个变量，
则decltype返回该变量的类型（包括顶层const 和 reference 在内）
```
const int ci = 0, &cj = ci;
decltype(ci) x = 0; // x is const int
decltype(cj) y = x; // y is const int&
decltype(cj) z;  // error, z is a reference, must be initialized
```

- 引用从来都是作为其所指对象的同义词出现，只有在decltype处是一个例外

- 如果decltype使用的表达式不是一个变量，则decltype返回表达式结果对应的类型
  **有的表达式返回一个引用类型，因为该表达式的结果对象是左值**

```
int i = 42, *p = &i, &r = i;
decltype(r+0) b; // ok, b is int
decltype(*p) c; // error, 因为*p为左值, c is int& (l-value)
```

- delctype((variable))的结果永远是引用

- decltype(variable)的结果只有当variable本身就是一个引用时才是引用

##### 自定义数据类型

- C++11规定，可以为数据成员提供一个类内初始值（in-class initializer）
创建对象时，类内初始值将用于初始化数据成员，没有初始值的成员将被默认初始化
对类内初始值的限制，或者放在花括号里，或者放在等号右边，不能使用圆括号
```
struct Sales_data
{
    std::string bookNo; // 默认初始化
    unsigned units_sold{0}; // ok
    double revenue = 0.0; // ok
    int x(10); // error
};
```

##### **标量类型（Scalar types）**

- **Arithmetic types** (3.9.1)，
**enumeration types**,
**pointer types**,
**pointer to member types** (3.9.2),
**std::nullptr_t**,
and 
**cv-qualified versions of these types** (3.9.3)
are collectively called 
**scalar types**.

- 标量类型是POD（Plain Old Data）类型

##### C风格字符串 (C-Style Character Strings)

以空字符null (‘\0’ 和 L’\0’)结尾的字符数组。
```
char s[] = {‘a’, ‘b’, ‘c’}; // 不是C风格字符串
char s[] = “abc”; // C风格字符串
```

字符串字面值的类型是const char类型的字符数组

通过(const) char*类型的指针来操纵C风格字符串
```
char *p = “hello”; // 指向只读数据块的指针，字符串可能被编译器放入字符串池。
p[1] = 'x'; // 可通过编译，但是会引起segmentation fault
char s[] = “hello”; // 可写数据块（全局或静态）或者堆栈（局部）
const char *p = “hello”;
const char s[] = “hello”;  // 注意这两种声明的区别
```

size_t是标准库中与机器相关的typedef类型定义，其类型定义在stddef.h中(cstddef)，标准规定其为unsigned integer类型

永远不要忘记字符串结束符 null

使用 `strn..` 函数处理C风格字符串

string类提供了c_str()函数用于返回C风格字符串，返回值类型是const char*
```
string s("hello world");
const char *str = s.c_str();
```

##### **POD(Plain Old Data) type**

> A type that consists of nothing but Plain Old Data.

> A POD type is a C++ type that has an equivalent in C, and that uses the same rules as C uses for initialization, copying, layout, and addressing.

> As an example, the C declaration struct Fred x; does not initialize the members of the Fred variable x. To make this same behavior happen in C++, Fred would need to not have any constructors. Similarly to make the C++ version of copying the same as the C version, the C++ Fred must not have overloaded the assignment operator. To make sure the other rules match, the C++ version must not have virtual functions, base classes, non-static members that are private or protected, or a destructor. It can, however, have static data members, static member functions, and non-static non-virtual member functions.

> The actual definition of a POD type is recursive and gets a little gnarly. Here's a slightly simplified definition of POD: a POD type's non-static data members must be public and can be of any of these types: bool, any numeric type including the various char variants, any enumeration type, any data-pointer type (that is, any type convertible to void*), any pointer-to-function type, or any POD type, including arrays of any of these.

> Note: data-pointers and pointers-to-function are okay, but pointers-to-member are not. Also note that references are not allowed. In addition, a POD type can't have constructors, virtual functions, base classes, or an overloaded assignment operator.
 
> 你可以将 POD 类型看作是一种来自外太空的用绿色保护层包装的数据类型，POD 意为“Plain Old Data”（译者：如果一定要译成中文，那就叫“彻头彻尾的老数据”怎么样！）这就是 POD 类型的含义。其确切定义相当粗糙（参见 C++ ISO 标准），其基本意思是 POD 类型包含与 C 兼容的原始数据。例如，结构和整型是 POD 类型，但带有构造函数或虚拟函数的类则不是。 POD 类型没有虚拟函数，基类，用户定义的构造函数，拷贝构造，赋值操作符或析构函数。

> 为了将 POD 类型概念化，你可以通过拷贝其比特来拷贝它们。

> 此外， POD 类型可以是非初始化的。例如：
> ```
     struct RECT r; // value undefined
     POINT *ppoints = new POINT[100]; // ditto
     String s; // calls ctor ==> not POD
> ```

> 非 POD 类型通常需要初始化，不论是调用缺省的构造函数（编译器提供的）还是自己写的构造函数。
