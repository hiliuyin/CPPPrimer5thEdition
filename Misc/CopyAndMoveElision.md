- 引用一段标准中对Copy/Move Elision的详细描述

> - When certain criteria are met, an implementation is allowed to omit the copy/move construction of a class object, even if the copy/move constructor and/or destructor for the object have side effects. In such cases, the implementation treats the source and target of the omitted copy/move operation as simply two different ways of referring to the same object, and the destruction of that object occurs at the later of the times when the two objects would have been destroyed without the optimization.123 This elision of copy/move operations, called copy elision, is permitted in the following circumstances (which may be combined to eliminate multiple copies):
>  + in a return statement in a function with a class return type, when the expression is the name of a non-volatile automatic object (other than a function or catch-clause parameter) with the same cvunqualified type as the function return type, the copy/move operation can be omitted by constructing the automatic object directly into the function’s return value
>  + in a throw-expression, when the operand is the name of a non-volatile automatic object (other than a function or catch-clause parameter) whose scope does not extend beyond the end of the innermost enclosing try-block (if there is one), the copy/move operation from the operand to the exception object (15.1) can be omitted by constructing the automatic object directly into the exception object
>  + when a temporary class object that has not been bound to a reference (12.2) would be copied/moved to a class object with the same cv-unqualified type, the copy/move operation can be omitted by constructing the temporary object directly into the target of the omitted copy/move
>  + when the exception-declaration of an exception handler (Clause 15) declares an object of the same type (except for cv-qualification) as the exception object (15.1), the copy/move operation can be omitted by treating the exception-declaration as an alias for the exception object if the meaning of the program will be unchanged except for the execution of constructors and destructors for the object declared by the exception-declaration.
>  + 123) Because only one object is destroyed instead of two, and one copy/move constructor is not executed, there is still one object destroyed for each one constructed.

- (N)RVO（(Named) Return Value Optimization）是Copy/Move Elision的一种，其条件可见上述第一条

- 当满足特定条件的时候，标准允许编译器实现进行一种优化，即不执行copy/move constructor，即使在copy/move constructor有side effects的情况下，这种优化也是允许的。
```
struct C {
  C() {}
  C(const C&) { std::cout << "A copy was made.\n"; } // copy constructor有side effects：输出字符串到std::cout
};

C f() {
  return C(); // (1) 调用copy constructor将C()拷贝到返回值（临时对象）
}

int main() {
  std::cout << "Hello World!\n";
  C obj = f(); // (2) 调用copy constructor将f()的返回值（临时对象）拷贝到obj
  return 0;
}

// 对于上述程序，根据编译器和环境不同，下列三种结果皆有可能
Hello World!
A copy was made. (1)
A copy was made. (2)
-------------------------  
Hello World!
A copy was made. (1)或(2)
-------------------------
// 将(1)和(2)全部优化掉
Hello World!
```

- in a return statement in a function with a class return type, 
when the expression is the name of a non-volatile automatic object (other than a function or catch-clause parameter) with the same cv-unqualified type as the function return type, 
the copy/move operation can be omitted by constructing the automatic object directly into the function’s return value

- 对于(N)RVO，`std::move`会阻止Move Elision
 + 返回值
```
std::string foo_1()
{
    std::string s;
    ......
    return s; // elide move constructor
}

std::string foo_2()
{
    std::string s;
    ......
    return std::move(s); // will NOT elide move constructor
}

std::string s1 = foo_1();
std::string s2 = foo_2();
```

- (N)RVO的发生场景
 + 函数按pass-by-value返回值
 + 函数按pass-by-value传递给参数一个临时对象
```
class Thing {
public:
  Thing();
  ~Thing();
  Thing(const Thing&);
};
Thing f() {
  return Thing();
}

Thing t2 = f(); // pass-by-value, copy elision

void foo(Thing t);
foo(Thing()); // pass a temporary object, copy elision
```

- Pass-by-value函数返回值的情况下，编译器的选择
 + 是否可以应用(N)RVO
 + 如果不能满足copy/move elision的条件，会去看move constructor是否可行
 + 如果move constructor不可行，那么调用copy constructor
```
std::vector<int> return_vector(void)
{
    std::vector<int> tmp {1,2,3,4,5};
    return tmp;
}

std::vector<int> rval_ref = return_vector(); // 编译器会按照上述描述执行
```

- 编译器即使执行了Copy/Move Elision，并不代表Copy/Move Constructor可以不存在，在语法上Copy/Move Constructor必须存在并可见
