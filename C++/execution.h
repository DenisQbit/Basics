// execution standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _EXECUTION_
#define _EXECUTION_
#include <yvals.h>
#if _STL_COMPILER_PREPROCESSOR

#if !_HAS_CXX17
#pragma message("The contents of <execution> are available only with C++17 or later.")
#else // ^^^ !_HAS_CXX17 / _HAS_CXX17 vvv
#include <algorithm>
#include <atomic>
#include <memory>
#include <mutex>
#include <numeric>
#include <queue>
#include <vector>
#include <xbit_ops.h>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_EXTERN_C
#ifdef _M_CEE
using __std_TP_WORK              = void;
using __std_TP_CALLBACK_INSTANCE = void;
using __std_TP_CALLBACK_ENVIRON  = void;
#else // ^^^ _M_CEE ^^^ // vvv !_M_CEE vvv
struct __std_TP_WORK; // not defined
struct __std_TP_CALLBACK_INSTANCE; // not defined
struct __std_TP_CALLBACK_ENVIRON; // not defined
#endif // _M_CEE

using __std_PTP_WORK              = __std_TP_WORK*;
using __std_PTP_CALLBACK_INSTANCE = __std_TP_CALLBACK_INSTANCE*;
using __std_PTP_CALLBACK_ENVIRON  = __std_TP_CALLBACK_ENVIRON*;

_NODISCARD unsigned int __stdcall __std_parallel_algorithms_hw_threads() noexcept;

using __std_PTP_WORK_CALLBACK = void(__stdcall*)(
    _Inout_ __std_PTP_CALLBACK_INSTANCE, _Inout_opt_ void*, _Inout_ __std_PTP_WORK);

_NODISCARD __std_PTP_WORK __stdcall __std_create_threadpool_work(
    _In_ __std_PTP_WORK_CALLBACK, _Inout_opt_ void*, _In_opt_ __std_PTP_CALLBACK_ENVIRON) noexcept;

void __stdcall __std_submit_threadpool_work(_Inout_ __std_PTP_WORK) noexcept;

void __stdcall __std_bulk_submit_threadpool_work(_Inout_ __std_PTP_WORK, _In_ size_t) noexcept;

void __stdcall __std_close_threadpool_work(_Inout_ __std_PTP_WORK) noexcept;

void __stdcall __std_wait_for_threadpool_work_callbacks(_Inout_ __std_PTP_WORK, _In_ int) noexcept;

void __stdcall __std_execution_wait_on_uchar(
    _In_ const volatile unsigned char* _Address, _In_ unsigned char _Compare) noexcept;

void __stdcall __std_execution_wake_by_address_all(_In_ const volatile void* _Address) noexcept;
_END_EXTERN_C

_STD_BEGIN
constexpr size_t _Oversubscription_multiplier = 32;
constexpr size_t _Oversubmission_multiplier   = 4;
constexpr size_t _Still_active                = static_cast<size_t>(-1);

namespace execution {
    class sequenced_policy {
        // indicates support for only sequential execution, and requests termination on exceptions
    public:
        using _Standard_execution_policy   = int;
        static constexpr bool _Parallelize = false;
        static constexpr bool _Ivdep       = false;
    };

    inline constexpr sequenced_policy seq{/* unspecified */};

    class parallel_policy {
        // indicates support by element access functions for parallel execution with parallel forward progress
        // guarantees, and requests termination on exceptions
    public:
        using _Standard_execution_policy   = int;
        static constexpr bool _Parallelize = true;
        static constexpr bool _Ivdep       = true;
    };

    inline constexpr parallel_policy par{/* unspecified */};

    class parallel_unsequenced_policy {
        // indicates support by element access functions for parallel execution with weakly parallel forward progress
        // guarantees, and requests termination on exceptions
        //
        // (at this time, equivalent to parallel_policy)
    public:
        using _Standard_execution_policy   = int;
        static constexpr bool _Parallelize = true;
        static constexpr bool _Ivdep       = true;
    };

    inline constexpr parallel_unsequenced_policy par_unseq{/* unspecified */};

#if _HAS_CXX20
    class unsequenced_policy {
        // indicates support by element access functions for weakly parallel forward progress guarantees, and for
        // executing interleaved on the same thread, and requests termination on exceptions
        //
        // (at this time, equivalent to sequenced_policy except for the for_each family)
    public:
        using _Standard_execution_policy   = int;
        static constexpr bool _Parallelize = false;
        static constexpr bool _Ivdep       = true;
    };

    inline constexpr unsequenced_policy unseq{/* unspecified */};
#endif // _HAS_CXX20

} // namespace execution

// All of the above are execution policies:
template <>
struct is_execution_policy<execution::sequenced_policy> : true_type {};

template <>
struct is_execution_policy<execution::parallel_policy> : true_type {};

template <>
struct is_execution_policy<execution::parallel_unsequenced_policy> : true_type {};

#if _HAS_CXX20
template <>
struct is_execution_policy<execution::unsequenced_policy> : true_type {};
#endif // _HAS_CXX20

struct _Parallelism_resources_exhausted : exception {
    _NODISCARD virtual const char* __CLR_OR_THIS_CALL what() const noexcept override {
        // return pointer to message string
        return "Insufficient resources were available to use additional parallelism.";
    }

#if !_HAS_EXCEPTIONS
protected:
    virtual void _Doraise() const override { // perform class-specific exception handling
        _RAISE(*this);
    }
#endif // !_HAS_EXCEPTIONS
};

[[noreturn]] inline void _Throw_parallelism_resources_exhausted() {
    _THROW(_Parallelism_resources_exhausted{});
}

enum class _Cancellation_status : bool { _Running, _Canceled };

struct _Cancellation_token {
    atomic<_Cancellation_status> _Is_canceled_impl{_Cancellation_status::_Running};

    bool _Is_canceled() const {
        return _Is_canceled_impl.load() == _Cancellation_status::_Canceled;
    }

    bool _Is_canceled_relaxed() const {
        return _Is_canceled_impl.load(memory_order_relaxed) == _Cancellation_status::_Canceled;
    }

    void _Cancel() {
        _Is_canceled_impl.store(_Cancellation_status::_Canceled);
    }

    _Cancellation_status _Status() const {
        return _Is_canceled_impl.load();
    }
};

class _Work_ptr {
public:
    template <class _Work, enable_if_t<!is_same_v<remove_cv_t<_Work>, _Work_ptr>, int> = 0>
    explicit _Work_ptr(_Work& _Operation)
        : _Ptp_work(__std_create_threadpool_work(&_Work::_Threadpool_callback, _STD addressof(_Operation), nullptr)) {
        // register work with the thread pool
        // usually, after _Work_ptr is constructed, a parallel algorithm runs to completion or terminates
        static_assert(noexcept(_Work::_Threadpool_callback(_STD declval<__std_PTP_CALLBACK_INSTANCE>(),
                          _STD declval<void*>(), _STD declval<__std_PTP_WORK>())),
            "Threadpool callbacks must be noexcept to enforce termination");
        if (!_Ptp_work) {
            // usually, the last place a bailout to serial execution can occur
            _Throw_parallelism_resources_exhausted();
        }
    }

    _Work_ptr(const _Work_ptr&) = delete;
    _Work_ptr& operator=(const _Work_ptr&) = delete;

    ~_Work_ptr() noexcept {
        __std_wait_for_threadpool_work_callbacks(_Ptp_work, true);
        __std_close_threadpool_work(_Ptp_work);
    }

    void _Submit() const noexcept {
        __std_submit_threadpool_work(_Ptp_work);
    }

    void _Submit(const size_t _Submissions) const noexcept {
        __std_bulk_submit_threadpool_work(_Ptp_work, _Submissions);
    }

    void _Submit_for_chunks(const size_t _Hw_threads, const size_t _Chunks) const noexcept {
        _Submit((_STD min)(_Hw_threads * _Oversubmission_multiplier, _Chunks));
    }

private:
    __std_PTP_WORK _Ptp_work;
};

template <class _Work>
void _Run_available_chunked_work(_Work& _Operation) {
    while (_Operation._Process_chunk() == _Cancellation_status::_Running) { // process while there are chunks remaining
    }
}

template <class _Work>
void _Run_chunked_parallel_work(const size_t _Hw_threads, _Work& _Operation) {
    // process chunks of _Operation on the thread pool
    const _Work_ptr _Work_op{_Operation};
    // setup complete, hereafter nothrow or terminate
    _Work_op._Submit_for_chunks(_Hw_threads, _Operation._Team._Chunks);
    _Run_available_chunked_work(_Operation);
}

// The parallel algorithms library below assumes that distance(first, last) fits into a size_t;
// forward iterators must refer to objects in memory and therefore must meet this requirement.
//
// Unlike the serial algorithms library, which can stay in the difference_type domain, here we need
// to talk with vector (which speaks size_t), and with Windows, which wants to speak unsigned int.
//
// This assumption should be localized to the chunk calculation functions; the rest of
// the library assumes that chunk numbers can be static_cast into the difference_type domain.

template <class _Diff>
constexpr size_t _Get_chunked_work_chunk_count(const size_t _Hw_threads, const _Diff _Count) {
    // get the number of chunks to break work into to parallelize
    const auto _Size_count = static_cast<size_t>(_Count); // no overflow due to forward iterators
    // we assume _Hw_threads * _Oversubscription_multiplier does not overflow
    return (_STD min)(_Hw_threads * _Oversubscription_multiplier, _Size_count);
}

template <class _Diff>
constexpr size_t _Get_least2_chunked_work_chunk_count(const size_t _Hw_threads, const _Diff _Count) {
    // get the number of chunks to break work into to parallelize, assuming chunks must be of size 2
    const auto _Size_count = static_cast<size_t>(_Count); // no overflow due to forward iterators
    // we assume _Hw_threads * _Oversubscription_multiplier does not overflow
    return _Get_chunked_work_chunk_count(_Hw_threads, _Size_count / 2);
}

struct _Parallelism_allocate_traits {
    __declspec(allocator) static void* _Allocate(const size_t _Bytes) {
        void* _Result = ::operator new(_Bytes, nothrow);
        if (!_Result) {
            _Throw_parallelism_resources_exhausted();
        }

        return _Result;
    }

#ifdef __cpp_aligned_new
    __declspec(allocator) static void* _Allocate_aligned(const size_t _Bytes, const size_t _Align) {
        void* _Result = ::operator new (_Bytes, align_val_t{_Align}, nothrow);
        if (!_Result) {
            _Throw_parallelism_resources_exhausted();
        }

        return _Result;
    }
#endif // __cpp_aligned_new
};

template <class _Ty = void>
struct _Parallelism_allocator {
    using value_type = _Ty;

    _Parallelism_allocator() = default;

    template <class _Other>
    constexpr _Parallelism_allocator(const _Parallelism_allocator<_Other>&) noexcept {}

    _Ty* allocate(const size_t _Count) {
        return static_cast<_Ty*>(
            _Allocate<_New_alignof<_Ty>, _Parallelism_allocate_traits>(_Get_size_of_n<sizeof(_Ty)>(_Count)));
    }

    void deallocate(_Ty* const _Ptr, const size_t _Count) {
        // no overflow check on the following multiply; we assume _Allocate did that check
        _Deallocate<_New_alignof<_Ty>>(_Ptr, sizeof(_Ty) * _Count);
    }

    template <class _Other>
    bool operator==(const _Parallelism_allocator<_Other>&) const noexcept {
        return true;
    }

    template <class _Other>
    bool operator!=(const _Parallelism_allocator<_Other>&) const noexcept {
        return false;
    }
};

template <class _Ty>
using _Parallel_vector = vector<_Ty, _Parallelism_allocator<_Ty>>;

template <class _Ty>
struct _Generalized_sum_drop { // drop off point for GENERALIZED_SUM intermediate results
    _Ty* _Data;
    size_t _Slots;
    atomic<size_t> _Frontier;

    explicit _Generalized_sum_drop(const size_t _Slots)
        : _Data(static_cast<_Ty*>(
            _Allocate<_New_alignof<_Ty>, _Parallelism_allocate_traits>(_Get_size_of_n<sizeof(_Ty)>(_Slots)))),
          _Slots(_Slots), _Frontier(0) {}

    ~_Generalized_sum_drop() noexcept {
        // pre: the caller has synchronized with all threads that modify _Data.
        _Destroy_range(begin(), end());
        // no overflow check on the following multiply; we assume _Allocate did that check
        _Deallocate<_New_alignof<_Ty>>(_Data, sizeof(_Ty) * _Slots);
    }

    template <class... _Args>
    void _Add_result(_Args&&... _Vals) noexcept /* terminates */ {
        // constructs a _Ty in place with _Vals parameters perfectly forwarded
        // pre: the number of results added is less than the size the drop was constructed with
        const size_t _Target = _Frontier++;
        _Construct_in_place(_Data[_Target], _STD forward<_Args>(_Vals)...);
    }

    _Ty* begin() {
        return _Data;
    }

    _Ty* end() {
        return _Data + _Frontier.load(memory_order_relaxed);
    }
};

template <class _Ty>
struct _Atomic_is_usually_lock_free : bool_constant<atomic<_Ty>::is_always_lock_free> {
    // deferred evaluation of atomic::is_always_lock_free
};

template <class _FwdIt>
inline constexpr bool _Use_atomic_iterator = conjunction_v<bool_constant<_Is_random_iter_v<_FwdIt>>,
    is_trivially_copyable<_FwdIt>, _Atomic_is_usually_lock_free<_FwdIt>>;

template <class _Ty>
struct _Parallel_choose_min_result { // parallel results collector which uses atomic<_Ty> to choose the minimum value
    _Ty _Last;
    atomic<_Ty> _Result;

    explicit _Parallel_choose_min_result(_Ty _Last_) : _Last{_Last_}, _Result{_Last} {}

    _Ty _Get_result() const { // load the imbued value
        return _Result.load(memory_order_relaxed);
    }

    bool _Complete() const { // tests whether a result has been found
        return _Result.load(memory_order_relaxed) != _Last;
    }

    void _Imbue(size_t, const _Ty _Local_result) { // atomically sets the result to min(result, _Local_result)
        _Ty _Expected{_Last};
        while (!_Result.compare_exchange_weak(_Expected, _Local_result) && _Expected > _Local_result) { // keep trying
        }
    }
};

template <class _Ty>
struct _Parallel_choose_max_result { // parallel results collector which uses atomic<_Ty> to choose the maximum value
    _Ty _Last;
    atomic<_Ty> _Result;

    explicit _Parallel_choose_max_result(_Ty _Last_) : _Last{_Last_}, _Result{_Last} {}

    _Ty _Get_result() const { // load the imbued value
        return _Result.load(memory_order_relaxed);
    }

    bool _Complete() const { // tests whether a result has been found
        return _Result.load(memory_order_relaxed) != _Last;
    }

    void _Imbue(size_t, const _Ty _Local_result) { // atomically sets the result to max(result, _Local_result)
        _Ty _Expected{_Last};
        if (_Result.compare_exchange_strong(_Expected, _Local_result)) {
            return;
        }

        while (_Expected < _Local_result && !_Result.compare_exchange_weak(_Expected, _Local_result)) { // keep trying
        }
    }
};

template <class _Ty>
struct _Parallel_choose_min_chunk {
    // parallel results collector which uses atomic<chunk number> to choose the lowest chunk's result
    _Ty _Result;
    atomic<size_t> _Selected_chunk;
    mutex _Mtx;

    explicit _Parallel_choose_min_chunk(_Ty _Last) : _Result(_Last), _Selected_chunk{_Still_active}, _Mtx{} {}

    _Ty _Get_result() const { // load the imbued value
        return _Result;
    }

    bool _Complete() const { // tests whether a result has been found
        return _Selected_chunk.load(memory_order_relaxed) != _Still_active;
    }

    void _Imbue(const size_t _Chunk, const _Ty _Local_result) {
        // atomically sets the result to the lowest chunk's value
        size_t _Expected = _Still_active;
        while (!_Selected_chunk.compare_exchange_weak(_Expected, _Chunk)) {
            // note: _Still_active is the maximum possible value, so it gets ignored implicitly
            if (_Chunk > _Expected) {
                return;
            }
        }

        lock_guard<mutex> _Lck(_Mtx); // TRANSITION, VSO-671180
        if (_Selected_chunk.load(memory_order_relaxed) == _Chunk) {
            _Result = _Local_result;
        }
    }
};

template <class _Ty>
struct _Parallel_choose_max_chunk {
    // parallel results collector which uses atomic<chunk number> to choose the highest chunk's result
    _Ty _Result;
    atomic<size_t> _Selected_chunk;
    mutex _Mtx;

    explicit _Parallel_choose_max_chunk(_Ty _Last) : _Result(_Last), _Selected_chunk{_Still_active}, _Mtx{} {}

    _Ty _Get_result() const { // load the imbued value
        return _Result;
    }

    bool _Complete() const { // tests whether a result has been found
        return _Selected_chunk.load(memory_order_relaxed) != _Still_active;
    }

    void _Imbue(const size_t _Chunk, const _Ty _Local_result) {
        // atomically sets the result to the highest chunk's value
        size_t _Expected = _Still_active;
        while (!_Selected_chunk.compare_exchange_weak(_Expected, _Chunk)) {
            // wrap _Still_active down to 0 so that only 1 branch is necessary:
            if (_Chunk + 1 < _Expected + 1) {
                return;
            }
        }

        lock_guard<mutex> _Lck(_Mtx); // TRANSITION, VSO-671180
        if (_Selected_chunk.load(memory_order_relaxed) == _Chunk) {
            _Result = _Local_result;
        }
    }
};

template <class _Ty>
struct alignas(_Ty) alignas(size_t) alignas(_Atomic_counter_t) _Circular_buffer { // work stealing deque extent type
    static_assert(is_trivial_v<_Ty>, "Work stealing deques work only with trivial operations");

    size_t _Log_size;
    _Atomic_counter_t _Ref_count;

    void _Release() {
        static_assert(is_trivially_destructible_v<_Circular_buffer>, "global delete requires trivial destruction");
        if (_MT_DECR(_Ref_count) == 0) {
            ::operator delete(this);
        }
    }

    static _Circular_buffer* _Allocate_circular_buffer(const size_t _New_log_size) {
        // allocate a circular buffer with space for 2^_New_log_size elements
        if (_New_log_size >= 32) {
            _Throw_parallelism_resources_exhausted();
        }

        const size_t _Count         = static_cast<size_t>(1) << _New_log_size;
        constexpr size_t _Max_bytes = static_cast<size_t>(-1) - sizeof(_Circular_buffer);
        if (_Max_bytes / sizeof(_Ty) < _Count) {
            _Throw_parallelism_resources_exhausted();
        }

        const size_t _Result_bytes = _Count * sizeof(_Ty) + sizeof(_Circular_buffer);
        static_assert(alignof(_Ty) <= alignof(max_align_t), "incapable of supporting the requested alignment");
        const auto _Result  = static_cast<_Circular_buffer*>(::operator new(_Result_bytes));
        _Result->_Log_size  = _New_log_size;
        _Result->_Ref_count = 1;
        return _Result;
    }

    static _Circular_buffer* _New_circular_buffer() { // allocate a circular buffer with a default number of elements
        return _Allocate_circular_buffer(6); // start with 64 elements
    }

    _Ty* _Get_base() { // get the base address where the _Ty instances are stored
        return reinterpret_cast<_Ty*>(this + 1);
    }

    const _Ty* _Get_base() const { // get the base address where the _Ty instances are stored
        return reinterpret_cast<const _Ty*>(this + 1);
    }

    _Ty& _Subscript(const size_t _Idx) { // get a reference to the _Idxth element
        const auto _Mask = (static_cast<size_t>(1) << _Log_size) - static_cast<size_t>(1);
        return _Get_base()[_Idx & _Mask];
    }

    const _Ty& _Subscript(const size_t _Idx) const { // get a reference to the _Idxth element
        const auto _Mask = (static_cast<size_t>(1) << _Log_size) - static_cast<size_t>(1);
        return _Get_base()[_Idx & _Mask];
    }

    _Circular_buffer* _Grow(const size_t _Bottom, const size_t _Top) const {
        // create a bigger _Circular_buffer suitable for use by a _Work_stealing_deque<_Ty> with bounds _Bottom and _Top
        const size_t _New_log_size = _Log_size + 1;
        _Circular_buffer* _Result  = _Allocate_circular_buffer(_New_log_size);
        for (size_t _Idx = _Top; _Idx < _Bottom; ++_Idx) {
            _Result->_Subscript(_Idx) = _Subscript(_Idx);
        }

        return _Result;
    }
};

#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier
template <class _Ty>
class alignas(hardware_destructive_interference_size) _Work_stealing_deque {
    // thread-local work-stealing deque, which allows efficient access from a single owner thread at the "bottom"
    // of the queue, and any thread access to the "top" of the queue. Originally described in the paper
    // "Dynamic Circular Work-Stealing Deque" by David Chase and Yossi Lev
public:
    _Work_stealing_deque()                            = default;
    _Work_stealing_deque(const _Work_stealing_deque&) = delete;
    _Work_stealing_deque& operator=(const _Work_stealing_deque&) = delete;

    ~_Work_stealing_deque() noexcept {
        _Segment->_Release();
    }

    void _Push_bottom(_Ty& _Val) {
        // attempts to push _Val onto the bottom of this queue
        // may be accessed by owning thread only
        const auto _Local_b = _Bottom.load();
        if (_Local_b == SIZE_MAX) {
            // we assume that any input range won't be divided into more than SIZE_MAX subproblems;
            // treat overflow of that kind as OOM
            _Throw_parallelism_resources_exhausted();
        }

        const auto _Local_t = _Top.load();
        const auto _Size    = _Local_b - _Local_t;
        if (_Size >= (static_cast<size_t>(1) << _Segment->_Log_size)) {
            auto _New_segment = _Segment->_Grow(_Local_b, _Local_t);
            _Circular_buffer<_Ty>* _Detached_segment;
            {
                lock_guard<mutex> _Lck(_Segment_lock); // TRANSITION, VSO-671180
                _Detached_segment = _STD exchange(_Segment, _New_segment);
            } // unlock

            _Detached_segment->_Release();
        }

        _Segment->_Subscript(_Local_b) = _Val;
        _Bottom.store(_Local_b + 1U);
    }

    bool _Steal(_Ty& _Val) noexcept {
        // attempt to pop an item from the top of this deque
        // may be accessed by any thread
        // returns false if the deque was empty and _Val is indeterminate; otherwise, returns true and sets _Val to the
        // element retrieved from the top of the deque.
        auto _Local_t = _Top.load();
        size_t _Desired_t;
        do {
            if (_Bottom.load() <= _Local_t) { // deque was empty
                return false;
            }

            _Circular_buffer<_Ty>* _Stealing_segment;
            {
                lock_guard<mutex> _Lck(_Segment_lock); // TRANSITION, VSO-671180
                _Stealing_segment = _Segment;
                _MT_INCR(_Stealing_segment->_Ref_count);
            }

            _Val = _Stealing_segment->_Subscript(_Local_t); // speculative read/write data race
            _Stealing_segment->_Release();
            // The above is technically prohibited by the C++ memory model, but happens
            // to be well defined on all hardware this implementation targets.
            // Hardware with trap representations or similar must not use this implementation.
            _Desired_t = _Local_t + 1U;
        } while (!_Top.compare_exchange_strong(_Local_t, _Desired_t)); // if a data race occurred, try again

        return true;
    }

    bool _Try_pop_bottom(_Ty& _Val) noexcept {
        // attempt to pop an item from the bottom of this deque into _Val
        // may be accessed by owning thread only
        auto _Local_b = _Bottom.load();
        if (_Local_b == 0) { // queue never contained any elements (should never happen)
            return false;
        }

        --_Local_b;
        _Bottom.store(_Local_b);
        auto _Local_t = _Top.load();
        if (_Local_b < _Local_t) { // all elements were stolen before we got here
            _Bottom.store(_Local_t);
            return false;
        }

        // memory model says following load is OK, since _Push_bottom can't run concurrently
        _Val = _Segment->_Subscript(_Local_b);

        if (_Local_b > _Local_t) {
            // other threads only look at top, so we get the bottom without synchronization
            return true;
        }

        // We're trying to read the last element that another thread may be trying to steal;
        // see who gets to keep the element through _Top (effectively, steal from ourselves)
        const auto _Desired_top = _Local_t + 1U;
        if (_Top.compare_exchange_strong(_Local_t, _Desired_top)) {
            _Bottom.store(_Desired_top);
            return true;
        } else {
            _Bottom.store(_Local_t);
            return false;
        }
    }

private:
    atomic<size_t> _Bottom{0}; // modified by only owning thread
    atomic<size_t> _Top{0}; // modified by all threads
    _Guarded_by_(_Segment_lock) _Circular_buffer<_Ty>* _Segment{_Circular_buffer<_Ty>::_New_circular_buffer()};
    mutex _Segment_lock{};
};
#pragma warning(pop)

enum class _Steal_result { _Success, _Abort, _Done };

template <class _Ty>
struct _Work_stealing_team;

template <class _Ty>
struct _Work_stealing_membership { // thread-local "ticket" that team members use to talk with a _Work_stealing_team
    using _Diff = typename _Ty::difference_type;

    size_t _Id;
    _Work_stealing_team<_Ty>* _Team;
    _Diff _Work_complete;

    void _Push_bottom(_Ty& _Val) {
        _Team->_Queues[_Id]._Push_bottom(_Val);
    }

    bool _Try_pop_bottom(_Ty& _Val) noexcept {
        return _Team->_Queues[_Id]._Try_pop_bottom(_Val);
    }

    _Steal_result _Steal(_Ty& _Val) noexcept {
        _Diff _Remaining;
        const auto _Completed_this_time = _STD exchange(_Work_complete, {});
        if (_Completed_this_time == 0) {
            _Remaining = _Team->_Remaining_work.load();
        } else {
            _Remaining = _Team->_Remaining_work -= _Completed_this_time;
        }

        if (_Remaining == 0) {
            return _Steal_result::_Done;
        }

        const size_t _High = _Team->_Queues_used.load() + 1;
        size_t _Idx        = _Id;
        for (;;) {
            if (_Idx == 0) {
                _Idx = _High;
            }

            --_Idx;
            if (_Idx == _Id) {
                return _Steal_result::_Abort;
            }

            if (_Team->_Queues[_Idx]._Steal(_Val)) {
                return _Steal_result::_Success;
            }
        }
    }

    void _Leave() noexcept {
        _Team->_Leave_team(_Id);
    }
};

template <class _Ty>
struct _Work_stealing_team { // inter-thread communication for threads working on a single task
    using _Diff = typename _Ty::difference_type;

    static _Parallel_vector<size_t> _Get_queues(const size_t _Queue_count) {
        _Parallel_vector<size_t> _Result(_Queue_count);
        _STD iota(_Result.begin(), _Result.end(), size_t{});
        return _Result;
    }

    _Work_stealing_team(size_t _Threads, _Diff _Total_work)
        : _Queues(_Threads), _Queues_used(0), _Remaining_work(_Total_work), _Available_mutex(),
          _Available_queues(greater{}, _Get_queues(_Threads)) {} // register work with the thread pool

    _Work_stealing_membership<_Ty> _Join_team() noexcept {
        size_t _Id;
        {
            lock_guard<mutex> _Lck(_Available_mutex); // TRANSITION, VSO-671180
            _Id = _Available_queues.top();
            _Available_queues.pop();
        } // unlock

        // set _Queues_used to the high water mark of queues used
        size_t _High_water = _Queues_used.load();
        while (_High_water < _Id && !_Queues_used.compare_exchange_weak(_High_water, _Id)) { // keep trying
        }

        return _Work_stealing_membership<_Ty>{_Id, this, 0};
    }

    void _Leave_team(size_t _Id) noexcept {
        lock_guard<mutex> _Lck(_Available_mutex); // TRANSITION, VSO-671180
        _Available_queues.push(_Id);
    }

    _Parallel_vector<_Work_stealing_deque<_Ty>> _Queues;
    atomic<size_t> _Queues_used;
    atomic<_Diff> _Remaining_work;

    mutex _Available_mutex;
    priority_queue<size_t, _Parallel_vector<size_t>, greater<>> _Available_queues;
};

template <class _Diff>
struct _Static_partition_key { // "pointer" identifying a static partition
    size_t _Chunk_number; // In range [0, numeric_limits<_Diff>::max()]
    _Diff _Start_at;
    _Diff _Size;

    explicit operator bool() const { // test if this is a valid key
        return _Chunk_number != static_cast<size_t>(-1);
    }
};

template <class _Diff>
struct _Static_partition_team { // common data for all static partitioned ops
    atomic<size_t> _Consumed_chunks;
    size_t _Chunks;
    _Diff _Count;
    _Diff _Chunk_size;
    _Diff _Unchunked_items;

    _Static_partition_team(const _Diff _Count_, const size_t _Chunks_)
        : _Consumed_chunks{0}, _Chunks{_Chunks_}, _Count{_Count_}, _Chunk_size{static_cast<_Diff>(
                                                                       _Count_ / static_cast<_Diff>(_Chunks_))},
          _Unchunked_items{static_cast<_Diff>(_Count_ % static_cast<_Diff>(_Chunks_))} {
        // Calculate common data for statically partitioning iterator ranges.
        // pre: _Count_ >= _Chunks_ && _Chunks_ >= 1
    }

    _Static_partition_key<_Diff> _Get_chunk_key(const size_t _This_chunk) const {
        const auto _This_chunk_diff = static_cast<_Diff>(_This_chunk);
        auto _This_chunk_size       = _Chunk_size;
        auto _This_chunk_start_at   = static_cast<_Diff>(_This_chunk_diff * _This_chunk_size);
        if (_This_chunk_diff < _Unchunked_items) {
            // chunks at index lower than _Unchunked_items get an extra item,
            // and need to shift forward by all their predecessors' extra items
            _This_chunk_start_at += _This_chunk_diff;
            ++_This_chunk_size;
        } else { // chunks without an extra item need to account for all the extra items
            _This_chunk_start_at += _Unchunked_items;
        }

        return {_This_chunk, _This_chunk_start_at, _This_chunk_size};
    }

    _Diff _Get_chunk_offset(const size_t _This_chunk) const {
        const auto _This_chunk_diff = static_cast<_Diff>(_This_chunk);
        return _This_chunk_diff * _Chunk_size + (_STD min)(_This_chunk_diff, _Unchunked_items);
    }

    _Static_partition_key<_Diff> _Get_next_key() {
        // retrieves the next static partition key to process, if it exists;
        // otherwise, retrieves an invalid partition key
        const auto _This_chunk = _Consumed_chunks++;
        if (_This_chunk < _Chunks) {
            return _Get_chunk_key(_This_chunk);
        }

        return {static_cast<size_t>(-1), 0, 0};
    }
};

template <class _FwdIt>
struct _Iterator_range { // record of a partition of work
    _FwdIt _First;
    _FwdIt _Last;
};

template <class _FwdIt, class _Diff = _Iter_diff_t<_FwdIt>, bool = _Is_random_iter_v<_FwdIt>>
struct _Static_partition_range;

template <class _RanIt, class _Diff>
struct _Static_partition_range<_RanIt, _Diff, true> {
    using _Target_diff = _Iter_diff_t<_RanIt>;
    using _URanIt      = _Unwrapped_t<const _RanIt&>;
    _URanIt _Start_at;
    using _Chunk_type = _Iterator_range<_URanIt>;

    _RanIt _Populate(const _Static_partition_team<_Diff>& _Team, _RanIt _First) {
        // statically partition a random-access iterator range and return next(_First, _Team._Count)
        // pre: _Populate hasn't yet been called on this instance
        auto _Result = _First + static_cast<_Target_diff>(_Team._Count); // does verification
        _Start_at    = _Get_unwrapped(_First);
        return _Result;
    }

    bool _Populate(const _Static_partition_team<_Diff>& _Team, _RanIt _First, _RanIt _Last) {
        // statically partition a random-access iterator range and check if the range ends at _Last
        // pre: _Populate hasn't yet been called on this instance
        _Adl_verify_range(_First, _Last);
        _Start_at = _Get_unwrapped(_First);
        return _Team._Count == _Last - _First;
    }

    _URanIt _Get_first(size_t /* _Chunk_number */, const _Diff _Offset) {
        // get the first iterator for _Chunk _Chunk_number (which is at offset _Offset)
        return _Start_at + static_cast<_Target_diff>(_Offset);
    }

    _Chunk_type _Get_chunk(const _Static_partition_key<_Diff> _Key) const {
        // get a static partition chunk from a random-access range
        // pre: _Key was generated by the _Static_partition_team instance passed to a previous call to _Populate
        const auto _First = _Start_at + static_cast<_Target_diff>(_Key._Start_at);
        return {_First, _First + static_cast<_Target_diff>(_Key._Size)};
    }
};

template <class _FwdIt, class _Diff>
struct _Static_partition_range<_FwdIt, _Diff, false> {
    using _Target_diff = _Iter_diff_t<_FwdIt>;
    using _UFwdIt      = _Unwrapped_t<const _FwdIt&>;
    _Parallel_vector<_UFwdIt> _Division_points;
    using _Chunk_type = _Iterator_range<_UFwdIt>;

    _FwdIt _Populate(const _Static_partition_team<_Diff>& _Team, _FwdIt _First) {
        // statically partition a forward iterator range and return next(_First, _Team._Count)
        // pre: _Populate hasn't yet been called on this instance
        const auto _Chunks = _Team._Chunks;
        _Division_points.resize(_Chunks + 1);
        // The following potentially narrowing cast is OK because caller has ensured
        // next(_First, _Team._Count) is valid (and _Count <= _Chunk_size)
        const auto _Chunk_size      = static_cast<_Target_diff>(_Team._Chunk_size);
        const auto _Unchunked_items = _Team._Unchunked_items;
        auto _Result                = _Division_points.begin();
        *_Result                    = _Get_unwrapped(_First);
        for (_Diff _Idx{}; _Idx < _Unchunked_items; ++_Idx) { // record bounds of chunks with an extra item
            _STD advance(_First, static_cast<_Target_diff>(_Chunk_size + 1));
            *++_Result = _Get_unwrapped(_First);
        }

        const auto _Diff_chunks = static_cast<_Diff>(_Chunks);
        for (_Diff _Idx = _Unchunked_items; _Idx < _Diff_chunks; ++_Idx) { // record bounds of chunks with no extra item
            _STD advance(_First, _Chunk_size);
            *++_Result = _Get_unwrapped(_First);
        }

        return _First;
    }

    bool _Populate(const _Static_partition_team<_Diff>& _Team, _FwdIt _First, _FwdIt _Last) {
        // statically partition a forward iterator range and check if the range ends at _Last
        // pre: _Populate hasn't yet been called on this instance
        const auto _Chunks = _Team._Chunks;
        _Division_points.resize(_Chunks + 1);
        const auto _Chunk_size      = _Team._Chunk_size;
        const auto _Unchunked_items = _Team._Unchunked_items;
        auto _Result                = _Division_points.begin();
        *_Result                    = _Get_unwrapped(_First);
        for (_Diff _Idx{}; _Idx < _Unchunked_items; ++_Idx) { // record bounds of chunks with an extra item
            for (_Diff _This_chunk_size = _Chunk_size + 1; 0 < _This_chunk_size--;) {
                if (_First == _Last) {
                    return false;
                }

                ++_First;
            }

            *++_Result = _Get_unwrapped(_First);
        }

        const auto _Diff_chunks = static_cast<_Diff>(_Chunks);
        for (_Diff _Idx = _Unchunked_items; _Idx < _Diff_chunks; ++_Idx) { // record bounds of chunks with no extra item
            for (_Diff _This_chunk_size = _Chunk_size; 0 < _This_chunk_size--;) {
                if (_First == _Last) {
                    return false;
                }

                ++_First;
            }

            *++_Result = _Get_unwrapped(_First);
        }

        return _First == _Last;
    }

    _UFwdIt _Get_first(const size_t _Chunk_number, _Diff /* _Offset */) {
        // get the first iterator for _Chunk _Chunk_number (which is at offset _Offset)
        return _Division_points[_Chunk_number];
    }

    _Chunk_type _Get_chunk(const _Static_partition_key<_Diff> _Key) const {
        // get a static partition chunk from a forward range
        // pre: _Key was generated by the _Static_partition_team instance passed to a previous call to _Populate
        return {_Division_points[_Key._Chunk_number], _Division_points[_Key._Chunk_number + 1]};
    }
};

template <class _BidIt, class _Diff = _Iter_diff_t<_BidIt>, bool = _Is_random_iter_v<_BidIt>>
struct _Static_partition_range_backward;

template <class _RanIt, class _Diff>
struct _Static_partition_range_backward<_RanIt, _Diff, true> {
    using _Target_diff = _Iter_diff_t<_RanIt>;
    _Unwrapped_t<const _RanIt&> _Start_at;
    using _Chunk_type = _Iterator_range<_Unwrapped_t<const _RanIt&>>;

    void _Populate(const _Static_partition_team<_Diff>& _Team, _RanIt _Last) {
        // statically partition a random-access iterator range ending at _Last
        // pre: _Populate hasn't yet been called on this instance
        _Start_at = _Get_unwrapped_n(_Last, -static_cast<_Target_diff>(_Team._Count));
    }

    _Chunk_type _Get_chunk(const _Static_partition_key<_Diff> _Key) const {
        // get a static partition chunk from a random-access range
        // pre: _Key was generated by the _Static_partition_team instance passed to a previous call to _Populate
        const auto _Last = _Start_at - static_cast<_Target_diff>(_Key._Start_at);
        return {_Last - static_cast<_Target_diff>(_Key._Size), _Last};
    }
};

template <class _BidIt, class _Diff>
struct _Static_partition_range_backward<_BidIt, _Diff, false> {
    using _Target_diff = _Iter_diff_t<_BidIt>;
    _Parallel_vector<_Unwrapped_t<const _BidIt&>> _Division_points;
    using _Chunk_type = _Iterator_range<_Unwrapped_t<const _BidIt&>>;

    void _Populate(const _Static_partition_team<_Diff>& _Team, _BidIt _Last) {
        // statically partition a bidirectional iterator range ending at _Last
        // pre: _Populate hasn't yet been called on this instance
        const auto _Chunks = _Team._Chunks;
        _Division_points.resize(_Chunks + 1);
        const auto _Neg_chunk_size  = static_cast<_Target_diff>(-_Team._Chunk_size);
        const auto _Unchunked_items = _Team._Unchunked_items;
        auto _Result = _Division_points.begin(); // does range checking by incrementing in the checked domain
        *_Result     = _Get_unwrapped(_Last);
        for (_Diff _Idx{}; _Idx < _Unchunked_items; ++_Idx) {
            _STD advance(_Last, static_cast<_Target_diff>(_Neg_chunk_size - 1));
            *++_Result = _Get_unwrapped(_Last);
        }

        const auto _Diff_chunks = static_cast<_Diff>(_Chunks);
        for (_Diff _Idx = _Unchunked_items; _Idx < _Diff_chunks; ++_Idx) {
            _STD advance(_Last, _Neg_chunk_size);
            *++_Result = _Get_unwrapped(_Last);
        }
    }

    _Chunk_type _Get_chunk(const _Static_partition_key<_Diff> _Key) const {
        // get a static partition chunk from a bidirectional range
        // pre: _Key was generated by the _Static_partition_team instance passed to a previous call to _Populate
        return {_Division_points[_Key._Chunk_number + 1], _Division_points[_Key._Chunk_number]};
    }
};

template <class _InIt1, class _InIt2>
_Common_diff_t<_InIt1, _InIt2> _Distance_any(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _InIt2 _Last2) {
    // get the distance from 2 ranges which should have identical lengths
    if constexpr (_Is_random_iter_v<_InIt1>) {
        return _Last1 - _First1;
    } else if constexpr (_Is_random_iter_v<_InIt2>) {
        return _Last2 - _First2;
    } else {
        return _STD distance(_First1, _Last1);
    }
}

template <class _InIt1, class _InIt2>
_Common_diff_t<_InIt1, _InIt2> _Distance_min(_InIt1 _First1, const _InIt1 _Last1, _InIt2 _First2, const _InIt2 _Last2) {
    // get min(distance(_First1, _Last1), distance(_First2, _Last2))
    using _CT = _Common_diff_t<_InIt1, _InIt2>;
    _CT _Result{};
    if constexpr (_Is_random_iter_v<_InIt1> && _Is_random_iter_v<_InIt2>) {
        const _CT _Count1 = _Last1 - _First1;
        const _CT _Count2 = _Last2 - _First2;
        _Result           = (_STD min)(_Count1, _Count2);
    } else if constexpr (_Is_random_iter_v<_InIt1>) {
        for (auto _Count1 = _Last1 - _First1; 0 < _Count1 && _First2 != _Last2; --_Count1) {
            ++_First2;
            ++_Result;
        }
    } else if constexpr (_Is_random_iter_v<_InIt2>) {
        for (auto _Count2 = _Last2 - _First2; 0 < _Count2 && _First1 != _Last1; --_Count2) {
            ++_First1;
            ++_Result;
        }
    } else {
        while (_First1 != _Last1 && _First2 != _Last2) {
            ++_First1;
            ++_First2;
            ++_Result;
        }
    }

    return _Result;
}

template <bool _Invert, class _FwdIt, class _Pr>
struct _Static_partitioned_all_of_family2 { // all_of/any_of/none_of task scheduled on the system thread pool
    _Static_partition_team<_Iter_diff_t<_FwdIt>> _Team;
    _Static_partition_range<_FwdIt> _Basis;
    _Pr _Pred;
    _Cancellation_token _Cancel_token;

    _Static_partitioned_all_of_family2(
        _FwdIt _First, const size_t _Hw_threads, const _Iter_diff_t<_FwdIt> _Count, _Pr _Pred_)
        : _Team{_Count, _Get_chunked_work_chunk_count(_Hw_threads, _Count)}, _Basis{}, _Pred(_Pred_), _Cancel_token{} {
        _Basis._Populate(_Team, _First);
    }

    _Cancellation_status _Process_chunk() {
        if (_Cancel_token._Is_canceled()) {
            return _Cancellation_status::_Canceled;
        }

        const auto _Key = _Team._Get_next_key();
        if (!_Key) {
            return _Cancellation_status::_Canceled;
        }
        // Once _Key is obtained, the amount of work should not be discarded (see GH-818).

        const auto _Range = _Basis._Get_chunk(_Key);
        for (auto _First = _Range._First; _First != _Range._Last; ++_First) {
            if (_Pred(*_First) ? _Invert : !_Invert) {
                _Cancel_token._Cancel();
                return _Cancellation_status::_Canceled;
            }
        }

        return _Cancellation_status::_Running;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_all_of_family2*>(_Context));
    }
};

template <bool _Invert, class _FwdIt, class _Pr>
bool _All_of_family_parallel(_FwdIt _First, const _FwdIt _Last, _Pr _Pred) {
    // test if all elements in [_First, _Last) satisfy _Pred (or !_Pred if _Invert is true) in parallel
    const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
    if (_Hw_threads > 1) { // parallelize on multiprocessor machines...
        const auto _Count = _STD distance(_First, _Last);
        if (_Count >= 2) { // ... with at least 2 elements
            _TRY_BEGIN
            _Static_partitioned_all_of_family2<_Invert, _FwdIt, _Pr> _Operation{_First, _Hw_threads, _Count, _Pred};
            _Run_chunked_parallel_work(_Hw_threads, _Operation);
            return !_Operation._Cancel_token._Is_canceled_relaxed();
            _CATCH(const _Parallelism_resources_exhausted&)
            // fall through to serial case below
            _CATCH_END
        }
    }

    for (; _First != _Last; ++_First) {
        if (_Pred(*_First) ? _Invert : !_Invert) {
            return false;
        }
    }

    return true;
}

template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD bool all_of(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Pr _Pred) noexcept /* terminates */ {
    // test if all elements in [_First, _Last) satisfy _Pred with the indicated execution policy
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt);
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        return _All_of_family_parallel<false>(_UFirst, _ULast, _Pass_fn(_Pred));
    } else {
        return _STD all_of(_UFirst, _ULast, _Pass_fn(_Pred));
    }
}

template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD bool any_of(_ExPo&&, const _FwdIt _First, const _FwdIt _Last, _Pr _Pred) noexcept /* terminates */ {
    // test if any element in [_First, _Last) satisfies _Pred with the indicated execution policy
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt);
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        return !_All_of_family_parallel<true>(_UFirst, _ULast, _Pass_fn(_Pred));
    } else {
        return _STD any_of(_UFirst, _ULast, _Pass_fn(_Pred));
    }
}

template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD bool none_of(_ExPo&&, const _FwdIt _First, const _FwdIt _Last, _Pr _Pred) noexcept /* terminates */ {
    // test if no element in [_First, _Last) satisfies _Pred with the indicated execution policy
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt);
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        return _All_of_family_parallel<true>(_UFirst, _ULast, _Pass_fn(_Pred));
    } else {
        return _STD none_of(_UFirst, _ULast, _Pass_fn(_Pred));
    }
}

template <class _FwdIt, class _Fn>
void _For_each_ivdep(_FwdIt _First, const _FwdIt _Last, _Fn _Func) {
    // perform function for each element [_First, _Last) assuming independent loop bodies
#pragma loop(ivdep)
    for (; _First != _Last; ++_First) {
        _Func(*_First);
    }
}

template <class _FwdIt, class _Diff, class _Fn>
struct _Static_partitioned_for_each2 { // for_each task scheduled on the system thread pool
    _Static_partition_team<_Diff> _Team;
    _Static_partition_range<_FwdIt, _Diff> _Basis;
    _Fn _Func;

    _Static_partitioned_for_each2(const size_t _Hw_threads, const _Diff _Count, _Fn _Fx)
        : _Team{_Count, _Get_chunked_work_chunk_count(_Hw_threads, _Count)}, _Basis{}, _Func(_Fx) {}

    _Cancellation_status _Process_chunk() {
        const auto _Key = _Team._Get_next_key();
        if (_Key) {
            const auto _Chunk = _Basis._Get_chunk(_Key);
            _For_each_ivdep(_Chunk._First, _Chunk._Last, _Func);
            return _Cancellation_status::_Running;
        }

        return _Cancellation_status::_Canceled;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_for_each2*>(_Context));
    }
};

template <class _ExPo, class _FwdIt, class _Fn, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
void for_each(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Fn _Func) noexcept /* terminates */ {
    // perform function for each element [_First, _Last) with the indicated execution policy
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt);
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) { // parallelize on multiprocessor machines...
            auto _Count = _STD distance(_UFirst, _ULast);
            if (_Count >= 2) { // ... with at least 2 elements
                _TRY_BEGIN
                auto _Passed_fn = _Pass_fn(_Func);
                _Static_partitioned_for_each2<decltype(_UFirst), decltype(_Count), decltype(_Passed_fn)> _Operation{
                    _Hw_threads, _Count, _Passed_fn};
                _Operation._Basis._Populate(_Operation._Team, _UFirst);
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                return;
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }
        }

        _For_each_ivdep(_UFirst, _ULast, _Pass_fn(_Func));
    } else if constexpr (remove_reference_t<_ExPo>::_Ivdep) {
        _For_each_ivdep(_UFirst, _ULast, _Pass_fn(_Func));
    } else {
        for (; _UFirst != _ULast; ++_UFirst) {
            _Func(*_UFirst);
        }
    }
}

template <class _FwdIt, class _Diff, class _Fn>
_FwdIt _For_each_n_ivdep(_FwdIt _First, _Diff _Count, _Fn _Func) {
    // perform function for each element [_First, _First + _Count) assuming independent loop bodies
#pragma loop(ivdep)
    for (; 0 < _Count; --_Count, (void) ++_First) {
        _Func(*_First);
    }

    return _First;
}

template <class _ExPo, class _FwdIt, class _Diff, class _Fn, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_FwdIt for_each_n(_ExPo&&, _FwdIt _First, const _Diff _Count_raw, _Fn _Func) noexcept /* terminates */ {
    // perform function for each element [_First, _First + _Count)
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt);
    _Algorithm_int_t<_Diff> _Count = _Count_raw;
    if (0 < _Count) {
        auto _UFirst = _Get_unwrapped_n(_First, _Count);
        if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
            const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
            if (_Hw_threads > 1 && _Count >= 2) { // parallelize on multiprocessor machines with at least 2 elements
                _TRY_BEGIN
                auto _Passed_fn = _Pass_fn(_Func);
                _Static_partitioned_for_each2<decltype(_UFirst), decltype(_Count), decltype(_Passed_fn)> _Operation{
                    _Hw_threads, _Count, _Passed_fn};
                _Seek_wrapped(_First, _Operation._Basis._Populate(_Operation._Team, _UFirst));
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                return _First;
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }

            _Seek_wrapped(_First, _For_each_n_ivdep(_UFirst, _Count, _Pass_fn(_Func)));
        } else if constexpr (remove_reference_t<_ExPo>::_Ivdep) {
            _Seek_wrapped(_First, _For_each_n_ivdep(_UFirst, _Count, _Pass_fn(_Func)));
        } else {
            for (; 0 < _Count; --_Count, (void) ++_UFirst) {
                _Func(*_UFirst);
            }

            _Seek_wrapped(_First, _UFirst);
        }
    }

    return _First;
}

template <class _FwdIt>
using _Parallel_find_results = conditional_t<_Use_atomic_iterator<_FwdIt>, _Parallel_choose_min_result<_FwdIt>,
    _Parallel_choose_min_chunk<_FwdIt>>;

template <class _FwdIt, class _Find_fx>
struct _Static_partitioned_find2 {
    _Static_partition_team<_Iter_diff_t<_FwdIt>> _Team;
    _Static_partition_range<_FwdIt> _Basis;
    _Parallel_find_results<_FwdIt> _Results;
    _Find_fx _Fx;

    _Static_partitioned_find2(
        const size_t _Hw_threads, const _Iter_diff_t<_FwdIt> _Count, const _FwdIt _Last, const _Find_fx _Fx_)
        : _Team{_Count, _Get_chunked_work_chunk_count(_Hw_threads, _Count)}, _Basis{}, _Results(_Last), _Fx(_Fx_) {}

    _Cancellation_status _Process_chunk() {
        if (_Results._Complete()) {
            return _Cancellation_status::_Canceled;
        }

        const auto _Key = _Team._Get_next_key();
        if (!_Key) {
            return _Cancellation_status::_Canceled;
        }
        // Once _Key is obtained, the amount of work should not be discarded (see GH-818).

        const auto _Range     = _Basis._Get_chunk(_Key);
        const auto _This_find = _Fx(_Range._First, _Range._Last);
        if (_This_find == _Range._Last) {
            return _Cancellation_status::_Running;
        }

        _Results._Imbue(_Key._Chunk_number, _This_find);
        return _Cancellation_status::_Canceled;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_find2*>(_Context));
    }
};

template <class _ExPo, class _FwdIt, class _Find_fx>
_FwdIt _Find_parallel_unchecked(_ExPo&&, const _FwdIt _First, const _FwdIt _Last, const _Find_fx _Fx) {
    // find first matching _Val, potentially in parallel
    if (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) {
            const auto _Count = _STD distance(_First, _Last);
            if (_Count >= 2) {
                _TRY_BEGIN
                _Static_partitioned_find2 _Operation{_Hw_threads, _Count, _Last, _Fx};
                _Operation._Basis._Populate(_Operation._Team, _First);
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                return _Operation._Results._Get_result();
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to _Fx below
                _CATCH_END
            }
        }
    }

    return _Fx(_First, _Last);
}

template <class _ExPo, class _FwdIt, class _Ty, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD _FwdIt find(_ExPo&& _Exec, _FwdIt _First, const _FwdIt _Last, const _Ty& _Val) noexcept /* terminates */ {
    // find first matching _Val
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt);
    using _UFwdIt = _Unwrapped_t<const _FwdIt&>;
    _Adl_verify_range(_First, _Last);
    _Seek_wrapped(_First,
        _Find_parallel_unchecked(_STD forward<_ExPo>(_Exec), _Get_unwrapped(_First), _Get_unwrapped(_Last),
            [&](const _UFwdIt _LFirst, const _UFwdIt _LLast) { return _Find_unchecked(_LFirst, _LLast, _Val); }));
    return _First;
}

template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD _FwdIt find_if(_ExPo&& _Exec, _FwdIt _First, const _FwdIt _Last, _Pr _Pred) noexcept /* terminates */ {
    // find first satisfying _Pred
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt);
    using _UFwdIt = _Unwrapped_t<const _FwdIt&>;
    _Adl_verify_range(_First, _Last);
    auto _Pass_pred = _Pass_fn(_Pred);
    _Seek_wrapped(_First,
        _Find_parallel_unchecked(_STD forward<_ExPo>(_Exec), _Get_unwrapped(_First), _Get_unwrapped(_Last),
            [=](const _UFwdIt _LFirst, const _UFwdIt _LLast) { return _STD find_if(_LFirst, _LLast, _Pass_pred); }));
    return _First;
}

template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD _FwdIt find_if_not(_ExPo&& _Exec, _FwdIt _First, const _FwdIt _Last, _Pr _Pred) noexcept /* terminates */ {
    // find first satisfying !_Pred
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt);
    using _UFwdIt = _Unwrapped_t<const _FwdIt&>;
    _Adl_verify_range(_First, _Last);
    auto _Pass_pred = _Pass_fn(_Pred);
    _Seek_wrapped(_First, _Find_parallel_unchecked(_STD forward<_ExPo>(_Exec), _Get_unwrapped(_First),
                              _Get_unwrapped(_Last), [=](const _UFwdIt _LFirst, const _UFwdIt _LLast) {
                                  return _STD find_if_not(_LFirst, _LLast, _Pass_pred);
                              }));
    return _First;
}

template <class _FwdIt1, class _FwdIt2>
_Iter_diff_t<_FwdIt1> _Get_find_end_forward_partition_size(
    _FwdIt1 _First1, const _FwdIt1 _Last1, _FwdIt2 _First2, const _FwdIt2 _Last2) {
    // get the count of the range of possible matches in a find_end operation for forward iterators
    if (_First2 == _Last2) {
        return 0;
    }

    for (;;) {
        if (_First1 == _Last1) { // haystack is shorter than needle
            return 0;
        }

        ++_First2;
        if (_First2 == _Last2) { // seek complete
            return _STD distance(_First1, _Last1);
        }

        ++_First1;
    }
}

template <class _FwdIt1, class _FwdIt2, class _Pr>
struct _Static_partitioned_find_end_forward {
    _Static_partition_team<_Iter_diff_t<_FwdIt1>> _Team;
    _Static_partition_range<_FwdIt1> _Basis;
    _Iterator_range<_FwdIt2> _Range2;
    _Pr _Pred;
    conditional_t<_Use_atomic_iterator<_FwdIt1>, _Parallel_choose_max_result<_FwdIt1>,
        _Parallel_choose_max_chunk<_FwdIt1>>
        _Results;

    _Static_partitioned_find_end_forward(const size_t _Hw_threads, const _Iter_diff_t<_FwdIt1> _Count,
        const _FwdIt1 _Last1, const _FwdIt2 _First2, const _FwdIt2 _Last2, const _Pr _Pred_)
        : _Team{_Count, _Get_chunked_work_chunk_count(_Hw_threads, _Count)}, _Basis{}, _Range2{_First2, _Last2},
          _Pred{_Pred_}, _Results(_Last1) {}

    _Cancellation_status _Process_chunk() {
        const auto _Key = _Team._Get_next_key();
        if (!_Key) {
            return _Cancellation_status::_Canceled;
        }

        const auto _Chunk_number = _Key._Chunk_number;
        const auto _Range        = _Basis._Get_chunk(_Key);
        _FwdIt1 _Result          = _Range._Last;
        for (auto _First1 = _Range._First; _First1 != _Range._Last; ++_First1) {
            if (_Equal_rev_pred_unchecked(_First1, _Range2._First, _Range2._Last, _Pred)) {
                _Result = _First1;
            }
        }

        if (_Result != _Range._Last) {
            _Results._Imbue(_Chunk_number, _Result);
        }

        return _Cancellation_status::_Running;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        (void) static_cast<_Static_partitioned_find_end_forward*>(_Context)->_Process_chunk();
    }
};

template <class _BidIt1, class _FwdIt2>
_BidIt1 _Get_find_end_backward_partition_start(
    const _BidIt1 _First1, _BidIt1 _Last1, _FwdIt2 _First2, const _FwdIt2 _Last2) {
    // gets the end of the range of possible matches for a find_end operation
    if constexpr (_Is_random_iter_v<_BidIt1> && _Is_random_iter_v<_FwdIt2>) {
        using _CT         = _Common_diff_t<_BidIt1, _FwdIt2>;
        const _CT _Count1 = _Last1 - _First1;
        const _CT _Count2 = _Last2 - _First2;
        _Iter_diff_t<_BidIt1> _Count{};
        if (0 < _Count2 && _Count2 <= _Count1) {
            _Count = static_cast<_Iter_diff_t<_BidIt1>>(_Count1 - _Count2 + 1);
        }
        return _First1 + _Count;
    } else {
        if (_First2 == _Last2) { // always 0 matches, give up
            return _First1;
        }

        for (;;) {
            if (_First1 == _Last1) { // haystack is shorter than needle, give up
                return _First1;
            }

            ++_First2;
            if (_First2 == _Last2) { // seek complete
                return _Last1;
            }

            --_Last1;
        }
    }
}

template <class _BidIt1, class _FwdIt2, class _Pr>
struct _Static_partitioned_find_end_backward2 {
    _Static_partition_team<_Iter_diff_t<_BidIt1>> _Team;
    _Static_partition_range_backward<_BidIt1> _Basis;
    conditional_t<_Use_atomic_iterator<_BidIt1>, _Parallel_choose_max_result<_BidIt1>,
        _Parallel_choose_min_chunk<_BidIt1>>
        _Results;
    _Iterator_range<_FwdIt2> _Range2;
    _Pr _Pred;

    _Static_partitioned_find_end_backward2(const size_t _Hw_threads, const _Iter_diff_t<_BidIt1> _Count,
        const _BidIt1 _Last1, const _FwdIt2 _First2, const _FwdIt2 _Last2, const _Pr _Pred_)
        : _Team{_Count, _Get_chunked_work_chunk_count(_Hw_threads, _Count)}, _Basis{},
          _Results(_Last1), _Range2{_First2, _Last2}, _Pred{_Pred_} {}

    _Cancellation_status _Process_chunk() {
        if (_Results._Complete()) {
            return _Cancellation_status::_Canceled;
        }

        const auto _Key = _Team._Get_next_key();
        if (!_Key) {
            return _Cancellation_status::_Canceled;
        }
        // Once _Key is obtained, the amount of work should not be discarded (see GH-818).

        const auto _Chunk_number = _Key._Chunk_number;
        const auto _Range        = _Basis._Get_chunk(_Key);
        auto _Last1              = _Range._Last;
        do {
            --_Last1;
            if (_Equal_rev_pred_unchecked(_Last1, _Range2._First, _Range2._Last, _Pred)) {
                _Results._Imbue(_Chunk_number, _Last1);
                return _Cancellation_status::_Canceled;
            }
        } while (_Last1 != _Range._First);
        return _Cancellation_status::_Running;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_find_end_backward2*>(_Context));
    }
};

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Pr, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD _FwdIt1 find_end(_ExPo&&, _FwdIt1 _First1, const _FwdIt1 _Last1, const _FwdIt2 _First2, const _FwdIt2 _Last2,
    _Pr _Pred) noexcept /* terminates */ {
    // find last [_First2, _Last2) satisfying _Pred
    _Adl_verify_range(_First1, _Last1);
    _Adl_verify_range(_First2, _Last2);
    const auto _UFirst1 = _Get_unwrapped(_First1);
    const auto _ULast1  = _Get_unwrapped(_Last1);
    const auto _UFirst2 = _Get_unwrapped(_First2);
    const auto _ULast2  = _Get_unwrapped(_Last2);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) {
            if constexpr (_Is_bidi_iter_v<_FwdIt1>) {
                const auto _Partition_start =
                    _Get_find_end_backward_partition_start(_UFirst1, _ULast1, _UFirst2, _ULast2);
                if (_UFirst1 == _Partition_start) {
                    _Seek_wrapped(_First1, _ULast1);
                    return _First1;
                }

                const auto _Count = _STD distance(_UFirst1, _Partition_start);
                if (_Count >= 2) {
                    _TRY_BEGIN
                    _Static_partitioned_find_end_backward2 _Operation{
                        _Hw_threads, _Count, _ULast1, _UFirst2, _ULast2, _Pass_fn(_Pred)};
                    _Operation._Basis._Populate(_Operation._Team, _Partition_start);
                    _Run_chunked_parallel_work(_Hw_threads, _Operation);
                    _Seek_wrapped(_First1, _Operation._Results._Get_result());
                    return _First1;
                    _CATCH(const _Parallelism_resources_exhausted&)
                    // fall through to serial case below
                    _CATCH_END
                }
            } else {
                const auto _Count = _Get_find_end_forward_partition_size(_UFirst1, _ULast1, _UFirst2, _ULast2);
                if (_Count >= 2) {
                    _TRY_BEGIN
                    _Static_partitioned_find_end_forward _Operation{
                        _Hw_threads, _Count, _ULast1, _UFirst2, _ULast2, _Pass_fn(_Pred)};
                    _Operation._Basis._Populate(_Operation._Team, _UFirst1);
                    _Run_chunked_parallel_work(_Hw_threads, _Operation);
                    _Seek_wrapped(_First1, _Operation._Results._Get_result());
                    return _First1;
                    _CATCH(const _Parallelism_resources_exhausted&)
                    // fall through to serial case below
                    _CATCH_END
                }
            }
        }
    }

    _Seek_wrapped(_First1, _STD find_end(_UFirst1, _ULast1, _UFirst2, _ULast2, _Pass_fn(_Pred)));
    return _First1;
}

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Pr, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD _FwdIt1 find_first_of(_ExPo&& _Exec, const _FwdIt1 _First1, _FwdIt1 _Last1, const _FwdIt2 _First2,
    const _FwdIt2 _Last2, _Pr _Pred) noexcept /* terminates */ {
    // look for one of [_First2, _Last2) that matches element
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    using _UFwdIt1 = _Unwrapped_t<const _FwdIt1&>;
    _Adl_verify_range(_First1, _Last1);
    _Adl_verify_range(_First2, _Last2);
    const auto _UFirst2 = _Get_unwrapped(_First2);
    const auto _ULast2  = _Get_unwrapped(_Last2);
    if (_UFirst2 == _ULast2) {
        return _Last1;
    }

    auto _Pass_pred = _Pass_fn(_Pred);
    _Seek_wrapped(_Last1, _Find_parallel_unchecked(_STD forward<_ExPo>(_Exec), _Get_unwrapped(_First1),
                              _Get_unwrapped(_Last1), [=](const _UFwdIt1 _LFirst1, const _UFwdIt1 _LLast1) {
                                  return _STD find_first_of(_LFirst1, _LLast1, _UFirst2, _ULast2, _Pass_pred);
                              }));
    return _Last1;
}

template <class _FwdIt, class _Pr>
struct _Static_partitioned_adjacent_find2 {
    _Static_partition_team<_Iter_diff_t<_FwdIt>> _Team;
    _Static_partition_range<_FwdIt> _Basis;
    _Parallel_find_results<_FwdIt> _Results;
    _Pr _Pred;

    _Static_partitioned_adjacent_find2(
        const size_t _Hw_threads, const _Iter_diff_t<_FwdIt> _Count, const _FwdIt _Last, const _Pr _Pred_)
        : _Team{_Count, _Get_chunked_work_chunk_count(_Hw_threads, _Count)}, _Basis{}, _Results{_Last}, _Pred{_Pred_} {}

    _Cancellation_status _Process_chunk() {
        if (_Results._Complete()) {
            return _Cancellation_status::_Canceled;
        }

        const auto _Key = _Team._Get_next_key();
        if (!_Key) {
            return _Cancellation_status::_Canceled;
        }
        // Once _Key is obtained, the amount of work should not be discarded (see GH-818).

        const auto _Chunk_number = _Key._Chunk_number;
        const auto _Range        = _Basis._Get_chunk(_Key);
        // tests [_First, _Last) for _Pred(*_Result, *next(_Result));
        // note: intentionally dereferences _Last
        auto _First = _Range._First;
        for (auto _Next = _First; _First != _Range._Last; _First = _Next) {
            ++_Next;
            if (_Pred(*_First, *_Next)) { // found match
                _Results._Imbue(_Chunk_number, _First);
                return _Cancellation_status::_Canceled;
            }
        }

        return _Cancellation_status::_Running;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_adjacent_find2*>(_Context));
    }
};

template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD _FwdIt adjacent_find(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Pr _Pred) noexcept /* terminates */ {
    // find first satisfying _Pred with successor
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) {
            const auto _Count = static_cast<_Iter_diff_t<_FwdIt>>(_STD distance(_UFirst, _ULast) - 1);
            if (_Count >= 2) {
                _TRY_BEGIN
                _Static_partitioned_adjacent_find2 _Operation{_Hw_threads, _Count, _ULast, _Pass_fn(_Pred)};
                _Operation._Basis._Populate(_Operation._Team, _UFirst);
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                _Seek_wrapped(_Last, _Operation._Results._Get_result());
                return _Last;
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to adjacent_find below
                _CATCH_END
            }
        }
    }

    _Seek_wrapped(_Last, _STD adjacent_find(_UFirst, _ULast, _Pass_fn(_Pred)));
    return _Last;
}

template <class _FwdIt, class _Pr>
struct _Static_partitioned_count_if2 {
    using _Diff = _Iter_diff_t<_FwdIt>;
    _Static_partition_team<_Diff> _Team;
    _Static_partition_range<_FwdIt> _Basis;
    _Pr _Pred;
    atomic<_Diff> _Results;

    _Static_partitioned_count_if2(const _Diff _Count, const size_t _Chunks, const _FwdIt _First, const _Pr _Pred_)
        : _Team{_Count, _Chunks}, _Basis{}, _Pred{_Pred_}, _Results{} {
        _Basis._Populate(_Team, _First);
    }

    _Diff _Process_chunks() {
        _Diff _Result{};
        while (const auto _Key = _Team._Get_next_key()) {
            const auto _Range = _Basis._Get_chunk(_Key);
            _Result += _STD count_if(_Range._First, _Range._Last, _Pred);
        }

        return _Result;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        const auto _This = static_cast<_Static_partitioned_count_if2*>(_Context);
        _This->_Results.fetch_add(_This->_Process_chunks(), memory_order_relaxed);
    }
};

template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD _Iter_diff_t<_FwdIt> count_if(_ExPo&&, const _FwdIt _First, const _FwdIt _Last, _Pr _Pred) noexcept
/* terminates */ {
    // count elements satisfying _Pred
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt);
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) {
            const auto _Count = _STD distance(_UFirst, _ULast);
            if (_Count >= 2) {
                const auto _Chunks = _Get_chunked_work_chunk_count(_Hw_threads, _Count);
                _TRY_BEGIN
                _Static_partitioned_count_if2 _Operation{_Count, _Chunks, _UFirst, _Pass_fn(_Pred)};
                _Iter_diff_t<_FwdIt> _Foreground_count;
                {
                    const _Work_ptr _Work{_Operation};
                    // setup complete, hereafter nothrow or terminate
                    _Work._Submit_for_chunks(_Hw_threads, _Chunks);
                    _Foreground_count = _Operation._Process_chunks();
                } // join with _Work_ptr threads

                return _Operation._Results.load(memory_order_relaxed) + _Foreground_count;
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to count_if below
                _CATCH_END
            }
        }
    }

    return _STD count_if(_UFirst, _ULast, _Pass_fn(_Pred));
}

template <class _ExPo, class _FwdIt, class _Ty, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD _Iter_diff_t<_FwdIt> count(_ExPo&& _Exec, const _FwdIt _First, const _FwdIt _Last, const _Ty& _Val) noexcept
/* terminates */ {
    // count elements that match _Val
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt);
    _Adl_verify_range(_First, _Last);
    return _STD count_if(_STD forward<_ExPo>(_Exec), _Get_unwrapped(_First), _Get_unwrapped(_Last),
        [&_Val](auto&& _Iter_val) { return _STD forward<decltype(_Iter_val)>(_Iter_val) == _Val; });
}

template <class _FwdIt1, class _FwdIt2,
    bool = _Use_atomic_iterator<_Unwrapped_t<const _FwdIt1&>>&& _Is_random_iter_v<_FwdIt2>,
    bool = _Use_atomic_iterator<_Unwrapped_t<const _FwdIt2&>>&& _Is_random_iter_v<_FwdIt1>>
struct _Static_partitioned_mismatch_results;

template <class _FwdIt1, class _FwdIt2, bool _Unused>
struct _Static_partitioned_mismatch_results<_FwdIt1, _FwdIt2, true, _Unused> {
    // atomically manipulate atomic<_FwdIt1> and calculate the second iterator by adding distance to it
    _Parallel_choose_min_result<_FwdIt1> _Storage;

    _Static_partitioned_mismatch_results(const _FwdIt1 _Last1, const _Unwrapped_t<const _FwdIt2&>&)
        : _Storage(_Last1) {}

    void _Imbue(const size_t _Chunk_number, const _FwdIt1 _First1, const _Unwrapped_t<const _FwdIt2&>&) {
        _Storage._Imbue(_Chunk_number, _First1);
    }

    pair<_FwdIt1, _FwdIt2> _Get_result(const _FwdIt1 _First1, const _FwdIt2 _First2) const {
        const _FwdIt1 _Result1 = _Storage._Get_result();
        return {_Result1, _First2 + static_cast<_Iter_diff_t<_FwdIt2>>(_Result1 - _First1)};
    }
};

template <class _FwdIt1, class _FwdIt2>
struct _Static_partitioned_mismatch_results<_FwdIt1, _FwdIt2, false, true> {
    // atomically manipulate atomic<_FwdIt2> and calculate the first iterator by adding distance to it
    _Parallel_choose_min_result<_FwdIt2> _Storage;

    _Static_partitioned_mismatch_results(const _Unwrapped_t<const _FwdIt1&>&, const _FwdIt2 _Last2)
        : _Storage(_Last2) {}

    void _Imbue(const size_t _Chunk_number, const _Unwrapped_t<const _FwdIt1&>&, const _FwdIt2 _First2) {
        _Storage._Imbue(_Chunk_number, _First2);
    }

    pair<_FwdIt1, _FwdIt2> _Get_result(const _FwdIt1 _First1, const _FwdIt2 _First2) const {
        const _FwdIt2 _Result2 = _Storage._Get_result();
        return {_First1 + static_cast<_Iter_diff_t<_FwdIt1>>(_Result2 - _First2), _Result2};
    }
};

template <class _FwdIt1, class _FwdIt2>
struct _Static_partitioned_mismatch_results<_FwdIt1, _FwdIt2, false, false> {
    // get both iterators by manipulating them under lock
    using _UFwdIt1 = _Unwrapped_t<const _FwdIt1&>;
    using _UFwdIt2 = _Unwrapped_t<const _FwdIt2&>;
    _Parallel_choose_min_chunk<pair<_UFwdIt1, _UFwdIt2>> _Storage;

    _Static_partitioned_mismatch_results(const _UFwdIt1 _Last1, const _UFwdIt2 _Last2) : _Storage({_Last1, _Last2}) {}

    void _Imbue(const size_t _Chunk_number, const _UFwdIt1 _First1, const _UFwdIt2 _First2) {
        _Storage._Imbue(_Chunk_number, {_First1, _First2});
    }

    pair<_FwdIt1, _FwdIt2> _Get_result(_FwdIt1 _First1, _FwdIt2 _First2) const {
        const auto _Result = _Storage._Get_result();
        _Seek_wrapped(_First2, _Result.second);
        _Seek_wrapped(_First1, _Result.first);
        return {_First1, _First2};
    }
};

template <class _FwdIt1, class _FwdIt2, class _Pr>
struct _Static_partitioned_mismatch2 {
    using _Diff = _Common_diff_t<_FwdIt1, _FwdIt2>;
    _Static_partition_team<_Diff> _Team;
    _Static_partition_range<_FwdIt1, _Diff> _Basis1;
    _Static_partition_range<_FwdIt2, _Diff> _Basis2;
    _Static_partitioned_mismatch_results<_FwdIt1, _FwdIt2> _Results;
    _Pr _Pred;

    _Static_partitioned_mismatch2(
        const size_t _Hw_threads, const _Diff _Count, const _FwdIt1 _First1, const _FwdIt2 _First2, const _Pr _Pred_)
        : _Team{_Count, _Get_chunked_work_chunk_count(_Hw_threads, _Count)}, _Basis1{}, _Basis2{},
          _Results(
              _Get_unwrapped(_Basis1._Populate(_Team, _First1)), _Get_unwrapped(_Basis2._Populate(_Team, _First2))),
          _Pred(_Pred_) {}

    _Cancellation_status _Process_chunk() {
        if (_Results._Storage._Complete()) {
            return _Cancellation_status::_Canceled;
        }

        const auto _Key = _Team._Get_next_key();
        if (!_Key) {
            return _Cancellation_status::_Canceled;
        }
        // Once _Key is obtained, the amount of work should not be discarded (see GH-818).

        const auto _Chunk_number = _Key._Chunk_number;
        const auto _Range1       = _Basis1._Get_chunk(_Key);
        auto _First1             = _Range1._First;
        auto _First2             = _Basis2._Get_chunk(_Key)._First;
        for (;;) {
            if (_First1 == _Range1._Last) {
                return _Cancellation_status::_Running;
            }

            if (!_Pred(*_First1, *_First2)) {
                _Results._Imbue(_Chunk_number, _First1, _First2);
                return _Cancellation_status::_Canceled;
            }

            ++_First1;
            ++_First2;
        }
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_mismatch2*>(_Context));
    }
};

#pragma warning(push)
#pragma warning(disable : 4868) // compiler may not enforce left-to-right evaluation order
                                // in braced initializer list (/Wall)
template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Pr, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD pair<_FwdIt1, _FwdIt2> mismatch(
    _ExPo&&, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _Pr _Pred) noexcept /* terminates */ {
    // return [_First1, _Last1)/[_First2, ...) mismatch
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    _Adl_verify_range(_First1, _Last1);
    const auto _UFirst1 = _Get_unwrapped(_First1);
    const auto _ULast1  = _Get_unwrapped(_Last1);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) {
            const auto _Count   = _STD distance(_UFirst1, _ULast1);
            const auto _UFirst2 = _Get_unwrapped_n(_First2, _Count);
            if (_Count >= 2) {
                _TRY_BEGIN
                _Static_partitioned_mismatch2 _Operation{_Hw_threads, _Count, _UFirst1, _UFirst2, _Pass_fn(_Pred)};
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                const auto _Result = _Operation._Results._Get_result(_UFirst1, _UFirst2);
                _Seek_wrapped(_First2, _Result.second);
                _Seek_wrapped(_First1, _Result.first);
                return {_First1, _First2};
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }

            const auto _Result = _STD mismatch(_UFirst1, _ULast1, _UFirst2, _Pass_fn(_Pred));
            _Seek_wrapped(_First2, _Result.second);
            _Seek_wrapped(_First1, _Result.first);
            return {_First1, _First2};
        }
    }

    const auto _Result = _STD mismatch(
        _UFirst1, _ULast1, _Get_unwrapped_n(_First2, _Idl_distance<_FwdIt1>(_UFirst1, _ULast1)), _Pass_fn(_Pred));
    _Seek_wrapped(_First2, _Result.second);
    _Seek_wrapped(_First1, _Result.first);
    return {_First1, _First2};
}
#pragma warning(pop)

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Pr, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD pair<_FwdIt1, _FwdIt2> mismatch(
    _ExPo&&, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2, _Pr _Pred) noexcept /* terminates */ {
    // return [_First1, _Last1)/[_First2, _Last2) mismatch
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    _Adl_verify_range(_First1, _Last1);
    _Adl_verify_range(_First2, _Last2);
    const auto _UFirst1 = _Get_unwrapped(_First1);
    const auto _ULast1  = _Get_unwrapped(_Last1);
    const auto _UFirst2 = _Get_unwrapped(_First2);
    const auto _ULast2  = _Get_unwrapped(_Last2);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) {
            const auto _Count = static_cast<_Iter_diff_t<_FwdIt1>>(_Distance_min(_UFirst1, _ULast1, _UFirst2, _ULast2));
            if (_Count >= 2) {
                _TRY_BEGIN
                _Static_partitioned_mismatch2 _Operation{_Hw_threads, _Count, _UFirst1, _UFirst2, _Pass_fn(_Pred)};
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                const auto _Result = _Operation._Results._Get_result(_UFirst1, _UFirst2);
                _Seek_wrapped(_First2, _Result.second);
                _Seek_wrapped(_First1, _Result.first);
                return {_First1, _First2};
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial below
                _CATCH_END
            }
        }
    }

    const auto _Result = _STD mismatch(_UFirst1, _ULast1, _UFirst2, _ULast2, _Pass_fn(_Pred));
    _Seek_wrapped(_First2, _Result.second);
    _Seek_wrapped(_First1, _Result.first);
    return {_First1, _First2};
}

template <class _FwdIt1, class _FwdIt2, class _Pr>
struct _Static_partitioned_equal2 {
    using _Diff = _Common_diff_t<_FwdIt1, _FwdIt2>;
    _Static_partition_team<_Diff> _Team;
    _Static_partition_range<_FwdIt1, _Diff> _Basis1;
    _Static_partition_range<_FwdIt2, _Diff> _Basis2;
    _Pr _Pred;
    _Cancellation_token _Cancel_token;

    _Static_partitioned_equal2(const size_t _Hw_threads, const _Diff _Count, _Pr _Pred_, const _FwdIt1&, const _FwdIt2&)
        : _Team{_Count, _Get_chunked_work_chunk_count(_Hw_threads, _Count)}, _Basis1{}, _Basis2{},
          _Pred(_Pred_), _Cancel_token{} {}

    _Cancellation_status _Process_chunk() {
        if (_Cancel_token._Is_canceled()) {
            return _Cancellation_status::_Canceled;
        }

        const auto _Key = _Team._Get_next_key();
        if (!_Key) {
            return _Cancellation_status::_Canceled;
        }
        // Once _Key is obtained, the amount of work should not be discarded (see GH-818).

        const auto _Range1       = _Basis1._Get_chunk(_Key);
        const auto _Range2_first = _Basis2._Get_chunk(_Key)._First;
        if (_STD equal(_Range1._First, _Range1._Last, _Range2_first, _Pred)) {
            return _Cancellation_status::_Running;
        }

        _Cancel_token._Cancel();
        return _Cancellation_status::_Canceled;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_equal2*>(_Context));
    }
};

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Pr, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD bool equal(_ExPo&&, const _FwdIt1 _First1, const _FwdIt1 _Last1, const _FwdIt2 _First2, _Pr _Pred) noexcept
/* terminates */ {
    // compare [_First1, _Last1) to [_First2, ...)
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    _Adl_verify_range(_First1, _Last1);
    const auto _UFirst1 = _Get_unwrapped(_First1);
    const auto _ULast1  = _Get_unwrapped(_Last1);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) {
            const auto _Count   = _STD distance(_UFirst1, _ULast1);
            const auto _UFirst2 = _Get_unwrapped_n(_First2, _Count);
            if (_Count >= 2) {
                _TRY_BEGIN
                _Static_partitioned_equal2 _Operation{_Hw_threads, _Count, _Pass_fn(_Pred), _UFirst1, _UFirst2};
                _Operation._Basis1._Populate(_Operation._Team, _UFirst1);
                _Operation._Basis2._Populate(_Operation._Team, _UFirst2);
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                return !_Operation._Cancel_token._Is_canceled_relaxed();
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }

            return _STD equal(_UFirst1, _ULast1, _UFirst2, _Pass_fn(_Pred));
        }
    }

    return _STD equal(_UFirst1, _ULast1, _First2, _Pass_fn(_Pred));
}

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Pr, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD bool equal(_ExPo&&, const _FwdIt1 _First1, const _FwdIt1 _Last1, const _FwdIt2 _First2, const _FwdIt2 _Last2,
    _Pr _Pred) noexcept /* terminates */ {
    // compare [_First1, _Last1) to [_First2, _Last2)
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    _Adl_verify_range(_First1, _Last1);
    _Adl_verify_range(_First2, _Last2);
    const auto _UFirst1 = _Get_unwrapped(_First1);
    const auto _ULast1  = _Get_unwrapped(_Last1);
    const auto _UFirst2 = _Get_unwrapped(_First2);
    const auto _ULast2  = _Get_unwrapped(_Last2);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) {
            const auto _Count = _Distance_any(_UFirst1, _ULast1, _UFirst2, _ULast2);
            if (_Count >= 2) {
                _TRY_BEGIN
                _Static_partitioned_equal2 _Operation{_Hw_threads, _Count, _Pass_fn(_Pred), _UFirst1, _UFirst2};
                if (!_Operation._Basis1._Populate(_Operation._Team, _UFirst1, _ULast1)) {
                    // left sequence didn't have length _Count
                    return false;
                }

                if (!_Operation._Basis2._Populate(_Operation._Team, _UFirst2, _ULast2)) {
                    // right sequence didn't have length _Count
                    return false;
                }

                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                return !_Operation._Cancel_token._Is_canceled_relaxed();
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to equal below
                _CATCH_END
            }
        }
    }

    return _STD equal(_UFirst1, _ULast1, _UFirst2, _ULast2, _Pass_fn(_Pred));
}

template <class _FwdItHaystack, class _FwdItPat, class _Pr>
struct _Static_partitioned_search2 {
    _Static_partition_team<_Iter_diff_t<_FwdItHaystack>> _Team;
    _Static_partition_range<_FwdItHaystack> _Basis;
    _Parallel_find_results<_FwdItHaystack> _Results;
    _FwdItPat _First2;
    _FwdItPat _Last2;
    _Pr _Pred;

    _Static_partitioned_search2(const size_t _Hw_threads, const _Iter_diff_t<_FwdItHaystack> _Count,
        const _FwdItHaystack _First1, const _FwdItHaystack _Last1, const _FwdItPat _First2_, const _FwdItPat _Last2_,
        _Pr _Pred_)
        : _Team{_Count, _Get_chunked_work_chunk_count(_Hw_threads, _Count)}, _Basis{}, _Results(_Last1),
          _First2(_First2_), _Last2(_Last2_), _Pred(_Pred_) {
        _Basis._Populate(_Team, _First1);
    }

    _Cancellation_status _Process_chunk() {
        if (_Results._Complete()) {
            return _Cancellation_status::_Canceled;
        }

        const auto _Key = _Team._Get_next_key();
        if (!_Key) {
            return _Cancellation_status::_Canceled;
        }
        // Once _Key is obtained, the amount of work should not be discarded (see GH-818).

        const auto _Range = _Basis._Get_chunk(_Key);
        for (auto _Candidate = _Range._First; _Candidate != _Range._Last; ++_Candidate) {
            if (_Equal_rev_pred_unchecked(_Candidate, _First2, _Last2, _Pred)) {
                _Results._Imbue(_Key._Chunk_number, _Candidate);
                return _Cancellation_status::_Canceled;
            }
        }

        return _Cancellation_status::_Running;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_search2*>(_Context));
    }
};

template <class _ExPo, class _FwdItHaystack, class _FwdItPat, class _Pr, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD _FwdItHaystack search(_ExPo&&, const _FwdItHaystack _First1, _FwdItHaystack _Last1, const _FwdItPat _First2,
    const _FwdItPat _Last2, _Pr _Pred) noexcept /* terminates */ {
    // find first [_First2, _Last2) match
    _Adl_verify_range(_First2, _Last2);
    const auto _UFirst2 = _Get_unwrapped(_First2);
    const auto _ULast2  = _Get_unwrapped(_Last2);
    if (_UFirst2 == _ULast2) {
        return _First1;
    }

    _Adl_verify_range(_First1, _Last1);
    const auto _UFirst1 = _Get_unwrapped(_First1);
    const auto _ULast1  = _Get_unwrapped(_Last1);

    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) {
            _Iter_diff_t<_FwdItHaystack> _Count;
            if constexpr (_Is_random_iter_v<_FwdItHaystack> && _Is_random_iter_v<_FwdItPat>) {
                const auto _HaystackDist = _ULast1 - _UFirst1;
                const auto _NeedleDist   = _ULast2 - _UFirst2;
                if (_NeedleDist > _HaystackDist) { // needle is longer than haystack, no match possible
                    return _Last1;
                }

                // +1 can't overflow because _NeedleDist > 0 because _First2 != _Last2
                _Count = static_cast<_Iter_diff_t<_FwdItHaystack>>(_HaystackDist - _NeedleDist + 1);
            } else {
                auto _UFirst1c = _UFirst1;
                auto _UFirst2c = _UFirst2;
                for (;;) {
                    if (_UFirst1c == _ULast1) {
                        // either haystack was shorter than needle so no match is possible, or haystack was empty so
                        // returning _Last1 is the same as returning _First1
                        return _Last1;
                    }

                    ++_UFirst2c;
                    if (_UFirst2c == _ULast2) {
                        _Count = _STD distance(_UFirst1c, _ULast1);
                        break;
                    }

                    ++_UFirst1c;
                }
            }

            if (_Count == 1) {
                // 1 match possible == ranges are of equal length -- do better than serial search because we've done the
                // work to calculate distance() of the inputs
                if (_Equal_rev_pred_unchecked(_UFirst1, _UFirst2, _ULast2, _Pass_fn(_Pred))) {
                    return _First1;
                }

                return _Last1;
            }

            _TRY_BEGIN
            _Static_partitioned_search2 _Operation{
                _Hw_threads, _Count, _UFirst1, _ULast1, _UFirst2, _ULast2, _Pass_fn(_Pred)};
            _Run_chunked_parallel_work(_Hw_threads, _Operation);
            _Seek_wrapped(_Last1, _Operation._Results._Get_result());
            return _Last1;
            _CATCH(const _Parallelism_resources_exhausted&)
            // fall through to search, below
            _CATCH_END
        }
    }

    _Seek_wrapped(_Last1, _STD search(_UFirst1, _ULast1, _UFirst2, _ULast2, _Pass_fn(_Pred)));
    return _Last1;
}

template <class _FwdIt, class _Ty, class _Pr>
struct _Static_partitioned_search_n2 {
    _Static_partition_team<_Iter_diff_t<_FwdIt>> _Team;
    _Static_partition_range<_FwdIt> _Basis;
    _Parallel_find_results<_FwdIt> _Results;
    _Iter_diff_t<_FwdIt> _Target_count;
    const _Ty& _Val;
    _Pr _Pred;

    _Static_partitioned_search_n2(const size_t _Hw_threads, const _Iter_diff_t<_FwdIt> _Candidates, const _FwdIt _First,
        const _FwdIt _Last, const _Iter_diff_t<_FwdIt> _Target_count_, const _Ty& _Val_, _Pr _Pred_)
        : _Team{_Candidates, _Get_chunked_work_chunk_count(_Hw_threads, _Candidates)}, _Basis{}, _Results(_Last),
          _Target_count(_Target_count_), _Val(_Val_), _Pred(_Pred_) {
        _Basis._Populate(_Team, _First);
    }

    _Cancellation_status _Process_chunk() {
        if (_Results._Complete()) {
            return _Cancellation_status::_Canceled;
        }

        const auto _Key = _Team._Get_next_key();
        if (!_Key) {
            return _Cancellation_status::_Canceled;
        }
        // Once _Key is obtained, the amount of work should not be discarded (see GH-818).

        const auto _Range = _Basis._Get_chunk(_Key);

        // any match in this chunk will have at least 1 element in _Range, so the furthest off the
        // end of the candidate range we will need to search is _Target_count - 1
        const _Iter_diff_t<_FwdIt> _Overshoot = _Target_count - 1;
        const auto _Serial_last               = _STD next(_Range._Last, _Overshoot);

        // we delegate to the serial algorithm targeting [_First, _Last + _Overshoot)
        // which we know is safe because we only partitioned _First + _Candidates start positions
        // if we have a match, it will be within [_First, _Last); otherwise the serial algorithm
        // will tell us _Last + _Overshoot
        const auto _Candidate = _STD search_n(_Range._First, _Serial_last, _Target_count, _Val, _Pred);
        if (_Candidate == _Serial_last) {
            return _Cancellation_status::_Running;
        }

        _Results._Imbue(_Key._Chunk_number, _Candidate);
        return _Cancellation_status::_Canceled;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_search_n2*>(_Context));
    }
};

template <class _ExPo, class _FwdIt, class _Diff, class _Ty, class _Pr, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD _FwdIt search_n(_ExPo&&, const _FwdIt _First, _FwdIt _Last, const _Diff _Count_raw, const _Ty& _Val,
    _Pr _Pred) noexcept /* terminates */ {
    // find first _Count * _Val satisfying _Pred
    const _Algorithm_int_t<_Diff> _Count = _Count_raw;
    if (_Count <= 0) {
        _Last = _First;
        return _Last;
    }

    if (static_cast<uintmax_t>(_Count) > static_cast<uintmax_t>((numeric_limits<_Iter_diff_t<_FwdIt>>::max)())) {
        // if the number of _Vals searched for is larger than the longest possible sequence, we can't find it
        return _Last;
    }

    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) {
            const auto _Haystack_count = _STD distance(_UFirst, _ULast);
            if (_Count > _Haystack_count) {
                return _Last;
            }

            // +1 can't overflow because _Count > 0
            const auto _Candidates = static_cast<_Iter_diff_t<_FwdIt>>(_Haystack_count - _Count + 1);
            _TRY_BEGIN
            _Static_partitioned_search_n2 _Operation{_Hw_threads, _Candidates, _UFirst, _ULast,
                static_cast<_Iter_diff_t<_FwdIt>>(_Count), _Val, _Pass_fn(_Pred)};
            _Run_chunked_parallel_work(_Hw_threads, _Operation);
            _Seek_wrapped(_Last, _Operation._Results._Get_result());
            return _Last;
            _CATCH(const _Parallelism_resources_exhausted&)
            // fall through to search_n, below
            _CATCH_END
        }
    }

    _Seek_wrapped(_Last, _STD search_n(_UFirst, _ULast, _Count, _Val, _Pass_fn(_Pred)));
    return _Last;
}

template <class _FwdIt1, class _FwdIt2, class _Fn>
struct _Static_partitioned_unary_transform2 {
    using _Diff = _Common_diff_t<_FwdIt1, _FwdIt2>;
    _Static_partition_team<_Diff> _Team;
    _Static_partition_range<_FwdIt1, _Diff> _Source_basis;
    _Static_partition_range<_FwdIt2, _Diff> _Dest_basis;
    _Fn _Func;

    _Static_partitioned_unary_transform2(
        const size_t _Hw_threads, const _Diff _Count, const _FwdIt1 _First, _Fn _Fx, const _FwdIt2&)
        : _Team{_Count, _Get_chunked_work_chunk_count(_Hw_threads, _Count)}, _Source_basis{}, _Dest_basis{},
          _Func(_Fx) {
        _Source_basis._Populate(_Team, _First);
    }

    _Cancellation_status _Process_chunk() {
        const auto _Key = _Team._Get_next_key();
        if (_Key) {
            const auto _Source = _Source_basis._Get_chunk(_Key);
            _STD transform(_Source._First, _Source._Last, _Dest_basis._Get_chunk(_Key)._First, _Func);
            return _Cancellation_status::_Running;
        }

        return _Cancellation_status::_Canceled;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_unary_transform2*>(_Context));
    }
};

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Fn, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_FwdIt2 transform(_ExPo&&, const _FwdIt1 _First, const _FwdIt1 _Last, _FwdIt2 _Dest, _Fn _Func) noexcept
/* terminates */ {
    // transform [_First, _Last) with _Func
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) { // parallelize on multiprocessor machines...
            const auto _Count = _STD distance(_UFirst, _ULast);
            const auto _UDest = _Get_unwrapped_n(_Dest, _Count);
            if (_Count >= 2) { // ... with at least 2 elements
                _TRY_BEGIN
                _Static_partitioned_unary_transform2 _Operation{_Hw_threads, _Count, _UFirst, _Pass_fn(_Func), _UDest};
                _Seek_wrapped(_Dest, _Operation._Dest_basis._Populate(_Operation._Team, _UDest));
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                return _Dest;
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }

            _Seek_wrapped(_Dest, _STD transform(_UFirst, _ULast, _UDest, _Pass_fn(_Func)));
            return _Dest;
        } else {
            _Seek_wrapped(
                _Dest, _STD transform(_UFirst, _ULast, _Get_unwrapped_n(_Dest, _Idl_distance<_FwdIt1>(_UFirst, _ULast)),
                           _Pass_fn(_Func)));
            return _Dest;
        }
    } else {
        _Seek_wrapped(_Dest, _STD transform(_UFirst, _ULast,
                                 _Get_unwrapped_n(_Dest, _Idl_distance<_FwdIt1>(_UFirst, _ULast)), _Pass_fn(_Func)));
        return _Dest;
    }
}

template <class _FwdIt1, class _FwdIt2, class _FwdIt3, class _Fn>
struct _Static_partitioned_binary_transform2 {
    using _Diff = _Common_diff_t<_FwdIt1, _FwdIt2, _FwdIt3>;
    _Static_partition_team<_Diff> _Team;
    _Static_partition_range<_FwdIt1, _Diff> _Source1_basis;
    _Static_partition_range<_FwdIt2, _Diff> _Source2_basis;
    _Static_partition_range<_FwdIt3, _Diff> _Dest_basis;
    _Fn _Func;

    _Static_partitioned_binary_transform2(
        const size_t _Hw_threads, const _Diff _Count, _FwdIt1 _First1, _FwdIt2 _First2, _Fn _Fx, const _FwdIt3&)
        : _Team{_Count, _Get_chunked_work_chunk_count(_Hw_threads, _Count)}, _Source1_basis{}, _Source2_basis{},
          _Dest_basis{}, _Func(_Fx) {
        _Source1_basis._Populate(_Team, _First1);
        _Source2_basis._Populate(_Team, _First2);
    }

    _Cancellation_status _Process_chunk() {
        const auto _Key = _Team._Get_next_key();
        if (_Key) {
            const auto _Source1 = _Source1_basis._Get_chunk(_Key);
            _STD transform(_Source1._First, _Source1._Last, _Source2_basis._Get_chunk(_Key)._First,
                _Dest_basis._Get_chunk(_Key)._First, _Func);
            return _Cancellation_status::_Running;
        }

        return _Cancellation_status::_Canceled;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_binary_transform2*>(_Context));
    }
};

#pragma warning(push)
#pragma warning(disable : 4868) // compiler may not enforce left-to-right evaluation order
                                // in braced initializer list (/Wall)
template <class _ExPo, class _FwdIt1, class _FwdIt2, class _FwdIt3, class _Fn,
    _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_FwdIt3 transform(_ExPo&&, const _FwdIt1 _First1, const _FwdIt1 _Last1, const _FwdIt2 _First2, _FwdIt3 _Dest,
    _Fn _Func) noexcept /* terminates */ {
    // transform [_First1, _Last1) and [_First2, ...) with _Func
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt3);
    _Adl_verify_range(_First1, _Last1);
    const auto _UFirst1 = _Get_unwrapped(_First1);
    const auto _ULast1  = _Get_unwrapped(_Last1);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) { // parallelize on multiprocessor machines...
            const auto _Count   = _STD distance(_UFirst1, _ULast1);
            const auto _UFirst2 = _Get_unwrapped_n(_First2, _Count);
            const auto _UDest   = _Get_unwrapped_n(_Dest, _Count);
            if (_Count >= 2) { // ... with at least 2 elements
                _TRY_BEGIN
                _Static_partitioned_binary_transform2 _Operation{
                    _Hw_threads, _Count, _UFirst1, _UFirst2, _Pass_fn(_Func), _UDest};
                _Seek_wrapped(_Dest, _Operation._Dest_basis._Populate(_Operation._Team, _UDest));
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                return _Dest;
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }

            _Seek_wrapped(_Dest, _STD transform(_UFirst1, _ULast1, _UFirst2, _UDest, _Pass_fn(_Func)));
            return _Dest;
        } else {
            const auto _Count = _Idl_distance<_FwdIt1>(_UFirst1, _ULast1);
            _Seek_wrapped(_Dest, _STD transform(_UFirst1, _ULast1, _Get_unwrapped_n(_First2, _Count),
                                     _Get_unwrapped_n(_Dest, _Count), _Pass_fn(_Func)));
            return _Dest;
        }
    } else {
        const auto _Count = _Idl_distance<_FwdIt1>(_UFirst1, _ULast1);
        _Seek_wrapped(_Dest, _STD transform(_UFirst1, _ULast1, _Get_unwrapped_n(_First2, _Count),
                                 _Get_unwrapped_n(_Dest, _Count), _Pass_fn(_Func)));
        return _Dest;
    }
}
#pragma warning(pop)

template <class _ExPo, class _FwdIt, class _Ty, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
void replace(_ExPo&& _Exec, const _FwdIt _First, const _FwdIt _Last, const _Ty& _Oldval, const _Ty& _Newval) noexcept
/* terminates */ {
    // replace each matching _Oldval with _Newval
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt);
    _STD for_each(_STD forward<_ExPo>(_Exec), _First, _Last, [&](auto&& _Value) {
        if (_STD forward<decltype(_Value)>(_Value) == _Oldval) {
            _STD forward<decltype(_Value)>(_Value) = _Newval;
        }
    });
}

template <class _ExPo, class _FwdIt, class _Pr, class _Ty, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
void replace_if(_ExPo&& _Exec, const _FwdIt _First, const _FwdIt _Last, _Pr _Pred, const _Ty& _Val) noexcept
/* terminates */ {
    // replace each satisfying _Pred with _Val
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt);
    _STD for_each(
        _STD forward<_ExPo>(_Exec), _First, _Last, [&_Val, _Lambda_pred = _Pass_fn(_Pred)](auto&& _Value) mutable {
            if (_Lambda_pred(_STD forward<decltype(_Value)>(_Value))) {
                _STD forward<decltype(_Value)>(_Value) = _Val;
            }
        });
}

template <class _FwdIt, class _Pr>
_FwdIt _Remove_move_if_unchecked(_FwdIt _First, const _FwdIt _Last, _FwdIt _Dest, _Pr _Pred) {
    // move omitting each element satisfying _Pred
    for (; _First != _Last; ++_First) {
        if (!_Pred(*_First)) {
            *_Dest = _STD move(*_First);
            ++_Dest;
        }
    }

    return _Dest;
}

template <class _FwdIt, class _Pr>
struct _Static_partitioned_remove_if2 {
    enum class _Chunk_state : unsigned char {
        _Serial, // while a chunk is in the serial state, it is touched only by an owner thread
        _Merging, // while a chunk is in the merging state, threads all try to CAS the chunk _Merging -> _Moving
                  // the thread that succeeds takes responsibility for moving the keepers from that chunk to the
                  // results
        _Moving, // while a chunk is in the moving state, the keepers are being moved to _Results
                 // only one chunk at a time is ever _Moving; this also serves to synchronize access to _Results
        _Done // when a chunk becomes _Done, it is complete / will never need to touch _Results again

        // as an optimization, if a thread sees that it has no predecessor (or its predecessor is _Done), it
        // may transition from _Serial directly to _Done, doing the moving step implicitly.
    };

#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier
    struct alignas(hardware_destructive_interference_size) alignas(_FwdIt) _Chunk_local_data {
        atomic<_Chunk_state> _State;
        _FwdIt _New_end;
    };
#pragma warning(pop)

    _Static_partition_team<_Iter_diff_t<_FwdIt>> _Team;
    _Static_partition_range<_FwdIt> _Basis;
    _Pr _Pred;
    _Parallel_vector<_Chunk_local_data> _Chunk_locals;
    _FwdIt _Results;

    _Static_partitioned_remove_if2(
        const size_t _Hw_threads, const _Iter_diff_t<_FwdIt> _Count, const _FwdIt _First, const _Pr _Pred_)
        : _Team{_Count, _Get_chunked_work_chunk_count(_Hw_threads, _Count)}, _Basis{}, _Pred{_Pred_},
          _Chunk_locals(_Team._Chunks), _Results{_First} {
        _Basis._Populate(_Team, _First);
    }

    _Cancellation_status _Process_chunk() {
        const auto _Key = _Team._Get_next_key();
        if (!_Key) {
            return _Cancellation_status::_Canceled;
        }

        // remove phase:
        auto _Merge_index = _Key._Chunk_number; // merge step will start from this index
        {
            auto& _Chunk_data = _Chunk_locals[_Merge_index];
            const auto _Range = _Basis._Get_chunk(_Key);
            if (_Merge_index == 0 || _Chunk_locals[_Merge_index - 1]._State.load() == _Chunk_state::_Done) {
                // no predecessor, so run serial algorithm directly into results
                if (_Merge_index == 0 || _Results == _Range._First) {
                    _Results = _STD remove_if(_Range._First, _Range._Last, _Pred);
                } else {
                    _Results = _Remove_move_if_unchecked(_Range._First, _Range._Last, _Results, _Pred);
                }

                _Chunk_data._State.store(_Chunk_state::_Done);
                ++_Merge_index; // this chunk is already merged
            } else { // predecessor, run serial algorithm in place and attempt to merge later
                _Chunk_data._New_end = _STD remove_if(_Range._First, _Range._Last, _Pred);
                _Chunk_data._State.store(_Chunk_state::_Merging);
                if (_Chunk_locals[_Merge_index - 1]._State.load() != _Chunk_state::_Done) {
                    // if the predecessor isn't done, whichever thread merges our predecessor will merge us too
                    return _Cancellation_status::_Running;
                }
            }
        }

        // merge phase: at this point, we have observed that our predecessor chunk has been merged to the output,
        // attempt to become the new merging thread if the previous merger gave up
        // note: it is an invariant when we get here that _Chunk_locals[_Merge_index - 1]._State == _Chunk_state::_Done
        for (; _Merge_index != _Team._Chunks; ++_Merge_index) {
            auto& _Merge_chunk_data = _Chunk_locals[_Merge_index];
            auto _Expected          = _Chunk_state::_Merging;
            if (!_Merge_chunk_data._State.compare_exchange_strong(_Expected, _Chunk_state::_Moving)) {
                // either the _Merge_index chunk isn't ready to merge yet, or another thread will do it
                return _Cancellation_status::_Running;
            }

            const auto _Merge_first   = _Basis._Get_first(_Merge_index, _Team._Get_chunk_offset(_Merge_index));
            const auto _Merge_new_end = _STD exchange(_Merge_chunk_data._New_end, {});
            if (_Results == _Merge_first) { // entire range up to now had no removals, don't bother moving
                _Results = _Merge_new_end;
            } else {
                _Results = _Move_unchecked(_Merge_first, _Merge_new_end, _Results);
            }

            _Merge_chunk_data._State.store(_Chunk_state::_Done);
        }

        return _Cancellation_status::_Canceled;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_remove_if2*>(_Context));
    }
};

template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD _FwdIt remove_if(_ExPo&&, _FwdIt _First, const _FwdIt _Last, _Pr _Pred) noexcept /* terminates */ {
    // remove each satisfying _Pred
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) {
            const auto _Count = _STD distance(_UFirst, _ULast);
            if (_Count >= 2) {
                _TRY_BEGIN
                _Static_partitioned_remove_if2 _Operation{_Hw_threads, _Count, _UFirst, _Pass_fn(_Pred)};
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                _Seek_wrapped(_First, _Operation._Results);
                return _First;
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }
        }
    }

    _Seek_wrapped(_First, _STD remove_if(_UFirst, _ULast, _Pass_fn(_Pred)));
    return _First;
}

template <class _ExPo, class _FwdIt, class _Ty, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD _FwdIt remove(_ExPo&& _Exec, const _FwdIt _First, const _FwdIt _Last, const _Ty& _Val) noexcept
/* terminates */ {
    // remove each matching _Val
    return _STD remove_if(_STD forward<_ExPo>(_Exec), _First, _Last,
        [&_Val](auto&& _Lhs) { return _STD forward<decltype(_Lhs)>(_Lhs) == _Val; });
}

template <class _Diff>
struct _Sort_work_item_impl { // data describing an individual sort work item
    using difference_type = _Diff;

    _Diff _Offset;
    _Diff _Size;
    _Diff _Ideal;
};

template <class _RanIt>
using _Sort_work_item = _Sort_work_item_impl<_Iter_diff_t<_RanIt>>;

template <class _RanIt, class _Pr>
bool _Process_sort_work_item(const _RanIt _Basis, _Pr _Pred, _Sort_work_item<_RanIt>& _Wi,
    _Sort_work_item<_RanIt>& _Right_fork_wi, _Iter_diff_t<_RanIt>& _Work_complete) noexcept /* terminates */ {
    // processes the sort work item, _Wi, relative to _Basis
    // if the sort is divided into quicksort sub-problems:
    //   the return value is true
    //   _Wi contains the left sub-problem; the caller should continue with this
    //   _Right_fork_wi contains the right sub-problem; the caller should allow this to be stolen
    // otherwise:
    //   the return value is false
    //   _Wi's range is completely sorted
    //   _Right_fork_wi is unmodified
    const auto _Size  = _Wi._Size;
    const auto _First = _Basis + _Wi._Offset;
    const auto _Last  = _First + _Size;
    const auto _Ideal = _Wi._Ideal;
    if (_Size <= _Isort_max<_RanIt>) {
        _Insertion_sort_unchecked(_First, _Last, _Pred);
        _Work_complete += _Size;
        return false;
    }

    if (0 < _Ideal) { // divide and conquer by partitioning (quicksort)
        const auto _Mid = _Partition_by_median_guess_unchecked(_First, _Last, _Pred);
        const auto _New_ideal =
            static_cast<_Iter_diff_t<_RanIt>>(_Ideal / 2 + _Ideal / 4); // allow 1.5 log2(N) divisions
        _Wi._Size      = _Mid.first - _First;
        _Wi._Ideal     = _New_ideal;
        _Right_fork_wi = {_Mid.second - _Basis, _Last - _Mid.second, _New_ideal};
        _Work_complete += _Mid.second - _Mid.first;
        return true;
    }

    // too many divisions; heap sort
    _Make_heap_unchecked(_First, _Last, _Pred);
    _Sort_heap_unchecked(_First, _Last, _Pred);
    _Work_complete += _Size;
    return false;
}

template <class _RanIt, class _Pr>
void _Process_sort_queue(const _RanIt _Basis, _Pr _Pred, _Work_stealing_membership<_Sort_work_item<_RanIt>>& _My_ticket,
    _Sort_work_item<_RanIt>& _Wi) noexcept /* terminates */ {
    _Sort_work_item<_RanIt> _Right_fork_wi;
    do { // process work items in the local queue
        while (_Process_sort_work_item(_Basis, _Pred, _Wi, _Right_fork_wi, _My_ticket._Work_complete)) {
            _TRY_BEGIN
            _My_ticket._Push_bottom(_Right_fork_wi);
            _CATCH(const _Parallelism_resources_exhausted&)
            // local queue is full and memory can't be acquired, process _Right_fork_wi serially
            const auto _First = _Basis + _Right_fork_wi._Offset;
            _Sort_unchecked(_First, _First + _Right_fork_wi._Size, _Right_fork_wi._Ideal, _Pred);
            _My_ticket._Work_complete += _Right_fork_wi._Size;
            _CATCH_END
        }
    } while (_My_ticket._Try_pop_bottom(_Wi));
}

template <class _RanIt, class _Pr>
struct _Sort_operation { // context for background threads
    _RanIt _Basis;
    _Pr _Pred;
    _Work_stealing_team<_Sort_work_item<_RanIt>> _Team;

    _Sort_operation(_RanIt _First, _Pr _Pred_arg, size_t _Threads, _Iter_diff_t<_RanIt> _Count)
        : _Basis(_First), _Pred(_Pred_arg), _Team(_Threads, _Count) {}

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, const __std_PTP_WORK _Work) noexcept /* terminates */ {
        auto* const _This = static_cast<_Sort_operation*>(_Context);
        const auto _Basis = _This->_Basis;
        const auto _Pred  = _This->_Pred;
        auto& _Team       = _This->_Team;
        auto _My_ticket   = _Team._Join_team();
        _Sort_work_item<_RanIt> _Wi;
        for (;;) {
            switch (_My_ticket._Steal(_Wi)) {
            case _Steal_result::_Success:
                _Process_sort_queue(_Basis, _Pred, _My_ticket, _Wi);
                break;
            case _Steal_result::_Abort:
                _My_ticket._Leave();
                __std_submit_threadpool_work(_Work);
                return;
            case _Steal_result::_Done:
                return;
            }
        }
    }
};

template <class _ExPo, class _RanIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
void sort(_ExPo&&, const _RanIt _First, const _RanIt _Last, _Pr _Pred) noexcept /* terminates */ {
    // order [_First, _Last)
    _Adl_verify_range(_First, _Last);
    const auto _UFirst                = _Get_unwrapped(_First);
    const auto _ULast                 = _Get_unwrapped(_Last);
    const _Iter_diff_t<_RanIt> _Ideal = _ULast - _UFirst;
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        size_t _Threads;
        if (_Ideal > _ISORT_MAX && (_Threads = __std_parallel_algorithms_hw_threads()) > 1) {
            // parallelize when input is large enough and we aren't on a uniprocessor machine
            _TRY_BEGIN
            _Sort_operation _Operation(_UFirst, _Pass_fn(_Pred), _Threads, _Ideal); // throws
            const _Work_ptr _Work{_Operation}; // throws
            auto& _Team     = _Operation._Team;
            auto _My_ticket = _Team._Join_team();
            _Work._Submit(_Threads - 1);
            _Sort_work_item<_RanIt> _Wi{0, _Ideal, _Ideal};
            _Steal_result _Sr;
            do {
                _Process_sort_queue(_UFirst, _Pass_fn(_Pred), _My_ticket, _Wi);

                do {
                    _Sr = _My_ticket._Steal(_Wi);
                } while (_Sr == _Steal_result::_Abort);
            } while (_Sr != _Steal_result::_Done);
            return;
            _CATCH(const _Parallelism_resources_exhausted&)
            // fall through to _Sort_unchecked, below
            _CATCH_END
        }
    }

    _Sort_unchecked(_UFirst, _ULast, _Ideal, _Pass_fn(_Pred));
}

template <class _Ty>
struct _Static_partitioned_temporary_buffer2 {
    _Optimistic_temporary_buffer<_Ty>& _Temp_buf;
    ptrdiff_t _Chunk_size;
    ptrdiff_t _Unchunked_items;

    template <class _Diff>
    explicit _Static_partitioned_temporary_buffer2(
        _Optimistic_temporary_buffer<_Ty>& _Temp_buf_raw, _Static_partition_team<_Diff>& _Team)
        : _Temp_buf(_Temp_buf_raw), _Chunk_size(static_cast<ptrdiff_t>(_Temp_buf._Capacity / _Team._Chunks)),
          _Unchunked_items(static_cast<ptrdiff_t>(_Temp_buf._Capacity % _Team._Chunks)) {}

    pair<_Ty*, ptrdiff_t> _Get_temp_buffer_for_chunk(const size_t _Chunk_number) {
        // get a view of the region of the temporary buffer allocated to chunk _Chunk_number
        return _Get_temp_buffer_for_chunk_range(_Chunk_number, _Chunk_number + 1);
    }

    pair<_Ty*, ptrdiff_t> _Get_temp_buffer_for_chunk_range(const size_t _Base, const size_t _End) {
        // get a view of the region of the temporary buffer allocated to the region of chunks [_Base, _End)
        const auto _Offset = _Get_offset(_Base);
        return {_Temp_buf._Data + _Offset, _Offset - _Get_offset(_End)};
    }

    _Ty* _Get_first(const size_t _Chunk_number) {
        // get a pointer to the first element of the temporary buffer allocated to chunk _Chunk_number
        return _Temp_buf._Data + _Get_offset(_Chunk_number);
    }

    ptrdiff_t _Get_offset(const size_t _Chunk_number) {
        // get the offset of the first element of the temporary buffer allocated to chunk _Chunk_number
        auto _Diff_chunk = static_cast<ptrdiff_t>(_Chunk_number);
        return _Diff_chunk * _Chunk_size + (_STD min)(_Diff_chunk, _Unchunked_items);
    }

    void _Destroy_all() { // destroy each element of the temporary buffer
        _Destroy_range(_Temp_buf._Data, _Temp_buf._Data + _Temp_buf._Capacity);
    }
};

inline size_t _Get_stable_sort_tree_height(const size_t _Count, const size_t _Hw_threads) {
    // Get height of merge tree for parallel stable_sort, a bottom-up merge sort.
    // * each merge takes two chunks from a buffer and copies to the other buffer in sorted order
    // * we want the overall result to end up in the input buffer and not into _Temp_buf; each merge
    //   "level" switches between the input buffer and the temporary buffer; as a result we want
    //   the number of merge "levels" to be even (and thus chunks must be 2 raised to an even power)
    // * the smallest chunk must be at least of size _ISORT_MAX
    // * we want a number of chunks as close to _Ideal_chunks as we can to minimize scheduling
    //   overhead, but can use more chunks than that
    const auto _Count_max_chunks       = _Count / _ISORT_MAX;
    const size_t _Log_count_max_chunks = _Floor_of_log_2(_Count_max_chunks);

    // if _Log_count_max_chunks is odd, that would break our 2 to even power invariant, so
    // go to the next smaller power of 2
    const auto _Count_max_tree_height = _Log_count_max_chunks & ~static_cast<size_t>(1);

    const auto _Ideal_chunks       = _Hw_threads * _Oversubscription_multiplier;
    const size_t _Log_ideal_chunks = _Floor_of_log_2(_Ideal_chunks);
#ifdef _WIN64
    const size_t _Max_tree_height = 62; // to avoid ptrdiff_t overflow
#else // ^^^ _WIN64 ^^^ // vvv !_WIN64 vvv
    const size_t _Max_tree_height = 30;
#endif // _WIN64
    const size_t _Clamped_ideal_chunks = (_STD min)(_Max_tree_height, _Log_ideal_chunks);

    // similarly, if _Clamped_ideal_chunks is odd, that would break our 2 to even power invariant,
    // so go to the next higher power of 2
    const auto _Ideal_tree_height = _Clamped_ideal_chunks + (_Clamped_ideal_chunks & 0x1U);

    return (_STD min)(_Count_max_tree_height, _Ideal_tree_height);
}

struct _Bottom_up_merge_tree {
    // merge tree / cells:
    // each level of the tree has the next 1 bit turned on:
    // 0 == done
    //         1               1
    //         2       3       1x
    //         4   5   6   7   1xx
    // chunks: 0 1 2 3 4 5 6 7
    // the starting cell for a given chunk is (_Chunk_number >> 1) + (1 << (height - 1))
    // (divide the chunk number by 2, and add the starting index of the bottom row of the tree)
    // a cell's parent is just left shifting the current cell by 1
    size_t _Height;
    // each cell's data is stored at _Buckets[_Cell - 1], since the 0th cell is unused / indicates termination
    _Parallel_vector<atomic<bool>> _Buckets;

    explicit _Bottom_up_merge_tree(const size_t _Height_)
        : _Height(_Height_), _Buckets((static_cast<size_t>(1) << _Height_) - 1) {}
};

struct _Bottom_up_tree_visitor {
    size_t _Cell;
    size_t _Base;
    size_t _Shift;

    _Bottom_up_tree_visitor(const size_t _Tree_height, const size_t _Initial_chunk)
        : _Cell((_Initial_chunk >> 1) + (static_cast<size_t>(1) << (_Tree_height - 1))),
          _Base(_Initial_chunk & ~static_cast<size_t>(1)), _Shift(1) {}

    bool _Try_give_up_merge_to_peer(_Bottom_up_merge_tree& _Merge_tree) const {
        // Attempt to mark that this child is done, to let our peer do the merge.
        // Returns whether we successfully gave responsibility for doing the current merge to our peer.
        return !_Merge_tree._Buckets[_Cell - 1].exchange(true);
    }

    bool _Go_to_parent() {
        // Attempt to go to the parent in the merge tree; returns whether the move to the parent was successful.
        _Shift <<= 1;
        _Base -= _Shift * (_Cell & static_cast<size_t>(1));
        _Cell >>= 1;
        return _Cell != 0;
    }
};

template <class _BidIt, class _Pr>
struct _Static_partitioned_stable_sort3 {
    using _Diff = _Iter_diff_t<_BidIt>;
    _Static_partition_team<_Diff> _Team;
    _Static_partition_range<_BidIt> _Basis;
    _Bottom_up_merge_tree _Merge_tree;
    _Static_partitioned_temporary_buffer2<_Iter_value_t<_BidIt>> _Temp_buf;
    _Pr _Pred;

    _Static_partitioned_stable_sort3(_Optimistic_temporary_buffer<_Iter_value_t<_BidIt>>& _Temp_buf_raw,
        const _Diff _Count, const size_t _Merge_tree_height_, const _BidIt _First, _Pr _Pred_)
        : _Team(_Count, static_cast<size_t>(1) << _Merge_tree_height_), _Basis{}, _Merge_tree(_Merge_tree_height_),
          _Temp_buf(_Temp_buf_raw, _Team), _Pred{_Pred_} {
        _Basis._Populate(_Team, _First);
    }

    _Cancellation_status _Process_chunk() {
        const auto _Key = _Team._Get_next_key();
        if (!_Key) {
            return _Cancellation_status::_Canceled;
        }

        {
            const auto _Serial_chunk = _Basis._Get_chunk(_Key);
            const auto _Temp_chunk   = _Temp_buf._Get_temp_buffer_for_chunk(_Key._Chunk_number);
            _Stable_sort_unchecked(
                _Serial_chunk._First, _Serial_chunk._Last, _Key._Size, _Temp_chunk.first, _Temp_chunk.second, _Pred);
        }

        _Bottom_up_tree_visitor _Visitor{_Merge_tree._Height, _Key._Chunk_number};
        if (_Visitor._Try_give_up_merge_to_peer(_Merge_tree)) { // peer will do the work to merge
            return _Cancellation_status::_Running;
        }

        if (_Temp_buf._Temp_buf._Capacity >= _Team._Count) { // enough space, combine using merge
            { // first merge is special; constructs the elements in the temporary buffer space
                const size_t _Base = _Visitor._Base;
                _Uninitialized_merge_move(_Basis._Get_first(_Base, _Team._Get_chunk_offset(_Base)),
                    _Basis._Get_first(_Base + 1, _Team._Get_chunk_offset(_Base + 1)),
                    _Basis._Get_first(_Base + 2, _Team._Get_chunk_offset(_Base + 2)), _Temp_buf._Get_first(_Base),
                    _Pred);
            }

            for (;;) { // walk remaining merge tree
                (void) _Visitor._Go_to_parent(); // can't be complete because the data is in the temporary buffer
                if (_Visitor._Try_give_up_merge_to_peer(_Merge_tree)) { // the other child will do the merge
                    return _Cancellation_status::_Running;
                }

                { // merge two sub-ranges from temporary buffer to input
                    const size_t _Base = _Visitor._Base;
                    const size_t _Mid  = _Base + _Visitor._Shift;
                    const size_t _End  = _Mid + _Visitor._Shift;
                    _Merge_move(_Temp_buf._Get_first(_Base), _Temp_buf._Get_first(_Mid), _Temp_buf._Get_first(_End),
                        _Basis._Get_first(_Base, _Team._Get_chunk_offset(_Base)), _Pred);
                }

                if (!_Visitor._Go_to_parent()) {
                    // temporary bits have been copied back to the input, no parent, so we're done
                    _Temp_buf._Destroy_all();
                    return _Cancellation_status::_Canceled;
                }

                if (_Visitor._Try_give_up_merge_to_peer(_Merge_tree)) { // the other child will do the merge
                    return _Cancellation_status::_Running;
                }

                { // merge two sub-ranges from input to temporary buffer
                    const size_t _Base = _Visitor._Base;
                    const size_t _Mid  = _Base + _Visitor._Shift;
                    const size_t _End  = _Mid + _Visitor._Shift;
                    _Merge_move(_Basis._Get_first(_Base, _Team._Get_chunk_offset(_Base)),
                        _Basis._Get_first(_Mid, _Team._Get_chunk_offset(_Mid)),
                        _Basis._Get_first(_End, _Team._Get_chunk_offset(_End)), _Temp_buf._Get_first(_Base), _Pred);
                }
            }
        } else { // not enough space, combine using inplace_merge
            for (;;) {
                const size_t _Base      = _Visitor._Base;
                const size_t _Mid       = _Base + _Visitor._Shift;
                const size_t _End       = _Mid + _Visitor._Shift;
                const auto _Base_offset = _Team._Get_chunk_offset(_Base);
                const auto _Mid_offset  = _Team._Get_chunk_offset(_Mid);
                const auto _End_offset  = _Team._Get_chunk_offset(_End);
                const auto _Temp_range  = _Temp_buf._Get_temp_buffer_for_chunk_range(_Base, _End);
                _Buffered_inplace_merge_unchecked(_Basis._Get_first(_Base, _Base_offset),
                    _Basis._Get_first(_Mid, _Mid_offset), _Basis._Get_first(_End, _End_offset),
                    _Mid_offset - _Base_offset, _End_offset - _Mid_offset, _Temp_range.first, _Temp_range.second,
                    _Pred);

                if (!_Visitor._Go_to_parent()) { // no parent, so we're done
                    return _Cancellation_status::_Canceled;
                }

                if (_Visitor._Try_give_up_merge_to_peer(_Merge_tree)) { // the other child will do the merge
                    return _Cancellation_status::_Running;
                }
            }
        }
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_stable_sort3*>(_Context));
    }
};

template <class _ExPo, class _BidIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
void stable_sort(_ExPo&&, const _BidIt _First, const _BidIt _Last, _Pr _Pred) noexcept /* terminates */ {
    // sort preserving order of equivalents
    _Adl_verify_range(_First, _Last);
    const auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast  = _Get_unwrapped(_Last);
    const auto _Count  = _STD distance(_UFirst, _ULast);
    if (_Count <= _ISORT_MAX) {
        _Insertion_sort_unchecked(_UFirst, _ULast, _Pass_fn(_Pred));
        return;
    }

    size_t _Hw_threads;
    bool _Attempt_parallelism;
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        _Hw_threads          = __std_parallel_algorithms_hw_threads();
        _Attempt_parallelism = _Hw_threads > 1;
    } else {
        _Attempt_parallelism = false;
    }

    _Optimistic_temporary_buffer<_Iter_value_t<_BidIt>> _Temp_buf{_Attempt_parallelism ? _Count : _Count - _Count / 2};
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        if (_Attempt_parallelism) {
            // forward+ iterator overflow assumption for size_t cast
            const auto _Tree_height = _Get_stable_sort_tree_height(static_cast<size_t>(_Count), _Hw_threads);
            if (_Tree_height != 0) {
                _TRY_BEGIN
                _Static_partitioned_stable_sort3 _Operation{_Temp_buf, _Count, _Tree_height, _UFirst, _Pass_fn(_Pred)};
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                return;
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }
        }
    }

    _Stable_sort_unchecked(_UFirst, _ULast, _Count, _Temp_buf._Data, _Temp_buf._Capacity, _Pass_fn(_Pred));
}

template <class _FwdIt, class _Pr>
struct _Static_partitioned_is_sorted_until {
    _Static_partition_team<_Iter_diff_t<_FwdIt>> _Team;
    // note offset partitioning:
    _Static_partition_range<_FwdIt> _Basis; // contains partition of [_First, _Last - 1)
    _Pr _Pred;
    _Parallel_find_results<_FwdIt> _Results;

    _Static_partitioned_is_sorted_until(
        _FwdIt _First, _FwdIt _Last, const size_t _Hw_threads, const _Iter_diff_t<_FwdIt> _Count, _Pr _Pred_)
        : _Team{_Count, _Get_chunked_work_chunk_count(_Hw_threads, _Count)}, _Basis{}, _Pred(_Pred_), _Results(_Last) {
        _Basis._Populate(_Team, _First);
    }

    _Cancellation_status _Process_chunk() {
        if (_Results._Complete()) {
            return _Cancellation_status::_Canceled;
        }

        const auto _Key = _Team._Get_next_key();
        if (!_Key) {
            return _Cancellation_status::_Canceled;
        }
        // Once _Key is obtained, the amount of work should not be discarded (see GH-818).

        auto _Range = _Basis._Get_chunk(_Key);
        auto _Next  = _Range._First;
        do {
            ++_Next;
            if (_DEBUG_LT_PRED(_Pred, *_Next, *_Range._First)) {
                _Results._Imbue(_Key._Chunk_number, _Next);
                return _Cancellation_status::_Canceled;
            }

            _Range._First = _Next;
        } while (_Range._First != _Range._Last);

        return _Cancellation_status::_Running;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_is_sorted_until*>(_Context));
    }
};

template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD _FwdIt is_sorted_until(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Pr _Pred) noexcept /* terminates */ {
    // find extent of range that is ordered by predicate
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt);
    _Adl_verify_range(_First, _Last);
    const auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast  = _Get_unwrapped(_Last);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) { // parallelize on multiprocessor machines
            auto _Count = _STD distance(_UFirst, _ULast);
            if (_Count >= 3) { // ... with at least 3 elements
                _TRY_BEGIN
                --_Count; // note unusual offset partitioning
                _Static_partitioned_is_sorted_until _Operation{_UFirst, _ULast, _Hw_threads, _Count, _Pass_fn(_Pred)};
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                _Seek_wrapped(_First, _Operation._Results._Get_result());
                return _First;
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }
        }
    }

    _Seek_wrapped(_First, _STD is_sorted_until(_UFirst, _ULast, _Pass_fn(_Pred)));
    return _First;
}

inline constexpr unsigned char _Contains_counterexample = 0;
inline constexpr unsigned char _Contains_true           = 1;
inline constexpr unsigned char _Contains_false          = 2;

template <class _FwdIt, class _Pr>
unsigned char _Chunk_is_partitioned_status(_FwdIt _First, _FwdIt _Last, _Pr _Pred) {
    // Returns the is_partitioned status for this range.
    // pre: Range is non-empty
    unsigned char _Result = 0;
    if (_Pred(*_First)) { // starts with T
        _Result = _Contains_true;
        for (;;) { // skip T partition
            ++_First;
            if (_First == _Last) { // all T
                return _Contains_true;
            }

            if (!_Pred(*_First)) { // range contains a switching point from T to F
                break;
            }
        }
    }

    _Result |= _Contains_false;
    while (++_First != _Last) { // verify F partition
        if (_Pred(*_First)) { // found an out of place element
            return _Contains_counterexample;
        }
    }

    return _Result;
}

template <class _FwdIt, class _Pr>
struct _Static_partitioned_is_partitioned {
    _Static_partition_team<_Iter_diff_t<_FwdIt>> _Team;
    _Static_partition_range<_FwdIt> _Basis;
    atomic<size_t> _Rightmost_true; // chunk number of the rightmost chunk found so far containing a T element
    atomic<size_t> _Leftmost_false; // chunk number of the leftmost chunk found so far containing an F element
    _Pr _Pred;

    _Static_partitioned_is_partitioned(
        const size_t _Hw_threads, const _Iter_diff_t<_FwdIt> _Count, const _FwdIt _First, _Pr _Pred_)
        : _Team(_Count, _Get_chunked_work_chunk_count(_Hw_threads, _Count)), _Basis{}, _Rightmost_true(0),
          _Leftmost_false(SIZE_MAX - 1), _Pred(_Pred_) {
        _Basis._Populate(_Team, _First);
    }

    _Cancellation_status _Process_chunk() {
        // Note that the cancellation status here is not used in the final returned answer of is_partitioned. Returning
        // _Cancellation_status::_Canceled is simply used as an "early fail" mechanism to avoid doing unnecessary work.
        // A final comparison of _Rightmost_true and _Leftmost_false is used to determine the final return value from
        // a call to is_partitioned.
        if (_Rightmost_true.load(memory_order_relaxed) > _Leftmost_false.load(memory_order_relaxed)) {
            // we've found a T to the right of an F, so we know the range cannot be partitioned and we can stop
            return _Cancellation_status::_Canceled;
        }

        const auto _Key = _Team._Get_next_key();
        if (!_Key) {
            return _Cancellation_status::_Canceled;
        }
        // Once _Key is obtained, the amount of work should not be discarded (see GH-818).

        // looking at chunks from either end, moving in towards the middle
        auto _Target_chunk_number = _Key._Chunk_number >> 1;
        if (!(_Key._Chunk_number & static_cast<size_t>(1))) {
            _Target_chunk_number = _Team._Chunks - _Target_chunk_number - 1;
        }

        const auto _Target_chunk_key  = _Team._Get_chunk_key(_Target_chunk_number);
        const auto _Chunk             = _Basis._Get_chunk(_Target_chunk_key);
        const auto _This_chunk_status = _Chunk_is_partitioned_status(_Chunk._First, _Chunk._Last, _Pred);

        if (_This_chunk_status == _Contains_counterexample) {
            // indicating that this chunk contains a counterexample, so the range is not partitioned
            _Rightmost_true.store(SIZE_MAX);
            return _Cancellation_status::_Canceled;
        }

        // after determining the is_partitioned status for this chunk,
        // we need to update the chunk numbers for leftmost F and rightmost T
        auto _Old_true = _Rightmost_true.load();
        if (_This_chunk_status & _Contains_true) {
            while (_Target_chunk_number > _Old_true) {
                if (_Rightmost_true.compare_exchange_weak(_Old_true, _Target_chunk_number)) {
                    _Old_true = _Target_chunk_number;
                    break;
                }
            }
        }

        // try to bail before doing more work if possible
        auto _Old_false = _Leftmost_false.load();
        if (_Old_true > _Old_false) {
            return _Cancellation_status::_Canceled;
        }

        if (_This_chunk_status & _Contains_false) {
            while (_Target_chunk_number < _Old_false) {
                if (_Leftmost_false.compare_exchange_weak(_Old_false, _Target_chunk_number)) {
                    _Old_false = _Target_chunk_number;
                    break;
                }
            }
        }

        // Other loads/stores may have been reordered around the loads of _Old_false and _Old_true, but this check may
        // allow us to avoid more atomic loads.
        // If such loads/stores have been reordered around the loads of _Old_false and _Old_true, then the next call to
        // _Process_chunk will exit given the check on the _Rightmost_true and _Leftmost_false values at the top of the
        // method.
        if (_Old_true > _Old_false) {
            return _Cancellation_status::_Canceled;
        }

        return _Cancellation_status::_Running;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_is_partitioned*>(_Context));
    }
};

template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD bool is_partitioned(_ExPo&&, const _FwdIt _First, const _FwdIt _Last, _Pr _Pred) noexcept /* terminates */ {
    // test if [_First, _Last) is partitioned by _Pred
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt);
    _Adl_verify_range(_First, _Last);
    const auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast  = _Get_unwrapped(_Last);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) { // parallelize on multiprocessor machines
            const auto _Count = _STD distance(_UFirst, _ULast);
            if (_Count >= 2) { // ... with at least 2 elements
                _TRY_BEGIN
                _Static_partitioned_is_partitioned _Operation{_Hw_threads, _Count, _UFirst, _Pass_fn(_Pred)};
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                return _Operation._Rightmost_true.load(memory_order_relaxed)
                    <= _Operation._Leftmost_false.load(memory_order_relaxed);
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }
        }
    }

    return _STD is_partitioned(_UFirst, _ULast, _Pass_fn(_Pred));
}

template <class _RanIt, class _Pr>
struct _Static_partitioned_is_heap_until {
    using _Diff = _Iter_diff_t<_RanIt>;
    _Static_partition_team<_Diff> _Team;
    _RanIt _Range_first;
    _Pr _Pred;
    _Parallel_find_results<_RanIt> _Results;

    _Static_partitioned_is_heap_until(
        _RanIt _First, _RanIt _Last, const size_t _Hw_threads, const _Diff _Count, _Pr _Pred_)
        : _Team{_Count, _Get_chunked_work_chunk_count(_Hw_threads, _Count)}, _Range_first(_First), _Pred(_Pred_),
          _Results(_Last) {}

    _Cancellation_status _Process_chunk() {
        if (_Results._Complete()) {
            return _Cancellation_status::_Canceled;
        }

        const auto _Key = _Team._Get_next_key();
        if (!_Key) {
            return _Cancellation_status::_Canceled;
        }
        // Once _Key is obtained, the amount of work should not be discarded (see GH-818).

        const auto _Chunk_range_size = _Key._Size;
        const auto _Chunk_offset     = _Key._Start_at;
        const auto _Last             = _Chunk_offset + _Chunk_range_size;

        const auto _Initial = (_STD max)(_Chunk_offset, _Diff{1});
        for (_Diff _Off = _Initial; _Off < _Last; ++_Off) {
            if (_DEBUG_LT_PRED(_Pred, *(_Range_first + ((_Off - 1) >> 1)), *(_Range_first + _Off))) {
                _Results._Imbue(_Key._Chunk_number, _Range_first + _Off);
                return _Cancellation_status::_Canceled;
            }
        }

        return _Cancellation_status::_Running;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_is_heap_until*>(_Context));
    }
};

template <class _ExPo, class _RanIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD _RanIt is_heap_until(_ExPo&&, _RanIt _First, _RanIt _Last, _Pr _Pred) noexcept /* terminates */ {
    // find extent of range that is a heap
    _REQUIRE_PARALLEL_ITERATOR(_RanIt);
    _Adl_verify_range(_First, _Last);
    const auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast  = _Get_unwrapped(_Last);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) { // parallelize on multiprocessor machines
            const auto _Count = _ULast - _UFirst;
            if (_Count >= 3) { // ... with at least 3 elements
                _TRY_BEGIN
                _Static_partitioned_is_heap_until _Operation{_UFirst, _ULast, _Hw_threads, _Count, _Pass_fn(_Pred)};
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                _Seek_wrapped(_First, _Operation._Results._Get_result());
                return _First;
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }
        }
    }

    _Seek_wrapped(_First, _STD is_heap_until(_UFirst, _ULast, _Pass_fn(_Pred)));
    return _First;
}

template <class _FwdIt, class _Pr>
pair<_FwdIt, _Iter_diff_t<_FwdIt>> _Partition_with_count_unchecked(_FwdIt _First, _FwdIt _Last, _Pr _Pred) {
    // move elements satisfying _Pred to front and track how many elements satisfy _Pred
    if constexpr (_Is_random_iter_v<_FwdIt>) {
        auto _Mid = _STD partition(_First, _Last, _Pred);
        return {_Mid, _Mid - _First};
    } else if constexpr (_Is_bidi_iter_v<_FwdIt>) {
        _Iter_diff_t<_FwdIt> _Trues{};
        for (;;) { // find any out-of-order pair
            for (;;) { // skip in-place elements at beginning
                if (_First == _Last) {
                    return {_First, _Trues};
                }

                if (!_Pred(*_First)) {
                    break;
                }

                ++_First;
                ++_Trues;
            }

            do { // skip in-place elements at end
                --_Last;
                if (_First == _Last) {
                    return {_First, _Trues};
                }
            } while (!_Pred(*_Last));

            _STD iter_swap(_First, _Last); // out of place, swap and loop
            ++_First;
            ++_Trues;
        }
    } else {
        _Iter_diff_t<_FwdIt> _Trues{};
        for (;;) { // skip in-place elements at beginning
            if (_First == _Last) {
                return {_First, _Trues};
            }

            if (!_Pred(*_First)) {
                break;
            }

            ++_First;
            ++_Trues;
        }

        for (_FwdIt _Next = _First; ++_Next != _Last;) {
            if (_Pred(*_Next)) {
                _STD iter_swap(_First, _Next); // out of place, swap and loop
                ++_First;
                ++_Trues;
            }
        }

        return {_First, _Trues};
    }
}

template <class _FwdIt, class _Pr>
pair<_FwdIt, _Iter_diff_t<_FwdIt>> _Partition_swap_backward(
    _FwdIt _First, _FwdIt _Last, _FwdIt _Beginning_of_falses, _Pr _Pred) {
    // Swap elements in [_First, _Last) satisfying _Pred with elements from _Beginning_of_falses.
    // Pre: _Beginning_of_falses < _First
    _Iter_diff_t<_FwdIt> _Trues{};
    if constexpr (_Is_bidi_iter_v<_FwdIt>) {
        while (_First != _Last) {
            --_Last;
            if (_Pred(*_Last)) {
                _STD iter_swap(_Beginning_of_falses, _Last);
                ++_Beginning_of_falses;
                ++_Trues;
                if (_Beginning_of_falses == _First) {
                    auto _Remain = _Partition_with_count_unchecked(_First, _Last, _Pred);
                    return {_Remain.first, static_cast<_Iter_diff_t<_FwdIt>>(_Remain.second + _Trues)};
                }
            }
        }
    } else {
        for (; _First != _Last; ++_First) {
            if (_Pred(*_First)) {
                _STD iter_swap(_First, _Beginning_of_falses);
                ++_Beginning_of_falses;
                ++_Trues;
            }
        }
    }

    return {_Beginning_of_falses, _Trues};
}

template <class _FwdIt>
_FwdIt _Partition_merge(const _FwdIt _False_first, const _FwdIt _True_first, const _FwdIt _True_last,
    const _Iter_diff_t<_FwdIt> _Count1, const _Iter_diff_t<_FwdIt> _Count2) {
    // Merge partition ranges where [_False_first, _True_first) are falses, [_True_first, _True_last) are trues
    // pre: _Count1 == distance(_False_first, _True_first) && _Count2 == distance(_True_first, _True_last)
    if (_Count1 < _Count2) { // move the false range to the end of the true range
        const _Iter_diff_t<_FwdIt> _Offset = _Count2 - _Count1;
        auto _Result                       = _True_first;
        if constexpr (_Is_random_iter_v<_FwdIt>) {
            _Result += _Offset;
        } else if constexpr (_Is_bidi_iter_v<_FwdIt>) {
            if (_Count1 < _Offset) {
                _Result = _True_last;
                _STD advance(_Result, -_Count1);
            } else {
                _STD advance(_Result, _Offset);
            }
        } else {
            _STD advance(_Result, _Offset);
        }

        _Swap_ranges_unchecked(_False_first, _True_first, _Result);
        return _Result;
    }

    // move the true range to the beginning of the false range
    return _Swap_ranges_unchecked(_True_first, _True_last, _False_first);
}

template <class _FwdIt, class _Pr>
struct _Static_partitioned_partition2 {
    using _Diff = _Iter_diff_t<_FwdIt>;

    enum class _Chunk_state : unsigned char {
        _Serial, // while a chunk is in the serial state, it is touched only by an owner thread
        _Merging, // while a chunk is in the merging state, threads all try to CAS the chunk _Merging -> _Swapping
                  // the thread that succeeds takes responsibility for swapping the trues from that chunk to the
                  // results
        _Swapping, // while a chunk is in the swapping state, the trues are being merged with _Results
                   // only one chunk at a time is ever _Swapping; this also serves to synchronize access to
                   // _Results and _Results_falses
        _Done // when a chunk becomes _Done, it is complete / will never need to touch _Results again
    };

#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier
    struct alignas(hardware_destructive_interference_size) alignas(_FwdIt) _Chunk_local_data {
        atomic<_Chunk_state> _State;
        _FwdIt _Beginning_of_falses;
        _Diff _Chunk_trues;
    };
#pragma warning(pop)

    _Static_partition_team<_Diff> _Team;
    _Static_partition_range<_FwdIt> _Basis;
    _Pr _Pred;
    _Parallel_vector<_Chunk_local_data> _Chunk_locals;
    _FwdIt _Results;
    _Diff _Results_falses;

    _Static_partitioned_partition2(const size_t _Hw_threads, const _Diff _Count, const _FwdIt _First, const _Pr _Pred_)
        : _Team{_Count, _Get_chunked_work_chunk_count(_Hw_threads, _Count)}, _Basis{}, _Pred{_Pred_},
          _Chunk_locals(_Team._Chunks), _Results{_First}, _Results_falses{} {
        _Basis._Populate(_Team, _First);
    }

    _Cancellation_status _Process_chunk() {
        const auto _Key = _Team._Get_next_key();
        if (!_Key) {
            return _Cancellation_status::_Canceled;
        }

        // serial-on-each-chunk phase:
        auto _Merge_index = _Key._Chunk_number; // merge step will start from this index
        {
            auto& _Chunk_data = _Chunk_locals[_Merge_index];
            const auto _Range = _Basis._Get_chunk(_Key);
            if (_Merge_index == 0 || _Chunk_locals[_Merge_index - 1]._State.load() == _Chunk_state::_Done) {
                // no predecessor, so run serial algorithm directly into results
                const auto _Chunk_results = _Merge_index == 0 || _Results == _Range._First
                                              ? _Partition_with_count_unchecked(_Range._First, _Range._Last, _Pred)
                                              : _Partition_swap_backward(_Range._First, _Range._Last, _Results, _Pred);
                _Results                  = _Chunk_results.first;
                _Chunk_data._Chunk_trues  = _Chunk_results.second;
                _Results_falses += _Key._Size - _Chunk_results.second;
                _Chunk_data._State.store(_Chunk_state::_Done);
                ++_Merge_index; // this chunk is already merged
            } else {
                // predecessor, run serial algorithm in place and attempt to merge later
                auto _Chunk_results              = _Partition_with_count_unchecked(_Range._First, _Range._Last, _Pred);
                _Chunk_data._Beginning_of_falses = _Chunk_results.first;
                _Chunk_data._Chunk_trues         = _Chunk_results.second;
                _Chunk_data._State.store(_Chunk_state::_Merging);
                if (_Chunk_locals[_Merge_index - 1]._State.load() != _Chunk_state::_Done) {
                    // if the predecessor isn't done, whichever thread merges our predecessor will merge us too
                    return _Cancellation_status::_Running;
                }
            }
        }

        // merge phase: at this point, we have observed that our predecessor chunk has been merged to the output,
        // attempt to become the new merging thread if the previous merger gave up
        // note: it is an invariant when we get here that _Chunk_locals[_Merge_index - 1]._State == _Chunk_state::_Done
        for (; _Merge_index != _Team._Chunks; ++_Merge_index) {
            auto& _Merge_chunk_data = _Chunk_locals[_Merge_index];
            auto _Expected          = _Chunk_state::_Merging;
            if (!_Merge_chunk_data._State.compare_exchange_strong(_Expected, _Chunk_state::_Swapping)) {
                // either the _Merge_index chunk isn't ready to merge yet, or another thread will do it
                return _Cancellation_status::_Running;
            }

            const auto _Merge_key   = _Team._Get_chunk_key(_Merge_index);
            const auto _Chunk_trues = _Merge_chunk_data._Chunk_trues;
            _Results                = _Partition_merge(_Results, _Basis._Get_first(_Merge_index, _Merge_key._Start_at),
                _STD exchange(_Merge_chunk_data._Beginning_of_falses, {}), _Results_falses, _Chunk_trues);
            _Results_falses += _Merge_key._Size - _Chunk_trues;
            _Merge_chunk_data._State.store(_Chunk_state::_Done);
        }

        return _Cancellation_status::_Canceled;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_partition2*>(_Context));
    }
};

template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_FwdIt partition(_ExPo&&, _FwdIt _First, const _FwdIt _Last, _Pr _Pred) noexcept /* terminates */ {
    // move elements satisfying _Pred to beginning of sequence
    _Adl_verify_range(_First, _Last);
    const auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast  = _Get_unwrapped(_Last);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) {
            const auto _Count = _STD distance(_UFirst, _ULast);
            if (_Count >= 2) {
                _TRY_BEGIN
                _Static_partitioned_partition2 _Operation{_Hw_threads, _Count, _UFirst, _Pass_fn(_Pred)};
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                _Seek_wrapped(_First, _Operation._Results);
                return _First;
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }
        }
    }

    _Seek_wrapped(_First, _STD partition(_UFirst, _ULast, _Pass_fn(_Pred)));
    return _First;
}

inline constexpr unsigned char _Local_available = 1;
inline constexpr unsigned char _Sum_available   = 2;

template <class _Ty>
struct _Scan_decoupled_lookback {
    // inter-chunk communication block in "Single-pass Parallel Prefix Scan with Decoupled Look-back" by Merrill and
    // Garland
    using value_type = _Ty;
    atomic<unsigned char> _State;
    _Storage_for<_Ty> _Local; // owned by thread iff _State < _Local_available; otherwise const and shared
    _Storage_for<_Ty> _Sum; // owned by thread iff _State < _Sum_available

    unsigned char _Get_available_state() const {
        for (;;) {
            const unsigned char _Local_state = _State.load();
            if (_Local_state != 0) {
                return _Local_state;
            }

            __std_execution_wait_on_uchar(reinterpret_cast<const unsigned char*>(&_State), _Local_state);
        }
    }

    void _Store_available_state(const unsigned char _New_state) {
        _State.store(_New_state);
        __std_execution_wake_by_address_all(&_State);
    }

    template <class _FwdIt, class _BinOp>
    void _Apply_exclusive_predecessor(_Ty& _Preceding, _FwdIt _First, const _FwdIt _Last, _BinOp _Reduce_op) {
        // apply _Preceding to [_First, _Last) and _Sum._Ref(), using _Reduce_op
        _Construct_in_place(_Sum._Ref(), _Reduce_op(_Preceding, _Local._Ref()));
        _State.store(_Local_available | _Sum_available);
        *_First = _Preceding;

#pragma loop(ivdep)
        while (++_First != _Last) {
            *_First = _Reduce_op(_Preceding, _STD move(*_First));
        }
    }

    template <class _FwdIt, class _BinOp>
    void _Apply_inclusive_predecessor(_Ty& _Preceding, _FwdIt _First, const _FwdIt _Last, _BinOp _Reduce_op) {
        // apply _Preceding to [_First, _Last) and _Sum._Ref(), using _Reduce_op
        _Construct_in_place(_Sum._Ref(), _Reduce_op(_Preceding, _Local._Ref()));
        _State.store(_Local_available | _Sum_available);

#pragma loop(ivdep)
        for (; _First != _Last; ++_First) {
            *_First = _Reduce_op(_Preceding, _STD move(*_First));
        }
    }

    ~_Scan_decoupled_lookback() {
        const auto _State_bits = _State.load(memory_order_relaxed);
        if (_State_bits & _Sum_available) {
            _Destroy_in_place(_Sum._Ref());
        }

        if (_State_bits & _Local_available) {
            _Destroy_in_place(_Local._Ref());
        }
    }
};

template <class _BidIt, class _BinOp>
typename _Iter_value_t<_BidIt>::value_type _Get_lookback_sum(const _BidIt _Current, _BinOp _Reduce_op) {
    // Calculate the sum of the _Scan_decoupled_lookback referenced by _Current.
    // pre: _Current->_State & _Local_available
    // pre: Some iterator i exists before _Current such that i->_Get_available_state & _Sum_available
    static_assert(_Is_specialization_v<_Iter_value_t<_BidIt>, _Scan_decoupled_lookback>, "Bad _Get_lookback_sum");
    auto _Prev = _Current;
    --_Prev;
    auto _Prev_state = _Prev->_Get_available_state();
    typename _Iter_value_t<_BidIt>::value_type _Result(
        _Reduce_op(_Prev_state & _Sum_available ? _Prev->_Sum._Ref() : _Prev->_Local._Ref(), _Current->_Local._Ref()));
    while (!(_Prev_state & _Sum_available)) {
        --_Prev;
        _Prev_state = _Prev->_Get_available_state();
        _Result =
            _Reduce_op(_Prev_state & _Sum_available ? _Prev->_Sum._Ref() : _Prev->_Local._Ref(), _STD move(_Result));
    }

    return _Result;
}

template <class _Ty>
struct _Casty_plus {
    // Adds the two arguments together and casts the result back to _Ty.
    // pre: the result from adding the two arguments together can fit in _Ty
    _NODISCARD constexpr _Ty operator()(_Ty _Val1, _Ty _Val2) const noexcept /* terminates */ {
        return static_cast<_Ty>(_Val1 + _Val2);
    }
};

template <class _Diff>
void _Surrender_elements_to_next_chunk(const size_t _Chunk_number,
    const typename _Parallel_vector<_Scan_decoupled_lookback<_Diff>>::iterator _Chunk_lookback_data) {
    // Deals with the case in which all of the elements in the chunk corresponding to _Chunk_lookback_data will be
    // handled by the following chunk, so this chunk needs to publish its results accordingly.
    if (_Chunk_number == 0) {
        // This is the first chunk, so we can immediately publish results. No need to set
        // _Chunk_lookback_data->_Local._Ref() to be 0, since chunk 0 has no predecessors and its local and total sums
        // are the same. Chunk_lookback_data->_Sum is already 0, so we can just publish results immediately.
        _Chunk_lookback_data->_Store_available_state(_Sum_available);
        return;
    }

    // We need to pass the previous chunk's sum to the right.
    // _Chunk_lookback_data->_Local is already 0, so we can just publish results immediately.
    _Chunk_lookback_data->_Store_available_state(_Local_available);

    const auto _Prev_chunk_lookback_data = _Prev_iter(_Chunk_lookback_data);
    _Diff _Prev_chunk_sum;
    if (_Prev_chunk_lookback_data->_Get_available_state() & _Sum_available) {
        _Prev_chunk_sum = _Prev_chunk_lookback_data->_Sum._Ref();
    } else {
        // Note that we can use _Casty_plus because _Diff is defined as _Common_diff<..., _RanIt3> and the maximum value
        // that will be placed in _Lookback by adding two of the previous sums together is the total number of elements
        // in the result. Assuming that _Dest has enough space for the result, the value produced by adding two previous
        // sums should fit inside _Diff.
        _Prev_chunk_sum = _Get_lookback_sum(_Prev_chunk_lookback_data, _Casty_plus<_Diff>{});
    }

    _Chunk_lookback_data->_Sum._Ref() = _Prev_chunk_sum;
    _Chunk_lookback_data->_Store_available_state(_Sum_available);
}

template <class _RanIt1, class _RanIt2, class _RanIt3>
void _Place_elements_from_indices(
    const _RanIt1 _First, _RanIt2 _Dest, _RanIt3 _Indices_first, const ptrdiff_t _Num_results) {
    // Places _Num_results elements at indices in _Indices_first from the range indicated by _First into _Dest.
    const auto _Last_index = _Indices_first + _Num_results;
    for (; _Indices_first != _Last_index; ++_Indices_first) {
        const auto _Curr_index = *_Indices_first;
        *_Dest                 = *(_First + static_cast<_Iter_diff_t<_RanIt1>>(_Curr_index));
        ++_Dest;
    }
}

template <class _RanIt1, class _RanIt2, class _RanIt3, class _Pr, class _SetOper>
struct _Static_partitioned_set_subtraction {
    using _Diff = _Common_diff_t<_RanIt1, _RanIt2, _RanIt3>;
    _Static_partition_team<_Diff> _Team;
    _Static_partition_range<_RanIt1, _Diff> _Basis;
    _Parallel_vector<_Diff> _Index_indicator; // buffer used to store information about indices in Range 1
    _Iterator_range<_RanIt2> _Range2;
    _RanIt3 _Dest;
    _Parallel_vector<_Scan_decoupled_lookback<_Diff>> _Lookback; // the "Single-pass Parallel Prefix Scan with
                                                                 // Decoupled Look-back" is used here to track
                                                                 // information about how many elements were placed
                                                                 // in _Dest by preceding chunks
    _Pr _Pred;
    _SetOper _Set_oper_per_chunk;

    _Static_partitioned_set_subtraction(const size_t _Hw_threads, const _Diff _Count, _RanIt1 _First1, _RanIt2 _First2,
        const _RanIt2 _Last2, _RanIt3 _Dest_, _Pr _Pred_, _SetOper _Set_oper)
        : _Team{_Count, _Get_chunked_work_chunk_count(_Hw_threads, _Count)}, _Basis{},
          _Index_indicator(static_cast<size_t>(_Count)), _Range2{_First2, _Last2}, _Dest(_Dest_),
          _Lookback(_Team._Chunks), _Pred(_Pred_), _Set_oper_per_chunk(_Set_oper) {
        _Basis._Populate(_Team, _First1);
    }

    _Cancellation_status _Process_chunk() {
        const auto _Key = _Team._Get_next_key();
        if (!_Key) {
            return _Cancellation_status::_Canceled;
        }

        const auto _Chunk_number        = _Key._Chunk_number;
        const auto _Chunk_lookback_data = _Lookback.begin() + static_cast<ptrdiff_t>(_Chunk_number);

        // iterators for the actual beginning of this chunk's range in _Range1 (after adjustments below)
        auto [_Range1_chunk_first, _Range1_chunk_last] = _Basis._Get_chunk(_Key);
        const bool _Last_chunk                         = _Chunk_number == _Team._Chunks - 1;

        // Get appropriate range for _Range1.
        // We don't want any spans of equal elements to reach across chunk boundaries.
        if (!_Last_chunk) {
            // Slide _Range1_chunk_last to the left so that there are no copies of *_Range1_chunk_last in _Range1_chunk.
            // Note that we know that this chunk is not the last, so we can look at the element at _Range1_chunk_last.
            _Range1_chunk_last = _STD lower_bound(_Range1_chunk_first, _Range1_chunk_last, *_Range1_chunk_last, _Pred);

            if (_Range1_chunk_last <= _Range1_chunk_first) {
                // All of the elements in _Range1's chunk are equal to the element at _Range1_chunk_last, so they will
                // be handled by the next chunk.
                _Surrender_elements_to_next_chunk<_Diff>(_Chunk_number, _Chunk_lookback_data);
                return _Cancellation_status::_Running;
            }
        }

        // Slide _Range1_chunk_first to the left so that all copies of *_Range1_chunk_first are in this chunk
        // of Range 1.
        _Range1_chunk_first = _STD lower_bound(_Basis._Start_at, _Range1_chunk_first, *_Range1_chunk_first, _Pred);

        // Get chunk in _Range2 that corresponds to our current chunk from _Range1
        auto _Range2_chunk_first = _STD lower_bound(_Range2._First, _Range2._Last, *_Range1_chunk_first, _Pred);
        auto _Range2_chunk_last =
            _STD upper_bound(_Range2_chunk_first, _Range2._Last, *_Prev_iter(_Range1_chunk_last), _Pred);

        // Publish results to rest of chunks.
        if (_Chunk_number == 0) {
            // Chunk 0 is special as it has no predecessor;
            // its local and total sums are the same and we can immediately put its results in _Dest.
            const auto _Num_results = _Set_oper_per_chunk._Update_dest(
                _Range1_chunk_first, _Range1_chunk_last, _Range2_chunk_first, _Range2_chunk_last, _Dest, _Pred);

            _Chunk_lookback_data->_Sum._Ref() = _Num_results;
            _Chunk_lookback_data->_Store_available_state(_Sum_available);
            return _Cancellation_status::_Running;
        }

        const auto _Prev_chunk_lookback_data = _Prev_iter(_Chunk_lookback_data);
        if (_Prev_chunk_lookback_data->_State.load() & _Sum_available) {
            // If the predecessor sum is already complete, we can incorporate its value directly for 1 pass.
            const auto _Prev_chunk_sum = _Prev_chunk_lookback_data->_Sum._Ref();
            auto _Chunk_specific_dest  = _Dest + static_cast<_Iter_diff_t<_RanIt3>>(_Prev_chunk_sum);
            const auto _Num_results    = _Set_oper_per_chunk._Update_dest(_Range1_chunk_first, _Range1_chunk_last,
                _Range2_chunk_first, _Range2_chunk_last, _Chunk_specific_dest, _Pred);

            _Chunk_lookback_data->_Sum._Ref() = static_cast<_Diff>(_Num_results + _Prev_chunk_sum);
            _Chunk_lookback_data->_Store_available_state(_Sum_available);
            return _Cancellation_status::_Running;
        }

        // Get range we can use for this chunk of range 1 in the buffer.
        auto _Index_chunk_first =
            _Index_indicator.begin() + static_cast<ptrdiff_t>(_Range1_chunk_first - _Basis._Start_at);

        // Determine the indices of elements that should be in the result from this chunk.
        const auto _Num_results             = _Set_oper_per_chunk._Mark_indices(_Range1_chunk_first, _Range1_chunk_last,
            _Range2_chunk_first, _Range2_chunk_last, _Index_chunk_first, _Pred);
        _Chunk_lookback_data->_Local._Ref() = _Num_results;
        _Chunk_lookback_data->_Store_available_state(_Local_available);

        // Apply the predecessor overall sum to current overall sum and elements.
        _Diff _Prev_chunk_sum;
        if (_Prev_chunk_lookback_data->_Get_available_state() & _Sum_available) {
            // Predecessor overall sum is done, use directly.
            _Prev_chunk_sum = _Prev_chunk_lookback_data->_Sum._Ref();
        } else {
            _Prev_chunk_sum = _Get_lookback_sum(_Prev_chunk_lookback_data, _Casty_plus<_Diff>{});
        }

        _Chunk_lookback_data->_Sum._Ref() = static_cast<_Diff>(_Num_results + _Prev_chunk_sum);
        _Chunk_lookback_data->_Store_available_state(_Sum_available);

        // Place elements from _Range1 in _Dest according to the offsets previously calculated.
        auto _Chunk_specific_dest = _Dest + static_cast<_Iter_diff_t<_RanIt3>>(_Prev_chunk_sum);
        _Place_elements_from_indices(
            _Range1_chunk_first, _Chunk_specific_dest, _Index_chunk_first, static_cast<ptrdiff_t>(_Num_results));
        return _Cancellation_status::_Running;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_set_subtraction*>(_Context));
    }
};

struct _Set_intersection_per_chunk {
    template <class _RanIt1, class _RanIt2, class _RanIt3, class _Pr>
    _Common_diff_t<_RanIt1, _RanIt2, _RanIt3> _Update_dest(
        _RanIt1 _First1, const _RanIt1 _Last1, _RanIt2 _First2, const _RanIt2 _Last2, _RanIt3 _Dest, _Pr _Pred) {
        // Copy elements from [_First, _Last1) that are also present in [_First2, _Last2) according to _Pred, to
        // _Dest. Returns the number of elements stored.
        return _STD set_intersection(_First1, _Last1, _First2, _Last2, _Dest, _Pred) - _Dest;
    }

    template <class _RanIt1, class _RanIt2, class _BidIt, class _Pr>
    _Iter_value_t<_BidIt> _Mark_indices(_RanIt1 _First1, const _RanIt1 _Last1, _RanIt2 _First2, const _RanIt2 _Last2,
        _BidIt _Index_chunk_first, _Pr _Pred) {
        // Stores offsets of elements in [_First1, _Last1) that are also present in [_First2, _Last2) according to
        // _Pred, to _Index_chunk_first. Returns the number of offsets stored.
        static_assert(is_integral_v<_Iter_value_t<_BidIt>>);
        static_assert(
            is_same_v<_Iter_value_t<_BidIt>, common_type_t<_Iter_value_t<_BidIt>, _Common_diff_t<_RanIt1, _RanIt2>>>);
        _DEBUG_ORDER_SET_UNWRAPPED(_RanIt2, _First1, _Last1, _Pred);
        _DEBUG_ORDER_SET_UNWRAPPED(_RanIt1, _First2, _Last2, _Pred);
        const auto _Index_chunk_first_save       = _Index_chunk_first;
        _Iter_diff_t<_RanIt1> _Curr_range1_index = 0;
        _Iter_diff_t<_RanIt2> _Curr_range2_index = 0;
        const auto _Range1_dist                  = _Last1 - _First1;
        const auto _Range2_dist                  = _Last2 - _First2;
        while (_Curr_range1_index < _Range1_dist && _Curr_range1_index < _Range2_dist) {
            if (_DEBUG_LT_PRED(_Pred, *(_First1 + _Curr_range1_index), *(_First2 + _Curr_range2_index))) {
                ++_Curr_range1_index;
            } else {
                if (!_Pred(*(_First2 + _Curr_range2_index), *(_First1 + _Curr_range1_index))) {
                    *_Index_chunk_first = static_cast<_Iter_value_t<_BidIt>>(_Curr_range1_index);
                    ++_Index_chunk_first;
                    ++_Curr_range1_index;
                }

                ++_Curr_range2_index;
            }
        }

        return static_cast<_Iter_value_t<_BidIt>>(_Index_chunk_first - _Index_chunk_first_save);
    }
};

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _FwdIt3, class _Pr,
    _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_FwdIt3 set_intersection(_ExPo&&, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2, _FwdIt3 _Dest,
    _Pr _Pred) noexcept /* terminates */ {
    // AND sets [_First1, _Last1) and [_First2, _Last2)
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt3);
    _Adl_verify_range(_First1, _Last1);
    _Adl_verify_range(_First2, _Last2);
    auto _UFirst1      = _Get_unwrapped(_First1);
    const auto _ULast1 = _Get_unwrapped(_Last1);
    auto _UFirst2      = _Get_unwrapped(_First2);
    const auto _ULast2 = _Get_unwrapped(_Last2);
    auto _UDest        = _Get_unwrapped_unverified(_Dest);
    using _Diff        = _Common_diff_t<_FwdIt1, _FwdIt2, _FwdIt3>;
    if constexpr (remove_reference_t<_ExPo>::_Parallelize
                  && _Is_random_iter_v<_FwdIt1> && _Is_random_iter_v<_FwdIt2> && _Is_random_iter_v<_FwdIt3>) {
        // only parallelize if desired, and all of the iterators given are random access
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) { // parallelize on multiprocessor machines
            const _Diff _Count1 = _ULast1 - _UFirst1;
            const _Diff _Count2 = _ULast2 - _UFirst2;
            if (_Count1 >= 2 && _Count2 >= 2) { // ... with each range containing at least 2 elements
                _TRY_BEGIN
                _Static_partitioned_set_subtraction _Operation(_Hw_threads, _Count1, _UFirst1, _UFirst2, _ULast2,
                    _UDest, _Pass_fn(_Pred), _Set_intersection_per_chunk());
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                _UDest += static_cast<_Iter_diff_t<_FwdIt3>>(_Operation._Lookback.back()._Sum._Ref());
                _Seek_wrapped(_Dest, _UDest);
                return _Dest;
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }
        }
    }

    _Seek_wrapped(_Dest, _STD set_intersection(_UFirst1, _ULast1, _UFirst2, _ULast2, _UDest, _Pass_fn(_Pred)));
    return _Dest;
}

struct _Set_difference_per_chunk {
    template <class _RanIt1, class _RanIt2, class _RanIt3, class _Pr>
    _Common_diff_t<_RanIt1, _RanIt2, _RanIt3> _Update_dest(
        _RanIt1 _First1, const _RanIt1 _Last1, _RanIt2 _First2, _RanIt2 _Last2, _RanIt3 _Dest, _Pr _Pred) {
        // Copy elements from [_First1, _Last1), except those in [_First2, _Last2) according to _Pred, to _Dest.
        // Returns the number of elements stored.
        return _STD set_difference(_First1, _Last1, _First2, _Last2, _Dest, _Pred) - _Dest;
    }

    template <class _RanIt1, class _RanIt2, class _BidIt, class _Pr>
    _Iter_value_t<_BidIt> _Mark_indices(_RanIt1 _First1, const _RanIt1 _Last1, _RanIt2 _First2, const _RanIt2 _Last2,
        _BidIt _Index_chunk_first, _Pr _Pred) {
        // Stores offsets of elements in [_First1, _Last1), except those in [_First2, _Last2) according to _Pred, to
        // _Index_chunk_first. Returns the number of offsets stored.
        static_assert(is_integral_v<_Iter_value_t<_BidIt>>);
        static_assert(
            is_same_v<_Iter_value_t<_BidIt>, common_type_t<_Iter_value_t<_BidIt>, _Common_diff_t<_RanIt1, _RanIt2>>>);
        _DEBUG_ORDER_SET_UNWRAPPED(_RanIt2, _First1, _Last1, _Pred);
        _DEBUG_ORDER_SET_UNWRAPPED(_RanIt1, _First2, _Last2, _Pred);
        const auto _Index_chunk_first_save       = _Index_chunk_first;
        _Iter_diff_t<_RanIt1> _Curr_range1_index = 0;
        _Iter_diff_t<_RanIt2> _Curr_range2_index = 0;
        const auto _Range1_dist                  = _Last1 - _First1;
        const auto _Range2_dist                  = _Last2 - _First2;
        while (_Curr_range1_index < _Range1_dist && _Curr_range2_index < _Range2_dist) {
            if (_DEBUG_LT_PRED(_Pred, *(_First1 + _Curr_range1_index), *(_First2 + _Curr_range2_index))) {
                *_Index_chunk_first = static_cast<_Iter_value_t<_BidIt>>(_Curr_range1_index);
                ++_Index_chunk_first;
                ++_Curr_range1_index;
            } else {
                if (!_Pred(*(_First2 + _Curr_range2_index), *(_First1 + _Curr_range1_index))) {
                    ++_Curr_range1_index;
                }

                ++_Curr_range2_index;
            }
        }

        // If we haven't traversed all of range 1 yet, we want to include the rest of it in the results.
        for (; _Curr_range1_index < _Range1_dist; ++_Curr_range1_index) {
            *_Index_chunk_first = static_cast<_Iter_value_t<_BidIt>>(_Curr_range1_index);
            ++_Index_chunk_first;
        }

        return static_cast<_Iter_value_t<_BidIt>>(_Index_chunk_first - _Index_chunk_first_save);
    }
};

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _FwdIt3, class _Pr,
    _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_FwdIt3 set_difference(_ExPo&&, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2, _FwdIt3 _Dest,
    _Pr _Pred) noexcept /* terminates */ {
    // take set [_First2, _Last2) from [_First1, _Last1)
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt3);
    _Adl_verify_range(_First1, _Last1);
    _Adl_verify_range(_First2, _Last2);
    auto _UFirst1      = _Get_unwrapped(_First1);
    const auto _ULast1 = _Get_unwrapped(_Last1);
    auto _UFirst2      = _Get_unwrapped(_First2);
    const auto _ULast2 = _Get_unwrapped(_Last2);
    auto _UDest        = _Get_unwrapped_unverified(_Dest);
    using _Diff        = _Common_diff_t<_FwdIt1, _FwdIt2, _FwdIt3>;
    if constexpr (remove_reference_t<_ExPo>::_Parallelize
                  && _Is_random_iter_v<_FwdIt1> && _Is_random_iter_v<_FwdIt2> && _Is_random_iter_v<_FwdIt3>) {
        // only parallelize if desired, and all of the iterators given are random access
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) { // parallelize on multiprocessor machines
            const _Diff _Count = _ULast1 - _UFirst1;
            if (_Count >= 2) { // ... with at least 2 elements in [_First1, _Last1)
                _TRY_BEGIN
                _Static_partitioned_set_subtraction _Operation(_Hw_threads, _Count, _UFirst1, _UFirst2, _ULast2, _UDest,
                    _Pass_fn(_Pred), _Set_difference_per_chunk());
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                _UDest += static_cast<_Iter_diff_t<_FwdIt3>>(_Operation._Lookback.back()._Sum._Ref());
                _Seek_wrapped(_Dest, _UDest);
                return _Dest;
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }
        }
    }

    _Seek_wrapped(_Dest, _STD set_difference(_UFirst1, _ULast1, _UFirst2, _ULast2, _UDest, _Pass_fn(_Pred)));
    return _Dest;
}

template <class _InIt, class _Ty, class _BinOp>
_Ty _Reduce_move_unchecked(_InIt _First, const _InIt _Last, _Ty _Val, _BinOp _Reduce_op) {
    // return reduction, choose optimization
    if constexpr (_Plus_on_arithmetic_ranges_reduction_v<_Unwrapped_t<const _InIt&>, _Ty, _BinOp>) {
        return _Reduce_plus_arithmetic_ranges(_First, _Last, _Val);
    } else {
        for (; _First != _Last; ++_First) {
            _Val = _Reduce_op(_STD move(_Val), _STD move(*_First)); // Requirement missing from N4713
        }

        return _Val;
    }
}

template <class _Ty, class _FwdIt, class _BinOp>
_Ty _Reduce_at_least_two(const _FwdIt _First, const _FwdIt _Last, _BinOp _Reduce_op) {
    // return reduction with no initial value
    // pre: distance(_First, _Last) >= 2
    if constexpr (_Plus_on_arithmetic_ranges_reduction_v<_FwdIt, _Ty, _BinOp>) {
        return _Reduce_plus_arithmetic_ranges(_First, _Last, _Ty{0});
    } else {
        auto _Next = _First;
        _Ty _Val   = _Reduce_op(*_First, *++_Next);
        while (++_Next != _Last) {
            _Val = _Reduce_op(_STD move(_Val), *_Next); // Requirement missing from N4713
        }

        return _Val;
    }
}

template <class _FwdIt, class _Ty, class _BinOp>
struct _Static_partitioned_reduce2 {
    // reduction task scheduled on the system thread pool
    _Static_partition_team<_Iter_diff_t<_FwdIt>> _Team;
    _Static_partition_range<_FwdIt> _Basis;
    _BinOp _Reduce_op;
    _Generalized_sum_drop<_Ty> _Results;

    _Static_partitioned_reduce2(
        const _Iter_diff_t<_FwdIt> _Count, const size_t _Chunks, const _FwdIt _First, _BinOp _Reduce_op_)
        : _Team{_Count, _Chunks}, _Basis{}, _Reduce_op(_Reduce_op_), _Results{_Team._Chunks} {
        _Basis._Populate(_Team, _First);
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        const auto _This = static_cast<_Static_partitioned_reduce2*>(_Context);
        auto _Key        = _This->_Team._Get_next_key();
        if (_Key) {
            auto _Chunk        = _This->_Basis._Get_chunk(_Key);
            auto _Local_result = _Reduce_at_least_two<_Ty>(_Chunk._First, _Chunk._Last, _This->_Reduce_op);
            while ((_Key = _This->_Team._Get_next_key())) {
                _Chunk        = _This->_Basis._Get_chunk(_Key);
                _Local_result = _STD reduce(_Chunk._First, _Chunk._Last, _STD move(_Local_result), _This->_Reduce_op);
            }

            _This->_Results._Add_result(_STD move(_Local_result));
        }
    }
};

template <class _ExPo, class _FwdIt, class _Ty, class _BinOp, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD _Ty reduce(_ExPo&&, const _FwdIt _First, const _FwdIt _Last, _Ty _Val, _BinOp _Reduce_op) noexcept
/* terminates */ {
    // return commutative and associative reduction of _Val and [_First, _Last), using _Reduce_op
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt);
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) { // parallelize on multiprocessor machines...
            const auto _Count  = _STD distance(_UFirst, _ULast);
            const auto _Chunks = _Get_least2_chunked_work_chunk_count(_Hw_threads, _Count);
            if (_Chunks > 1) {
                _TRY_BEGIN
                auto _Passed_fn = _Pass_fn(_Reduce_op);
                _Static_partitioned_reduce2<decltype(_UFirst), _Ty, decltype(_Passed_fn)> _Operation{
                    _Count, _Chunks, _UFirst, _Passed_fn};
                {
                    // we don't use _Run_chunked_parallel_work here because the initial value on background threads
                    // is synthesized from the input, but on this thread the initial value is _Val
                    const _Work_ptr _Work{_Operation};
                    // setup complete, hereafter nothrow or terminate
                    _Work._Submit_for_chunks(_Hw_threads, _Chunks);
                    while (const auto _Stolen_key = _Operation._Team._Get_next_key()) {
                        auto _Chunk = _Operation._Basis._Get_chunk(_Stolen_key);
                        _Val        = _STD reduce(_Chunk._First, _Chunk._Last, _STD move(_Val), _Pass_fn(_Reduce_op));
                    }
                } // join with _Work_ptr threads

                auto& _Results = _Operation._Results;
                return _Reduce_move_unchecked(_Results.begin(), _Results.end(), _STD move(_Val), _Pass_fn(_Reduce_op));
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }
        }
    }

    return _STD reduce(_UFirst, _ULast, _STD move(_Val), _Pass_fn(_Reduce_op));
}

template <class _FwdIt1, class _FwdIt2, class _Ty, class _BinOp1, class _BinOp2>
struct _Static_partitioned_transform_reduce_binary2 { // transform-reduction task scheduled on the system thread pool
    using _Diff = _Common_diff_t<_FwdIt1, _FwdIt2>;
    _Static_partition_team<_Diff> _Team;
    _Static_partition_range<_FwdIt1, _Diff> _Basis1;
    _Static_partition_range<_FwdIt2, _Diff> _Basis2;
    _BinOp1 _Reduce_op;
    _BinOp2 _Transform_op;
    _Generalized_sum_drop<_Ty> _Results;

    _Static_partitioned_transform_reduce_binary2(const _Diff _Count, const size_t _Chunks, const _FwdIt1 _First1,
        const _FwdIt2 _First2, _BinOp1 _Reduce_op_, _BinOp2 _Transform_op_)
        : _Team{_Count, _Chunks}, _Basis1{}, _Basis2{}, _Reduce_op(_Reduce_op_),
          _Transform_op(_Transform_op_), _Results{_Chunks} {
        _Basis1._Populate(_Team, _First1);
        _Basis2._Populate(_Team, _First2);
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        const auto _This = static_cast<_Static_partitioned_transform_reduce_binary2*>(_Context);
        auto _Key        = _This->_Team._Get_next_key();
        if (_Key) {
            auto _Reduce_op    = _This->_Reduce_op;
            auto _Transform_op = _This->_Transform_op;
            auto _Chunk1       = _This->_Basis1._Get_chunk(_Key);
            auto _First2 =
                _This->_Basis2._Get_first(_Key._Chunk_number, _This->_Team._Get_chunk_offset(_Key._Chunk_number));
            auto _Next1 = _Chunk1._First;
            auto _Next2 = _First2;
            // Requirement missing from N4713:
            _Ty _Val = _Reduce_op(_Transform_op(*_Chunk1._First, *_First2), _Transform_op(*++_Next1, *++_Next2));
            while (++_Next1 != _Chunk1._Last) {
                // Requirement missing from N4713:
                _Val = _Reduce_op(_STD move(_Val), _Transform_op(*_Next1, *++_Next2));
            }

            while ((_Key = _This->_Team._Get_next_key())) {
                _Chunk1 = _This->_Basis1._Get_chunk(_Key);
                _First2 =
                    _This->_Basis2._Get_first(_Key._Chunk_number, _This->_Team._Get_chunk_offset(_Key._Chunk_number));
                _Next1 = _Chunk1._First;
                _Next2 = _First2;
                for (; _Next1 != _Chunk1._Last; ++_Next1, (void) ++_Next2) {
                    // Requirement missing from N4713:
                    _Val = _Reduce_op(_STD move(_Val), _Transform_op(*_Next1, *_Next2));
                }
            }

            _This->_Results._Add_result(_STD move(_Val));
        }
    }
};

#pragma warning(push)
#pragma warning(disable : 4868) // compiler may not enforce left-to-right evaluation order
                                // in braced initializer list (/Wall)
template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Ty, class _BinOp1, class _BinOp2,
    _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD _Ty transform_reduce(_ExPo&&, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _Ty _Val, _BinOp1 _Reduce_op,
    _BinOp2 _Transform_op) noexcept /* terminates */ {
    // return commutative and associative transform-reduction of sequences, using _Reduce_op and _Transform_op
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    _Adl_verify_range(_First1, _Last1);
    auto _UFirst1      = _Get_unwrapped(_First1);
    const auto _ULast1 = _Get_unwrapped(_Last1);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) { // parallelize on multiprocessor machines...
            const auto _Count  = _STD distance(_UFirst1, _ULast1);
            auto _UFirst2      = _Get_unwrapped_n(_First2, _Count);
            const auto _Chunks = _Get_least2_chunked_work_chunk_count(_Hw_threads, _Count);
            if (_Chunks > 1) {
                _TRY_BEGIN
                auto _Passed_reduce    = _Pass_fn(_Reduce_op);
                auto _Passed_transform = _Pass_fn(_Transform_op);
                _Static_partitioned_transform_reduce_binary2<decltype(_UFirst1), decltype(_UFirst2), _Ty,
                    decltype(_Passed_reduce), decltype(_Passed_transform)>
                    _Operation{_Count, _Chunks, _UFirst1, _UFirst2, _Passed_reduce, _Passed_transform};
                { // ditto no _Run_chunked_parallel_work for the same reason as reduce
                    const _Work_ptr _Work{_Operation};
                    // setup complete, hereafter nothrow or terminate
                    _Work._Submit_for_chunks(_Hw_threads, _Chunks);
                    while (const auto _Stolen_key = _Operation._Team._Get_next_key()) {
                        const auto _Chunk_number = _Stolen_key._Chunk_number;
                        const auto _Chunk1       = _Operation._Basis1._Get_chunk(_Stolen_key);
                        _Val                     = _STD transform_reduce(_Chunk1._First, _Chunk1._Last,
                                                _Operation._Basis2._Get_first(
                                _Chunk_number, _Operation._Team._Get_chunk_offset(_Chunk_number)),
                                                _STD move(_Val), _Pass_fn(_Reduce_op), _Pass_fn(_Transform_op));
                    }
                } // join with _Work_ptr threads

                auto& _Results = _Operation._Results; // note: already transformed
                return _Reduce_move_unchecked(_Results.begin(), _Results.end(), _STD move(_Val), _Pass_fn(_Reduce_op));
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }

            return _STD transform_reduce(
                _UFirst1, _ULast1, _UFirst2, _STD move(_Val), _Pass_fn(_Reduce_op), _Pass_fn(_Transform_op));
        }
    }

    return _STD transform_reduce(_UFirst1, _ULast1,
        _Get_unwrapped_n(_First2, _Idl_distance<_FwdIt1>(_UFirst1, _ULast1)), _STD move(_Val), _Pass_fn(_Reduce_op),
        _Pass_fn(_Transform_op));
}
#pragma warning(pop)

template <class _FwdIt, class _Ty, class _BinOp, class _UnaryOp>
struct _Static_partitioned_transform_reduce2 { // transformed reduction task scheduled on the system thread pool
    _Static_partition_team<_Iter_diff_t<_FwdIt>> _Team;
    _Static_partition_range<_FwdIt> _Basis;
    _BinOp _Reduce_op;
    _UnaryOp _Transform_op;
    _Generalized_sum_drop<_Ty> _Results;

    _Static_partitioned_transform_reduce2(const _Iter_diff_t<_FwdIt> _Count, const size_t _Chunks, _FwdIt _First,
        _BinOp _Reduce_op_, _UnaryOp _Transform_op_)
        : _Team{_Count, _Chunks}, _Basis{}, _Reduce_op(_Reduce_op_), _Transform_op(_Transform_op_), _Results{_Chunks} {
        _Basis._Populate(_Team, _First);
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        const auto _This = static_cast<_Static_partitioned_transform_reduce2*>(_Context);
        auto _Key        = _This->_Team._Get_next_key();
        if (_Key) {
            auto& _Reduce_op    = _This->_Reduce_op;
            auto& _Transform_op = _This->_Transform_op;
            auto _Chunk         = _This->_Basis._Get_chunk(_Key);
            auto _Next          = _Chunk._First;
            _Ty _Val{_Reduce_op(_Transform_op(*_Chunk._First), _Transform_op(*++_Next))};
            while (++_Next != _Chunk._Last) {
                _Val = _Reduce_op(_STD move(_Val), _Transform_op(*_Next));
            }

            while ((_Key = _This->_Team._Get_next_key())) {
                _Chunk = _This->_Basis._Get_chunk(_Key);
                _Next  = _Chunk._First;
                for (; _Next != _Chunk._Last; ++_Next) {
                    _Val = _Reduce_op(_STD move(_Val), _Transform_op(*_Next));
                }
            }

            _This->_Results._Add_result(_STD move(_Val));
        }
    }
};

template <class _ExPo, class _FwdIt, class _Ty, class _BinOp, class _UnaryOp,
    _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_NODISCARD _Ty transform_reduce(_ExPo&&, const _FwdIt _First, const _FwdIt _Last, _Ty _Val, _BinOp _Reduce_op,
    _UnaryOp _Transform_op) noexcept /* terminates */ {
    // return commutative and associative reduction of transformed sequence, using _Reduce_op and _Transform_op
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt);
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) { // parallelize on multiprocessor machines...
            const auto _Count  = _STD distance(_UFirst, _ULast);
            const auto _Chunks = _Get_least2_chunked_work_chunk_count(_Hw_threads, _Count);
            if (_Chunks > 1) {
                _TRY_BEGIN
                auto _Passed_reduce    = _Pass_fn(_Reduce_op);
                auto _Passed_transform = _Pass_fn(_Transform_op);
                _Static_partitioned_transform_reduce2<decltype(_UFirst), _Ty, decltype(_Passed_reduce),
                    decltype(_Passed_transform)>
                    _Operation{_Count, _Chunks, _UFirst, _Passed_reduce, _Passed_transform};
                { // ditto no _Run_chunked_parallel_work for the same reason as reduce
                    const _Work_ptr _Work{_Operation};
                    // setup complete, hereafter nothrow or terminate
                    _Work._Submit_for_chunks(_Hw_threads, _Chunks);
                    while (auto _Stolen_key = _Operation._Team._Get_next_key()) {
                        // keep processing remaining chunks to comply with N4687 [intro.progress]/14
                        auto _Chunk = _Operation._Basis._Get_chunk(_Stolen_key);
                        _Val = _STD transform_reduce(_Chunk._First, _Chunk._Last, _STD move(_Val), _Pass_fn(_Reduce_op),
                            _Pass_fn(_Transform_op));
                    }
                } // join with _Work_ptr threads

                auto& _Results = _Operation._Results; // note: already transformed
                return _Reduce_move_unchecked(_Results.begin(), _Results.end(), _STD move(_Val), _Pass_fn(_Reduce_op));
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }
        }
    }

    return _STD transform_reduce(_UFirst, _ULast, _STD move(_Val), _Pass_fn(_Reduce_op), _Pass_fn(_Transform_op));
}

struct _No_init_tag {
    explicit _No_init_tag() = default;
}; // tag to indicate that no initial value is to be used

template <class _FwdIt1, class _FwdIt2, class _BinOp, class _Ty>
_FwdIt2 _Exclusive_scan_per_chunk(_FwdIt1 _First, const _FwdIt1 _Last, _FwdIt2 _Dest, _BinOp _Reduce_op, _Ty& _Val) {
    // local-sum for parallel exclusive_scan; writes local sums into [_Dest + 1, _Dest + (_Last - _First)) and stores
    // successor sum in _Val
    // pre: _Val is *uninitialized* && _First != _Last
    _Construct_in_place(_Val, *_First);
    for (;;) {
        ++_First;
        ++_Dest;
        if (_First == _Last) {
            return _Dest;
        }

        _Ty _Tmp(_Reduce_op(_Val, *_First)); // temp to enable _First == _Dest
        *_Dest = _Val;
        _Val   = _STD move(_Tmp);
    }
}

template <class _FwdIt1, class _FwdIt2, class _BinOp, class _Ty>
void _Exclusive_scan_per_chunk_complete(
    _FwdIt1 _First, const _FwdIt1 _Last, _FwdIt2 _Dest, _BinOp _Reduce_op, _Ty& _Val, _Ty& _Init) {
    // Sum for parallel exclusive_scan with predecessor available, into [_Dest, _Dest + (_Last - _First)) and stores
    // successor sum in _Val.
    // Pre: _Val is *uninitialized* && _First != _Last && predecessor sum is in _Init
    _Construct_in_place(_Val, _Reduce_op(_Init, *_First));
    *_Dest = _Init;
    while (++_First != _Last) {
        ++_Dest;
        _Ty _Tmp(_Reduce_op(_Val, *_First)); // temp to enable _First == _Dest
        *_Dest = _STD move(_Val);
        _Val   = _STD move(_Tmp);
    }
}

template <class _FwdIt1, class _FwdIt2, class _Ty, class _BinOp>
struct _Static_partitioned_exclusive_scan2 {
    using _Diff = _Common_diff_t<_FwdIt1, _FwdIt2>;
    _Static_partition_team<_Diff> _Team;
    _Static_partition_range<_FwdIt1, _Diff> _Basis1;
    _Static_partition_range<_FwdIt2, _Diff> _Basis2;
    _Parallel_vector<_Scan_decoupled_lookback<_Ty>> _Lookback;
    _Ty& _Initial;
    _BinOp _Reduce_op;

    _Static_partitioned_exclusive_scan2(const size_t _Hw_threads, const _Diff _Count, const _FwdIt1 _First,
        _Ty& _Initial_, _BinOp _Reduce_op_, const _FwdIt2&)
        : _Team{_Count, _Get_chunked_work_chunk_count(_Hw_threads, _Count)}, _Basis1{}, _Basis2{},
          _Lookback(_Team._Chunks), _Initial(_Initial_), _Reduce_op(_Reduce_op_) {
        _Basis1._Populate(_Team, _First);
    }

    _Cancellation_status _Process_chunk() {
        const auto _Key = _Team._Get_next_key();
        if (!_Key) {
            return _Cancellation_status::_Canceled;
        }

        const auto _Chunk_number = _Key._Chunk_number;
        const auto _In_range     = _Basis1._Get_chunk(_Key);
        const auto _Dest         = _Basis2._Get_first(_Chunk_number, _Team._Get_chunk_offset(_Chunk_number));
        // Run local exclusive_scan on this chunk
        const auto _Chunk = _Lookback.begin() + static_cast<ptrdiff_t>(_Chunk_number);
        if (_Chunk_number == 0) { // chunk 0 is special as it has no predecessor; its local and total sums are the same
            _Exclusive_scan_per_chunk_complete(
                _In_range._First, _In_range._Last, _Dest, _Reduce_op, _Chunk->_Sum._Ref(), _Initial);
            _Chunk->_Store_available_state(_Sum_available);
            return _Cancellation_status::_Running;
        }

        const auto _Prev_chunk = _Prev_iter(_Chunk);
        if (_Prev_chunk->_State.load() & _Sum_available) {
            // if predecessor sum already complete, we can incorporate its value directly for 1 pass
            _Exclusive_scan_per_chunk_complete(
                _In_range._First, _In_range._Last, _Dest, _Reduce_op, _Chunk->_Sum._Ref(), _Prev_chunk->_Sum._Ref());
            _Chunk->_Store_available_state(_Sum_available);
            return _Cancellation_status::_Running;
        }

        // Calculate local sum and publish to other threads
        const auto _Last =
            _Exclusive_scan_per_chunk(_In_range._First, _In_range._Last, _Dest, _Reduce_op, _Chunk->_Local._Ref());
        _Chunk->_Store_available_state(_Local_available);

        // Apply the predecessor overall sum to current overall sum and elements
        if (_Prev_chunk->_Get_available_state() & _Sum_available) { // predecessor overall sum done, use directly
            _Chunk->_Apply_exclusive_predecessor(_Prev_chunk->_Sum._Ref(), _Dest, _Last, _Reduce_op);
        } else {
            auto _Tmp = _Get_lookback_sum(_Prev_chunk, _Reduce_op);
            _Chunk->_Apply_exclusive_predecessor(_Tmp, _Dest, _Last, _Reduce_op);
        }

        return _Cancellation_status::_Running;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_exclusive_scan2*>(_Context));
    }
};

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Ty, class _BinOp,
    _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_FwdIt2 exclusive_scan(_ExPo&&, const _FwdIt1 _First, const _FwdIt1 _Last, _FwdIt2 _Dest, _Ty _Val,
    _BinOp _Reduce_op) noexcept /* terminates */ {
    // set each value in [_Dest, _Dest + (_Last - _First)) to the associative reduction of predecessors and _Val
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    _Adl_verify_range(_First, _Last);
    const auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast  = _Get_unwrapped(_Last);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) { // parallelize on multiprocessor machines
            const auto _Count = _STD distance(_UFirst, _ULast);
            const auto _UDest = _Get_unwrapped_n(_Dest, _Count);
            if (_Count >= 2) { // ... with at least 2 elements
                _TRY_BEGIN
                _Static_partitioned_exclusive_scan2 _Operation{
                    _Hw_threads, _Count, _UFirst, _Val, _Pass_fn(_Reduce_op), _UDest};
                _Seek_wrapped(_Dest, _Operation._Basis2._Populate(_Operation._Team, _UDest));
                // Note that _Val is used as temporary storage by whichever thread runs the first chunk.
                // If any thread starts any chunk, initialization is complete, so we can't enter the
                // catch or serial fallback below, so that's OK.
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                return _Dest;
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }

            _Seek_wrapped(_Dest, _STD exclusive_scan(_UFirst, _ULast, _UDest, _STD move(_Val), _Pass_fn(_Reduce_op)));
            return _Dest;
        }
    }

    _Seek_wrapped(
        _Dest, _STD exclusive_scan(_UFirst, _ULast, _Get_unwrapped_n(_Dest, _Idl_distance<_FwdIt1>(_UFirst, _ULast)),
                   _STD move(_Val), _Pass_fn(_Reduce_op)));
    return _Dest;
}

template <class _FwdIt1, class _FwdIt2, class _BinOp, class _Ty>
_FwdIt2 _Inclusive_scan_per_chunk(
    _FwdIt1 _First, const _FwdIt1 _Last, _FwdIt2 _Dest, _BinOp _Reduce_op, _Ty& _Val, _No_init_tag) {
    // Local-sum for parallel inclusive_scan; writes local inclusive prefix sums into _Dest and stores overall sum in
    // _Val.
    // pre: _Val is *uninitialized* && _First != _Last
    _Construct_in_place(_Val, *_First);
    for (;;) {
        *_Dest = _Val;
        ++_Dest;
        ++_First;
        if (_First == _Last) {
            return _Dest;
        }

        _Val = _Reduce_op(_STD move(_Val), *_First);
    }
}

template <class _FwdIt1, class _FwdIt2, class _BinOp, class _Ty, class _Ty_fwd,
    enable_if_t<!is_same_v<_No_init_tag, remove_const_t<remove_reference_t<_Ty_fwd>>>, int> = 0>
_FwdIt2 _Inclusive_scan_per_chunk(
    _FwdIt1 _First, const _FwdIt1 _Last, _FwdIt2 _Dest, _BinOp _Reduce_op, _Ty& _Val, _Ty_fwd&& _Predecessor) {
    // local-sum for parallel inclusive_scan; writes local inclusive prefix sums into _Dest and stores overall sum in
    // _Val.
    // pre: _Val is *uninitialized* && _First != _Last
    _Construct_in_place(_Val, _Reduce_op(_STD forward<_Ty_fwd>(_Predecessor), *_First));
    for (;;) {
        *_Dest = _Val;
        ++_Dest;
        ++_First;
        if (_First == _Last) {
            return _Dest;
        }

        _Val = _Reduce_op(_STD move(_Val), *_First);
    }
}

template <class _Ty, class _Init_ty, class _FwdIt1, class _FwdIt2, class _BinOp>
struct _Static_partitioned_inclusive_scan2 {
    using _Diff = _Common_diff_t<_FwdIt1, _FwdIt2>;
    _Static_partition_team<_Diff> _Team;
    _Static_partition_range<_FwdIt1, _Diff> _Basis1;
    _Static_partition_range<_FwdIt2, _Diff> _Basis2;
    _Parallel_vector<_Scan_decoupled_lookback<_Ty>> _Lookback;
    _BinOp _Reduce_op;
    _Init_ty& _Initial;

    _Static_partitioned_inclusive_scan2(
        const size_t _Hw_threads, const _Diff _Count, _BinOp _Reduce_op_, _Init_ty& _Initial_)
        : _Team{_Count, _Get_chunked_work_chunk_count(_Hw_threads, _Count)}, _Basis1{}, _Basis2{},
          _Lookback(_Team._Chunks), _Reduce_op(_Reduce_op_), _Initial(_Initial_) {}

    _Cancellation_status _Process_chunk() {
        const auto _Key = _Team._Get_next_key();
        if (!_Key) {
            return _Cancellation_status::_Canceled;
        }

        const auto _Chunk_number = _Key._Chunk_number;
        const auto _In_range     = _Basis1._Get_chunk(_Key);
        const auto _Dest         = _Basis2._Get_first(_Chunk_number, _Team._Get_chunk_offset(_Chunk_number));
        // Run local inclusive_scan on this chunk
        const auto _Chunk = _Lookback.begin() + static_cast<ptrdiff_t>(_Chunk_number);
        if (_Chunk_number == 0) { // chunk 0 is special as it has no predecessor; its local and total sums are the same
            _Inclusive_scan_per_chunk(
                _In_range._First, _In_range._Last, _Dest, _Reduce_op, _Chunk->_Sum._Ref(), _STD move(_Initial));
            _Chunk->_Store_available_state(_Sum_available);
            return _Cancellation_status::_Running;
        }

        const auto _Prev_chunk = _Prev_iter(_Chunk);
        if (_Prev_chunk->_State.load() & _Sum_available) {
            // if predecessor sum already complete, we can incorporate its value directly for 1 pass
            _Inclusive_scan_per_chunk(
                _In_range._First, _In_range._Last, _Dest, _Reduce_op, _Chunk->_Sum._Ref(), _Prev_chunk->_Sum._Ref());
            _Chunk->_Store_available_state(_Sum_available);
            return _Cancellation_status::_Running;
        }

        // Calculate local sum and publish to other threads
        const auto _Last = _Inclusive_scan_per_chunk(
            _In_range._First, _In_range._Last, _Dest, _Reduce_op, _Chunk->_Local._Ref(), _No_init_tag{});
        _Chunk->_Store_available_state(_Local_available);

        // Apply the predecessor overall sum to current overall sum and elements
        if (_Prev_chunk->_Get_available_state() & _Sum_available) { // predecessor overall sum done, use directly
            _Chunk->_Apply_inclusive_predecessor(_Prev_chunk->_Sum._Ref(), _Dest, _Last, _Reduce_op);
        } else {
            auto _Tmp = _Get_lookback_sum(_Prev_chunk, _Reduce_op);
            _Chunk->_Apply_inclusive_predecessor(_Tmp, _Dest, _Last, _Reduce_op);
        }

        return _Cancellation_status::_Running;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_inclusive_scan2*>(_Context));
    }
};

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _BinOp, class _Ty,
    _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_FwdIt2 inclusive_scan(_ExPo&&, _FwdIt1 _First, _FwdIt1 _Last, _FwdIt2 _Dest, _BinOp _Reduce_op, _Ty _Val) noexcept
/* terminates */ {
    // compute partial noncommutative and associative reductions including _Val into _Dest, using _Reduce_op
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    _Adl_verify_range(_First, _Last);
    const auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast  = _Get_unwrapped(_Last);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) { // parallelize on multiprocessor machines
            const auto _Count = _STD distance(_First, _Last);
            auto _UDest       = _Get_unwrapped_n(_Dest, _Count);
            if (_Count >= 2) { // ... with at least 2 elements
                _TRY_BEGIN
                auto _Passed_op = _Pass_fn(_Reduce_op);
                _Static_partitioned_inclusive_scan2<_Ty, _Ty, _Unwrapped_t<const _FwdIt1&>, decltype(_UDest),
                    decltype(_Passed_op)>
                    _Operation{_Hw_threads, _Count, _Passed_op, _Val};
                _Operation._Basis1._Populate(_Operation._Team, _UFirst);
                _Seek_wrapped(_Dest, _Operation._Basis2._Populate(_Operation._Team, _UDest));
                // Note that _Val is moved from by whichever thread runs the first chunk.
                // If any thread starts any chunk, initialization is complete, so we can't enter the
                // catch or serial fallback below.
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                return _Dest;
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }

            _Seek_wrapped(_Dest, _STD inclusive_scan(_UFirst, _ULast, _UDest, _Pass_fn(_Reduce_op), _STD move(_Val)));
            return _Dest;
        }
    }

    _Seek_wrapped(
        _Dest, _STD inclusive_scan(_UFirst, _ULast, _Get_unwrapped_n(_Dest, _Idl_distance<_FwdIt1>(_UFirst, _ULast)),
                   _Pass_fn(_Reduce_op), _STD move(_Val)));
    return _Dest;
}

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _BinOp, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_FwdIt2 inclusive_scan(_ExPo&&, _FwdIt1 _First, _FwdIt1 _Last, _FwdIt2 _Dest, _BinOp _Reduce_op) noexcept
/* terminates */ {
    // compute partial noncommutative and associative reductions into _Dest, using _Reduce_op
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    _Adl_verify_range(_First, _Last);
    const auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast  = _Get_unwrapped(_Last);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) { // parallelize on multiprocessor machines
            const auto _Count = _STD distance(_UFirst, _ULast);
            auto _UDest       = _Get_unwrapped_n(_Dest, _Count);
            if (_Count >= 2) { // ... with at least 2 elements
                _TRY_BEGIN
                _No_init_tag _Tag;
                auto _Passed_op = _Pass_fn(_Reduce_op);
                _Static_partitioned_inclusive_scan2<_Iter_value_t<_FwdIt1>, _No_init_tag, _Unwrapped_t<const _FwdIt1&>,
                    decltype(_UDest), decltype(_Passed_op)>
                    _Operation{_Hw_threads, _Count, _Passed_op, _Tag};
                _Operation._Basis1._Populate(_Operation._Team, _UFirst);
                _Seek_wrapped(_Dest, _Operation._Basis2._Populate(_Operation._Team, _UDest));
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                return _Dest;
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }

            _Seek_wrapped(_Dest, _STD inclusive_scan(_UFirst, _ULast, _UDest, _Pass_fn(_Reduce_op)));
            return _Dest;
        }
    }

    _Seek_wrapped(_Dest, _STD inclusive_scan(_UFirst, _ULast,
                             _Get_unwrapped_n(_Dest, _Idl_distance<_FwdIt1>(_UFirst, _ULast)), _Pass_fn(_Reduce_op)));
    return _Dest;
}

template <class _FwdIt1, class _FwdIt2, class _BinOp, class _UnaryOp, class _Ty>
_FwdIt2 _Transform_exclusive_scan_per_chunk(
    _FwdIt1 _First, const _FwdIt1 _Last, _FwdIt2 _Dest, _BinOp _Reduce_op, _UnaryOp _Transform_op, _Ty& _Val) {
    // Local-sum for parallel transform_exclusive_scan; writes local sums into [_Dest + 1, _Dest + (_Last - _First)) and
    // stores successor sum in _Val.
    // pre: _Val is *uninitialized* && _First != _Last
    _Construct_in_place(_Val, _Transform_op(*_First));
    for (;;) {
        ++_First;
        ++_Dest;
        if (_First == _Last) {
            return _Dest;
        }

        _Ty _Tmp(_Reduce_op(_Val, _Transform_op(*_First))); // temp to enable _First == _Dest
        *_Dest = _Val;
        _Val   = _STD move(_Tmp);
    }
}

template <class _FwdIt1, class _FwdIt2, class _BinOp, class _UnaryOp, class _Ty>
void _Transform_exclusive_scan_per_chunk_complete(_FwdIt1 _First, const _FwdIt1 _Last, _FwdIt2 _Dest, _BinOp _Reduce_op,
    _UnaryOp _Transform_op, _Ty& _Val, _Ty& _Init) {
    // Sum for parallel transform_exclusive_scan with predecessor available, into [_Dest, _Dest + (_Last - _First)) and
    // stores successor sum in _Val.
    // pre: _Val is *uninitialized* && _First != _Last && predecessor sum is in _Init
    _Construct_in_place(_Val, _Reduce_op(_Init, _Transform_op(*_First)));
    *_Dest = _Init;
    while (++_First != _Last) {
        ++_Dest;
        _Ty _Tmp(_Reduce_op(_Val, _Transform_op(*_First))); // temp to enable _First == _Dest
        *_Dest = _STD move(_Val);
        _Val   = _STD move(_Tmp);
    }
}

template <class _FwdIt1, class _FwdIt2, class _Ty, class _BinOp, class _UnaryOp>
struct _Static_partitioned_transform_exclusive_scan2 {
    using _Diff = _Common_diff_t<_FwdIt1, _FwdIt2>;
    _Static_partition_team<_Diff> _Team;
    _Static_partition_range<_FwdIt1, _Diff> _Basis1;
    _Static_partition_range<_FwdIt2, _Diff> _Basis2;
    _Parallel_vector<_Scan_decoupled_lookback<_Ty>> _Lookback;
    _Ty& _Initial;
    _BinOp _Reduce_op;
    _UnaryOp _Transform_op;

    _Static_partitioned_transform_exclusive_scan2(const size_t _Hw_threads, const _Diff _Count, const _FwdIt1 _First,
        _Ty& _Initial_, _BinOp _Reduce_op_, _UnaryOp _Transform_op_, const _FwdIt2&)
        : _Team{_Count, _Get_chunked_work_chunk_count(_Hw_threads, _Count)}, _Basis1{}, _Basis2{},
          _Lookback(_Team._Chunks), _Initial(_Initial_), _Reduce_op(_Reduce_op_), _Transform_op(_Transform_op_) {
        _Basis1._Populate(_Team, _First);
    }

    _Cancellation_status _Process_chunk() {
        const auto _Key = _Team._Get_next_key();
        if (!_Key) {
            return _Cancellation_status::_Canceled;
        }

        const auto _Chunk_number = _Key._Chunk_number;
        const auto _In_range     = _Basis1._Get_chunk(_Key);
        const auto _Dest         = _Basis2._Get_first(_Chunk_number, _Team._Get_chunk_offset(_Chunk_number));
        // Run local transform_exclusive_scan on this chunk
        const auto _Chunk = _Lookback.begin() + static_cast<ptrdiff_t>(_Chunk_number);
        if (_Chunk_number == 0) { // chunk 0 is special as it has no predecessor; its local and total sums are the same
            _Transform_exclusive_scan_per_chunk_complete(
                _In_range._First, _In_range._Last, _Dest, _Reduce_op, _Transform_op, _Chunk->_Sum._Ref(), _Initial);
            _Chunk->_Store_available_state(_Sum_available);
            return _Cancellation_status::_Running;
        }

        const auto _Prev_chunk = _Prev_iter(_Chunk);
        if (_Prev_chunk->_State.load() & _Sum_available) {
            // if predecessor sum already complete, we can incorporate its value directly for 1 pass
            _Transform_exclusive_scan_per_chunk_complete(_In_range._First, _In_range._Last, _Dest, _Reduce_op,
                _Transform_op, _Chunk->_Sum._Ref(), _Prev_chunk->_Sum._Ref());
            _Chunk->_Store_available_state(_Sum_available);
            return _Cancellation_status::_Running;
        }

        // Calculate local sum and publish to other threads
        const auto _Last = _Transform_exclusive_scan_per_chunk(
            _In_range._First, _In_range._Last, _Dest, _Reduce_op, _Transform_op, _Chunk->_Local._Ref());
        _Chunk->_Store_available_state(_Local_available);

        // Apply the predecessor overall sum to current overall sum and elements
        if (_Prev_chunk->_Get_available_state() & _Sum_available) { // predecessor overall sum done, use directly
            _Chunk->_Apply_exclusive_predecessor(_Prev_chunk->_Sum._Ref(), _Dest, _Last, _Reduce_op);
        } else {
            auto _Tmp = _Get_lookback_sum(_Prev_chunk, _Reduce_op);
            _Chunk->_Apply_exclusive_predecessor(_Tmp, _Dest, _Last, _Reduce_op);
        }

        return _Cancellation_status::_Running;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_transform_exclusive_scan2*>(_Context));
    }
};

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Ty, class _BinOp, class _UnaryOp,
    _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_FwdIt2 transform_exclusive_scan(_ExPo&&, const _FwdIt1 _First, const _FwdIt1 _Last, _FwdIt2 _Dest, _Ty _Val,
    _BinOp _Reduce_op, _UnaryOp _Transform_op) noexcept /* terminates */ {
    // set each value in [_Dest, _Dest + (_Last - _First)) to the associative reduction of transformed predecessors
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    _Adl_verify_range(_First, _Last);
    const auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast  = _Get_unwrapped(_Last);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) { // parallelize on multiprocessor machines
            const auto _Count = _STD distance(_UFirst, _ULast);
            const auto _UDest = _Get_unwrapped_n(_Dest, _Count);
            if (_Count >= 2) { // ... with at least 2 elements
                _TRY_BEGIN
                _Static_partitioned_transform_exclusive_scan2 _Operation{
                    _Hw_threads, _Count, _UFirst, _Val, _Pass_fn(_Reduce_op), _Pass_fn(_Transform_op), _UDest};
                _Seek_wrapped(_Dest, _Operation._Basis2._Populate(_Operation._Team, _UDest));
                // Note that _Val is used as temporary storage by whichever thread runs the first chunk.
                // If any thread starts any chunk, initialization is complete, so we can't enter the
                // catch or serial fallback below, so that's OK.
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                return _Dest;
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }

            _Seek_wrapped(_Dest, _STD transform_exclusive_scan(_UFirst, _ULast, _UDest, _STD move(_Val),
                                     _Pass_fn(_Reduce_op), _Pass_fn(_Transform_op)));
            return _Dest;
        }
    }

    _Seek_wrapped(_Dest,
        _STD transform_exclusive_scan(_UFirst, _ULast, _Get_unwrapped_n(_Dest, _Idl_distance<_FwdIt1>(_UFirst, _ULast)),
            _STD move(_Val), _Pass_fn(_Reduce_op), _Pass_fn(_Transform_op)));
    return _Dest;
}

template <class _FwdIt1, class _FwdIt2, class _BinOp, class _UnaryOp, class _Ty>
_FwdIt2 _Transform_inclusive_scan_per_chunk(_FwdIt1 _First, const _FwdIt1 _Last, _FwdIt2 _Dest, _BinOp _Reduce_op,
    _UnaryOp _Transform_op, _Ty& _Val, _No_init_tag) {
    // Local-sum for parallel transform_inclusive_scan; writes local inclusive prefix sums into _Dest and stores overall
    // sum in _Val.
    // pre: _Val is *uninitialized* && _First != _Last
    _Construct_in_place(_Val, _Transform_op(*_First));
    for (;;) {
        *_Dest = _Val;
        ++_Dest;
        ++_First;
        if (_First == _Last) {
            return _Dest;
        }

        _Val = _Reduce_op(_STD move(_Val), _Transform_op(*_First));
    }
}

template <class _FwdIt1, class _FwdIt2, class _BinOp, class _UnaryOp, class _Ty, class _Ty_fwd,
    enable_if_t<!is_same_v<_No_init_tag, remove_const_t<remove_reference_t<_Ty_fwd>>>, int> = 0>
_FwdIt2 _Transform_inclusive_scan_per_chunk(_FwdIt1 _First, const _FwdIt1 _Last, _FwdIt2 _Dest, _BinOp _Reduce_op,
    _UnaryOp _Transform_op, _Ty& _Val, _Ty_fwd&& _Predecessor) {
    // local-sum for parallel transform_inclusive_scan; writes local inclusive prefix sums into _Dest and stores overall
    // sum in _Val
    // pre: _Val is *uninitialized* && _First != _Last
    _Construct_in_place(_Val, _Reduce_op(_STD forward<_Ty_fwd>(_Predecessor), _Transform_op(*_First)));
    for (;;) {
        *_Dest = _Val;
        ++_Dest;
        ++_First;
        if (_First == _Last) {
            return _Dest;
        }

        _Val = _Reduce_op(_STD move(_Val), _Transform_op(*_First));
    }
}

template <class _Ty, class _Init_ty, class _FwdIt1, class _FwdIt2, class _BinOp, class _UnaryOp>
struct _Static_partitioned_transform_inclusive_scan2 {
    using _Diff = _Common_diff_t<_FwdIt1, _FwdIt2>;
    _Static_partition_team<_Diff> _Team;
    _Static_partition_range<_FwdIt1, _Diff> _Basis1;
    _Static_partition_range<_FwdIt2, _Diff> _Basis2;
    _Parallel_vector<_Scan_decoupled_lookback<_Ty>> _Lookback;
    _BinOp _Reduce_op;
    _UnaryOp _Transform_op;
    _Init_ty& _Initial;

    _Static_partitioned_transform_inclusive_scan2(
        const size_t _Hw_threads, const _Diff _Count, _BinOp _Reduce_op_, _UnaryOp _Transform_op_, _Init_ty& _Initial_)
        : _Team{_Count, _Get_chunked_work_chunk_count(_Hw_threads, _Count)}, _Basis1{}, _Basis2{},
          _Lookback(_Team._Chunks), _Reduce_op(_Reduce_op_), _Transform_op(_Transform_op_), _Initial(_Initial_) {}

    _Cancellation_status _Process_chunk() {
        const auto _Key = _Team._Get_next_key();
        if (!_Key) {
            return _Cancellation_status::_Canceled;
        }

        const auto _Chunk_number = _Key._Chunk_number;
        const auto _In_range     = _Basis1._Get_chunk(_Key);
        const auto _Dest         = _Basis2._Get_first(_Chunk_number, _Team._Get_chunk_offset(_Chunk_number));
        // Run local transform_inclusive_scan on this chunk
        const auto _Chunk = _Lookback.begin() + static_cast<ptrdiff_t>(_Chunk_number);
        if (_Chunk_number == 0) { // chunk 0 is special as it has no predecessor; its local and total sums are the same
            _Transform_inclusive_scan_per_chunk(_In_range._First, _In_range._Last, _Dest, _Reduce_op, _Transform_op,
                _Chunk->_Sum._Ref(), _STD move(_Initial));
            _Chunk->_Store_available_state(_Sum_available);
            return _Cancellation_status::_Running;
        }

        const auto _Prev_chunk = _Prev_iter(_Chunk);
        if (_Prev_chunk->_State.load() & _Sum_available) {
            // if predecessor sum already complete, we can incorporate its value directly for 1 pass
            _Transform_inclusive_scan_per_chunk(_In_range._First, _In_range._Last, _Dest, _Reduce_op, _Transform_op,
                _Chunk->_Sum._Ref(), _Prev_chunk->_Sum._Ref());
            _Chunk->_Store_available_state(_Sum_available);
            return _Cancellation_status::_Running;
        }

        // Calculate local sum and publish to other threads
        const auto _Last = _Transform_inclusive_scan_per_chunk(
            _In_range._First, _In_range._Last, _Dest, _Reduce_op, _Transform_op, _Chunk->_Local._Ref(), _No_init_tag{});
        _Chunk->_Store_available_state(_Local_available);

        // Apply the predecessor overall sum to current overall sum and elements
        if (_Prev_chunk->_Get_available_state() & _Sum_available) { // predecessor overall sum done, use directly
            _Chunk->_Apply_inclusive_predecessor(_Prev_chunk->_Sum._Ref(), _Dest, _Last, _Reduce_op);
        } else {
            auto _Tmp = _Get_lookback_sum(_Prev_chunk, _Reduce_op);
            _Chunk->_Apply_inclusive_predecessor(_Tmp, _Dest, _Last, _Reduce_op);
        }

        return _Cancellation_status::_Running;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_transform_inclusive_scan2*>(_Context));
    }
};

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Ty, class _BinOp, class _UnaryOp,
    _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_FwdIt2 transform_inclusive_scan(_ExPo&&, const _FwdIt1 _First, const _FwdIt1 _Last, _FwdIt2 _Dest, _BinOp _Reduce_op,
    _UnaryOp _Transform_op, _Ty _Val) noexcept /* terminates */ {
    // compute partial noncommutative and associative transformed reductions including _Val into _Dest
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    _Adl_verify_range(_First, _Last);
    const auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast  = _Get_unwrapped(_Last);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) { // parallelize on multiprocessor machines
            const auto _Count = _STD distance(_UFirst, _ULast);
            auto _UDest       = _Get_unwrapped_n(_Dest, _Count);
            if (_Count >= 2) { // ... with at least 2 elements
                _TRY_BEGIN
                auto _Passed_reduce    = _Pass_fn(_Reduce_op);
                auto _Passed_transform = _Pass_fn(_Transform_op);
                _Static_partitioned_transform_inclusive_scan2<_Ty, _Ty, _Unwrapped_t<const _FwdIt1&>, decltype(_UDest),
                    decltype(_Passed_reduce), decltype(_Passed_transform)>
                    _Operation{_Hw_threads, _Count, _Passed_reduce, _Passed_transform, _Val};
                _Operation._Basis1._Populate(_Operation._Team, _UFirst);
                _Seek_wrapped(_Dest, _Operation._Basis2._Populate(_Operation._Team, _UDest));
                // Note that _Val is moved from by whichever thread runs the first chunk.
                // If any thread starts any chunk, initialization is complete, so we can't enter the
                // catch or serial fallback below.
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                return _Dest;
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }

            _Seek_wrapped(_Dest, _STD transform_inclusive_scan(_UFirst, _ULast, _UDest, _Pass_fn(_Reduce_op),
                                     _Pass_fn(_Transform_op), _STD move(_Val)));
            return _Dest;
        }
    }

    _Seek_wrapped(_Dest,
        _STD transform_inclusive_scan(_UFirst, _ULast, _Get_unwrapped_n(_Dest, _Idl_distance<_FwdIt1>(_UFirst, _ULast)),
            _Pass_fn(_Reduce_op), _Pass_fn(_Transform_op), _STD move(_Val)));
    return _Dest;
}

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _BinOp, class _UnaryOp,
    _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_FwdIt2 transform_inclusive_scan(_ExPo&&, const _FwdIt1 _First, const _FwdIt1 _Last, _FwdIt2 _Dest, _BinOp _Reduce_op,
    _UnaryOp _Transform_op) noexcept /* terminates */ {
    // compute partial noncommutative and associative transformed reductions into _Dest
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    _Adl_verify_range(_First, _Last);
    const auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast  = _Get_unwrapped(_Last);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) { // parallelize on multiprocessor machines
            const auto _Count = _STD distance(_UFirst, _ULast);
            auto _UDest       = _Get_unwrapped_n(_Dest, _Count);
            if (_Count >= 2) { // ... with at least 2 elements
                _TRY_BEGIN
                _No_init_tag _Tag;
                auto _Passed_reduce    = _Pass_fn(_Reduce_op);
                auto _Passed_transform = _Pass_fn(_Transform_op);
                using _Intermediate_t  = decay_t<decltype(_Transform_op(*_UFirst))>;
                _Static_partitioned_transform_inclusive_scan2<_Intermediate_t, _No_init_tag,
                    _Unwrapped_t<const _FwdIt1&>, decltype(_UDest), decltype(_Passed_reduce),
                    decltype(_Passed_transform)>
                    _Operation{_Hw_threads, _Count, _Passed_reduce, _Passed_transform, _Tag};
                _Operation._Basis1._Populate(_Operation._Team, _UFirst);
                _Seek_wrapped(_Dest, _Operation._Basis2._Populate(_Operation._Team, _UDest));
                _Run_chunked_parallel_work(_Hw_threads, _Operation);
                return _Dest;
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }

            _Seek_wrapped(_Dest,
                _STD transform_inclusive_scan(_UFirst, _ULast, _UDest, _Pass_fn(_Reduce_op), _Pass_fn(_Transform_op)));
            return _Dest;
        }
    }

    _Seek_wrapped(_Dest,
        _STD transform_inclusive_scan(_UFirst, _ULast, _Get_unwrapped_n(_Dest, _Idl_distance<_FwdIt1>(_UFirst, _ULast)),
            _Pass_fn(_Reduce_op), _Pass_fn(_Transform_op)));
    return _Dest;
}

template <class _FwdIt1, class _FwdIt2, class _BinOp>
struct _Static_partitioned_adjacent_difference2 {
    using _Diff = _Common_diff_t<_FwdIt1, _FwdIt2>;
    _Static_partition_team<_Diff> _Team;
    // note offset partitioning:
    _Static_partition_range<_FwdIt1, _Diff> _Basis1; // contains partition of [_First, _Last - 1)
    _Static_partition_range<_FwdIt2, _Diff> _Basis2; // contains partition of [_Dest + 1, _Dest + (_Last - _First))
    _BinOp _Diff_op;

    _Static_partitioned_adjacent_difference2(
        const size_t _Hw_threads, const _Diff _Count, const _FwdIt1 _First, _BinOp _Diff_op_, const _FwdIt2&)
        : _Team{_Count, _Get_chunked_work_chunk_count(_Hw_threads, _Count)}, _Basis1{}, _Basis2{}, _Diff_op(_Diff_op_) {
        _Basis1._Populate(_Team, _First);
    }

    _Cancellation_status _Process_chunk() {
        const auto _Key = _Team._Get_next_key();
        if (!_Key) {
            return _Cancellation_status::_Canceled;
        }

        const auto _Chunk_number = _Key._Chunk_number;
        auto _In_range           = _Basis1._Get_chunk(_Key);
        auto _Dest               = _Basis2._Get_first(_Chunk_number, _Team._Get_chunk_offset(_Chunk_number));
        auto _Next               = _In_range._First;
        do {
            ++_Next; // note: steps 1 element into the following chunk
            *_Dest = _Diff_op(*_Next, *_In_range._First);
            ++_Dest;
            _In_range._First = _Next;
        } while (_In_range._First != _In_range._Last);

        return _Cancellation_status::_Running;
    }

    static void __stdcall _Threadpool_callback(
        __std_PTP_CALLBACK_INSTANCE, void* const _Context, __std_PTP_WORK) noexcept /* terminates */ {
        _Run_available_chunked_work(*static_cast<_Static_partitioned_adjacent_difference2*>(_Context));
    }
};

template <class _FwdIt1, class _FwdIt2, class _BinOp>
_FwdIt2 _Adjacent_difference_seq(_FwdIt1 _First, const _FwdIt1 _Last, _FwdIt2 _Dest, _BinOp _Diff_op) {
    // compute adjacent differences into _Dest, serially
    if (_First != _Last) {
        *_Dest = *_First;
        ++_Dest;
        for (auto _Next = _First; ++_Next != _Last; _First = _Next) {
            *_Dest = _Diff_op(*_Next, *_First);
            ++_Dest;
        }
    }

    return _Dest;
}

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _BinOp, _Enable_if_execution_policy_t<_ExPo> /* = 0 */>
_FwdIt2 adjacent_difference(_ExPo&&, const _FwdIt1 _First, const _FwdIt1 _Last, _FwdIt2 _Dest, _BinOp _Diff_op) noexcept
/* terminates */ {
    // compute adjacent differences into _Dest
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    if constexpr (remove_reference_t<_ExPo>::_Parallelize) {
        const size_t _Hw_threads = __std_parallel_algorithms_hw_threads();
        if (_Hw_threads > 1) { // parallelize on multiprocessor machines
            auto _Count       = _STD distance(_UFirst, _ULast);
            const auto _UDest = _Get_unwrapped_n(_Dest, _Count);
            if (_Count >= 2) { // ... with at least 2 elements
                _TRY_BEGIN
                --_Count; // note unusual offset partitioning
                _Static_partitioned_adjacent_difference2 _Operation{
                    _Hw_threads, _Count, _UFirst, _Pass_fn(_Diff_op), _UDest};
                auto _Result = _Operation._Basis2._Populate(_Operation._Team, _Next_iter(_UDest));
                const _Work_ptr _Work_op{_Operation};
                // setup complete, hereafter nothrow or terminate
                _Work_op._Submit_for_chunks(_Hw_threads, _Operation._Team._Chunks);
                // must be done after setup is complete to avoid duplicate assign in serial fallback:
                *_UDest = *_UFirst;
                _Run_available_chunked_work(_Operation);
                _Seek_wrapped(_Dest, _Result);
                return _Dest;
                _CATCH(const _Parallelism_resources_exhausted&)
                // fall through to serial case below
                _CATCH_END
            }

            _Seek_wrapped(_Dest, _Adjacent_difference_seq(_UFirst, _ULast, _UDest, _Pass_fn(_Diff_op)));
            return _Dest;
        }
    }

    // Don't call serial adjacent_difference because it's described as creating a temporary we can avoid
    _Seek_wrapped(_Dest, _Adjacent_difference_seq(_UFirst, _ULast,
                             _Get_unwrapped_n(_Dest, _Idl_distance<_FwdIt1>(_UFirst, _ULast)), _Pass_fn(_Diff_op)));
    return _Dest;
}
_STD_END
#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _HAS_CXX17
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _EXECUTION_
