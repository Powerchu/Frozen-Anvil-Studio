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


	// ======================================== OPERATORS ======================================== // 

	template <typename U>
	inline auto operator = (const U&) -> VARIANT_ENABLE_IF_SFINAE(U, Variant&);

	template <typename U>
	inline auto operator = (U&&) -> VARIANT_ENABLE_IF_SFINAE(U, Variant&);


private:
	__declspec (align (Utility::MetaMax<size_t, alignof(Ty) ...>::value))
		char raw[Utility::MetaMax<size_t, sizeof(Ty) ...>::value];
};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template<typename ... Ty>
inline constexpr Variant<Ty...>::Variant(void) noexcept
	: raw{}
{
}

template<typename ... Ty> template<typename U, typename>
inline Variant<Ty...>::Variant(const U& _obj) noexcept(std::is_nothrow_copy_constructible_v<U>)
{
	::new (reinterpret_cast<Utility::Decay_t<U>*>(&raw)) Utility::Decay_t<U> { _obj };
}

template<typename ... Ty> template<typename U, typename>
inline Variant<Ty...>::Variant(U&& _obj) noexcept(std::is_nothrow_move_constructible_v<U>)
{
	::new (reinterpret_cast<Utility::Decay_t<U>*>(&raw)) Utility::Decay_t<U> { Utility::Move(_obj) };
}


template<typename ... Ty> template<typename U>
inline auto Variant<Ty...>::As(void) noexcept -> VARIANT_ENABLE_IF_SFINAE(U, U&)
{
	return *reinterpret_cast<U*>(&raw);
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


template<typename ... Ty> template<typename U>
inline auto Variant<Ty...>::operator = (const U& _rhs) -> VARIANT_ENABLE_IF_SFINAE(U, Variant&)
{
	*reinterpret_cast<U*>(&raw) = _rhs;
	return *this;
}

template<typename ... Ty> template<typename U>
inline auto Variant<Ty...>::operator = (U&& _rhs) -> VARIANT_ENABLE_IF_SFINAE(U, Variant&)
{
	*reinterpret_cast<U*>(&raw) = Utility::Move(_rhs);
	return *this;
}



#undef VARIANT_ENABLE_IF_SFINAE


#endif		// INCLUDE GUARD

