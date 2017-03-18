- Erase-Remove是应用于STL容器的一种惯用法

- `std::remove`算法并不真正删除元素，而只是将元素重新排序：将所有不满足删除条件的元素置于容器的前部，所有需要被删除的元素置于容器的尾部，
返回值是一个指向第一个需要被删除的元素的迭代器，容器的尾部迭代器（end）不会失效。

- `std::erase`算法真正删除元素

- 将`std::remove`和`std::erase`结合起来，从而产生了Erase-Remove惯用法
```
template <typename T>
void remove(std::vector<T>& v, const T& item)
{
    v.erase(std::remove(v.begin(), v.end(), item), v.end()); // std::remove算法不会使v.end()失效
}

std::vector<int> v;
......
remove(v, 99); // 删除掉所有值等于99的元素
```

```
v.erase(std::remove_if(v.begin(), v.end(), [](int x){ return x == 99; }), v.end());
```
