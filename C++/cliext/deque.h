// deque stl/clr header
// Copyright (c) Microsoft Corporation. All rights reserved.
#ifndef _CLI_DEQUE_
 #define _CLI_DEQUE_
#include <cliext/iterator>

namespace cliext {
	namespace impl {
//
// TEMPLATE CLASS _Get_sizeof
//
template<typename _Value_t>
	value struct _Get_sizeof
	{	// get size of a type
	static int value()
		{	// return size
		try
			{	// try to determine size of type
			return (System::Runtime::InteropServices::Marshal::
				SizeOf(_Value_t::typeid));
			}
		catch (System::Object^)
			{	// failed, assume int
			return (4);
			}
		}
	};

template<typename _Value_t>
	value struct _Get_sizeof<_Value_t^>
	{	// get size of a handle type
	static int value()
		{	// return size
		return (System::Runtime::InteropServices::Marshal::
			SizeOf(System::IntPtr::typeid));
		}
	};

//
// TEMPLATE CLASS deque_impl
//
template<typename _Value_t,
	bool _Is_ref>
	ref class deque_impl
	:	public _STLCLR IDeque<_Value_t>
	{	// double-ended queue of elements
public:
	// types
	typedef deque_impl<_Value_t, _Is_ref> _Mytype_t;
	typedef _STLCLR IDeque<_Value_t> _Mycont_it;
	typedef System::Collections::Generic::IEnumerable<_Value_t> _Myenum_it;
	typedef cli::array<_Value_t> _Myarray_t;
	typedef cli::array<_Myarray_t^> _Mymap_t;
	typedef _Cont_make_value<_Value_t, _Is_ref> _Mymake_t;

	typedef RandomAccessIterator<_Mytype_t>
		iterator;
	typedef ConstRandomAccessIterator<_Mytype_t>
		const_iterator;
	typedef ReverseRandomAccessIterator<_Mytype_t>
		reverse_iterator;
	typedef ReverseRandomAccessIterator<_Mytype_t>
		const_reverse_iterator;

	typedef int size_type;
	typedef int difference_type;
	typedef _Value_t value_type;
	typedef value_type% reference;
	typedef value_type% const_reference;

	typedef _Mycont_it generic_container;
	typedef value_type generic_value;
	typedef _STLCLR Generic::ContainerRandomAccessIterator<_Value_t>
		generic_iterator;
	typedef _STLCLR Generic::ReverseRandomAccessIterator<_Value_t>
		generic_reverse_iterator;

	// constants
	static const int _Maxsize = MAX_CONTAINER_SIZE;
	static const int _Mapshift = 5;	// minimum map size is 1 << _Mapshift

	// constructors
	deque_impl()
		{	// construct empty deque
		_Buy(0);
		}

	deque_impl(_Mytype_t% _Right)
		{	// construct by copying _Right
		size_type _Count = _Right.size();
		size_type _Idx = 0;

		for (_Buy(_Count); _Idx < _Count; ++_Idx)
			push_back(_Right.at(_Idx));
		}

	explicit deque_impl(size_type _Count)
		{	// construct from _Count * value_type()
		for (_Buy(_Count); 0 < _Count; --_Count)
			push_back(value_type());
		}

	deque_impl(size_type _Count, value_type _Val)
		{	// construct from _Count * _Val
		for (_Buy(_Count); 0 < _Count; --_Count)
			push_back(_Val);
		}

	template<typename _InIt_t>
		deque_impl(_InIt_t _First, _InIt_t _Last)
		{	// construct from [_First, _Last)
		_Construct(_First, _Last, _Iter_category(_First));
		}

	template<typename _InIt_t>
		void _Construct(_InIt_t _Count, _InIt_t _Val,
			_Int_iterator_tag)
		{	// initialize with _Count * _Val
		if (_Count < 0)
			throw gcnew System::ArgumentOutOfRangeException();
		for (_Buy((size_type)_Count); 0 < _Count; --_Count)
			push_back((value_type)_Val);
		}

	template<typename _InIt_t>
		void _Construct(_InIt_t _First, _InIt_t _Last,
			input_iterator_tag)
		{	// initialize with [_First, _Last), input iterators
		for (_Buy(0); _First != _Last; ++_First)
			push_back((value_type)*_First);
		}

	template<typename _InIt_t>
		void _Construct(_InIt_t _First, _InIt_t _Last,
			forward_iterator_tag)
		{	// initialize with [_First, _Last), forward iterators
		size_type _Size = cliext::distance(_First, _Last);

		if (_Size < 0)
			throw gcnew System::ArgumentOutOfRangeException();
		for (_Buy(_Size); 0 < _Size; --_Size, ++_First)
			push_back((value_type)*_First);
		}

	deque_impl(System::Collections::Generic::IEnumerable<_Value_t>^ _Right)
		{	// initialize with enumeration
		_Buy(0);
		for each (value_type _Val in _Right)
			push_back(_Val);
		}

	// destructor
	~deque_impl()
		{	// destroy the object
		clear();
		_Mymap = nullptr;
		_Mybias = 0;
		_Mysize = 0;
		++_Mygen;
		}

	// accessors
	unsigned long get_generation()
		{	// get underlying container generation
		return (_Mygen);
		}

	size_type get_bias(iterator _Where)
		{	// get offset from valid iterator
		if (_Where.container() != this)
			throw gcnew System::ArgumentException();
		return (_Where.get_bias());
		}

	bool valid_bias(size_type _Bias)
		{	// test if _Bias is currently a valid bias
		return ((unsigned int)_Bias - begin_bias()
			<= (unsigned int)size());	// unsigned to handle bias wraparound
		}

	reference at(size_type _Pos)
		{	// subscript mutable sequence with checking
		return (at_bias(begin_bias() + _Pos));
		}

	reference at_bias(size_type _Bias)
		{	// subscript mutable sequence with checking, biased
		if ((unsigned int)size() <= (unsigned int)_Bias - begin_bias())
			throw gcnew System::ArgumentOutOfRangeException();

		int _Blocksize = 1 << _Blockshift;

		_Bias &= (_Mymap->Length << _Blockshift) - 1;
		return (_Mymap[_Bias >> _Blockshift][_Bias & (_Blocksize - 1)]);
		}

	int begin_bias()
		{	// get bias of beginning of current sequence
		return (_Mybias);
		}

	int end_bias()
		{	// get bias of end of current sequence
		return (begin_bias() + size());
		}

	property value_type default[size_type]
		{	// get or set subscripted element
		virtual value_type get(size_type _Pos)
			{	// get _Pos element
			return (at(_Pos));
			}

		virtual void set(size_type _Pos, value_type _Val)
			{	// set _Pos element
			at(_Pos) = _Val;
			}
		};

	property value_type front_item
		{	// get or set first element
		virtual value_type get()
			{	// get first element
			return (front());
			}

		virtual void set(value_type _Val)
			{	// set first element
			front() = _Val;
			}
		};

	property value_type back_item
		{	// get or set last element
		virtual value_type get()
			{	// get last element
			return (back());
			}

		virtual void set(value_type _Val)
			{	// set last element
			back() = _Val;
			}
		};

	reference front()
		{	// get first element of mutable sequence
		if (empty())
			throw gcnew System::NullReferenceException();
		return (at(0));
		}

	reference back()
		{	// get last element of mutable sequence
		if (empty())
			throw gcnew System::NullReferenceException();
		return (at(size() - 1));
		}

	// converters
	_Myarray_t^ to_array()
		{	// convert to array
		_Myarray_t^ _Ans = gcnew _Myarray_t(size());

		for (int _Idx = size(); 0 <= --_Idx; )
			_Ans[_Idx] = _Mymake_t::make_value(at(_Idx));
		return (_Ans);
		}

	// iterator generators
	iterator make_iterator(size_type _Bias)
		{	// return iterator for offset
		return (iterator(this, _Bias));
		}

	iterator begin()
		{	// return iterator for beginning of mutable sequence
		return (make_iterator(begin_bias()));
		}

	iterator end()
		{	// return iterator for end of mutable sequence
		return (make_iterator(end_bias()));
		}

	reverse_iterator rbegin()
		{	// return reverse iterator for beginning of mutable sequence
		return (reverse_iterator(end()));
		}

	reverse_iterator rend()
		{	// return reverse iterator for end of mutable sequence
		return (reverse_iterator(begin()));
		}

	// size controllers
//	void reserve(size_type _Capacity);
//	size_type capacity();

	virtual void resize(size_type _Newsize)
		{	// determine new length, padding with value_type elements
		resize(_Newsize, value_type());
		}

	void resize(size_type _Newsize, value_type _Val)
		{	// determine new length, padding with _Val elements
		if (_Newsize < 0)
			throw gcnew System::ArgumentOutOfRangeException();

		difference_type _Count = _Newsize - size();

		for (; 0 < _Count; --_Count)
			push_back(_Val);
		for (; _Count < 0; ++_Count)
			pop_back();
		}

	size_type size()
		{	// return length of sequence
		return (_Mysize);
		}

	bool empty()
		{	// test if sequence is empty
		return (size() == 0);
		}

	// mutators
	void push_front(value_type _Val)
		{	// insert element at beginning
		int _Blocksize = 1 << _Blockshift;

		if ((_Mybias & (_Blocksize - 1)) == 0
			&& _Mymap->Length <= (_Mysize + _Blocksize) / _Blocksize)
			_Growmap();	// starting new block and no spare block
		--_Mybias;

		size_type _Newoff = _Mybias
			& ((_Mymap->Length << _Blockshift) - 1);
		size_type _Block = _Newoff >> _Blockshift;

		if (_Mymap[_Block] == nullptr)
			_Mymap[_Block] = gcnew _Myarray_t(_Blocksize);
		_Mymap[_Block][_Newoff & (_Blocksize - 1)] =
			_Mymake_t::make_value(_Val);
		++_Mysize;
		++_Mygen;
		}

	void pop_front()
		{	// erase element at beginning
		if (empty())
			throw gcnew System::InvalidOperationException();

		_Mymake_t::unmake_value(front());
		++_Mybias;
		--_Mysize;
		++_Mygen;
		}

	void push_back(value_type _Val)
		{	// insert element at end
		int _Blocksize = 1 << _Blockshift;

		if (((_Mybias + _Mysize) & (_Blocksize - 1)) == 0
			&& _Mymap->Length <= (_Mysize + _Blocksize) / _Blocksize)
			_Growmap();	// starting new block and no spare block

		size_type _Newoff = (_Mybias + _Mysize)
			& ((_Mymap->Length << _Blockshift) - 1);
		size_type _Block = _Newoff >> _Blockshift;

		if (_Mymap[_Block] == nullptr)
			_Mymap[_Block] = gcnew _Myarray_t(_Blocksize);
		_Mymap[_Block][_Newoff & (_Blocksize - 1)] =
			_Mymake_t::make_value(_Val);
		++_Mysize;
		++_Mygen;
		}

	void pop_back()
		{	// erase element at end
		if (empty())
			throw gcnew System::InvalidOperationException();

		_Mymake_t::unmake_value(back());
		--_Mysize;
		++_Mygen;
		}

	void assign(size_type _Count, value_type _Val)
		{	// assign _Count * _Val
		if (_Count < 0)
			throw gcnew System::ArgumentOutOfRangeException();
		clear();
		for (; 0 < _Count; --_Count)
			push_back(_Val);
		}

	void assign(_STLCLR Generic::IInputIterator<_Value_t>^ _First,
		_STLCLR Generic::IInputIterator<_Value_t>^ _Last)
		{	// initialize with [_First, _Last), input iterators
		if (_Iter_container(_First) != this)
			clear();

		size_type _Oldsize = size();

		for (; !_First->equal_to(_Last); _First->next())
			push_back((value_type)_First->get_cref());	// append new stuff
		for (; 0 < _Oldsize; --_Oldsize)
			pop_front();	// erase any leftover old stuff
		}

	void assign(_Myenum_it^ _Right)
		{	// initialize with enumeration
		size_type _Oldsize = size();

		for each (value_type _Val in _Right)
			push_back(_Val);	// append new stuff
		for (; 0 < _Oldsize; --_Oldsize)
			pop_front();	// erase any leftover old stuff
		}

	void assign(System::Collections::IEnumerable^ _Right)
		{	// initialize with enumeration
		size_type _Oldsize = size();

		for each (value_type _Val in _Right)
			push_back(_Val);	// append new stuff
		for (; 0 < _Oldsize; --_Oldsize)
			pop_front();	// erase any leftover old stuff
		}

	iterator insert(iterator _Where, value_type _Val)
		{	// insert _Val at _Where
		return (make_iterator(
			insert_n(get_bias(_Where), 1, _Val)));
		}

	void insert(iterator _Where,
		size_type _Count, value_type _Val)
		{	// insert _Count * _Val at _Where
		insert_n(get_bias(_Where), _Count, _Val);
		}

	void insert(iterator _Where_iter,
		_STLCLR Generic::IInputIterator<_Value_t>^ _First,
		_STLCLR Generic::IInputIterator<_Value_t>^ _Last)
		{	// insert [_First, _Last) at _Where, input iterators
		size_type _Where = get_bias(_Where_iter);

		if (!valid_bias(_Where))
			throw gcnew System::InvalidOperationException();

		if (_First->equal_to(_Last))
			;
		else if (_Where - begin_bias() < end_bias() - _Where)
			{	// add elements near beginning
			size_type _Oldfirst = begin_bias();

			for (; !_First->equal_to(_Last); _First->next())
				push_front((value_type)_First->get_cref());	// prepend flipped

			if (_Oldfirst != _Where)
				{	// insert not at beginning, flip new stuff into place
				reverse_n(_Oldfirst, _Where);
				reverse_n(begin_bias(), _Where);
				}
			else
				reverse_n(begin_bias(), _Oldfirst);	// flip new stuff in place
			}
		else
			{	// add elements near end
			size_type _Oldlast = end_bias();

			for (; !_First->equal_to(_Last); _First->next())
				push_back((value_type)_First->get_cref());	// append
			if (_Oldlast != _Where)
				{	// insert not at end, flip new stuff into place
				reverse_n(_Where, _Oldlast);
				reverse_n(_Oldlast, end_bias());
				reverse_n(_Where, end_bias());
				}
			}
		}

	void insert(iterator _Where_iter,
		System::Collections::Generic::IEnumerable<_Value_t>^ _Right)
		{	// insert enumeration at _Where, possibly from this container
		size_type _Where = get_bias(_Where_iter);

		if (!valid_bias(_Where))
			throw gcnew System::InvalidOperationException();

		if (_Where - begin_bias() < end_bias() - _Where)
			{	// add elements near beginning
			size_type _Oldfirst = begin_bias();

			for each (value_type _Val in _Right)
				push_front(_Val);	// flipped
			if (_Oldfirst != _Where)
				{	// insert not at beginning, flip new stuff into place
				reverse_n(_Oldfirst, _Where);
				reverse_n(begin_bias(), _Where);
				}
			else
				reverse_n(begin_bias(), _Oldfirst);	// flip new stuff in place
			}
		else
			{	// add elements near end
			size_type _Oldlast = end_bias();

			for each (value_type _Val in _Right)
				push_back(_Val);	// not flipped
			if (_Oldlast != _Where)
				{	// insert not at end, flip new stuff into place
				reverse_n(_Where, _Oldlast);
				reverse_n(_Oldlast, end_bias());
				reverse_n(_Where, end_bias());
				}
			}
		}

	void insert(iterator _Where_iter,
		System::Collections::IEnumerable^ _Right)
		{	// insert enumeration at _Where, possibly from this container
		size_type _Where = get_bias(_Where_iter);

		if (!valid_bias(_Where))
			throw gcnew System::InvalidOperationException();

		if (_Where - begin_bias() < end_bias() - _Where)
			{	// add elements near beginning
			size_type _Oldfirst = begin_bias();

			for each (value_type _Val in _Right)
				push_front(_Val);	// flipped
			if (_Oldfirst != _Where)
				{	// insert not at beginning, flip new stuff into place
				reverse_n(_Oldfirst, _Where);
				reverse_n(begin_bias(), _Where);
				}
			else
				reverse_n(begin_bias(), _Oldfirst);	// flip new stuff in place
			}
		else
			{	// add elements near end
			size_type _Oldlast = end_bias();

			for each (value_type _Val in _Right)
				push_back(_Val);	// not flipped
			if (_Oldlast != _Where)
				{	// insert not at end, flip new stuff into place
				reverse_n(_Where, _Oldlast);
				reverse_n(_Oldlast, end_bias());
				reverse_n(_Where, end_bias());
				}
			}
		}

	size_type insert_n(size_type _Where,
		size_type _Count, value_type _Val)
		{	// insert _Count * _Val at _Where
		if (_Count < 0 || !valid_bias(_Where))
			throw gcnew System::ArgumentOutOfRangeException();

		if (_Count == 0)
			return (_Where);
		else if (_Where - begin_bias() < end_bias() - _Where)
			{	// add elements near beginning
			size_type _Oldfirst = begin_bias();

			for (; 0 < _Count; --_Count)
				push_front(_Val);
			if (_Oldfirst != _Where)
				{	// insert not at beginning, flip new stuff into place
				reverse_n(_Oldfirst, _Where);
				reverse_n(begin_bias(), _Where);
				}
			return (_Where - 1);
			}
		else
			{	// add elements near end
			size_type _Oldlast = end_bias();
			size_type _Ans = _Where + _Count - 1;

			for (; 0 < _Count; --_Count)
				push_back(_Val);
			if (_Oldlast != _Where)
				{	// insert not at end, flip new stuff into place
				reverse_n(_Where, _Oldlast);
				reverse_n(_Where, end_bias());
				}
			return (_Ans);
			}
		}

	iterator erase(iterator _Where)
		{	// erase element at _Where
		size_type _Bias = get_bias(_Where);

		return (make_iterator(erase_n(_Bias, _Bias + 1)));
		}

	iterator erase(iterator _First, iterator _Last)
		{	// erase [_First, _Last)
		return (make_iterator(
			erase_n(get_bias(_First), get_bias(_Last))));
		}

	size_type erase_n(size_type _First, size_type _Last)
		{	// erase [_First, _Last)
		if (!valid_bias(_First)
			|| !valid_bias(_Last)
			|| _Last < _First)
			throw gcnew System::InvalidOperationException();

		if (_First == _Last)
			return (_First);
		else if (_First - begin_bias() < end_bias() - _Last)
			{	// erase finite sequence closer to front
			size_type _Count = _First - begin_bias();
			size_type _Stride = _Last - _First;

			for (_First = _Last - 1; 0 < _Count; --_Count, --_First)
				at_bias(_First) = at_bias(_First - _Stride);	// copy up
			for (; 0 < _Stride; --_Stride)
				pop_front();
			return (_Last);
			}
		else
			{	// erase finite sequence closer to back
			size_type _Count = end_bias() - _Last;
			size_type _Stride = _Last - _First;

			for (; 0 < _Count; --_Count, ++_Last)
				at_bias(_Last - _Stride) = at_bias(_Last);	// copy down
			for (; 0 < _Stride; --_Stride)
				pop_back();
			return (_First);
			}
		}

	void reverse_n(size_type _First, size_type _Last)
		{	// reverse a subrange
		if (!valid_bias(_First)
			|| !valid_bias(_Last)
			|| _Last < _First)
			throw gcnew System::InvalidOperationException();

		for (; _First != _Last && _First != --_Last; ++_First)
			{	// swap distinct _First and _Last
			value_type _Temp = at_bias(_First);

			at_bias(_First) = at_bias(_Last);
			at_bias(_Last) = _Temp;
			}
		}

	void clear()
		{	// erase all
		for (; !empty(); )
			pop_back();
		}

	void swap(_Mytype_t% _Right)
		{	// exchange contents with _Right
		if ((Object^)this != %_Right)
			{	// worth doing, swap
			_Mymap_t^ _Tmap = _Mymap;
			size_type _Tbias = _Mybias;
			size_type _Tsize = _Mysize;

			_Mymap = _Right._Mymap;
			_Right._Mymap = _Tmap;

			_Mybias = _Right._Mybias;
			_Right._Mybias = _Tbias;

			_Mysize = _Right._Mysize;
			_Right._Mysize = _Tsize;

			++_Mygen;
			++_Right._Mygen;
			}
		}

	// operators
	deque_impl% operator=(deque_impl% _Right)
		{	// assign
		if ((Object^)this != %_Right)
			{	// worth assigning, do it
			clear();
			for (size_type _Idx = 0; _Idx < _Right.size(); ++_Idx)
				push_back(_Right.at(_Idx));
			}
		return (*this);
		}

_STLCLR_FIELD_ACCESS:
	void _Buy(size_type _Capacity)
		{	// allocate map with _Capacity elements
		size_type _Valsize = _Get_sizeof<value_type>::value();

		_Blockshift = _Valsize <= 1 ? 6
			: _Valsize <= 2 ? 5
			: _Valsize <= 4 ? 4
			: _Valsize <= 8 ? 3
			: _Valsize <= 16 ? 2
			: _Valsize <= 32 ? 1
			: 0;	// elements per block is 1 << _Blockshift

		_Mymap = nullptr;
		_Mybias = 0;
		_Mysize = 0;
		_Mygen = 0;
		if (_Capacity < 0)
			throw gcnew System::ArgumentOutOfRangeException();

		size_type _Mapsize = 1 << _Mapshift;
		size_type _Dequesize = _Mapsize << _Blockshift;

		for (; _Dequesize < _Capacity && _Maxsize - _Dequesize < _Dequesize;
			_Mapsize <<= 1, _Dequesize <<= 1)
			;	// double map size until big enough, but not too big
		_Mymap = gcnew _Mymap_t(_Mapsize);
		}

	void _Growmap()
		{	// grow map by doubling its size
		if (_Maxsize - (_Mymap->Length << _Blockshift)
			< (_Mymap->Length << _Blockshift))	// can't double map size
			throw gcnew System::ArgumentOutOfRangeException();

		_Mymap_t^ _Newmap = gcnew _Mymap_t(2 * _Mymap->Length);
		size_type _Count = _Mymap->Length;
		size_type _Block = _Mybias >> _Blockshift;

		for (; 0 < _Count; --_Count, ++_Block)
			_Newmap[_Block % _Newmap->Length] =
				_Mymap[_Block % _Mymap->Length];
		_Mymap = _Newmap;
		}

	// data members
	_Mymap_t^ _Mymap;	// array of array of _Value_t
	int _Blockshift;	// 2 ^ _Blockshift elements per block
	int _Mybias;		// offset of current element zero
	size_type _Mysize;	// number of active elements
	unsigned long _Mygen;	// current change generation

	// interfaces
public:
	virtual System::Object^ Clone()
		{	// clone the deque
		return (gcnew deque_impl(*this));
		}

private:
	property size_type Count
		{	// element count
		virtual size_type get() sealed
			= System::Collections::ICollection::Count::get
			{	// get element count
			return (size());
			}
		};

	property bool IsSynchronized
		{	// synchronized status
		virtual bool get() sealed
			= System::Collections::ICollection::IsSynchronized::get
			{	// test if synchronized
			return (false);
			}
		};

	property System::Object^ SyncRoot
		{	// synchronizer
		virtual System::Object^ get() sealed
			= System::Collections::ICollection::SyncRoot::get
			{	// get synchronizer
			return (this);
			}
		};

	virtual void CopyTo(System::Array^ _Dest_arg, int _First) sealed
		= System::Collections::ICollection::CopyTo
		{	// copy to _Dest_arg, beginning at _First
		cli::array<System::Object^>^ _Dest =
			(cli::array<System::Object ^>^)_Dest_arg;
		for (int _Idx = size(); 0 <= --_Idx; )
			{	// copy back to front
			_Dest[_First + _Idx] = _Mymake_t::make_value(at(_Idx));
			}
		}

	virtual System::Collections::IEnumerator^ GetEnumerator() sealed
		= System::Collections::IEnumerable::GetEnumerator
		{	// get enumerator for the container
		return (gcnew _STLCLR DequeEnumerator<_Value_t>(this, begin_bias()));
		}

	virtual unsigned long get_generation_virtual() sealed
		= _Mycont_it::get_generation
		{	// get underlying container generation
		return (get_generation());
		}

	virtual bool valid_bias_virtual(size_type _Bias) sealed
		= _Mycont_it::valid_bias
		{	// test if _Bias is currently a valid bias
		return (valid_bias(_Bias));
		}

	virtual reference at_virtual(size_type _Pos) sealed
		= _Mycont_it::at
		{	// subscript mutable sequence with checking
		return (at(_Pos));
		}

	virtual reference at_bias_virtual(size_type _Bias) sealed
		= _Mycont_it::at_bias
		{	// subscript mutable sequence with checking, biased
		return (at_bias(_Bias));
		}

	virtual int begin_bias_virtual() sealed
		= _Mycont_it::begin_bias
		{	// get bias of beginning of current sequence
		return (begin_bias());
		}

	virtual int end_bias_virtual() sealed
		= _Mycont_it::end_bias
		{	// get bias of end of current sequence
		return (end_bias());
		}

	virtual reference front_virtual() sealed
		= _Mycont_it::front
		{	// get first element of mutable sequence
		return (front());
		}

	virtual reference back_virtual() sealed
		= _Mycont_it::back
		{	// get last element of mutable sequence
		return (back());
		}

	// iterator generators
	virtual generic_iterator begin_virtual() sealed
		= _Mycont_it::begin
		{	// return iterator for beginning of mutable sequence
		return (begin());
		}
	virtual generic_iterator end_virtual() sealed
		= _Mycont_it::end
		{	// return iterator for end of mutable sequence
		return (end());
		}

	virtual generic_reverse_iterator rbegin_virtual() sealed
		= _Mycont_it::rbegin
		{	// return reverse iterator for beginning of mutable sequence
		return (generic_reverse_iterator(end()));
		}

	virtual generic_reverse_iterator rend_virtual() sealed
		= _Mycont_it::rend
		{	// return reverse iterator for end of mutable sequence
		return (generic_reverse_iterator(begin()));
		}

	// size controllers
//	virtual void reserve_virtual(size_type _Capacity);
//	virtual size_type capacity_virtual();

	virtual void resize_virtual(size_type _Newsize) sealed
		= _Mycont_it::resize
		{	// determine new length, padding with value_type elements
		resize(_Newsize);
		}

	virtual void resize_virtual(size_type _Newsize, value_type _Val) sealed
		= _Mycont_it::resize
		{	// determine new length, padding with _Val elements
		resize(_Newsize, _Val);
		}

	virtual size_type size_virtual() sealed
		= _Mycont_it::size
		{	// return length of sequence
		return (size());
		}

	virtual bool empty_virtual() sealed
		= _Mycont_it::empty
		{	// test if sequence is empty
		return (empty());
		}

	// mutators
	virtual void push_front_virtual(value_type _Val) sealed
		= _Mycont_it::push_front
		{	// insert element at end
		push_front(_Val);
		}

	virtual void pop_front_virtual() sealed
		= _Mycont_it::pop_front
		{	// erase element at end
		pop_front();
		}

	virtual void push_back_virtual(value_type _Val) sealed
		= _Mycont_it::push_back
		{	// insert element at end
		push_back(_Val);
		}

	virtual void pop_back_virtual() sealed
		= _Mycont_it::pop_back
		{	// erase element at end
		pop_back();
		}

	virtual void assign_virtual(size_type _Count, value_type _Val) sealed
		= _Mycont_it::assign
		{	// assign _Count * _Val
		assign(_Count, _Val);
		}

	virtual void assign_virtual(
		_STLCLR Generic::IInputIterator<_Value_t>^ _First,
		_STLCLR Generic::IInputIterator<_Value_t>^ _Last) sealed
		= _Mycont_it::assign
		{	// initialize with [_First, _Last), input iterators
		assign(_First, _Last);
		}

	virtual void assign_virtual(
		System::Collections::IEnumerable^ _Right) sealed
		= _Mycont_it::assign
		{	// initialize with enumeration
		assign(_Right);
		}

	virtual generic_iterator insert_virtual(generic_iterator _Where,
		value_type _Val) sealed
		= _Mycont_it::insert
		{	// insert _Val at _Where
		return (insert(iterator(_Where), _Val));
		}

	virtual void insert_virtual(generic_iterator _Where,
		size_type _Count, value_type _Val) sealed
			= _Mycont_it::insert
		{	// insert _Count * _Val at _Where
		return (insert(iterator(_Where), _Count, _Val));
		}

	virtual void insert_virtual(generic_iterator _Where_iter,
		_STLCLR Generic::IInputIterator<_Value_t>^ _First,
		_STLCLR Generic::IInputIterator<_Value_t>^ _Last) sealed
			= _Mycont_it::insert
		{	// insert [_First, _Last) at _Where, input iterators
		insert(iterator(_Where_iter), _First, _Last);
		}

	virtual void insert_virtual(generic_iterator _Where_iter,
		System::Collections::IEnumerable^ _Right) sealed
			= _Mycont_it::insert
		{	// insert enumeration at _Where, possibly from this container
		insert(iterator(_Where_iter), _Right);
		}

	virtual generic_iterator erase_virtual(generic_iterator _Where) sealed
		= _Mycont_it::erase
		{	// erase element at _Where
		return (erase(iterator(_Where)));
		}

	virtual generic_iterator erase_virtual(generic_iterator _First,
		generic_iterator _Last) sealed
		= _Mycont_it::erase
		{	// erase [_First, _Last)
		return (erase(iterator(_First), iterator(_Last)));
		}

	virtual void clear_virtual() sealed
		= _Mycont_it::clear
		{	// erase all
		clear();
		}

	virtual void swap_virtual(_Mycont_it^ _Right) sealed
		= _Mycont_it::swap
		{	// exchange contents with _Right
		swap(*(_Mytype_t^)_Right);
		}
	};

//
// TEMPLATE REF CLASS deque_base
//
template<typename _Value_t,
	bool _Is_ref>
	ref class deque_base
	:	public deque_impl<_Value_t, _Is_ref>,
			System::Collections::Generic::ICollection<_Value_t>,
			System::Collections::Generic::IEnumerable<_Value_t>,
			System::Collections::Generic::IList<_Value_t>
	{	// double-ended queue of value/handle elements
public:
	// types
	typedef deque_base<_Value_t, _Is_ref> _Mytype_t;
	typedef deque_impl<_Value_t, _Is_ref> _Mybase_t;
//	typedef _STLCLR IDeque<_Value_t> _Mycont_it;
	typedef _Cont_make_value<_Value_t, _Is_ref> _Mymake_t;

//	typedef int size_type;
//	typedef int difference_type;
//	typedef _Value_t value_type;
//	typedef value_type% reference;
//	typedef value_type% const_reference;

//	typedef _Mycont_it generic_container;
//	typedef value_type generic_value;

	// basics
	deque_base()
		:	_Mybase_t()
		{	// construct default
		}

	deque_base(deque_base% _Right)
		:	_Mybase_t(_Right)
		{	// construct by copying a deque
		}

	deque_base% operator=(deque_base% _Right)
		{	// assign
		_Mybase_t::operator=(_Right);
		return (*this);
		}

	operator _Mycont_it^()
		{	// convert to interface
		return (this);
		}

	// constructors
	explicit deque_base(size_type _Count)
		:	_Mybase_t(_Count)
		{	// construct from _Count * value_type()
		}

	deque_base(size_type _Count, value_type _Val)
		:	_Mybase_t(_Count, _Val)
		{	// construct from _Count * _Val
		}

	template<typename _InIt_t>
		deque_base(_InIt_t _First, _InIt_t _Last)
		:	_Mybase_t(_First, _Last)
		{	// construct from [_First, _Last)
		}

	deque_base(_Myenum_it^ _Right)
		:	_Mybase_t(_Right)
		{	// initialize with enumeration
		}

	// mutators
	template<typename _InIt_t>
		void assign(_InIt_t _First, _InIt_t _Last)
		{	// assign [_First, _Last)
		_Assign(_First, _Last, _Iter_category(_First));
		}

	template<typename _InIt_t>
		void _Assign(_InIt_t _Count_arg, _InIt_t _Val,
			_Int_iterator_tag%)
		{	// assign _Count * _Val
		value_type _Count = (value_type)_Count_arg;

		if (_Count < 0)
			throw gcnew System::ArgumentOutOfRangeException();
		clear();
		for (; 0 < _Count; --_Count)
			push_back((value_type)_Val);
		}

	template<typename _InIt_t>
		void _Assign(_InIt_t _First, _InIt_t _Last,
			input_iterator_tag%)
		{	// initialize with [_First, _Last), input iterators
		if (_Iter_container(_First) != this)
			clear();

		size_type _Oldsize = size();

		for (; _First != _Last; ++_First)
			push_back((value_type)*_First);	// append new stuff
		for (; 0 < _Oldsize; --_Oldsize)
			pop_front();	// erase any leftover old stuff
		}

	template<typename _InIt_t>
		void _Assign(_InIt_t _First, _InIt_t _Last,
			random_access_iterator_tag%)
		{	// initialize with [_First, _Last), input iterators
		if (_Last < _First)
			throw gcnew System::ArgumentOutOfRangeException();

		if (_Iter_container(_First) != this)
			clear();

		size_type _Oldsize = size();

		for (; _First != _Last; ++_First)
			push_back((value_type)*_First);	// append new stuff
		for (; 0 < _Oldsize; --_Oldsize)
			pop_front();	// erase any leftover old stuff
		}

	template<typename _InIt_t>
		void insert(iterator _Where, _InIt_t _First, _InIt_t _Last)
		{	// insert [_First, _Last) at _Where
		_Insert(get_bias(_Where), _First, _Last,
			_Iter_category(_First));
		}

	template<typename _InIt_t>
		void _Insert(size_type _Where, _InIt_t _First, _InIt_t _Last,
			_Int_iterator_tag%)
		{	// insert _Count * _Val at _Where
		insert_n(_Where, (size_type)_First, (value_type)_Last);
		}

	template<typename _InIt_t>
		void _Insert(size_type _Where, _InIt_t _First, _InIt_t _Last,
			input_iterator_tag%)
		{	// insert [_First, _Last) at _Where, input iterators
		if (!valid_bias(_Where))
			throw gcnew System::InvalidOperationException();

		if (_First == _Last)
			;
		else if (_Where - begin_bias() < end_bias() - _Where)
			{	// add elements near beginning
			size_type _Oldfirst = begin_bias();

			for (; _First != _Last; ++_First)
				push_front((value_type)*_First);	// prepend flipped
			if (_Oldfirst != _Where)
				{	// insert not at beginning, flip new stuff into place
				reverse_n(_Oldfirst, _Where);
				reverse_n(begin_bias(), _Where);
				}
			else
				reverse_n(begin_bias(), _Oldfirst);	// flip new stuff in place
			}
		else
			{	// add elements near end
			size_type _Oldlast = end_bias();

			for (; _First != _Last; ++_First)
				push_back((value_type)*_First);	// append
			if (_Oldlast != _Where)
				{	// insert not at end, flip new stuff into place
				reverse_n(_Where, _Oldlast);
				reverse_n(_Oldlast, end_bias());
				reverse_n(_Where, end_bias());
				}
			}
		}

	// interfaces
private:
	property size_type Count_generic
		{	// element count
		virtual size_type get() sealed
			= System::Collections::Generic::ICollection<_Value_t>::Count::get
			{	// get element count
			return (size());
			}
		};

	property bool IsReadOnly
		{	// test if read only
		virtual bool get() sealed
			= System::Collections::Generic::ICollection<_Value_t>
				::IsReadOnly::get
			{	// test if read only
			return (false);
			}
		};

	virtual void CopyTo(_Myarray_t^ _Dest, int _First) sealed
		= System::Collections::Generic::ICollection<_Value_t>::CopyTo
		{	// copy to _Dest, beginning at _First
		for (int _Idx = size(); 0 <= --_Idx; )
			{	// copy back to front
			_Dest[_First + _Idx] = _Mymake_t::make_value(at(_Idx));
			}
		}

	virtual System::Collections::Generic::IEnumerator<_Value_t>^
		GetEnumerator() sealed
		= System::Collections::Generic::IEnumerable<_Value_t>::GetEnumerator
		{	// get enumerator for the container
		return (gcnew _STLCLR DequeEnumerator<_Value_t>(this, begin_bias()));
		}

	virtual void Add(value_type _Val) sealed
		= System::Collections::Generic::ICollection<_Value_t>::Add
		{	// add element with value _Val
		insert_n(begin_bias() + size(), 1, _Val);
		}

	virtual void Clear() sealed
		= System::Collections::Generic::ICollection<_Value_t>::Clear
		{	// erase all elements
		clear();
		}

	virtual bool Contains(value_type _Val) sealed
		= System::Collections::Generic::ICollection<_Value_t>::Contains
		{	// search for element matching value _Val
		for (size_type _Idx = 0; _Idx != size(); ++_Idx)
			if (((System::Object^)_Val)->Equals(
				(System::Object^)at(_Idx)))
				return (true);
		return (false);
		}

	virtual bool Remove(value_type _Val) sealed
		= System::Collections::Generic::ICollection<_Value_t>::Remove
		{	// remove first element matching value _Val
		for (size_type _Idx = 0; _Idx != size(); ++_Idx)
			if (((System::Object^)_Val)->Equals(
				(System::Object^)at(_Idx)))
				{	// found a match, remove it
				size_type _Bias = begin_bias() + _Idx;

				erase_n(_Bias, _Bias + 1);
				return (true);
				}
		return (false);
		}

	virtual int IndexOf(value_type _Val) sealed
		= System::Collections::Generic::IList<_Value_t>::IndexOf
		{	// find index of element that matches _Val
		int _Idx = 0;

		for (; _Idx < size(); ++_Idx)
			if (at(_Idx) == _Val)
				return (_Idx);
		return (-1);
		}

	virtual void Insert(int _Idx, value_type _Val) sealed
		= System::Collections::Generic::IList<_Value_t>::Insert
		{	// insert _Val before _Idx
		insert_n(begin_bias() + _Idx, 1, _Val);
		}

	virtual void RemoveAt(int _Idx) sealed
		= System::Collections::Generic::IList<_Value_t>::RemoveAt
		{	// erase element at _Idx
		erase_n(begin_bias() + _Idx, begin_bias() + _Idx + 1);
		}
	};

//
// TEMPLATE CLASS deque_select
//
template<typename _Value_t,
	bool _Is_ref>
	ref class deque_select
	:	public deque_base<_Value_t, _Is_ref>
	{	// double-ended queue of elements
public:
	// types
	typedef _Value_t _Gvalue_t;

	typedef deque_select<_Value_t, _Is_ref> _Mytype_t;
	typedef deque_base<_Gvalue_t, _Is_ref> _Mybase_t;
//	typedef _STLCLR IDeque<_Gvalue_t> _Mycont_it;
//	typedef System::Collections::Generic::IEnumerable<_Gvalue_t> _Myenum_it;

//	typedef int size_type;
//	typedef int difference_type;
	typedef _Value_t value_type;
	typedef value_type% reference;
	typedef value_type% const_reference;

//	typedef _Mycont_it generic_container;
//	typedef typename _Mybase_t::value_type generic_value;

	// basics
	deque_select()
		:	_Mybase_t()
		{	// construct default
		}

	deque_select(deque_select% _Right)
		:	_Mybase_t(_Right)
		{	// construct by copying a deque
		}

	deque_select% operator=(deque_select% _Right)
		{	// assign
		_Mybase_t::operator=(_Right);
		return (*this);
		}

	// constructors
	explicit deque_select(size_type _Count)
		:	_Mybase_t(_Count)
		{	// construct from _Count * value_type()
		}

	deque_select(size_type _Count, value_type _Val)
		:	_Mybase_t(_Count, _Val)
		{	// construct from _Count * _Val
		}

	template<typename _InIt_t>
		deque_select(_InIt_t _First, _InIt_t _Last)
		:	_Mybase_t(_First, _Last)
		{	// construct from [_First, _Last)
		}

	deque_select(_Myenum_it^ _Right)
		:	_Mybase_t(_Right)
		{	// initialize with enumeration
		}
	};

//
// TEMPLATE CLASS deque_select: _Value_t REF SPECIALIZATION
//
template<typename _Value_t>
	ref class deque_select<_Value_t, true>
	:	public deque_base<_Value_t^, true>
	{	// double-ended queue of elements
public:
	// types
	typedef _Value_t^ _Gvalue_t;

	typedef deque_select<_Value_t, true> _Mytype_t;
	typedef deque_base<_Gvalue_t, true> _Mybase_t;
//	typedef _STLCLR IDeque<_Gvalue_t> _Mycont_it;
//	typedef System::Collections::Generic::IEnumerable<_Gvalue_t> _Myenum_it;

//	typedef int size_type;
//	typedef int difference_type;
	typedef _Value_t value_type;
	typedef value_type% reference;
	typedef value_type% const_reference;

//	typedef _Mycont_it generic_container;
//	typedef typename _Mybase_t::value_type generic_value;

	// basics
	deque_select()
		:	_Mybase_t()
		{	// construct default
		}

	deque_select(deque_select% _Right)
		:	_Mybase_t(_Right)
		{	// construct by copying a deque
		}

	deque_select% operator=(deque_select% _Right)
		{	// assign
		_Mybase_t::operator=(_Right);
		return (*this);
		}

	// constructors
	explicit deque_select(size_type _Count)
		{	// construct from _Count * value_type()
		resize(_Count);
		}

	deque_select(size_type _Count, value_type _Val)
		{	// construct from _Count * _Val
		resize(_Count, _Val);
		}

	template<typename _InIt_t>
		deque_select(_InIt_t _First, _InIt_t _Last)
		:	_Mybase_t(_First, _Last)
		{	// construct from [_First, _Last)
		}

	deque_select(_Myenum_it^ _Right)
		:	_Mybase_t(_Right)
		{	// initialize with enumeration
		}

	// size controllers
	virtual void resize(size_type _Newsize) override
		{	// determine new length, padding with value_type elements
		value_type _Val;

		_Mybase_t::resize(_Newsize, %_Val);
		}

	void resize(size_type _Newsize, value_type _Val)
		{	// determine new length, padding with _Val elements
		_Mybase_t::resize(_Newsize, %_Val);
		}

	// accessors
	reference at(size_type _Pos) new
		{	// subscript mutable sequence with checking
		return (*_Mybase_t::at(_Pos));
		}

	property value_type default[size_type]
		{	// get or set subscripted element
		virtual value_type get(size_type _Pos)
			{	// get _Pos element
			return (*_Mybase_t::at(_Pos));
			}

		virtual void set(size_type _Pos, value_type _Val)
			{	// set _Pos element
			_Mybase_t::at(_Pos) = gcnew value_type(_Val);
			}
		};

	property value_type front_item
		{	// get or set first element
		virtual value_type get()
			{	// get first element
			return (*_Mybase_t::front_item);
			}

		virtual void set(value_type _Val)
			{	// set first element
			_Mybase_t::front_item = gcnew value_type(_Val);
			}
		};

	property value_type back_item
		{	// get or set last element
		virtual value_type get()
			{	// get last element
			return (*_Mybase_t::back_item);
			}

		virtual void set(value_type _Val)
			{	// set last element
			_Mybase_t::back_item = gcnew value_type(_Val);
			}
		};

	reference front() new
		{	// get first element of mutable sequence
		return (*_Mybase_t::front());
		}

	reference back() new
		{	// get last element of mutable sequence
		return (*_Mybase_t::back());
		}

	// mutators
	void push_front(value_type _Val)
		{	// insert element at beginning
		_Mybase_t::push_front(%_Val);
		}

	void push_back(value_type _Val)
		{	// insert element at end
		_Mybase_t::push_back(%_Val);
		}

	void assign(size_type _Count, value_type _Val)
		{	// assign _Count * _Val
		_Mybase_t::assign(_Count, %_Val);
		}

	iterator insert(iterator _Where, value_type _Val)
		{	// insert _Val at _Where
		return (_Mybase_t::insert(_Where, %_Val));
		}

	void insert(iterator _Where,
		size_type _Count, value_type _Val)
		{	// insert _Count * _Val at _Where
		return (_Mybase_t::insert(_Where, _Count, %_Val));
		}
	};
	}	// namespace cliext::impl

//
// TEMPLATE CLASS deque
//
template<typename _Value_t>
	ref class deque
	:	public impl::deque_select<_Value_t,
		__is_ref_class(typename _Dehandle<_Value_t>::type)
			&& !is_handle<_Value_t>::value>
	{	// double-ended queue of elements
public:
	// types
	typedef deque<_Value_t> _Mytype_t;
	typedef impl::deque_select<_Value_t,
		__is_ref_class(typename _Dehandle<_Value_t>::type)
			&& !is_handle<_Value_t>::value> _Mybase_t;
//	typedef _STLCLR IDeque<_Value_t> _Mycont_it;

//	typedef int size_type;
//	typedef int difference_type;
//	typedef _Value_t value_type;
//	typedef value_type% reference;
//	typedef value_type% const_reference;

//	typedef _Mycont_it generic_container;
//	typedef value_type generic_value;

	// basics
	deque()
		:	_Mybase_t()
		{	// construct default
		}

	deque(deque% _Right)
		:	_Mybase_t((_Mybase_t%)_Right)
		{	// construct by copying a deque
		}

	deque(deque^ _Right)
		:	_Mybase_t((_Mybase_t%)*_Right)
		{	// construct by copying a deque
		}

	deque% operator=(deque% _Right)
		{	// assign
		_Mybase_t::operator=(_Right);
		return (*this);
		}

	deque% operator=(deque^ _Right)
		{	// assign
		_Mybase_t::operator=(*_Right);
		return (*this);
		}

	// constructors
	explicit deque(size_type _Count)
		:	_Mybase_t(_Count)
		{	// construct from _Count * value_type()
		}

	deque(size_type _Count, value_type _Val)
		:	_Mybase_t(_Count, _Val)
		{	// construct from _Count * _Val
		}

	template<typename _InIt_t>
		deque(_InIt_t _First, _InIt_t _Last)
		:	_Mybase_t(_First, _Last)
		{	// construct from [_First, _Last)
		}

	deque(_Myenum_it^ _Right)
		:	_Mybase_t(_Right)
		{	// initialize with enumeration
		}

	// mutators
	void swap(deque% _Right)
		{	// exchange contents with _Right
		_Mybase_t::swap(_Right);
		}

	// interfaces
	virtual System::Object^ Clone() override
		{	// clone the vector
		return (gcnew _Mytype_t(*this));
		}
	};

//
// TEMPLATE COMPARISONS
//
template<typename _Value_t> inline
	bool operator==(deque<_Value_t>% _Left,
		deque<_Value_t>% _Right)
	{	// test if _Left == _Right
	deque<_Value_t>::size_type _Size = _Left.size();

	if (_Size != _Right.size())
		return (false);
	else
		{	// same length, compare elements
		for (int _Idx = 0; _Idx != _Size; ++_Idx)
			if (_Left.at(_Idx) != _Right.at(_Idx))
				return (false);
		return (true);
		}
	}

template<typename _Value_t> inline
	bool operator!=(deque<_Value_t>% _Left,
		deque<_Value_t>% _Right)
	{	// test if _Left != _Right
	return (!(_Left == _Right));
	}

template<typename _Value_t> inline
	bool operator<(deque<_Value_t>% _Left,
		deque<_Value_t>% _Right)
	{	// test if _Left < _Right
	deque<_Value_t>::size_type _Idx = 0;

	for (; _Idx != _Left.size() && _Idx != _Right.size(); ++_Idx)
		if (_Left.at(_Idx) < _Right.at(_Idx))
			return (true);
		else if (_Right.at(_Idx) < _Left.at(_Idx))
			return (false);
	return (_Idx == _Left.size() && _Idx != _Right.size());
	}

template<typename _Value_t> inline
	bool operator>=(deque<_Value_t>% _Left,
		deque<_Value_t>% _Right)
	{	// test if _Left >= _Right
	return (!(_Left < _Right));
	}

template<typename _Value_t> inline
	bool operator>(deque<_Value_t>% _Left,
		deque<_Value_t>% _Right)
	{	// test if _Left > _Right
	return (_Right < _Left);
	}

template<typename _Value_t> inline
	bool operator<=(deque<_Value_t>% _Left,
		deque<_Value_t>% _Right)
	{	// test if _Left <= _Right
	return (!(_Right < _Left));
	}

//
// TEMPLATE FUNCTION std::swap
//
template<typename _Value_t> inline
	void swap(deque<_Value_t>% _Left,
		deque<_Value_t>% _Right)
	{	// swap two deques
	_Left.swap(_Right);
	}
}	// namespace cliext
#endif // _CLI_DEQUE_
