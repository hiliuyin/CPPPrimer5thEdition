#####条款35: 相较于`std::thread`，优先使用`std::async`
- C++11的一个巨大进步就是在语言层面引入了支持多线程的标准库，从而使得C++程序员告别了从前必须依赖第三方线程库的时代
程序员可以忽略平台上的差异，无需介意底层实现究竟是POSIX线程库还是其它，当然C++ API也提供了std::thread::native_handle
获得底层线程库的native handler。与此同时，对于程序员而言，跨平台多线程程序开发要比从前简单明了了。程序员无需再去造轮子去屏蔽底层差异了。

- `std::thread` 和 `std::async` 之间有很多需要特别关注的区别
- 从操作系统的角度上看，`std::thread`可以映射到通常概念上的操作系统线程
- `std::thread`是定义在std命名空间的类，它一定会创建一个新的线程对象(thread)
```
int doAsyncWork();
std::thread t(doAsyncWork);
```
- `std::thread`支持移动构造和移动赋值，但是不支持拷贝构造和拷贝赋值

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
 + 如果启动策略是`std::launch::deferred`，那么`std::future::wait_for`和`std::future::wait_until`会立即返回`std::future_status::deferred`，下面的例子展示了这个问题
 + 如果启动策略是`std::launch::deferred`，对于`std::future::wait`则不存在这个问题，它会阻塞当前线程直到`std::future_status::ready`状态
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

- 什么时候`std::thread`对象是joinable状态: 一个joinable的`std::thread`对象对应着一个活动的(active)底层线程，即使该线程对象那个已经运行结束但还未join，也看作是joinable的
 + 正在运行或者准备运行的线程
 + 被阻塞的线程
 + 等待被调度的线程
 + 已经运行结束还未joined的线程

- `std::thread::join`的用法
 + 会阻塞当前线程，直到join对应的线程对象执行完毕
 + 如果`this->get_id() == std::this_thread::get_id()`，则发生死锁
 + join返回后，线程对象进入unjoinable状态
 
- 什么时候`std::thread`对象是unjoinable状态
 + 通过默认构造函数创建的`std::thread`对象
 + 底层线程已经被移动绑定到其它`std::thread`对象
 + 已经joined的线程
 + 已经detached的`std::thread`对象，detach意味着`std::thread`对象和底层线程已经解绑

- `std::thread`对象勿忘detach或join，一种代码常用的写法就是线程启动后立刻将`std::thread`对象和底层执行线程detach
``` 
std::packaged_task<int()> task([](){ return 7; });
std::future<int> f1 = task.get_future();
std::thread(std::move(task)).detach(); // 线程启动后立刻detach
 
std::promise<int> p;
std::future<int> f3 = p.get_future();
std::thread( [&p]{ p.set_value_at_thread_exit(9); }).detach(); // 线程启动后立刻detach
 
std::cout << "Waiting..." << std::flush;
f1.wait();
f3.wait();
```

- 一个很重要的问题：处于joinable状态的`std::thread`对象，如果该对象的析构函数被调用的话，那么会导致程序中止。
 + 用RAII惯用法，可以优雅的解决这个问题，即：Make std::threads unjoinable on all paths.
```
class RAIIThread
{
public:
    enum class DtorAction { join, detach };
    
    RAIIThread(std::thread &&t, DtorAction act) : act_(act), t_(std::move(t)) {}
    ~RAIIThread() // 析构函数中，将joinable的std::thread对象变为unjoinable状态，从而使得即使析构函数被调用，程序也不会中止
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
- C++11引入了模板类`std::future`用于存取异步执行线程的结果
 + `std::future`只支持移动操作，不支持拷贝操作
- 异步执行线程的结果，不能保存在callee（被调用线程）的`std::promise`处
 + 因为在`std::future`被访问之前，callee就可能已经被销毁（线程结束）
- 异步执行线程的结果，不能保存在caller（调用线程）的`std::future`
 + `std::shared_future`用于多个线程对象访问
 + `std::shared_future`可以由`std::future`创建，即使`std::future`中的线程执行结果是只支持移动的，因此`std::future`不能随意销毁

- 有一块heap区域，称之为shared state，用于存储异步执行线程的结果

- `std::future`的析构函数的行为准则
 + The destructor for the last future referring to a shared state for a non-deferred task launched via std::async blocks until the task completes. In essence, the destructor for such a future does an implicit join on the thread on which the asynchronously executing task is running.
 + The destructor for all other futures simply destroys the future object. For asynchronously running tasks, this is akin to an implicit detach on the underlying thread. For deferred tasks for which this is the final future, it means that the deferred task will never run.
 + 好拗口的俩准则啊！
 + 对于`std::launch::async`的thread，要么implicit join，要么implicit detach
 + 对于`std::launch::deferred`的thread，直接销毁future对象，也就是该thread不会被执行
 
- `std::packaged_task`提供了另外一种方式用于异步执行结果的返回
```
int calcValue();
std::packaged_task<int()> pt(calcValue);
auto fut = pt.get_future();

std::thread t(std::move(pt)); // std::packaged_task只支持移动操作，不支持拷贝操作
......
```

- 关于`std::promise`极好的解释：  
http://stackoverflow.com/questions/11004273/what-is-stdpromise

- `std::async`, `std::packaged_task` 和 `std::promise`提供了三种不同层级的方式用于异步操作

- `std::async`是最方便直观的方式，通过这个模板函数的返回值即可得到其返回的future，但是究竟过程如何我们是无法控制的
```
auto fut = std::async(foo, 1.5, 'x', false);  // is a std::future<int>
.....
// 在某处，可以方便地得到异步执行的结果（当然根据`std::async`的启动策略，亦有可能是同步执行）
auto res = fut.get();  // is an int
```

- `std::packaged_task`提供了一个比`std::async`更底层更灵活的方式，它生成了一个可以调用的对象，将究竟何时何地调用的决定权留给了用户
```
std::packaged_task<int(double, char, bool)> tsk(foo);
auto fut = tsk.get_future();    // is a std::future<int>
......
// 在某处，用户可以灵活地通过`std::thread`对象执行
std::thread thr(std::move(tsk), 1.5, 'x', false); // `std::packaged_task`对象只支持移动操作
......
// 同样，通过get获取future的结果
auto res ＝ fut.get(); // is an int
```

- `std::promise`提供了最底层的方式，本质上`std::packaged_task`的实现也是使用了`std::promise`

- 最简单直接的例子
```
int main()
{
    // future from a packaged_task
    std::packaged_task<int()> task([](){ return 7; }); // wrap the function
    std::future<int> f1 = task.get_future();  // get a future
    std::thread(std::move(task)).detach(); // launch on a thread
 
    // future from an async()
    std::future<int> f2 = std::async(std::launch::async, [](){ return 8; });
 
    // future from a promise
    std::promise<int> p;
    std::future<int> f3 = p.get_future();
    std::thread( [&p]{ p.set_value_at_thread_exit(9); }).detach();
 
    std::cout << "Waiting..." << std::flush;
    f1.wait();
    f2.wait();
    f3.wait();
    std::cout << "Done!\nResults are: "
              << f1.get() << ' ' << f2.get() << ' ' << f3.get() << '\n';
}
```

#####条款39: Consider void futures for one-shot event communication.
- `std::condition_variable`是一种非常常用的多线程同步原语，阻塞一个或多个线程去等待某个条件的满足，当某个线程更改了条件并发出通知，这些阻塞的线程会被唤醒并且继续执行后续的操作。
- `std::condition_variable`的用法(假设两个线程A和B)
- 线程A (更改条件并发出通知)
 + 获得mutex（可以通过`std::lock_guard`，RAII范式的实现）
 + 在获得mutex的条件下，更改条件，例如某个变量
 + 通知其它阻塞状态等待条件满足的线程，通过：notify_one 或 notify_all（通知时不需要在获得mutex的前提下）
- 线程B (阻塞等待条件的满足)
 + 获取mutex（通过`std::unique_lock<std::mutex>`）
 + 调用wait，wait_for 或 wait_until。这些wait操作会把之前获取到的mutex释放，并且将线程B挂起进入阻塞状态。wait操作是原子的。
 + 当条件变量被通知，超时或者虚假唤醒时，线程B被唤醒，并且重新获取到mutex，该操作也是原子的。因为存在虚假唤醒的情况，此时必须检查条件是否真正被满足。
- 上面的描述有点拗口，还是读下面的代码去理解一下。
```
std::mutex m;
std::condition_variable cv;
std::string data;
bool ready = false;
bool processed = false;
 
void worker_thread()
{
    // Wait until main() sends data
    std::unique_lock<std::mutex> lk(m); // std::condition_variable必须和std::unique_lock<std::mutex>搭配使用
    cv.wait(lk, []{return ready;});     // 使用了lambda表达式，很优雅的实现，同时解决了虚假唤醒问题 
 
    // after the wait, we own the lock.
    std::cout << "Worker thread is processing data\n";
    data += " after processing";
 
    // Send data back to main()
    processed = true;
    std::cout << "Worker thread signals data processing completed\n";
 
    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    lk.unlock(); // 通知时不需要在获得mutex的前提下
    cv.notify_one();
}
 
int main()
{
    std::thread worker(worker_thread);
 
    data = "Example data";
    // send data to the worker thread
    {
        std::lock_guard<std::mutex> lk(m);
        ready = true;
        std::cout << "main() signals data ready for processing\n";
    }
    cv.notify_one(); // 通知时不需要在获得mutex的前提下
 
    // wait for the worker
    {
        std::unique_lock<std::mutex> lk(m); // std::condition_variable必须和std::unique_lock<std::mutex>搭配使用
        cv.wait(lk, []{return processed;}); // 使用了lambda表达式，很优雅的实现，同时解决了虚假唤醒问题 
    }
    std::cout << "Back in main(), data = " << data << '\n';
 
    worker.join();
}
```
- 详细解释下`wait()`如何防止虚假唤醒
```
template< class Predicate >
void wait( std::unique_lock<std::mutex>& lock, Predicate pred );
// 相当于
while (!pred()) {
    wait(lock);
}
```

- 使用`std::atomic`可以实现轮询（polling）机制的线程同步
```
// 线程A
std::atomic<bool> flag(false);
...
flag = true;

// 线程B
...
while (!flage)); // 轮询机制（polling）
...
```

- 用`std::condition_variable`亦可以实现类似效果的线程同步
```
// 线程A
std::condition_variable cv;
std::mutex m;

bool flag(false); // 不再需要`std::atomic`
...
{
    std::lock_guard<std::mutex> g(m);  // 获得mutex
    flag = true;                       // 更改条件
    // destructor释放mutex
}
cv.notify_one(); // 通知线程B

// 线程B
{
    std::unique_lock<std::mutex> lk(m); // 获得mutex
    cv.wait(lk, [] { return flag; });
    // 释放mutx并阻塞线程B（原子操作），等待条件满足，当条件满足时，重新获得mutex（原子操作）
    // 通过lambda表达式优雅的解决了虚假唤醒问题
    ...
}
...
```

- 使用`std::future`和`std::promise`可以实现上述效果的线程同步
```
std::promise<void> p;
// 线程A
...
p.set_value();
...
// 线程B
...
p.get_future().wait();
...
```
- 从cppreference上看到的很好的例子
```
int main()
{
    std::istringstream iss_numbers{"3 4 1 42 23 -23 93 2 -289 93"};
    std::istringstream iss_letters{" a 23 b,e a2 k k?a;si,ksa c"};
 
    std::vector<int> numbers;
    std::vector<char> letters;
    std::promise<void> numbers_promise, letters_promise;
 
    auto numbers_ready = numbers_promise.get_future();
    auto letter_ready = letters_promise.get_future();
 
    std::thread value_reader([&]
    {
        // I/O operations.
        std::copy(std::istream_iterator<int>{iss_numbers},
                  std::istream_iterator<int>{},
                  std::back_inserter(numbers));
 
        //Notify for numbers.
        numbers_promise.set_value();
 
        std::copy_if(std::istreambuf_iterator<char>{iss_letters},
                     std::istreambuf_iterator<char>{},
                     std::back_inserter(letters),
                     ::isalpha);
 
        //Notify for letters.
        letters_promise.set_value();
    });
 
 
    numbers_ready.wait();
 
    std::sort(numbers.begin(), numbers.end());
 
    if (letter_ready.wait_for(std::chrono::seconds(1)) ==
            std::future_status::timeout)
    {
        //output the numbers while letters are being obtained.
        for (int num : numbers) std::cout << num << ' ';
        numbers.clear(); //Numbers were already printed.
    }
 
    letter_ready.wait();
    std::sort(letters.begin(), letters.end());
 
    //If numbers were already printed, it does nothing.
    for (int num : numbers) std::cout << num << ' ';
    std::cout << '\n';
 
    for (char let : letters) std::cout << let << ' ';
    std::cout << '\n';
 
    value_reader.join();
}
```

#####条款40: Use `std::atomic` for concurrency, `volatile` for special memory
- C++11在语言层面上引入了`std::atomic`，有了这些原子操作，无锁（lock-free）并发编程成为了可能。
- `std::atomic` is for data acccessed from multiple threads without using mutexes. It's a tool for writing concurrent software.

##### MISCS
- 只有单一资源轮候时，不会出现死锁；当有两个以上的资源轮候时，则有可能出现死锁。
- `std::lock`使用了避免死锁的算法
```
void f()
{
    ......
    // don't actually take the locks yet
    std::unique_lock<std::mutex> lock1(from.m, std::defer_lock);
    std::unique_lock<std::mutex> lock2(to.m, std::defer_lock);
 
    // lock both unique_locks without deadlock
    std::lock(lock1, lock2); 
    // Locks the given Lockable objects lock1, lock2, ..., lockn using a deadlock avoidance algorithm to avoid deadlock.
    ......
}
```

- C++11内存模型
http://stackoverflow.com/questions/6319146/c11-introduced-a-standardized-memory-model-what-does-it-mean-and-how-is-it-g
