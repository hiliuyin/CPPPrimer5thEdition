- 类的基本思想是 **数据抽象(data abstraction)**和 **封装(encapsulation)**
- **数据抽象**是一种依赖于接口（interface）和实现（implementation）分离的编程以及设计技术
- **封装** 实现了类的接口和实现的分离，封装后的类隐藏了它的实现细节。
- 类提供了用户自定义自己的数据类型，类常被称为用户定义的类型（User-Defined Type, UDT）。
- 简而言之，类就是定义了一个新的类型和新的作用域。

#### 类的声明和定义
- 类定义形成了一个独立的类域，类域可以嵌套。
```
class A { ... };
class A a1; // ok, C++沿用了C的声明方式
A a2; // ok, C++的声明方式
```
- 只有当类的类体被完整定义时，它才被视为已完全定义，所以一个类不能有其自身类型的数据成员。
- 当类头出现后，即可视为该类已经被声明，因此可以在类体内定义指向类类型的指针或者类引用类型的数据成员。
```
class A {
    A a_; // error
    A* a_; // ok
    A& a_; // ok
};
```

- 类声明（前向声明, forward declaration）引入了一个不完全定义。
 + 不完全定义只可使用于定义指向该类型的指针和引用
 + 或者用于声明（而不是定义）使用该类型作为形参或返回值类型的函数
```
class A; // forward declaration，前向声明
A* pa; // ok
A& a = *pa; // ok
A foo(A a); // ok
A foo(A a) {} // error
```

- 类定义不会引起内存的分配，只有类的实例出现时，才会分配内存

#### 名字查找（name lookup）和类的作用域
- 对于定义在类内部的成员函数来说，解析名字的方式与传统的名字查找规则有所区别
类的定义分两步处理：首先，编译成员的声明, 直到类全部可见后才编译函数体
按照这种两阶段的方式处理类可以简化类代码的组织结构，因为成员函数体直到整个类可见后才会被处理
因此它能使用类中定义的任何名字
这种两阶段的处理方式只适用于成员函数体中使用的名字

声明中使用的名字，包括返回类型或者参数列表中使用的名字，都必须在使用前确保可见
如果某个成员的声明使用了类中尚未出现的名字，则编译器则会在定义该类的外部作用域中继续查找
```
typedef double Money;
string bal;
class Account {
public:
    Money balance() { return bal; } // Money是类外声明的类型别名，bal是类的数据成员
private:
    Money bal; // Money是类外声明的类型别名
}
```
 
- 类型名要特殊处理
一般来说，内层作用域可以重新定义外层作用域中的名字，即使该名字已经在内层作用域中使用过
然而在类中，如果成员使用了外层作用域中的某个名字，而该名字代表一种类型，则类不能在之后重新定义该名字
因此类型名的定义通常出现在类的开始处，这样就能确保所有使用该类型的成员都出现在类型名的定义之后
```
typedef double Money;
class Account {
public:
    Money balance() { return bal; }
private:
    typedef double Money; // error, 不能重新定义Money, 用clang可以通过编译？？？
    using Money = double; // error?  用clang可以通过编译
    Money bal;
...
};
```

- 成员函数中使用的名字按照如下方式解析：
 + 首先在成员函数内查找该名字的声明，只有在该名字出现之前的声明才被考虑
 + 如果在成员函数内没有找到，则在类内继续查找，这时类的所有成员都可以被考虑
 + 如果类内也没有找到该名字的声明，则在成员函数定义之前的作用域内继续查找
```
int height;
class Screen {
public:
    typedef std::string::size_type pos;
    void dummy_fcn(pos height)
    {
        cursor = width * height; // 哪个height? 形参
    }
private:
    pos height = 0, width = 0;
    pos cursor = 0;
}
```
- 尽管类的成员被隐藏了，但我们仍然可以通过加上类名或显式地使用this指针来强制访问成员
```
cursor = width * this->height; // 数据成员
cursor = width * Screen::height; // 数据成员 
```
- 尽管外部作用域的对象被隐藏了，但我们可以使用作用域运算符访问到它
```
cursor = width * ::height; // 外部作用域定义的height
```
- 在外部作用域的名字查找不仅仅是类定义之前的外部作用域，还包括成员函数定义之前的外部作用域中的声明
```
int height;
class Screen {
public:
    typedef std::string::size_type pos;
    void setHeight(pos);
    pos height = 0;
};

Screen::pos verify(Screen::pos); // 在setHeight成员函数定义之前
void Screen::setHeight(pos var) {
    height = verify(var); // ok，verify可以被正常调用
}
```

#### 成员函数（member function）
- 在类内部定义的函数是隐式的inline函数，也可以加上inline关键字做为显式声明。
```
class A
{
    int foo() {}  // inline函数
};
```
- 在类体外定义的函数，可以加上inline关键字显式地声明inline函数。
- 成员函数可以被重载
- 对非static成员函数，编译器隐式地增加了一个this指针参数，用于区分类的不同对象实例。

- 非const成员函数的this指针类型是指向类类型的const指针。
```
A::a(); // this的类型: A* const
```

- const成员函数的this指针类型是指向const类类型的const指针。
```
A::a() const; // this的类型: const A* const
```

- 非常量对象，以及非常量对象的指针和引用可以调用const成员函数和非const成员函数
- 常量对象，以及常量对象的指针和引用只能调用const成员函数

- this指针是被隐式地定义的，但是可以在成员函数中显式地使用this指针。
- 对this指针解引用得到了类对象。
```
class A {
public:
    A& test() { return *this; }
}
A a; // this: &a; *this: a
```

- 每个类的实例共享类成员函数的代码段。
```
class A
{
public:
    A(int value) { m_value = value; }
    void Print1() { printf("hello world"); }
    void Print2() { printf("%d", m_value); } // 相当于printf("%d", this->m_value);
private:
    int m_value;
};

int main()
{
    A *pA = nullptr;
    pA->Print1(); // 非虚函数
    pA->Print2(); // 通过this指针访问对象的数据成员m_value，segmentation fault
    return 0;
}
```

#### 类成员访问
- 关键字public，private 和protected 称为访问限定符（access specifier）
- 公有成员（public）可以在任何地方被访问
- 私有成员（private）可以被类成员函数和类友元访问
- 保护成员（protected）可以被类成员函数和派生类访问，亦可被友元访问
- 使用class和struct定义类的唯一区别就是默认的访问权限, 在缺省情况下，未指定访问限定符，class中的成员为private的，struct中的成员为public的

#### 友元
- 友元机制允许一个类授权其它的函数和类访问其非公有成员
- 友元可以是一个命名空间函数，也可以是类成员函数，或者是一个类
- 友元声明只可出现在类体中，友元不是类的成员也不受访问限定符的约束
- 友元声明仅仅指定了访问的权限，而不是一个通常意义上的函数声明
```
class A
{
    friend int foo(int);  // 友元声明，普通函数
    // B::foo必须在A类之前被声明
    friend int B::foo(int); // 类成员函数
    friend class B; // 类
};

int foo(int); // 函数声明，不是必须声明在友元声明之前
```

- 每个类负责控制自己的友元类或友元函数，友元关系不存在传递性。
- 尽管重载函数的名字相同，但是它们仍然是不同的函数，如果一个类想把一组重载函数声明成它的友元，需要对这组重载函数的每一个分别声明
- 类和非成员函数的声明不是必须在它们的友元声明之前，当一个名字第一次出现在一个友元声明中时，我们隐式地假定该名字在当前作用域中是可见的。
- 友元函数可以定义在函数的内部，但是即使在类的内部定义了该友元函数，我们也必须在类的外部提供相应的声明从而使得函数可见
- 当然有些编译器并不强制执行关于友元的限定规则
```
struct X ｛
   friend void f() {} // 定义在类内部
   ...
   void foo() { f(); } // error, f还没有被声明
};

void X::g() { f(); } // error, f还没有被声明
void f();
void X::h() { f(); } // ok
```
 
#### 类型成员
- 除了数据成员和函数成员外，类还可以自定义某种类型在类中的别名
- 用来定义类型的成员必须先定义后使用
```
class Screen
{
public:
    typedef std::string::size_type pos;
    ...
private:
    pos cursor = 0;
    pos height = 0, width = 0;
};
class Screen
{
public:
    using pos = std::string::size_type; // 等价于typedef
    ...
};
```
- 在类的作用域之外，使用作用域运算符访问类类型成员
```
Screen::pos ht = 10, wd = 10;
Screen::pos Screen::size() const  // 类外定义的成员函数的返回类型需要用作用域运算符显式地指明类
{
    return height * width;
}
```
 
#### 可变数据成员（mutable data member）
- 将数据成员声明为mutable，则允许在const成员函数中修改该数据成员
```
class Screen {
public:
    void some_member() const;
private:
    mutable size_t access_ctr; // 可变数据成员
};
void Screen::some_member() const
{
    ++access_ctr;
}
```
 
#### 类数据成员的初始值
- 当我们提供一个类内初始值时，必须以符号＝或者花括号表示
```
class Window_mgr {
private:
    std::size_t window_cnt = 0; // 类内初始值
    std::vector<Screen> screens{Screen(24, 80, '')}; // 类内初始值
};
```
 
#### const成员函数
- 为了尊重类的实例的常量性，编译器必须区分安全的和不安全的成员函数
- 语义上，将成员函数声明为const，表示函数不会修改类的对象
- const在成员函数的声明和定义处都需要显式声明
```
// a.h
class A
{
    int foo(int) const;
};
// a.cpp
int A::foo(int) const { ... }   
```
- 指向const成员函数的指针类型需要显式地声明const限定符
```
int (A::*)(int) const;
```
- 如何通过成员函数指针调用成员函数
```
using PMF = int (A::*)(int) const;
PMF pmf = &A::foo;
A a;
(a.*pmf)();
```

- const成员函数对应的this指针类型是指向const类对象的const指针
- 将一个成员函数声明为const并不能完全阻止可能做到的修改动作，例如通过指针成员去间接修改指针所指的对象
```
class Text
{
public:
    void foo(const string& s) const;
private:
    char* text_;
};
void Text::foo(const string& s) const
{
    text_ = s.cstr();  // error：编译器可以监测到
    text_[0] = s[0];  // ok：通过指针间接的修改，编译器无法监测到这种行为
}
```

- 类的const对象只能调用类的const成员函数，除了类的构造函数和析构函数
- const限定符修饰类成员函数参与类函数重载决议
- const成员函数所绑定的this指针类型为指向const类对象的const指针
- const成员函数返回值类型如果是类的引用类型的话，只能是const引用
```
class A {
    const A& GetThis() const { return *this; }
    // this指针类型是 const A* const, 解引用后*this的类型是const A, 因此只能绑定到const A&上
    A& GetThis() const { return *this; }
    // error: invalid initialization of reference of type 'A&' from expression of type 'const A'
};
```

#### volatile成员函数
- volatile类实例只能调用volatile类成员函数，构造函数和析构函数除外。
- volatile限定符参与函数重载决议。
- volatile成员函数的this指针类型是指向const类类型对象的volatile指针。

#### 构造函数(Constructor)
- 构造函数是特殊的成员函数，在创建类的对象的时候用于初始化对象
- 构造函数的执行可以看成两个阶段，初始化阶段，然后是执行函数内的语句
- 无论是否在初始化列表中显式初始化，类类型的数据成员总是在初始化阶段被初始化
```
class A
{
public:
    A(const string& s);
private:
    string name_;
};

A(const string& s) : name_(s) {} // 调用string的构造函数初始化name_
A(const string& s) // 调用string的缺省构造函数初始化name_
{  name_ = s; // 赋值运算 }
```

- 构造函数和类名同名，没有返回值，可以有形参
- 构造函数可以被重载
- 构造函数不能声明为const的，当创建类的一个const对象时，直到构造函数完成初始化过程，对象才真正取得其const属性
- 类通过一个特殊的构造函数来控制默认初始化过程，这个函数称之为默认构造函数（default constructor）

- 默认构造函数无需任何实参; 如果一个构造函数为所有参数都提供了默认实参，那它实际上也定义了默认构造函数
```
struct Sales_data
{
    Sales_data() {}
    Sales_data(int i = 100) {}
};
Sales_data s; // error, ambigious
```

#### 合成的默认构造函数（Synthesized Default Constructor）
- 如果我们的类没有显式地定义构造函数，那么编译器会隐式地定义一个默认构造函数。
编译器创建的构造函数称为合成的默认构造函数（synthesized default constructor）
- 它按照如下规则初始化类的数据成员：
 + 如果存在类内初始值，用它来初始化数据成员
 + 否则，默认初始化该数据成员
- 只有当类没有声明任何构造函数时，编译器才会自动地生成默认构造函数
- 定义在块中的内置类型和复合类型（数组和指针）的对象被默认初始化时，它们的值是未定义的
因此如果类包含有内置类型或复合类型的数据成员，则只有当这些成员全都被赋予了类内的初始值时，这个类才适合于使用合成的默认构造函数
- 有的时候编译器不能为某些类合成默认构造函数，例如，如果类中某个数据成员的类型没有默认构造函数，那么编译器将无法初始化该成员
- C++11中，如果我们需要默认的行为，可以通过在参数列表后面加上=default来要求编译器生成构造函数
- 如果 =default 在类内部，则默认构造函数是内联的；如果在类外部，则默认情况下不是内联的
```
struct Sales_data {
    Sales_data() = default;
    Sales_data(const std::string &s) : bookNo(s) {}
};
```
```
struct Sales_data
{
    Sales_data();
};
Sales_data::Sales_data() = default;
```

#### 构造函数初始值列表（constructor initialize list）
- 通常情况下，构造函数使用类内初始值不失为一个好的选择
- 当某个数据成员没有被构造函数初始值列表显式地初始化，它将以与合成默认构造函数相同的方式隐式初始化
```
Sales_data(const std::string &s, unsigned n, double p) :
    bookNo(s), units_sold(n), revenue(p*n) {}
```

- 如果数据成员是const，引用或属于某种没有提供默认构造函数的类类型的时候，必须通过构造函数初始值列表为这些数据成员提供初值
```
class ConstRef {
public:
    ConstRef(int ii);
private:
    int i;
    const int ci;
    int &ri;
};
ConstRef::ConstRef(int ii)
{
    i = ii;
    ci = ii; // error, 未初始化
    ri = i; // error, 未初始化
}

ConstRef::ConstRef(int ii) : ci(ii), ri(i) {} // ok
```
- 构造函数初始值列表只用于初始化数据成员的值，而不限定初始化的具体执行顺序
- 数据成员的初始化顺序与它们在类定义中的出现顺序一致

#### 委托构造函数（Delegating Constructor）
- 一个类的委托构造函数使用它所属类的其它构造函数执行他自己的初始化过程，或者说他把它自己的一些（或者全部）职责委托给了其它构造函数
- 当一个构造函数委托给另一个构造函数时，受委托的构造函数的初始值列表和函数体被依次执行，最后才会执行委托者的函数体
```
class Sales_data {
public:
    Sales_data(std::string s, unsigned cnt, double price) : 
        bookNo(s), units_sold(cnt), revenue(cnt*price) {}

    // delegating constructor
    Sales_data() : Sales_data("", 0, 0) {}
    Sales_data(std::string s) : Sales_data(s, 0, 0) {}
    Sales_data(std::istream &is) : Sales_data() { ... }
};
```
 
#### 隐式的类类型转换
- 可以用单个实参调用的构造函数实际定义了从构造函数的参数类型转换为此类类型的隐式转换机制, 我们称这种构造函数为转换构造函数（converting constructor）
```
class Sales_item {
public:
    // default argument for book is the empty string
    Sales_item(const std::string &book = "") : isbn(book), units_sold(0), revenue(0.0) { }
    Sales_item(std::istream &is);
};
string null_book = "9-999-99999-9";
item.same_isbn(null_book); // ok
item.same_isbn(cin);       // ok
```

- 只允许一步类类型转换，编译器只会自动地执行一步类型转换
- 可以通过将构造函数声明为explicit，来抑制由构造函数定义的隐式转换, explicit关键字只能出现在类内的构造函数声明处
```
class Sales_item {
public:
    // default argument for book is the empty string
    explicit Sales_item(const std::string &book = "") : isbn(book), units_sold(0), revenue(0.0) { }
    explicit Sales_item(std::istream &is);
};
item.same_isbn(null_book); // error: string constructor is explicit
item.same_isbn(cin);       // error: istream constructor is explicit
```
- 通常而言，除非有明显的理由需要定义隐式转换，否则，单个实参可以调用的构造函数应该声明为explicit。
- 当使用 **`explicit`**关键字 声明 构造函数时，构造函数只能以直接初始化的形式使用
```
Sales_data item1(null_book); // ok, 直接初始化
Sales_data item2 = null_book; // error, 不能用于拷贝初始化
```

- 尽管编译器不会将explicit构造函数用于隐式转换过程，但我们可以使用这样的构造函数显式地进行强制转换
```
item.combine(Sales_data(null_book)); // ok
item.combine(static_cast<Sales_data>(cin)); // ok
```
 
#### 标准库中含有显式构造函数的类
- 接受一个单参数const char*的string构造函数不是explicit的
```
string str = "hello world"; // ok, copy initialization
string str("hello world"); // ok, direct initialization
```

- 接受一个容量参数的vector构造函数是explicit的
```
vector<int> v(n); // ok, direct initialization
vector<int> v = n; // error, explicit constructor
```
 
#### 聚合类（Aggregate class）
- 聚合类使得用户可以直接访问其成员，并且具有特殊的初始化语法
- 当一个类满足如下条件时，我们说它是聚合的
 + 所有成员都是public的
 + 没有定义任何构造函数
 + 没有类内初始值
 + 没有基类，也没有virtual函数
- 和初始化数组元素的规则一样，如果初始化列表中的元素个数少于类的数据成员个数，则靠后的成员被值初始化
```
struct Data {
    int ival;
    string s;
};
Data val1 = {0, "Anna"};
```

#### 类的静态成员
- 类的static成员的名字位于类域中
```
int foo(); // 全局域
class A
{
    static int foo(); // 类域
};
```
- 类的static成员可以是public的或private的

- 类的static成员可以通过类的对象，引用或指针访问，也可以用类名加上限定修饰符（::）访问
```
class A
{
    static int foo();
};

A::foo();
A a;
A &aa = a;
A *pa = &a;
a.foo();
aa.foo();
pa->foo();
```
 
- 类的static成员独立于类的所有实例，类的static成员是类的一部分，但不是类对象的组成部分。
```
class A
{
   static int x; // sizeof(A) = 1
};
int A::x = 10;

class B
{
    int x; // sizeof(B) = 4
};
```

- static成员函数没有this指针
- 指向static成员函数的指针和普通指针一样。
```
class A
{
    static int foo();
};
int (*pf)() = &A::foo;
```
 
- static成员函数只能访问静态数据成员，因为static成员函数没有this指针，所以无法区分类的对象。
- static成员函数不能被声明为const和volatile
在语义上，const成员函数不会修改类的对象，类的对象是通过this指针加以区分，而static成员函数并没有this指针。

- static成员函数不能被声明为virtual
- static只可出现在类体内，而不能出现在类体外的函数定义处
```
class A {
    static int foo(int);
};
int A::foo(int) {} // 不需要static
```
 
- static数据成员不是通过构造函数初始化的，而是在类定义的时候初始化
- static数据成员必须在类体外定义：
```
class A {
   static int x_;   // 声明
};
int A::x_ = 100;  // 定义+初始化
```

- ODR（One Definition Rule）要求static数据成员的定义不应该放在头文件中

- static数据成员的类型可以是不完全类型
```
class A {
    static A a; // ok
    A* a; // ok
    A& a; // ok
    A a; // error
};
```
 
- static数据成员可以作为类成员函数的缺省实参。
```
class Screen {
public:
    Screen& clear(char = bkground);
private:
    static const char bkground = '#';
};
```
 
- 通常情况下，类的static成员不应该在类的内部初始化。
- 然而，我们可以为static成员提供const类型的类内初始值，不过要求static成员必须是字面值常量类型的constexpr，初始值必须是常量表达式
```
class Account {
public:
    static double rate() { return interestRate; }
    static void rate(double);
private:
    static const int period = 30; // ok，等价于static constexpr int period = 30;
    double daily_tbl[period]; // ok, period is constant expression
};

struct A
{
     static constexpr double a_ = 1.00; // ok
};
```
- const的static成员在类体内初始化时，仍然需要类体外的定义（无须指定初始化值）。
```
constexpr int Account::period; // ok, definition
```

- 指向类的非static成员的指针
```
class Screen {
public:
    typedef std::string::size_type index;
    char get() const; // char (Screen::*)() const
    char get(index ht, index wd) const;  // char (Screen::*)(index, index) const
private:
    std::string contents;   // 指向Screen类中的std::string类型的指针类型为：std::string Screen::*
    index cursor;
    index height, width;
};
std::string Screen::*ps = &Screen::contents;  // 和普通指针类型相比增加了类类型
```
 
- 引用指向类的非static数据成员的指针需要通过类的对象
```
Screen screen, *pscreen;
screen.*ps;
pscreen->*ps;
```
- 除了普通函数指针要求的参数表类型和返回类型外（const和volatile限定符也需匹配），还需显式地加上对应的类类型
```
char (Screen::*pf)() const = &Screen::get;
```

- 引用指向类的非static成员函数的指针需要通过类的对象
```
Screen screen, *pscreen;
(screen.*pf)();
(pscreen->*pf)();
```
  
