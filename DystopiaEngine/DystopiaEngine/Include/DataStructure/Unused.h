/* HEADER *********************************************************************************/
/*!
\file	Unused.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Global Dummy Variable class

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _UNUSED_VAR_H_
#define _UNUSED_VAR_H_

#include "Utility/Meta.h"

#if defined(DEBUG) | defined(_DEBUG)
#include "Utility/DebugAssert.h"
#endif // Debug only includes


class Unused
{
	template <typename T> struct Enforcer;

public:
	// ====================================== CONSTRUCTORS ======================================= // 

	constexpr explicit Unused(void)          noexcept = default;
	constexpr explicit Unused(Unused&&)      noexcept = default;
	constexpr explicit Unused(Unused const&) noexcept = default;
	
	template <typename T>
	constexpr explicit Unused(T&&) noexcept;


	// =================================== CONTAINER FUNCTIONS =================================== // 

	template <typename T>
	constexpr Enforcer<T> EnforceType(void) noexcept;

	template <typename T>     inline void push_back    (T&&)    noexcept {}
	template <typename ... T> inline void emplace_back (T&&...) noexcept {}

	template <typename T>     inline void Insert       (T&&)    noexcept {}
	template <typename ... T> inline void Emplace      (T&&...) noexcept {}
	template <typename ... T> inline void EmplaceBack  (T&&...) noexcept {}

	template <typename T>
	constexpr Unused& operator = (T&&) noexcept;

	template <typename T>
	constexpr void operator () (T&&) const noexcept;

	constexpr Unused& operator = (Unused&&)      noexcept = default;
	constexpr Unused& operator = (Unused const&) noexcept = default;

private:

	template <typename T>
	struct Enforcer
	{
		constexpr inline explicit Enforcer(Unused&) noexcept;

		constexpr Enforcer<T>& operator = (T&&) noexcept;
	};
};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename T>
constexpr inline Unused::Unused(T&&) noexcept
{}


template <typename T>
constexpr inline typename Unused::Enforcer<T> Unused::EnforceType(void) noexcept
{
	return Enforcer<T>{ *this };
}

template <typename T>
constexpr inline Unused& Unused::operator = (T&&) noexcept
{
	return *this;
}

template<typename T>
inline constexpr void Unused::operator()(T&&) const noexcept
{
}

template<typename T>
constexpr inline Unused::Enforcer<T>::Enforcer(Unused&) noexcept
{
}

template <typename T>
constexpr inline typename Unused::Enforcer<T>& Unused::Enforcer<T>::operator = (T&&) noexcept
{
	return *this;
}

template <typename U, typename T>
constexpr decltype(auto) operator >> (U&& lhs, Unused::Enforcer<T> const&) noexcept(noexcept(Ut::declval<U>() >> Ut::declval<T&>()))
{
	alignas(T) char dummy[sizeof(T)];
	return lhs >> *(reinterpret_cast<T*>(dummy));
}



#endif		// INCLUDE GUARD


