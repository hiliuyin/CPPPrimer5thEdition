######条款31: 避免使用默认的捕捉方式
- C++11中，lambda表达式支持两种捕捉方式：by-value 和 by-reference
```
auto byValue = [=]() {};
auto byRef = [&]() {};
```
- by-reference捕捉方式使用不当，会导致捕捉的引用无效(dangling)
```
using FilterContainer = std::vector<std::function<bool(int)>>;
Filter Container filters;

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
- 在C++11中，可以使用`std::bind`达到同样的效果，因为可以把对象移动构造进一个`std::bind`对象
```
```
 


#####条款33: 
#####条款34: 相较于`std::bind`, lambdas通常是更优的选择
- C++14
