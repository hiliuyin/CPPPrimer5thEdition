- pimpl 是 point to implementation 的缩写
- 一种使用`std::unique_ptr`的pimpl的实现
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

    void foo();

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
    
    void foo() {}
};

Widget::Widget() : impl_(std::make_unique<WidgetImpl>()) {} // WidgetImpl的完全定义必须可见

Widget::~Widget() = default; 
// 必须在*.cpp中定义，因为unique_ptr的销毁过程会调用Widget的析构函数，此时WidgetImpl完全定义必须是可见的

Widget::Widget(Widget&& w) = default; // 因为unique_ptr本身支持移动语义，因此=default可以满足要求
Widget& Widget::operator=(Widget&& w) = default;

Widget::Widget(const Widget& rhs) : impl_(std::make_unique<WidgetImpl>(*rhs.impl_) {}
Widget& Widget::operator=(const Widget& rhs)
{
    if (this != &rhs)
    {
        *impl_ = *rhs.impl_;
    }
    return *this;
}

void Widget::foo()
{
    impl_->foo();
}
```
