#####条款35: 相较于`std::thread`，优先使用`std::async`
- C++11的一个巨大进步就是在语言层面引入了支持线程的标准库，从而使得C++程序员告别了之前必须依赖第三方线程库的时代
- `std::thread` 和 `std::async` 之间有很多微妙的区别
- `std::thread`可以映射到通常概念上的操作系统线程
- `std::thread`是定义在std空间的类，通过它一定会创建一个新的线程对象(thread)，`std::thread`不支持拷贝构造和拷贝赋值，支持移动构造和移动赋值
```
int doAsyncWork();
std::thread t(doAsyncWork);
```
- `std::async`是定义在std空间的模板函数，它的返回值类型是`std::future`
```
int doAsyncWork();
auto fut = std::async(doAsyncWork);
```
- 通过`std::async`，无论doAsyncWork是正常返回，还是在执行中抛出了异常，都可以通过`std::future`提供的get方法获得结果
- 通过`std::thread`，如果doAsyncWork抛出异常，那么就会调用`std::terminate`中止程序的继续执行；而且也没有提供类似`std::async`那样直接的方式获得doAsyncWork的返回值
- 通过`std::thread`必然创建新的线程，如果系统资源不足的话，会抛出`std::system_error`异常，即使将doAsyncWork显式声明为`noexcept`并不能阻止异常的产生
```
int doAsyncWork() noexcept;
std::thread t(doAsyncWork);
```
- `std::async`不一定会创建新的线程去执行异步任务，在默认的执行策略下，有可能会在当前线程中执行（异步的形式，实质上却是同步函数调用），在这种情况下，就会避免线程过载。

- 在GUI线程中使用`std::async`，应该显式指定`std::launch::async`执行策略，否则可能将需要实时响应的GUI线程阻塞。

#####条款36: 在确定需要异步执行的前提下，必须显式指定`std::launch::async`执行策略
- 

