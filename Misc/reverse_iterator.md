- reverse_iterator和iterator的关系
  + 通过reverse_iterator::base()可以得到iterator
 ```
 std::reverse_iterator(it).base() == it
 ```
 + reverse_iterator::base和iterator之间存在着偏移
 ```
 &*(reverse_iterator(i)) == &*(i - 1)
 &*(rit.base() - 1) == &*rit
 ```
 ![](http://upload.cppreference.com/mwiki/images/3/39/range-rbegin-rend.svg)
 
- 对于stl容器的有些函数，只接受iterator作为参数
```
iterator erase( iterator pos ); // (until C++11)
iterator erase( const_iterator pos ); //(since C++11)

iterator erase( iterator first, iterator last ); // (until C++11)
iterator erase( const_iterator first, const_iterator last ); // (since C++11)
```

- 当用reverse_iterator遍历容器时，当调用只接受iterator为参数的函数时，需要特别注意
```
// 删除vector里面的所有非零元素
std::vector<int> v = {-1, 0, 1, 2, 3, 4, 5, 6};
for (auto rit = v.rbegin(); rit != v.rend(); ) {
    if (*rit != 0) {
        rit = std::reverse_iterator<std::vector<int>::iterator>(v.erase(std::next(rit).base()));
    } else {
        ++rit;
    }
}

// 优雅高效的方法
// remove-erase idiom
v.erase(std::remove_if(v.begin(), v.end(), [](int x){return x != 0;}), v.end());
```
