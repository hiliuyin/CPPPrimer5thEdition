- Resource Acquisition Is Initialization 即著名的 RAII 惯用法

- RAII惯用法带来了两大好处
 + 使得资源能够在离开作用域时确保被释放
 + 保证异常安全
 
- RAII惯用法将资源的分配和释放分别置于一个资源管理类的构造函数和析构函数，利用了C++标准中有关destructor的调用规则:
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
    ~AutoDelete () throw() { delete ptr_; } 
  private:
    T *ptr_;
};

class ScopedLock : NonCopyable// Scoped Lock idiom
{
  public:
    ScopedLock (Lock & l) : lock_(l) { lock_.acquire(); }
    ~ScopedLock () throw () { lock_.release(); } 
  private:
    Lock& lock_;
};

void foo ()
{
  X * p = new X;
  AutoDelete<X> safe_del(p); // Memory will not leak
  p = 0;
  // Although, above assignment "p = 0" is not necessary
  // as we would not have a dangling pointer in this example. 
  // It is a good programming practice.

  if (...)
    if (...)
      return; 
 
  // No need to call delete here.
  // Destructor of safe_del will delete memory
}
void X::bar()
{
  ScopedLock safe_lock(l); // Lock will be released certainly
  if (...)
    if (...)
      throw "ERROR"; 
  // No need to call release here.
  // Destructor of safe_lock will release the lock
}
```
 
