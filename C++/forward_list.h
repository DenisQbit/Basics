// forward_list standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _FORWARD_LIST_
#define _FORWARD_LIST_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#include <xmemory>

#if _HAS_CXX17
#include <xpolymorphic_allocator.h>
#endif // _HAS_CXX17

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
template <class _Mylist, class _Base = _Iterator_base0>
class _Flist_unchecked_const_iterator : public _Base {
public:
    using iterator_category = forward_iterator_tag;

    using _Nodeptr        = typename _Mylist::_Nodeptr;
    using value_type      = typename _Mylist::value_type;
    using difference_type = typename _Mylist::difference_type;
    using pointer         = typename _Mylist::const_pointer;
    using reference       = const value_type&;

    _Flist_unchecked_const_iterator() noexcept : _Ptr() {}

    _Flist_unchecked_const_iterator(_Nodeptr _Pnode, const _Mylist* _Plist) noexcept : _Ptr(_Pnode) {
        this->_Adopt(_Plist);
    }

    _NODISCARD reference operator*() const noexcept {
        return _Ptr->_Myval;
    }

    _NODISCARD pointer operator->() const noexcept {
        return pointer_traits<pointer>::pointer_to(**this);
    }

    _Flist_unchecked_const_iterator& operator++() noexcept {
        _Ptr = _Ptr->_Next;
        return *this;
    }

    _Flist_unchecked_const_iterator operator++(int) noexcept {
        _Flist_unchecked_const_iterator _Tmp = *this;
        _Ptr                                 = _Ptr->_Next;
        return _Tmp;
    }

    _NODISCARD bool operator==(const _Flist_unchecked_const_iterator& _Right) const noexcept {
        return _Ptr == _Right._Ptr;
    }

#if !_HAS_CXX20
    _NODISCARD bool operator!=(const _Flist_unchecked_const_iterator& _Right) const noexcept {
        return !(*this == _Right);
    }
#endif // !_HAS_CXX20

    _NODISCARD bool operator==(_Default_sentinel) const noexcept {
        return _Ptr == nullptr;
    }

#if !_HAS_CXX20
    _NODISCARD bool operator!=(_Default_sentinel) const noexcept {
        return _Ptr != nullptr;
    }
#endif // !_HAS_CXX20

    _Nodeptr _Ptr; // pointer to node
};

template <class _Mylist>
class _Flist_unchecked_iterator : public _Flist_unchecked_const_iterator<_Mylist> {
public:
    using _Mybase           = _Flist_unchecked_const_iterator<_Mylist>;
    using iterator_category = forward_iterator_tag;

    using _Nodeptr        = typename _Mylist::_Nodeptr;
    using value_type      = typename _Mylist::value_type;
    using difference_type = typename _Mylist::difference_type;
    using pointer         = typename _Mylist::pointer;
    using reference       = value_type&;

    using _Mybase::_Mybase;

    _NODISCARD reference operator*() const noexcept {
        return const_cast<reference>(_Mybase::operator*());
    }

    _NODISCARD pointer operator->() const noexcept {
        return pointer_traits<pointer>::pointer_to(**this);
    }

    _Flist_unchecked_iterator& operator++() noexcept {
        _Mybase::operator++();
        return *this;
    }

    _Flist_unchecked_iterator operator++(int) noexcept {
        _Flist_unchecked_iterator _Tmp = *this;
        _Mybase::operator++();
        return _Tmp;
    }
};

template <class _Mylist>
class _Flist_const_iterator : public _Flist_unchecked_const_iterator<_Mylist, _Iterator_base> {
public:
    using _Mybase           = _Flist_unchecked_const_iterator<_Mylist, _Iterator_base>;
    using iterator_category = forward_iterator_tag;

    using _Nodeptr        = typename _Mylist::_Nodeptr;
    using value_type      = typename _Mylist::value_type;
    using difference_type = typename _Mylist::difference_type;
    using pointer         = typename _Mylist::const_pointer;
    using reference       = const value_type&;

    using _Mybase::_Mybase;

    _NODISCARD reference operator*() const noexcept {
#if _ITERATOR_DEBUG_LEVEL == 2
        const auto _Mycont = static_cast<const _Mylist*>(this->_Getcont());
        _STL_ASSERT(_Mycont, "cannot dereference value-initialized forward_list iterator");
        _STL_VERIFY(this->_Ptr != _Mycont->_Before_head(), "cannot dereference forward_list before_begin");
#endif // _ITERATOR_DEBUG_LEVEL == 2

        return this->_Ptr->_Myval;
    }

    _Flist_const_iterator& operator++() noexcept {
#if _ITERATOR_DEBUG_LEVEL == 2
        _STL_VERIFY(this->_Getcont(), "forward_list iterator not incrementable");
#endif // _ITERATOR_DEBUG_LEVEL == 2

        this->_Ptr = this->_Ptr->_Next;
        return *this;
    }

    _Flist_const_iterator operator++(int) noexcept {
        _Flist_const_iterator _Tmp = *this;
        this->_Ptr                 = this->_Ptr->_Next;
        return _Tmp;
    }

    _NODISCARD bool operator==(const _Flist_const_iterator& _Right) const noexcept {
#if _ITERATOR_DEBUG_LEVEL == 2
        _STL_VERIFY(this->_Getcont() == _Right._Getcont(), "forward_list iterators incompatible");
#endif // _ITERATOR_DEBUG_LEVEL == 2

        return this->_Ptr == _Right._Ptr;
    }

#if !_HAS_CXX20
    _NODISCARD bool operator!=(const _Flist_const_iterator& _Right) const noexcept {
        return !(*this == _Right);
    }
#endif // !_HAS_CXX20

#if _ITERATOR_DEBUG_LEVEL == 2
    friend void _Verify_range(const _Flist_const_iterator& _First, const _Flist_const_iterator& _Last) noexcept {
        _STL_VERIFY(
            _First._Getcont() == _Last._Getcont(), "forward_list iterators in range are from different containers");
    }
#endif // _ITERATOR_DEBUG_LEVEL == 2

    using _Prevent_inheriting_unwrap = _Flist_const_iterator;

    _NODISCARD _Flist_unchecked_const_iterator<_Mylist> _Unwrapped() const noexcept {
        return _Flist_unchecked_const_iterator<_Mylist>(this->_Ptr, static_cast<const _Mylist*>(this->_Getcont()));
    }

    void _Seek_to(const _Flist_unchecked_const_iterator<_Mylist> _It) noexcept {
        this->_Ptr = _It._Ptr;
    }
};

template <class _Mylist>
class _Flist_iterator : public _Flist_const_iterator<_Mylist> {
public:
    using _Mybase           = _Flist_const_iterator<_Mylist>;
    using iterator_category = forward_iterator_tag;

    using _Nodeptr        = typename _Mylist::_Nodeptr;
    using value_type      = typename _Mylist::value_type;
    using difference_type = typename _Mylist::difference_type;
    using pointer         = typename _Mylist::pointer;
    using reference       = value_type&;

    using _Mybase::_Mybase;

    _NODISCARD reference operator*() const noexcept {
        return const_cast<reference>(_Mybase::operator*());
    }

    _NODISCARD pointer operator->() const noexcept {
        return pointer_traits<pointer>::pointer_to(**this);
    }

    _Flist_iterator& operator++() noexcept {
        _Mybase::operator++();
        return *this;
    }

    _Flist_iterator operator++(int) noexcept {
        _Flist_iterator _Tmp = *this;
        _Mybase::operator++();
        return _Tmp;
    }

    using _Prevent_inheriting_unwrap = _Flist_iterator;

    _NODISCARD _Flist_unchecked_iterator<_Mylist> _Unwrapped() const noexcept {
        return _Flist_unchecked_iterator<_Mylist>(this->_Ptr, static_cast<const _Mylist*>(this->_Getcont()));
    }
};

template <class _Value_type, class _Size_type, class _Difference_type, class _Pointer, class _Const_pointer,
    class _Reference, class _Const_reference, class _Nodeptr_type>
struct _Flist_iter_types {
    using value_type      = _Value_type;
    using size_type       = _Size_type;
    using difference_type = _Difference_type;
    using pointer         = _Pointer;
    using const_pointer   = _Const_pointer;
    using _Nodeptr        = _Nodeptr_type;
};

template <class _Value_type, class _Voidptr>
struct _Flist_node { // forward_list node
    using _Nodeptr = _Rebind_pointer_t<_Voidptr, _Flist_node>;

    _Nodeptr _Next; // successor node
    _Value_type _Myval; // the stored value

    _Flist_node(const _Flist_node&) = delete;
    _Flist_node& operator=(const _Flist_node&) = delete;

    template <class _Alnode>
    static void _Freenode(_Alnode& _Al, _Nodeptr _Pnode) noexcept {
        _Destroy_in_place(_Pnode->_Next);
        allocator_traits<_Alnode>::destroy(_Al, _STD addressof(_Pnode->_Myval));
        _Al.deallocate(_Pnode, 1);
    }
};

template <class _Ty>
struct _Flist_simple_types : _Simple_types<_Ty> {
    using _Node    = _Flist_node<_Ty, void*>;
    using _Nodeptr = _Node*;
};

template <class _Val_types>
class _Flist_val : public _Container_base {
public:
    using _Nodeptr = typename _Val_types::_Nodeptr;
    using _Node    = typename pointer_traits<_Nodeptr>::element_type;

    using value_type      = typename _Val_types::value_type;
    using size_type       = typename _Val_types::size_type;
    using difference_type = typename _Val_types::difference_type;
    using pointer         = typename _Val_types::pointer;
    using const_pointer   = typename _Val_types::const_pointer;
    using reference       = value_type&;
    using const_reference = const value_type&;

    _Flist_val() noexcept : _Myhead() {} // initialize data

    _Nodeptr _Before_head() const noexcept { // return pointer to the "before begin" pseudo node
        return pointer_traits<_Nodeptr>::pointer_to(reinterpret_cast<_Node&>(const_cast<_Nodeptr&>(_Myhead)));
    }

#if _ITERATOR_DEBUG_LEVEL == 2
    void _Orphan_ptr(_Nodeptr _Ptr) noexcept { // orphan iterators with specified node pointers
        const auto _BHead = _Before_head();
        _Lockit _Lock(_LOCK_DEBUG);
        _Iterator_base12** _Pnext = &this->_Myproxy->_Myfirstiter;
        while (*_Pnext) {
            const auto _Pnextptr = static_cast<_Flist_const_iterator<_Flist_val>&>(**_Pnext)._Ptr;
            if (_Pnextptr == _BHead || (_Ptr != nullptr && _Pnextptr != _Ptr)) {
                _Pnext = &(*_Pnext)->_Mynextiter;
            } else { // orphan the iterator
                (*_Pnext)->_Myproxy = nullptr;
                *_Pnext             = (*_Pnext)->_Mynextiter;
            }
        }
    }
#endif // _ITERATOR_DEBUG_LEVEL == 2

    template <class _Pr2>
    static _Nodeptr _Inplace_merge(_Nodeptr _BFirst1, const _Nodeptr _BMid, const _Nodeptr _BLast, _Pr2 _Pred) {
        // Merge the sorted ranges (_BFirst1, _BMid] and (_BMid, _BLast)
        // Returns one before the new logical end of the range.
        auto _First2 = _BMid->_Next;
        for (;;) { // process 1 splice
            _Nodeptr _First1;
            for (;;) { // advance _BFirst1 over elements already in position
                if (_BFirst1 == _BMid) {
                    return _BLast;
                }

                _First1 = _BFirst1->_Next;
                if (_DEBUG_LT_PRED(_Pred, _First2->_Myval, _First1->_Myval)) {
                    // _First2->_Myval is out of order
                    break;
                }

                // _First1->_Myval is already in position; advance
                _BFirst1 = _First1;
            }

            // find the end of the "run" of elements less than _First1->_Myval in the 2nd range
            auto _BRun_end = _First2;
            _Nodeptr _Run_end;
            for (;;) {
                _Run_end = _BRun_end->_Next;
                if (_BRun_end == _BLast) {
                    break;
                }

                if (!_DEBUG_LT_PRED(_Pred, _Run_end->_Myval, _First1->_Myval)) {
                    // _Run_end is the first element in (_BMid->_Myval, _BLast->_Myval) that shouldn't precede
                    // _First1->_Myval.
                    // After the splice _First1->_Myval will be in position and must not be compared again.
                    break;
                }

                _BRun_end = _Run_end;
            }

            _BMid->_Next     = _Run_end; // snip out the run from its old position
            _BFirst1->_Next  = _First2; // insert into new position
            _BRun_end->_Next = _First1;
            if (_BRun_end == _BLast) {
                return _BMid;
            }

            _BFirst1 = _First1;
            _First2  = _Run_end;
        }
    }

    template <class _Pr2>
    static _Nodeptr _Sort2(const _Nodeptr _BFirst, _Pr2 _Pred) {
        // Sort (_BFirst, _BFirst + 2], unless nullptr is encountered.
        // Returns a pointer one before the end of the sorted region.
        const auto _First1 = _BFirst->_Next;
        if (!_First1) {
            return _BFirst;
        }

        auto _First2 = _First1->_Next;
        if (!_First2 || !_DEBUG_LT_PRED(_Pred, _First2->_Myval, _First1->_Myval)) {
            return _First1;
        }

        // swap _First2 and _First1
        _First1->_Next = _First2->_Next; // snip out *_First2
        _BFirst->_Next = _First2; // insert *_First2 before *_First1
        _First2->_Next = _First1;
        return _First2;
    }

    template <class _Pr2>
    static _Nodeptr _Sort(const _Nodeptr _BFirst, size_type _Bound, _Pr2 _Pred) {
        // Sort (_BFirst, _BFirst + _Bound), unless nullptr is encountered.
        // Returns a pointer one before the end of the sorted region.
        if (_Bound <= 2) {
            return _Sort2(_BFirst, _Pred);
        }

        const auto _Half_bound = _Bound / 2;
        const auto _BMid       = _Sort(_BFirst, _Half_bound, _Pred);
        if (!_BMid->_Next) {
            return _BMid;
        }

        const auto _BLast = _Sort(_BMid, _Half_bound, _Pred);
        return _Inplace_merge(_BFirst, _BMid, _BLast, _Pred);
    }

    template <class _Pr2>
    static void _Sort(_Nodeptr _BFirst, _Pr2 _Pred) {
        auto _BMid       = _Sort2(_BFirst, _Pred);
        size_type _Bound = 2;
        do {
            if (!_BMid->_Next) {
                return;
            }

            const auto _BLast = _Sort(_BMid, _Bound, _Pred);
            _BMid             = _Inplace_merge(_BFirst, _BMid, _BLast, _Pred);
            _Bound <<= 1;
        } while (_Bound != 0);
    }

    _Nodeptr _Myhead; // pointer to head node
};

template <class _Alnode>
struct _Flist_insert_after_op2 {
    // forward_list insert-after operation which maintains exception safety
    using _Alnode_traits = allocator_traits<_Alnode>;
    using pointer        = typename _Alnode_traits::pointer;
    using value_type     = typename _Alnode_traits::value_type;

    explicit _Flist_insert_after_op2(_Alnode& _Al_) : _Al(_Al_), _Tail() {}

    _Flist_insert_after_op2(const _Flist_insert_after_op2&) = delete;
    _Flist_insert_after_op2& operator=(const _Flist_insert_after_op2&) = delete;

    template <class... _CArgT>
    void _Append_n(typename _Alnode_traits::size_type _Count, const _CArgT&... _Carg) {
        // Append _Count elements, constructed from _Carg
        if (_Count <= 0) {
            return;
        }

        _Alloc_construct_ptr<_Alnode> _Newnode(_Al);
        if (_Tail == pointer{}) {
            _Newnode._Allocate(); // throws
            _Alnode_traits::construct(_Al, _STD addressof(_Newnode._Ptr->_Myval), _Carg...); // throws
            _Head = _Newnode._Ptr;
            _Tail = _Newnode._Ptr;
            --_Count;
        }

        for (; 0 < _Count; --_Count) {
            _Newnode._Allocate(); // throws
            _Alnode_traits::construct(_Al, _STD addressof(_Newnode._Ptr->_Myval), _Carg...); // throws
            _Construct_in_place(_Tail->_Next, _Newnode._Ptr);
            _Tail = _Newnode._Ptr;
        }

        (void) _Newnode._Release();
    }

    template <class _InIt, class _Sentinel>
    void _Append_range_unchecked(_InIt _First, const _Sentinel _Last) {
        // Append the values in [_First, _Last)
        if (_First == _Last) { // throws
            return;
        }

        _Alloc_construct_ptr<_Alnode> _Newnode(_Al);
        if (_Tail == pointer{}) {
            _Newnode._Allocate(); // throws
            _Alnode_traits::construct(_Al, _STD addressof(_Newnode._Ptr->_Myval), *_First); // throws
            const auto _Newhead = _Newnode._Release();
            _Head               = _Newhead;
            _Tail               = _Newhead;
            ++_First; // throws
        }

        while (_First != _Last) { // throws
            _Newnode._Allocate(); // throws
            _Alnode_traits::construct(_Al, _STD addressof(_Newnode._Ptr->_Myval), *_First); // throws
            const auto _Newtail = _Newnode._Release();
            _Construct_in_place(_Tail->_Next, _Newtail);
            _Tail = _Newtail;
            ++_First; // throws
        }
    }

    pointer _Attach_after(pointer _After) noexcept {
        // Attaches the elements in *this after _After, and resets *this to the default-initialized state
        const auto _Local_tail = _Tail;
        if (_Local_tail == pointer{}) {
            return _After;
        }

        _Construct_in_place(_Local_tail->_Next, _After->_Next);
        _After->_Next = _Head;
        _Tail         = pointer{};

        return _Local_tail;
    }

    ~_Flist_insert_after_op2() {
        if (_Tail == pointer{}) {
            return;
        }

        _Construct_in_place(_Tail->_Next, pointer{});
        pointer _Subject = _Head;
        while (_Subject) {
            value_type::_Freenode(_Al, _STD exchange(_Subject, _Subject->_Next));
        }
    }

private:
    _Alnode& _Al;
    pointer _Tail; // Points to the most recently constructed node. If pointer{}, the value of _Head is indeterminate.
                   // _Tail->_Next is not constructed.
    pointer _Head; // Points at the first constructed node.
};

template <class _Ty, class _Alloc = allocator<_Ty>>
class forward_list { // singly linked list
private:
    using _Alty          = _Rebind_alloc_t<_Alloc, _Ty>;
    using _Alty_traits   = allocator_traits<_Alty>;
    using _Node          = _Flist_node<_Ty, typename _Alty_traits::void_pointer>;
    using _Alnode        = _Rebind_alloc_t<_Alloc, _Node>;
    using _Alnode_traits = allocator_traits<_Alnode>;
    using _Nodeptr       = typename _Alnode_traits::pointer;

    static_assert(!_ENFORCE_MATCHING_ALLOCATORS || is_same_v<_Ty, typename _Alloc::value_type>,
        _MISMATCHED_ALLOCATOR_MESSAGE("forward_list<T, Allocator>", "T"));

    using _Scary_val = _Flist_val<conditional_t<_Is_simple_alloc_v<_Alnode>, _Flist_simple_types<_Ty>,
        _Flist_iter_types<_Ty, typename _Alty_traits::size_type, typename _Alty_traits::difference_type,
            typename _Alty_traits::pointer, typename _Alty_traits::const_pointer, _Ty&, const _Ty&, _Nodeptr>>>;

public:
    using value_type      = _Ty;
    using allocator_type  = _Alloc;
    using size_type       = typename _Alty_traits::size_type;
    using difference_type = typename _Alty_traits::difference_type;
    using pointer         = typename _Alty_traits::pointer;
    using const_pointer   = typename _Alty_traits::const_pointer;
    using reference       = value_type&;
    using const_reference = const value_type&;

    using iterator                  = _Flist_iterator<_Scary_val>;
    using const_iterator            = _Flist_const_iterator<_Scary_val>;
    using _Unchecked_iterator       = _Flist_unchecked_iterator<_Scary_val>;
    using _Unchecked_const_iterator = _Flist_unchecked_const_iterator<_Scary_val>;

    forward_list() noexcept(is_nothrow_default_constructible_v<_Alnode>) // strengthened
        : _Mypair(_Zero_then_variadic_args_t{}) {
        _Alloc_proxy();
    }

    explicit forward_list(_CRT_GUARDOVERFLOW size_type _Count, const _Alloc& _Al = _Alloc())
        : _Mypair(_One_then_variadic_args_t{}, _Al) { // construct list from _Count * _Ty(), optional allocator
        _Flist_insert_after_op2<_Alnode> _Insert_op(_Getal());
        _Insert_op._Append_n(_Count);
        _Alloc_proxy();
        _Insert_op._Attach_after(_Mypair._Myval2._Before_head());
    }

    forward_list(_CRT_GUARDOVERFLOW size_type _Count, const _Ty& _Val)
        : _Mypair(_Zero_then_variadic_args_t{}) { // construct list from _Count * _Val
        _Flist_insert_after_op2<_Alnode> _Insert_op(_Getal());
        _Insert_op._Append_n(_Count, _Val);
        _Alloc_proxy();
        _Insert_op._Attach_after(_Mypair._Myval2._Before_head());
    }

    forward_list(_CRT_GUARDOVERFLOW size_type _Count, const _Ty& _Val, const _Alloc& _Al)
        : _Mypair(_One_then_variadic_args_t{}, _Al) { // construct list from _Count * _Val, allocator
        _Flist_insert_after_op2<_Alnode> _Insert_op(_Getal());
        _Insert_op._Append_n(_Count, _Val);
        _Alloc_proxy();
        _Insert_op._Attach_after(_Mypair._Myval2._Before_head());
    }

    explicit forward_list(const _Alloc& _Al) noexcept // strengthened
        : _Mypair(_One_then_variadic_args_t{}, _Al) {
        _Alloc_proxy();
    }

    forward_list(const forward_list& _Right)
        : _Mypair(_One_then_variadic_args_t{}, _Alnode_traits::select_on_container_copy_construction(_Right._Getal())) {
        _Flist_insert_after_op2<_Alnode> _Insert_op(_Getal());
        _Insert_op._Append_range_unchecked(_Right._Unchecked_begin(), _Right._Unchecked_end());
        _Alloc_proxy();
        _Insert_op._Attach_after(_Mypair._Myval2._Before_head());
    }

    forward_list(const forward_list& _Right, const _Alloc& _Al) : _Mypair(_One_then_variadic_args_t{}, _Al) {
        _Flist_insert_after_op2<_Alnode> _Insert_op(_Getal());
        _Insert_op._Append_range_unchecked(_Right._Unchecked_begin(), _Right._Unchecked_end());
        _Alloc_proxy();
        _Insert_op._Attach_after(_Mypair._Myval2._Before_head());
    }

    template <class _Iter, enable_if_t<_Is_iterator_v<_Iter>, int> = 0>
    forward_list(_Iter _First, _Iter _Last) : _Mypair(_Zero_then_variadic_args_t{}) {
        _Adl_verify_range(_First, _Last);
        _Flist_insert_after_op2<_Alnode> _Insert_op(_Getal());
        _Insert_op._Append_range_unchecked(_Get_unwrapped(_First), _Get_unwrapped(_Last));
        _Alloc_proxy();
        _Insert_op._Attach_after(_Mypair._Myval2._Before_head());
    }

    template <class _Iter, enable_if_t<_Is_iterator_v<_Iter>, int> = 0>
    forward_list(_Iter _First, _Iter _Last, const _Alloc& _Al) : _Mypair(_One_then_variadic_args_t{}, _Al) {
        _Adl_verify_range(_First, _Last);
        _Flist_insert_after_op2<_Alnode> _Insert_op(_Getal());
        _Insert_op._Append_range_unchecked(_Get_unwrapped(_First), _Get_unwrapped(_Last));
        _Alloc_proxy();
        _Insert_op._Attach_after(_Mypair._Myval2._Before_head());
    }

    forward_list(forward_list&& _Right) noexcept // strengthened
        : _Mypair(_One_then_variadic_args_t{}, _STD move(_Right._Getal())) {
        _Alloc_proxy();
        _Take_head(_Right);
    }

    forward_list(forward_list&& _Right, const _Alloc& _Al) noexcept(
        _Alnode_traits::is_always_equal::value) // strengthened
        : _Mypair(_One_then_variadic_args_t{}, _Al) {
        if constexpr (!_Alty_traits::is_always_equal::value) {
            if (_Getal() != _Right._Getal()) {
                _Flist_insert_after_op2<_Alnode> _Insert_op(_Getal());
                _Insert_op._Append_range_unchecked(
                    _STD make_move_iterator(_Right._Unchecked_begin()), _Default_sentinel{});
                _Alloc_proxy();
                _Insert_op._Attach_after(_Mypair._Myval2._Before_head());
                return;
            }
        }

        _Alloc_proxy();
        _Take_head(_Right);
    }

private:
    void _Move_assign(forward_list& _Right, _Equal_allocators) noexcept {
        clear();
        _Pocma(_Getal(), _Right._Getal());
        _Take_head(_Right);
    }

    void _Move_assign(forward_list& _Right, _Propagate_allocators) noexcept {
        if (_Getal() == _Right._Getal()) {
            _Move_assign(_Right, _Equal_allocators{});
        } else {
            _Mypair._Myval2._Orphan_all();
            clear();
            _Mypair._Myval2._Reload_proxy(
                _GET_PROXY_ALLOCATOR(_Alty, _Getal()), _GET_PROXY_ALLOCATOR(_Alty, _Right._Getal()));
            _Pocma(_Getal(), _Right._Getal());
            _Take_head(_Right);
        }
    }

    void _Move_assign(forward_list& _Right, _No_propagate_allocators) {
        if (_Getal() == _Right._Getal()) {
            _Move_assign(_Right, _Equal_allocators{});
        } else {
            _Assign_unchecked(_STD make_move_iterator(_Right._Unchecked_begin()), _Right._Unchecked_end());
        }
    }

public:
    forward_list& operator=(forward_list&& _Right) noexcept(
        noexcept(_Move_assign(_Right, _Choose_pocma<_Alnode>{}))) /* strengthened */ {
        if (this != _STD addressof(_Right)) {
            _Move_assign(_Right, _Choose_pocma<_Alnode>{});
        }

        return *this;
    }

private:
    void _Take_head(forward_list& _Right) noexcept { // take contents from _Right, same allocator
        _Swap_proxy_and_iterators(_Right);
        _Mypair._Myval2._Myhead = _STD exchange(_Right._Mypair._Myval2._Myhead, nullptr);
    }

public:
    void push_front(_Ty&& _Val) { // insert element at beginning
        _Insert_after(_Mypair._Myval2._Before_head(), _STD move(_Val));
    }

    iterator insert_after(const_iterator _Where, _Ty&& _Val) { // insert _Val after _Where
        return emplace_after(_Where, _STD move(_Val));
    }

    template <class... _Valty>
    decltype(auto) emplace_front(_Valty&&... _Val) { // insert element at beginning
        _Insert_after(_Mypair._Myval2._Before_head(), _STD forward<_Valty>(_Val)...);

#if _HAS_CXX17
        return front();
#endif // _HAS_CXX17
    }

    template <class... _Valty>
    iterator emplace_after(const_iterator _Where, _Valty&&... _Val) { // insert element after _Where
#if _ITERATOR_DEBUG_LEVEL == 2
        _STL_VERIFY(
            _Where._Getcont() == _STD addressof(_Mypair._Myval2), "forward_list insert_after iterator outside range");
#endif // _ITERATOR_DEBUG_LEVEL == 2
        _Insert_after(_Where._Ptr, _STD forward<_Valty>(_Val)...);
        return _Make_iter(_Where._Ptr->_Next);
    }

private:
    template <class... _Valty>
    void _Insert_after(_Nodeptr _Pnode, _Valty&&... _Val) { // insert element after _Where
        _Alloc_construct_ptr<_Alnode> _Newnode(_Getal());
        _Newnode._Allocate(); // throws
        _Alnode_traits::construct(
            _Newnode._Al, _STD addressof(_Newnode._Ptr->_Myval), _STD forward<_Valty>(_Val)...); // throws
        _Construct_in_place(_Newnode._Ptr->_Next, _Pnode->_Next);
        _Pnode->_Next = _Newnode._Release();
    }

public:
    forward_list(initializer_list<_Ty> _Ilist, const _Alloc& _Al = allocator_type())
        : _Mypair(_One_then_variadic_args_t{}, _Al) {
        auto&& _Alproxy = _GET_PROXY_ALLOCATOR(_Alnode, _Getal());
        _Container_proxy_ptr<_Alty> _Proxy(_Alproxy, _Mypair._Myval2);
        insert_after(before_begin(), _Ilist.begin(), _Ilist.end());
        _Proxy._Release();
    }

    forward_list& operator=(initializer_list<_Ty> _Ilist) {
        assign(_Ilist.begin(), _Ilist.end());
        return *this;
    }

    void assign(initializer_list<_Ty> _Ilist) {
        assign(_Ilist.begin(), _Ilist.end());
    }

    iterator insert_after(const_iterator _Where,
        initializer_list<_Ty> _Ilist) { // insert initializer_list
        return insert_after(_Where, _Ilist.begin(), _Ilist.end());
    }

    ~forward_list() noexcept {
        clear();
#if _ITERATOR_DEBUG_LEVEL != 0 // TRANSITION, ABI
        auto&& _Alproxy = _GET_PROXY_ALLOCATOR(_Alty, _Getal());
        _Delete_plain_internal(_Alproxy, _Mypair._Myval2._Myproxy);
#endif // _ITERATOR_DEBUG_LEVEL != 0
    }

private:
    void _Copy_assign(const forward_list& _Right, false_type) {
        _Pocca(_Getal(), _Right._Getal());
        _Assign_unchecked(_Right._Unchecked_begin(), _Right._Unchecked_end());
    }

    void _Copy_assign(const forward_list& _Right, true_type) {
        if (_Getal() != _Right._Getal()) {
            _Mypair._Myval2._Orphan_all();
            clear();
            _Mypair._Myval2._Reload_proxy(
                _GET_PROXY_ALLOCATOR(_Alnode, _Getal()), _GET_PROXY_ALLOCATOR(_Alnode, _Right._Getal()));
        }

        _Copy_assign(_Right, _No_propagate_allocators{});
    }

public:
    forward_list& operator=(const forward_list& _Right) {
        if (this != _STD addressof(_Right)) {
            _Copy_assign(_Right, _Choose_pocca<_Alnode>{});
        }

        return *this;
    }

    _NODISCARD iterator before_begin() noexcept {
        return iterator(_Mypair._Myval2._Before_head(), _STD addressof(_Mypair._Myval2));
    }

    _NODISCARD const_iterator before_begin() const noexcept {
        return const_iterator(_Mypair._Myval2._Before_head(), _STD addressof(_Mypair._Myval2));
    }

    _NODISCARD const_iterator cbefore_begin() const noexcept {
        return before_begin();
    }

    _NODISCARD iterator begin() noexcept {
        return iterator(_Mypair._Myval2._Myhead, _STD addressof(_Mypair._Myval2));
    }

    _NODISCARD const_iterator begin() const noexcept {
        return const_iterator(_Mypair._Myval2._Myhead, _STD addressof(_Mypair._Myval2));
    }

    _NODISCARD iterator end() noexcept {
        return iterator(nullptr, _STD addressof(_Mypair._Myval2));
    }

    _NODISCARD const_iterator end() const noexcept {
        return const_iterator(nullptr, _STD addressof(_Mypair._Myval2));
    }

    _Unchecked_iterator _Unchecked_before_begin() noexcept {
        return _Unchecked_iterator(_Mypair._Myval2._Before_head(), nullptr);
    }

    _Unchecked_iterator _Unchecked_begin() noexcept {
        return _Unchecked_iterator(_Mypair._Myval2._Myhead, nullptr);
    }

    _Unchecked_const_iterator _Unchecked_begin() const noexcept {
        return _Unchecked_const_iterator(_Mypair._Myval2._Myhead, nullptr);
    }

    _Default_sentinel _Unchecked_end() const noexcept {
        return {};
    }

    _Unchecked_const_iterator _Unchecked_end_iter() const noexcept {
        return _Unchecked_const_iterator(nullptr, nullptr);
    }

    iterator _Make_iter(_Nodeptr _Where) const noexcept {
        return iterator(_Where, _STD addressof(_Mypair._Myval2));
    }

    _NODISCARD const_iterator cbegin() const noexcept {
        return begin();
    }

    _NODISCARD const_iterator cend() const noexcept {
        return end();
    }

private:
    template <class... _Args>
    void _Resize(_CRT_GUARDOVERFLOW size_type _Newsize, const _Args&... _Vals) {
        auto& _Al = _Getal();
        auto _Ptr = _Mypair._Myval2._Before_head();
        for (;;) {
            auto _Next = _Ptr->_Next;
            if (!_Next) {
                // list too short, insert remaining _Newsize objects initialized from _Vals...
                _Flist_insert_after_op2<_Alnode> _Insert_op(_Al);
                _Insert_op._Append_n(_Newsize, _Vals...);
                _Insert_op._Attach_after(_Ptr);
                return;
            }

            if (_Newsize == 0) {
                // list is too long, erase the _Next and after
                _Ptr->_Next = nullptr;
                do {
                    const auto _Nextafter = _Next->_Next;
#if _ITERATOR_DEBUG_LEVEL == 2
                    _Mypair._Myval2._Orphan_ptr(_Next);
#endif // _ITERATOR_DEBUG_LEVEL == 2
                    _Node::_Freenode(_Al, _Next);
                    _Next = _Nextafter;
                } while (_Next);

                return;
            }

            _Ptr = _Next;
            --_Newsize;
        }
    }

public:
    void resize(_CRT_GUARDOVERFLOW size_type _Newsize) {
        _Resize(_Newsize);
    }

    void resize(_CRT_GUARDOVERFLOW size_type _Newsize, const _Ty& _Val) {
        _Resize(_Newsize, _Val);
    }

    _NODISCARD size_type max_size() const noexcept {
        return (_STD min)(
            static_cast<size_type>((numeric_limits<difference_type>::max)()), _Alnode_traits::max_size(_Getal()));
    }

    _NODISCARD bool empty() const noexcept {
        return _Mypair._Myval2._Myhead == nullptr;
    }

    _NODISCARD allocator_type get_allocator() const noexcept {
        return static_cast<allocator_type>(_Getal());
    }

    _NODISCARD reference front() noexcept /* strengthened */ {
#if _CONTAINER_DEBUG_LEVEL > 0
        _STL_VERIFY(_Mypair._Myval2._Myhead != nullptr, "front() called on empty forward_list");
#endif // _CONTAINER_DEBUG_LEVEL > 0

        return _Mypair._Myval2._Myhead->_Myval;
    }

    _NODISCARD const_reference front() const noexcept /* strengthened */ {
#if _CONTAINER_DEBUG_LEVEL > 0
        _STL_VERIFY(_Mypair._Myval2._Myhead != nullptr, "front() called on empty forward_list");
#endif // _CONTAINER_DEBUG_LEVEL > 0

        return _Mypair._Myval2._Myhead->_Myval;
    }

    void push_front(const _Ty& _Val) {
        _Insert_after(_Mypair._Myval2._Before_head(), _Val);
    }

    void pop_front() noexcept /* strengthened */ {
        _Erase_after(_Mypair._Myval2._Before_head());
    }

private:
    template <class _UIter, class _Sentinel>
    void _Assign_unchecked(_UIter _UFirst, _Sentinel _ULast) {
        auto _Myfirst = _Mypair._Myval2._Before_head();
        for (; _UFirst != _ULast; ++_UFirst) {
            auto _Next = _Myfirst->_Next;
            if (!_Myfirst->_Next) {
                _Flist_insert_after_op2<_Alnode> _Insert_op(_Getal());
                _Insert_op._Append_range_unchecked(_UFirst, _ULast);
                _Insert_op._Attach_after(_Myfirst);
                return;
            }

            _Next->_Myval = *_UFirst;
            _Myfirst      = _Next;
        }

        for (auto _To_delete = _STD exchange(_Myfirst->_Next, nullptr); _To_delete;) {
            auto _Next = _To_delete->_Next;
#if _ITERATOR_DEBUG_LEVEL == 2
            _Mypair._Myval2._Orphan_ptr(_To_delete);
#endif // _ITERATOR_DEBUG_LEVEL == 2
            _Node::_Freenode(_Getal(), _To_delete);
            _To_delete = _Next;
        }
    }

public:
    template <class _Iter, enable_if_t<_Is_iterator_v<_Iter>, int> = 0>
    void assign(_Iter _First, _Iter _Last) {
        _Adl_verify_range(_First, _Last);
        _Assign_unchecked(_Get_unwrapped(_First), _Get_unwrapped(_Last));
    }

    void assign(_CRT_GUARDOVERFLOW size_type _Count, const _Ty& _Val) {
        clear();
        insert_after(before_begin(), _Count, _Val);
    }

    iterator insert_after(const_iterator _Where, const _Ty& _Val) { // insert _Val after _Where
#if _ITERATOR_DEBUG_LEVEL == 2
        _STL_VERIFY(_Where._Getcont() == _STD addressof(_Mypair._Myval2), "insert_after location incompatible");
#endif // _ITERATOR_DEBUG_LEVEL == 2
        _Insert_after(_Where._Ptr, _Val);
        return _Make_iter(_Where._Ptr->_Next);
    }

    iterator insert_after(const_iterator _Where, _CRT_GUARDOVERFLOW size_type _Count, const _Ty& _Val) {
        // insert _Count * _Val after _Where
#if _ITERATOR_DEBUG_LEVEL == 2
        _STL_VERIFY(
            _Where._Getcont() == _STD addressof(_Mypair._Myval2), "forward_list insert_after location incompatible");
#endif // _ITERATOR_DEBUG_LEVEL == 2

        if (_Count != 0) {
            _Flist_insert_after_op2<_Alnode> _Insert_op(_Getal());
            _Insert_op._Append_n(_Count, _Val);
            _Where._Ptr = _Insert_op._Attach_after(_Where._Ptr);
        }

        return _Make_iter(_Where._Ptr);
    }

    template <class _Iter, enable_if_t<_Is_iterator_v<_Iter>, int> = 0>
    iterator insert_after(const_iterator _Where, _Iter _First, _Iter _Last) {
        // insert [_First, _Last) after _Where
#if _ITERATOR_DEBUG_LEVEL == 2
        _STL_VERIFY(
            _Where._Getcont() == _STD addressof(_Mypair._Myval2), "forward_list insert_after location incompatible");
#endif // _ITERATOR_DEBUG_LEVEL == 2

        _Adl_verify_range(_First, _Last);
        const auto _UFirst = _Get_unwrapped(_First);
        const auto _ULast  = _Get_unwrapped(_Last);
        if (_UFirst == _ULast) {
            return _Make_iter(_Where._Ptr);
        }

        _Flist_insert_after_op2<_Alnode> _Insert_op(_Getal());
        _Insert_op._Append_range_unchecked(_UFirst, _ULast);
        return _Make_iter(_Insert_op._Attach_after(_Where._Ptr));
    }

private:
    void _Erase_after(_Nodeptr _Pnode) noexcept { // erase element after _Pnode
        auto _Subject = _Pnode->_Next;
#if _ITERATOR_DEBUG_LEVEL == 2
        _Mypair._Myval2._Orphan_ptr(_Subject);
#endif // _ITERATOR_DEBUG_LEVEL == 2
        _Pnode->_Next = _Subject->_Next;
        _Node::_Freenode(_Getal(), _Subject);
    }

public:
    iterator erase_after(const_iterator _Where) noexcept /* strengthened */ {
#if _ITERATOR_DEBUG_LEVEL == 2
        _STL_VERIFY(
            _Where._Getcont() == _STD addressof(_Mypair._Myval2), "forward_list erase_after iterator outside range");
#endif // _ITERATOR_DEBUG_LEVEL == 2
        _Erase_after(_Where._Ptr);
        return _Make_iter(_Where._Ptr->_Next);
    }

    iterator erase_after(const_iterator _First, const_iterator _Last) noexcept /* strengthened */ {
#if _ITERATOR_DEBUG_LEVEL == 2
        const auto _Mycont = _STD addressof(_Mypair._Myval2);
        _STL_VERIFY(_First._Getcont() == _Mycont && _Last._Getcont() == _Mycont,
            "forward_list erase_after iterator range from incorrect container");
#endif // _ITERATOR_DEBUG_LEVEL == 2
        auto _Before = _First._Ptr;
        if (_Before != _Last._Ptr) {
            auto& _Al = _Getal();
            for (;;) {
                const auto _Subject = _Before->_Next;
                if (_Subject == _Last._Ptr) {
                    break;
                }

#if _ITERATOR_DEBUG_LEVEL == 2
                _Mypair._Myval2._Orphan_ptr(_Subject);
#endif // _ITERATOR_DEBUG_LEVEL == 2
                _Before->_Next = _Subject->_Next;
                _Node::_Freenode(_Al, _Subject);
            }
        }

        return _Make_iter(_Last._Ptr);
    }

    void clear() noexcept { // erase all
#if _ITERATOR_DEBUG_LEVEL == 2
        _Mypair._Myval2._Orphan_ptr(nullptr);
#endif // _ITERATOR_DEBUG_LEVEL == 2

        _Nodeptr _Pnext;
        _Nodeptr _Pnode = _STD exchange(_Mypair._Myval2._Myhead, nullptr);

        auto& _Al = _Getal();
        for (; _Pnode; _Pnode = _Pnext) { // delete an element
            _Pnext = _Pnode->_Next;
            _Node::_Freenode(_Al, _Pnode);
        }
    }

    void swap(forward_list& _Right) noexcept /* strengthened */ {
        if (this != _STD addressof(_Right)) {
            _Pocs(_Getal(), _Right._Getal());
            _Swap_proxy_and_iterators(_Right);
            _Swap_adl(_Mypair._Myval2._Myhead, _Right._Mypair._Myval2._Myhead);
        }
    }

    void splice_after(const_iterator _Where, forward_list& _Right) noexcept /* strengthened */ {
        // splice all of _Right after _Where
#if _ITERATOR_DEBUG_LEVEL == 2
        _STL_VERIFY(
            _Where._Getcont() == _STD addressof(_Mypair._Myval2), "forward_list splice_after iterator outside range");
#endif // _ITERATOR_DEBUG_LEVEL == 2

        if (this != _STD addressof(_Right) && !_Right.empty()) { // worth splicing, do it
            _Splice_after(_Where._Unwrapped(), _Right, _Right._Unchecked_before_begin(), _Default_sentinel{});
        }
    }

    void splice_after(const_iterator _Where, forward_list&& _Right) noexcept /* strengthened */ {
        // splice all of _Right after _Where
        splice_after(_Where, _Right);
    }

    void splice_after(const_iterator _Where, forward_list& _Right, const_iterator _First) noexcept /* strengthened */ {
        // splice _Right (_First, _First + 2) after _Where
#if _ITERATOR_DEBUG_LEVEL == 2
        _STL_VERIFY(
            _Where._Getcont() == _STD addressof(_Mypair._Myval2), "forward_list splice_after iterator outside range");
        _STL_VERIFY(_First._Getcont() == _STD addressof(_Right._Mypair._Myval2),
            "forward_list splice_after iterator outside range");
#endif // _ITERATOR_DEBUG_LEVEL == 2

        _Splice_after(_Where._Ptr, _Right, _First._Ptr);
    }

    void splice_after(const_iterator _Where, forward_list&& _Right, const_iterator _First) noexcept /* strengthened */ {
        // splice _Right (_First, _First + 2) after _Where
        splice_after(_Where, _Right, _First);
    }

    void splice_after(const_iterator _Where, forward_list& _Right, const_iterator _First, const_iterator _Last) noexcept
    /* strengthened */ {
        // splice _Right (_First, _Last) after _Where
#if _ITERATOR_DEBUG_LEVEL == 2
        _STL_VERIFY(
            _Where._Getcont() == _STD addressof(_Mypair._Myval2), "forward_list splice_after iterator outside range");
        const auto _Rightcont = _STD addressof(_Right._Mypair._Myval2);
        _STL_VERIFY(_First._Getcont() == _Rightcont && _Last._Getcont() == _Rightcont,
            "forward_list splice_after range outside right");
#endif // _ITERATOR_DEBUG_LEVEL == 2

        _Splice_after(_Where._Unwrapped(), _Right, _First._Unwrapped(), _Last._Unwrapped());
    }

    void splice_after(const_iterator _Where, forward_list&& _Right, const_iterator _First,
        const_iterator _Last) noexcept /* strengthened */ {
        // splice _Right [_First, _Last) after _Where
        splice_after(_Where, _Right, _First, _Last);
    }

    struct _Flist_node_remove_op {
        // tracks nodes pending removal in a remove operation, so that program-defined predicates may reference those
        // elements until the removal is complete.

        explicit _Flist_node_remove_op(forward_list& _List_) noexcept
            : _List(_List_), _Head(), _Tail(_STD addressof(_Head)) {}

        _Flist_node_remove_op(const _Flist_node_remove_op&) = delete;
        _Flist_node_remove_op& operator=(const _Flist_node_remove_op&) = delete;

        _Nodeptr _Transfer_back(const _Nodeptr _Predecessor) noexcept {
            // extract _Predecessor->_Next from the container, and add it to the singly-linked list of nodes to destroy
            // returns the successor of the removed node

            // snip the node out
            const auto _Removed = _Predecessor->_Next;
            const auto _Next    = _Removed->_Next;
            _Removed->_Next     = _Nodeptr();
            _Predecessor->_Next = _Next;

            *_Tail = _Removed;
            _Tail  = _STD addressof(_Removed->_Next);

            return _Next;
        }

        ~_Flist_node_remove_op() {
            auto& _Al    = _List._Getal();
            auto _Target = _Head;
            while (_Target) {
                auto _Next = _Target->_Next;
#if _ITERATOR_DEBUG_LEVEL == 2
                _List._Mypair._Myval2._Orphan_ptr(_Target);
#endif // _ITERATOR_DEBUG_LEVEL == 2
                _Alnode_traits::destroy(_Al, _STD addressof(_Target->_Next));
                _Alnode_traits::destroy(_Al, _STD addressof(_Target->_Myval));
                _Al.deallocate(_Target, 1);
                _Target = _Next;
            }
        }

        forward_list& _List;
        _Nodeptr _Head;
        _Nodeptr* _Tail;
    };

    auto remove(const _Ty& _Val) { // erase each element matching _Val
        return remove_if([&](const _Ty& _Other) { return _Other == _Val; });
    }

    template <class _Pr1>
    auto remove_if(_Pr1 _Pred) { // erase each element satisfying _Pr1
        _Flist_node_remove_op _Op(*this);
        auto _Firstb       = _Unchecked_before_begin();
        size_type _Removed = 0;

        for (auto _First = _Unchecked_begin(); _First._Ptr;) {
            if (_Pred(*_First)) {
                _First._Ptr = _Op._Transfer_back(_Firstb._Ptr);
                ++_Removed;
            } else { // advance iterators
                _Firstb = _First;
                ++_First;
            }
        }

#if _HAS_CXX20
        return _Removed;
#else // _HAS_CXX20
        (void) _Removed;
#endif // _HAS_CXX20
    }

    auto unique() { // erase each element matching previous
        return unique(equal_to<>{});
    }

    template <class _Pr2>
    auto unique(_Pr2 _Pred) { // erase each element satisfying _Pred with previous
        _Flist_node_remove_op _Op(*this);
        auto _First        = _Unchecked_begin();
        size_type _Removed = 0;
        if (_First._Ptr) { // worth doing
            auto _After = _First;
            ++_After;
            while (_After._Ptr) {
                if (_Pred(*_First, *_After)) {
                    _After._Ptr = _Op._Transfer_back(_First._Ptr);
                    ++_Removed;
                } else {
                    _First = _After;
                    ++_After;
                }
            }
        }

#if _HAS_CXX20
        return _Removed;
#else // _HAS_CXX20
        (void) _Removed;
#endif // _HAS_CXX20
    }

    void merge(forward_list& _Right) { // merge in elements from _Right, both ordered by operator<
        _Merge1(_Right, less<>{});
    }

    void merge(forward_list&& _Right) { // merge in elements from _Right, both ordered by operator<
        _Merge1(_Right, less<>{});
    }

    template <class _Pr2>
    void merge(forward_list& _Right, _Pr2 _Pred) { // merge in elements from _Right, both ordered by _Pred
        _Merge1(_Right, _Pass_fn(_Pred));
    }

    template <class _Pr2>
    void merge(forward_list&& _Right, _Pr2 _Pred) { // merge in elements from _Right, both ordered by _Pred
        _Merge1(_Right, _Pass_fn(_Pred));
    }

private:
    template <class _Pr2>
    void _Merge1(forward_list& _Right, _Pr2 _Pred) { // merge in elements from _Right, both ordered by _Pred
#if _ITERATOR_DEBUG_LEVEL != 0
        _DEBUG_ORDER_UNWRAPPED(_Unchecked_begin(), _Default_sentinel{}, _Pred);
#endif // _ITERATOR_DEBUG_LEVEL != 0
        if (this == _STD addressof(_Right)) {
            return;
        }

        auto& _My_data    = _Mypair._Myval2;
        auto& _Right_data = _Right._Mypair._Myval2;
#if _ITERATOR_DEBUG_LEVEL != 0
        _DEBUG_ORDER_UNWRAPPED(_Right._Unchecked_begin(), _Default_sentinel{}, _Pred);
        if constexpr (!_Alnode_traits::is_always_equal::value) {
            _STL_VERIFY(_Getal() == _Right._Getal(), "list allocators incompatible for merge");
        }
#endif // _ITERATOR_DEBUG_LEVEL != 0

        if (!_My_data._Myhead) {
            // *this is empty; take all elements of _Right with no comparisons
#if _ITERATOR_DEBUG_LEVEL == 2
            {
                _Lockit _Lock(_LOCK_DEBUG);
                _Transfer_non_before_begin_ownership(_Right);
            } // unlock
#endif // _ITERATOR_DEBUG_LEVEL == 2
            _My_data._Myhead    = _Right_data._Myhead;
            _Right_data._Myhead = nullptr;
            return;
        }

#if _ITERATOR_DEBUG_LEVEL == 2
        constexpr bool _Noexcept =
            noexcept(_DEBUG_LT_PRED(_Pred, _My_data._Myhead->_Myval, _Right_data._Myhead->_Myval));
        if constexpr (_Noexcept) {
            // if the comparison is noexcept, we can take all the iterators in one go and avoid quadratic updates of
            // the iterator chain
            _Lockit _Lock(_LOCK_DEBUG);
            _Transfer_non_before_begin_ownership(_Right);
        } // unlock
#endif // _ITERATOR_DEBUG_LEVEL == 2

        if (!_Right_data._Myhead) {
            return;
        }

        auto _BFirst1 = _My_data._Before_head();
        auto _First2  = _Right_data._Myhead;
        for (;;) { // process 1 splice
            _Nodeptr _First1;
            for (;;) { // advance _BFirst1 over elements already in position
                _First1 = _BFirst1->_Next;
                if (!_First1) { // all elements in _Right are greater than elements in *this, splice them all
                    _BFirst1->_Next     = _First2;
                    _Right_data._Myhead = nullptr;
                    return;
                }

                if (_DEBUG_LT_PRED(_Pred, _First2->_Myval, _First1->_Myval)) {
                    // _First2->_Myval is out of order
                    break;
                }

                // _First1->_Myval is already in position; advance
                _BFirst1 = _First1;
            }

            // find the end of the "run" of elements less than _First1->_Myval in _Right
            auto _BRun_end = _First2;
            _Nodeptr _Run_end;
            for (;;) {
                _Run_end = _BRun_end->_Next;
                if (!_Run_end) {
                    break;
                }

                if (!_DEBUG_LT_PRED(_Pred, _Run_end->_Myval, _First1->_Myval)) {
                    // _Run_end is the first element in _Right that shouldn't precede _First1->_Myval.
                    // After the splice _First1->_Myval will be in position and must not be compared again.
                    break;
                }

                _BRun_end = _Run_end;
            }

#if _ITERATOR_DEBUG_LEVEL == 2
            if constexpr (!_Noexcept) {
                _Lockit _Lock(_LOCK_DEBUG);
                for (auto _Next = _First2; _Next != _Run_end; _Next = _Next->_Next) {
                    _Transfer_ownership(_Right, _Next);
                }
            } // unlock
#endif // _ITERATOR_DEBUG_LEVEL == 2

            _Right_data._Myhead = _Run_end; // snip out the run from its old position
            _BFirst1->_Next     = _First2; // insert into new position
            _BRun_end->_Next    = _First1;
            if (!_Run_end) {
                return;
            }

            _BFirst1 = _First1;
            _First2  = _Run_end;
        }
    }

public:
    void sort() { // order sequence
        _Scary_val::_Sort(_Mypair._Myval2._Before_head(), less<>{});
    }

    template <class _Pr2>
    void sort(_Pr2 _Pred) { // order sequence
        _Scary_val::_Sort(_Mypair._Myval2._Before_head(), _Pass_fn(_Pred));
    }

    void reverse() noexcept { // reverse sequence
        auto _Current = _Mypair._Myval2._Myhead;
        if (!_Current) {
            // empty forward_list
            return;
        }

        _Nodeptr _Prev{};
        for (;;) {
            const auto _Next = _Current->_Next;
            _Current->_Next  = _Prev;
            if (!_Next) {
                _Mypair._Myval2._Myhead = _Current;
                return;
            }

            _Prev    = _Current;
            _Current = _Next;
        }
    }

private:
#if _ITERATOR_DEBUG_LEVEL == 2
    void _Transfer_non_before_begin_ownership(forward_list& _Right) noexcept {
        // requires holding the debug lock
        const auto _Mycont            = _STD addressof(_Mypair._Myval2);
        const auto _Myproxy           = _Mycont->_Myproxy;
        const auto _Right_before_head = _Right._Mypair._Myval2._Before_head();
        _Iterator_base12** _Pnext     = &_Right._Mypair._Myval2._Myproxy->_Myfirstiter;
        while (*_Pnext) {
            const auto _Pnextptr = static_cast<const_iterator&>(**_Pnext)._Ptr;
            if (_Pnextptr == _Right_before_head) {
                _Pnext = &(*_Pnext)->_Mynextiter;
            } else { // take the iterator
                const auto _Extracted   = *_Pnext;
                *_Pnext                 = (*_Pnext)->_Mynextiter;
                _Extracted->_Myproxy    = _Myproxy;
                _Extracted->_Mynextiter = _Myproxy->_Myfirstiter;
                _Myproxy->_Myfirstiter  = _Extracted;
            }
        }
    }

    void _Transfer_ownership(forward_list& _Right, _Nodeptr _Target) noexcept {
        // requires holding the debug lock
        const auto _Mycont        = _STD addressof(_Mypair._Myval2);
        const auto _Myproxy       = _Mycont->_Myproxy;
        _Iterator_base12** _Pnext = &_Right._Mypair._Myval2._Myproxy->_Myfirstiter;
        while (*_Pnext) {
            const auto _Pnextptr = static_cast<const_iterator&>(**_Pnext)._Ptr;
            if (_Pnextptr != _Target) {
                _Pnext = &(*_Pnext)->_Mynextiter;
            } else { // take the iterator
                const auto _Extracted   = *_Pnext;
                *_Pnext                 = (*_Pnext)->_Mynextiter;
                _Extracted->_Myproxy    = _Myproxy;
                _Extracted->_Mynextiter = _Myproxy->_Myfirstiter;
                _Myproxy->_Myfirstiter  = _Extracted;
            }
        }
    }
#endif // _ITERATOR_DEBUG_LEVEL == 2

    void _Splice_after(_Nodeptr _Where, forward_list& _Right, _Nodeptr _Prev) noexcept {
        // splice _Right (_Prev, _Prev + 2) after _Where
#if _ITERATOR_DEBUG_LEVEL == 0
        (void) _Right;
#else // ^^^ _ITERATOR_DEBUG_LEVEL == 0 // _ITERATOR_DEBUG_LEVEL != 0 vvv
        if constexpr (!_Alnode_traits::is_always_equal::value) {
            _STL_VERIFY(_Getal() == _Right._Getal(), "forward_list containers incompatible for splice_after");
        }
#endif // _ITERATOR_DEBUG_LEVEL == 0

#if _ITERATOR_DEBUG_LEVEL == 2
        if (this != _STD addressof(_Right)) {
            _Lockit _Lock(_LOCK_DEBUG);
            _Transfer_ownership(_Right, _Prev->_Next);
        }
#endif // _ITERATOR_DEBUG_LEVEL == 2

        if (_Where != _Prev) {
            const auto _First = _Prev->_Next;
            if (_Where != _First) {
                _Prev->_Next  = _First->_Next;
                _First->_Next = _Where->_Next;
                _Where->_Next = _First;
            }
        }
    }

    template <class _Sentinel>
    void _Splice_after(_Unchecked_const_iterator _Where, forward_list& _Right, _Unchecked_const_iterator _First,
        _Sentinel _Last) noexcept {
        // splice _Right (_First, _Last) just after _Where
#if _ITERATOR_DEBUG_LEVEL == 0
        (void) _Right;
#else // ^^^ _ITERATOR_DEBUG_LEVEL == 0 // _ITERATOR_DEBUG_LEVEL != 0 vvv
        if constexpr (!_Alnode_traits::is_always_equal::value) {
            _STL_VERIFY(_Getal() == _Right._Getal(), "forward_list containers incompatible for splice_after");
        }
#endif // _ITERATOR_DEBUG_LEVEL == 0

        if (_First == _Last) {
            return;
        }

#if _ITERATOR_DEBUG_LEVEL == 2
        if (this != _STD addressof(_Right)) { // transfer ownership of (_First, _Last)
            _Lockit _Lock(_LOCK_DEBUG);
            _Unchecked_const_iterator _Next = _First;
            while (++_Next != _Last) { // transfer ownership
                _Transfer_ownership(_Right, _Next._Ptr);
            }
        }
#endif // _ITERATOR_DEBUG_LEVEL == 2

        // find prev(_Last)
        _Unchecked_const_iterator _After = _First;
        ++_After;
        if (_After == _Last) {
            return;
        }

        _Unchecked_const_iterator _Prev_last = _First;
        do {
            _Prev_last = _After;
            ++_After;
        } while (_After != _Last);

        const auto _Extracted_head = _First._Ptr->_Next;
        _First._Ptr->_Next         = _After._Ptr;
        _Prev_last._Ptr->_Next     = _Where._Ptr->_Next;
        _Where._Ptr->_Next         = _Extracted_head;
    }

    void _Alloc_proxy() {
        _Mypair._Myval2._Alloc_proxy(_GET_PROXY_ALLOCATOR(_Alnode, _Getal()));
    }

    void _Swap_proxy_and_iterators(forward_list& _Right) noexcept {
        _Mypair._Myval2._Swap_proxy_and_iterators(_Right._Mypair._Myval2);
    }

    _Alnode& _Getal() noexcept {
        return _Mypair._Get_first();
    }

    const _Alnode& _Getal() const noexcept {
        return _Mypair._Get_first();
    }

    _Compressed_pair<_Alnode, _Scary_val> _Mypair;
};

#if _HAS_CXX17
template <class _Iter, class _Alloc = allocator<_Iter_value_t<_Iter>>,
    enable_if_t<conjunction_v<_Is_iterator<_Iter>, _Is_allocator<_Alloc>>, int> = 0>
forward_list(_Iter, _Iter, _Alloc = _Alloc()) -> forward_list<_Iter_value_t<_Iter>, _Alloc>;
#endif // _HAS_CXX17

template <class _Ty, class _Alloc>
void swap(forward_list<_Ty, _Alloc>& _Left, forward_list<_Ty, _Alloc>& _Right) noexcept /* strengthened */ {
    _Left.swap(_Right);
}

template <class _Ty, class _Alloc>
_NODISCARD bool operator==(const forward_list<_Ty, _Alloc>& _Left, const forward_list<_Ty, _Alloc>& _Right) {
    return _STD equal(
        _Left._Unchecked_begin(), _Left._Unchecked_end_iter(), _Right._Unchecked_begin(), _Right._Unchecked_end_iter());
}

#if !_HAS_CXX20
template <class _Ty, class _Alloc>
_NODISCARD bool operator!=(const forward_list<_Ty, _Alloc>& _Left, const forward_list<_Ty, _Alloc>& _Right) {
    return !(_Left == _Right);
}
#endif // !_HAS_CXX20

#ifdef __cpp_lib_concepts
template <class _Ty, class _Alloc>
_NODISCARD _Synth_three_way_result<_Ty> operator<=>(
    const forward_list<_Ty, _Alloc>& _Left, const forward_list<_Ty, _Alloc>& _Right) {
    return _STD lexicographical_compare_three_way(_Left._Unchecked_begin(), _Left._Unchecked_end_iter(),
        _Right._Unchecked_begin(), _Right._Unchecked_end_iter(), _Synth_three_way{});
}
#else // ^^^ defined(__cpp_lib_concepts) / !defined(__cpp_lib_concepts) vvv
template <class _Ty, class _Alloc>
_NODISCARD bool operator<(const forward_list<_Ty, _Alloc>& _Left, const forward_list<_Ty, _Alloc>& _Right) {
    return _STD lexicographical_compare(
        _Left._Unchecked_begin(), _Left._Unchecked_end_iter(), _Right._Unchecked_begin(), _Right._Unchecked_end_iter());
}

template <class _Ty, class _Alloc>
_NODISCARD bool operator>(const forward_list<_Ty, _Alloc>& _Left, const forward_list<_Ty, _Alloc>& _Right) {
    return _Right < _Left;
}

template <class _Ty, class _Alloc>
_NODISCARD bool operator<=(const forward_list<_Ty, _Alloc>& _Left, const forward_list<_Ty, _Alloc>& _Right) {
    return !(_Right < _Left);
}

template <class _Ty, class _Alloc>
_NODISCARD bool operator>=(const forward_list<_Ty, _Alloc>& _Left, const forward_list<_Ty, _Alloc>& _Right) {
    return !(_Left < _Right);
}
#endif // ^^^ !defined(__cpp_lib_concepts) ^^^

#if _HAS_CXX20
template <class _Ty, class _Alloc, class _Uty>
typename forward_list<_Ty, _Alloc>::size_type erase(forward_list<_Ty, _Alloc>& _Cont, const _Uty& _Val) {
    return _Cont.remove_if([&](_Ty& _Elem) { return _Elem == _Val; });
}

template <class _Ty, class _Alloc, class _Pr>
typename forward_list<_Ty, _Alloc>::size_type erase_if(forward_list<_Ty, _Alloc>& _Cont, _Pr _Pred) {
    return _Cont.remove_if(_Pass_fn(_Pred));
}
#endif // _HAS_CXX20

#if _HAS_CXX17
namespace pmr {
    template <class _Ty>
    using forward_list = _STD forward_list<_Ty, polymorphic_allocator<_Ty>>;
} // namespace pmr
#endif // _HAS_CXX17
_STD_END
#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _FORWARD_LIST_
