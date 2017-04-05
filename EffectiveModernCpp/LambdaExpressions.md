##### 条款31: 避免使用默认的捕捉方式
- C++11中，lambda表达式支持两种捕捉方式：by-value 和 by-reference
```
auto byValue = [=]() {};
auto byRef = [&]() {};
```
- by-reference捕捉方式使用不当，会导致捕捉的引用无效(dangling)
```
using FilterContainer = std::vector<std::function<bool(int)>>;
FilterContainer filters;

void addDivisorFilter()
{
    auto calc1 = computeSomeValue1();
    auto calc2 = computeSomeValue2();
    auto divisor = computeDivisor(calc1, calc2);
    filters.emplace_back(
        [&](int value) { return value % divisor == 0; } // danger! reference to divisor will dangle!
        );
}
```
- by-value捕捉同样存在风险，例如对指针捕捉(尤其是对this的隐式捕捉）；而且有时候会造成代码阅读的困扰
```
std::vector<std::function<bool(int)>> filters;
class Widget
{
public:
    void addFilter() const 
    {
        filters.emplace_back([=](int value) { return value % divisor == 0; });
    }

private:
    int divisor;
};

void doSomeWork()    
{
    std::unique_ptr<Widget> pw = std::make_unique<Widget>();
    pw->addFilter();
}
// now filters hold dangeling this pointer
```
- 可以使用一些技巧避免by-value的这种危险情况发生
```
void Widget::addFilter() const 
{
    auto divisorCopy = divisor;
    filters.emplace_back([divisorCopy](int value) { return value % divisorCopy == 0; });
}
```
- 在C++14中，使用generalized lambda capture可以解决上述问题
```
void Widget::addFilter() const
{
    filters.emplace_back(                // C++14:
        [divisor = divisor](int value)   // copy divisor to closure
        { return value % divisor == 0; } // use the copy
    );
}
```
- 对于具有static storage duration的对象，lambda表达式的捕捉方式是by-reference的
```
void addDivisorFilter()
{
    static auto calc1 = computeSomeValue1(); // now static
    static auto calc2 = computeSomeValue2(); // now static
    static auto divisor = computeDivisor(calc1, calc2); // now static
    
    filters.emplace_back(
        [=](int value)                   // captures nothing!
        { return value % divisor == 0; } // refers to above static
    );
    ++divisor; // modify divisor
}
```
##### 条款32: 使用初始化捕捉（init capture）使得lambdas支持移动操作
- C++11中，lambdas仅仅支持 by-value 和 by-reference 两种捕捉方式
- C++14中，lambdas引入了新特性init capture，通过init capure，可以使得lambdas支持移动捕捉
```
auto upw = std::make_unique<T>();
auto func1 = [upw = std::move(upw)]()
{
    // do something on upw
};

auto func2 = [upw = std::make_unique<T>()]()
{
    // do something on upw
};
```
- 在C++11中，可以使用`std::bind`达到同样的效果，因为可以把移动构造对象进一个`std::bind`对象
```
std::vector<std::string> data(10, "hello");
auto func = std::bind(
    [](const std::vector<std::string>& data)
    {
        for (auto it = data.cbegin(); it != data.cend(); ++it)
            std::cout << *it << std::endl;
    },
    std::move(data));
```
```
auto func2 = [upw = std::make_unique<T>()]() {
    // do something on upw
};
// 在C++11中可以用std::bind模拟
auto func3 = std::bind([](const std::unique_ptr<T>& ptr) { // do something on ptr },
    std::make_unique<T>());
```

##### 条款34: 相较于`std::bind`, lambdas通常是更优的选择
- C++11中，移动捕捉方式只能通过`std::bind`模拟
- C++11中，lambdas是单态的，而`std::bind`是多态的
```
auto f = [](auto a, auto b) { cout << a << ' ' << b; } // 在C++11中，这是不可以的
f("test", 1.2f);

// 通过std::bind可以模拟这种实现
struct foo
{
  typedef void result_type;

  template < typename A, typename B >
  void operator()(A a, B b)
  {
    cout << a << ' ' << b;
  }
};
auto f = bind(foo(), _1, _2);
f("test", 1.2f); // will print "test 1.2"

```
- C++14中，lambdas引入了init capture从而支持移动捕捉方式
- C++14中，lambdas的参数可以使用auto
- 通常而言，lambdas的代码可读性比`std::bind`要好
```
class PolyWidget
{
public:
    template <typename T>
    void operator()(const T& param)
    {
        std::cout << param << std::endl;
    };
};

PolyWidget pw;
auto boundPW = std::bind(pw, std::placeholders::_1);
boundPW(2016);
boundPW("hello world");

// 在C++14中，用lambdas可以优雅的实现上述例子
class PolyWidget
{
public:
    template <typename T>
    void operator()(const T& param) /* const */
    {
        std::cout << param << std::endl;
    };
};
PolyWidget pw;
auto boundPW = [pw = pw](const auto& param) { pw(param); }; // C++14
boundPW(2016);
boundPW("hello world");

// 如果 Widget::operator() 是const函数
auto boundPW = [pw](const auto& param) { pw(param); }; // C++11, 这样是可以的
```

- by-value捕捉和mutable关键字
```
void test( const int &value )
{
    auto testConstRefMutableCopy = [value] () mutable {
        value = 2; // compile error: Cannot assign to a variable captured by copy in a non-mutable lambda
    };

    int valueCopy = value;
    auto testCopyMutableCopy = [valueCopy] () mutable {
        valueCopy = 2; // compiles OK
    };
}
```
  + 在lambda表达式中的value，虽然是by-value捕捉，但是因为外部它被声明为const int&，所以在lambda表达式内部它的类型是const int
  + lambda表达式被声明为mutable，并不能改变value的top-level const属性
  + 在C++14中，使用init-capture可以解决这个问题，即`[value = value]`, init-capture中使用的是`auto`推断规则，因此value的top-level const属性被抛弃
  + by-value捕捉，如果lambda表达式没有被声明为mutable，那么即使被by-value捕捉的外部变量不是top-level const的，在lambda表达式内部，它还是top-level const的
```
int x = 100;
auto f1 = [=]() { x = 1000 }; // error
auto f2 = [=]() mutable { x = 1000; }; // ok
```

##### Miscs
- lambda本质上就是一个创建在栈上的没有类型的C++对象，和其它的C++对象一样
- lambda会创建一个栈上的临时对象，因此可以用它去初始化变量
```
auto lamb = [](){return 0;};
```
- lambda作为栈上对象，和其它对象一样，也有constructor/destructor；对于capture list不为空的lambda，其捕获的变量会作为这个栈上对象的成员存在着。
 ＋ 对于按值捕捉，lambda产生的类必须为每一个值捕获的变量建立对应的数据成员，同时创建构造函数，用其捕获的变量的值来初始化数据成员
 ＋ 对于按引用捕捉，编译器可以直接使用该引用而无需在lambda产生的类中将其存储为数据成员

- lambda可以看作是 匿名函数子（anonymous functor）的语法糖（函数调用运算符的语法糖）
```
// In C++03
namespace {
  struct f {
    void operator()(int) {
      // 函数调用运算符，do something
    }
  };
}
void func(std::vector<int>& v) {
  f f;
  std::for_each(v.begin(), v.end(), f);
}
```
```
// In C++11
std::for_each(v.begin(), v.end(), [](int) { /* do something here*/ });
```

- lambda实现了一种closure（闭包）
> + A closure (also lexical closure, function closure or function value) is a function together with
a referencing environment for the non-local variables of that function. (Wiki)
> + A "closure" is an expression (typically a function) that can have free variables together with an environment that binds those variables (that "closes" the expression). (JavaScript)
> + 闭包中的闭不是“封闭内部状态”，而是“封闭外部状态”，一个函数如何封闭外部状态呢，当外部状态的scope失效时，还有一份留在内部状态中。延长了作用域链的叫闭包。对C++而言，当lambda表达式创建的时候，会拷贝一份引用的变量，以此实现内部状态和外部状态的隔离。

- lambda亦可以去创建`std::function`对象，`std::function`对象会拥有lambda捕获变量的拷贝
```
auto func_lamb = std::function<int()>(lamb);
auto func_lamb_ptr = new std::function<int()>(lamb); // 亦可以创建在堆上
```

- `std::function`是值语义的，但是支持移动操作

- lambda表达式在capture list为空的情况下可以转换为函数指针，但是并不意味着它就是函数指针

> + The closure type for a lambda-expression with no lambda-capture has a public non-virtual non-explicit const conversion function to pointer to function having the same parameter and return types as the closure type’s function call operator. The value returned by this conversion function shall be the address of a function that, when invoked, has the same effect as invoking the closure type’s function call operator.

```
void f1(int (*)(int)) {}
void f2(char (*)(int)) {}
void h(int (*)(int)) {}  // #1
void h(char (*)(int)) {} // #2
auto glambda = [](auto a) { return a; };
f1(glambda); // ok
f2(glambda); // error: not convertible
h(glambda);  // ok: calls #1 since #2 is not convertible
 
int& (*fpi)(int*) = [](auto* a)->auto& { return *a; }; // ok
```

- 每个lambda表达式的类型都是完全不同的，即使它们的签名完全一致，即使两者一字不差，
因此如果想把lambda表达式放入到标准库容器，例如`std::vector`，那么可以使用`std::function`或者其它东东。
```
auto ignore = [&]() { return 10; };
std::vector<decltype(ignore)> v;
v.push_back([&]() { return 100; }); // error!
-------------------------------------------
std::vector<std::function<int()>> functors;
functors.push_back([&] { return 100; });
functors.push_back([&] { return 10; });
```
- Useful links:
http://stackoverflow.com/questions/7627098/what-is-a-lambda-expression-in-c11
