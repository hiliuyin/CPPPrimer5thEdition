#####条款23: 理解`std::move`和`std::forward`
- `std::move`和`std::forward`都是模板函数，`std::move`支持隐式实例化，`std::forward`必须显式实例化
- `std::move`的一种实现
```
// TEMPLATE FUNCTION move
template<class _Ty> inline
	constexpr
	typename remove_reference<_Ty>::type&&
		move(_Ty&& _Arg) _NOEXCEPT
	{	// forward _Arg as movable
	    return (static_cast<typename remove_reference<_Ty>::type&&>(_Arg));
	}
```
- `std::forward`的一种实现
```
// TEMPLATE FUNCTION forward
template<class _Ty> inline
	constexpr
	_Ty&& forward(
		typename remove_reference<_Ty>::type& _Arg) _NOEXCEPT
	{	// forward an lvalue as either an lvalue or an rvalue
	    return (static_cast<_Ty&&>(_Arg));
	}

template<class _Ty> inline
	constexpr
	_Ty&& forward(
		typename remove_reference<_Ty>::type&& _Arg) _NOEXCEPT
	{	// forward an rvalue as an rvalue
	    static_assert(!is_lvalue_reference<_Ty>::value, "bad forward call");
	    return (static_cast<_Ty&&>(_Arg));
	}
```
