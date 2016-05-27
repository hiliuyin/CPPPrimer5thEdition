#####条款35: 相较于`std::thread`，优先使用`std::async`
- C++11的一个巨大进步就是在语言层面引入了支持线程的标准库，从而使得C++程序员告别了之前必须依赖第三方线程库的时代
- `std::thread` 和 `std::async` 之间有很多微妙的区别
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
- 通过`std::thread`，如果doAsyncWork抛出异常，那么就会调用`std::terminate`中止程序的继续执行；而且也无从得知doAsyncWork的返回值

- 将doAsyncWork显式声明为noexcept并不能阻止某些异常的产生
```
int doAsyncWork() noexcept;
std::thread t(doAsyncWokr);
```



