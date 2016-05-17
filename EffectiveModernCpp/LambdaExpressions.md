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
#####条款34: 相较于`std::bind`, lambdas通常是更优的选择
