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
			using Val_t   = Ut::Decay_t<T>;
			using uVal_t  = Ut::MakeUnsigned_t<Val_t>;
			using Child_t = ::Math::NormInt<T>;

		public:

			// ====================================== CONSTRUCTORS ======================================= // 

			constexpr inline NormInt(void) noexcept;

			template <typename U, typename = Ut::EnableIf_t<Ut::IsFloatType<Ut::Decay_t<U>>::value>>
			constexpr inline explicit NormInt(U&& _v) noexcept;
			constexpr inline explicit NormInt(Val_t const&) noexcept;

			constexpr inline NormInt(NormInt&&)      noexcept = default;
			constexpr inline NormInt(NormInt const&) noexcept = default;


			// ===================================== MEMBER FUNCTIONS ==================================== // 

			template <typename U>
			constexpr inline U As(void) const noexcept;

			constexpr inline Ut::Decay_t<T> Raw(void) const noexcept;
			constexpr inline float Float(void) const noexcept;


			// ===================================== FLOAT OPERATIONS ==================================== // 

			constexpr inline auto operator - (void) const noexcept -> Ut::EnableIf_t<Ut::IsSigned<T>::value, Child_t>;

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


			constexpr inline NormInt& operator = (NormInt&&) noexcept;
			constexpr inline NormInt& operator = (NormInt const&) noexcept;

			template <typename U>
			constexpr inline decltype(auto) operator  = (Math::NormInt<U> const&) noexcept;
			template <typename U>
			constexpr inline decltype(auto) operator *= (Math::NormInt<U> const&) noexcept;
			template <typename U>
			constexpr inline decltype(auto) operator /= (Math::NormInt<U> const&) noexcept;
			template <typename U>
			constexpr inline decltype(auto) operator += (Math::NormInt<U> const&) noexcept;
			template <typename U>
			constexpr inline decltype(auto) operator -= (Math::NormInt<U> const&) noexcept;

			template <typename U>
			constexpr inline auto operator <  (U&&) const noexcept -> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>;
			template <typename U>
			constexpr inline auto operator == (U&&) const noexcept -> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>;
			template <typename U>
			constexpr inline auto operator != (U&&) const noexcept -> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>;

			template <typename U>
			constexpr inline bool operator < (Math::NormInt<U> const&) const noexcept;
			template <typename U>
			constexpr inline bool operator == (Math::NormInt<U> const&) const noexcept;
			template <typename U>
			constexpr inline bool operator != (Math::NormInt<U> const&) const noexcept;

			constexpr operator float(void) const noexcept;

		private:
			uVal_t value;
		};
	}

	template <typename T>
	struct NormInt : Internal::NormInt<NormInt<T>, T>
	{
		using Internal::NormInt<NormInt<T>, T>::NormInt;
	};


	// ======================================== OPERATORS ======================================== // 

	template <typename T, typename U>
	constexpr inline auto operator - (NormInt<T> const&, NormInt<U> const&) noexcept;
	template <typename T, typename U>
	constexpr inline auto operator + (NormInt<T> const&, NormInt<U> const&) noexcept;
	template <typename T, typename U>
	constexpr inline auto operator * (NormInt<T> const&, NormInt<U> const&) noexcept;
	template <typename T, typename U>
	constexpr inline auto operator / (NormInt<T> const&, NormInt<U> const&) noexcept;

	template <typename T, typename U>
	constexpr inline NormInt<T> operator - (U&&, NormInt<T> const&) noexcept;
	template <typename T, typename U>
	constexpr inline NormInt<T> operator + (U&&, NormInt<T> const&) noexcept;
	template <typename T, typename U>
	constexpr inline NormInt<T> operator * (U&&, NormInt<T> const&) noexcept;
	template <typename T, typename U>
	constexpr inline NormInt<T> operator / (U&&, NormInt<T> const&) noexcept;
	
	template <typename T, typename U>
	constexpr inline NormInt<T> operator - (NormInt<T> const&, U&&) noexcept;
	template <typename T, typename U>
	constexpr inline NormInt<T> operator + (NormInt<T> const&, U&&) noexcept;
	template <typename T, typename U>
	constexpr inline NormInt<T> operator * (NormInt<T> const&, U&&) noexcept;
	template <typename T, typename U>
	constexpr inline NormInt<T> operator / (NormInt<T> const&, U&&) noexcept;
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
	: value{ static_cast<uVal_t>(_v) }
{
}

template <typename T> template <typename U, typename>
inline constexpr NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::NormInt(U&& _v) noexcept
	: value{ static_cast<uVal_t>(_v * std::numeric_limits<Ut::RemoveRef_t<Val_t>>::max()) }
{
}

template <typename T>
inline constexpr Ut::Decay_t<T> NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::Raw(void) const noexcept
{
	return static_cast<Val_t>(value);
}

template<typename T>
inline constexpr float NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::Float(void) const noexcept
{
	return As<float>();
}


template <typename T> template <typename U>
inline constexpr U NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::As(void) const noexcept
{
	static_assert(Ut::IsFloatType<U>::value || Ut::IsSame<Val_t, U>::value || Ut::IsSame<uVal_t, U>::value,
		"Must be converted to floating point type!");

	if constexpr (Ut::IsFloatType<U>::value)
	{
		if constexpr (Ut::IsSigned<Val_t>::value)
		{
			auto a = static_cast<Val_t>(value);
			return Ut::Max((a * Ut::Decay_t<U>{ 1 }) / std::numeric_limits<Val_t>::max(), Ut::Decay_t<U>{ -1 });
		}
		else
		{
			return (value * Ut::Decay_t<U>{ 1 }) / std::numeric_limits<Val_t>::max();
		}
	}
	else
	{
		return static_cast<U>(value);
	}
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


template<typename T>
inline constexpr NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator float(void) const noexcept
{
	return As<float>();
}


template<typename T>
inline constexpr NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>&
NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator = (NormInt&& _rhs) noexcept
{
	value = Ut::Move(_rhs.value);
	return *this;
}

template<typename T>
inline constexpr NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>&
NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator = (NormInt const& _rhs) noexcept
{
	value = _rhs.value;
	return *this;
}


template <typename T>
inline constexpr auto NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator - (void) const noexcept
	-> Ut::EnableIf_t<Ut::IsSigned<T>::value, Child_t>
{
	return Child_t{ ~value + 1 };
}

template <typename T> template <typename U>
inline constexpr auto NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator = (U&& _v) noexcept
	-> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>
{
	return static_cast<Child_t&>(*this = NormInt{ Ut::Fwd<U>(_v) });
}

template <typename T> template <typename U>
inline constexpr auto NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator *= (U&& _v) noexcept
	-> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>
{
	value = Ut::Round<uVal_t>(As<Val_t>() * _v);

	return static_cast<Child_t&>(*this);
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
	return static_cast<Child_t&>(*this += Child_t{ Ut::Fwd<U>(_v) });
}

template <typename T> template <typename U>
inline constexpr auto NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator -= (U&& _v) noexcept
	-> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>
{
	return static_cast<Child_t&>(*this -= Child_t{ Ut::Fwd<U>(_v) });
}


template <typename T> template <typename U>
inline constexpr decltype(auto) NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator = (Math::NormInt<U> const& _rhs) noexcept
{
	if constexpr (Ut::IsSame<Ut::Decay_t<U>, Val_t>::value)
	{
		value = _rhs.As<uVal_t>();
		return static_cast<Child_t&>(*this);
	}
	else
	{
		return *this = _rhs.As<double>();
	}
}

template <typename T> template <typename U>
inline constexpr decltype(auto) NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator *= (Math::NormInt<U> const& _rhs) noexcept
{
	return *this *= _rhs.As<double>();
}

template <typename T> template <typename U>
inline constexpr decltype(auto) NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator /= (Math::NormInt<U> const& _rhs) noexcept
{
	return *this /= _rhs.As<double>();
}

template <typename T> template <typename U>
inline constexpr decltype(auto) NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator += (Math::NormInt<U> const& _rhs) noexcept
{
	if constexpr (Ut::IsSame<uVal_t, Ut::MakeUnsigned_t<Ut::Decay_t<U>>>::value)
	{
		value += _rhs.As<uVal_t>();
		return static_cast<Child_t&>(*this);
	}
	else
		return *this = _rhs.As<double>();
}

template <typename T> template <typename U>
inline constexpr decltype(auto) NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator -= (Math::NormInt<U> const& _rhs) noexcept
{
	if constexpr (Ut::IsSame<uVal_t, Ut::MakeUnsigned_t<Ut::Decay_t<U>>>::value)
	{
		value -= _rhs.As<uVal_t>();
		return static_cast<Child_t&>(*this);
	}
	else
		return *this = _rhs.As<double>();
}

template <typename T>  template <typename U>
inline constexpr auto NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator < (U&& _rhs) const noexcept
	-> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>
{
	return As<Ut::Decay_t<U>>() < _rhs;
}

template <typename T>  template <typename U>
inline constexpr auto NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator == (U&& _rhs) const noexcept
	-> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>
{
	return Ut::ApproxEq(As<Ut::Decay_t<U>>(), Ut::Fwd<U>(_rhs));
}

template <typename T> template <typename U>
inline constexpr auto NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator != (U&& _rhs) const noexcept
	 -> Ut::EnableIf_t<Ut::IsFloatType_v<Ut::Decay_t<U>>, Child_t&>
{
	return !(this->operator == (Ut::Fwd<U>(_rhs)));
}

template <typename T> template <typename U>
inline constexpr bool NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator < (Math::NormInt<U> const& _rhs) const noexcept
{
	if constexpr (Ut::IsSame<Val_t, Ut::Decay_t<U>>::value)
		return value == _rhs.As<uVal_t>();
	else
		return As<float>() < _rhs.As<float>();
}

template <typename T> template <typename U>
inline constexpr bool NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator == (Math::NormInt<U> const& _rhs) const noexcept
{
	if constexpr (Ut::IsSame<Val_t, Ut::Decay_t<U>>::value)
		return value == _rhs.As<uVal_t>();
	else
		return Ut::ApproxEq(As<float>(), _rhs.As<float>());
}

template <typename T> template <typename U>
inline constexpr bool NormInt<::Math::NormInt<T>, Ut::EnableIf_t<Ut::IsIntegral_v<T>, T>>::operator != (Math::NormInt<U> const& _rhs) const noexcept
{
	return !(*this == _rhs);
}
} // namespace Math::Internal


template <typename T, typename U>
constexpr auto Math::operator - (Math::NormInt<T> const& _lhs, Math::NormInt<U> const& _rhs) noexcept
{
	if constexpr (sizeof(T) > sizeof(U))
		return Math::NormInt<T>(_lhs) -= _rhs;
	else
		return Math::NormInt<U>(_rhs) -= _lhs;
}

template <typename T, typename U>
constexpr auto Math::operator + (Math::NormInt<T> const& _lhs, Math::NormInt<U> const& _rhs) noexcept
{
	if constexpr (sizeof(T) > sizeof(U))
		return Math::NormInt<T>(_lhs) += _rhs;
	else
		return Math::NormInt<U>(_rhs) += _lhs;
}

template <typename T, typename U>
constexpr auto Math::operator * (Math::NormInt<T> const& _lhs, Math::NormInt<U> const& _rhs) noexcept
{
	if constexpr (sizeof(T) > sizeof(U))
		return Math::NormInt<T>(_lhs) *= _rhs;
	else
		return Math::NormInt<U>(_rhs) *= _lhs;
}

template <typename T, typename U>
constexpr auto Math::operator / (Math::NormInt<T> const& _lhs, Math::NormInt<U> const& _rhs) noexcept
{
	if constexpr (sizeof(T) > sizeof(U))
		return Math::NormInt<T>(_lhs) /= _rhs;
	else
		return Math::NormInt<U>(_rhs) /= _lhs;
}

template <typename T, typename U>
constexpr Math::NormInt<T> Math::operator + (U&& _lhs, Math::NormInt<T> const& _rhs) noexcept
{
	return NormInt<T>(_rhs) += Ut::Fwd<U>(_lhs);
}

template <typename T, typename U>
constexpr Math::NormInt<T> Math::operator + (Math::NormInt<T> const& _lhs, U&& _rhs) noexcept
{
	return NormInt<T>(_lhs) += Ut::Fwd<U>(_rhs);
}

template <typename T, typename U>
constexpr Math::NormInt<T> Math::operator - (U&& _lhs, Math::NormInt<T> const& _rhs) noexcept
{
	return NormInt<T>(_rhs) -= Ut::Fwd<U>(_lhs);
}

template <typename T, typename U>
constexpr Math::NormInt<T> Math::operator - (Math::NormInt<T> const& _lhs, U&& _rhs) noexcept
{
	return NormInt<T>(_lhs) -= Ut::Fwd<U>(_rhs);
}

template <typename T, typename U>
constexpr Math::NormInt<T> Math::operator * (U&& _lhs, Math::NormInt<T> const& _rhs) noexcept
{
	return NormInt<T>(_rhs) *= Ut::Fwd<U>(_lhs);
}

template <typename T, typename U>
constexpr Math::NormInt<T> Math::operator * (Math::NormInt<T> const& _lhs, U&& _rhs) noexcept
{
	return NormInt<T>(_lhs) *= Ut::Fwd<U>(_rhs);
}

template <typename T, typename U>
constexpr Math::NormInt<T> Math::operator / (U&& _lhs, Math::NormInt<T> const& _rhs) noexcept
{
	return NormInt<T>(_rhs) /= Ut::Fwd<U>(_lhs);
}

template <typename T, typename U>
constexpr Math::NormInt<T> Math::operator / (Math::NormInt<T> const& _lhs, U&& _rhs) noexcept
{
	return NormInt<T>(_lhs) /= Ut::Fwd<U>(_rhs);
}



#endif		// INCLUDE GUARD

