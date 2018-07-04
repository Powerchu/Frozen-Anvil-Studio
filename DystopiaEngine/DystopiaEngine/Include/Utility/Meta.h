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
	template <bool, typename true_t, typename false_t>
	struct IfElse
	{
		using type = true_t;
		using result = true_t;
	};

	template <typename true_t, typename false_t>
	struct IfElse<false, true_t, false_t>
	{
		using type = false_t;
		using result = false_t;
	};

	template <bool b, typename true_t, typename false_t>
	using IfElse_t = typename IfElse<b, true_t, false_t>::result;

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

	template <typename T, T _value>
	struct Constant
	{
		static constexpr T value = _value;
	};

	template<bool _b, typename T>
	struct EnableIf
	{
		// Force fail if false
	};

	template<typename T>
	struct EnableIf<true, T>
	{
		using type = T;
	};

	template <bool _b, typename T>
	using EnableIf_t = typename EnableIf<_b, T>::type;

	template <typename T, typename U>
	struct IsSame : Constant <bool, false> {};

	template <typename T>
	struct IsSame<T, T> : Constant <bool, true> {};

	template <typename T>
	struct NotLvalueRef : Constant<bool, true> {};

	template <typename T>
	struct NotLvalueRef<T&> : Constant<bool, false> {};

	template <typename T>
	struct IsIntegral : Constant<bool,
		IsSame<char, T>					::value ||
		IsSame<unsigned char, T>		::value ||
		IsSame<short, T>				::value ||
		IsSame<unsigned short, T>		::value ||
		IsSame<int, T>					::value ||
		IsSame<unsigned, T>				::value ||
		IsSame<long, T>					::value ||
		IsSame<unsigned long, T>		::value ||
		IsSame<long long, T>			::value ||
		IsSame<unsigned long long, T>	::value
	> {};

	template <typename T>
	struct IsFloatType : Constant<bool,
		IsSame<float, T>		::value ||
		IsSame<double, T>		::value ||
		IsSame<long double, T>	::value
	> {};

	template <typename T>
	struct IsNumeric : Constant<bool,
		IsIntegral<T>			::value ||
		IsFloatType<T>			::value
	> {};

	template <typename T>
	struct IsSigned : Constant<bool, ((T( -1 )) < (T( 0 )))> {};

	template <typename T>
	struct HasMember : Constant<bool, sizeof(Helper::HasMember<T>(nullptr)) == sizeof(char)> {};
}



#endif		// INCLUDE GUARD

