/* HEADER *********************************************************************************/
/*!
\file	Variant.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Something like a union.
	TODO: Use visitor to get the value of the correct current type

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _VARIANT_H_
#define _VARIANT_H_

#include "Utility\Meta.h"
#include "Utility\MetaAlgorithms.h"

#include <type_traits>



template <typename ... Ty>
class Variant
{
#define VARIANT_ENABLE_IF_SFINAE(_TYPE_, _RET_) Utility::EnableIf_t<Utility::MetaFind<Utility::Decay_t<_TYPE_>, Ty ...>::value, _RET_>
	using AllTypes = Utility::MetaAutoIndexer_t<Ty...>;
public:
	// ====================================== CONSTRUCTORS ======================================= // 

	inline constexpr Variant(void) noexcept;
	template <typename U, typename = VARIANT_ENABLE_IF_SFINAE(U, void)>
	Variant(const U&) noexcept(std::is_nothrow_copy_constructible_v<U>);
	template <typename U, typename = VARIANT_ENABLE_IF_SFINAE(U, void)>
	Variant(U&&) noexcept(std::is_nothrow_move_constructible_v<U>);


	// =================================== CONTAINER FUNCTIONS =================================== // 

	// Use this only when you know what you're doing
	template <typename U>
	inline auto As(void) noexcept -> VARIANT_ENABLE_IF_SFINAE(U, U&);

	template <typename Visitor>
	void Visit(Visitor&&);


	// ======================================== OPERATORS ======================================== // 

	template <typename U>
	inline auto operator = (const U&) -> VARIANT_ENABLE_IF_SFINAE(U, Variant&);

	template <typename U>
	inline auto operator = (U&&) -> VARIANT_ENABLE_IF_SFINAE(U, Variant&);


private:
	__declspec (align (Utility::MetaMax<size_t, alignof(Ty) ...>::value))
		char raw[Utility::MetaMax<size_t, sizeof(Ty) ...>::value];

	short mType;

	template <typename U>
	static void Destroy(U*) noexcept;
	inline void DestroyCurrent(void) noexcept;
};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template<typename ... Ty>
inline constexpr Variant<Ty...>::Variant(void) noexcept
	: raw{}
{
}

template<typename ... Ty> template<typename U, typename>
inline Variant<Ty...>::Variant(const U& _obj) noexcept(std::is_nothrow_copy_constructible_v<U>) :
	mType { Utility::MetaFind_t<Utility::Decay_t<U>, AllTypes>::value }
{
	::new (reinterpret_cast<Utility::Decay_t<U>*>(&raw)) Utility::Decay_t<U> { _obj };
}

template<typename ... Ty> template<typename U, typename>
inline Variant<Ty...>::Variant(U&& _obj) noexcept(std::is_nothrow_move_constructible_v<U>) :
	mType{ Utility::MetaFind_t<Utility::Decay_t<U>, AllTypes>::value }
{
	::new (reinterpret_cast<Utility::Decay_t<U>*>(&raw)) Utility::Decay_t<U> { Utility::Move(_obj) };
}


template<typename ... Ty> template<typename U>
inline auto Variant<Ty...>::As(void) noexcept -> VARIANT_ENABLE_IF_SFINAE(U, U&)
{
	return *reinterpret_cast<U*>(&raw);
}

template<typename ... Ty> template <typename Visitor>
inline void Variant<Ty...>::Visit(Visitor&& _visitor)
{
	static void(*SwitchTable[])(char*, Visitor&&) = {
		[](char* _raw, Visitor&& _v) -> void { _v(*reinterpret_cast<Ty*>(_raw)); } ...
	};

	SwitchTable[mType](raw, Utility::Forward<Visitor>(_visitor));
}


template<typename ... Ty>
inline void Variant<Ty...>::DestroyCurrent(void) noexcept
{
	static void(*SwitchTable[])(char*) = {
		[](char* _raw) -> void { Destroy<Ty>(reinterpret_cast<Ty*>(_raw)); } ...
	};

	SwitchTable[mType](raw);
}

template<typename ... Ty> template <typename U>
static inline void Variant<Ty...>::Destroy(U* _ptr) noexcept
{
	_ptr->~U();
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


template<typename ... Ty> template<typename U>
inline auto Variant<Ty...>::operator = (const U& _rhs) -> VARIANT_ENABLE_IF_SFINAE(U, Variant&)
{
	DestroyCurrent();
	mType = Utility::MetaFind_t<Utility::Decay_t<U>, AllTypes>::value;
	::new (reinterpret_cast<Utility::Decay_t<U>*>(&raw)) Utility::Decay_t<U> { _rhs };
	return *this;
}

template<typename ... Ty> template<typename U>
inline auto Variant<Ty...>::operator = (U&& _rhs) -> VARIANT_ENABLE_IF_SFINAE(U, Variant&)
{
	DestroyCurrent();
	mType = Utility::MetaFind_t<Utility::Decay_t<U>, AllTypes>::value;
	::new (reinterpret_cast<Utility::Decay_t<U>*>(&raw)) Utility::Decay_t<U> { Utility::Move(_rhs) };
	return *this;
}



#undef VARIANT_ENABLE_IF_SFINAE


#endif		// INCLUDE GUARD

