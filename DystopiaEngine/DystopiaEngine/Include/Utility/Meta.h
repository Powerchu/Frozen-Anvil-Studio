/* HEADER *********************************************************************************/
/*!
\file	Meta.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Contains commonly used Metafunctions or you can just #include <type_traits>.
	Written for educational purposes is a legit excuse since we're students.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _META_H_
#define _META_H_

#include "Utility\MetaHelper.h"


namespace Utility
{
	// Constant
	// ========= ===========================================================

	template <typename T, T _value>
	struct Constant
	{
		static constexpr T value = _value;
	};


	// If - Else
	// =========== =========================================================

	template <bool, typename true_t, typename false_t>
	struct IfElse
	{
		using type = true_t;
	};

	template <typename true_t, typename false_t>
	struct IfElse<false, true_t, false_t>
	{
		using type = false_t;
	};

	template <bool b, typename true_t, typename false_t>
	using IfElse_t = typename IfElse<b, true_t, false_t>::type;


	// RemoveRef
	// ========== ==========================================================

	template <typename T>
	struct RemoveRef
	{
		using type = T;
	};

	template<typename T>
	struct RemoveRef<T&>
	{
		using type = T;
	};

	template<typename T>
	struct RemoveRef<T&&>
	{
		using type = T;
	};

	template <typename T>
	using RemoveRef_t = typename RemoveRef<T>::type;


	// RemoveConst
	// ============= =======================================================

	template <typename T>
	struct RemoveConst
	{
		using type = T;
	};

	template<typename T>
	struct RemoveConst<const T>
	{
		using type = T;
	};

	template <typename T>
	using RemoveConst_t = typename RemoveConst<T>::type;


	// Decay
	// ======== ============================================================

	template <typename T>
	struct Decay
	{
		using type = RemoveConst_t<RemoveRef_t<T>>;
	};

	template <typename T, typename ... Ty>
	struct Decay<T(Ty...)>
	{
		using type = T(*)(Ty...);
	};

	template <typename T, size_t N>
	struct Decay<T[N]>
	{
		using type = T*;
	};

	template <typename T>
	using Decay_t = typename Decay<T>::type;


	// EnableIf
	// ========= ===========================================================

	template <bool _b, typename T = void>
	struct EnableIf
	{
		// Force fail if false
	};

	template <typename T>
	struct EnableIf<true, T>
	{
		using type = T;
	};

	template <bool _b, typename T = void>
	using EnableIf_t = typename EnableIf<_b, T>::type;


	// IsSame
	// ======== ============================================================

	template <typename T, typename U>
	struct IsSame : Constant <bool, false> {};

	template <typename T>
	struct IsSame<T, T> : Constant <bool, true> {};


	// Is l-value ref
	// ================ ====================================================

	template <typename T>
	struct IsLvalueRef : Constant<bool, false> {};

	template <typename T>
	struct IsLvalueRef<T&> : Constant<bool, true> {};

	template <typename T>
	struct NotLvalueRef : Constant<bool, !IsLvalueRef<T>::value> {};


	// Is Integral
	// ============= =======================================================

	template <typename T>
	struct IsIntegral : Constant<bool,
		IsSame<char, T>				  ::value ||
		IsSame<unsigned char, T>	  ::value ||
		IsSame<short, T>			  ::value ||
		IsSame<unsigned short, T>	  ::value ||
		IsSame<int, T>				  ::value ||
		IsSame<unsigned, T>			  ::value ||
		IsSame<long, T>				  ::value ||
		IsSame<unsigned long, T>	  ::value ||
		IsSame<long long, T>		  ::value ||
		IsSame<unsigned long long, T> ::value
	> {};


	// Is Float
	// =========== =========================================================

	template <typename T>
	struct IsFloatType : Constant<bool,
		IsSame<float, T>	   ::value ||
		IsSame<double, T>	   ::value ||
		IsSame<long double, T> ::value
	> {};


	// Is Numeral
	// =========== =========================================================

	template <typename T>
	struct IsNumeric : Constant<bool,
		IsIntegral<T>  ::value ||
		IsFloatType<T> ::value
	> {};


	// Is Signed
	// =========== =========================================================

	template <typename T>
	struct IsSigned : Constant<bool, T( -1 ) < T( 0 )> {};


	// Has Members
	// ============= =======================================================

	template <typename T>
	struct HasMember : Constant<bool, 
		sizeof(Helper::HasMember<T>(nullptr)) == sizeof(char)>
	{};


	// Is Pointer
	// ============= =======================================================

	template <typename ... T>
	struct IsPointer : Constant <bool, false>
	{};

	template <typename Ty, typename ... T>
	struct IsPointer<Ty*, T...> : Constant <bool, IsPointer<T...>::value>
	{};

	template <typename Ty>
	struct IsPointer<Ty*> : Constant <bool, true>
	{};


	// Force Evaulate	   Warning: Causes the compiler to crash
	// ================ ====================================================

	// Warning: Crashes the compiler
	template <typename T, T value>
	struct ForceEval : Constant <T, value>
	{};

	// Warning: Crashes the compiler
	template <auto _Val>
	auto ForceEval_v = ForceEval<decltype(_Val), _Val>::value;

}



#endif		// INCLUDE GUARD

