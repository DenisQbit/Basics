// future standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _FUTURE_
#define _FUTURE_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#ifdef _M_CEE
#error <future> is not supported when compiling with /clr or /clr:pure.
#endif // _M_CEE

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED
#include <experimental/resumable>
#endif // _RESUMABLE_FUNCTIONS_SUPPORTED

#include <__msvc_chrono.hpp>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <ppltasks.h>
#include <system_error>
#include <thread>
#include <utility>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
template <class _Alloc>
struct _Allocator_deleter {
    _Alloc _Al;

    using pointer = typename allocator_traits<_Alloc>::pointer;
    void operator()(pointer _Ptr) noexcept { // delete the pointer
        _Destroy_in_place(*_Ptr);
        _Al.deallocate(_Ptr, 1);
    }
};

template <class _Alloc>
using _Unique_ptr_alloc = unique_ptr<typename _Alloc::value_type, _Allocator_deleter<_Alloc>>;

template <class _Alloc, class... _Args>
_NODISCARD _Unique_ptr_alloc<_Alloc> _Make_unique_alloc(_Alloc& _Al, _Args&&... _Vals) {
    // construct an object with an allocator and return it owned by a unique_ptr
    _Alloc_construct_ptr<_Alloc> _Constructor{_Al};
    _Constructor._Allocate();
    _Construct_in_place(*_Constructor._Ptr, _STD forward<_Args>(_Vals)...);
    return _Unique_ptr_alloc<_Alloc>(_Constructor._Release(), _Allocator_deleter<_Alloc>{_Al});
}

struct _Nil {}; // empty struct, for unused argument types

enum class future_errc { // names for futures errors
    broken_promise = 1,
    future_already_retrieved,
    promise_already_satisfied,
    no_state
};

enum class launch { // names for launch options passed to async
    async    = 0x1,
    deferred = 0x2
};

_BITMASK_OPS(launch)

enum class future_status { // names for timed wait function returns
    ready,
    timeout,
    deferred
};

[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Throw_future_error(const error_code& _Code);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Rethrow_future_exception(exception_ptr _Ptr);

template <>
struct is_error_code_enum<future_errc> : true_type {};

_NODISCARD const error_category& future_category() noexcept;

_NODISCARD inline error_code make_error_code(future_errc _Errno) noexcept {
    return error_code(static_cast<int>(_Errno), _STD future_category());
}

_NODISCARD inline error_condition make_error_condition(future_errc _Errno) noexcept {
    return error_condition(static_cast<int>(_Errno), _STD future_category());
}

inline const char* _Future_error_map(int _Errcode) noexcept { // convert to name of future error
    switch (static_cast<future_errc>(_Errcode)) { // switch on error code value
    case future_errc::broken_promise:
        return "broken promise";

    case future_errc::future_already_retrieved:
        return "future already retrieved";

    case future_errc::promise_already_satisfied:
        return "promise already satisfied";

    case future_errc::no_state:
        return "no state";

    default:
        return nullptr;
    }
}

class future_error : public logic_error { // future exception
public:
    explicit future_error(error_code _Errcode) // internal, TRANSITION, will be removed
        : logic_error(""), _Mycode(_Errcode) {}

    explicit future_error(future_errc _Errno) : logic_error(""), _Mycode(_STD make_error_code(_Errno)) {}

    _NODISCARD const error_code& code() const noexcept {
        return _Mycode;
    }

    _NODISCARD virtual const char* __CLR_OR_THIS_CALL what() const noexcept override { // get message string
        return _Future_error_map(_Mycode.value());
    }

#if !_HAS_EXCEPTIONS
protected:
    virtual void _Doraise() const override { // perform class-specific exception handling
        _RAISE(*this);
    }
#endif // !_HAS_EXCEPTIONS

private:
    error_code _Mycode; // the stored error code
};

class _Future_error_category2 : public error_category { // categorize a future error
public:
    constexpr _Future_error_category2() noexcept : error_category(_Future_addr) {}

    virtual const char* name() const noexcept override {
        return "future";
    }

    virtual string message(int _Errcode) const override {
        const char* _Name = _Future_error_map(_Errcode);
        if (_Name) {
            return _Name;
        }

        return _Syserror_map(_Errcode);
    }
};

_NODISCARD inline const error_category& future_category() noexcept {
    return _Immortalize_memcpy_image<_Future_error_category2>();
}

template <class _Ty>
class _Associated_state;

template <class _Ty>
struct __declspec(novtable) _Deleter_base { // abstract base class for managing deletion of state objects
    virtual void _Delete(_Associated_state<_Ty>*) = 0;
    virtual ~_Deleter_base() noexcept {}
};

template <class _Ty, class _Derived, class _Alloc>
struct _State_deleter : _Deleter_base<_Ty> { // manage allocator and deletion state objects
    _State_deleter(const _Alloc& _Al) : _My_alloc(_Al) {}

    _State_deleter(const _State_deleter&) = delete;
    _State_deleter& operator=(const _State_deleter&) = delete;

    virtual void _Delete(_Associated_state<_Ty>* _State) override;

    _Alloc _My_alloc;
};

template <class _Ty>
class _Associated_state { // class for managing associated synchronous state
public:
    using _State_type = _Ty;
    using _Mydel      = _Deleter_base<_Ty>;

    _Associated_state(_Mydel* _Dp = nullptr)
        : _Refs(1), // non-atomic initialization
          _Exception(), _Retrieved(false), _Ready(false), _Ready_at_thread_exit(false), _Has_stored_result(false),
          _Running(false), _Deleter(_Dp) {
        // TRANSITION: _Associated_state ctor assumes _Ty is default constructible
    }

    virtual ~_Associated_state() noexcept {
        if (_Has_stored_result && !_Ready) { // registered for release at thread exit
            _Cond._Unregister(_Mtx);
        }
    }

    void _Retain() { // increment reference count
        _MT_INCR(_Refs);
    }

    void _Release() { // decrement reference count and destroy when zero
        if (_MT_DECR(_Refs) == 0) {
            _Delete_this();
        }
    }

private:
    _Atomic_counter_t _Refs;

public:
    virtual void _Wait() { // wait for signal
        unique_lock<mutex> _Lock(_Mtx);
        _Maybe_run_deferred_function(_Lock);
        while (!_Ready) {
            _Cond.wait(_Lock);
        }
    }

    struct _Test_ready { // wraps _Associated_state
        _Test_ready(const _Associated_state* _St) : _State(_St) {}

        bool operator()() const { // test state
            return _State->_Ready != 0;
        }
        const _Associated_state* _State;
    };

    template <class _Rep, class _Per>
    future_status _Wait_for(const chrono::duration<_Rep, _Per>& _Rel_time) { // wait for duration
        unique_lock<mutex> _Lock(_Mtx);
        if (_Has_deferred_function()) {
            return future_status::deferred;
        }

        if (_Cond.wait_for(_Lock, _Rel_time, _Test_ready(this))) {
            return future_status::ready;
        }

        return future_status::timeout;
    }

    template <class _Clock, class _Dur>
    future_status _Wait_until(const chrono::time_point<_Clock, _Dur>& _Abs_time) { // wait until time point
        unique_lock<mutex> _Lock(_Mtx);
        if (_Has_deferred_function()) {
            return future_status::deferred;
        }

        if (_Cond.wait_until(_Lock, _Abs_time, _Test_ready(this))) {
            return future_status::ready;
        }

        return future_status::timeout;
    }

    virtual _Ty& _Get_value(bool _Get_only_once) {
        unique_lock<mutex> _Lock(_Mtx);
        if (_Get_only_once && _Retrieved) {
            _Throw_future_error(make_error_code(future_errc::future_already_retrieved));
        }

        if (_Exception) {
            _Rethrow_future_exception(_Exception);
        }

        _Retrieved = true;
        _Maybe_run_deferred_function(_Lock);
        while (!_Ready) {
            _Cond.wait(_Lock);
        }

        if (_Exception) {
            _Rethrow_future_exception(_Exception);
        }

        return _Result;
    }

    void _Set_value(const _Ty& _Val, bool _At_thread_exit) { // store a result
        unique_lock<mutex> _Lock(_Mtx);
        _Set_value_raw(_Val, &_Lock, _At_thread_exit);
    }

    void _Set_value_raw(const _Ty& _Val, unique_lock<mutex>* _Lock,
        bool _At_thread_exit) { // store a result while inside a locked block
        if (_Has_stored_result) {
            _Throw_future_error(make_error_code(future_errc::promise_already_satisfied));
        }

        _Result = _Val;
        _Do_notify(_Lock, _At_thread_exit);
    }

    void _Set_value(_Ty&& _Val, bool _At_thread_exit) { // store a result
        unique_lock<mutex> _Lock(_Mtx);
        _Set_value_raw(_STD forward<_Ty>(_Val), &_Lock, _At_thread_exit);
    }

    void _Set_value_raw(_Ty&& _Val, unique_lock<mutex>* _Lock,
        bool _At_thread_exit) { // store a result while inside a locked block
        if (_Has_stored_result) {
            _Throw_future_error(make_error_code(future_errc::promise_already_satisfied));
        }

        _Result = _STD forward<_Ty>(_Val);
        _Do_notify(_Lock, _At_thread_exit);
    }

    void _Set_value(bool _At_thread_exit) { // store a (void) result
        unique_lock<mutex> _Lock(_Mtx);
        _Set_value_raw(&_Lock, _At_thread_exit);
    }

    void _Set_value_raw(
        unique_lock<mutex>* _Lock, bool _At_thread_exit) { // store a (void) result while inside a locked block
        if (_Has_stored_result) {
            _Throw_future_error(make_error_code(future_errc::promise_already_satisfied));
        }

        _Do_notify(_Lock, _At_thread_exit);
    }

    void _Set_exception(exception_ptr _Exc, bool _At_thread_exit) { // store a result
        unique_lock<mutex> _Lock(_Mtx);
        _Set_exception_raw(_Exc, &_Lock, _At_thread_exit);
    }

    void _Set_exception_raw(exception_ptr _Exc, unique_lock<mutex>* _Lock,
        bool _At_thread_exit) { // store a result while inside a locked block
        if (_Has_stored_result) {
            _Throw_future_error(make_error_code(future_errc::promise_already_satisfied));
        }

        _Exception = _Exc;
        _Do_notify(_Lock, _At_thread_exit);
    }

    bool _Is_ready() const {
        return _Ready != 0;
    }

    bool _Is_ready_at_thread_exit() const {
        return _Ready_at_thread_exit;
    }

    bool _Already_has_stored_result() const {
        return _Has_stored_result;
    }

    bool _Already_retrieved() const {
        return _Retrieved;
    }

    void _Abandon() { // abandon shared state
        unique_lock<mutex> _Lock(_Mtx);
        if (!_Has_stored_result) { // queue exception
            future_error _Fut(make_error_code(future_errc::broken_promise));
            _Set_exception_raw(_STD make_exception_ptr(_Fut), &_Lock, false);
        }
    }

protected:
    void _Make_ready_at_thread_exit() { // set ready status at thread exit
        if (_Ready_at_thread_exit) {
            _Ready = true;
        }
    }

    void _Maybe_run_deferred_function(unique_lock<mutex>& _Lock) { // run a deferred function if not already done
        if (!_Running) { // run the function
            _Running = true;
            _Run_deferred_function(_Lock);
        }
    }

public:
    _Ty _Result;
    exception_ptr _Exception;
    mutex _Mtx;
    condition_variable _Cond;
    bool _Retrieved;
    int _Ready;
    bool _Ready_at_thread_exit;
    bool _Has_stored_result;
    bool _Running;

private:
    virtual bool _Has_deferred_function() const noexcept { // overridden by _Deferred_async_state
        return false;
    }

    virtual void _Run_deferred_function(unique_lock<mutex>&) {} // do nothing

    virtual void _Do_notify(unique_lock<mutex>* _Lock, bool _At_thread_exit) { // notify waiting threads
        // TRANSITION, ABI: This is virtual, but never overridden.
        _Has_stored_result = true;
        if (_At_thread_exit) { // notify at thread exit
            _Cond._Register(*_Lock, &_Ready);
        } else { // notify immediately
            _Ready = true;
            _Cond.notify_all();
        }
    }

    void _Delete_this() { // delete this object
        if (_Deleter) {
            _Deleter->_Delete(this);
        } else {
            delete this;
        }
    }

    _Mydel* _Deleter;

public:
    _Associated_state(const _Associated_state&) = delete;
    _Associated_state& operator=(const _Associated_state&) = delete;
};

template <class _Ty, class _Derived, class _Alloc>
void _State_deleter<_Ty, _Derived, _Alloc>::_Delete(
    _Associated_state<_Ty>* _State) { // delete _State and this using stored allocator
    using _State_allocator = _Rebind_alloc_t<_Alloc, _Derived>;
    _State_allocator _St_alloc(_My_alloc);

    using _Deleter_allocator = _Rebind_alloc_t<_Alloc, _State_deleter>;
    _Deleter_allocator _Del_alloc(_My_alloc);

    _Derived* _Ptr = static_cast<_Derived*>(_State);

    _Delete_plain_internal(_St_alloc, _Ptr);
    _Delete_plain_internal(_Del_alloc, this);
}

template <class>
class _Packaged_state;

template <class _Ret, class... _ArgTypes>
class _Packaged_state<_Ret(_ArgTypes...)>
    : public _Associated_state<_Ret> { // class for managing associated asynchronous state for packaged_task
public:
    using _Mybase = _Associated_state<_Ret>;
    using _Mydel  = typename _Mybase::_Mydel;

    template <class _Fty2>
    _Packaged_state(const _Fty2& _Fnarg) : _Fn(_Fnarg) {}

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
    template <class _Fty2, class _Alloc>
    _Packaged_state(const _Fty2& _Fnarg, const _Alloc& _Al, _Mydel* _Dp)
        : _Mybase(_Dp), _Fn(allocator_arg, _Al, _Fnarg) {}
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

    template <class _Fty2>
    _Packaged_state(_Fty2&& _Fnarg) : _Fn(_STD forward<_Fty2>(_Fnarg)) {}

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
    template <class _Fty2, class _Alloc>
    _Packaged_state(_Fty2&& _Fnarg, const _Alloc& _Al, _Mydel* _Dp)
        : _Mybase(_Dp), _Fn(allocator_arg, _Al, _STD forward<_Fty2>(_Fnarg)) {}
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

    void _Call_deferred(_ArgTypes... _Args) { // set deferred call
        _TRY_BEGIN
        // call function object and catch exceptions
        this->_Set_value(_Fn(_STD forward<_ArgTypes>(_Args)...), true);
        _CATCH_ALL
        // function object threw exception; record result
        this->_Set_exception(_STD current_exception(), true);
        _CATCH_END
    }

    void _Call_immediate(_ArgTypes... _Args) { // call function object
        _TRY_BEGIN
        // call function object and catch exceptions
        this->_Set_value(_Fn(_STD forward<_ArgTypes>(_Args)...), false);
        _CATCH_ALL
        // function object threw exception; record result
        this->_Set_exception(_STD current_exception(), false);
        _CATCH_END
    }

    const function<_Ret(_ArgTypes...)>& _Get_fn() {
        return _Fn;
    }

private:
    function<_Ret(_ArgTypes...)> _Fn;
};

template <class _Ret, class... _ArgTypes>
class _Packaged_state<_Ret&(_ArgTypes...)>
    : public _Associated_state<_Ret*> { // class for managing associated asynchronous state for packaged_task
public:
    using _Mybase = _Associated_state<_Ret*>;
    using _Mydel  = typename _Mybase::_Mydel;

    template <class _Fty2>
    _Packaged_state(const _Fty2& _Fnarg) : _Fn(_Fnarg) {}

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
    template <class _Fty2, class _Alloc>
    _Packaged_state(const _Fty2& _Fnarg, const _Alloc& _Al, _Mydel* _Dp)
        : _Mybase(_Dp), _Fn(allocator_arg, _Al, _Fnarg) {}
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

    template <class _Fty2>
    _Packaged_state(_Fty2&& _Fnarg) : _Fn(_STD forward<_Fty2>(_Fnarg)) {}

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
    template <class _Fty2, class _Alloc>
    _Packaged_state(_Fty2&& _Fnarg, const _Alloc& _Al, _Mydel* _Dp)
        : _Mybase(_Dp), _Fn(allocator_arg, _Al, _STD forward<_Fty2>(_Fnarg)) {}
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

    void _Call_deferred(_ArgTypes... _Args) { // set deferred call
        _TRY_BEGIN
        // call function object and catch exceptions
        this->_Set_value(_STD addressof(_Fn(_STD forward<_ArgTypes>(_Args)...)), true);
        _CATCH_ALL
        // function object threw exception; record result
        this->_Set_exception(_STD current_exception(), true);
        _CATCH_END
    }

    void _Call_immediate(_ArgTypes... _Args) { // call function object
        _TRY_BEGIN
        // call function object and catch exceptions
        this->_Set_value(_STD addressof(_Fn(_STD forward<_ArgTypes>(_Args)...)), false);
        _CATCH_ALL
        // function object threw exception; record result
        this->_Set_exception(_STD current_exception(), false);
        _CATCH_END
    }

    const function<_Ret&(_ArgTypes...)>& _Get_fn() {
        return _Fn;
    }

private:
    function<_Ret&(_ArgTypes...)> _Fn;
};

template <class... _ArgTypes>
class _Packaged_state<void(_ArgTypes...)>
    : public _Associated_state<int> { // class for managing associated asynchronous state for packaged_task
public:
    using _Mybase = _Associated_state<int>;
    using _Mydel  = typename _Mybase::_Mydel;

    template <class _Fty2>
    _Packaged_state(const _Fty2& _Fnarg) : _Fn(_Fnarg) {}

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
    template <class _Fty2, class _Alloc>
    _Packaged_state(const _Fty2& _Fnarg, const _Alloc& _Al, _Mydel* _Dp)
        : _Mybase(_Dp), _Fn(allocator_arg, _Al, _Fnarg) {}
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

    template <class _Fty2>
    _Packaged_state(_Fty2&& _Fnarg) : _Fn(_STD forward<_Fty2>(_Fnarg)) {}

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
    template <class _Fty2, class _Alloc>
    _Packaged_state(_Fty2&& _Fnarg, const _Alloc& _Al, _Mydel* _Dp)
        : _Mybase(_Dp), _Fn(allocator_arg, _Al, _STD forward<_Fty2>(_Fnarg)) {}
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

    void _Call_deferred(_ArgTypes... _Args) { // set deferred call
        _TRY_BEGIN
        // call function object and catch exceptions
        _Fn(_STD forward<_ArgTypes>(_Args)...);
        this->_Set_value(1, true);
        _CATCH_ALL
        // function object threw exception; record result
        this->_Set_exception(_STD current_exception(), true);
        _CATCH_END
    }

    void _Call_immediate(_ArgTypes... _Args) { // call function object
        _TRY_BEGIN
        // call function object and catch exceptions
        _Fn(_STD forward<_ArgTypes>(_Args)...);
        this->_Set_value(1, false);
        _CATCH_ALL
        // function object threw exception; record result
        this->_Set_exception(_STD current_exception(), false);
        _CATCH_END
    }

    const function<void(_ArgTypes...)>& _Get_fn() {
        return _Fn;
    }

private:
    function<void(_ArgTypes...)> _Fn;
};

template <class _Ty, class _Alloc>
_Associated_state<_Ty>* _Make_associated_state(const _Alloc& _Al) {
    // construct an _Associated_state object with an allocator
    using _Delty   = _State_deleter<_Ty, _Associated_state<_Ty>, _Alloc>;
    using _Aldelty = _Rebind_alloc_t<_Alloc, _Delty>;
    using _Alstate = _Rebind_alloc_t<_Alloc, _Associated_state<_Ty>>;

    _Aldelty _Del_alloc(_Al);
    _Alstate _State_alloc(_Al);
    auto _Del = _Make_unique_alloc(_Del_alloc, _Al);
    auto _Res = _Make_unique_alloc(_State_alloc, _Unfancy(_Del.get()));
    (void) _Del.release(); // ownership of _Del.get() now transferred to _Res
    return _Unfancy(_Res.release()); // ownership transferred to caller
}

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
template <class _Pack_state, class _Fty2, class _Alloc>
_Pack_state* _Make_packaged_state(_Fty2&& _Fnarg, const _Alloc& _Al) {
    // construct a _Packaged_state object with an allocator from an rvalue function object
    using _Delty   = _State_deleter<typename _Pack_state::_Mybase::_State_type, _Pack_state, _Alloc>;
    using _Aldelty = _Rebind_alloc_t<_Alloc, _Delty>;
    using _Alstate = _Rebind_alloc_t<_Alloc, _Pack_state>;

    _Aldelty _Del_alloc(_Al);
    _Alstate _State_alloc(_Al);
    auto _Del = _Make_unique_alloc(_Del_alloc, _Al);
    auto _Res = _Make_unique_alloc(_State_alloc, _STD forward<_Fty2>(_Fnarg), _Al, _Unfancy(_Del.get()));
    (void) _Del.release(); // ownership of _Del.get() now transferred to _Res
    return _Unfancy(_Res.release()); // ownership transferred to caller
}
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

template <class _Rx>
class _Deferred_async_state : public _Packaged_state<_Rx()> {
    // class for managing associated synchronous state for deferred execution from async
public:
    template <class _Fty2>
    _Deferred_async_state(const _Fty2& _Fnarg) : _Packaged_state<_Rx()>(_Fnarg) {}

    template <class _Fty2>
    _Deferred_async_state(_Fty2&& _Fnarg) : _Packaged_state<_Rx()>(_STD forward<_Fty2>(_Fnarg)) {}

private:
    virtual bool _Has_deferred_function() const noexcept override {
        // this function is considered to be deferred until it's invoked
        return !this->_Running;
    }

    virtual void _Run_deferred_function(unique_lock<mutex>& _Lock) override { // run the deferred function
        _Lock.unlock();
        _Packaged_state<_Rx()>::_Call_immediate();
        _Lock.lock();
    }
};

template <class _Rx>
class _Task_async_state : public _Packaged_state<_Rx()> {
    // class for managing associated synchronous state for asynchronous execution from async
public:
    using _Mybase     = _Packaged_state<_Rx()>;
    using _State_type = typename _Mybase::_State_type;

    template <class _Fty2>
    _Task_async_state(_Fty2&& _Fnarg) : _Mybase(_STD forward<_Fty2>(_Fnarg)) {
        _Task = ::Concurrency::create_task([this]() { // do it now
            this->_Call_immediate();
        });

        this->_Running = true;
    }

    virtual ~_Task_async_state() noexcept {
        _Wait();
    }

    virtual void _Wait() override { // wait for completion
        _Task.wait();
    }

    virtual _State_type& _Get_value(bool _Get_only_once) override {
        // return the stored result or throw stored exception
        _Task.wait();
        return _Mybase::_Get_value(_Get_only_once);
    }

private:
    ::Concurrency::task<void> _Task;
};

template <class _Ty>
class _State_manager {
    // class for managing possibly non-existent associated asynchronous state object
public:
    _State_manager() : _Assoc_state(nullptr) { // construct with no associated asynchronous state object
        _Get_only_once = false;
    }

    _State_manager(_Associated_state<_Ty>* _New_state, bool _Get_once)
        : _Assoc_state(_New_state) { // construct with _New_state
        _Get_only_once = _Get_once;
    }

    _State_manager(const _State_manager& _Other, bool _Get_once = false) : _Assoc_state(nullptr) {
        _Copy_from(_Other);
        _Get_only_once = _Get_once;
    }

    _State_manager(_State_manager&& _Other, bool _Get_once = false) : _Assoc_state(nullptr) {
        _Move_from(_Other);
        _Get_only_once = _Get_once;
    }

    ~_State_manager() noexcept {
        if (_Assoc_state) {
            _Assoc_state->_Release();
        }
    }

    _State_manager& operator=(const _State_manager& _Other) {
        _Copy_from(_Other);
        return *this;
    }

    _State_manager& operator=(_State_manager&& _Other) {
        _Move_from(_Other);
        return *this;
    }

    _NODISCARD bool valid() const noexcept {
        return _Assoc_state && !(_Get_only_once && _Assoc_state->_Already_retrieved());
    }

    void wait() const { // wait for signal
        if (!valid()) {
            _Throw_future_error(make_error_code(future_errc::no_state));
        }

        _Assoc_state->_Wait();
    }

    template <class _Rep, class _Per>
    future_status wait_for(const chrono::duration<_Rep, _Per>& _Rel_time) const { // wait for duration
        if (!valid()) {
            _Throw_future_error(make_error_code(future_errc::no_state));
        }

        return _Assoc_state->_Wait_for(_Rel_time);
    }

    template <class _Clock, class _Dur>
    future_status wait_until(const chrono::time_point<_Clock, _Dur>& _Abs_time) const { // wait until time point
#if _HAS_CXX20
        static_assert(chrono::is_clock_v<_Clock>, "Clock type required");
#endif // _HAS_CXX20
        if (!valid()) {
            _Throw_future_error(make_error_code(future_errc::no_state));
        }

        return _Assoc_state->_Wait_until(_Abs_time);
    }

    _Ty& _Get_value() const {
        if (!valid()) {
            _Throw_future_error(make_error_code(future_errc::no_state));
        }

        return _Assoc_state->_Get_value(_Get_only_once);
    }

    void _Set_value(const _Ty& _Val, bool _Defer) { // store a result
        if (!valid()) {
            _Throw_future_error(make_error_code(future_errc::no_state));
        }

        _Assoc_state->_Set_value(_Val, _Defer);
    }

    void _Set_value(_Ty&& _Val, bool _Defer) { // store a result
        if (!valid()) {
            _Throw_future_error(make_error_code(future_errc::no_state));
        }

        _Assoc_state->_Set_value(_STD forward<_Ty>(_Val), _Defer);
    }

    void _Abandon() { // abandon shared state
        if (_Assoc_state) {
            _Assoc_state->_Abandon();
        }
    }

    void _Set_exception(exception_ptr _Exc, bool _Defer) { // store a result
        if (!valid()) {
            _Throw_future_error(make_error_code(future_errc::no_state));
        }

        _Assoc_state->_Set_exception(_Exc, _Defer);
    }

    void _Swap(_State_manager& _Other) { // exchange with _Other
        _STD swap(_Assoc_state, _Other._Assoc_state);
    }

    _Associated_state<_Ty>* _Ptr() const {
        return _Assoc_state;
    }

    void _Copy_from(const _State_manager& _Other) { // copy stored associated asynchronous state object from _Other
        if (this != _STD addressof(_Other)) {
            if (_Assoc_state) {
                _Assoc_state->_Release();
            }

            if (_Other._Assoc_state) { // do the copy
                _Other._Assoc_state->_Retain();
                _Assoc_state   = _Other._Assoc_state;
                _Get_only_once = _Other._Get_only_once;
            } else {
                _Assoc_state = nullptr;
            }
        }
    }

    void _Move_from(_State_manager& _Other) { // move stored associated asynchronous state object from _Other
        if (this != _STD addressof(_Other)) {
            if (_Assoc_state) {
                _Assoc_state->_Release();
            }

            _Assoc_state        = _Other._Assoc_state;
            _Other._Assoc_state = nullptr;
            _Get_only_once      = _Other._Get_only_once;
        }
    }

    bool _Is_ready() const {
        return _Assoc_state && _Assoc_state->_Is_ready();
    }

    bool _Is_ready_at_thread_exit() const {
        return _Assoc_state && _Assoc_state->_Is_ready_at_thread_exit();
    }

private:
    _Associated_state<_Ty>* _Assoc_state;
    bool _Get_only_once;
};

template <class _Ty>
class shared_future;

template <class _Ty>
class future : public _State_manager<_Ty> {
    // class that defines a non-copyable asynchronous return object that holds a value
    using _Mybase = _State_manager<_Ty>;

public:
    static_assert(!is_array_v<_Ty> && is_object_v<_Ty> && is_destructible_v<_Ty>,
        "T in future<T> must meet the Cpp17Destructible requirements (N4878 [futures.unique.future]/4).");

    future() noexcept {}

    future(future&& _Other) noexcept : _Mybase(_STD move(_Other), true) {}

    future& operator=(future&& _Right) noexcept {
        _Mybase::operator=(_STD move(_Right));
        return *this;
    }

    future(const _Mybase& _State, _Nil) : _Mybase(_State, true) {}

    ~future() noexcept {}

    _Ty get() {
        // block until ready then return the stored result or throw the stored exception
        future _Local{_STD move(*this)};
        return _STD move(_Local._Get_value());
    }

    _NODISCARD shared_future<_Ty> share() noexcept {
        return shared_future<_Ty>(_STD move(*this));
    }

    future(const future&) = delete;
    future& operator=(const future&) = delete;
};

template <class _Ty>
class future<_Ty&> : public _State_manager<_Ty*> {
    // class that defines a non-copyable asynchronous return object that holds a reference
    using _Mybase = _State_manager<_Ty*>;

public:
    future() noexcept {}

    future(future&& _Other) noexcept : _Mybase(_STD move(_Other), true) {}

    future& operator=(future&& _Right) noexcept {
        _Mybase::operator=(_STD move(_Right));
        return *this;
    }

    future(const _Mybase& _State, _Nil) : _Mybase(_State, true) {}

    ~future() noexcept {}

    _Ty& get() {
        // block until ready then return the stored result or throw the stored exception
        future _Local{_STD move(*this)};
        return *_Local._Get_value();
    }

    _NODISCARD shared_future<_Ty&> share() noexcept {
        return shared_future<_Ty&>(_STD move(*this));
    }

    future(const future&) = delete;
    future& operator=(const future&) = delete;
};

template <>
class future<void> : public _State_manager<int> {
    // class that defines a non-copyable asynchronous return object that does not hold a value
    using _Mybase = _State_manager<int>;

public:
    future() noexcept {}

    future(future&& _Other) noexcept : _Mybase(_STD move(_Other), true) {}

    future& operator=(future&& _Right) noexcept {
        _Mybase::operator=(_STD move(_Right));
        return *this;
    }

    future(const _Mybase& _State, _Nil) : _Mybase(_State, true) {}

    ~future() noexcept {}

    void get() {
        // block until ready then return or throw the stored exception
        future _Local{_STD move(*this)};
        _Local._Get_value();
    }

    _NODISCARD shared_future<void> share() noexcept;

    future(const future&) = delete;
    future& operator=(const future&) = delete;
};

template <class _Ty>
class shared_future : public _State_manager<_Ty> {
    // class that defines a copyable asynchronous return object that holds a value
    using _Mybase = _State_manager<_Ty>;

public:
    static_assert(!is_array_v<_Ty> && is_object_v<_Ty> && is_destructible_v<_Ty>,
        "T in shared_future<T> must meet the Cpp17Destructible requirements (N4878 [futures.shared.future]/4).");

    shared_future() noexcept {}

    shared_future(const shared_future& _Other) noexcept : _Mybase(_Other) {}

    shared_future& operator=(const shared_future& _Right) noexcept {
        _Mybase::operator=(_Right);
        return *this;
    }

    shared_future(future<_Ty>&& _Other) noexcept : _Mybase(_STD forward<_Mybase>(_Other)) {}

    shared_future(shared_future&& _Other) noexcept : _Mybase(_STD move(_Other)) {}

    shared_future& operator=(shared_future&& _Right) noexcept {
        _Mybase::operator=(_STD move(_Right));
        return *this;
    }

    ~shared_future() noexcept {}

    const _Ty& get() const {
        // block until ready then return the stored result or throw the stored exception
        return this->_Get_value();
    }
};

template <class _Ty>
class shared_future<_Ty&> : public _State_manager<_Ty*> {
    // class that defines a copyable asynchronous return object that holds a reference
    using _Mybase = _State_manager<_Ty*>;

public:
    shared_future() noexcept {}

    shared_future(const shared_future& _Other) noexcept : _Mybase(_Other) {}

    shared_future& operator=(const shared_future& _Right) noexcept {
        _Mybase::operator=(_Right);
        return *this;
    }

    shared_future(future<_Ty&>&& _Other) noexcept : _Mybase(_STD forward<_Mybase>(_Other)) {}

    shared_future(shared_future&& _Other) noexcept : _Mybase(_STD move(_Other)) {}

    shared_future& operator=(shared_future&& _Right) noexcept {
        _Mybase::operator=(_STD move(_Right));
        return *this;
    }

    ~shared_future() noexcept {}

    _Ty& get() const {
        // block until ready then return the stored result or throw the stored exception
        return *this->_Get_value();
    }
};

template <>
class shared_future<void> : public _State_manager<int> {
    // class that defines a copyable asynchronous return object that does not hold a value
    using _Mybase = _State_manager<int>;

public:
    shared_future() noexcept {}

    shared_future(const shared_future& _Other) noexcept : _Mybase(_Other) {}

    shared_future& operator=(const shared_future& _Right) noexcept {
        _Mybase::operator=(_Right);
        return *this;
    }

    shared_future(shared_future&& _Other) noexcept : _Mybase(_STD move(_Other)) {}

    shared_future(future<void>&& _Other) noexcept : _Mybase(_STD forward<_Mybase>(_Other)) {}

    shared_future& operator=(shared_future&& _Right) {
        _Mybase::operator=(_STD move(_Right));
        return *this;
    }

    ~shared_future() noexcept {}

    void get() const { // block until ready then return or throw the stored exception
        this->_Get_value();
    }
};

_NODISCARD inline shared_future<void> future<void>::share() noexcept {
    return shared_future<void>(_STD move(*this));
}

template <class _Ty>
class _Promise {
public:
    _Promise(_Associated_state<_Ty>* _State_ptr) : _State(_State_ptr, false), _Future_retrieved(false) {}

    _Promise(_Promise&& _Other) : _State(_STD move(_Other._State)), _Future_retrieved(_Other._Future_retrieved) {}

    _Promise& operator=(_Promise&& _Other) {
        _State            = _STD move(_Other._State);
        _Future_retrieved = _Other._Future_retrieved;
        return *this;
    }

    ~_Promise() noexcept {}

    void _Swap(_Promise& _Other) {
        _State._Swap(_Other._State);
        _STD swap(_Future_retrieved, _Other._Future_retrieved);
    }

    const _State_manager<_Ty>& _Get_state() const {
        return _State;
    }
    _State_manager<_Ty>& _Get_state() {
        return _State;
    }

    _State_manager<_Ty>& _Get_state_for_set() {
        if (!_State.valid()) {
            _Throw_future_error(make_error_code(future_errc::no_state));
        }

        return _State;
    }

    _State_manager<_Ty>& _Get_state_for_future() {
        if (!_State.valid()) {
            _Throw_future_error(make_error_code(future_errc::no_state));
        }

        if (_Future_retrieved) {
            _Throw_future_error(make_error_code(future_errc::future_already_retrieved));
        }

        _Future_retrieved = true;
        return _State;
    }

    bool _Is_valid() const noexcept {
        return _State.valid();
    }

    bool _Is_ready() const {
        return _State._Is_ready();
    }

    bool _Is_ready_at_thread_exit() const {
        return _State._Is_ready_at_thread_exit();
    }

    _Promise(const _Promise&) = delete;
    _Promise& operator=(const _Promise&) = delete;

private:
    _State_manager<_Ty> _State;
    bool _Future_retrieved;
};

template <class _Ty>
class promise { // class that defines an asynchronous provider that holds a value
public:
    static_assert(!is_array_v<_Ty> && is_object_v<_Ty> && is_destructible_v<_Ty>,
        "T in promise<T> must meet the Cpp17Destructible requirements (N4878 [futures.promise]/1).");

    promise() : _MyPromise(new _Associated_state<_Ty>) {}

    template <class _Alloc>
    promise(allocator_arg_t, const _Alloc& _Al) : _MyPromise(_Make_associated_state<_Ty>(_Al)) {}

    promise(promise&& _Other) noexcept : _MyPromise(_STD move(_Other._MyPromise)) {}

    promise& operator=(promise&& _Other) noexcept {
        promise(_STD move(_Other)).swap(*this);
        return *this;
    }

    ~promise() noexcept {
        if (_MyPromise._Is_valid() && !_MyPromise._Is_ready() && !_MyPromise._Is_ready_at_thread_exit()) {
            // exception if destroyed before function object returns
            future_error _Fut(make_error_code(future_errc::broken_promise));
            _MyPromise._Get_state()._Set_exception(_STD make_exception_ptr(_Fut), false);
        }
    }

    void swap(promise& _Other) noexcept {
        _MyPromise._Swap(_Other._MyPromise);
    }

    _NODISCARD future<_Ty> get_future() {
        return future<_Ty>(_MyPromise._Get_state_for_future(), _Nil());
    }

    void set_value(const _Ty& _Val) {
        _MyPromise._Get_state_for_set()._Set_value(_Val, false);
    }

    void set_value_at_thread_exit(const _Ty& _Val) {
        _MyPromise._Get_state_for_set()._Set_value(_Val, true);
    }

    void set_value(_Ty&& _Val) {
        _MyPromise._Get_state_for_set()._Set_value(_STD forward<_Ty>(_Val), false);
    }

    void set_value_at_thread_exit(_Ty&& _Val) {
        _MyPromise._Get_state_for_set()._Set_value(_STD forward<_Ty>(_Val), true);
    }

    void set_exception(exception_ptr _Exc) {
        _MyPromise._Get_state_for_set()._Set_exception(_Exc, false);
    }

    void set_exception_at_thread_exit(exception_ptr _Exc) {
        _MyPromise._Get_state_for_set()._Set_exception(_Exc, true);
    }

    promise(const promise&) = delete;
    promise& operator=(const promise&) = delete;

private:
    _Promise<_Ty> _MyPromise;
};

template <class _Ty>
class promise<_Ty&> { // class that defines an asynchronous provider that holds a reference
public:
    promise() : _MyPromise(new _Associated_state<_Ty*>) {}

    template <class _Alloc>
    promise(allocator_arg_t, const _Alloc& _Al) : _MyPromise(_Make_associated_state<_Ty*>(_Al)) {}

    promise(promise&& _Other) noexcept : _MyPromise(_STD move(_Other._MyPromise)) {}

    promise& operator=(promise&& _Other) noexcept {
        promise(_STD move(_Other)).swap(*this);
        return *this;
    }

    ~promise() noexcept {
        if (_MyPromise._Is_valid() && !_MyPromise._Is_ready() && !_MyPromise._Is_ready_at_thread_exit()) {
            // exception if destroyed before function object returns
            future_error _Fut(make_error_code(future_errc::broken_promise));
            _MyPromise._Get_state()._Set_exception(_STD make_exception_ptr(_Fut), false);
        }
    }

    void swap(promise& _Other) noexcept {
        _MyPromise._Swap(_Other._MyPromise);
    }

    _NODISCARD future<_Ty&> get_future() {
        return future<_Ty&>(_MyPromise._Get_state_for_future(), _Nil());
    }

    void set_value(_Ty& _Val) {
        _MyPromise._Get_state_for_set()._Set_value(_STD addressof(_Val), false);
    }

    void set_value_at_thread_exit(_Ty& _Val) {
        _MyPromise._Get_state_for_set()._Set_value(_STD addressof(_Val), true);
    }

    void set_exception(exception_ptr _Exc) {
        _MyPromise._Get_state_for_set()._Set_exception(_Exc, false);
    }

    void set_exception_at_thread_exit(exception_ptr _Exc) {
        _MyPromise._Get_state_for_set()._Set_exception(_Exc, true);
    }

    promise(const promise&) = delete;
    promise& operator=(const promise&) = delete;

private:
    _Promise<_Ty*> _MyPromise;
};

template <>
class promise<void> { // defines an asynchronous provider that does not hold a value
public:
    promise() : _MyPromise(new _Associated_state<int>) {}

    template <class _Alloc>
    promise(allocator_arg_t, const _Alloc& _Al) : _MyPromise(_Make_associated_state<int>(_Al)) {}

    promise(promise&& _Other) noexcept : _MyPromise(_STD move(_Other._MyPromise)) {}

    promise& operator=(promise&& _Other) noexcept {
        promise(_STD move(_Other)).swap(*this);
        return *this;
    }

    ~promise() noexcept {
        if (_MyPromise._Is_valid() && !_MyPromise._Is_ready() && !_MyPromise._Is_ready_at_thread_exit()) {
            // exception if destroyed before function object returns
            future_error _Fut(make_error_code(future_errc::broken_promise));
            _MyPromise._Get_state()._Set_exception(_STD make_exception_ptr(_Fut), false);
        }
    }

    void swap(promise& _Other) noexcept {
        _MyPromise._Swap(_Other._MyPromise);
    }

    _NODISCARD future<void> get_future() {
        return future<void>(_MyPromise._Get_state_for_future(), _Nil());
    }

    void set_value() {
        _MyPromise._Get_state_for_set()._Set_value(1, false);
    }

    void set_value_at_thread_exit() {
        _MyPromise._Get_state_for_set()._Set_value(1, true);
    }

    void set_exception(exception_ptr _Exc) {
        _MyPromise._Get_state_for_set()._Set_exception(_Exc, false);
    }

    void set_exception_at_thread_exit(exception_ptr _Exc) {
        _MyPromise._Get_state_for_set()._Set_exception(_Exc, true);
    }

    promise(const promise&) = delete;
    promise& operator=(const promise&) = delete;

private:
    _Promise<int> _MyPromise;
};

template <class _Ty, class _Alloc>
struct uses_allocator<promise<_Ty>, _Alloc> : true_type {};

template <class _Ty>
void swap(promise<_Ty>& _Left, promise<_Ty>& _Right) noexcept {
    _Left.swap(_Right);
}

template <class _Fret>
struct _P_arg_type { // type for functions returning T
    using type = _Fret;
};

template <class _Fret>
struct _P_arg_type<_Fret&> { // type for functions returning reference to T
    using type = _Fret*;
};

template <>
struct _P_arg_type<void> { // type for functions returning void
    using type = int;
};

template <class>
class packaged_task; // not defined

template <class _Ret, class... _ArgTypes>
class packaged_task<_Ret(_ArgTypes...)> {
    // class that defines an asynchronous provider that returns the result of a call to a function object
public:
    using _Ptype              = typename _P_arg_type<_Ret>::type;
    using _MyPromiseType      = _Promise<_Ptype>;
    using _MyStateManagerType = _State_manager<_Ptype>;
    using _MyStateType        = _Packaged_state<_Ret(_ArgTypes...)>;

    packaged_task() noexcept : _MyPromise(0) {}

    template <class _Fty2, enable_if_t<!is_same_v<_Remove_cvref_t<_Fty2>, packaged_task>, int> = 0>
    explicit packaged_task(_Fty2&& _Fnarg) : _MyPromise(new _MyStateType(_STD forward<_Fty2>(_Fnarg))) {}

    packaged_task(packaged_task&& _Other) noexcept : _MyPromise(_STD move(_Other._MyPromise)) {}

    packaged_task& operator=(packaged_task&& _Other) noexcept {
        _MyPromise = _STD move(_Other._MyPromise);
        return *this;
    }

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
    template <class _Fty2, class _Alloc, enable_if_t<!is_same_v<_Remove_cvref_t<_Fty2>, packaged_task>, int> = 0>
    packaged_task(allocator_arg_t, const _Alloc& _Al, _Fty2&& _Fnarg)
        : _MyPromise(_Make_packaged_state<_MyStateType>(_STD forward<_Fty2>(_Fnarg), _Al)) {}
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

    ~packaged_task() noexcept {
        _MyPromise._Get_state()._Abandon();
    }

    void swap(packaged_task& _Other) noexcept {
        _STD swap(_MyPromise, _Other._MyPromise);
    }

    _NODISCARD bool valid() const noexcept {
        return _MyPromise._Is_valid();
    }

    _NODISCARD future<_Ret> get_future() {
        return future<_Ret>(_MyPromise._Get_state_for_future(), _Nil());
    }

    void operator()(_ArgTypes... _Args) {
        if (_MyPromise._Is_ready()) {
            _Throw_future_error(make_error_code(future_errc::promise_already_satisfied));
        }

        _MyStateManagerType& _State = _MyPromise._Get_state_for_set();
        _MyStateType* _Ptr          = static_cast<_MyStateType*>(_State._Ptr());
        _Ptr->_Call_immediate(_STD forward<_ArgTypes>(_Args)...);
    }

    void make_ready_at_thread_exit(_ArgTypes... _Args) {
        if (_MyPromise._Is_ready()) {
            _Throw_future_error(make_error_code(future_errc::promise_already_satisfied));
        }

        _MyStateManagerType& _State = _MyPromise._Get_state_for_set();
        if (_State._Ptr()->_Already_has_stored_result()) {
            _Throw_future_error(make_error_code(future_errc::promise_already_satisfied));
        }

        _MyStateType* _Ptr = static_cast<_MyStateType*>(_State._Ptr());
        _Ptr->_Call_deferred(_STD forward<_ArgTypes>(_Args)...);
    }

    void reset() { // reset to newly constructed state
        _MyStateManagerType& _State         = _MyPromise._Get_state_for_set();
        _MyStateType* _MyState              = static_cast<_MyStateType*>(_State._Ptr());
        function<_Ret(_ArgTypes...)> _Fnarg = _MyState->_Get_fn();
        _MyPromiseType _New_promise(new _MyStateType(_Fnarg));
        _MyPromise._Get_state()._Abandon();
        _MyPromise._Swap(_New_promise);
    }

    packaged_task(const packaged_task&) = delete;
    packaged_task& operator=(const packaged_task&) = delete;

private:
    _MyPromiseType _MyPromise;
};

#if _HAS_CXX17
#define _PACKAGED_TASK_DEDUCTION_GUIDE(CALL_OPT, X1, X2, X3) \
    template <class _Ret, class... _Types>                   \
    packaged_task(_Ret(CALL_OPT*)(_Types...)) -> packaged_task<_Ret(_Types...)>; // intentionally discards CALL_OPT

_NON_MEMBER_CALL(_PACKAGED_TASK_DEDUCTION_GUIDE, X1, X2, X3)
#undef _PACKAGED_TASK_DEDUCTION_GUIDE

template <class _Fx>
packaged_task(_Fx) -> packaged_task<typename _Deduce_signature<_Fx>::type>;
#endif // _HAS_CXX17

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
template <class _Ty, class _Alloc>
struct uses_allocator<packaged_task<_Ty>, _Alloc> : true_type {};
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

template <class _Ty>
void swap(packaged_task<_Ty>& _Left, packaged_task<_Ty>& _Right) noexcept {
    _Left.swap(_Right);
}

template <class... _Types, size_t... _Indices>
auto _Invoke_stored_explicit(tuple<_Types...>&& _Tuple, index_sequence<_Indices...>) -> decltype(
    _STD invoke(_STD get<_Indices>(_STD move(_Tuple))...)) { // invoke() a tuple with explicit parameter ordering
    return _STD invoke(_STD get<_Indices>(_STD move(_Tuple))...);
}

template <class... _Types>
auto _Invoke_stored(tuple<_Types...>&& _Tuple)
    -> decltype(_Invoke_stored_explicit(_STD move(_Tuple), index_sequence_for<_Types...>{})) { // invoke() a tuple
    return _Invoke_stored_explicit(_STD move(_Tuple), index_sequence_for<_Types...>{});
}

template <class... _Types>
class _Fake_no_copy_callable_adapter {
    // async() is built on packaged_task internals which incorrectly use
    // std::function, which requires that things be copyable. We can't fix this in an
    // update, so this adapter turns copies into terminate(). When VSO-153581 is
    // fixed, remove this adapter.
    using _Storaget = tuple<decay_t<_Types>...>;

public:
    explicit _Fake_no_copy_callable_adapter(_Types&&... _Vals) : _Storage(_STD forward<_Types>(_Vals)...) {
        // Initializes _Fake_no_copy_callable_adapter with a decayed callable object and arguments
    }

    _Fake_no_copy_callable_adapter(const _Fake_no_copy_callable_adapter& _Other)
        : _Storage(_STD move(_Other._Storage)) {
        _STD terminate(); // Very Bad Things
    }

    _Fake_no_copy_callable_adapter(_Fake_no_copy_callable_adapter&& _Other) = default;
    _Fake_no_copy_callable_adapter& operator=(const _Fake_no_copy_callable_adapter&) = delete;
    _Fake_no_copy_callable_adapter& operator=(_Fake_no_copy_callable_adapter&&) = delete;

    auto operator()() -> decltype(_Invoke_stored(_STD move(_STD declval<_Storaget&>()))) {
        return _Invoke_stored(_STD move(_Storage));
    }

private:
    mutable _Storaget _Storage;
};

template <class _Ret, class _Fty>
_Associated_state<typename _P_arg_type<_Ret>::type>* _Get_associated_state(
    launch _Psync, _Fty&& _Fnarg) { // construct associated asynchronous state object for the launch type
    switch (_Psync) { // select launch type
    case launch::deferred:
        return new _Deferred_async_state<_Ret>(_STD forward<_Fty>(_Fnarg));
    case launch::async: // TRANSITION, fixed in vMajorNext, should create a new thread here
    default:
        return new _Task_async_state<_Ret>(_STD forward<_Fty>(_Fnarg));
    }
}

template <class _Fty, class... _ArgTypes>
_NODISCARD future<_Invoke_result_t<decay_t<_Fty>, decay_t<_ArgTypes>...>> async(
    launch _Policy, _Fty&& _Fnarg, _ArgTypes&&... _Args) {
    // manages a callable object launched with supplied policy
    using _Ret   = _Invoke_result_t<decay_t<_Fty>, decay_t<_ArgTypes>...>;
    using _Ptype = typename _P_arg_type<_Ret>::type;
    _Promise<_Ptype> _Pr(
        _Get_associated_state<_Ret>(_Policy, _Fake_no_copy_callable_adapter<_Fty, _ArgTypes...>(
                                                 _STD forward<_Fty>(_Fnarg), _STD forward<_ArgTypes>(_Args)...)));

    return future<_Ret>(_Pr._Get_state_for_future(), _Nil());
}

template <class _Fty, class... _ArgTypes>
_NODISCARD future<_Invoke_result_t<decay_t<_Fty>, decay_t<_ArgTypes>...>> async(_Fty&& _Fnarg, _ArgTypes&&... _Args) {
    // manages a callable object launched with default policy
    return _STD async(launch::async | launch::deferred, _STD forward<_Fty>(_Fnarg), _STD forward<_ArgTypes>(_Args)...);
}

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED
// Experimental coroutine support for std::future. Subject to change/removal!
namespace experimental {
    template <class _Ty, class... _ArgTypes>
    struct coroutine_traits<future<_Ty>, _ArgTypes...> {
        // defines resumable traits for functions returning future<_Ty>
        struct promise_type {
            promise<_Ty> _MyPromise;

            future<_Ty> get_return_object() {
                return _MyPromise.get_future();
            }

            suspend_never initial_suspend() const noexcept {
                return {};
            }

            suspend_never final_suspend() const noexcept {
                return {};
            }

            template <class _Ut>
            void return_value(_Ut&& _Value) {
                _MyPromise.set_value(_STD forward<_Ut>(_Value));
            }

            void unhandled_exception() {
                _MyPromise.set_exception(_STD current_exception());
            }
        };
    };

    template <class... _ArgTypes>
    struct coroutine_traits<future<void>, _ArgTypes...> {
        // defines resumable traits for functions returning future<void>
        struct promise_type {
            promise<void> _MyPromise;

            future<void> get_return_object() {
                return _MyPromise.get_future();
            }

            suspend_never initial_suspend() const noexcept {
                return {};
            }

            suspend_never final_suspend() const noexcept {
                return {};
            }

            void return_void() {
                _MyPromise.set_value();
            }

            void unhandled_exception() {
                _MyPromise.set_exception(_STD current_exception());
            }
        };
    };

    template <class _Ty>
    struct _Future_awaiter {
        future<_Ty>& _Fut;

        bool await_ready() const {
            return _Fut._Is_ready();
        }

        void await_suspend(experimental::coroutine_handle<> _ResumeCb) {
            // TRANSITION, change to .then if and when future gets .then
            thread _WaitingThread([&_Fut = _Fut, _ResumeCb]() mutable {
                _Fut.wait();
                _ResumeCb();
            });
            _WaitingThread.detach();
        }

        decltype(auto) await_resume() {
            return _Fut.get();
        }
    };

} // namespace experimental

// Coroutines TS and C++20 coroutine adapter.
template <class _Ty>
auto operator co_await(future<_Ty>&& _Fut) {
    return experimental::_Future_awaiter<_Ty>{_Fut};
}

template <class _Ty>
auto operator co_await(future<_Ty>& _Fut) {
    return experimental::_Future_awaiter<_Ty>{_Fut};
}
#endif // _RESUMABLE_FUNCTIONS_SUPPORTED

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _FUTURE_
