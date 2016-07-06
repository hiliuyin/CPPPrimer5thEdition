- Copy-and-Swap idiom可以用于创建异常安全的赋值运算符的实现，又叫做 Create-Temporary-and-Swap

- string的赋值运算符的实现是经典的Copy-and-Swap例子
```
class string
{
public:
    string(const string& other); (1)
    string(string&& other);      (2)

    string& operator=(string other) // 异常安全, 如果创建other的过程中发生了异常，那么this数据并不会被破坏
    {
        swap(other);
        return *this;
    }
    
    // 而且上面的实现，如果实参是左值，那么other被(1)初始化；如果实参是右值，那么other被(2)初始化
    
private:
    void swap(string &other) noexcept
    {
        using std::swap;
        swap(data_, other.data_);
    }
    
    char *data_;
};
```
