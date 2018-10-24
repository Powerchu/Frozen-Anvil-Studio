/* HEADER *********************************************************************************/
/*!
\file	MagicArray.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Run-time auto resizing array. Does not invalidate pointers to elements!

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _MAGIC_ARRAY_H_
#define _MAGIC_ARRAY_H_

#include "Globals.h"
#include "Utility/Meta.h"
#include "Utility/Utility.h"
#include "Math/MathUtility.h"

#include "DataStructure/Array.h"
#include "Allocator/DefaultAlloc.h"

#include <intrin.h>			// _BitScanForward64
#include <initializer_list> // init-list

#pragma intrinsic(_BitScanForward64)



template <typename, typename>
class _DLL_EXPORT_ONLY MagicArray;

namespace Ctor
{
	template <typename T, unsigned BLOCK_SIZE = 64, unsigned MAX_BLOCKS = 16, typename Alloc = Dystopia::DefaultAllocator<T[]>>
	struct _DLL_EXPORT_ONLY MagicArrayBuilder
	{
		static_assert(Ut::IsPowerOf2(BLOCK_SIZE), "Block Size must be a power of 2");

		template <unsigned NEW_SIZE>
		using SetBlockSize = MagicArrayBuilder<T, NEW_SIZE, MAX_BLOCKS>;

		template <unsigned NEW_SIZE>
		using SetBlockLimit = MagicArrayBuilder<T, BLOCK_SIZE, NEW_SIZE>;

		template <template <typename> class NewAlloc>
		using SetAllocator = NewAlloc<T[]>;

		using type = MagicArray<T, MagicArrayBuilder>;


	private:
		static constexpr unsigned blk_sz  = BLOCK_SIZE;
		static constexpr unsigned blk_max = MAX_BLOCKS;
		static constexpr unsigned shift   = Math::Log<BLOCK_SIZE>::value;
		static constexpr unsigned offset  = BLOCK_SIZE - 1;

		using Alloc_t = Alloc;

		friend class MagicArray<T, MagicArrayBuilder>;

		MagicArrayBuilder() = delete;
		MagicArrayBuilder(MagicArrayBuilder&&) = delete;
		MagicArrayBuilder(const MagicArrayBuilder&) = delete;
	};
}

template <typename T, typename Params = Ctor::MagicArrayBuilder<T>>
class _DLL_EXPORT_ONLY MagicArray
{
	struct Iterator;

public:
	// ==================================== CONTAINER DEFINES ==================================== // 

	using Itor_t = Iterator;
	using Val_t  = T;
	using Ptr_t  = T *;
	using Sz_t   = unsigned long long;


	// ====================================== CONSTRUCTORS ======================================= // 

	MagicArray(void) noexcept;
	MagicArray(const MagicArray& _other) noexcept;
	MagicArray(MagicArray&& _other) noexcept;

	~MagicArray(void);


	// =================================== CONTAINER FUNCTIONS =================================== // 

	// Returns a forward iterator
	inline Itor_t begin(void) const noexcept; 
	inline Itor_t end(void) const noexcept;

	inline Sz_t size(void) const noexcept;
	constexpr inline Sz_t Cap(void) const noexcept;

	void clear(void) noexcept;

//	inline void shrink(void);
//	inline void reserve(Sz_t _nSize);

	Ptr_t Insert(const Val_t& _obj);
	Ptr_t Insert(const Val_t& _obj, const Sz_t _nIndex);

	template<typename ...Ps>
	Ptr_t Emplace(Ps&& ...args);

	template<typename Ty, typename ...Ps>
	Ty* EmplaceAs(Ps&& ...args);

	//inline void Remove(void) noexcept;
	//inline void Remove(const T&) noexcept;
	inline void Remove(const Sz_t _nIndex);
	void Remove(T* _pObj);
//	void Remove(const Itor_t _pObj);

	inline bool IsEmpty(void) const noexcept;


	// ======================================== OPERATORS ======================================== // 

	Val_t& operator[] (const Sz_t _nIndex) noexcept;
	Val_t& operator[] (const Sz_t _nIndex) const noexcept;

	MagicArray& operator= (const MagicArray& _other);
	MagicArray& operator= (MagicArray&& _other) noexcept;

	
private:

	struct _DLL_EXPORT_ONLY Block
	{
		static constexpr uint64_t Range = Params::blk_sz > 63 ? ~(0Ui64) : (Math::Power<Params::blk_sz>(2Ui64) - 1);

		T* mpArray;
		uint64_t present[Params::blk_sz > 63 ? Params::blk_sz >> 6 : 1]{ ~Range };

		static inline uint64_t GetPresentIndex(uint64_t);

		bool IsFull(void) const noexcept;
		T* ConsumeNextEmpty(void);

		template <typename ... Ps>
		Ptr_t EmplaceNextEmpty(Ps&& ...);
		template <typename Ty, typename ... Ps>
		Ty* EmplaceNextEmptyAs(Ps&& ...);
		Ptr_t InsertNextEmpty(const Val_t& _obj);

		Block(void) noexcept = default;
	};

	struct _DLL_EXPORT_ONLY Iterator
	{
		Block const * mpBlock;
		Block const * mpEnd;
		Ptr_t mpAt;
		Sz_t mnIndex;

		Iterator(Array<Block, Params::blk_max> const&, const Ptr_t&, Sz_t);

		Itor_t operator++(int) noexcept;
		Itor_t& operator++(void) noexcept;

		Val_t& operator* (void) const noexcept;
		Ptr_t operator->(void) const noexcept;
		bool operator== (std::nullptr_t)  const noexcept;
		bool operator== (const Iterator&) const noexcept;
		bool operator!= (const Iterator&) const noexcept;

		explicit operator bool(void) const noexcept;
	};


	void Allocate(Block&);
	void Destroy(Val_t&) noexcept;

	Array<Block, Params::blk_max> mDirectory;
};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename T, typename PP>
MagicArray<T, PP>::MagicArray(void) noexcept
	: mDirectory{}
{

}

template <typename T, typename PP>
MagicArray<T, PP>::MagicArray(const MagicArray& _other) noexcept
	: mDirectory{ _other.mDirectory }
{

}

template <typename T, typename PP>
MagicArray<T, PP>::MagicArray(MagicArray&& _other) noexcept
	: mDirectory{ Ut::Move(_other.mDirectory) }
{
	std::memset(_other.mDirectory, 0, sizeof(_other.mDirectory));
}

template <typename T, typename PP>
MagicArray<T, PP>::~MagicArray(void)
{
	clear();

	for (auto& e : mDirectory)
	{
		PP::Alloc_t::Free(e.mpArray);
		e.mpArray = nullptr;
	}
}

template <typename T, typename PP>
inline typename MagicArray<T, PP>::Itor_t MagicArray<T, PP>::begin(void) const noexcept
{
	Itor_t ret{ mDirectory, mDirectory[0].mpArray, 0 };

	if (0 == (mDirectory[0].present[0] & 0x1))
		++ret;

	return ret;
}

template <typename T, typename PP>
inline typename MagicArray<T, PP>::Itor_t MagicArray<T, PP>::end(void) const noexcept
{
	return MagicArray<T, PP>::Itor_t{ nullptr, nullptr, 0 };
}

template <typename T, typename PP>
inline typename MagicArray<T, PP>::Sz_t MagicArray<T, PP>::size(void) const noexcept
{
	Sz_t size = 0;
	for (auto& blk : mDirectory)
	{
		for (auto e : blk.present)
		{
			e &= blk.Range;
			e = e - (e >> 1 & 0x5555555555555555);
			e = (e & 0x3333333333333333) + (e >> 2 & 0x3333333333333333);
			size += (((e >> 4) + e) & 0x0F0F0F0F0F0F0F0F) * 0x0101010101010101 >> 56;
		}
	}

	return size;
}

template <typename T, typename PP>
constexpr inline typename MagicArray<T, PP>::Sz_t MagicArray<T, PP>::Cap(void) const noexcept
{
	return PP::blk_sz * PP::blk_max;
}

template <typename T, typename PP>
void MagicArray<T, PP>::clear(void) noexcept
{
	for (auto& blk : mDirectory)
	{
		auto ptr = blk.mpArray;
		if (ptr)
		{
			for (auto& e : blk.present)
			{
				e &= blk.Range;
				while (e)
				{
					if (e & 0x1)
						Destroy(*ptr);
					e >>= 1;
					++ptr;
				}
			}

			blk.present[0] = ~blk.Range;
		}
		else
			break;
	}
}

template <typename T, typename PP>
typename MagicArray<T, PP>::Ptr_t MagicArray<T, PP>::Insert(const Val_t& _obj)
{
	for (auto& e : mDirectory)
	{
		if (!e.mpArray)
			Allocate(e);

		if (auto ptr = e.InsertNextEmpty(_obj))
			return ptr;
	}

	return nullptr;
}

template <typename T, typename PP>
typename MagicArray<T, PP>::Ptr_t MagicArray<T, PP>::Insert(const Val_t& _obj, const Sz_t _nIndex)
{
	auto& blk = mDirectory[_nIndex >> PP::shift];

	if (!blk.mpArray)
	{
		Allocate(blk);
	}

	auto offset = _nIndex & PP::offset;
	Ptr_t ptr = blk.mpArray + offset;

	if (0x1 & (blk.present[Block::GetPresentIndex(offset)] >> (_nIndex & 63)))
	{
		*ptr = _obj;
	}
	else
	{
		new (ptr) T{ _obj };
		blk.present[Block::GetPresentIndex(offset)] |= 1Ui64 << (_nIndex & 63);
	}

	return ptr;
}

template <typename T, typename PP> template<typename ...Ps>
typename MagicArray<T, PP>::Ptr_t MagicArray<T, PP>::Emplace(Ps&& ...args)
{
	for (auto& e : mDirectory)
	{
		if (!e.mpArray)
			Allocate(e);

		if (auto ptr = e.EmplaceNextEmpty(Ut::Forward<Ps>(args)...))
			return ptr;
	}

	return nullptr;
}

template<typename T, typename PP> template<typename Ty, typename ...Ps>
Ty* MagicArray<T, PP>::EmplaceAs(Ps&& ...args)
{
	for (auto& e : mDirectory)
	{
		if (!e.mpArray)
			Allocate(e);

		if (auto ptr = e.EmplaceNextEmptyAs<Ty>(Ut::Forward<Ps>(args)...))
			return ptr;
	}

	return nullptr;
}

template<typename T, typename PP>
inline void MagicArray<T, PP>::Remove(const Sz_t _nIndex)
{
	auto& blk = mDirectory[_nIndex >> PP::shift];
	auto offset = _nIndex & PP::offset;

#if _DEBUG
	if (0x1 & (page.present[blk.GetPresentIndex(offset)] >> (_nIndex & 63)))
	{
		__debugbreak();
	}
#endif


	Destroy(blk.mpArray[offset]);
	page.present[page.GetPresentIndex(offset)] &= ~(1Ui64 << (_nIndex & 63));
}

template<typename T, typename PP>
void MagicArray<T, PP>::Remove(T* _pObj)
{
	for (auto& blk : mDirectory)
	{
		auto diff = _pObj - blk.mpArray;

		if (diff < PP::blk_sz && diff >= 0)
		{
			Destroy(*_pObj);
			blk.present[blk.GetPresentIndex(diff)] &= ~(1Ui64 << (diff & 63Ui64));
			return;
		}
	}
}

template<typename T, typename PP>
inline bool MagicArray<T, PP>::IsEmpty(void) const noexcept
{
	for (auto& blk : mDirectory)
	{
		if (blk.mpArray)
		{
			for (auto e : blk.present)
				if (e & blk.Range)
					return false;
		}
		else
		{
			return true;
		}
	}
	return true;
}

template<typename T, typename PP>
inline void MagicArray<T, PP>::Allocate(Block& _blk)
{
	_blk.mpArray = static_cast<T*>(PP::Alloc_t::Alloc(PP::blk_sz));
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


template <typename T, typename PP>
T& MagicArray<T, PP>::operator[] (Sz_t _nIndex) noexcept
{
	return const_cast<T&>(const_cast<const MagicArray&>(*this)[_nIndex]);
}

template <typename T, typename PP>
T& MagicArray<T, PP>::operator[] (Sz_t _nIndex) const noexcept
{
#if _DEBUG
	/* Array index out of range */
	auto& blk = mDirectory[_nIndex >> PP::shift];
	if (nullptr == blk.mpArray)
		__debugbreak();

	if (0 == (0x1 & (blk.present[Block::GetPresentIndex(_nIndex & PP::offset)] >> (_nIndex & 63))))
		__debugbreak();
#endif

	return mDirectory[_nIndex >> PP::shift].mpArray[_nIndex & PP::offset];
}

template <typename T, typename PP>
MagicArray<T, PP>& MagicArray<T, PP>::operator= (const MagicArray<T, PP>& _other)
{
	clear();

	for (auto& e : _other)
	{
		Insert(e);
	}

	return *this;
}

template <typename T, typename PP>
MagicArray<T, PP>& MagicArray<T, PP>::operator= (MagicArray<T, PP>&& _other) noexcept
{
	Ut::Swap(mDirectory, _other.mDirectory);
	return *this;
}



// =============================================== NESTED CLASSES =============================================== // 


template<typename T, typename Params>
inline typename uint64_t MagicArray<T, Params>::Block::GetPresentIndex(uint64_t _nIndex)
{
	return _nIndex >> Math::Log<sizeof(uint64_t) * 8>::value;
}

template <typename T, typename PP>
bool MagicArray<T, PP>::Block::IsFull(void) const noexcept
{
	for (auto& e : present)
		if (0 != (~e & Range))
			return false;

	return true;
}

template <typename T, typename PP>
T* MagicArray<T, PP>::Block::ConsumeNextEmpty(void)
{
	T* ptr = mpArray;

	for (auto& e : present)
	{
		auto tmp = ~e & Range;
		if (0 == tmp)
		{
			ptr += Math::Min<decltype(PP::blk_sz)>(64, PP::blk_sz);
			continue;
		}

		unsigned long index = 0;
		_BitScanForward64(&index, tmp);
		e |= 1Ui64 << index;

		return ptr + index;
	}

	return nullptr;
}

template <typename T, typename PP> template <typename ... Ps>
typename MagicArray<T, PP>::Ptr_t MagicArray<T, PP>::Block::EmplaceNextEmpty(Ps&& ... args)
{
	if (auto ptr = ConsumeNextEmpty())
	{
		return new (ptr) T{ Ut::Forward<Ps>(args)... };
	}

	return nullptr;
}

template <typename T, typename PP> template <typename Ty, typename ...Ps>
Ty* MagicArray<T, PP>::Block::EmplaceNextEmptyAs(Ps&& ... args)
{
	if (auto ptr = ConsumeNextEmpty())
	{
		return new (ptr) Ty{ Ut::Forward<Ps>(args)... };
	}

	return nullptr;
}

template <typename T, typename PP>
typename MagicArray<T, PP>::Ptr_t MagicArray<T, PP>::Block::InsertNextEmpty(const Val_t& _obj)
{
	if (auto ptr = ConsumeNextEmpty())
	{
		return new (ptr) T{ _obj };
	}

	return nullptr;
}

template<typename T, typename Params>
inline void MagicArray<T, Params>::Destroy(Val_t& it) noexcept
{
	it;
	it.~T();
}

template<typename T, typename Params>
inline MagicArray<T, Params>::Iterator::Iterator(Array<Block, Params::blk_max> const& _dir, const Ptr_t& _pAt, Sz_t _nIndex) :
	mpBlock{ _dir.begin() }, mpEnd{ _dir.end() }, mpAt{ _pAt }, mnIndex{ _nIndex }
{
}

template<typename T, typename PP>
inline typename MagicArray<T, PP>::Itor_t MagicArray<T, PP>::Iterator::operator++(int) noexcept
{
	It_t cpy = *this;
	++*this;
	return cpy;
}

template<typename T, typename PP>
inline typename MagicArray<T, PP>::Itor_t& MagicArray<T, PP>::Iterator::operator++(void) noexcept
{
	do
	{
		if (++mnIndex < PP::blk_sz)
		{
			++mpAt;
		}
		else
		{
			++mpBlock;
			if (mpBlock != mpEnd)
			{
				mpAt = mpBlock->mpArray;
				mnIndex = 0;
				if (!mpAt)
					break;
			}
			else
			{
				mpAt = nullptr;
				break;
			}
		}
	} while (0 == (0x1 & (mpBlock->present[Block::GetPresentIndex(mnIndex)] >> (mnIndex & 63))));
	return *this;
}

template<typename T, typename PP>
inline typename MagicArray<T, PP>::Val_t& MagicArray<T, PP>::Iterator::operator*(void) const noexcept
{
	return *mpAt;
}

template<typename T, typename PP>
inline typename MagicArray<T, PP>::Ptr_t MagicArray<T, PP>::Iterator::operator->(void) const noexcept
{
	return mpAt;
}

template<typename T, typename PP>
inline bool MagicArray<T, PP>::Iterator::operator == (std::nullptr_t) const noexcept
{
	return mpAt == nullptr;
}

template<typename T, typename PP>
inline bool operator == (std::nullptr_t, typename MagicArray<T, PP>::Iterator const& _rhs) noexcept
{
	return _rhs == nullptr;
}

template<typename T, typename PP>
inline bool MagicArray<T, PP>::Iterator::operator == (const Iterator& _rhs) const noexcept
{
	return mpAt == _rhs.mpAt;
}

template<typename T, typename PP>
inline bool MagicArray<T, PP>::Iterator::operator != (const Iterator& _rhs) const noexcept
{
	return mpAt != _rhs.mpAt;
}

template<typename T, typename PP>
inline MagicArray<T, PP>::Iterator::operator bool(void) const noexcept
{
	return !!mpAt;
}



#endif		// INCLUDE GUARD

