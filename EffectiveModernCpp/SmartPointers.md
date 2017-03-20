### 条款18: 使用`std::unique_ptr`管理需独占资源所有权的对象
- C++11引入了`std::unique_ptr`，替代了之前的`std::auto_ptr`
- `std::unique_ptr` 是类模板
- `std::unique_ptr` 只支持移动语义，禁用拷贝语义操作
- `std::unique_ptr` 不可作为容器中的元素
- `std::unique_ptr` 是轻量级的，通常情况下，`std::unique_ptr`的大小和raw pointer的大小一样
- `std::unique_ptr` 支持自定义释放内存的操作，即自定义的deleter
- 如果要自定义deleter，只能通过`std::unique_ptr`的构造函数；`std::make_unique`没有重载支持自定义deleter的函数
- 如果用函数指针作为自定义的deleter，那么实例化的`std::unique_ptr`的大小大于raw pointer
- 如果用lambda表达式作为自定义的deleter，那么不会增加实例化的`std::unique_ptr`的大小
```
enum class InvestType
{
    Stock = 0,
    Bond = 1,
    RealEstate = 2
};
class Investment
{
public:
    virtual ~Investment() = default; // 多态的基类析构函数必须是virtual的，否则通过指向派生类的基类指针delete是undefined behavior
};
class Stock : public Investment {};
class Bond : public Investment {};
class RealEstate : public Investment {};

auto delInvmt = [](Investment* pInvestment) // lambda expression, 自定义deleter
{
    std::cout << "custom deleter";
    delete pInvestment;
};

template <typename... Ts>
std::unique_ptr<Investment, decltype(delInvmt)> makeInvestment(InvestType type, Ts&&... params)
{
    std::unique_ptr<Investment, decltype(delInvmt)> pInv(nullptr, delInvmt); // 自定义deleter，通过构造函数
    
    if (type == InvestType::Stock)
        pInv.reset(new Stock(std::forward<Ts>(params)...));
    else if (type == InvestType::Bond)
        pInv.reset(new Bond(std::forward<Ts>(params)...));
    else if (type == InvestType::RealEstate)
        pInv.reset(new RealEstate(std::forward<Ts>(params)...));

    return pInv;
}

void foo()
{
    auto pInv = makeInvestment(InvestType::Bond);
}
```

- `std::unique_ptr<T[]>`用于分配数组

- `std::unique_ptr`非常适用于factory模式
- 可以将`std::unique_ptr`右值转化为`std::shared_ptr`，因为`std::shared_ptr`的一个构造函数接受`std::unique_ptr`的右值引用作为参数
```
std::unique_ptr<Investment> makeInvestment()
{
    std::unique_ptr<Investment> p(new Investment());
    return p;
}
std::shared_ptr<Investment> si = makeInvestment();
```
-  为什么对`std::unique_ptr`进行sizeof的结果能和 raw pointer 一样大小
 + 因为`class _Dp = default_delete<_Tp>`是一个empty类，因此编译器可以实施base empty class optimization
 + 关于base empty class optimization，参见 http://www.stroustrup.com/bs_faq2.html#sizeof-empty
```
template <class _Tp, class _Dp = default_delete<_Tp> >
class _LIBCPP_TYPE_VIS_ONLY unique_ptr
{
public:
    typedef _Tp element_type;
    typedef _Dp deleter_type;
    typedef typename __pointer_type<_Tp, deleter_type>::type pointer;
private:
    __compressed_pair<pointer, deleter_type> __ptr_;
...
};
```
### 条款19: 使用`std::shared_ptr`管理共享资源的对象
- 相较于`std::unique_ptr`，`std::shared_ptr`是重量级的
- `std::shared_ptr`的引用计数的增减是原子操作
- `std::shared_ptr`的大小是raw pointer的两倍，是因为它包含两个指针，一个指向创建的对象，一个指向control block
- `std::shared_ptr`的control block中包含有strong/weak的引用计数，自定义deleter 等等
- `std::shared_ptr`支持自定义deleter，但自定义deleter不会增加`std::shared_ptr`的大小，因为自定义deleter位于control block中
- `std::shared_ptr`自定义deleter的方式和`std::unique_ptr`不同，两者的类模板定义方式也不同，自定义deleter类型是`std::unique_ptr`类模板的模板类型参数
```
auto loggingDel = [](Widget* pw) { delete pw; };

std::unique_ptr<Widget, decltype(loggingDel)> upw(new Widget(), loggingDel);
std::shared_ptr<Widget> spw(new Widget(), loggingDel);
```
- 什么时候需要分配control block?
  + 调用`std::make_shared`
  + 调用`std::shared_ptr`的构造函数从raw pointer创建`std::shared_ptr`
  + 当`std::shared_ptr`从`std::unique_ptr`或`std::auto_ptr`创建出来的
- 使用`std::make_shared`创建`std::shared_ptr`可以避免两次调用new操作符分配两次内存（一次是对象，一次是control block）

- `std::shared_ptr`支持拷贝语义，也支持移动语义
- `std::shared_ptr`可作为容器中的元素

- 尽量避免使用raw pointr去创建`std::shared_ptr`
```
auto pw = new Widget;
std::shared_ptr<Widget> spw1(pw);
std::shared_ptr<Widget> spw2(pw);
```
- 如果用raw pointer创建`std::shared_ptr`，通过临时对象可以避免上述情况
```
std::shared_ptr<Widget> spw1(new Widget);
std::shared_ptr<Widget> spw1(spw2);
```

- 使用`std::make_shared`时，要避免引用循环；使用`std::weak_ptr`可以避免这种情况
```
struct A { std::shared_ptr<A> ptr; };
void f()
{
    std::shared_ptr<A> p1 = std::make_shared<A>();
    std::shared_ptr<A> p2 = std::make_shared<A>();
    p1->ptr = p2;
    p2->ptr = p1;
} // 离开此作用域，p1和p2不会被销毁
```
- `std::enable_shared_from_this`是奇异递归模板（CRTP: **Curiously Recurring Template Pattern**）
- `std::enable_shared_from_this`可以使我们安全地从this指针创建`std::shared_ptr`
```
std::vector<std::shared_ptr<Widget>> processWidgets;
void Widget::process()
{
    processWidgets.emplace_back(this); // 如果外部已经存在shared_ptr管理这个Widget对象呢？
}
```
- `std::enable_shared_from_this`提供了`shared_from_this`成员函数
```
class Widget : public std::enable_shared_from_this<Widget>
{
public:
    void process();
};

void Widget::process()
{
    processWidgets.emplace_back(shared_from_this());
}
```
- `shared_from_this`不会创建control block，因此必须存在已经创建好的管理该对象的`std::shared_ptr`，否则会抛出异常
```
// 通过下面的模式，可以避免这种情况
class Widget;
std::vector<std::shared_ptr<Widget>> processedWidgets;

class Widget : public std::enable_shared_from_this<Widget>
{
public:
    template <typename... Ts>
    static std::shared_ptr<Widget> createWidget(Ts&&... params) 
    {
        auto ptr = std::make_shared<Widget>(std::forward<Ts>(params)...);
        return ptr;
    }
    
    void process()
    {
        processedWidgets.emplace_back(shared_from_this());
    }
    
private:
    Widget() = default;
};
```

#### 条款20: 如何使用`std::weak_ptr`
- `std::weak_ptr`是`std::shared_ptr`的伴随类，它是一种弱引用，指向`std::shared_ptr`所管理的对象
- `std::weak_ptr`共享使用`std::shared_ptr`的control block，control block包括有`std::weak_ptr`的弱引用计数
- 使用`std::weak_ptr`可避免引用循环
- `std::weak_ptr`适合用于观察者模式
- `expired()`可用于检查`std::weak_ptr`对应的`std::shared_ptr`是否有效
```
auto spw = std::make_shared<Widget>();
std::weak_ptr<Widget> wpw(spw);
spw = nullptr;
...
if (wpw.expired())
    ...
```
- `lock()`可以返回有效的`std::shared_ptr`或者`nullptr`
```
std::shared_ptr<Widget> spw1 = wpw.lock();
```

### 条款21: 优先使用`std::make_shared`和`std::make_unique`而不是new操作符
- 使用`std::make_shared`和`std::make_unique`函数可以使代码变的优雅
```
std::shared_ptr<Widget> pw(new Widget());
auto pw = std::make_shared<Widget>();
```
- 如果要定制deleter的话，那么就不能使用`std::make_shared`和`std::make_unique`
- 对`std::shared_ptr`，使用`std::make_shared`更加高效，可以减少一次内存分配
```
std::shared_ptr<Widget> pw(new Widget()); // new操作符分配一次内存；然后再分配control block
auto pw = std::make_shared<Widget>(); // 一次分配所有内存(包括管理的对象和control block)
```
- 异常安全是使用`std::make_shared`和`std::make_unique`的又一优点
```
processWidget(std::shared_ptr<Widget>(new Widget), computePriority()); // 非异常安全
processWidget(std::make_shared<Widget>(), computePriority()); // 异常安全
```
- `std::make_shared`和`std::make_unique`  
May throw std::bad_alloc or any exception thrown by the constructor of T. If an exception is thrown, this function has no effect.

### 条款22: 当使用Pimpl惯用法时，必须将构造函数和析构函数定义在*.cpp文件中
- Pimpl惯用法
```
// In Widget.h
class Widget
{
public:
    Widget();
    ~Widget(); 
    // 必须显式定义析构函数，因为编译器生成的destructor会发现WidgetImpl不是完全定义从而导致编译错误，但是不能在头文件中定义
    
    Widget(Widget&& w); 
    // 因为显式定义了析构函数，所以move constructor和move assignment operator必须显式定义，否则会被编译器定义为delete
    Widget& operator=(Widget&& w);
    
    Widget(const Widget& rhs); 
    // 必须显式定义拷贝构造函数和拷贝赋值运算符，因为std::unique_ptr仅支持移动语义，而且编译器生成的拷贝函数执行的是浅拷贝
    Widget& operator=(const Widget& rhs);

private:
    struct WidgetImpl; // Incomplete type
    std::unique_ptr<WidgetImpl> impl_; // std::unique_ptr 支持Incomplete type 
};

// In Widget.cpp
struct Widget::WidgetImpl
{
    std::string name;
    std::vector<double> data;
    Gadget g1, g2, g3;
};

Widget::Widget() : impl_(std::make_unique<WidgetImpl>()) {} // WidgetImpl的完全定义必须可见

Widget::~Widget() = default; 
// 必须在*.cpp中定义，因为unique_ptr的销毁过程会调用Widget的析构函数，此时WidgetImpl完全定义必须是可见的

Widget::Widget(Widget&& w) = default; // 因为unique_ptr本身支持移动语义，因此=default可以满足要求
Widget& Widget::operator=(Widget&& w) = default;

Widget::Widget(const Widget& rhs) : impl(std::make_unique<WidgetImpl>(*rhs.impl_) {}
Widget& Widget::operator=(const Widget& rhs)
{
    if (this != &rhs)
    {
        *impl_ = *rhs.impl_;
    }
    return *this;
}
```

- miscs  
http://stackoverflow.com/questions/6012157/is-stdunique-ptrt-required-to-know-the-full-definition-of-t/


