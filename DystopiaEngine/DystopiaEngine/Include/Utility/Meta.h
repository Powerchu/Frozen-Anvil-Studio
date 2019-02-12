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

#include "Utility/MetaHelper.h"


namespace Ut
{
	struct NULL_TYPE;

	// declval
	// ========= ===========================================================

	template <typename T>
	T declval(void) noexcept;


	// Constant
	// ========= ===========================================================

	template <typename T, T _value>
	struct Constant
	{
		static constexpr T value = _value;

		constexpr operator T () { return value; }
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


	// RemoveLowConst
	// ================ ====================================================

	template <typename T>
	struct RemoveLowConst
	{
		using type = T;
	};

	template<typename T>
	struct RemoveLowConst<const T>
	{
		using type = const typename RemoveLowConst<T>::type;
	};

	template<typename T>
	struct RemoveLowConst<T const*>
	{
		using type = T*;
	};

	template<typename T>
	struct RemoveLowConst<T const&>
	{
		using type = T&;
	};

	template<typename T>
	struct RemoveLowConst<T const &&>
	{
		using type = T&&;
	};

	template <typename T>
	using RemoveLowConst_t = typename RemoveLowConst<T>::type;


	// AddLowConst
	// ================ ====================================================

	template <typename T>
	struct AddLowConst
	{
		using type = T;
	};

	template<typename T>
	struct AddLowConst<const T>
	{
		using type = const typename AddLowConst<T>::type;
	};

	template<typename T>
	struct AddLowConst<T*>
	{
		using type = T const*;
	};

	template<typename T>
	struct AddLowConst<T&>
	{
		using type = T const&;
	};

	template<typename T>
	struct AddLowConst<T&&>
	{
		using type = T const &&;
	};

	template <typename T>
	using AddLowConst_t = typename AddLowConst<T>::type;


	// Decay
	// ======== ============================================================

	template <typename T>
	struct Decay
	{
		using type = RemoveConst_t<T>;
	};

	template <typename T>
	struct Decay<T&>
	{
		using type = typename Decay<RemoveRef_t<T>>::type;
	};

	template <typename T>
	struct Decay<T&&>
	{
		using type = typename Decay<RemoveRef_t<T>>::type;
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


	// ElemType
	// ========== ==========================================================

	template <typename T>
	struct ElemType
	{
		using type = T;
	};

	template <typename T>
	struct ElemType<T&>
	{
		using type = typename ElemType<RemoveRef_t<T>>::type;
	};

	template <typename T>
	struct ElemType<T*>
	{
		using type = T;
	};

	template <typename T, unsigned N>
	struct ElemType<T[N]>
	{
		using type = T;
	};

	template <typename T>
	using ElemType_t = typename ElemType<T>::type;


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

	template <auto V, auto ... Vs>
	struct IsAnySameV : Constant<bool, false> {};

	template <auto L, auto R, auto ... Vs>
	struct IsAnySameV<L, R, Vs...> : 
		Constant <bool, L == R || IsAnySameV<L, Vs...>::value> 
	{};


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

	template <typename Ty>
	constexpr inline auto IsIntegral_v = IsIntegral<Ty>::value;


	// Is Float
	// =========== =========================================================

	template <typename T>
	struct IsFloatType : Constant<bool,
		IsSame<float, T>	   ::value ||
		IsSame<double, T>	   ::value ||
		IsSame<long double, T> ::value
	> {};

	template <typename T>
	inline constexpr auto IsFloatType_v = IsFloatType<T>::value;


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


	// RemoveSigned
	// ============== ======================================================

	template <typename T>
	struct MakeUnsigned
	{
		using type = T;
	};

	template<>
	struct MakeUnsigned<char>
	{
		using type = unsigned char;
	};

	template<>
	struct MakeUnsigned<short>
	{
		using type = unsigned short;
	};

	template<>
	struct MakeUnsigned<int>
	{
		using type = unsigned int;
	};

	template<>
	struct MakeUnsigned<long>
	{
		using type = unsigned long;
	};

	template<>
	struct MakeUnsigned<long long>
	{
		using type = unsigned long long;
	};

	template <typename T>
	using MakeUnsigned_t = typename MakeUnsigned<T>::type;


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


	// Is Reference
	// ============== ======================================================

	template <typename T>
	struct IsReference : Constant <bool, false>
	{};

	template <typename T>
	struct IsReference<T&> : Constant <bool, true>
	{};

	template <typename T>
	struct IsReference<T&&> : Constant <bool, true>
	{};


	// Is C String
	// ============== ======================================================

	template <typename T>
	struct IsCString : Constant <bool,
		IsSame<RemoveLowConst_t<Decay_t<T>>, char*>::value
	>
	{

	};

	template <typename T>
	struct IsWString : Constant <bool, 
		IsSame<RemoveLowConst_t<Decay_t<T>>, wchar_t*>::value
	>
	{

	};


	// Force Evaulate	   Warning: May cause the compiler to crash
	// ================ ====================================================

	// Warning: May crash the compiler
	template <typename T, T value>
	struct ForceEval : Constant <T, value>
	{};
	
	// Warning: May crash the compiler
	template <auto _Val>
	auto ForceEval_v = ForceEval<decltype(_Val), _Val>::value;


	// Type	   
	// ====== ==============================================================

	template <typename Ty>
	struct Type
	{
		using type = Ty;
		using result = Ty;
	};

	template <>
	struct Type<NULL_TYPE>
	{
	};

	template <typename T>
	using Type_t = typename Type<T>::type;


	// ConvertType	   
	// ============ ========================================================

	template <typename T, typename ... Ty>
	struct ConvertType : public Type <decltype(Helper::ConversionSelector<Ty...>{}(declval<Ut::Decay_t<T>>()))>
	{

	};

	template <typename T, typename ... Ty>
	using ConvertType_t = typename ConvertType<T, Ty...>::type;


	// IsConvertible	   
	// ============== ======================================================

	template <typename From, typename To>
	struct IsConvertible
	{
	private:
		static int  Converter(To);
		static int (&(Converter(...)))[999];

	public:
		static constexpr bool value = sizeof(decltype(Converter(declval<From>()))) == sizeof(int);
	};


	// TryCast	   
	// ============== ======================================================

	template <typename From, typename To, typename = To>
	struct TryCaster : public Ut::Constant<bool, false>
	{};

	template <typename From, typename To>
	struct TryCaster<From, To, Type_t<decltype(static_cast<To>(declval<From>()))>> : public Constant<bool, true>
	{};

	template <typename Ty, typename U>
	inline constexpr bool TryCast(U&&) { return Ut::TryCaster<U, Ty, Ty>::value; }
	template <typename Ty>
	inline constexpr bool TryCast(Ty&&) { return true; }


	// HasVTable	   
	// ============== ======================================================

	template <typename T>
	struct HasVTable
	{
	private:
		struct Base : Ut::RemoveRef_t<T> { virtual void dummy() {}; };

	public:
		static constexpr bool value = sizeof(Base) == sizeof(Ut::RemoveRef_t<T>);
	};


	// IsEmptyClass	   
	// ============== ======================================================

	// TODO: Multiple and virtual inheritance
	template <typename T>
	struct IsEmptyClass
	{
	private:
		struct Base : T { char dummy; };

		constexpr static bool eval(void)
		{
			if constexpr (HasVTable<T>::value)
			{
				return sizeof(T) == sizeof(void*);
			}
			else
			{
				return sizeof(Base) == sizeof(T);
			}
		}

	public:
		constexpr static bool value = eval();
	};
}



#endif		// INCLUDE GUARD

