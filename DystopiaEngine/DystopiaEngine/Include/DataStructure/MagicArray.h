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
		static constexpr unsigned blk_sz = BLOCK_SIZE;
		static constexpr unsigned blk_max = MAX_BLOCKS;

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
	using Info_t = Params;


	// ====================================== CONSTRUCTORS ======================================= // 

	MagicArray(void) noexcept;
	explicit MagicArray(Sz_t _nSize);
	MagicArray(const MagicArray& _other);
	MagicArray(MagicArray&& _other) noexcept;
	template <typename U, typename =
		Utility::EnableIf_t<
			Utility::IsSame<Utility::Decay_t<U>, T>::value &&
			!Utility::IsIntegral<Utility::Decay_t<U>>::value >>
	MagicArray(std::initializer_list<U>);

	~MagicArray(void);


	// =================================== CONTAINER FUNCTIONS =================================== // 

	inline Itor_t begin(void) const noexcept;
	inline Itor_t end(void) const noexcept;

	inline Sz_t size(void) const noexcept;
	inline Sz_t Cap(void) const noexcept;

	inline void push_back(const T& _obj);
	inline void pop_back(void) noexcept;

	inline void clear(void) noexcept;

	inline void shrink(void);
	inline void reserve(Sz_t _nSize);

	void Insert(const T& _obj);
	void Insert(const T& _obj, const Sz_t _nIndex);
	void Insert(const T& _obj, const Itor_t _pPos);

	template<typename ...Args>
	Itor_t Emplace(Args &&...args);

	// Removes the last element of the array
	inline void Remove(void) noexcept;
	inline void Remove(const T&) noexcept;
	void Remove(const Sz_t _nIndex);
	void Remove(const Itor_t _pObj);

	inline bool IsEmpty(void) const noexcept;


	// ======================================== OPERATORS ======================================== // 

	Val_t& operator[] (Sz_t _nIndex);
	const Val_t& operator[] (Sz_t _nIndex) const;

	MagicArray& operator= (const MagicArray& _other);
	MagicArray& operator= (MagicArray&& _other) noexcept;

	
private:

};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


// here



// ============================================ OPERATOR OVERLOADING ============================================ // 


// here



#endif		// INCLUDE GUARD

