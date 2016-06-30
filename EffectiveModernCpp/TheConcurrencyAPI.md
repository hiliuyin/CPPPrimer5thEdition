#####条款35: 相较于`std::thread`，优先使用`std::async`
- C++11的一个巨大进步就是在语言层面引入了支持多线程的标准库，从而使得C++程序员告别了从前必须依赖第三方线程库的时代
- `std::thread` 和 `std::async` 之间有很多需要特别关注的区别

- 从操作系统的角度上看，`std::thread`可以映射到通常概念上的操作系统线程
- `std::thread`是定义在std命名空间的类，因此它一定会创建一个新的线程对象(thread)
```
int doAsyncWork();
std::thread t(doAsyncWork);
```
- `std::thread`支持移动构造和移动赋值，但是不支持拷贝构造和拷贝赋值，

- `std::async`是定义在std空间的模板函数，它的返回值类型是`std::future`
```
int doAsyncWork();
auto fut = std::async(doAsyncWork); // fut is std::future type
```
- 通过`std::async`，无论doAsyncWork正常返回，还是在执行中抛出了异常，都可以通过`std::future`提供的get()方法获得结果
- 通过`std::thread`，如果doAsyncWork抛出异常，那么就会调用`std::terminate`中止程序的继续执行；而且也没有提供类似`std::async`那样直接的方式获得doAsyncWork的返回值
- 通过`std::thread`必然创建新的线程，如果系统资源不足的话，会抛出`std::system_error`异常，将doAsyncWork显式声明为`noexcept`并不能阻止异常的产生
```
int doAsyncWork() noexcept;
std::thread t(doAsyncWork);
```
- `std::async`不一定会创建新的线程去执行异步任务，在默认的执行策略下，有可能会在当前线程中执行（异步的形式，实质上却是同步函数调用），在这种情况下，就会避免线程过载。
- 在GUI线程中使用`std::async`，应该显式指定`std::launch::async`启动策略，否则可能将需要实时响应的GUI线程阻塞。
- `std::thread`提供了`native_handle`方法可以返回底层的线程对象（例如pthread或者windows线程），通过这个接口，我们可以通过底层的API去操作线程

#####条款36: 如果确定需要异步执行，必须显式指定`std::launch::async`启动策略
- `std::launch`枚举类定义了`std::async`的启动策略
- `std::launch::async`启动策略：保证了任务在另一个线程中异步执行
- `std::launch::deferred`启动策略：任务会被延迟执行，只有当`std::async`的返回值std::future的get或wait方法被调用的时候才会在调用get或wait方法的线程上下文中同步执行并且阻塞该线程直到任务执行完成；如果get或wait没有被调用，那么任务永远不会被执行。即所谓的延迟求值(lazy evaluation)。
- `std::async`的默认启动策略相当于：
```
auto fut = std::async(std::launch::async | std::launch::deferred, f);
```
- `std::async`如果是默认启动策略，那么究竟是异步执行还是同步执行，还是不被执行，我们不能对此做任何假设。
- 显式指定`std::launch::async`参数以保证任务被异步执行
- 如果`std::async`是默认启动策略或者`std::launch::deferred`启动策略，那么需要注意get, wait, wait_for和wait_until的用法
```
void f()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

auto fut = std::async(std::launch::deferred, f);
while (fut.wait_for(std::chrono::microseconds(100)) != std::future_status::ready)
{
    // infinite loop here, since wait_for always return std::future_status::deferred
}
```
```
// 将上述代码改成下面这样，就可以正确执行
void f()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

auto fut = std::async(std::launch::deferred, f);
if (fut.wait_for(std::chrono::seconds(0)) == std::future_status::deferred)
{
    fut.wait();
}
else
{
    while (fut.wait_for(std::chrono::microseconds(100)) != std::future_status::ready)
    {
    }
}
```

#####条款37: 确保`std::thread`在所有路径上unjoinable
- `std::thread`对象有joinable和unjoinable两种状态
- 什么时候`std::thread`对象是joinable状态: 一个joinable的`std::thread`对象对应着一个底层的线程
 + 正在运行或者准备运行的线程
 + 被阻塞的线程
 + 等待被调度的线程
 + 已经运行结束还未joined的线程
- 什么时候`std::thread`对象是unjoinable状态
 + 通过默认构造函数创建的`std::thread`对象
 + 底层线程已经被移动绑定到其它`std::thread`对象
 + 已经joined的线程
 + 已经detached的`std::thread`对象，detach意味着`std::thread`对象和底层线程已经解绑

- 一个很*重要*的问题：处于joinable状态的`std::thread`对象，如果该对象的析构函数被调用的话，那么会导致程序中止。
- 用RAII惯用法，可以优雅的解决这个问题: **Make std::threads unjoinable on all paths.**
```
class RAIIThread
{
public:
    enum class DtorAction { join, detach };
    
    RAIIThread(std::thread &&t, DtorAction act) : act_(act), t_(std::move(t)) {}
    ~RAIIThread()
    {
        if (t_.joinable())
        {
            if (act_ == DtorAction::join)
                t_.join();
            else if (act_ == DtorAction::detach)
                t_.detach();
        }
    }
    
    RAIIThread(RAIIThread &&t) = default;
    RAIIThread& operator=(RAIIThread &&rhs) = default;
    
private:
    DtorAction act_;
    std::thread t_;
};
```

#####条款38: Be aware of varying thread handle destructor behavior.
