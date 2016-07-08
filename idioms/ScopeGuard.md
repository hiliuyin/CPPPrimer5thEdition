- Scope Guard范式 是 RAII范式 的变种
 + 确保资源在抛出异常时能被释放，而在正常返回时不被释放
 + 异常安全

- Scope Guard范式的实现是在 RAII范式 实现的基础上加上条件检测
```
class ScopeGuard
{
public:
    ScopeGuard(Lock& l) : engaged_(true) { l.acquire(); }
    
    ~ScopeGuard()
    {
        if (engaged_) // 只有engaged_为true时，才会释放资源
        {
            l.release();
        }
    }
    
    void release()
    {
        engaged_ = false;
    }

private:
    bool engaged_;
};

void foo(Lock& l)
{
    ScopeGuard guard(l);
    
    // 如果发生异常，那么资源会被释放
    
    guard.release(); // engaged_置为false，当函数返回时，不会释放资源
}
```
