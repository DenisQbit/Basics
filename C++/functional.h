// functional standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _FUNCTIONAL_
#define _FUNCTIONAL_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#include <exception>
#include <tuple>
#include <typeinfo>
#include <xmemory>
#include <xstddef>
#if _HAS_CXX17
#include <memory>
#include <unordered_map>
#endif // _HAS_CXX17
#ifdef __cpp_lib_concepts
#include <compare>
#endif // __cpp_lib_concepts

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
// plus, minus, and multiplies are defined in <xstddef>

template <class _Ty = void>
struct divides {
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _FIRST_ARGUMENT_TYPE_NAME;
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _SECOND_ARGUMENT_TYPE_NAME;
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _RESULT_TYPE_NAME;

    _NODISCARD constexpr _Ty operator()(const _Ty& _Left, const _Ty& _Right) const {
        return _Left / _Right;
    }
};

template <class _Ty = void>
struct modulus {
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _FIRST_ARGUMENT_TYPE_NAME;
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _SECOND_ARGUMENT_TYPE_NAME;
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _RESULT_TYPE_NAME;

    _NODISCARD constexpr _Ty operator()(const _Ty& _Left, const _Ty& _Right) const {
        return _Left % _Right;
    }
};

template <class _Ty = void>
struct negate {
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _ARGUMENT_TYPE_NAME;
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _RESULT_TYPE_NAME;

    _NODISCARD constexpr _Ty operator()(const _Ty& _Left) const {
        return -_Left;
    }
};

// equal_to, not_equal_to, greater, less, greater_equal, and less_equal are defined in <xstddef>

template <class _Ty = void>
struct logical_and {
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _FIRST_ARGUMENT_TYPE_NAME;
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _SECOND_ARGUMENT_TYPE_NAME;
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef bool _RESULT_TYPE_NAME;

    _NODISCARD constexpr bool operator()(const _Ty& _Left, const _Ty& _Right) const {
        return _Left && _Right;
    }
};

template <class _Ty = void>
struct logical_or {
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _FIRST_ARGUMENT_TYPE_NAME;
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _SECOND_ARGUMENT_TYPE_NAME;
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef bool _RESULT_TYPE_NAME;

    _NODISCARD constexpr bool operator()(const _Ty& _Left, const _Ty& _Right) const {
        return _Left || _Right;
    }
};

template <class _Ty = void>
struct logical_not {
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _ARGUMENT_TYPE_NAME;
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef bool _RESULT_TYPE_NAME;

    _NODISCARD constexpr bool operator()(const _Ty& _Left) const {
        return !_Left;
    }
};

template <class _Ty = void>
struct bit_and {
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _FIRST_ARGUMENT_TYPE_NAME;
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _SECOND_ARGUMENT_TYPE_NAME;
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _RESULT_TYPE_NAME;

    _NODISCARD constexpr _Ty operator()(const _Ty& _Left, const _Ty& _Right) const {
        return _Left & _Right;
    }
};

template <class _Ty = void>
struct bit_or {
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _FIRST_ARGUMENT_TYPE_NAME;
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _SECOND_ARGUMENT_TYPE_NAME;
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _RESULT_TYPE_NAME;

    _NODISCARD constexpr _Ty operator()(const _Ty& _Left, const _Ty& _Right) const {
        return _Left | _Right;
    }
};

template <class _Ty = void>
struct bit_xor {
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _FIRST_ARGUMENT_TYPE_NAME;
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _SECOND_ARGUMENT_TYPE_NAME;
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _RESULT_TYPE_NAME;

    _NODISCARD constexpr _Ty operator()(const _Ty& _Left, const _Ty& _Right) const {
        return _Left ^ _Right;
    }
};

template <class _Ty = void>
struct bit_not {
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _ARGUMENT_TYPE_NAME;
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ty _RESULT_TYPE_NAME;

    _NODISCARD constexpr _Ty operator()(const _Ty& _Left) const {
        return ~_Left;
    }
};

// void specializations of plus, minus, and multiplies are defined in <xstddef>

template <>
struct divides<void> {
    template <class _Ty1, class _Ty2>
    _NODISCARD constexpr auto operator()(_Ty1&& _Left, _Ty2&& _Right) const
        -> decltype(_STD forward<_Ty1>(_Left) / _STD forward<_Ty2>(_Right)) {
        return _STD forward<_Ty1>(_Left) / _STD forward<_Ty2>(_Right);
    }

    using is_transparent = int;
};

template <>
struct modulus<void> {
    template <class _Ty1, class _Ty2>
    _NODISCARD constexpr auto operator()(_Ty1&& _Left, _Ty2&& _Right) const
        -> decltype(_STD forward<_Ty1>(_Left) % _STD forward<_Ty2>(_Right)) {
        return _STD forward<_Ty1>(_Left) % _STD forward<_Ty2>(_Right);
    }

    using is_transparent = int;
};

template <>
struct negate<void> {
    template <class _Ty>
    _NODISCARD constexpr auto operator()(_Ty&& _Left) const -> decltype(-_STD forward<_Ty>(_Left)) {
        return -_STD forward<_Ty>(_Left);
    }

    using is_transparent = int;
};

// void specializations of equal_to, not_equal_to, greater, less, greater_equal, and less_equal are defined in <xstddef>

template <>
struct logical_and<void> {
    template <class _Ty1, class _Ty2>
    _NODISCARD constexpr auto operator()(_Ty1&& _Left, _Ty2&& _Right) const
        -> decltype(_STD forward<_Ty1>(_Left) && _STD forward<_Ty2>(_Right)) {
        return _STD forward<_Ty1>(_Left) && _STD forward<_Ty2>(_Right);
    }

    using is_transparent = int;
};

template <>
struct logical_or<void> {
    template <class _Ty1, class _Ty2>
    _NODISCARD constexpr auto operator()(_Ty1&& _Left, _Ty2&& _Right) const
        -> decltype(_STD forward<_Ty1>(_Left) || _STD forward<_Ty2>(_Right)) {
        return _STD forward<_Ty1>(_Left) || _STD forward<_Ty2>(_Right);
    }

    using is_transparent = int;
};

template <>
struct logical_not<void> {
    template <class _Ty>
    _NODISCARD constexpr auto operator()(_Ty&& _Left) const -> decltype(!_STD forward<_Ty>(_Left)) {
        return !_STD forward<_Ty>(_Left);
    }

    using is_transparent = int;
};

template <>
struct bit_and<void> {
    template <class _Ty1, class _Ty2>
    _NODISCARD constexpr auto operator()(_Ty1&& _Left, _Ty2&& _Right) const
        -> decltype(_STD forward<_Ty1>(_Left) & _STD forward<_Ty2>(_Right)) {
        return _STD forward<_Ty1>(_Left) & _STD forward<_Ty2>(_Right);
    }

    using is_transparent = int;
};

template <>
struct bit_or<void> {
    template <class _Ty1, class _Ty2>
    _NODISCARD constexpr auto operator()(_Ty1&& _Left, _Ty2&& _Right) const
        -> decltype(_STD forward<_Ty1>(_Left) | _STD forward<_Ty2>(_Right)) {
        return _STD forward<_Ty1>(_Left) | _STD forward<_Ty2>(_Right);
    }

    using is_transparent = int;
};

template <>
struct bit_xor<void> {
    template <class _Ty1, class _Ty2>
    _NODISCARD constexpr auto operator()(_Ty1&& _Left, _Ty2&& _Right) const
        -> decltype(_STD forward<_Ty1>(_Left) ^ _STD forward<_Ty2>(_Right)) {
        return _STD forward<_Ty1>(_Left) ^ _STD forward<_Ty2>(_Right);
    }

    using is_transparent = int;
};

template <>
struct bit_not<void> {
    template <class _Ty>
    _NODISCARD constexpr auto operator()(_Ty&& _Left) const -> decltype(~_STD forward<_Ty>(_Left)) {
        return ~_STD forward<_Ty>(_Left);
    }

    using is_transparent = int;
};

#if _HAS_DEPRECATED_NEGATORS
_STL_DISABLE_DEPRECATED_WARNING
template <class _Fn>
class _CXX17_DEPRECATE_NEGATORS unary_negate {
public:
    using argument_type = typename _Fn::argument_type;
    using result_type   = bool;

    constexpr explicit unary_negate(const _Fn& _Func) : _Functor(_Func) {}

    _NODISCARD constexpr bool operator()(const argument_type& _Left) const {
        return !_Functor(_Left);
    }

private:
    _Fn _Functor;
};

template <class _Fn>
_CXX17_DEPRECATE_NEGATORS _NODISCARD constexpr unary_negate<_Fn> not1(const _Fn& _Func) {
    return unary_negate<_Fn>(_Func);
}

template <class _Fn>
class _CXX17_DEPRECATE_NEGATORS binary_negate {
public:
    using first_argument_type  = typename _Fn::first_argument_type;
    using second_argument_type = typename _Fn::second_argument_type;
    using result_type          = bool;

    constexpr explicit binary_negate(const _Fn& _Func) : _Functor(_Func) {}

    _NODISCARD constexpr bool operator()(const first_argument_type& _Left, const second_argument_type& _Right) const {
        return !_Functor(_Left, _Right);
    }

private:
    _Fn _Functor;
};

template <class _Fn>
_CXX17_DEPRECATE_NEGATORS _NODISCARD constexpr binary_negate<_Fn> not2(const _Fn& _Func) {
    return binary_negate<_Fn>(_Func);
}
_STL_RESTORE_DEPRECATED_WARNING
#endif // _HAS_DEPRECATED_NEGATORS

#if _HAS_AUTO_PTR_ETC
_STL_DISABLE_DEPRECATED_WARNING
template <class _Fn>
class binder1st : public unary_function<typename _Fn::second_argument_type,
                      typename _Fn::result_type> { // functor adapter _Func(stored, right)
public:
    using _Base         = unary_function<typename _Fn::second_argument_type, typename _Fn::result_type>;
    using argument_type = typename _Base::argument_type;
    using result_type   = typename _Base::result_type;

    binder1st(const _Fn& _Func, const typename _Fn::first_argument_type& _Left) : op(_Func), value(_Left) {}

    result_type operator()(const argument_type& _Right) const {
        return op(value, _Right);
    }

    result_type operator()(argument_type& _Right) const {
        return op(value, _Right);
    }

protected:
    _Fn op;
    typename _Fn::first_argument_type value; // the left operand
};

template <class _Fn, class _Ty>
_NODISCARD binder1st<_Fn> bind1st(const _Fn& _Func, const _Ty& _Left) {
    typename _Fn::first_argument_type _Val(_Left);
    return binder1st<_Fn>(_Func, _Val);
}

template <class _Fn>
class binder2nd : public unary_function<typename _Fn::first_argument_type,
                      typename _Fn::result_type> { // functor adapter _Func(left, stored)
public:
    using _Base         = unary_function<typename _Fn::first_argument_type, typename _Fn::result_type>;
    using argument_type = typename _Base::argument_type;
    using result_type   = typename _Base::result_type;

    binder2nd(const _Fn& _Func, const typename _Fn::second_argument_type& _Right) : op(_Func), value(_Right) {}

    result_type operator()(const argument_type& _Left) const {
        return op(_Left, value);
    }

    result_type operator()(argument_type& _Left) const {
        return op(_Left, value);
    }

protected:
    _Fn op;
    typename _Fn::second_argument_type value; // the right operand
};

template <class _Fn, class _Ty>
_NODISCARD binder2nd<_Fn> bind2nd(const _Fn& _Func, const _Ty& _Right) {
    typename _Fn::second_argument_type _Val(_Right);
    return binder2nd<_Fn>(_Func, _Val);
}
_STL_RESTORE_DEPRECATED_WARNING

template <class _Arg, class _Result, class _Fn = _Result (*)(_Arg)>
class pointer_to_unary_function : public unary_function<_Arg, _Result> { // functor adapter (*pfunc)(left)
public:
    explicit pointer_to_unary_function(_Fn _Left) : _Pfun(_Left) {}

    _Result operator()(_Arg _Left) const {
        return _Pfun(_Left);
    }

protected:
    _Fn _Pfun; // the function pointer
};

template <class _Arg1, class _Arg2, class _Result, class _Fn = _Result (*)(_Arg1, _Arg2)>
class pointer_to_binary_function
    : public binary_function<_Arg1, _Arg2, _Result> { // functor adapter (*pfunc)(left, right)
public:
    explicit pointer_to_binary_function(_Fn _Left) : _Pfun(_Left) {}

    _Result operator()(_Arg1 _Left, _Arg2 _Right) const {
        return _Pfun(_Left, _Right);
    }

protected:
    _Fn _Pfun; // the function pointer
};

#define _PTR_FUN(CALL_OPT, X1, X2, X3)                                                                      \
    template <class _Arg, class _Result>                                                                    \
    _NODISCARD pointer_to_unary_function<_Arg, _Result, _Result(CALL_OPT*)(_Arg)> ptr_fun(                  \
        _Result(CALL_OPT* _Left)(_Arg)) {                                                                   \
        return pointer_to_unary_function<_Arg, _Result, _Result(CALL_OPT*)(_Arg)>(_Left);                   \
    }                                                                                                       \
    template <class _Arg1, class _Arg2, class _Result>                                                      \
    _NODISCARD pointer_to_binary_function<_Arg1, _Arg2, _Result, _Result(CALL_OPT*)(_Arg1, _Arg2)> ptr_fun( \
        _Result(CALL_OPT* _Left)(_Arg1, _Arg2)) {                                                           \
        return pointer_to_binary_function<_Arg1, _Arg2, _Result, _Result(CALL_OPT*)(_Arg1, _Arg2)>(_Left);  \
    }

_NON_MEMBER_CALL(_PTR_FUN, X1, X2, X3)
#undef _PTR_FUN

template <class _Result, class _Ty>
class mem_fun_t : public unary_function<_Ty*, _Result> { // functor adapter (*p->*pfunc)(), non-const *pfunc
public:
    explicit mem_fun_t(_Result (_Ty::*_Pm)()) : _Pmemfun(_Pm) {}

    _Result operator()(_Ty* _Pleft) const {
        return (_Pleft->*_Pmemfun)();
    }

private:
    _Result (_Ty::*_Pmemfun)(); // the member function pointer
};

template <class _Result, class _Ty, class _Arg>
class mem_fun1_t : public binary_function<_Ty*, _Arg, _Result> { // functor adapter (*p->*pfunc)(val), non-const *pfunc
public:
    explicit mem_fun1_t(_Result (_Ty::*_Pm)(_Arg)) : _Pmemfun(_Pm) {}

    _Result operator()(_Ty* _Pleft, _Arg _Right) const {
        return (_Pleft->*_Pmemfun)(_Right);
    }

private:
    _Result (_Ty::*_Pmemfun)(_Arg); // the member function pointer
};

template <class _Result, class _Ty>
class const_mem_fun_t : public unary_function<const _Ty*, _Result> { // functor adapter (*p->*pfunc)(), const *pfunc
public:
    explicit const_mem_fun_t(_Result (_Ty::*_Pm)() const) : _Pmemfun(_Pm) {}

    _Result operator()(const _Ty* _Pleft) const {
        return (_Pleft->*_Pmemfun)();
    }

private:
    _Result (_Ty::*_Pmemfun)() const; // the member function pointer
};

template <class _Result, class _Ty, class _Arg>
class const_mem_fun1_t
    : public binary_function<const _Ty*, _Arg, _Result> { // functor adapter (*p->*pfunc)(val), const *pfunc
public:
    explicit const_mem_fun1_t(_Result (_Ty::*_Pm)(_Arg) const) : _Pmemfun(_Pm) {}

    _Result operator()(const _Ty* _Pleft, _Arg _Right) const {
        return (_Pleft->*_Pmemfun)(_Right);
    }

private:
    _Result (_Ty::*_Pmemfun)(_Arg) const; // the member function pointer
};

template <class _Result, class _Ty>
_NODISCARD mem_fun_t<_Result, _Ty> mem_fun(_Result (_Ty::*_Pm)()) {
    return mem_fun_t<_Result, _Ty>(_Pm);
}

template <class _Result, class _Ty, class _Arg>
_NODISCARD mem_fun1_t<_Result, _Ty, _Arg> mem_fun(_Result (_Ty::*_Pm)(_Arg)) {
    return mem_fun1_t<_Result, _Ty, _Arg>(_Pm);
}

template <class _Result, class _Ty>
_NODISCARD const_mem_fun_t<_Result, _Ty> mem_fun(_Result (_Ty::*_Pm)() const) {
    return const_mem_fun_t<_Result, _Ty>(_Pm);
}

template <class _Result, class _Ty, class _Arg>
_NODISCARD const_mem_fun1_t<_Result, _Ty, _Arg> mem_fun(_Result (_Ty::*_Pm)(_Arg) const) {
    return const_mem_fun1_t<_Result, _Ty, _Arg>(_Pm);
}

template <class _Result, class _Ty>
class mem_fun_ref_t : public unary_function<_Ty, _Result> { // functor adapter (*left.*pfunc)(), non-const *pfunc
public:
    explicit mem_fun_ref_t(_Result (_Ty::*_Pm)()) : _Pmemfun(_Pm) {}

    _Result operator()(_Ty& _Left) const {
        return (_Left.*_Pmemfun)();
    }

private:
    _Result (_Ty::*_Pmemfun)(); // the member function pointer
};

template <class _Result, class _Ty, class _Arg>
class mem_fun1_ref_t
    : public binary_function<_Ty, _Arg, _Result> { // functor adapter (*left.*pfunc)(val), non-const *pfunc
public:
    explicit mem_fun1_ref_t(_Result (_Ty::*_Pm)(_Arg)) : _Pmemfun(_Pm) {}

    _Result operator()(_Ty& _Left, _Arg _Right) const {
        return (_Left.*_Pmemfun)(_Right);
    }

private:
    _Result (_Ty::*_Pmemfun)(_Arg); // the member function pointer
};

template <class _Result, class _Ty>
class const_mem_fun_ref_t : public unary_function<_Ty, _Result> { // functor adapter (*left.*pfunc)(), const *pfunc
public:
    explicit const_mem_fun_ref_t(_Result (_Ty::*_Pm)() const) : _Pmemfun(_Pm) {}

    _Result operator()(const _Ty& _Left) const {
        return (_Left.*_Pmemfun)();
    }

private:
    _Result (_Ty::*_Pmemfun)() const; // the member function pointer
};

template <class _Result, class _Ty, class _Arg>
class const_mem_fun1_ref_t
    : public binary_function<_Ty, _Arg, _Result> { // functor adapter (*left.*pfunc)(val), const *pfunc
public:
    explicit const_mem_fun1_ref_t(_Result (_Ty::*_Pm)(_Arg) const) : _Pmemfun(_Pm) {}

    _Result operator()(const _Ty& _Left, _Arg _Right) const {
        return (_Left.*_Pmemfun)(_Right);
    }

private:
    _Result (_Ty::*_Pmemfun)(_Arg) const; // the member function pointer
};

template <class _Result, class _Ty>
_NODISCARD mem_fun_ref_t<_Result, _Ty> mem_fun_ref(_Result (_Ty::*_Pm)()) {
    return mem_fun_ref_t<_Result, _Ty>(_Pm);
}

template <class _Result, class _Ty, class _Arg>
_NODISCARD mem_fun1_ref_t<_Result, _Ty, _Arg> mem_fun_ref(_Result (_Ty::*_Pm)(_Arg)) {
    return mem_fun1_ref_t<_Result, _Ty, _Arg>(_Pm);
}

template <class _Result, class _Ty>
_NODISCARD const_mem_fun_ref_t<_Result, _Ty> mem_fun_ref(_Result (_Ty::*_Pm)() const) {
    return const_mem_fun_ref_t<_Result, _Ty>(_Pm);
}

template <class _Result, class _Ty, class _Arg>
_NODISCARD const_mem_fun1_ref_t<_Result, _Ty, _Arg> mem_fun_ref(_Result (_Ty::*_Pm)(_Arg) const) {
    return const_mem_fun1_ref_t<_Result, _Ty, _Arg>(_Pm);
}
#endif // _HAS_AUTO_PTR_ETC

template <class _Memptr>
class _Mem_fn : public _Weak_types<_Memptr> {
private:
    _Memptr _Pm;

public:
    constexpr explicit _Mem_fn(_Memptr _Val) noexcept : _Pm(_Val) {}

    template <class... _Types>
    _CONSTEXPR20 auto operator()(_Types&&... _Args) const
        noexcept(noexcept(_STD invoke(_Pm, _STD forward<_Types>(_Args)...)))
            -> decltype(_STD invoke(_Pm, _STD forward<_Types>(_Args)...)) {
        return _STD invoke(_Pm, _STD forward<_Types>(_Args)...);
    }
};

template <class _Rx, class _Ty>
_NODISCARD _CONSTEXPR20 _Mem_fn<_Rx _Ty::*> mem_fn(_Rx _Ty::*_Pm) noexcept {
    return _Mem_fn<_Rx _Ty::*>(_Pm);
}

#if _HAS_CXX17
struct _Not_fn_tag {
    explicit _Not_fn_tag() = default;
};

template <class _Decayed>
class _Not_fn : private _Ebco_base<_Decayed> {
private:
    using _Mybase = _Ebco_base<_Decayed>;

public:
    template <class _Callable, class _Tag, enable_if_t<is_same_v<_Tag, _Not_fn_tag>, int> = 0>
    constexpr explicit _Not_fn(_Callable&& _Obj, _Tag) noexcept(
        is_nothrow_constructible_v<_Decayed, _Callable>) // strengthened
        : _Mybase(_STD forward<_Callable>(_Obj)) {} // store a callable object

    constexpr _Not_fn(const _Not_fn&) = default;
    constexpr _Not_fn(_Not_fn&&)      = default;

    template <class... _Types>
    _CONSTEXPR20 auto operator()(_Types&&... _Args) & noexcept(
        noexcept(!_STD invoke(this->_Get_val(), _STD forward<_Types>(_Args)...)))
        -> decltype(!_STD declval<invoke_result_t<_Decayed&, _Types...>>()) {
        return !_STD invoke(this->_Get_val(), _STD forward<_Types>(_Args)...);
    }

    template <class... _Types>
    _CONSTEXPR20 auto operator()(_Types&&... _Args) const& noexcept(
        noexcept(!_STD invoke(this->_Get_val(), _STD forward<_Types>(_Args)...)))
        -> decltype(!_STD declval<invoke_result_t<const _Decayed&, _Types...>>()) {
        return !_STD invoke(this->_Get_val(), _STD forward<_Types>(_Args)...);
    }

    template <class... _Types>
    _CONSTEXPR20 auto operator()(_Types&&... _Args) && noexcept(
        noexcept(!_STD invoke(_STD move(this->_Get_val()), _STD forward<_Types>(_Args)...)))
        -> decltype(!_STD declval<invoke_result_t<_Decayed, _Types...>>()) {
        return !_STD invoke(_STD move(this->_Get_val()), _STD forward<_Types>(_Args)...);
    }

    template <class... _Types>
    _CONSTEXPR20 auto operator()(_Types&&... _Args) const&& noexcept(
        noexcept(!_STD invoke(_STD move(this->_Get_val()), _STD forward<_Types>(_Args)...)))
        -> decltype(!_STD declval<invoke_result_t<const _Decayed, _Types...>>()) {
        return !_STD invoke(_STD move(this->_Get_val()), _STD forward<_Types>(_Args)...);
    }
};

template <class _Callable>
_NODISCARD _CONSTEXPR20 _Not_fn<decay_t<_Callable>> not_fn(_Callable&& _Obj) noexcept(
    is_nothrow_constructible_v<decay_t<_Callable>, _Callable>) /* strengthened */ {
    // wrap a callable object to be negated
    return _Not_fn<decay_t<_Callable>>(_STD forward<_Callable>(_Obj), _Not_fn_tag{});
}
#endif // _HAS_CXX17

class bad_function_call : public exception { // exception thrown when an empty std::function is called
public:
    bad_function_call() noexcept {}

    _NODISCARD virtual const char* __CLR_OR_THIS_CALL what() const noexcept override {
        // return pointer to message string
        return "bad function call";
    }
};

[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xbad_function_call();

struct _Unforced { // tag to distinguish bind() from bind<R>()
    explicit _Unforced() = default;
};

// helper to give INVOKE an explicit return type; avoids undesirable Expression SFINAE
template <class _Rx, bool = is_void_v<_Rx>>
struct _Invoker_ret { // selected for _Rx being cv void
    template <class _Fx, class... _Valtys>
    static _CONSTEXPR20 void _Call(_Fx&& _Func, _Valtys&&... _Vals) noexcept(_Select_invoke_traits<_Fx,
        _Valtys...>::_Is_nothrow_invocable::value) { // INVOKE, "implicitly" converted to void
        _STD invoke(static_cast<_Fx&&>(_Func), static_cast<_Valtys&&>(_Vals)...);
    }
};

template <class _Rx>
struct _Invoker_ret<_Rx, false> { // selected for all _Rx other than cv void and _Unforced
    template <class _Fx, class... _Valtys>
    static _CONSTEXPR20 _Rx _Call(_Fx&& _Func, _Valtys&&... _Vals) noexcept(_Select_invoke_traits<_Fx,
        _Valtys...>::template _Is_nothrow_invocable_r<_Rx>::value) { // INVOKE, implicitly converted to _Rx
        return _STD invoke(static_cast<_Fx&&>(_Func), static_cast<_Valtys&&>(_Vals)...);
    }
};

template <>
struct _Invoker_ret<_Unforced, false> { // selected for _Rx being _Unforced
    template <class _Fx, class... _Valtys>
    static _CONSTEXPR20 auto _Call(_Fx&& _Func, _Valtys&&... _Vals) noexcept(
        _Select_invoke_traits<_Fx, _Valtys...>::_Is_nothrow_invocable::value)
        -> decltype(_STD invoke(static_cast<_Fx&&>(_Func), static_cast<_Valtys&&>(_Vals)...)) { // INVOKE, unchanged
        return _STD invoke(static_cast<_Fx&&>(_Func), static_cast<_Valtys&&>(_Vals)...);
    }
};

template <class _Fty>
class function;

template <class _Ty>
_INLINE_VAR constexpr bool _Testable_callable_v =
    disjunction_v<is_pointer<_Ty>, _Is_specialization<_Ty, function>, is_member_pointer<_Ty>>;

template <class _Ty>
bool _Test_callable(const _Ty& _Arg) noexcept { // determine whether std::function must store _Arg
    if constexpr (_Testable_callable_v<_Ty>) {
        return !!_Arg;
    } else {
        return true;
    }
}

template <class _Rx, class... _Types>
class __declspec(novtable) _Func_base { // abstract base for implementation types
public:
    virtual _Func_base* _Copy(void*) const                 = 0;
    virtual _Func_base* _Move(void*) noexcept              = 0;
    virtual _Rx _Do_call(_Types&&...)                      = 0;
    virtual const type_info& _Target_type() const noexcept = 0;
    virtual void _Delete_this(bool) noexcept               = 0;

#if _HAS_STATIC_RTTI
    const void* _Target(const type_info& _Info) const noexcept {
        return _Target_type() == _Info ? _Get() : nullptr;
    }
#endif // _HAS_STATIC_RTTI

    _Func_base()                  = default;
    _Func_base(const _Func_base&) = delete;
    _Func_base& operator=(const _Func_base&) = delete;
    // dtor non-virtual due to _Delete_this()

private:
    virtual const void* _Get() const noexcept = 0;
};

constexpr size_t _Space_size = (_Small_object_num_ptrs - 1) * sizeof(void*);

template <class _Impl> // determine whether _Impl must be dynamically allocated
_INLINE_VAR constexpr bool _Is_large = sizeof(_Impl) > _Space_size || alignof(_Impl) > alignof(max_align_t)
                                    || !_Impl::_Nothrow_move::value;

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
template <class _Callable, class _Alloc, class _Rx, class... _Types>
class _Func_impl final : public _Func_base<_Rx, _Types...> {
    // derived class for specific implementation types that use allocators
public:
    using _Mybase        = _Func_base<_Rx, _Types...>;
    using _Myalty        = _Rebind_alloc_t<_Alloc, _Func_impl>;
    using _Myalty_traits = allocator_traits<_Myalty>;
    using _Nothrow_move  = is_nothrow_move_constructible<_Callable>;

    template <class _Other1, class _Other2>
    _Func_impl(_Other1&& _Val, _Other2&& _Ax)
        : _Mypair(_One_then_variadic_args_t{}, _STD forward<_Other2>(_Ax), _STD forward<_Other1>(_Val)) {}

    // dtor non-virtual due to _Delete_this()

private:
    virtual _Mybase* _Copy(void* _Where) const override {
        auto& _Myax = _Mypair._Get_first();
        if constexpr (_Is_large<_Func_impl>) {
            _Myalty _Rebound(_Myax);
            _Alloc_construct_ptr<_Myalty> _Constructor{_Rebound};
            _Constructor._Allocate();
            _Construct_in_place(*_Constructor._Ptr, _Mypair._Myval2, _Myax);
            return _Constructor._Release();
        } else {
            const auto _Ptr = static_cast<_Func_impl*>(_Where);
            _Construct_in_place(*_Ptr, _Mypair._Myval2, _Myax);
            return _Ptr;
        }
    }

    virtual _Mybase* _Move(void* _Where) noexcept override {
        if constexpr (_Is_large<_Func_impl>) {
            return nullptr;
        } else {
            const auto _Ptr = static_cast<_Func_impl*>(_Where);
            _Construct_in_place(*_Ptr, _STD move(_Mypair._Myval2), _STD move(_Mypair._Get_first()));
            return _Ptr;
        }
    }

    virtual _Rx _Do_call(_Types&&... _Args) override { // call wrapped function
        return _Invoker_ret<_Rx>::_Call(_Mypair._Myval2, _STD forward<_Types>(_Args)...);
    }

    virtual const type_info& _Target_type() const noexcept override {
#if _HAS_STATIC_RTTI
        return typeid(_Callable);
#else // _HAS_STATIC_RTTI
        _CSTD abort();
#endif // _HAS_STATIC_RTTI
    }

    virtual const void* _Get() const noexcept override {
        return _STD addressof(_Mypair._Myval2);
    }

    virtual void _Delete_this(bool _Deallocate) noexcept override { // destroy self
        _Myalty _Al(_Mypair._Get_first());
        _Destroy_in_place(*this);
        if (_Deallocate) {
            _Deallocate_plain(_Al, this);
        }
    }

    _Compressed_pair<_Alloc, _Callable> _Mypair;
};
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

template <class _Callable, class _Rx, class... _Types>
class _Func_impl_no_alloc final : public _Func_base<_Rx, _Types...> {
    // derived class for specific implementation types that don't use allocators
public:
    using _Mybase       = _Func_base<_Rx, _Types...>;
    using _Nothrow_move = is_nothrow_move_constructible<_Callable>;

    template <class _Other, enable_if_t<!is_same_v<_Func_impl_no_alloc, decay_t<_Other>>, int> = 0>
    explicit _Func_impl_no_alloc(_Other&& _Val) : _Callee(_STD forward<_Other>(_Val)) {}

    // dtor non-virtual due to _Delete_this()

private:
    virtual _Mybase* _Copy(void* _Where) const override {
        if constexpr (_Is_large<_Func_impl_no_alloc>) {
            (void) _Where; // TRANSITION, DevCom-1004719
            return _Global_new<_Func_impl_no_alloc>(_Callee);
        } else {
            return ::new (_Where) _Func_impl_no_alloc(_Callee);
        }
    }

    virtual _Mybase* _Move(void* _Where) noexcept override {
        if constexpr (_Is_large<_Func_impl_no_alloc>) {
            (void) _Where; // TRANSITION, DevCom-1004719
            return nullptr;
        } else {
            return ::new (_Where) _Func_impl_no_alloc(_STD move(_Callee));
        }
    }

    virtual _Rx _Do_call(_Types&&... _Args) override { // call wrapped function
        return _Invoker_ret<_Rx>::_Call(_Callee, _STD forward<_Types>(_Args)...);
    }

    virtual const type_info& _Target_type() const noexcept override {
#if _HAS_STATIC_RTTI
        return typeid(_Callable);
#else // _HAS_STATIC_RTTI
        _CSTD abort();
#endif // _HAS_STATIC_RTTI
    }

    virtual const void* _Get() const noexcept override {
        return _STD addressof(_Callee);
    }

    virtual void _Delete_this(bool _Dealloc) noexcept override { // destroy self
        this->~_Func_impl_no_alloc();
        if (_Dealloc) {
            _Deallocate<alignof(_Func_impl_no_alloc)>(this, sizeof(_Func_impl_no_alloc));
        }
    }

    _Callable _Callee;
};

#ifdef __CUDACC__ // TRANSITION, CUDA
#define _USE_FUNCTION_INT_0_SFINAE 0
#else
#define _USE_FUNCTION_INT_0_SFINAE 1
#endif // __CUDACC__

template <class _Ret, class... _Types>
class _Func_class : public _Arg_types<_Types...> {
public:
    using result_type = _Ret;

    using _Ptrt = _Func_base<_Ret, _Types...>;

    _Func_class() noexcept {
        _Set(nullptr);
    }

    _Ret operator()(_Types... _Args) const {
        if (_Empty()) {
            _Xbad_function_call();
        }
        const auto _Impl = _Getimpl();
        return _Impl->_Do_call(_STD forward<_Types>(_Args)...);
    }

    ~_Func_class() noexcept {
        _Tidy();
    }

protected:
    template <class _Fx, class _Function>
    using _Enable_if_callable_t =
        enable_if_t<conjunction_v<negation<is_same<decay_t<_Fx>, _Function>>, _Is_invocable_r<_Ret, _Fx, _Types...>>,
            int>;

    bool _Empty() const noexcept {
        return !_Getimpl();
    }

    void _Reset_copy(const _Func_class& _Right) { // copy _Right's stored object
        if (!_Right._Empty()) {
            _Set(_Right._Getimpl()->_Copy(&_Mystorage));
        }
    }

    void _Reset_move(_Func_class&& _Right) noexcept { // move _Right's stored object
        if (!_Right._Empty()) {
            if (_Right._Local()) { // move and tidy
                _Set(_Right._Getimpl()->_Move(&_Mystorage));
                _Right._Tidy();
            } else { // steal from _Right
                _Set(_Right._Getimpl());
                _Right._Set(nullptr);
            }
        }
    }

    template <class _Fx>
    void _Reset(_Fx&& _Val) { // store copy of _Val
        if (!_Test_callable(_Val)) { // null member pointer/function pointer/std::function
            return; // already empty
        }

        using _Impl = _Func_impl_no_alloc<decay_t<_Fx>, _Ret, _Types...>;
        if constexpr (_Is_large<_Impl>) {
            // dynamically allocate _Val
            _Set(_Global_new<_Impl>(_STD forward<_Fx>(_Val)));
        } else {
            // store _Val in-situ
            _Set(::new (static_cast<void*>(&_Mystorage)) _Impl(_STD forward<_Fx>(_Val)));
        }
    }

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
    template <class _Fx, class _Alloc>
    void _Reset_alloc(_Fx&& _Val, const _Alloc& _Ax) { // store copy of _Val with allocator
        if (!_Test_callable(_Val)) { // null member pointer/function pointer/std::function
            return; // already empty
        }

        using _Myimpl = _Func_impl<decay_t<_Fx>, _Alloc, _Ret, _Types...>;
        if constexpr (_Is_large<_Myimpl>) {
            // dynamically allocate _Val
            using _Alimpl = _Rebind_alloc_t<_Alloc, _Myimpl>;
            _Alimpl _Al(_Ax);
            _Alloc_construct_ptr<_Alimpl> _Constructor{_Al};
            _Constructor._Allocate();
            _Construct_in_place(*_Constructor._Ptr, _STD forward<_Fx>(_Val), _Ax);
            _Set(_Unfancy(_Constructor._Release()));
        } else {
            // store _Val in-situ
            const auto _Ptr = reinterpret_cast<_Myimpl*>(&_Mystorage);
            _Construct_in_place(*_Ptr, _STD forward<_Fx>(_Val), _Ax);
            _Set(_Ptr);
        }
    }
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

    void _Tidy() noexcept {
        if (!_Empty()) { // destroy callable object and maybe delete it
            _Getimpl()->_Delete_this(!_Local());
            _Set(nullptr);
        }
    }

    void _Swap(_Func_class& _Right) noexcept { // swap contents with contents of _Right
        if (!_Local() && !_Right._Local()) { // just swap pointers
            _Ptrt* _Temp = _Getimpl();
            _Set(_Right._Getimpl());
            _Right._Set(_Temp);
        } else { // do three-way move
            _Func_class _Temp;
            _Temp._Reset_move(_STD move(*this));
            _Reset_move(_STD move(_Right));
            _Right._Reset_move(_STD move(_Temp));
        }
    }

#if _HAS_STATIC_RTTI
    const type_info& _Target_type() const noexcept {
        return _Getimpl() ? _Getimpl()->_Target_type() : typeid(void);
    }

    const void* _Target(const type_info& _Info) const noexcept {
        return _Getimpl() ? _Getimpl()->_Target(_Info) : nullptr;
    }
#endif // _HAS_STATIC_RTTI

private:
    bool _Local() const noexcept { // test for locally stored copy of object
        return _Getimpl() == static_cast<const void*>(&_Mystorage);
    }

    union _Storage { // storage for small objects (basic_string is small)
        max_align_t _Dummy1; // for maximum alignment
        char _Dummy2[_Space_size]; // to permit aliasing
        _Ptrt* _Ptrs[_Small_object_num_ptrs]; // _Ptrs[_Small_object_num_ptrs - 1] is reserved
    };

    _Storage _Mystorage;
    enum { _EEN_IMPL = _Small_object_num_ptrs - 1 }; // helper for expression evaluator
    _Ptrt* _Getimpl() const noexcept { // get pointer to object
        return _Mystorage._Ptrs[_Small_object_num_ptrs - 1];
    }

    void _Set(_Ptrt* _Ptr) noexcept { // store pointer to object
        _Mystorage._Ptrs[_Small_object_num_ptrs - 1] = _Ptr;
    }
};

template <class _Tx>
struct _Get_function_impl {
    static_assert(_Always_false<_Tx>, "std::function does not accept non-function types as template arguments.");
};

#define _GET_FUNCTION_IMPL(CALL_OPT, X1, X2, X3)                                                  \
    template <class _Ret, class... _Types>                                                        \
    struct _Get_function_impl<_Ret CALL_OPT(_Types...)> { /* determine type from argument list */ \
        using type = _Func_class<_Ret, _Types...>;                                                \
    };

_NON_MEMBER_CALL(_GET_FUNCTION_IMPL, X1, X2, X3)
#undef _GET_FUNCTION_IMPL

#ifdef __cpp_noexcept_function_type
#define _GET_FUNCTION_IMPL_NOEXCEPT(CALL_OPT, X1, X2, X3)                                                         \
    template <class _Ret, class... _Types>                                                                        \
    struct _Get_function_impl<_Ret CALL_OPT(_Types...) noexcept> {                                                \
        static_assert(                                                                                            \
            _Always_false<_Ret>, "std::function does not accept noexcept function types as template arguments."); \
    };
_NON_MEMBER_CALL(_GET_FUNCTION_IMPL_NOEXCEPT, X1, X2, X3)
#undef _GET_FUNCTION_IMPL_NOEXCEPT
#endif // __cpp_noexcept_function_type

template <class _Fty>
class function : public _Get_function_impl<_Fty>::type { // wrapper for callable objects
private:
    using _Mybase = typename _Get_function_impl<_Fty>::type;

public:
    function() noexcept {}

    function(nullptr_t) noexcept {}

    function(const function& _Right) {
        this->_Reset_copy(_Right);
    }

#if _USE_FUNCTION_INT_0_SFINAE
    template <class _Fx, typename _Mybase::template _Enable_if_callable_t<_Fx&, function> = 0>
#else // ^^^ _USE_FUNCTION_INT_0_SFINAE // !_USE_FUNCTION_INT_0_SFINAE vvv
    template <class _Fx, class = typename _Mybase::template _Enable_if_callable_t<_Fx&, function>>
#endif // _USE_FUNCTION_INT_0_SFINAE
    function(_Fx _Func) {
        this->_Reset(_STD move(_Func));
    }

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
    template <class _Alloc>
    function(allocator_arg_t, const _Alloc&) noexcept {}

    template <class _Alloc>
    function(allocator_arg_t, const _Alloc&, nullptr_t) noexcept {}

    template <class _Alloc>
    function(allocator_arg_t, const _Alloc& _Ax, const function& _Right) {
        this->_Reset_alloc(_Right, _Ax);
    }

#if _USE_FUNCTION_INT_0_SFINAE
    template <class _Fx, class _Alloc, typename _Mybase::template _Enable_if_callable_t<_Fx&, function> = 0>
#else // ^^^ _USE_FUNCTION_INT_0_SFINAE // !_USE_FUNCTION_INT_0_SFINAE vvv
    template <class _Fx, class _Alloc, class = typename _Mybase::template _Enable_if_callable_t<_Fx&, function>>
#endif // _USE_FUNCTION_INT_0_SFINAE
    function(allocator_arg_t, const _Alloc& _Ax, _Fx _Func) {
        this->_Reset_alloc(_STD move(_Func), _Ax);
    }
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

    function& operator=(const function& _Right) {
        function(_Right).swap(*this);
        return *this;
    }

    function(function&& _Right) noexcept {
        this->_Reset_move(_STD move(_Right));
    }

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
    template <class _Alloc>
    function(allocator_arg_t, const _Alloc& _Al, function&& _Right) {
        this->_Reset_alloc(_STD move(_Right), _Al);
    }
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

    function& operator=(function&& _Right) noexcept /* strengthened */ {
        if (this != _STD addressof(_Right)) {
            this->_Tidy();
            this->_Reset_move(_STD move(_Right));
        }
        return *this;
    }

#if _USE_FUNCTION_INT_0_SFINAE
    template <class _Fx, typename _Mybase::template _Enable_if_callable_t<decay_t<_Fx>&, function> = 0>
#else // ^^^ _USE_FUNCTION_INT_0_SFINAE // !_USE_FUNCTION_INT_0_SFINAE vvv
    template <class _Fx, class = typename _Mybase::template _Enable_if_callable_t<decay_t<_Fx>&, function>>
#endif // _USE_FUNCTION_INT_0_SFINAE
    function& operator=(_Fx&& _Func) {
        function(_STD forward<_Fx>(_Func)).swap(*this);
        return *this;
    }

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
    template <class _Fx, class _Alloc>
    void assign(_Fx&& _Func, const _Alloc& _Ax) {
        function(allocator_arg, _Ax, _STD forward<_Fx>(_Func)).swap(*this);
    }
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

    function& operator=(nullptr_t) noexcept {
        this->_Tidy();
        return *this;
    }

    template <class _Fx>
    function& operator=(reference_wrapper<_Fx> _Func) noexcept {
        this->_Tidy();
        this->_Reset(_Func);
        return *this;
    }

    void swap(function& _Right) noexcept {
        this->_Swap(_Right);
    }

    explicit operator bool() const noexcept {
        return !this->_Empty();
    }

#if _HAS_STATIC_RTTI
    _NODISCARD const type_info& target_type() const noexcept {
        return this->_Target_type();
    }

    template <class _Fx>
    _NODISCARD _Fx* target() noexcept {
        return reinterpret_cast<_Fx*>(const_cast<void*>(this->_Target(typeid(_Fx))));
    }

    template <class _Fx>
    _NODISCARD const _Fx* target() const noexcept {
        return reinterpret_cast<const _Fx*>(this->_Target(typeid(_Fx)));
    }
#else // _HAS_STATIC_RTTI
    const type_info& target_type() const noexcept = delete; // requires static RTTI

    template <class _Fx>
    _Fx* target() noexcept = delete; // requires static RTTI

    template <class _Fx>
    const _Fx* target() const noexcept = delete; // requires static RTTI
#endif // _HAS_STATIC_RTTI
};

#if _HAS_CXX17
#define _FUNCTION_POINTER_DEDUCTION_GUIDE(CALL_OPT, X1, X2, X3) \
    template <class _Ret, class... _Types>                      \
    function(_Ret(CALL_OPT*)(_Types...)) -> function<_Ret(_Types...)>; // intentionally discards CALL_OPT

_NON_MEMBER_CALL(_FUNCTION_POINTER_DEDUCTION_GUIDE, X1, X2, X3)
#undef _FUNCTION_POINTER_DEDUCTION_GUIDE

template <class _Fx, class = void>
struct _Deduce_signature {}; // can't deduce signature when &_Fx::operator() is missing, inaccessible, or ambiguous

template <class _Fx>
struct _Deduce_signature<_Fx, void_t<decltype(&_Fx::operator())>>
    : _Is_memfunptr<decltype(&_Fx::operator())>::_Guide_type {}; // N4842 [func.wrap.func.con]/12

template <class _Fx>
function(_Fx) -> function<typename _Deduce_signature<_Fx>::type>;
#endif // _HAS_CXX17

template <class _Fty>
void swap(function<_Fty>& _Left, function<_Fty>& _Right) noexcept {
    _Left.swap(_Right);
}

template <class _Fty>
_NODISCARD bool operator==(const function<_Fty>& _Other, nullptr_t) noexcept {
    return !_Other;
}

#if !_HAS_CXX20
template <class _Fty>
_NODISCARD bool operator==(nullptr_t, const function<_Fty>& _Other) noexcept {
    return !_Other;
}

template <class _Fty>
_NODISCARD bool operator!=(const function<_Fty>& _Other, nullptr_t) noexcept {
    return static_cast<bool>(_Other);
}

template <class _Fty>
_NODISCARD bool operator!=(nullptr_t, const function<_Fty>& _Other) noexcept {
    return static_cast<bool>(_Other);
}
#endif // !_HAS_CXX20

template <int _Nx>
struct _Ph { // placeholder
    static_assert(_Nx > 0, "invalid placeholder index");
};

template <class _Tx>
struct is_placeholder : integral_constant<int, 0> {}; // _Tx is not a placeholder

template <int _Nx>
struct is_placeholder<_Ph<_Nx>> : integral_constant<int, _Nx> {}; // _Ph is a placeholder

template <class _Tx>
struct is_placeholder<const _Tx> : is_placeholder<_Tx>::type {}; // ignore cv-qualifiers

template <class _Tx>
struct is_placeholder<volatile _Tx> : is_placeholder<_Tx>::type {}; // ignore cv-qualifiers

template <class _Tx>
struct is_placeholder<const volatile _Tx> : is_placeholder<_Tx>::type {}; // ignore cv-qualifiers

template <class _Ty>
_INLINE_VAR constexpr int is_placeholder_v = is_placeholder<_Ty>::value;

template <class _Ret, class _Fx, class... _Types>
class _Binder;

template <class _Tx>
struct is_bind_expression : false_type {}; // _Tx is not a bind expression

template <class _Ret, class _Fx, class... _Types>
struct is_bind_expression<_Binder<_Ret, _Fx, _Types...>> : true_type {}; // _Binder is a bind expression

template <class _Tx>
struct is_bind_expression<const _Tx> : is_bind_expression<_Tx>::type {}; // ignore cv-qualifiers

template <class _Tx>
struct is_bind_expression<volatile _Tx> : is_bind_expression<_Tx>::type {}; // ignore cv-qualifiers

template <class _Tx>
struct is_bind_expression<const volatile _Tx> : is_bind_expression<_Tx>::type {}; // ignore cv-qualifiers

template <class _Ty>
_INLINE_VAR constexpr bool is_bind_expression_v = is_bind_expression<_Ty>::value;

template <class _Cv_TiD, bool = _Is_specialization_v<remove_cv_t<_Cv_TiD>, reference_wrapper>,
    bool = is_bind_expression_v<_Cv_TiD>, int = is_placeholder_v<_Cv_TiD>>
struct _Select_fixer;

template <class _Cv_TiD>
struct _Select_fixer<_Cv_TiD, true, false, 0> { // reference_wrapper fixer
    template <class _Untuple>
    static constexpr auto _Fix(_Cv_TiD& _Tid, _Untuple&&) noexcept -> typename _Cv_TiD::type& {
        // unwrap a reference_wrapper
        return _Tid.get();
    }
};

template <class _Cv_TiD>
struct _Select_fixer<_Cv_TiD, false, true, 0> { // nested bind fixer
    template <class _Untuple, size_t... _Jx>
    static constexpr auto _Apply(_Cv_TiD& _Tid, _Untuple&& _Ut, index_sequence<_Jx...>) noexcept(
        noexcept(_Tid(_STD get<_Jx>(_STD move(_Ut))...))) -> decltype(_Tid(_STD get<_Jx>(_STD move(_Ut))...)) {
        // call a nested bind expression
        return _Tid(_STD get<_Jx>(_STD move(_Ut))...);
    }

    template <class _Untuple>
    static constexpr auto _Fix(_Cv_TiD& _Tid, _Untuple&& _Ut) noexcept(
        noexcept(_Apply(_Tid, _STD move(_Ut), make_index_sequence<tuple_size_v<_Untuple>>{})))
        -> decltype(_Apply(_Tid, _STD move(_Ut), make_index_sequence<tuple_size_v<_Untuple>>{})) {
        // call a nested bind expression
        return _Apply(_Tid, _STD move(_Ut), make_index_sequence<tuple_size_v<_Untuple>>{});
    }
};

template <class _Cv_TiD>
struct _Select_fixer<_Cv_TiD, false, false, 0> { // identity fixer
    template <class _Untuple>
    static constexpr _Cv_TiD& _Fix(_Cv_TiD& _Tid, _Untuple&&) noexcept {
        // pass a bound argument as an lvalue (important!)
        return _Tid;
    }
};

template <class _Cv_TiD, int _Jx>
struct _Select_fixer<_Cv_TiD, false, false, _Jx> { // placeholder fixer
    static_assert(_Jx > 0, "invalid is_placeholder value");

    template <class _Untuple>
    static constexpr auto _Fix(_Cv_TiD&, _Untuple&& _Ut) noexcept
        -> decltype(_STD get<_Jx - 1>(_STD move(_Ut))) { // choose the Jth unbound argument (1-based indexing)
        return _STD get<_Jx - 1>(_STD move(_Ut));
    }
};

template <class _Cv_TiD, class _Untuple>
constexpr auto _Fix_arg(_Cv_TiD& _Tid, _Untuple&& _Ut) noexcept(
    noexcept(_Select_fixer<_Cv_TiD>::_Fix(_Tid, _STD move(_Ut))))
    -> decltype(_Select_fixer<_Cv_TiD>::_Fix(_Tid, _STD move(_Ut))) { // translate an argument for bind
    return _Select_fixer<_Cv_TiD>::_Fix(_Tid, _STD move(_Ut));
}

template <class _Ret, size_t... _Ix, class _Cv_FD, class _Cv_tuple_TiD, class _Untuple>
_CONSTEXPR20 auto _Call_binder(_Invoker_ret<_Ret>, index_sequence<_Ix...>, _Cv_FD& _Obj, _Cv_tuple_TiD& _Tpl,
    _Untuple&& _Ut) noexcept(noexcept(_Invoker_ret<_Ret>::_Call(_Obj,
    _Fix_arg(_STD get<_Ix>(_Tpl), _STD move(_Ut))...)))
    -> decltype(_Invoker_ret<_Ret>::_Call(_Obj, _Fix_arg(_STD get<_Ix>(_Tpl), _STD move(_Ut))...)) {
    // bind() and bind<R>() invocation
    return _Invoker_ret<_Ret>::_Call(_Obj, _Fix_arg(_STD get<_Ix>(_Tpl), _STD move(_Ut))...);
}

template <class _Ret>
struct _Forced_result_type { // used by bind<R>()
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef _Ret _RESULT_TYPE_NAME;
};

template <class _Ret, class _Fx>
struct _Binder_result_type { // provide result_type (sometimes)
    using _Decayed = decay_t<_Fx>;

    using _All_weak_types = _Weak_types<_Decayed>;

    using type =
        conditional_t<is_same_v<_Ret, _Unforced>, _Weak_result_type<_All_weak_types>, _Forced_result_type<_Ret>>;
};

template <class _Ret, class _Fx, class... _Types>
class _Binder : public _Binder_result_type<_Ret, _Fx>::type { // wrap bound callable object and arguments
private:
    using _Seq    = index_sequence_for<_Types...>;
    using _First  = decay_t<_Fx>;
    using _Second = tuple<decay_t<_Types>...>;

    _Compressed_pair<_First, _Second> _Mypair;

public:
    constexpr explicit _Binder(_Fx&& _Func, _Types&&... _Args)
        : _Mypair(_One_then_variadic_args_t{}, _STD forward<_Fx>(_Func), _STD forward<_Types>(_Args)...) {}

#define _CALL_BINDER                                                                  \
    _Call_binder(_Invoker_ret<_Ret>{}, _Seq{}, _Mypair._Get_first(), _Mypair._Myval2, \
        _STD forward_as_tuple(_STD forward<_Unbound>(_Unbargs)...))

    template <class... _Unbound>
    _CONSTEXPR20 auto operator()(_Unbound&&... _Unbargs) noexcept(noexcept(_CALL_BINDER)) -> decltype(_CALL_BINDER) {
        return _CALL_BINDER;
    }

    template <class... _Unbound>
    _CONSTEXPR20 auto operator()(_Unbound&&... _Unbargs) const noexcept(noexcept(_CALL_BINDER))
        -> decltype(_CALL_BINDER) {
        return _CALL_BINDER;
    }

#undef _CALL_BINDER
};

template <class _Fx, class... _Types>
_NODISCARD _CONSTEXPR20 _Binder<_Unforced, _Fx, _Types...> bind(_Fx&& _Func, _Types&&... _Args) {
    return _Binder<_Unforced, _Fx, _Types...>(_STD forward<_Fx>(_Func), _STD forward<_Types>(_Args)...);
}

template <class _Ret, class _Fx, class... _Types>
_NODISCARD _CONSTEXPR20 _Binder<_Ret, _Fx, _Types...> bind(_Fx&& _Func, _Types&&... _Args) {
    return _Binder<_Ret, _Fx, _Types...>(_STD forward<_Fx>(_Func), _STD forward<_Types>(_Args)...);
}

namespace placeholders {
    _INLINE_VAR constexpr _Ph<1> _1{};
    _INLINE_VAR constexpr _Ph<2> _2{};
    _INLINE_VAR constexpr _Ph<3> _3{};
    _INLINE_VAR constexpr _Ph<4> _4{};
    _INLINE_VAR constexpr _Ph<5> _5{};
    _INLINE_VAR constexpr _Ph<6> _6{};
    _INLINE_VAR constexpr _Ph<7> _7{};
    _INLINE_VAR constexpr _Ph<8> _8{};
    _INLINE_VAR constexpr _Ph<9> _9{};
    _INLINE_VAR constexpr _Ph<10> _10{};
    _INLINE_VAR constexpr _Ph<11> _11{};
    _INLINE_VAR constexpr _Ph<12> _12{};
    _INLINE_VAR constexpr _Ph<13> _13{};
    _INLINE_VAR constexpr _Ph<14> _14{};
    _INLINE_VAR constexpr _Ph<15> _15{};
    _INLINE_VAR constexpr _Ph<16> _16{};
    _INLINE_VAR constexpr _Ph<17> _17{};
    _INLINE_VAR constexpr _Ph<18> _18{};
    _INLINE_VAR constexpr _Ph<19> _19{};
    _INLINE_VAR constexpr _Ph<20> _20{};
} // namespace placeholders

#if _HAS_CXX20
template <size_t... _Ix, class _Cv_FD, class _Cv_tuple_TiD, class... _Unbound>
constexpr auto _Call_front_binder(index_sequence<_Ix...>, _Cv_FD&& _Obj, _Cv_tuple_TiD&& _Tpl,
    _Unbound&&... _Unbargs) noexcept(noexcept(_STD invoke(_STD forward<_Cv_FD>(_Obj),
    _STD get<_Ix>(_STD forward<_Cv_tuple_TiD>(_Tpl))..., _STD forward<_Unbound>(_Unbargs)...)))
    -> decltype(_STD invoke(_STD forward<_Cv_FD>(_Obj), _STD get<_Ix>(_STD forward<_Cv_tuple_TiD>(_Tpl))...,
        _STD forward<_Unbound>(_Unbargs)...)) {
    return _STD invoke(_STD forward<_Cv_FD>(_Obj), _STD get<_Ix>(_STD forward<_Cv_tuple_TiD>(_Tpl))...,
        _STD forward<_Unbound>(_Unbargs)...);
}

template <class _Fx, class... _Types>
class _Front_binder { // wrap bound callable object and arguments
private:
    using _Seq = index_sequence_for<_Types...>;

    _Compressed_pair<_Fx, tuple<_Types...>> _Mypair;

    _STL_INTERNAL_STATIC_ASSERT(is_same_v<_Fx, decay_t<_Fx>>);
    _STL_INTERNAL_STATIC_ASSERT((is_same_v<_Types, decay_t<_Types>> && ...));

public:
    template <class _FxInit, class... _TypesInit,
        enable_if_t<sizeof...(_TypesInit) != 0 || !is_same_v<remove_cvref_t<_FxInit>, _Front_binder>, int> = 0>
    constexpr explicit _Front_binder(_FxInit&& _Func, _TypesInit&&... _Args)
        : _Mypair(_One_then_variadic_args_t{}, _STD forward<_FxInit>(_Func), _STD forward<_TypesInit>(_Args)...) {}

    template <class... _Unbound>
    constexpr auto operator()(_Unbound&&... _Unbargs) & noexcept(noexcept(
        _Call_front_binder(_Seq{}, _Mypair._Get_first(), _Mypair._Myval2, _STD forward<_Unbound>(_Unbargs)...)))
        -> decltype(
            _Call_front_binder(_Seq{}, _Mypair._Get_first(), _Mypair._Myval2, _STD forward<_Unbound>(_Unbargs)...)) {
        return _Call_front_binder(_Seq{}, _Mypair._Get_first(), _Mypair._Myval2, _STD forward<_Unbound>(_Unbargs)...);
    }

    template <class... _Unbound>
    constexpr auto operator()(_Unbound&&... _Unbargs) const& noexcept(noexcept(
        _Call_front_binder(_Seq{}, _Mypair._Get_first(), _Mypair._Myval2, _STD forward<_Unbound>(_Unbargs)...)))
        -> decltype(
            _Call_front_binder(_Seq{}, _Mypair._Get_first(), _Mypair._Myval2, _STD forward<_Unbound>(_Unbargs)...)) {
        return _Call_front_binder(_Seq{}, _Mypair._Get_first(), _Mypair._Myval2, _STD forward<_Unbound>(_Unbargs)...);
    }

    template <class... _Unbound>
    constexpr auto operator()(_Unbound&&... _Unbargs) && noexcept(noexcept(_Call_front_binder(
        _Seq{}, _STD move(_Mypair._Get_first()), _STD move(_Mypair._Myval2), _STD forward<_Unbound>(_Unbargs)...)))
        -> decltype(_Call_front_binder(
            _Seq{}, _STD move(_Mypair._Get_first()), _STD move(_Mypair._Myval2), _STD forward<_Unbound>(_Unbargs)...)) {
        return _Call_front_binder(
            _Seq{}, _STD move(_Mypair._Get_first()), _STD move(_Mypair._Myval2), _STD forward<_Unbound>(_Unbargs)...);
    }

    template <class... _Unbound>
    constexpr auto operator()(_Unbound&&... _Unbargs) const&& noexcept(noexcept(_Call_front_binder(
        _Seq{}, _STD move(_Mypair._Get_first()), _STD move(_Mypair._Myval2), _STD forward<_Unbound>(_Unbargs)...)))
        -> decltype(_Call_front_binder(
            _Seq{}, _STD move(_Mypair._Get_first()), _STD move(_Mypair._Myval2), _STD forward<_Unbound>(_Unbargs)...)) {
        return _Call_front_binder(
            _Seq{}, _STD move(_Mypair._Get_first()), _STD move(_Mypair._Myval2), _STD forward<_Unbound>(_Unbargs)...);
    }
};

template <class _Fx, class... _Types>
_NODISCARD constexpr auto bind_front(_Fx&& _Func, _Types&&... _Args) {
    static_assert(is_constructible_v<decay_t<_Fx>, _Fx>,
        "std::bind_front requires the decayed callable to be constructible from an undecayed callable");
    static_assert(is_move_constructible_v<decay_t<_Fx>>,
        "std::bind_front requires the decayed callable to be move constructible");
    static_assert(conjunction_v<is_constructible<decay_t<_Types>, _Types>...>,
        "std::bind_front requires the decayed bound arguments to be constructible from undecayed bound arguments");
    static_assert(conjunction_v<is_move_constructible<decay_t<_Types>>...>,
        "std::bind_front requires the decayed bound arguments to be move constructible");

    return _Front_binder<decay_t<_Fx>, decay_t<_Types>...>(_STD forward<_Fx>(_Func), _STD forward<_Types>(_Args)...);
}
#endif // _HAS_CXX20

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
template <class _Fty, class _Alloc>
struct uses_allocator<function<_Fty>, _Alloc> : true_type {}; // true_type if container allocator enabled
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

#if _HAS_CXX17
inline void _Add_alloc_size(size_t& _Size, const size_t _Size_added, const size_t _Alignment) {
    // moves _Size up to account for storing an object of size _Size_added with alignment _Alignment in a memory buffer
    // assumes that the memory buffer will be allocated for the "worst" alignment used in the resulting buffer
    const size_t _Align_masked = _Size & (_Alignment - 1u);
    _Size += _Size_added;
    if (_Align_masked != 0) {
        _Size += _Alignment - _Align_masked;
    }
}

template <class _Ty>
void _Add_alloc_size(size_t& _Size) { // moves _Size up to account for allocating a _Ty
    _Add_alloc_size(_Size, sizeof(_Ty), alignof(_Ty));
}

template <class _Ty>
void _Add_alloc_size(size_t& _Size, const size_t _Count) {
    // moves _Size up to account for allocating an array of _Count _Ty instances
    _Add_alloc_size(_Size, _Get_size_of_n<sizeof(_Ty)>(_Count), alignof(_Ty));
}

inline void* _Decode_aligned_block(void*& _Base, size_t _Size, const size_t _Alignment) {
    // "allocates" an object of size _Size and alignment _Alignment from _Base, and shifts
    // _Base up by the size necessary to decode that object.
    auto _Space  = static_cast<size_t>(-1);
    auto _Result = _STD align(_Alignment, _Size, _Base, _Space);
    _Base        = static_cast<char*>(_Base) + _Size;
    return _Result;
}

template <class _Ty>
_Ty* _Decode_aligned_block(void*& _Base) { // "allocates" a _Ty from _Base
    return static_cast<_Ty*>(_Decode_aligned_block(_Base, sizeof(_Ty), alignof(_Ty)));
}

template <class _Ty>
_Ty* _Decode_aligned_block(void*& _Base, const size_t _Count) {
    // "allocates" an array of _Count _Ty instances from _Base
    return static_cast<_Ty*>(_Decode_aligned_block(_Base, sizeof(_Ty) * _Count, alignof(_Ty)));
}

struct _Global_delete {
    void operator()(void* const _Ptr) const {
        ::operator delete(_Ptr);
    }
};

template <class _FwdItHaystack, class _FwdItPat, class _Pred_eq>
_CONSTEXPR20 pair<_FwdItHaystack, _FwdItHaystack> _Search_pair_unchecked(_FwdItHaystack _First1, _FwdItHaystack _Last1,
    _FwdItPat _First2, _FwdItPat _Last2, _Pred_eq& _Eq, forward_iterator_tag, forward_iterator_tag) {
    // find first [_First2, _Last2) satisfying _Eq, arbitrary iterators
    for (;; ++_First1) { // loop until match or end of a sequence
        _FwdItHaystack _Mid1 = _First1;
        for (_FwdItPat _Mid2 = _First2;; ++_Mid1, (void) ++_Mid2) {
            if (_Mid2 == _Last2) {
                return {_First1, _Mid1};
            }

            if (_Mid1 == _Last1) {
                return {_Last1, _Last1};
            }

            if (!_Eq(*_Mid1, *_Mid2)) {
                break;
            }
        }
    }
}

template <class _FwdItHaystack, class _FwdItPat, class _Pred_eq>
_CONSTEXPR20 pair<_FwdItHaystack, _FwdItHaystack> _Search_pair_unchecked(_FwdItHaystack _First1, _FwdItHaystack _Last1,
    _FwdItPat _First2, _FwdItPat _Last2, _Pred_eq& _Eq, random_access_iterator_tag, random_access_iterator_tag) {
    // find first [_First2, _Last2) satisfying _Eq, random-access iterators
    _Iter_diff_t<_FwdItHaystack> _Count1 = _Last1 - _First1;
    _Iter_diff_t<_FwdItPat> _Count2      = _Last2 - _First2;

    for (; _Count2 <= _Count1; ++_First1, (void) --_Count1) { // room for match, try it
        _FwdItHaystack _Mid1 = _First1;
        for (_FwdItPat _Mid2 = _First2;; ++_Mid1, (void) ++_Mid2) {
            if (_Mid2 == _Last2) {
                return {_First1, _Mid1};
            }

            if (!_Eq(*_Mid1, *_Mid2)) {
                break;
            }
        }
    }

    return {_Last1, _Last1};
}

template <class _FwdItPat, class _Pred_eq = equal_to<>>
class default_searcher { // functor to search haystacks for needles
public:
    _CONSTEXPR20 default_searcher(_FwdItPat _First, _FwdItPat _Last, _Pred_eq _Eq = _Pred_eq())
        : _Data{_One_then_variadic_args_t{}, _STD move(_Eq), pair<_FwdItPat, _FwdItPat>{_First, _Last}} {
        const auto& _Pat = _Data._Myval2;
        _Adl_verify_range(_Pat.first, _Pat.second);
    }

    template <class _FwdItHaystack>
    _NODISCARD _CONSTEXPR20 pair<_FwdItHaystack, _FwdItHaystack> operator()(
        _FwdItHaystack _First, _FwdItHaystack _Last) const {
        // search [_First, _Last) for the searcher's pattern
        _Adl_verify_range(_First, _Last);
        const auto& _Eq  = _Data._Get_first();
        const auto& _Pat = _Data._Myval2;
        _Adl_verify_range(_Pat.first, _Pat.second); // check again to ensure container is not destroyed
        const auto _Result =
            _Search_pair_unchecked(_Get_unwrapped(_First), _Get_unwrapped(_Last), _Get_unwrapped(_Pat.first),
                _Get_unwrapped(_Pat.second), _Eq, _Iter_cat_t<_FwdItHaystack>{}, _Iter_cat_t<_FwdItPat>{});
        _Seek_wrapped(_Last, _Result.second);
        _Seek_wrapped(_First, _Result.first);
        return {_First, _Last};
    }

private:
    _Compressed_pair<_Pred_eq, pair<_FwdItPat, _FwdItPat>> _Data;
};


template <class _RanItPat, class _Hash_ty, class _Pred_eq>
struct _Boyer_moore_hash_delta_1_table { // stores the Boyer-Moore delta_1 table using a hash table
    using _Value_t = _Iter_value_t<_RanItPat>;
    using _Diff    = _Iter_diff_t<_RanItPat>;

    _Boyer_moore_hash_delta_1_table(_RanItPat _Pat_first_arg, _Unwrapped_t<const _RanItPat&> _UPat_first,
        const _Diff _Pat_size_arg, _Hash_ty&& _Hash_fn, _Pred_eq&& _Eq)
        : _Pat_first(_Pat_first_arg), _Pat_size(_Pat_size_arg),
          _Map(0, _STD move(_Hash_fn), _STD move(_Eq)) { // initialize a delta_1 hash table
        for (_Diff _Idx = 1; _Idx <= _Pat_size; ++_Idx, (void) ++_UPat_first) {
            _Map.insert_or_assign(*_UPat_first, _Pat_size - _Idx);
        }
    }

    _Diff _Lookup(const _Value_t& _Value) const {
        // lookup the "character" _Value in the table, returning the maximum shift if found
        const auto _Iter = _Map.find(_Value);
        if (_Iter == _Map.end()) {
            return _Pat_size;
        }

        return _Iter->second;
    }

    _Pred_eq _Get_eq() const {
        return _Map.key_eq();
    }

    const _RanItPat _Pat_first;
    const _Diff _Pat_size;

private:
    unordered_map<_Value_t, _Diff, _Hash_ty, _Pred_eq> _Map;
};


template <class _RanItPat, _Iter_diff_t<_RanItPat> _Limit>
struct _Boyer_moore_flat_delta_1_table { // stores the Boyer-Moore delta_1 table using a plain array lookup
    using _Value_t = _Iter_value_t<_RanItPat>;
    using _Diff    = _Iter_diff_t<_RanItPat>;

    _Boyer_moore_flat_delta_1_table(_RanItPat _Pat_first_arg, _Unwrapped_t<const _RanItPat&> _UPat_first,
        const _Diff _Pat_size_arg, _Unused_parameter, _Unused_parameter)
        : _Pat_first(_Pat_first_arg), _Pat_size(_Pat_size_arg) { // initialize a delta_1 flat table
        _STD fill(_STD begin(_Table), _STD end(_Table), _Pat_size);
        for (_Diff _Idx = 1; _Idx <= _Pat_size; ++_Idx, (void) ++_UPat_first) {
            _Table[_Unsigned_value(*_UPat_first)] = _Pat_size - _Idx;
        }
    }

    _Diff _Lookup(const _Value_t _Value) const { // lookup the "character" _Value in the table
        const auto _UValue = _Unsigned_value(_Value);
        if (_UValue < _STD size(_Table)) {
            return _Table[_UValue];
        }

        return _Pat_size;
    }

    equal_to<> _Get_eq() const {
        return {};
    }

    const _RanItPat _Pat_first;
    const _Diff _Pat_size;

private:
    _Diff _Table[_Limit];
};


template <class _RanItPat, class _Pred_eq>
void _Build_boyer_moore_delta_2_table(_Iter_diff_t<_RanItPat>* const _Shifts, const _RanItPat _Pat_first,
    const _Iter_diff_t<_RanItPat> _Pat_size, _Pred_eq& _Eq) {
    // Builds Boyer-Moore's delta_2 table from a pattern [_Pat_first, _Pat_first + _Pat_size).
    // pre: _Shifts is a pointer to _Pat_size _Iter_diff_t<_RanItPat>s.
    // This is the `dd'` algorithm from "Fast Pattern Matching In Strings" by Knuth, Morris, and Pratt (1977).
    // Note that the published algorithm used 1-based indexing!
    using _Diff = _Iter_diff_t<_RanItPat>;

    if (_Pat_size == 0) {
        return;
    }

    if ((numeric_limits<_Diff>::max)() - _Pat_size < _Pat_size) {
        _Xlength_error("Boyer-Moore pattern is too long");
    }

    const auto _Mx = static_cast<size_t>(_Pat_size);

    const unique_ptr<size_t[]> _Fx{new size_t[_Mx]};

    for (size_t _Kx = 1; _Kx <= _Mx; ++_Kx) {
        _Shifts[_Kx - 1] = static_cast<_Diff>(2 * _Mx - _Kx);
    }

    size_t _Tx = _Mx + 1;
    for (size_t _Jx = _Mx; _Jx > 0; --_Jx, --_Tx) {
        _Fx[_Jx - 1] = _Tx;
        while (_Tx <= _Mx && !_Eq(_Pat_first[_Jx - 1], _Pat_first[_Tx - 1])) {
            _Shifts[_Tx - 1] = (_STD min)(_Shifts[_Tx - 1], static_cast<_Diff>(_Mx - _Jx));
            _Tx              = _Fx[_Tx - 1];
        }
    }

    // The code below is the "Rytter correction" from "Algorithms For String Searching: A Survey" by Baeza-Yates (1989),
    // originally from "A Correct Preprocessing Algorithm For Boyer-Moore String-Searching" by Rytter (1980).

    size_t _Qx = _Tx;
    _Tx        = _Mx + 1 - _Qx;
    for (size_t _Jx = 1, _Tx1 = 0; _Jx <= _Tx; ++_Tx1, ++_Jx) {
        _Fx[_Jx - 1] = _Tx1;
        while (_Tx1 >= 1 && !_Eq(_Pat_first[_Jx - 1], _Pat_first[_Tx1 - 1])) {
            _Tx1 = _Fx[_Tx1 - 1];
        }
    }

    size_t _Qx1 = 1;
    while (_Qx < _Mx) {
        for (size_t _Kx = _Qx1; _Kx <= _Qx; ++_Kx) {
            _Shifts[_Kx - 1] = (_STD min)(_Shifts[_Kx - 1], static_cast<_Diff>(_Mx + _Qx - _Kx));
        }
        _Qx1 = _Qx + 1;

        const size_t _Temp = _Fx[_Tx - 1];

        _Qx = _Qx + _Tx - _Temp;
        _Tx = _Temp;
    }
}


template <class _Delta1_t, class _RanItHaystack>
pair<_RanItHaystack, _RanItHaystack> _Boyer_moore_search(
    const _Delta1_t& _Delta1, typename _Delta1_t::_Diff* _Delta2, _RanItHaystack _First, _RanItHaystack _Last) {
    static_assert(is_same_v<typename _Delta1_t::_Value_t, _Iter_value_t<_RanItHaystack>>,
        "boyer_moore_searcher requires matching iterator value types");
    using _Diff = typename _Delta1_t::_Diff;
    _Adl_verify_range(_First, _Last);
    auto _UFirst         = _Get_unwrapped(_First);
    const auto _ULast    = _Get_unwrapped(_Last);
    const auto _Pat_size = _Delta1._Pat_size;
    if (_Pat_size == 0) {
        return {_First, _First};
    }

    const auto _UPat_first = _Get_unwrapped_n(_Delta1._Pat_first, _Pat_size);
    const auto _Eq         = _Delta1._Get_eq();
    _Diff _Shift           = _Pat_size - 1;
    while (_Shift < _ULast - _UFirst) {
        _UFirst += _Shift;
        _Shift = _Delta1._Lookup(*_UFirst);
        if (_Shift == 0) { // that is, *_UFirst == "_Pat.back()"
            _Diff _Idx = _Pat_size - 1;
            do {
                if (_Idx == 0) {
                    _Seek_wrapped(_Last, _UFirst + _Pat_size);
                    _Seek_wrapped(_First, _UFirst);
                    return {_First, _Last};
                }

                --_Idx;
                --_UFirst;
            } while (_Eq(*_UFirst, _UPat_first[_Idx]));
            _Shift = (_STD max)(_Delta1._Lookup(*_UFirst), _Delta2[_Idx]);
        }
    }

    _Seek_wrapped(_Last, _ULast);
    _Seek_wrapped(_First, _ULast);
    return {_First, _Last};
}


template <class _Delta1_t, class _RanItHaystack>
pair<_RanItHaystack, _RanItHaystack> _Boyer_moore_horspool_search(
    const _Delta1_t& _Delta1, _RanItHaystack _First, _RanItHaystack _Last) {
    static_assert(is_same_v<typename _Delta1_t::_Value_t, _Iter_value_t<_RanItHaystack>>,
        "boyer_moore_horspool_searcher requires matching iterator value types");
    using _Diff = typename _Delta1_t::_Diff;
    _Adl_verify_range(_First, _Last);
    const auto _Pat_size = _Delta1._Pat_size;
    if (_Pat_size == 0) {
        return {_First, _First};
    }

    auto _UFirst           = _Get_unwrapped(_First);
    const auto _ULast      = _Get_unwrapped(_Last);
    const auto _UPat_first = _Get_unwrapped_n(_Delta1._Pat_first, _Pat_size);
    const auto _Eq         = _Delta1._Get_eq();
    _Diff _Shift           = _Pat_size - 1;
    while (_Shift < _ULast - _UFirst) {
        _UFirst += _Shift;
        _Shift = _Delta1._Lookup(*_UFirst);
        if (_Shift == 0) { // that is, *_UFirst == "_Pat.back()"
            const auto _Candidate = _UFirst - (_Pat_size - 1);
            if (_STD equal(_UPat_first, _UPat_first + (_Pat_size - 1), _Candidate, _Pass_fn(_Eq))) {
                _Seek_wrapped(_Last, _Candidate + _Pat_size);
                _Seek_wrapped(_First, _Candidate);
                return {_First, _Last};
            }

            _Shift = 1;
        }
    }

    _Seek_wrapped(_Last, _ULast);
    _Seek_wrapped(_First, _ULast);
    return {_First, _Last};
}


template <class _RanItPat, class _Hash_ty, class _Pred_eq, class _Delta1_t>
struct _Single_delta1_type_boyer_moore_traits {
    using _Diff = _Iter_diff_t<_RanItPat>;

    // uses buffers of the form {
    // _Atomic_counter_t _Ref_count
    // _Delta1_t _Delta1
    // _Diff _Delta2[_Pattern_size] // not used for Boyer-Moore-Horspool
    // }

    template <bool _Build_delta2>
    static void* _Build_boyer_moore(_RanItPat _First, _RanItPat _Last, _Hash_ty _Hash_fn, _Pred_eq _Eq) {
        // builds data tables for the Boyer-Moore string search algorithm
        _Adl_verify_range(_First, _Last);
        const auto _UFirst       = _Get_unwrapped(_First);
        const auto _Pat_size_raw = _Get_unwrapped(_Last) - _UFirst;
        using _CT                = common_type_t<_Iter_diff_t<_RanItPat>, size_t>;
        if (static_cast<_CT>(_Pat_size_raw) > static_cast<_CT>(SIZE_MAX)) {
            _Xbad_alloc();
        }

        const auto _Pat_size = static_cast<size_t>(_Pat_size_raw);
        size_t _Buf_size     = 0;
        _Add_alloc_size<_Atomic_counter_t>(_Buf_size);
        _Add_alloc_size<_Delta1_t>(_Buf_size);
        if (_Build_delta2) {
            _Add_alloc_size<_Diff>(_Buf_size, _Pat_size);
        }

        unique_ptr<void, _Global_delete> _Buf_bytes(::operator new(_Buf_size));
        void* _Buf                                      = _Buf_bytes.get();
        *_Decode_aligned_block<_Atomic_counter_t>(_Buf) = 1;
        void* const _Delta1                             = _Decode_aligned_block<_Delta1_t>(_Buf);
        if (_Build_delta2) {
            _Build_boyer_moore_delta_2_table(
                _Decode_aligned_block<_Diff>(_Buf, _Pat_size), _UFirst, _Pat_size_raw, _Eq);
        }

        ::new (_Delta1) _Delta1_t(_First, _UFirst, _Pat_size_raw, _STD move(_Hash_fn), _STD move(_Eq));
        return _Buf_bytes.release();
    }

    template <class _RanItHaystack>
    static pair<_RanItHaystack, _RanItHaystack> _Use_boyer_moore(
        void* _Data, _RanItHaystack _First, _RanItHaystack _Last) {
        // decodes data tables for the Boyer-Moore string search algorithm
        (void) _Decode_aligned_block<_Atomic_counter_t>(_Data);
        const auto _Delta1 = _Decode_aligned_block<_Delta1_t>(_Data);
        const auto _Delta2 = _Decode_aligned_block<_Diff>(_Data, static_cast<size_t>(_Delta1->_Pat_size));
        return _Boyer_moore_search(*_Delta1, _Delta2, _First, _Last);
    }

    template <class _RanItHaystack>
    static pair<_RanItHaystack, _RanItHaystack> _Use_boyer_moore_horspool(
        void* _Data, _RanItHaystack _First, _RanItHaystack _Last) {
        // decodes data tables for the Boyer-Moore string search algorithm
        (void) _Decode_aligned_block<_Atomic_counter_t>(_Data);
        const auto _Delta1 = _Decode_aligned_block<_Delta1_t>(_Data);
        return _Boyer_moore_horspool_search(*_Delta1, _First, _Last);
    }

    static void _Destroy(void* const _Base) noexcept {
        // destroys data tables for either the Boyer-Moore or Boyer-Moore-Horspool string search algorithms
        void* _Data = _Base;
        (void) _Decode_aligned_block<_Atomic_counter_t>(_Data);
        _Decode_aligned_block<_Delta1_t>(_Data)->~_Delta1_t();
        ::operator delete(_Base);
    }
};

template <class _RanItPat, class _Hash_ty, class _Pred_eq>
using _Boyer_moore_traits_char_mode = _Single_delta1_type_boyer_moore_traits<_RanItPat, _Hash_ty, _Pred_eq,
    _Boyer_moore_flat_delta_1_table<_RanItPat, 256>>;

template <class _RanItPat>
struct _Boyer_moore_traits_wchar_t_mode {
    using _Value_t       = _Iter_value_t<_RanItPat>;
    using _Diff          = _Iter_diff_t<_RanItPat>;
    using _Big_table_t   = _Boyer_moore_flat_delta_1_table<_RanItPat, 65536>;
    using _Small_table_t = _Boyer_moore_flat_delta_1_table<_RanItPat, 256>;

    // uses buffers of the form {
    // _Atomic_counter_t _Ref_count
    // bool _Use_large_table // true if anything in the pattern is > 255
    // conditional_t<_Use_large_table, _Big_table_t, _Small_table_t> _Delta1
    // _Diff _Delta2[_Pattern_size] // not used for Boyer-Moore-Horspool
    // }

    template <bool _Build_delta2>
    static void* _Build_boyer_moore(_RanItPat _First, _RanItPat _Last, _Unused_parameter, _Unused_parameter) {
        // builds data tables for the Boyer-Moore string search algorithm
        _Adl_verify_range(_First, _Last);
        const auto _UFirst       = _Get_unwrapped(_First);
        const auto _ULast        = _Get_unwrapped(_Last);
        const auto _Pat_size_raw = _ULast - _UFirst;
        using _CT                = common_type_t<_Iter_diff_t<_RanItPat>, size_t>;
        if (static_cast<_CT>(_Pat_size_raw) > static_cast<_CT>(SIZE_MAX)) {
            _Xbad_alloc();
        }

        const auto _Pat_size = static_cast<size_t>(_Pat_size_raw);
        size_t _Buf_size     = 0;
        _Add_alloc_size<_Atomic_counter_t>(_Buf_size);
        bool _Use_large_table = false;

        for (auto _Temp = _UFirst; _Temp != _ULast; ++_Temp) {
            if (_Unsigned_value(*_Temp) > 255) {
                _Use_large_table = true;
                break;
            }
        }

        _Add_alloc_size<bool>(_Buf_size);
        if (_Use_large_table) {
            _Add_alloc_size<_Big_table_t>(_Buf_size);
        } else {
            _Add_alloc_size<_Small_table_t>(_Buf_size);
        }

        if (_Build_delta2) {
            _Add_alloc_size<_Diff>(_Buf_size, _Pat_size);
        }

        unique_ptr<void, _Global_delete> _Buf_bytes(::operator new(_Buf_size));
        void* _Buf                                      = _Buf_bytes.get();
        *_Decode_aligned_block<_Atomic_counter_t>(_Buf) = 1;
        *_Decode_aligned_block<bool>(_Buf)              = _Use_large_table;
        if (_Use_large_table) {
            using _Delta1_t = _Big_table_t;
            ::new (static_cast<void*>(_Decode_aligned_block<_Delta1_t>(_Buf)))
                _Delta1_t(_First, _UFirst, _Pat_size_raw, {}, {});
        } else {
            using _Delta1_t = _Small_table_t;
            ::new (static_cast<void*>(_Decode_aligned_block<_Delta1_t>(_Buf)))
                _Delta1_t(_First, _UFirst, _Pat_size_raw, {}, {});
        }

        if (_Build_delta2) {
            equal_to<> _Eq;
            _Build_boyer_moore_delta_2_table(
                _Decode_aligned_block<_Diff>(_Buf, _Pat_size), _UFirst, _Pat_size_raw, _Eq);
        }

        return _Buf_bytes.release();
    }

    template <class _RanItHaystack>
    static pair<_RanItHaystack, _RanItHaystack> _Use_boyer_moore(
        void* _Data, _RanItHaystack _First, _RanItHaystack _Last) {
        // decodes data tables for the Boyer-Moore string search algorithm
        (void) _Decode_aligned_block<_Atomic_counter_t>(_Data);
        if (*_Decode_aligned_block<bool>(_Data)) {
            const auto _Delta1 = _Decode_aligned_block<_Big_table_t>(_Data);
            const auto _Delta2 = _Decode_aligned_block<_Diff>(_Data, static_cast<size_t>(_Delta1->_Pat_size));
            return _Boyer_moore_search(*_Delta1, _Delta2, _First, _Last);
        } else {
            const auto _Delta1 = _Decode_aligned_block<_Small_table_t>(_Data);
            const auto _Delta2 = _Decode_aligned_block<_Diff>(_Data, static_cast<size_t>(_Delta1->_Pat_size));
            return _Boyer_moore_search(*_Delta1, _Delta2, _First, _Last);
        }
    }

    template <class _RanItHaystack>
    static pair<_RanItHaystack, _RanItHaystack> _Use_boyer_moore_horspool(
        void* _Data, _RanItHaystack _First, _RanItHaystack _Last) {
        // decodes data tables for the Boyer-Moore string search algorithm
        (void) _Decode_aligned_block<_Atomic_counter_t>(_Data);
        if (*_Decode_aligned_block<bool>(_Data)) {
            const auto _Delta1 = _Decode_aligned_block<_Big_table_t>(_Data);
            return _Boyer_moore_horspool_search(*_Delta1, _First, _Last);
        } else {
            const auto _Delta1 = _Decode_aligned_block<_Small_table_t>(_Data);
            return _Boyer_moore_horspool_search(*_Delta1, _First, _Last);
        }
    }

    static void _Destroy(void* const _Base) noexcept {
        // destroys data tables for either the Boyer-Moore or Boyer-Moore-Horspool string search algorithms
        void* _Data = _Base;
        (void) _Decode_aligned_block<_Atomic_counter_t>(_Data);
        if (*_Decode_aligned_block<bool>(_Data)) {
            const auto _Delta1 = _Decode_aligned_block<_Big_table_t>(_Data);
            _Delta1->~_Big_table_t();
        } else {
            const auto _Delta1 = _Decode_aligned_block<_Small_table_t>(_Data);
            _Delta1->~_Small_table_t();
        }
        static_assert(is_trivially_destructible_v<_Diff>, "allows Boyer-Moore and Boyer-Moore-Horspool to "
                                                          "share cleanup functions");
        ::operator delete(_Base);
    }
};

template <class _RanItPat, class _Hash_ty, class _Pred_eq>
using _Boyer_moore_traits_general_mode = _Single_delta1_type_boyer_moore_traits<_RanItPat, _Hash_ty, _Pred_eq,
    _Boyer_moore_hash_delta_1_table<_RanItPat, _Hash_ty, _Pred_eq>>;

template <class _RanItPat, class _Hash_ty, class _Pred_eq, class _Value_t = _Iter_value_t<_RanItPat>>
using _Boyer_moore_traits =
    conditional_t<is_integral_v<_Value_t> && sizeof(_Value_t) <= 2
                      && (is_same_v<equal_to<>, _Pred_eq> || is_same_v<equal_to<_Value_t>, _Pred_eq>),
        conditional_t<sizeof(_Value_t) == 1, _Boyer_moore_traits_char_mode<_RanItPat, _Hash_ty, _Pred_eq>,
            _Boyer_moore_traits_wchar_t_mode<_RanItPat>>,
        _Boyer_moore_traits_general_mode<_RanItPat, _Hash_ty, _Pred_eq>>;


template <class _RanItPat, class _Hash_ty = hash<_Iter_value_t<_RanItPat>>, class _Pred_eq = equal_to<>>
class boyer_moore_searcher {
public:
    boyer_moore_searcher(
        const _RanItPat _First, const _RanItPat _Last, _Hash_ty _Hash_fn = _Hash_ty(), _Pred_eq _Eq = _Pred_eq())
        : _Data(_Traits::template _Build_boyer_moore<true>(_First, _Last, _STD move(_Hash_fn), _STD move(_Eq))) {
        // preprocess a pattern for use with the Boyer-Moore string search algorithm
    }

    boyer_moore_searcher(const boyer_moore_searcher& _Other) noexcept // strengthened
        : _Data(_Other._Data) {
        _MT_INCR(*static_cast<_Atomic_counter_t*>(_Data));
    }

    ~boyer_moore_searcher() noexcept {
        if (_MT_DECR(*static_cast<_Atomic_counter_t*>(_Data)) == 0) {
            _Traits::_Destroy(_Data);
        }
    }

    boyer_moore_searcher& operator=(const boyer_moore_searcher& _Other) noexcept /* strengthened */ {
        boyer_moore_searcher _Cpy(_Other);
        swap(_Data, _Cpy._Data);
        return *this;
    }

    template <class _RanItHaystack>
    _NODISCARD pair<_RanItHaystack, _RanItHaystack> operator()(
        const _RanItHaystack _First, const _RanItHaystack _Last) const {
        // search for the preprocessed pattern in [_First, _Last)
        return _Traits::_Use_boyer_moore(_Data, _First, _Last);
    }

private:
    using _Traits = _Boyer_moore_traits<_RanItPat, _Hash_ty, _Pred_eq>;
    void* _Data;
};


template <class _RanItPat, class _Hash_ty = hash<_Iter_value_t<_RanItPat>>, class _Pred_eq = equal_to<>>
class boyer_moore_horspool_searcher { // equivalent to Boyer-Moore without the second table
public:
    boyer_moore_horspool_searcher(
        const _RanItPat _First, const _RanItPat _Last, _Hash_ty _Hash_fn = _Hash_ty(), _Pred_eq _Eq = _Pred_eq())
        : _Data(_Traits::template _Build_boyer_moore<false>(_First, _Last, _STD move(_Hash_fn), _STD move(_Eq))) {
        // preprocess a pattern for use with the Boyer-Moore-Horspool string search algorithm
    }

    boyer_moore_horspool_searcher(const boyer_moore_horspool_searcher& _Other) noexcept // strengthened
        : _Data(_Other._Data) {
        _MT_INCR(*static_cast<_Atomic_counter_t*>(_Data));
    }

    ~boyer_moore_horspool_searcher() noexcept {
        if (_MT_DECR(*static_cast<_Atomic_counter_t*>(_Data)) == 0) {
            _Traits::_Destroy(_Data);
        }
    }

    boyer_moore_horspool_searcher& operator=(const boyer_moore_horspool_searcher& _Other) noexcept /* strengthened */ {
        boyer_moore_horspool_searcher _Cpy(_Other);
        swap(_Data, _Cpy._Data);
        return *this;
    }

    template <class _RanItHaystack>
    _NODISCARD pair<_RanItHaystack, _RanItHaystack> operator()(
        const _RanItHaystack _First, const _RanItHaystack _Last) const {
        // search for the preprocessed pattern in [_First, _Last)
        return _Traits::_Use_boyer_moore_horspool(_Data, _First, _Last);
    }

private:
    using _Traits = _Boyer_moore_traits<_RanItPat, _Hash_ty, _Pred_eq>;
    void* _Data;
};
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    struct not_equal_to {
        // clang-format off
        template <class _Ty1, class _Ty2>
            requires equality_comparable_with<_Ty1, _Ty2>
        _NODISCARD constexpr bool operator()(_Ty1&& _Left, _Ty2&& _Right) const noexcept(noexcept(
            static_cast<bool>(static_cast<_Ty1&&>(_Left) == static_cast<_Ty2&&>(_Right)))) /* strengthened */ {
            return !static_cast<bool>(static_cast<_Ty1&&>(_Left) == static_cast<_Ty2&&>(_Right));
        }
        // clang-format on

        using is_transparent = int;
    };

    struct greater_equal {
        // clang-format off
        template <class _Ty1, class _Ty2>
            requires totally_ordered_with<_Ty1, _Ty2>
        _NODISCARD constexpr bool operator()(_Ty1&& _Left, _Ty2&& _Right) const noexcept(noexcept(
            static_cast<bool>(static_cast<_Ty1&&>(_Left) < static_cast<_Ty2&&>(_Right)))) /* strengthened */ {
            return !static_cast<bool>(static_cast<_Ty1&&>(_Left) < static_cast<_Ty2&&>(_Right));
        }
        // clang-format on

        using is_transparent = int;
    };

    struct less_equal {
        // clang-format off
        template <class _Ty1, class _Ty2>
            requires totally_ordered_with<_Ty1, _Ty2>
        _NODISCARD constexpr bool operator()(_Ty1&& _Left, _Ty2&& _Right) const noexcept(noexcept(
            static_cast<bool>(static_cast<_Ty2&&>(_Right) < static_cast<_Ty1&&>(_Left)))) /* strengthened */ {
            return !static_cast<bool>(static_cast<_Ty2&&>(_Right) < static_cast<_Ty1&&>(_Left));
        }
        // clang-format on

        using is_transparent = int;
    };
} // namespace ranges
#endif // __cpp_lib_concepts

#if _HAS_TR1_NAMESPACE
namespace _DEPRECATE_TR1_NAMESPACE tr1 {
    using _STD bad_function_call;
    using _STD bind;
    using _STD function;
    using _STD is_bind_expression;
    using _STD is_placeholder;
    using _STD mem_fn;
    using _STD swap;
    namespace placeholders {
        using namespace _STD placeholders;
    }
} // namespace tr1
#endif // _HAS_TR1_NAMESPACE

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _FUNCTIONAL_
