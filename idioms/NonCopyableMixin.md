- Non-Copyable范式用于禁止类对象的拷贝构造和拷贝复制操作。

- `boost::noncopyable`就是Non-Copyable范式的一种实现
```
class noncopyable
{
protected:
    // 将constructor和destructor置于protected访问权限中，noncopyable对象是不能直接创建的
    noncopyable() = default; 
    ~noncopyable() = default; // 无需virtual修饰，因为无需多态

    // 将copy constructor和copy assignment设定为delete的
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
};

class NonCopyableClass : private noncopyable // private继承
{
.....
};
```
