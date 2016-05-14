- 面向对象程序设计（Object-oriented programming）的核心思想是 **数据抽象**，**继承** 和 **动态绑定**
 + 通过**数据抽象**我们可以将类的 接口 和 实现分离;
 + 使用**继承（inheritance）**可以定义类似的类型并对其相似的关系建模;
 + 使用**动态绑定（dynamic binding）**可以在一定程度上忽略相似类型的区别，而以统一的方式使用它们的对象

- OOP的核心思想是多态性（polymorphism）
我们把具有继承关系的多个类型称为多态类型，因为我们能使用这些类型的多种形式而无须在意它们的差异
**引用或指针的静态类型和动态类型可能不一致这一事实** 是 C++语言支持多态性的根本所在

###### 基类
- 基类通常都应该定义一个虚析构函数(virtual destructor)，即使该函数不执行任何操作也是如此
- 在C++语言中，基类必须将它的两种成员函数区分开来，
 + 一种是基类希望其派生类进行覆盖(override)的函数，基类通常将其定义为虚函数(virtual)
 + 另一种是基类希望派生类直接继承而不要改变的函数
- 任何构造函数之外的非静态函数都可以是虚函数
- 如果基类把一个函数定义为虚函数，那么该函数在派生类中隐式的也是虚函数
- 成员函数如果没有被声明为虚函数，则其解析过程发生在编译时而非运行时
- 派生类可以继承定义在基类中的成员，但是派生类的成员函数不一定有权访问从基类继承而来的成员
- 派生类能访问继承而来的公有成员，而不能访问继承而来的私有成员
- 如果基类希望它的派生类有权访问某一类成员，同时禁止其他用户访问，则可以用`protected`访问说明符说明这样的成员

###### 派生类
- 派生类必须将其继承而来的成员函数需要覆盖的那些重新声明
- 派生类经常但不总是覆盖继承的虚函数，如果派生类没有覆盖(override)基类中的某个虚函数，则派生类会直接继承其在基类中的版本
- 一个派生类对象包含多个组成部分:
 + 一个含有派生类自己定义的（非静态）成员的子对象，
 + 以及一个与该派生类继承的基类对应的子对象
C++标准并没有明确规定派生类的对象在内存中如何分布，在一个对象中，继承自基类的部分和派生类自定义的部分不一定是连续存储的

- 派生类到基类（derived-to-base）类型转换
因为派生类对象中含有与其基类对应的组成部分，
所以我们能把派生类对象当成基类对象来使用，而且也能将基类的指针或引用绑定到派生类对象中的基类部分
```
class Derived : public Base {};
Base b;
Derived d;
Base *pb1 = &b;
Base *pb2 = &d;
Base &rb = d;
```

- 编译器会隐式地执行派生类到基类的转换
这种隐式特性意味着我们可以把派生类对象或者派生类对象的引用用在需要基类引用的地方
同样也可以把派生类对象的指针用在需要基类指针的地方

- 在派生类对象中含有其与基类对应的组成部分，这一事实是继承的关键所在

- 派生类构造函数
每个类控制它自己的成员初始化过程，派生类也必须使用基类的构造函数来初始化它的基类部分
除非我们显式地指出，否则派生类对象的基类部分会像数据成员一样执行 **默认初始化(default initialization)**
如果想使用其它的基类构造函数，则需要以类名加圆括号内的实参列表的形式为构造函数提供初始值
```
class Base
{
public:
    Base() : pi(new int(123)) {}
    virtual ~Base() { delete pi; }
private:
    int *pi;
};
class Derived : public Base
{
public:
    Derived() : /* Base(), */ x(1024) {}
private:
    int x;
};
Derived d; // d的Base部分默认初始化，即执行Base的default ctr
```

- 派生类可以访问基类的公有成员和受保护成员
- 派生类的作用域嵌套在基类的作用域之内
- 如果想将某个类用做基类，则该类必须已经定义而非仅仅声明(基类必须是complete type)
- 一个类不能派生自它自身
```
class Base;
class Derived : Base { ... }; // error, Base class has incomplete type
```
- C++11引入了关键字`final`用于防止继承的发生
```
class NoDerived final {};
class Base {};
class Last final : Base {};
class Bad : NoDerived {}; // error
class Bad2 : Last {}; // error
```
 
###### 静态成员
- 如果基类定义了一个静态成员，则在整个继承体系中只存在该成员的**唯一**定义
- 静态成员遵循通用的访问规则，如果基类中的成员是private的，则派生类无权访问

###### 类型转换和继承
- 理解基类和派生类之间的类型转换是理解C++语言面向对象编程的关键所在
- 通常情况下，如果我们想把引用或指针绑定到一个对象上，
  则引用或指针的类型应与对象的类型一致，或者对象的类型含有一个可接受的const类型转换规则
- 存在继承关系的类是一个重要的例外：我们可以将基类的指针或引用绑定到派生类对象上
- 和内置指针一样，智能指针类也支持派生类向基类的类型转换，这意味着我们可以将一个派生类对象的指针存储在一个基类的智能指针内

###### 静态类型（static type）和动态类型（dynamic type）
- 静态类型是编译时已知的，动态类型是运行时才可知
- 基类的指针或引用的静态类型可能与其动态类型不一致
- 如果表达式既不是引用也不是指针，则它的动态类型永远与静态类型一致

- 不存在从基类向派生类的隐式类型转换
因为一个基类的对象可能是派生类对象的一部分，也可能不是，所以不存在从基类向派生类的自动类型转换
```
Base b;
Derived *d = &b; // error
Derived &d = b; // error
```

- 即使一个基类指针或引用绑定在一个派生类对象上，我们也不能执行从基类向派生类的转换
```
Derived d;
Base *pb = &d;
Derived *pd = pb; // error
```

- 编译器在编译时无法确定某个特定的转换在运行时是否安全，这是因为编译器只能通过检查指针或引用的静态类型来推断该转换是否合法
- 如果在基类中含有一个或多个虚函数，我们可以使用dynamic_cast请求一个类型转换，该转换的安全检查将在运行时执行
- 如果我们已知某个基类向派生类的转换是安全的，则我们可以使用static_cast来强制覆盖掉编译器的检查工作

- 在对象之间不存在类型转换
派生类向基类的自动类型转换只对指针或引用类型有效，在派生类类型和基类类型之间不存在这样的转换
当我们用一个派生类对象为一个基类对象初始化或赋值时，只有该派生类对象中的基类部分会被拷贝，移动或赋值，它的派生类部分被忽略掉
```
Derived d;
Base b(d); // 派生类部分被切割（sliced down）
b = d; // 派生类部分被切割（sliced down）
```

###### 虚函数
- 通常情况下，如果我们不使用某个函数，则无需为该函数提供定义
但是我们必须为每一个虚函数提供定义，而不管它是否被用到了，这是因为编译器也无法确定到底会使用哪个函数
对虚函数的调用可能在运行时才被解析
```
class Base
{
public:
    virtual ~Base() = default;
    virtual void foo1(int*); // linker error, no definition
};
Base b;
```

- 派生类中的虚函数
如果一个派生类的函数覆盖了继承而来的虚函数，则它的形参类型必须与它覆盖的基类虚函数完全一致
同样，派生类中的虚函数的返回类型也必须与基类函数匹配
**该规则存在一个例外，当类的虚函数返回类型是类本身的指针或引用时，上述规则无效**
也就是说，如果D由B派生而来，则基类的虚函数可以返回B*而派生类的对应函数可以返回D*，
只不过这样的返回类型要求从D到B的类型转换是可访问的
```
class Base
{
public:
    virtual ~Base() = default;
    virtual Base* getObject() { return this; }
    virtual Base& getRef() { return *this; }
};
class Derived : public Base
{
public:
    Derived* getObject() { return this; }
    Derived& getRef() { return *this; }
};
Base *b = new Derived();
b->getObject();
    
Base& rb = *b;
rb.getRef();
```

###### `final`和`override`说明符
派生类如果定义了一个函数与基类中虚函数的名字相同但是形参列表不同，这仍然是合法的行为，
编译器认为新定义的函数与基类中原有的函数是相互独立的，派生类的函数并没有覆盖掉基类中的版本，
但是这种情况经常意味着发生了错误
- C++11引入了override关键字来说明派生类中的虚函数(只有虚函数才能被标示为override)，
这样使得程序员的意图更加清晰的同时让编译器可以为我们发现一些错误
如果用override标记了某个虚函数，但该函数没有覆盖已存在的虚函数，此时编译器将报错
```
struct B
{
    virtual void f1(int) const;
    virtual void f2();
    void f3();
};
struct D1 : B
{
    void f1(int) const override;
    void f1(int) override; // error, 没有覆盖基类的虚函数
    void f3() override; // error, 不是虚函数
    void f4() override; // error, 新声明的函数
};
```
- 如果我们将某个虚函数标记为`final`(只有虚函数能被标示为final)，则之后任何尝试覆盖该函数的操作都将引发编译器报错
```
struct D2 : B {
    void f1(int) const final;
};
struct D3 : D2 {
    void f2();
    void f1(int) const; // error
};
```
 
###### 虚函数和默认实参
- 如果某次函数调用使用了默认实参，则该实参值由本次调用的**静态类型**决定
- 因此如果虚函数使用默认实参，则基类和派生类中定义的默认实参最好一致

###### 回避虚函数的机制
- 可以使用作用域运算符来实现回避虚函数
```
double undiscounted = baseP->Quote::net_price(42); // 强制调用基类定义的版本
```
- 如果一个派生类虚函数需要调用它的基类版本，但是没有使用作用域运算符，
则在运行时该调用会被解析为对派生类版本自身的调用，从而导致无限递归

###### 抽象基类（abstract base class）
- 纯虚函数（pure virtual），在类的内部声明虚函数时，在分号之前使用了`=0`
一个纯虚函数不需要（但是可以）被定义，不过函数体必须定义在类的外部
- 含有纯虚函数（或者没有覆盖直接继承）的类是抽象基类
- 抽象基类负责定义接口，而后续的类可以覆盖该接口
- 我们不能创建抽象基类的对象

###### 访问控制和继承
- 每个类分别控制着其成员对于派生类是否可访问（accessible）
- 受保护的成员(protected)
 + 和私有成员类似，受保护的成员对于类的用户来说是不可访问的
 + 和公有成员类似，受保护的成员对于派生类的成员和友元来说是可访问的
 + 派生类的成员和友元只能通过派生类对象来访问基类的受保护成员，派生类对于一个基类对象中的受保护成员没有任何访问特权
```
class Base {
protected:
    int prot_mem;
};
class Sneaky : public Base {
    friend void clobber(Sneaky &s) { s.j = s.prot_mem = 0; } // ok
    friend void clobber(Base &b) { b.prot_mem = 0; } // error
    void foo(Base &b)
    {
        b.prot_mem++; // error
        prot_mem++; // ok
    }
    int j;
};
```
- 公有，私有和受保护继承
某个类对其继承而来的成员的访问权限受两个因素影响：一是基类中该成员的访问说明符，二是派生类的派生列表中的访问说明符
- 派生访问说明符对于派生类的成员（及友元）能否访问其直接基类的成员没什么影响，对基类成员的访问权限只与基类中的访问说明符有关
- 派生访问说明符的目的是控制派生类用户（包括派生类的派生类在内）对于基类成员的访问权限
- 派生访问说明符还可以控制继承自派生类的新类的访问权限
```
class Base {
public:
    void pub_mem();
protected:
    int prot_mem;
private:
    char priv_mem;
};
struct Pub_Derv : public Base {
    int f() { return prot_mem; } // ok
    char g() { return priv_mem; } // error
};
struct Prot_Derv : protected Base {
    int foo() { return prot_mem; } // ok
};
struct Priv_Derv : private Base {
    int f1() const { return prot_mem; } // ok, private不影响派生类的访问权限
};
struct Derived_from_Public : public Pub_Derv {
    int use_base() { return prot_mem; } // ok
};
struct Derived_from_Private : public Priv_Derv {
    int use_base() { return prot_mem; } // error
};
Pub_Derv d1;
Priv_Derv d2;
Prot_Derv d3;
d1.pub_mem(); // ok
d2.pub_mem(); // error
d3.pub_mem(); // error
```

- 派生类向基类转换的可访问性
派生类向基类的转换是否可访问由该转换的代码决定，同时派生类的派生访问说明符也会有影响

只有当D公有地继承B时，用户代码才能使用派生类向基类的转换；如果D继承B的方式是受保护的或私有的，则用户代码不能使用该转换
不论D以什么方式继承B，D的成员函数和友元都能使用派生类向基类的转换
如果D继承B的方式是公有的或受保护的，则D的派生类的成员和友元可以使用D向B的类型转换；如果D继承B的方式是私有的，则不能使用
 
- 友元和继承
友元关系不能传递，也不能继承
每个类负责控制各自成员的访问权限，对基类的访问权限由基类本身控制，即使对于派生类的基类部分也是如此
```
class Base {
    friend class Pal;
};
class Pal {
public:
    int f(Base b) { return b.prot_mem; }
    int f2(Sneaky s) { return s.j; } // error, Pal不是Sneaky的友元
    int f3(Sneaky s) { return s.prot_mem; } // ok, Pal是Base的友元，对派生类的基类部分的访问权限由基类控制
};
```

- 改变个别成员的可访问性
如果需要改变派生类继承的某个名字的访问级别，可以使用using声明
派生类只能为那些它可以访问的名字提供using声明
```
class Base {
public:
    std::size_t size() const { return n; }
protected:
    std::size_t n;
private:
    int x;
};

class Derived : private Base {
public:
    using Base::size; // ok, size()在Base是public的
protected:
    using Base::n; // ok, n在Base是protected的
    using Base::x; // error, x在Base中是private的
};
Derived d;
d.size(); // ok
```

- 默认的继承保护级别
 + struct默认public继承；class默认private继承

- 继承中的类作用域
 + 派生类的作用域嵌套在其基类的作用域之内

- 在编译时进行名字查找
一个对象，指针或引用的静态类型决定了该对象的哪些成员是可见的，
即使静态类型和动态类型不一致，我们能使用哪些成员依旧是由静态类型决定的

- 派生类的成员将隐藏同名的基类成员
派生类也能重用定义在其直接基类或间接基类中的名字，此时定义在内层作用域（即派生类）的名字将隐藏定义在外层作用域（即基类）的名字
可以通过作用域运算符来使用隐藏的成员

###### 名字查找和继承
- 理解函数调用的解析过程对于理解C++的继承至关重要，假定我们调用p->mem()或obj.mem()，则依次执行以下四个步骤：
 + 首先确定p或obj的静态类型，因为我们调用的是一个成员，所以该类型必然是类类型
 + 在p或obj的静态类型对应的类中查找mem，如果找不到，则依次在直接基类中不断查找直至到达继承链的顶端，如果仍然找不到，则编译器报错
 + 一旦找到了mem，就进行常规的类型检查以确认调用是否合法
 + 假设调用合法，则编译器根据调用的是否是虚函数而产生不同的代码，
如果是虚函数而且我们通过引用或指针进行调用，则编译器生成的代码将在运行时确定到底运行该函数的哪个版本，依据的是对象的动态类型；
如果不是虚函数或者通过对象进行的调用，则编译器产生一个常规的函数调用
```
struct Base {
    int memfcn();
};
struct Derived : Base {
    int memfcn(int); // 隐藏基类的memfcn
};
Derived d;
Base b;
b.memfcn();
d.memfcn(10);
d.memfcn(); // error
d.Base::memfcn(); // ok
```

- 假如基类与派生类的虚函数接受的实参不同，则我们无法通过基类的引用或指针调用派生类的虚函数了
```
class Base {
public:
    virtual int fcn();
};

class D1 : public Base {
public:
    int fcn(int); // 隐藏了基类的fcn，这个fcn不是虚函数，D1继承了Base::fcn()的定义
    virtual void f2();
};

class D2 : public D1 {
public:
    int fcn(int); // 隐藏了D1::fcn(int)
    int fcn(); // 覆盖了Base::fcn()的虚函数
    void f2(); // 覆盖了D1::f2()的虚函数
};

Base bobj;
D1 d1obj;
D2 d2obj;

Base *bp1 = &bobj;
Base *bp2 = &d1obj;
Base *bp3 = &d2obj;
bp1->fcn(); // Base::fcn(), bp1的静态类型和动态类型都是Base
bp2->fcn(); // Base::fcn(), bp2的静态类型是Base, 动态类型是D1, 但是D1继承了Base::fcn()的定义
bp3->fcn(); // D2::fcn(), bp3的静态类型是Base, 动态类型是D2, 动态绑定到D2::fcn()

D1 *d1p = &d1obj;
D2 *d2p = &d2obj;
bp2->f2(); // error, bp2的静态类型是Base, 编译器根据静态类型去查找是否有匹配的函数, Base中未定义f2()
d1p->f2(); // D1::f2(), d1p的静态类型和动态类型都是D1
d2p->f2(); // D2::f2(), d2p的静态类型和动态类型都是D2

Base *p1 = &d2obj;
D1 *p2 = &d2obj;
D2 *p3 = &d2obj;
p1->fcn(42); // error, p1的静态类型是Base, 编译器根据静态类型去查找是否有匹配的函数, Base::fcn()参数不匹配
p2->fcn(42); // 静态绑定，D1::fcn(int)
p3->fcn(42); // 静态绑定，D2::fcn(int)
```

- 覆盖重载的函数
成员函数无论是否是虚函数都能被重载，派生类可以覆盖重载函数的0个或多个实例
如果派生类希望所有的重载版本对于它来说都是可见的，那么他就需要覆盖所有的版本，或者一个也不覆盖

- 使用using声明语句可以把基类中成员函数的所有重载实例添加到派生类作用域中，此时派生类只需要定义其特有的函数就可以了，
而无需为继承而来的其他函数重新定义

##### 构造函数和拷贝控制
- 虚析构函数: 如果基类的析构函数不是虚函数，则delete一个指向派生类对象的基类指针将产生未定义的行为
> Virtual destructors are useful when you can delete an instance of a derived class through a pointer to base class:
```
class Base
{
    // some virtual methods
};
class Derived : public Base
{
    ~Derived()
    {
        // Do some important cleanup
    }
}
```
> Here, you'll notice that I didn't declare Base's destructor to be virtual.
Now, let's have a look at the following snippet:
```
Base *b = new Derived();
// use b
delete b; // Here's the problem!
```
> Since Base's destructor is not virtual and b is a Base* pointing to a Derived object, delete b has undefined behaviour. In most implementations, the call to the destructor will be resolved like any non-virtual code, meaning that the destructor of the base class will be called but not the one of the derived class, resulting in resources leak.

> To sum up, always make base classes' destructors virtual when they're meant to be manipulated polymorphically.

> If you want to prevent the deletion of an instance through a base class pointer, you can make the base class destuctor protected and nonvirtual; by doing so, the compiler won't let you call delete on a base class pointer.

- 虚析构函数将阻止合成移动操作

###### 合成拷贝控制和继承
基类或派生类的合成拷贝控制成员的行为和其他合成的构造函数，赋值运算符或析构函数类似
它们对类本身的成员依次进行初始化，赋值或销毁的操作，
此外，这些合成的成员还负责使用直接基类中对应的操作对一个对象的直接基类部分进行初始化，赋值或销毁的操作

无论基类成员是合成的版本还是自定义的版本都没有太大影响，唯一的要求就是相应的成员应该可访问并且不是一个被删除的函数

派生类中删除的拷贝控制与基类的关系
基类或派生类出于同样的原因将其合成的默认构造函数或者任何一个拷贝控制成员定义成被删除的函数

- 移动操作和继承
因为基类缺少移动操作会阻止派生类拥有自己的合成移动操作，所以我们确实需要移动操作时应该首先在基类中进行定义
```
class Quote {
public:
    Quote() = default;
    Quote(const Quote&) = default;
    Quote(Quote&&) = default;
    Quote& operator=(const Quote&) = default;
    Quote& operator=(Quote&&) = default;
    virtual ~Quote() = default;
}; 
```
###### 派生类的拷贝控制成员
- 当派生类定义了拷贝或移动操作时，该操作负责拷贝或移动包括基类部分成员在内的整个对象
- 在默认情况下，基类默认构造函数初始化派生类对象的基类部分
- 如果我们想拷贝（或移动）基类部分，则必须在派生类的构造函数初始值列表中显式地使用基类的拷贝（或移动）构造函数
```
class Base {};
class D : public Base {
public:
    D(const D& d) : Base(d) ... {}
    D(D &&d) : Base(std::move(d)) ... {}
};
```
- 同样，派生类的复制运算符也必须显式地为其基类部分赋值
```
D& operator=(const D &rhs) {
    Base::operator=(rhs);
    return *this;
}
```

- 派生类析构函数只负责销毁由派生类自己分配的资源，基类部分是隐式销毁的; 对象销毁的顺序与其创建的顺序相反

- 如果在构造函数或析构函数中调用了某个虚函数，则我们应该执行与构造函数或析构函数所属类型相对应的虚函数版本
