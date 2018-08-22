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

#include "Utility\Meta.h"
#include "Utility\Utility.h"
#include "Math\MathUtility.h"

#include "DataStructure\Array.h"

#include <initializer_list> // init-list


template <typename, typename>
class MagicArray;

namespace Ctor
{
	template <typename T, unsigned BLOCK_SIZE = 32, unsigned MAX_BLOCKS = 32>
	struct MagicArrayBuilder
	{
		static_assert(Utility::IsPowerOf2(BLOCK_SIZE), "Block Size must be a power of 2");

		template <unsigned NEW_SIZE>
		using SetBlockSize = MagicArrayBuilder<T, NEW_SIZE, MAX_BLOCKS>;

		template <unsigned NEW_SIZE>
		using SetBlockLimit = MagicArrayBuilder<T, BLOCK_SIZE, NEW_SIZE>;

		using type = MagicArray<T, MagicArrayBuilder>;


	private:
		static constexpr unsigned blk_sz  = BLOCK_SIZE;
		static constexpr unsigned blk_max = MAX_BLOCKS;
		static constexpr unsigned shift   = Math::Log2<BLOCK_SIZE>::value;
		static constexpr unsigned offset  = BLOCK_SIZE - 1;

		friend class MagicArray<T, MagicArrayBuilder>;

		MagicArrayBuilder() = delete;
		MagicArrayBuilder(MagicArrayBuilder&&) = delete;
		MagicArrayBuilder(const MagicArrayBuilder&) = delete;
	};
}

template <typename T, typename Params = MagicArrayBuilder<T>>
class MagicArray
{
public:
	// ==================================== CONTAINER DEFINES ==================================== // 

	using Itor_t = T*;
	using Val_t  = T;
	using Ptr_t  = T *;
	using Sz_t   = unsigned;


	// ====================================== CONSTRUCTORS ======================================= // 

	MagicArray(void) noexcept;
	MagicArray(const MagicArray& _other) noexcept;
	MagicArray(MagicArray&& _other) noexcept;

	~MagicArray(void);


	// =================================== CONTAINER FUNCTIONS =================================== // 

	inline Itor_t begin(void) const noexcept;
	inline Itor_t end(void) const noexcept;

	inline Sz_t size(void) const noexcept;
	inline Sz_t Cap(void) const noexcept;

	void clear(void) noexcept;

	inline void shrink(void);
	inline void reserve(Sz_t _nSize);

	Ptr_t Insert(const Val_t& _obj);
	Ptr_t Insert(const Val_t& _obj, const Sz_t _nIndex);

	template<typename ...Ps>
	Itor_t Emplace(Ps&& ...args);

	//inline void Remove(void) noexcept;
	//inline void Remove(const T&) noexcept;
	void Remove(const Sz_t _nIndex);
	void Remove(const Itor_t _pObj);

	inline bool IsEmpty(void) const noexcept;


	// ======================================== OPERATORS ======================================== // 

	Val_t& operator[] (const Sz_t _nIndex);
	const Val_t& operator[] (const Sz_t _nIndex) const;

	MagicArray& operator= (const MagicArray& _other);
	MagicArray& operator= (MagicArray&& _other) noexcept;

	
private:

	struct Block
	{
		static constexpr uint64_t Range = Params::blk_sz > 63 ? ~(0Ui64) : (Math::Power<Params::blk_sz>(2Ui64) - 1);

		T* mpArray;
		uint64_t present[Params::blk_sz > 63 ? Params::blk_sz >> 6 : 1]{ ~Range };

		bool IsFull(void) const;
		T* NextEmpty(void) const;

		template <typename ... Ps>
		Ptr_t EmplaceNextEmpty(Ps&& ...);
		Ptr_t InsertNextEmpty(const Val_t& _obj);

		Block(void) noexcept = default;
		~Block(void);
	};


	void Allocate(Block&);

	Block* mpCurrent;
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
	: mDirectory{ Utility::Move(_other.mDirectory) }
{
	std::memset(_other.mDirectory, 0, sizeof(_other.mDirectory));
}

template <typename T, typename PP>
MagicArray<T, PP>::~MagicArray(void)
{
	clear();

	for (auto& e : mDirectory)
		delete e.mpArray;
}

template <typename T, typename PP>
void MagicArray<T, PP>::clear(void) noexcept
{
	
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
	auto& page  = mDirectory[_nIndex >> PP::shift];

	if (!page.mpArray)
	{
		Allocate(page);
	}

	auto offset = _nIndex & PP::offset;
	Ptr_t ptr = page.mpArray + offset;

	if (0x1 & (page.present[offset >> 6] >> (_nIndex & 63)))
	{
		*ptr = _obj;
	}
	else
	{
		new (ptr) T{ _obj };
		page.present[offset >> 6] |= 1Ui64 << (_nIndex & 63);
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

		if (auto ptr = e.EmplaceNextEmpty(Utility::Forward<Ps>(args)...))
			return ptr;
	}
	return nullptr;
}

template<typename T, typename PP>
inline void MagicArray<T, PP>::Allocate(Block& _blk)
{
	_blk.mpArray = static_cast<T*>(::operator new[](sizeof(T) * PP::blk_sz));
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


template <typename T, typename PP>
T& MagicArray<T, PP>::operator[] (Sz_t _nIndex)
{
	return const_cast<T&>(const_cast<const MagicArray&>(*this)[_nIndex]);
}

template <typename T, typename PP>
const T& MagicArray<T, PP>::operator[] (Sz_t _nIndex) const
{
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
	Utility::Swap(mDirectory, _other.mDirectory);
	return *this;
}



// =============================================== NESTED CLASSES =============================================== // 


template <typename T, typename PP>
bool MagicArray<T, PP>::Block::IsFull(void) const
{
	for (auto& e : present)
		if (0 != (~e & Range))
			return false;

	return true;
}

template <typename T, typename PP>
T* MagicArray<T, PP>::Block::NextEmpty(void) const
{
	T* ptr = mpArray;

	for (auto e : present)
	{
		if (0 == (~e & Range))
		{
			ptr += Math::Min<decltype(PP::blk_sz)>(64, PP::blk_sz);
			continue;
		}

		unsigned long index = 0;
		_BitScanForward64(&index, (~e & Range));

		return ptr + index;
	}

	return nullptr;
}

template <typename T, typename PP> template <typename ... Ps>
typename MagicArray<T, PP>::Ptr_t MagicArray<T, PP>::Block::EmplaceNextEmpty(Ps&& ... args)
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
		new (ptr + index) T{ args... };

		return ptr + index;
	}

	return nullptr;
}

template <typename T, typename PP>
typename MagicArray<T, PP>::Ptr_t MagicArray<T, PP>::Block::InsertNextEmpty(const Val_t& _obj)
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
		new (ptr + index) T{ _obj };

		return ptr + index;
	}

	return nullptr;
}

template<typename T, typename Params>
inline MagicArray<T, Params>::Block::~Block(void)
{
	::operator delete[] (static_cast<void*>(mpArray));
}



#endif		// INCLUDE GUARD

