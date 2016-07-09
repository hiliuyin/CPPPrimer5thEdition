- CRTP 即 Curiously Recurring Template Pattern（奇异递归模板模式）

- CRTP的基本形式
```
template <class T>
class Base
{
    // 基类中的方法可以通过template访问派生类的成员
};
class Derived : public Base<Derived> // Derived类的Base类模板特化的类型实参是Derived类，奇异递归(Curiously Recurring)
{
    // ...
};
```

- CRTP的优点:
 + 编译期多态（静态多态），比通过虚函数实现的运行时多态（动态多态）的效率要高
 + 代码异常简洁优雅，简化接口，代码复用

- `std::enable_shared_from_this`的实现就是CRTP的实际应用
```
template<class _Tp>
class enable_shared_from_this
{
    mutable weak_ptr<_Tp> __weak_this_;
    
protected:
    enable_shared_from_this() _NOEXCEPT {}
    
    enable_shared_from_this(enable_shared_from_this const&) _NOEXCEPT {}
    
    enable_shared_from_this& operator=(enable_shared_from_this const&) _NOEXCEPT
        { return *this; }
    
    ~enable_shared_from_this() {}

public:
    shared_ptr<_Tp> shared_from_this()
        {return shared_ptr<_Tp>(__weak_this_);}
    
    shared_ptr<_Tp const> shared_from_this() const
        {return shared_ptr<const _Tp>(__weak_this_);}
};

struct Good: std::enable_shared_from_this<Good>
{
    std::shared_ptr<Good> getptr() {
        return shared_from_this();
    }
};
 
struct Bad
{
    std::shared_ptr<Bad> getptr() {
        return std::shared_ptr<Bad>(this);
    }
    ~Bad() { std::cout << "Bad::~Bad() called\n"; }
};
 
int main()
{
    // Good: the two shared_ptr's share the same object
    std::shared_ptr<Good> gp1(new Good);
    std::shared_ptr<Good> gp2 = gp1->getptr();
    std::cout << "gp2.use_count() = " << gp2.use_count() << '\n';
 
    // Bad, each shared_ptr thinks it's the only owner of the object
    std::shared_ptr<Bad> bp1(new Bad);
    std::shared_ptr<Bad> bp2 = bp1->getptr();
    std::cout << "bp2.use_count() = " << bp2.use_count() << '\n';
} // UB: double-delete of Bad
```

