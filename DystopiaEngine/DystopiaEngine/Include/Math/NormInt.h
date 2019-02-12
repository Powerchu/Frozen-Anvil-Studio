/* HEADER *********************************************************************************/
/*!
\file	NormInt.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Normalised Integer helper class.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _MATH_NORMINT_H_
#define _MATH_NORMINT_H_

#include "Globals.h"
#include "Math/MathUtility.h"

#include "Utility/Meta.h"	            // EnableIf
#include "Utility/MetaAlgorithms.h"     // MetaSortV
#include "Utility/MetaDataStructures.h" // IntegralList

#include <limits>


namespace Math
{
	template <typename T>
	struct NormInt;

	namespace Internal
	{
		template <typename, typename>
		struct NormInt;

		template <typename T>
		struct NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>
		{
		private:
			using Val_t = Ut::Decay_t<T>;
			using Child_t = ::Math::NormInt<T>;

		public:

			// ====================================== CONSTRUCTORS ======================================= // 

			constexpr inline NormInt(void) noexcept;
			constexpr inline explicit NormInt(Val_t const&) noexcept;

			template <typename U, Ut::EnableIf_t<Ut::IsFloatType<U>::value>>
			constexpr inline explicit NormInt(U&& _v) noexcept;

			constexpr inline NormInt(NormInt&&)      noexcept = default;
			constexpr inline NormInt(NormInt const&) noexcept = default;


			// ===================================== MEMBER FUNCTIONS ==================================== // 

			template <typename U>
			constexpr inline U As(void) const noexcept;

			constexpr inline Ut::Decay_t<T> Raw(void) const noexcept;


			// ===================================== FLOAT OPERATIONS ==================================== // 

			constexpr inline auto operator -  (void) const noexcept -> Ut::EnableIf_t<Ut::IsSigned<T>::value, NormInt>;

			template <typename U>
			constexpr inline auto operator  = (U&&) noexcept -> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>;
			template <typename U>
			constexpr inline auto operator *= (U&&) noexcept -> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>;
			template <typename U>
			constexpr inline auto operator /= (U&&) noexcept -> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>;
			template <typename U>
			constexpr inline auto operator += (U&&) noexcept -> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>;
			template <typename U>
			constexpr inline auto operator -= (U&&) noexcept -> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>;

			template <typename U>
			constexpr inline decltype(auto) operator  = (Math::NormInt<U>) noexcept;
			template <typename U>
			constexpr inline decltype(auto) operator *= (Math::NormInt<U>) noexcept;
			template <typename U>
			constexpr inline decltype(auto) operator /= (Math::NormInt<U>) noexcept;
			template <typename U>
			constexpr inline decltype(auto) operator += (Math::NormInt<U>) noexcept;
			template <typename U>
			constexpr inline decltype(auto) operator -= (Math::NormInt<U>) noexcept;

			template <typename U>
			constexpr inline auto operator <  (U&&) const noexcept -> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>;
			template <typename U>
			constexpr inline auto operator == (U&&) const noexcept -> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>;
			template <typename U>
			constexpr inline auto operator != (U&&) const noexcept -> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>;

			template <typename U>
			constexpr inline bool operator <  (Math::NormInt<U>) const noexcept;
			template <typename U>
			constexpr inline bool operator == (Math::NormInt<U>) const noexcept;
			template <typename U>
			constexpr inline bool operator != (Math::NormInt<U>) const noexcept;

			constexpr operator float(void) const noexcept;

		private:
			Ut::MakeUnsigned_t<Ut::Decay_t<T>> value;
		};
	}

	template <typename T>
	struct NormInt : Internal::NormInt<NormInt<T>, T>
	{
		using Internal::NormInt<NormInt<T>, T>::NormInt;
	};


	// ======================================== OPERATORS ======================================== // 

	template <typename T, typename U>
	constexpr inline NormInt<T> operator - (NormInt<T> const&, NormInt<U> const&);
	template <typename T, typename U>
	constexpr inline NormInt<T> operator + (NormInt<T> const&, NormInt<U> const&);
	template <typename T, typename U>
	constexpr inline NormInt<T> operator * (NormInt<T> const&, NormInt<U> const&);
	template <typename T, typename U>
	constexpr inline NormInt<T> operator / (NormInt<T> const&, NormInt<U> const&);

	template <typename T>
	constexpr inline NormInt<T> operator - (float, NormInt<T>);
	template <typename T>
	constexpr inline NormInt<T> operator + (float, NormInt<T>);
	template <typename T>
	constexpr inline NormInt<T> operator * (float, NormInt<T>);
	template <typename T>
	constexpr inline NormInt<T> operator / (float, NormInt<T>);
	
	template <typename T>
	constexpr inline NormInt<T> operator - (NormInt<T>, float);
	template <typename T>
	constexpr inline NormInt<T> operator + (NormInt<T>, float);
	template <typename T>
	constexpr inline NormInt<T> operator * (NormInt<T>, float);
	template <typename T>
	constexpr inline NormInt<T> operator / (NormInt<T>, float);
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 
namespace Math::Internal {


template <typename T>
inline constexpr NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::NormInt(void) noexcept
	: value{}
{

}

template <typename T>
inline constexpr NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::NormInt(Val_t const& _v) noexcept
	: value{ static_cast<Ut::MakeUnsigned_t<Val_t>>(_v) }
{
}

template <typename T> template <typename U, Ut::EnableIf_t<Ut::IsFloatType<U>::value>>
inline constexpr NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::NormInt(U&& _v) noexcept
	: value{ _v * std::numeric_limits<Ut::RemoveRef_t<Val_t>>::max() }
{
}

template <typename T>
inline constexpr Ut::Decay_t<T> NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::Raw(void) const noexcept
{
	return reinterpret_cast<Ut::Decay_t<T>>(value);
}


template <typename T> template <typename U>
inline constexpr U NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::As(void) const noexcept
{
	static_assert(Ut::IsFloatType<U>::value || Ut::IsSame<Val_t, U>::value, "Must be converted to floating point type!");

	if constexpr (Ut::IsFloatType<U>::value)
	{
		if constexpr (Ut::IsSigned<Val_t>::value)
		{
			auto a = static_cast<Val_t>(value);

			return (Ut::Decay_t<U>{ 1 } *a) /
				(a < Ut::Constant<Val_t, 0>::value ? -std::numeric_limits<Val_t>::lowest() : std::numeric_limits<Val_t>::max());
		}
		else
		{
			return (Ut::Decay_t<U>{ 1 } *value) / std::numeric_limits<Val_t>::max();
		}
	}
	else
	{
		return static_cast<Val_t>(value);
	}
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


template<typename T>
inline constexpr NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator float(void) const noexcept
{
	return As<float>();
}

template <typename T>
inline constexpr auto NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator - (void) const noexcept
	-> Ut::EnableIf_t<Ut::IsSigned<T>::value, NormInt>
{
	return NormInt{ ~value + 1 };
}

template <typename T> template <typename U>
inline constexpr auto NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator = (U&& _v) noexcept
	-> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>
{
	return static_cast<::Math::NormInt<T>&>(*this = NormInt{ Ut::Fwd<U>(_v) });
}

template <typename T> template <typename U>
inline constexpr auto NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator *= (U&& _v) noexcept
	-> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>
{
	value = Ut::Round<decltype(value)>(As<Val_t>() * _v);

	return static_cast<::Math::NormInt<T>&>(*this);
}

template <typename T> template <typename U>
inline constexpr auto NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator /= (U&& _v) noexcept
	-> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>
{
	return (*this *= 1.f / _v);
}

template <typename T> template <typename U>
inline constexpr auto NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator += (U&& _v) noexcept
	-> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>
{
	return static_cast<::Math::NormInt<T>&>(*this += NormInt{ Ut::Fwd<U>(_v) });
}

template <typename T> template <typename U>
inline constexpr auto NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator -= (U&& _v) noexcept
	-> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>
{
	return static_cast<::Math::NormInt<T>&>(*this -= NormInt{ Ut::Fwd<U>(_v) });
}

template <typename T> template <typename U>
inline constexpr decltype(auto) NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator = (Math::NormInt<U> _rhs) noexcept
{
	return *this = _rhs.As<long double>();
}

template <typename T> template <typename U>
inline constexpr decltype(auto) NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator *= (Math::NormInt<U> _rhs) noexcept
{
	return *this *= _rhs.As<long double>();
}

template <typename T> template <typename U>
inline constexpr decltype(auto) NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator /= (Math::NormInt<U> _rhs) noexcept
{
	return *this /= _rhs.As<long double>();
}

template <typename T> template <typename U>
inline constexpr decltype(auto) NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator += (Math::NormInt<U> _rhs) noexcept
{
	return *this += _rhs.As<long double>();
}

template <typename T> template <typename U>
inline constexpr decltype(auto) NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator -= (Math::NormInt<U> _rhs) noexcept
{
	return *this -= _rhs.As<long double>();
}

template <typename T>  template <typename U>
inline constexpr auto NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator < (U&& _rhs) const noexcept
	-> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>
{
	return *this < NormInt{ Ut::Fwd<U>(_rhs) }.As<Val_t>();
}

template <typename T>  template <typename U>
inline constexpr auto NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator == (U&& _rhs) const noexcept
	-> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>
{
	return *this == NormInt{ Ut::Fwd<U>(_rhs) }.As<Val_t>();
}

template <typename T> template <typename U>
inline constexpr auto NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator != (U&& _rhs) const noexcept
	 -> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>
{
	return !(this->operator == (Ut::Fwd<U>(_rhs)));
}


}
#endif		// INCLUDE GUARD

