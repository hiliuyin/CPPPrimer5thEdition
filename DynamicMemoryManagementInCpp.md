#### 内存分配
##### `new operator` (new操作符)
- new操作符是语言内置的，它总是完成如下两件事情，而且我们不能以任何方式来改变其行为:
 + 分配足够的内存以便容纳所需类型的对象, 调用一个函数来完成必需的内存分配，你能够重写或重载这个函数来改变它的行为。
   new操作符为分配内存所调用函数的名字是 `operator new`
 + 对于内建类型，要么无初始值，要么使用初始化式中的值初始化对象；
   对于类对象，调用构造函数初始化内存中的对象；
``` 
int* p1 = new int;  // default-initialization, 无初始化值
int* p2 = new int(); // value-initialization, zero-initialization
int* p3 = new int(100);  // direct-initialization, 100为初始值
int* p4 = new int(a*b);  // 运行时计算a*b的结果做为初始值
```
```       
string* ps = new string("Memory Management"); // direct-initialization
// 相当于执行了下面的伪代码：
void* memory = operator new(sizeof(string)); // 调用operator new得到未经处理的内存
call string::string("Memory Management") on *memory; // 调用构造函数为string对象初始化内存中的对象
string *ps = static_cast(memory);    // ps指针指向新的对象
```

- 很多C++程序员会这样写代码, 这样的代码相信在很多源码中都能够找到，写这个代码的程序员想当然的使用new operator。
```
void foo() {
    string *ps = new string; // 如果分配失败，会抛出std::bad_alloc异常
    if (ps == nullptr) // error
        return; // cannot reach here
}
```
上述代码中，如果ps分配失败的话，有两种可能:
如果new_handler（后面会讲到）为nullptr的话，它会抛出`std::bad_alloc`异常，而并不会返回nullptr;
如果new_handler不为nullptr的话，new_handler会被反复的调用，直到它完成了它的任务（后面会讲到）。
曾经错误的以为，上述代码修改成如下就能正常工作：
```
void foo() {
    string *ps = new (std::nothrow) string; 
    // nothrow版本的operator new被调用，只能保证operator new不会抛出异常
    // 但是无法保证在ctor被调用期间不抛出异常
    if (ps == nullptr)  // error, 这个判断是没有任何意义的
        return;
}
```

- C++使用直接初始化（Direct-Initialization）语法规则来初始化动态创建的对象

##### operator new (函数)
- 在<new>头文件中，标准C++中定义了operator new的三种重载形式:
 1. `void* operator new(std::size_t size) throw(std::bad_alloc); ` 失败的话，会抛出std::bad_alloc异常
 2. `void* operator new(std::size_t size, const std::nothrow_t&) throw();` 失败的话，不抛出异常，而是直接返回NULL
 3. `void* operator new(std::size_t size, void* ptr) throw();` **placement new**, 不抛出异常
```
try {
    void* rawMemory = operator new(sizeof(string)); // 调用<1>, 失败则抛出异常
}
catch (std::bad_alloc &) {
    // do something
}
```

```
void* rawMemory = operator new(sizeof(T), std::nothrow); // 调用<2>
if (rawMemory == nullptr)  // 失败则返回nullptr
    std::cerr << "Unable to satisfy request for memory\n" << std::endl;
```
- operator new函数的返回值类型为void\*，参数std::size_t用于确定分配多少内存。
- 我们可以重载operator new函数，但是第一个参数类型必须是std::size_t。
- operator new函数和C中的malloc函数一样，只是负责分配内存，而对构造函数一无所知。

- new表达式中紧跟着new的括号内的实参将传递给operator new函数size_t后面的形参
- new表达式中括号后的类型的sizeof运算的结果作为实参传递给operator new函数的size_t形参
`string *ps = new (std::nothrow) string;`

##### placement new
<3> 是一个特殊的operator new函数，被称为placement new
它用于在raw内存（已经被分配但是尚未处理的内存）中构造一个对象。
调用placement new通常通过如下格式：
`new (place_address) type-specifier`
placement new函数通常定义很简单，经典的定义就是返回一个地址:
`void* operator new(std::size_t, void* location) { return location; }`

```
class T {
public:
    static void* operator new(std::size_t, void* location) { return location; }
};

T* buildTInBuffer(void* buffer) {
    return new (buffer) T;  // 调用placement new函数，在buffer处创建对象
}
void foo() {
    void* pb = malloc(sizeof(T)); // 分配raw内存                  
    T* pt = buildTInBuffer(pb); // 调用placement new函数
}
```

##### **placement delete**
与placement new表达式相匹配的delete表达式
一般性而言，placement new 指带任意额外的参数的operator new，
placement delete则和它配对，完全匹配这些额外的参数。
```
class T {
public:
    static void* operator new(std::size_t size, std::ostream& log_stream) throw(std::bad_alloc);
    static void operator delete(void* p, std::ostream& log_stream) throw(); // 匹配的placement delete
};
T* p = new (std::cerr) T;
```

如果在调用ctor的时候抛出异常，那么如何将已经分配好的内存释放掉呢？
运行期系统会寻找参数类型和个数都与operator new相同的operator delete
因此，对应的operator delete函数如下：
`void operator delete(void*, std::ostream&) throw();`
但是，如果没有该函数，则抛出异常会引起内存泄露。

- placement delete只有在对应的placement new调用而触发的构造函数抛出异常的时候被调用;
- 正常情况下，delete运算符只会调用通用的operator delete
```
class A;
char* buf = new char[sizeof(A)]; // new operator
A* pa = new (buf) A; // placement new
delete[] buf; // 注意delete的是buf
```    

```
struct T {
    // static void operator delete(void* p) {}   (1)
    static void* operator new(size_t n, void* addr) { printf("placement new\n"); return addr; } 
    static void operator delete(void* p, void* addr) { printf("placement delete\n"); }
};

int main()
{
    int x;
    T* pt = new (&x) T;  // ok, 调用placement new
    delete pt;   // error, 因为T中的placement delete会hide全局的operator delete，因此编译器找不到对应的operator delete
    pt->~T();   // ok, 显式的调用析构函数
    T::operator delete(pt, &x);  // ok, 显式的调用placement delete
    return 0;
}
```

##### 关于 new operator 和 operator new 之间区别的总结:
- 你想在堆上建立一个对象，应该用new操作符, 它既分配内存又为对象调用构造函数。
- 如果你仅仅想分配内存，就应该调用operator new函数；它不会调用构造函数。
- 如果你想定制自己的在堆对象被建立时的内存分配过程，你应该写你自己的operator new函数，然后使用new操作符，new操作符会调用你定制的operator new。
- 如果你想在一块已经获得指针的内存里建立一个对象，应该用placement new。

##### new_handler:
当operator new无法满足需求时，它会在抛出异常或者返回nullptr之前先调用一个client专属的错误处理函数，此函数即为 new_handler。
C++并不支持class专属的new_handler，所以我们需要让每个class提供自己的set_new_handler和operator new。
在`<new>`中，C++标准在std命名空间中有如下声明：
```
namespace std
{
    typedef void (*new_handler)();
        // If you write your own error handler to be called by @c new, it must be of this type.
    new_handler set_new_handler(new_handler) throw(); 
        // Takes a replacement handler as the argument, returns the previous handler.
}
```
```
void noMoreMemory() {
   std::cerr << "No more memory to be allocated" << std::endl;
}
void foo() {
    std::set_new_handler( noMoreMemory );
    int *p = new int[100000000];
}
```

- 当operator new函数无法满足内存需求时，如果new_handler不是nullptr，那么它会反复的调用new_handler，直至有足够的内存可以分配为止。
因此，new_handler应该完成以下的事情之一:
 + 让更多的内存可用，例如: 可以事先预留好一块内存，在new_handler被调用时释放这块内存
 + 安装一个不同的new_handler
 + 卸载new_handler
 + 抛出一个型别为std::bad_alloc或其派生型别的exception
 + 直接调用abort()或者exit()

`new_handler set_new_handler(new_handler p) throw();` 的返回值是之前的new_handler函数指针。

##### 一种基于RAII手法的new_handler资源的实现：
```
class NewHandlerHolder : private Noncopyable
{
public:
    NewHandlerHolder(std::new_handler p) : current_handler_(p) {}
    ~NewHandlerHolder() { ::set_new_handler(current_handler_); }
private:
    std::new_handler current_handler_;
};
template <typename T>
class NewHandlerSupport
{
public:
    static std::new_handler set_new_handler(std::new_handler p);
    static void* operator new(std::size_t size) throw();

private:
    static std::new_handler current_handler_;
};
std::new_handler NewHandlerSupport::current_handler_ = NULL;

template <typename T>
std::new_handler NewHandlerSupport::set_new_handler(std::new_handler p)
{
    std::new_handler old_handler = current_handler_;
    current_handler_ = p;
    return old_handler;
}

template <typename T>
void* NewHandlerSupport::operator new(std::size_t size) throw()
{
    NewHandlerHolder nh(std::set_new_handler(current_handler_));
    return ::operator new(size);
}
```

- C++标准中对operator new 和 operator delete的规定:
 + 即使用户要求的是0大小的内存，operator new也应该传回的一个合法指针；
 + 如果分配失败，operator new会反复尝试配置内存，并且在每次失败后去调用new_handler函数，
 + 只有当new_handler为nullptr时，operator new才会抛出异常或者返回nullptr。
 + 使用operator delete去删除一个nullptr指针是安全的。
```
if(p != nullptr)  // 没必要
    delete p;
```

#### 内存释放
##### delete operator(delete操作符)
- 和new operator（new操作符）对应，同new操作符一样，它总是完成如下两件事情，而且我们不能以任何方式去改变其行为。
 1. 调用析构函数析构对象；
 2. 销毁new操作符分配的内存
```
ps->~string(); // call the object's destructor
operator delete(ps); // deallocate the memory the object occupied
```

- 在<new>头文件中，标准C++中规定的operator delete的三种重载形式，对应与operator new的三种重载形式:
```
 void operator delete(void* ptr) throw();
    // void * operator new(std::size_t size) throw( std::bad_alloc );
 void operator delete(void* ptr, const std::nothrow_t&) throw(); 
    // void * operator new(std::size_t size, const std::nothrow_t&) throw();
 void operator delete (void* ptr, void*) throw();
    // void * operator new(std::size_t size, void* ptr)
```
- 如果被删除的对象的基类中遗漏了virtual destructor，那么operator delete将无法正确运行 (Undefined behavior)

####创建数组对象
- 在<new>头文件中，标准C++中规定的operator new[]的三种重载形式:
```
void* operator new[](std::size_t size) throw(std::bad_alloc);
void* operator new[](std::size_t size, const std::nothrow_t&) throw();
void* operator new[](std::size_t size, void* ptr) throw();
```

在<new>头文件中，标准C++中规定的operator delete[]的三种重载形式，同样对应与operator new[]的三种重载形式:
```
void operator delete[](void* ptr) throw();
void operator delete[](void* ptr, const std::nothrow_t&) throw();
void operator delete[](void* ptr, void*) throw();
```

- 我们永远都不要以多态的方式来处理数组
C++规定：传递给函数operator delete[]的指针的静态类型和动态类型必须是相同的。
通过base class指针删除一个由derived class objects构成的数组，其结果未定义。
```
struct B
{
    virtual ~B() {}
    void operator delete[](void* p, size_t n) { ::operator delete(p); }
};

struct D : B
{
    void operator delete[](void* p, size_t n) { ::operator delete(p); }
};

int main()
{
    B* pb = new D[100];
    delete [] pb;   // Undefined behavior

    D* pd = new D[100];
    delete [] pd;   // ok, call D::operator delete[]

    return 0;
}
```

- new operator[]运算符会针对数组中的每个类对象调用其default constructor，对于内建型别则无法进行初始化(default initialization)
```
int* p = new int[1000];  // 不会初始化, default initialization
string * ps = new string[10];   // 针对每个数组元素调用string default constructor, default initialization
```

- 多维数组的动态分配和释放
```
int (*p)[1024] = new int[4][1024]; // 在C和C++中，多维数组实质上是数组的数组
```

- 对于动态分配的数组，只有第一维可以用运行时计算的表达式来指定，其它维数必须是编译时刻就确定的常量值
```
extern int getDim();
int (*p)[1024] = new int[getDim()][1024]; // ok
int **p = new int[4][getDim()];  // error
```

#### 常量对象的动态分配和释放
```
const int* p = new const int(1024); // direct initialization
delete p;
```
- 常量对象必须初始化，因此我们无法用new动态创建内建型别的const数组，只能动态创建类对象的const数组（default ctr）
- 用new表达式返回的值作为初始值的指针必须是一个指向const类型的指针
```
const int* p = new const int[100]; // error
const int* p = new int[100]; // ok
const A* pa = new const A[10]; // ok, A的default constructor被隐式调用做为初始化值
struct B
{
private:
    B() {};
};
int main()
{
    const B* p = new const B[10]; // error, B的default ctor为private
    delete p;
    return 0;
}
```

#### 注意事项
- 我们通常应该显式的将函数operator new和operator delete声明为静态函数，它们永远都不能是非静态函数。
即使没有被显式的使用static来进行声明，它们始终还是静态函数。
```
class T {
public:
    static void* operator new(std::size_t) throw( std::bad_alloc );
    static void operator delete(void*) throw();
};
```

- 注意成员operator new和operator delete的作用域: 动态分配对象所在的作用域并不重要，其类型决定了调用的函数。
```
int* pi = new int; // ::operator new
T* pt = new T; // T::operator new
```

- operator delete 和 operator delete[]的重载形式
```
  struct B
  {
      virtual ~B() {}
      void operator delete[](void* p) { ::operator delete[](p); }   // ok, usual
      void operator delete[](void* p, size_t) { ::operator delete[](p); }  // ok
  };
  struct D : B
  {
      void operator delete[](void* p) { ::operator delete[](p); }   // ok, usual
      void operator delete[](void* p, size_t) { ::operator delete[](p); }  // ok
  };
```

- 如果在类中定义了operator new(或者operator new[])和operator delete(或者operator delete[])中的任意一个函数，那么一定要同时定义另外一个函数。
 + 即总是成对提供operator new和operator delete函数。
 + operator new和 operator delete必须一并写出，使它们能够共享相同的假设。
 + 当我们定义了类专属的operator new函数进行某种特殊的分配操作，那么编译器可能会需要operator delete的重载，即使实际上从来也不会调用它。

- 注意区分 指针的生命期 和 指针指向对象的生命期 之间的区别:
```
int* p = new int(1000); // p生命期和它是 自动对象，局部静态对象 还是 全局对象 有关；
delete p; // 动态int对象的生命期终止，但是p依然存在
```

#### QUIZ 1
```
void *p1 = new char[10*sizeof string];
void *p2 = operator new(10*sizeof string);
void *p3 = operator new[](10*sizeof string);
```
以上三条语句有什么区别？
`new operator[], operator new() 和 operator new[]`

```
string* ptring = static_cast<string*>(p1);
new (p1) string("Hello, world!");  // placement new()
p1->string::string("Hello,world!") // error: ctor不可以用.或者->来调用，需要调用的时候要靠placement new operator;
                                             dtor只能用.或者->来调用，如果不能用delete operator的话
```
以上两条语句有什么区别？

```
char* pc = static_cast<char*>(p1);
delete p1;                                // error
delete []p1;                              // ok
```
以上两条语句的效果有区别么？并请写出，p2、p3的内存释放语句
```
operator delete(p2);
operator delete[](p3);
```

#### QUIZ 2
- 写一个类bar，要求bar的对象只能从堆上分配，包括对象数组。

首先，这个题考查了对ctor和dtor的理解：
一个对象分配在栈上的途径是直接构造或者拷贝构造在栈上，退出函数的时候还需要调用dtor来析构对象，也就是说，构造和析构两个语义缺一不可！
既然如此，那么就可以用access control来破坏这个语义需求：让ctor或者dtor不可被访问，然后提供相应的Create或者Destroy方法来补偿缺失的语义！但是到底应该non-public哪个呢？是ctor还是dtor呢？到底应该private还是应该protected呢？

显然，很多人会想到简单的把dtor给non-public了，理由很简单也很有效，那就是dtor只有一个！不过这很快就会带来一个问题，因为new[] operator需要访问dtor（后面会解释原因），单纯的non-public一个dtor会导致申请对象数组失败。

既然简单的把dtor限制住会带来麻烦，那么就把ctor给限制住吧，虽然这样麻烦了一点，谁让出题的人bt到要申请数组呢！
但实际上，限制ctor也没有那么简单，首先一个就是原题并没有说明bar的作用，因此很多人可能不会考虑到copy ctor的影响，
而如果没有显式的声明copy ctor，编译器会合成一个public inline的copy ctor，这将导致我们的需求出现缺口，
因为我们可以先从堆上new一个对象，然后拷贝构造给栈上的一个对象，反正dtor是可用的，这样并没有语法错误；
除了copy ctor的影响，还有一个问题就是default ctor也是个麻烦事，如果一个类提供了任何一个ctor，编译器就不会合成一个default ctor，
而如果这个类没有提供一个可以无参数调用的ctor（无参数或者所有的参数都有默认值），new一个数组也会导致失败，
因为new[] operator不能带初始化参数（不考虑placement new）！

关于ctor和dtor还有一个问题在于，non-public到底应该处理成proteced还是private呢，这取决于对类继承的需要和理解，
由于派生类需要访问基类的某个ctor以及dtor，因此private将阻碍类继承！

正因为有了申请对象数组的要求，单纯的将dtor给non-public是不可取的，
那么就选择将所有的ctor包括copy ctor以及default ctor都给non-public了吧，
然后提供一对static成员函数分别创建单个对象和数组（即所谓的工厂方法），如下:
```
class bar {
    bar() {}
    bar(const bar& rhs) {}
    /* 由于dtor不是non-public，因此任何ctor都不能放过 */
public:
    static bar *CreateSingle() { return new bar(); }
    static bar *CreateArray(size_t n) { return new bar[n]; }
    void Init(/*...*/) { /*initialize the bar object*/ }
};
```
基本上，这样已经可以很好的运作的

下面我们来考虑更深远一点的问题，嗯，也就是比较虚一点的问题！
很显然，由于dtor是public的，任何一个ctor的遗漏都会导致需求的失败；
另外，CreateXXXX这种接口也导致了分配/回收的不对称，
特别的，当它们的实现被当作黑盒的时候，到底应该用delete还是delete[]回收或者其他形式的回收可能只能被文档来约束了。
由于dtor是public的，一个bar*指针既可以被delete也可以被delete[]，这有时候也是错误的根源！

要解决这两个问题，应该怎么办？
比较简单的做法是把dtor也给non-public了，然后提供一对回收操作DestroyXXXX来析构和回收对象！
嗯，这里面又有一个无聊的细节了，DestroyXXXX应该是static成员函数还是普通成员函数呢？
大部分人很快就能发现，这其实无所谓，都可以，CreateXXXX只能是static成员函数而DestroyXXXX却不一定必须。
当然，保持一致性，那就干脆也搞成static成员函数吧，把指针显式传入DestroyXXXX函数！
接下来就会有一个不见得被所有人能想到的问题了，那就是这个指针应该是const bar*还是bar*呢，
很多人可能习惯的就写成了bar*，这就会导致下列代码不能通过，必须使用const_cast才可以
```
const bar *p = bar::CreateSingle();
bar::DestroySingle(p);  // 错误，必须写成bar::DestroySingle((bar*)p);
```
原因在于bar*可以适应const bar*参数，但是反过来不可以！
有了上面的讨论，得到的bar设计为
```
class bar {
    ~bar() {}
    bar() {}
    bar(const bar& rhs) {}
    /*为了保证bar的用户使用CreateXXXX来创建对象，还是应该不放过所有ctor*/

public:
    static bar *CreateSingle() {return new bar();}
    static bar *CreateArray(size_t n) {return new bar[n];}
    static void DestroySingle(const bar *p) {delete p;}
    static void DestroyArray(const bar *p) {delete[] p;}
    /*注，如果DestroyXXXX是non-static成员函数，一定要是const成员函数*/

    void Init(/*...*/) {/*initialize the bar object*/}
};
```
类似bar *和const bar*的讨论，如果DestroyXXXX是non-static成员函数，那么应该是const成员函数

最后，来一点吹毛求疵，bar的用户可能会使用CreateSingle来创建对象却使用DestroyArray来释放，
或者反过来使用CreateArray来创建却使用DestroySingle来释放，编译器并不能检查出来这个错误，怎么办？
下面这样不是一个完美的方案，或者说没有很完美的方案了
```
class bar {
    ~bar() {}
    bar() {}
    bar(const bar& rhs) {}
    /*为了保证bar的用户使用CreateXXXX来创建对象，还是应该不放过所有ctor*/

public:
    static bar *Create(size_t n) {return new bar[n];}
    static void Destroy(const bar *p) {delete[] p;}
    /*注，如果Destroy是non-static成员函数，一定要是const成员函数*/

    void Init(/*...*/) {/*initialize the bar object*/}
};
```
也就是说，即使申请一个bar对象，我们也采用new[]的方式，这样对返回值的使用并没有什么影响，创建和回收也已经匹配了，
唯一的缺点就是创建单个bar对象的时候也会纪录创建的对象个数，这带来了一定的空间损失！

最后，补充一下为什么dtor为non-public的时候new[]会失败：
new bar[n]的语义是先分配足够大的内存，然后构造n个bar对象并且返回首地址。
如果构造到m个（m <= n）的时候遇到了一个异常，则需要将已经构造好的所有对象都析构掉然后回收内存最后rethrow该异常通知程序员。
因此，new[]是需要生成代码来调用dtor的！
