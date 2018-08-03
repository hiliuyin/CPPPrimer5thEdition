##### 标准库类型`std::string`

头文件不应该包含using声明

直接初始化和拷贝初始化

string s5 = "hiya"; // 拷贝初始化

string s6("hiya"); // 直接初始化

string s7(10, 'c'); // 直接初始化

string s8 = string(10, 'c'); // 拷贝初始化

size函数返回string 对象的长度（即string对象中字符的个数）

    size_type size() const;

string::size_type类型

string类及其他大多数标准库类型都定义了几种配套的类型，这些配套类型体现了标准库类型与机器无关的特性，

类型size_type即是其中的一种，它位于对应的类空间。

 

可以通过auto或decltype来推断size_type的类型：auto len = line.size();

 

由于历史原因，也为了和C兼容，C++中的字符串字面值并不是标准库类型string的对象。

 

使用C++版本的C标准库头文件

name.h -> cname （命名空间std）

 

使用基于范围的for语句

for (declaration : expression)

statement

expression部分是一个对象，用于表示一个序列

declaration部分负责定义一个变量，该变量将被用于访问序列中的基础元素

每次迭代，declaration部分的变量会被初始化为expression部分的下一个元素值

string str("hello world");

for (auto c : str)

std::cout << c << endl;

 

for (auto &c : str)

c = toupper(c);

 

for (decltype(str.size()) i = 0;

i < str.size();

++i)

str[i] = toupper(str[i]);

 

标准库类型vector

vector是模版而非类型，由vector生成的类型必须包含vector中元素的类型，例如：vector<int>

 

vector能容纳绝大多数类型的对象作为其元素，但是因为引用不是对象，所以不存在包含引用的vector

 

定义和初始化vector对象

默认初始化

vector<T> v1; // 默认初始化，空vector

 

直接初始化

vector<T> v2(v1); // 直接初始化，v2中包含有v1所有元素的副本

 

拷贝初始化

vector<T> v2 = v1; // 拷贝初始化，等价于v2(v1)

 

创建指定数量的元素

vector<T> v3(n, val); // v3中包含了n个重复的元素，每个元素的值都是val

 

列表初始化vector对象

c++11标准提供了为vector对象的元素赋初值的方法，即列表初始化

vector<T> v5{a, b, c ...}; // 列表初始化，v5包含了初始值个数的元素，每个元素被赋予相应的初始值

vector<T> v5 = {a, b, c ...}; // 等价于v5{a, b, c ...}

 

vector<string> articles = {"a", "an", "the" };

vector<string> v1{"a", "an", "the"};

 

值初始化（value-initialized）

通常情况下，可以只提供vector对象容纳的元素数量而不用略去初始值

此时库会创建一个值初始化元素初值，并把它赋给容器中的所有元素，这个初值由vector对象中元素的类型决定

如果vector对象的元素是内置类型，则元素初始值自动设为0

如果是某种类类型，则元素由类默认初始化

vector<T> v4(n); // v4中包含了n个重复地执行了值初始化的对象

 

vector<int> ivec(10); // 10个元素，每个都初始化为0

vector<string> svec(10); // 10个元素，每个都是空string对象

 

列表初始化还是元素数量

确认无法执行列表初始化后，编译器会尝试用默认值初始化vector对象

vector<int> v1(10); // 值初始化，10个元素，每个元素的值都是0

vector<int> v2{10}; // 列表初始化，1个元素，该元素的值为10

 

vector<int> v3(10, 1); // 10个元素，每个元素的值都是1

vector<int> v4{10, 1}; // 2个元素，值分别是10和1

 

vector<string> v5{"hi"}; // 列表初始化，一个元素

vector<string> v6("hi"); // error

vector<string> v7{10}; // 值初始化，10个空string元素

vector<string> v8{10, "hi"}; // 10个string元素，每个值为"hi"

 

vector对象能高效增长

C++标准要求vector对象应该能在运行时高效快速地添加元素

除非所有元素的值都一样，一旦元素的值有所不同，更有效的办法是先定义一个空的vector对象，再在运行时向其中添加元素

 

迭代器（Iterator）

迭代器提供了对对象的间接访问

迭代器分为有效和无效，有效的迭代器或者指向某个元素，或者指向容器中尾元素的下一位置；其他情况都属于无效

 

begin()返回只想第一个元素的迭代器，end()返回指向尾元素的下一位置（one past the end）的迭代器

如果容器为空，则begin()和end()返回的是同一个迭代器，都是尾后迭代器

vector<int> v;

auto b = v.begin(), e = v.end();

 

string s;

for (auto it = s.begin(); it != s.end(); ++it)

*it = toupper(*it);

 

迭代器的类型是 iterator 和 const_iterator

iterator的对象可读可写，const_iterator能读取但是不能修改

const_iterator既可用于常量对象，也可用于非常量对象

iterator不可用于常量对象

const vector<int> cv;

vector<int>::const_iterator citer = cv.begin(); // ok

vector<int>::iterator iter = cv.begin(); // error

 

begin和end运算符返回的具体类型由对象是否是常量决定，如果对象是常量，返回const_iterator；如果不是常量，返回iterator

vector<int> v;

auto b1 = v.begin(), e1 = v.end(); // b1和e1的类型是vector<int>::iterator

 

const vector<int> cv;

auto b2 = cv.begin(), e2 = cv.end(); // b2和e2的类型是vector<int>::const_iterator

 

C++11引入了两个新函数，分别是cbegin()和cend()，返回值始终都是const_iterator

vector<int> v;

auto it = v.cbegin(); // it的类型是vector<int>::const_iterator

 

迭代器运算（iterator arithmetic）

template <typename T>
bool binary_search(vector<T> &v, const T &target)
{
    auto beg = v.begin();
    auto end = v.end();

    auto mid = beg + (end-beg)/2; // attention
    while (mid != end && *mid != target)
    {

        if (target < *mid)

            end = mid;

        else

            beg = mid+1;

        mid = beg + (end-beg)/2;

    }

   

    return mid!=end;

}

 

// iter1 - iter2：名为difference_type的有符号整型数

// 参与关系运算符运算的两个迭代器必须合法且指向同一个容器的元素（或者尾元素的下一位置）

 

  
