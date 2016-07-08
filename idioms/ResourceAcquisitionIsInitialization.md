- Resource Acquisition Is Initialization 即著名的 RAII范式

- RAII范式带来了两大好处
 + 使得资源能够在离开作用域时确保被释放
 + 保证异常安全
 
- `std::unique_ptr` 和 `std::shared_ptr` 就是RAII范式在标准库中的应用

- RAII范式将资源的分配和释放分别置于一个资源管理类的构造函数和析构函数，利用了C++标准中有关destructor的调用规则:
C++标准保证了destructor会在离开已经被成功创建对象的作用域时被调用（包括正常的离开和抛出异常）

```
//  Private copy constructor and copy assignment ensure classes derived 
//  from class NonCopyable cannot be copied.
class NonCopyable 
{
   NonCopyable (NonCopyable const &); // private copy constructor
   NonCopyable & operator = (NonCopyable const &); // private assignment operator
};

template <class T>
class AutoDelete : NonCopyable
{
  public:
    AutoDelete (T * p = 0) : ptr_(p) {}
    ~AutoDelete () throw() { delete ptr_; } // 析构函数中释放
  private:
    T *ptr_;
};

class ScopedLock : NonCopyable// Scoped Lock idiom
{
  public:
    ScopedLock (Lock & l) : lock_(l) { lock_.acquire(); }
    ~ScopedLock () throw () { lock_.release(); } // 析构函数中释放
  private:
    Lock& lock_;
};

void foo ()
{
  X * p = new X;
  AutoDelete<X> safe_del(p); // 不会发生内存泄漏
  p = 0;
  // Although, above assignment "p = 0" is not necessary
  // as we would not have a dangling pointer in this example. 
  // It is a good programming practice.

  if (...)
    if (...)
      return; 
 
  // 无需显式调用delete，safe_del的析构函数一定会被隐式调用去释放内存
}
void X::bar()
{
  ScopedLock safe_lock(l); // 锁一定会被释放
  if (...)
    if (...)
      throw "ERROR"; 
  // 无需显式释放锁，safe_lock的析构函数一定会被隐式调用去释放锁
}
```
 
