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
    
/*
    // 如果不使用Copy-And-Swap idiom，可能的一种实现
    string& operator=(const string& other)
    {
        if (&other != this) // 必须检测自赋值
        {
            char str = new char[strlen(other.data_)+1]; // 必须注意异常安全问题，如果分配内存过程中抛出异常，不能破坏*this数据
            strcpy(str, other.data_);
            delete[] data_;
            data_ = str;
        }
        return *this;
    }

    string& operator=(string&& other)
    {
        if (&other != this)
        {
            data_ = other.data_;
            other.data_ = nullptr;
        }
        return *this;
    }
*/
    
private:
    void swap(string &other) noexcept
    {
        using std::swap;
        swap(data_, other.data_);
    }
    
    char *data_;
};
```
