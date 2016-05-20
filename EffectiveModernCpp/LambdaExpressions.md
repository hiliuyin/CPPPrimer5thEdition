
#####条款31: 避免使用默认的捕捉方式
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
- 在C++14中，使用generalized lambda capure可以解决上述问题
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
#####条款32: 使用初始化捕捉（init capture）使得lambdas支持移动操作
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

#####条款34: 相较于`std::bind`, lambdas通常是更优的选择
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
 + lambda表达式被声明为mutable，并不能改变value的const属性
 + 在C++14中，使用init-capture可以解决这个问题，即`[value = value]`
 + init-capture中使用的是`auto`推断规则，因此value的const属性被抛弃
