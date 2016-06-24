##### Default Initialization (默认初始化)
- 当一个变量初始化时，如果没有提供初始化式，那么执行默认初始化
- 默认初始化的形式如下
 + `T object;`
 + `new T;`

##### Value Initialization (值初始化)
- 当一个变量初始化时，如果提供的是一个空的初始化式，那么执行值初始化
- 值初始化的形式如下
 + 'T();' 
 + 'new T();'
 + 'Class::Class(...) : member() {...'
 + 'T{};'
 + 'new T{};'
 + 'Class::Class(...) : member{} {...'
- 对于所有的标准容器（std::vector, std::list, etc.），如果我们创建该容器时候调用的是单个容器大小参数的构造函数，那么容器内所有元素将被值初始化。
如果调用resize()，那么同样对于新增加的元素也是被值初始化。
```
std::vector<int> v(3);  // value-initialization of each element, the value of each element is 0
```

##### Direct Initialization (直接初始化)
- 显式匹配最佳的构造函数去初始化对象
- 直接初始化的形式如下
 + `T object ( arg );`
 + `T object ( arg1, arg2, ... );`
 + `T object { arg };`
 + `T object { arg1, arg2, ... };`
 + `T ( other )`
 + `T ( arg1, arg2, ... );`
 + `static_cast< T >( other )`
 + `new T(args, ...)`
 + `Class::Class() : member(args, ...) {...	`
 + `[arg](){...`
- 对于lambda表达式的参数捕捉而言，如果使用了拷贝捕捉，那么参数是被直接初始化的
```
class C
{
public:
    C() {};
    explicit C(const C&) {}
};

void foo()
{
    C c1;
    auto f = [c1]() {}; // 此处使用直接初始化
    
    f();
}
```
- 如果构造函数被explicit修饰，那么该构造函数只能在直接初始化时调用，而拷贝初始化不能调用

##### Copy Initialization (拷贝初始化)
- 以另一个对象为蓝本，拷贝构造一个对象
- 拷贝初始化的形式
 + `T object = other;`
 + `T object = {other} ;`
 + `f(other)`
 + `return other;`	
 + `throw object;`
 + `catch (T object)`
 + `T array[N] = {other};`

##### List Initialization (列表初始化)
- direct-list-initialization
```
T object { arg1, arg2, ... };	(1)	
T { arg1, arg2, ... };	(2)	
new T { arg1, arg2, ... }	(3)	
Class { T member { arg1, arg2, ... }; };	(4)	
Class::Class() : member{arg1, arg2, ...} {...	(5)	
```

- copy-list-initialization
```
T object = {arg1, arg2, ...};	(6)	
function( { arg1, arg2, ... } ) ;	(7)	
return { arg1, arg2, ... } ;	(8)	
object[ { arg1, arg2, ... } ] ;	(9)	
object = { arg1, arg2, ... } ;	(10)	
U( { arg1, arg2, ... } )	(11)	
Class { T member = { arg1, arg2, ... }; };	(12)	
```
