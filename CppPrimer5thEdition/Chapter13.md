##### 拷贝构造函数（Copy Constructor）
- 如果一个构造函数的第一个参数是自身类类型的引用，且任何额外参数都有默认值，则此构造函数是拷贝构造函数
- 虽然可以定义一个非const引用的拷贝构造函数，但是此参数几乎总是一个const的引用
- 拷贝构造函数在很多情况下都会被隐式调用，因此拷贝构造函数通常不应该是explicit的

- 拷贝构造函数的第一个参数类型必须是引用类型
  如果不是引用类型则调用永远不会成功，为了调用拷贝构造函数，我们必须拷贝它的实参
  为了拷贝实参，我们又必须调用拷贝构造函数，由此循环下去

##### 合成拷贝构造函数（synthesized copy constructor）
- 如果编译器没有为类定义一个拷贝构造函数，编译器会为我们定义一个拷贝构造函数, 
编译器从给定对象中依次将每个非static成员拷贝到正在创建的对象中
 + 对于类类型的成员，会调用其拷贝构造函数来拷贝
 + 对于内置类型的成员则直接拷贝
 + 对于数组类型，则逐个元素的拷贝，如果数组元素是类类型，则用元素类型的拷贝构造函数进行拷贝

##### 拷贝初始化（copy initialization）
- 当使用直接初始化时，编译器使用普通的函数匹配来选择参数最匹配的构造函数
- 当使用拷贝初始化时，编译器将右侧运算对象拷贝到正在创建的对象中，如果需要的话还会进行隐式类型转换
```
string dots(10, '.'); // 直接初始化
string s(dots); // 直接初始化

string s2 = dots; // 拷贝初始化
string null_book = "9-999-99999-9"; // 拷贝初始化
string nines = string(100, '9'); // 拷贝初始化
```

- 拷贝初始化在如下情况下发生
 + 使用 ＝ 定义变量时
 + 将一个对象作为实参传递给一个非引用类型的形参
 + 将一个返回类型为非引用类型的函数返回一个对象
 + 用花括号列表初始化一个数组中的元素或一个聚合类中的成员
 + 某些类类型还会对它们所分配的对象使用拷贝初始化，例如初始化标准库容器或是调用insert或push成员，与之相反，用emplace则相反 

- 拷贝初始化的限制
如果我们使用的初始化值需要调用一个explicit的构造函数来进行类型转换, 那么只能使用直接初始化或者显式地调用 explicit构造函数
```
vector<int> v1(10); // 直接初始化
vector<int> v2 = 10; // error, 接受大小参数的构造函数是explicit的
void f(vector<int>);
f(10); // error
f(vector<int>(10)); // ok, 显式地调用构造函数

class A
{
public:
    A() {}
    explicit A(const A&) {}
};

A a1; // ok, default constructor
A a2 = a1; // error, direct initialization, copy constructor is explicit
A a3(a1); // ok, explicitly called copy constructor
```
 
- 编译器可以绕过拷贝构造函数进行优化
在拷贝初始化过程中，编译器可以（但不是必须）跳过拷贝/移动构造函数，直接创建对象
但是即使编译器略过了拷贝/移动构造函数，拷贝/移动构造函数必须是存在而且可以访问的（不能是private的）
```
string null_book = "9-999-99999-9"; // 1)
string null_book("9-999-99999-9"); // 2) 编译器可能将1)转换为，直接略过拷贝/移动构造函数
```

##### 拷贝赋值运算符（Copy Assignment Operator）
- 如果类未定义自己的拷贝赋值运算符，编译器会为它合成一个
- 拷贝赋值运算符接受一个与其所在类相同类型的参数
```
class Foo {
public:
    Foo& operator=(const Foo&);
};
```
 
- 为了和内置类型的赋值保持一致，赋值运算符通常返回一个指向其左侧运算对象的引用

- 通常标准库要求保存在容器中的类型要有赋值运算符，且其返回值是左侧运算对象的引用

- 合成拷贝赋值运算符（synthesized copy-assignment operator）
 + 将右侧运算对象的每个非static成员赋予左侧运算对象的对应成员, 这一工作是通过成员类型的拷贝赋值运算符来完成的
 + 对于数组类型的成员，逐个赋值数组元素

##### 析构函数（Destructor）
- 析构函数执行与构造函数相反的操作
 + 构造函数初始化对象的非static数据成员以及一些其他工作
 + 析构函数释放对象使用的资源，并销毁对象的非static数据成员

- 析构函数没有返回值，也没有参数，不能被重载
- 在析构函数中，首先执行函数体，然后销毁成员, 成员按照初始化顺序的逆序销毁
- 析构函数的析构部分是隐式的，成员销毁时发生什么完全依赖于成员的类型
 + 销毁类类型的成员需要调用成员自己的析构函数
 + 内置类型没有析构函数，因此销毁内置类型成员什么也不需要做

- 无论何时一个对象被销毁，就会自动调用其析构函数
 + 变量在离开其作用域时被销毁
 + 当一个对象被销毁时，其成员被销毁
 + 容器（无论是标准库容器还是数组）被销毁时，其元素被销毁
 + 对于动态分配的对象，当对指向它的指针应用delete运算符时被销毁
 + 对于临时对象，当创建它的表达式结束时被销毁
 
- 合成析构函数（synthesized destructor）
 + 当一个类未定义自己的析构函数时，编译器会为它定义一个合成析构函数

##### 三/五法则
- 如果一个类需要自定义析构函数，几乎可以肯定它也需要自定义拷贝赋值运算符和拷贝构造函数
- 需要拷贝操作的类也需要赋值操作，反之亦然

##### 使用＝default
```
class Sales_data {
public:
    Sales_data() = default; // inline
    Sales_data(const Sales_data&) = default; // inline
    Sales_data& operator=(const Sales_data &);
    ~Sales_data() = default; // inline
};

Sales_data& Sales_data::operator=(const Sales_data &) = default; // 非内联
```
 
##### 阻止拷贝
- 定义删除的函数（=delete）
- C++11允许将拷贝构造函数和拷贝赋值运算符定义为删除的函数（deleted function）
- 删除的函数时这样一种函数，虽然我们声明了它们，但是不能以任何方式使用它们
- 和=default不同，=delete必须出现在第一次声明的地方
```
struct NoCopy {
    NoCopy() = default;
    NoCopy(const NoCopy&) = delete; // 阻止拷贝
    NoCopy& operator=(const NoCopy&) = delete; // 阻止赋值
};
```
 
- 对于析构函数已删除的类型，不能定义该类型的对象或者释放指向该类型动态分配对象的指针
```
struct NoDtor {
    NoDtor() = default;
    ~NoDtor() = delete;
};

NoDtor nd; // error
NoDtor *p = new NoDtor(); // ok
delete p; // error
```

- private拷贝控制
在C++11标准发布之前，类是通过将其拷贝构造函数和拷贝赋值运算符声明为private来阻止拷贝
我们将这些拷贝控制成员声明为private的，并且不定义它们，声明但不定义一个成员函数是合法的
```
class PrivateCopy {
public:
    PrivateCopy() = default;
    ~PrivateCopy();
private:
    PrivateCopy(const PrivateCopy&);
    PrivateCopy& operator=(const PrivateCopy&);
};
```
 
##### 合成的拷贝控制成员可能是删除的
- 本质上，当不可能拷贝，赋值或者销毁类的成员时，类的合成的拷贝控制成员就被定义为删除的
- 对某些类来说，编译器将会把合成的拷贝控制成员定义为删除的函数
 + 如果类的某个成员的析构函数是删除的或不可访问的，则类的合成析构函数被定义为删除的
 + 如果类的某个成员的拷贝构造函数是删除的或不可访问的，则类的合成拷贝构造函数被定义为删除的
 + 如果类的某个成员的析构函数是删除的或不可访问的，则类的合成拷贝构造函数被定义为删除的
 + 如果类的某个成员的拷贝赋值运算符是删除的或不可访问的，或是类有一个const的或引用成员，则类的合成拷贝赋值运算符被定义为删除的
 + 如果类的某个成员的析构函数是删除的或不可访问的，或是类有一个引用成员，它没有类内初始化值，或是类有一个const成员，它没有类内初始化值且其类型未显式地定义默认构造函数，则该类的默认构造函数被定义为删除的

##### 行为像值的类
```
class HasPtr
{
public:
    HasPtr(const string &s = string()) :
        ps(new string(s)), i(0) {}

    HasPtr(const HasPtr &p) :
        ps(new string(*p.ps)), i(p.i) {}

    ~HasPtr() { delete ps; }

    HasPtr& operator=(const HasPtr &rhs) // 异常安全，可以处理自赋值情况
    {
        auto newp = new string(*rhs.ps);
        delete ps;
        ps = newp;
        i = rhs.i;
        return *this;
    }

private:
    std::string *ps;
    int i;
};
```

- 该类的另一种实现，使用了copy & swap
```
class HasPtr
{
    friend void swap(HasPtr&, HasPtr&);
    
public:
    HasPtr(const std::string& s = std::string())
    : ps(new std::string(s)), i(0)
    {}
    
    HasPtr(const HasPtr &p)
    : ps(new std::string(*p.ps)), i(p.i)
    {}
    
    ~HasPtr()
    {
        delete ps;
    }
    /*
    HasPtr& operator=(const HasPtr &rhs)
    {
        auto newp = new std::string(*rhs.ps);
        delete ps;
        ps = newp;
        i = rhs.i;
        return *this;
    }
    */
    
    HasPtr& operator=(HasPtr rhs)
    {
        swap(*this, rhs);
        return *this;
    }
    
    
private:
    std::string *ps;
    int i;
};

void swap(HasPtr &lhs, HasPtr &rhs)
{
    using std::swap;
    swap(lhs.ps, rhs.ps);
    swap(lhs.i, rhs.i);
}
```

##### 行为像指针的类
```
class HasPtr
{
public:
    HasPtr(const string &s = string()) :
        ps(new string(s)), i(0), use(new size_t(1)) {}

    HasPtr(const HasPtr &p) :
        ps(p.ps), i(p.i), use(p.use) { ++*use; }

    HasPtr& operator=(const HasPtr &rhs)
    {
        ++*rhs.use;
        if (--*use == 0)
        {
            delete ps;
            delete use;
        }
        ps = rhs.ps;
        i = rhs.i;
        use = rhs.use;

        return *this;
    }

    ~HasPtr()
    {
        if (--*use == 0)
        {
            delete ps;
            delete use;
        }
    }

private:
    string *ps;
    int i;
    size_t *use;
};
```

##### 对象移动
- C++11一个最主要的特性是可以移动而非拷贝对象的能力。
- 在某些情况下，对象拷贝后就立即被销毁了。在这些情况下，移动而非拷贝对象会大幅提升性能。

###### 右值引用
- 必须绑定到右值的引用，通过&&来获得右值引用
- 右值引用只能绑定到一个将要销毁的对象，因此我们可以自由的将一个右值引用的资源移动到另一个对象中
- 右值引用不能绑定到一个左值上
```
int i = 42;
int &r = i; // l-value reference
int &&rr = i; // error
int &&rr = 42; // ok, 绑定到literal 
int &&rr2 = i*42; // ok, 绑定到右值
const int &r2 = 42; // ok, const的左值引用
```
- 左值持久，右值短暂
- 变量是左值，因此不能将一个右值引用绑定到一个变量上，即使这个变量是右值引用类型也不行
```
int &&rr1 = 42;
int &&rr2 = rr1; // error
``` 
- 可以显式地将一个左值转换为对应的右值引用类型
- 可以通过std::move()函数获得绑定到左值上的右值引用
```
#include <utility>
int &&rr3 = std::move(rr1); // ok
```
- 调用std::move()意味着承诺：我们可以销毁一个移后源对象，也可以赋予它新值，但不能使用一个移后源对象的值

###### 移动构造函数（move constructor）
- 移动构造函数的第一个参数是该类类型的一个右值引用，其它任何额外的参数都有默认实参
- 除了完成资源移动，移动构造函数还必须确保移后源对象处于一个状态，即销毁它是无害的
- 一旦资源完成移动，源对象必须不在指向被移动的资源，这些资源的所有权已经归属新创建的对象
```
StrVec::StrVec(StrVec &&s) noexcept : // 移动操作不应抛出异常
    elements(s.elements), first_free(s.first_free), cap(s.cap)
{
    // 另s进入这样的状态，即对它析构是安全的
    s.elements = s.first_free = s.cap = nullptr;
}
```

- 不抛出异常的移动构造函数和移动赋值运算符必须显式标记为`noexcept`
- 虽然移动操作通常不抛出异常，但是抛出异常也是允许的，
  同时，标准库容器对异常发生时其自身的行为提供保障:
  例如，vector保证，如果我们调用push_back()时发生异常，vector自身不会发生改变，
  因此除非vector知道元素类型的移动构造函数不会抛出异常，
  否则在重新分配内存的过程中，它就必须使用拷贝构造函数而不是移动构造函数
 
##### 移动赋值运算符（move-assignment operator）
```
StrVec& StrVec::operator=(StrVec &&rhs) noexcept
{
    // 直接检测自赋值，此右值rhs可能是move调用返回的结果
    if (this != &rhs)
    {
        free();
        elements = rhs.elements;
        first_free = rhs.first_free;
        cap = rhs.cap;
        rhs.elements = rhs.first_free = rhs.cap = nullptr;
    }
    return *this;
}
```
- 移后源对象必须可析构
  在移动操作之后，移后源对象必须保持有效的，可析构的状态，但是用户不能对其值进行任何假设

###### 合成的移动操作
- 只有当一个类没有定义任何自己版本的拷贝控制成员，且它的所有数据成员都能移动构造或移动赋值时，
编译器才会为它合成移动构造函数或移动赋值运算符
```
struct X {
    int i; // 内置类型可以移动
    std::string s; // string定义了自己的移动操作
};
struct hasX {
    X mem; // X有合成的移动操作
};
X x, x2 = std::move(x); // 使用合成的移动构造函数
hasX hx, hx2 = std::move(hx); // 使用合成的移动构造函数
```

- 定义了一个移动构造函数或移动赋值运算符的类也必须定义自己的拷贝操作，否则，这些成员默认地被定义为删除的
```
class Moveable
{
public:
    Moveable() = default;
    Moveable(Moveable&&) = default;
};
Moveable m1;
Moveable m2(m1); // error, 未定义copy ctr, 编译器隐式定义copy ctr为删除
Moveable m3;
m3 = m2; // error, 未定义copy assignment operator
```

- 与拷贝操作不同，移动操作永远不会隐式地定义为删除的函数，
但是如果我们显式地要求编译器生成`=default`的移动操作, 且编译器不能移动所有成员，
则编译器会将移动操作定义为删除的函数
```
class HugeMem
{
public:
    HugeMem(int size) : sz(size > 0 ? size : 1), c(new int[sz]) {}
    ~HugeMem() { delete[] c; } 
    HugeMem(HugeMem&& hm) = delete;
private:
    int sz;
    int* c;
};
class Moveable
{
public:
    Moveable() : h(1024) {}
    ~Moveable() = default;    
    Moveable(Moveable&& m) = default; // 虽然被定义为default, 但是成员h的move ctr被定义为delete
private:
    HugeMem h;
};
Moveable m1;
Moveable m2(std::move(m1)); // error, move ctr is delete
```

###### 移动右值，拷贝左值
- 如果一个类既有移动构造函数，也有拷贝构造函数，编译器使用普通的函数匹配规则来去定使用哪个构造函数
```
StrVec v1, v2;
v1 = v2; // v2是左值，使用拷贝赋值
StrVec getVec(istream &);
v2 = getVec(cin); // getVec返回的是右值，StrVec&&是精确匹配，使用移动赋值(如果调用拷贝赋值运算符需要进行一次到const的转换)
```
 
- 如果没有移动构造函数，右值也被拷贝
如果一个类有一个可用的拷贝构造函数而没有移动构造函数，则其对象是通过拷贝构造函数来移动的（拷贝赋值运算符和移动赋值运算符的情况类似）
用拷贝构造函数代替移动构造函数几乎肯定是安全的（赋值运算符的情况类似）
```
class Foo {
public:
    Foo() = default;
    Foo(const Foo&);
};
Foo x;
Foo y(x); // 拷贝构造函数，x是左值
Foo z(std::move(x)); // 拷贝构造函数，因为没有定义移动构造函数
```
 
- 拷贝并交换赋值运算符和移动操作
```
class HasPtr {
public:
    HasPtr(HasPtr &&p) noexcept : ps(p.ps), i(p.i)
    {
        p.ps = nullptr;
    }
    HasPtr& operator=(HasPtr rhs)
    {
        swap(*this, rhs);
        return *this;
    }
};
hp = hp2; // hp2是一个左值，rhs由拷贝构造函数初始化
hp = std::move(hp2); // rhs由移动构造函数初始化
```

###### 更新的"三/五法则"
所有的五个拷贝控制成员应该看错一个整体：
一般而言，如果一个类定义了任何一个拷贝操作，它就应该定义所有五个操作。
某些类必须定义拷贝构造函数，拷贝赋值运算符 和 析构函数 才能正确工作。
这些类通常拥有一个资源，而拷贝成员必须拷贝此资源。
一般来说，拷贝一个资源会导致一些额外开销。
在这种拷贝并非必须的情况下，定义了 移动构造函数 和 移动赋值运算符 的类就可以避免此问题。

###### 实现String
```
class String
{
public:
    String() : data_(new char[1]) // default ctr
    {
        data_[0] = '\0';
    }
    
    ~String()
    {
        delete[] data_;
    }
    
    const char* c_str() const
    {
        return data_;
    }
    
    size_t size() const
    {
        return strlen(data_);
    }
    
    String(const String& s) : data_(new char[strlen(s.data_)+1]) // copy ctr
    {
        strcpy(data_, s.data_);
    }
    
    String& operator=(String rhs) // pass-by-value
    {
        swap(rhs); // copy-and-swap idiom
        return *this;
    }
    
    String(String&& s) : data_(s.data_) // move ctr
    {
        s.data_ = nullptr;
    }
    
    String& operator=(String&& rhs) // move assignment operator
    {
        swap(rhs);
        return *this;
    }
    
private:
    void swap(String& rhs)
    {
        using std::swap;
        swap(data_, rhs.data_);
    }
    
    char* data_;
};
```
