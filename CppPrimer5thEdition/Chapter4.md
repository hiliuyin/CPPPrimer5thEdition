### **隐式类型转换（implicit cast）**

- 编译器在必要的时候，隐式地将类型转换规则应用到内置类型和类类型的对象上。

- 在下列情况下，将会发生隐式类型转换：
 + 在混合类型的表达式中，其操作数被转换为相同的类型；
 + 用作条件的表达式被转换为bool类型；
 + 用一个表达式初始化某个变量，或者将一个表达式赋值给某个变量，则该表达式被转换为该变量的类型；
 + 函数调用发生时，实参被转换为形参的类型。

C++为内置类型定义了一组转换规则，包括：

#### 算术转换
算术转换规则将二元操作符的操作数转换为同一类型，并且表达式的值也具有相同类型。
算术转换规则定义了一个类型转换层次，规定了操作数应该按照什么规则转换为表达式中最宽的类型。    

- 整型提升：
对于所有比 int 型小的类型，包括：char, unsigned/signed char，short，unsigned/signed short，
如果该类型的最大值可包含于int类型，则将会被提升到int类型，否则提升到unsigned int类型。
```
unsigned int x = 0xFF;
int y = -10;
if (x > y) // int converted to unsigned int

char flag = 0xAA; // signed or unsigned
if (flag != (unsigned int)0xAA) // char promoted to int, then converted to unsigned int
```

- 对于包含浮点类型和整型类型的混合运算表达式，则会先施行整型提升，再将提升后的值转换为浮点数类型。

- 浮点数的类型转换也有转换层次：float -> double -> long double
```
bool      flag;
char      cval;
short     sval;
unsigned short usval;
int       ival;
unsigned int   uival;
long      lval;
unsigned long  ulval;
float     fval;
double    dval;
3.14159L + 'a'; // promote 'a' to int, then convert to long double
dval + ival;    // ival converted to double
dval + fval;    // fval converted to double
ival = dval;    // dval converted (by truncation) to int
flag = dval;    // if dval is 0, then flag is false, otherwise true
cval + fval;    // cval promoted to int, then converted to float
sval + cval;    // sval and cval promoted to int
cval + lval;    // cval promoted to int, then converted to long
ival + ulval;   // ival converted to unsigned long
usval + ival;   // promotion depends on size of unsigned short and int
uival + lval;   // conversion depends on size of unsigned int and long
```

#### 指针转换
- 数组名在多数情况下会被隐式转换为指向数组元素类型的指针
```
extern void foo(int x[]);
int x[10];
foo(x); // int[10] converted to int*

extern void foo(int (*x)[5]);
int x[4][5];
foo(x);  // int[4][5] converted to int(*)[5]
```

- 例外情况如下：
 + 数组用作decltype关键字的参数时，取址运算符作用于数组名，sizeof运算符，typeid运算符，
 + 用数组名去初始化数组引用类型

- 整型常量 0 可以被转换为任何指针类型

- nullptr能转换成任意指针类型

- 指向任意非常量的指针可以被转换为void*类型

- 指向任意对象的指针可以被转换为const void*类型
 
#### bool类型转换：
- 算术值和指针值均可被转换为bool类型，如果算术值和指针值为 0, 则转换为 false；否则被转换为 true。
```
if (cp) /* ... */ // true if cp is not zero
while (*cp) /* ... */ // dereference cp and convert resulting char to bool
```

- bool类型可以被转换为算术类型，false 转换为 0；true 转换为 1。

#### 枚举类型转换：
- 枚举类型的对象和枚举类型的成员可被转换为整型类型，其转换结果可用于任何要求整型值的地方。
- 枚举类型转换为何种整型类型，取决于枚举类型的最大值。C++中按照 int -> unsigned int -> long -> unsigned long依次选择。

#### const转换：
- 当用非const对象去初始化const对象的引用时，非const对象被转换为const对象；
- 当用非const对象的地址或指向非const对象的指针去初始化指向const对象的指针时，亦发生const转换。
```
int i;
const int ci = 0;
const int &j = i; // ok: convert non-const to reference to const int
const int *p = &ci; // ok: convert address of non-const to address of a const
```

#### C++标准库也定义了一系列转换规则
重要类型转换如下：
```
while(cin > s) // isstream -> bool
string s = "hello world"; // const char* -> string
```

##### 显式转换
###### **`static_cast`**

 - 任何具有明确定义的类型转换，只要不包含底层const，都可以使用static_cast

 - static_cast对于编译器无法自动执行的类型转换也非常有用
例如，可以使用static_cast找回存在于void*指针中的值
```
void *p = &d;
double *dp = static_cast<double*>(p);
```

#### **`dynamic_cast`** 运行时类型识别

#### **`const_cast`**
 - const_cast只能改变运行对象的底层const
```
const char *pc;
char *p = const_cast<char*>(pc); // ok，但是通过p写值是undefined behavior
```
 - 只有const_cast能改变表达式的常量属性，使用其他形式的明明强制类型转换改变表达式的常量属性都将引发编译器错误
 - 同样，也不能用const_cast改变表达式的类型
```
const char *cp;
char *q = static_cast<char*>(cp); // error
static_cast<string>(cp); // ok
const_cast<string>(cp); // error
``` 

#### **`reinterpret_cast`**
 - `reinterpret_cast`通常为运算对象的位模式提供较低层次上的重新解释
```
int *ip;
char *pc = reinterpret_cast<char*>(ip);
```

 － `reinterpret_cast`本质上依赖于机器
```
using PF = void (*)(); // 等价于 typedef void (*PF)();
PF pf;
void *p1 = pf; // error
void *p2 = (void*)pf; // ok
void *p3 = static_cast<void*>(pf); // error
void *p4 = reinterpret_cast<void*>(pf); // ok
```

#### 旧式的强制类型转换
```
type (expr); // 函数形式
(type) expr; // C语言风格
```
