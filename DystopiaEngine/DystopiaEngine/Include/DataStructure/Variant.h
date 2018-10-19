/* HEADER *********************************************************************************/
/*!
\file	Variant.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Something like a union.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _VARIANT_H_
#define _VARIANT_H_

#include "Utility/Meta.h"
#include "Utility/MetaAlgorithms.h"
#include "Utility/DebugAssert.h"

#include <cstring>
#include <type_traits>



template <typename ... Ty>
class Variant
{
#define VARIANT_ENABLE_IF_SFINAE(_TYPE_, _RET_) Ut::EnableIf_t<Ut::MetaFind<Ut::Decay_t<_TYPE_>, Ty ...>::value, _RET_>
#define VARIANT_TYPE_RESOLUTION(_TYPE_)         typename Ut::ConvertType<_TYPE_, Ty...>::result
	static_assert(sizeof...(Ty) != 0, "Variant must contain at least one type!");
	using AllTypes = Ut::MetaAutoIndexer_t<Ty...>;

public:
	// ====================================== CONSTRUCTORS ======================================= // 

	inline constexpr Variant(void) noexcept;
	inline Variant(Variant&&) noexcept = default;
	inline Variant(const Variant&) noexcept = default;

	template <typename U, typename Actual_t = VARIANT_TYPE_RESOLUTION(U)>
	inline explicit Variant(U&&) noexcept(std::is_nothrow_constructible_v<Actual_t, U>);

	~Variant(void) noexcept;


	// =================================== CONTAINER FUNCTIONS =================================== // 

	// Use this only when you know what you're doing
	template <typename U>
	inline auto As(void) noexcept -> VARIANT_ENABLE_IF_SFINAE(U, U&);

	// Use this only when you know what you're doing
	template <typename U>
	inline auto As(void) const noexcept -> VARIANT_ENABLE_IF_SFINAE(U, U const&);

	template <typename Visitor>
	inline void Visit(Visitor&&);


	// ======================================== OPERATORS ======================================== // 

	template <typename U>
	inline auto operator = (U&&) -> VARIANT_ENABLE_IF_SFINAE(VARIANT_TYPE_RESOLUTION(U), Variant&);

	Variant& operator = (Variant&&);
	Variant& operator = (const Variant&);


private:
	__declspec (align (Ut::MetaMax<size_t, alignof(Ty) ...>::value))
		char raw[Ut::MetaMax<size_t, sizeof(Ty) ...>::value];

	unsigned short mType;
	static constexpr auto mInvalidType = Ut::Constant<decltype(mType), ~0>::value;

	bool IsValidType(void) const noexcept;

	template <typename U>
	static void Destroy(U*) noexcept;
	inline void DestroyCurrent(void) noexcept;
};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename ... Ty>
inline constexpr Variant<Ty...>::Variant(void) noexcept
	: raw{}, mType{ mInvalidType }
{
}

template <typename ... Ty>
inline Variant<Ty...>::~Variant(void) noexcept
{
	if (IsValidType())
		DestroyCurrent();
}

template< typename ... Ty> template <typename U, typename Actual_t>
inline Variant<Ty...>::Variant(U&& _obj) noexcept(std::is_nothrow_constructible_v<Actual_t, U>) :
	mType{ Ut::MetaFind_t<Actual_t, AllTypes>::value }
{
	::new (reinterpret_cast<void*>(&raw)) Actual_t ( Ut::Move(_obj) );
}


template <typename ... Ty> template <typename U>
inline auto Variant<Ty...>::As(void) noexcept -> VARIANT_ENABLE_IF_SFINAE(U, U&)
{
	return const_cast<U&>(const_cast<Variant const *>(this)->As<U>());
}

template <typename ... Ty> template <typename U>
inline auto Variant<Ty...>::As(void) const noexcept -> VARIANT_ENABLE_IF_SFINAE(U, U const&)
{
#if defined(_DEBUG)

	DEBUG_BREAK(!(Ut::MetaFind_t<U, AllTypes>::value == mType),
		"Variant Error: Wrong type!\n");

#endif

	return *reinterpret_cast<U const*>(&raw);
}

template <typename ... Ty> template <typename Visitor>
inline void Variant<Ty...>::Visit(Visitor&& _visitor)
{
	static void(*SwitchTable[])(char*, Visitor&&) = {
		[](char* _raw, Visitor&& _v) -> void { _v(*reinterpret_cast<Ty*>(_raw)); } ...
	};

	if (IsValidType())
		SwitchTable[mType](raw, Ut::Forward<Visitor>(_visitor));
}


template <typename ...Ty>
inline bool Variant<Ty...>::IsValidType(void) const noexcept
{
	return Ut::Constant<decltype(mType), ~0>::value ^ mType;
}

template <typename ... Ty>
inline void Variant<Ty...>::DestroyCurrent(void) noexcept
{
	static void(*SwitchTable[])(char*) = {
		[](char* _raw) -> void { Destroy<Ty>(reinterpret_cast<Ty*>(_raw)); } ...
	};

	if (IsValidType())
		SwitchTable[mType](raw);
}

template <typename ... Ty> template <typename U>
static inline void Variant<Ty...>::Destroy(U* _ptr) noexcept
{
	_ptr->~U();
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


template <typename ... Ty> template <typename U>
inline auto Variant<Ty...>::operator = (U&& _rhs) -> VARIANT_ENABLE_IF_SFINAE(VARIANT_TYPE_RESOLUTION(U), Variant&)
{
	using Actual_t = VARIANT_TYPE_RESOLUTION(U);

	DestroyCurrent();
	mType = Ut::MetaFind_t<Ut::Decay_t<Actual_t>, AllTypes>::value;
	::new (reinterpret_cast<void*>(&raw)) Ut::Decay_t<Actual_t> ( Ut::Move(_rhs) );

	return *this;
}

template <typename ... Ty>
Variant<Ty...>& Variant<Ty...>::operator = (Variant<Ty...>&& _rhs)
{
	if (this != &_rhs)
	{
		DestroyCurrent();
		std::memcpy(this, &_rhs, sizeof(Variant<Ty...>));
		_rhs.mType = mInvalidType;
	}

	return *this;
}

template <typename ... Ty>
Variant<Ty...>& Variant<Ty...>::operator=(const Variant<Ty...>& _rhs)
{
	static void(*SwitchDifferent[])(char*, const Variant<Ty...>&) = {
		[](char* _raw, const Variant<Ty...>& _rhs) -> void {
			::new (reinterpret_cast<void*>(_raw)) Ty { _rhs.As<Ty>() };
		} ...
	};

	static void(*SwitchEqual[])(char*, const Variant<Ty...>&) = {
		[](char* _raw, const Variant<Ty...>& _rhs) -> void {
			*reinterpret_cast<Ty*>(_raw) = _rhs.As<Ty>();
		} ...
	};

	if (this != &_rhs)
	{
		if (mType != _rhs.mType)
		{
			DestroyCurrent();
			mType = _rhs.mType;

			if (IsValidType())
				SwitchDifferent[mType](raw, _rhs);
		}
		else if (IsValidType())
		{
			SwitchEqual[mType](raw, _rhs);
		}
	}

	return *this;
}



#undef VARIANT_ENABLE_IF_SFINAE


#endif		// INCLUDE GUARD

