/* HEADER *********************************************************************************/
/*!
\file	EnumOps.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Generic Operators for enums

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _ENUM_OPS_H_
#define _ENUM_OPS_H_

#include "Utility/Meta.h"

#include <type_traits>   // is_enum, underlying_type


namespace Ut
{
	template <typename T>
	struct EnumBitOps
	{
		static constexpr bool value = false;
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename Ty, typename = Ut::EnableIf_t<std::is_enum_v<Ty>>>
inline auto operator | (Ty const& _lhs, Ty const& _rhs) noexcept -> Ut::EnableIf_t<Ut::EnumBitOps<Ty>::value, Ty>
{
	using T = std::underlying_type_t<Ty>;
	return static_cast<Ty>(static_cast<T>(_lhs) | static_cast<T>(_rhs));
}

template <typename Ty, typename = Ut::EnableIf_t<std::is_enum_v<Ty>>>
inline auto operator & (Ty const& _lhs, Ty const& _rhs) noexcept -> Ut::EnableIf_t<Ut::EnumBitOps<Ty>::value, Ty>
{
	using T = std::underlying_type_t<Ty>;
	return static_cast<Ty>(static_cast<T>(_lhs) & static_cast<T>(_rhs));
}

template <typename Ty, typename = Ut::EnableIf_t<std::is_enum_v<Ty>>>
inline auto operator ^ (Ty const& _lhs, Ty const& _rhs) noexcept -> Ut::EnableIf_t<Ut::EnumBitOps<Ty>::value, Ty>
{
	using T = std::underlying_type_t<Ty>;
	return static_cast<Ty>(static_cast<T>(_lhs) ^ static_cast<T>(_rhs));
}

template <typename Ty, typename = Ut::EnableIf_t<std::is_enum_v<Ty>>>
inline auto operator ~ (Ty const& _v) noexcept -> Ut::EnableIf_t<Ut::EnumBitOps<Ty>::value, Ty>
{
	using T = std::underlying_type_t<Ty>;
	return static_cast<Ty>(~static_cast<T>(_v));
}

template <typename Ty, typename = Ut::EnableIf_t<std::is_enum_v<Ty>>>
inline auto operator |= (Ty& _lhs, Ty const& _rhs) noexcept -> Ut::EnableIf_t<Ut::EnumBitOps<Ty>::value, Ty&>
{
	return _lhs = _lhs | _rhs;
}

template <typename Ty, typename = Ut::EnableIf_t<std::is_enum_v<Ty>>>
inline auto operator &= (Ty& _lhs, Ty const& _rhs) noexcept -> Ut::EnableIf_t<Ut::EnumBitOps<Ty>::value, Ty&>
{
	return _lhs = _lhs & _rhs;
}

template <typename Ty, typename = Ut::EnableIf_t<std::is_enum_v<Ty>>>
inline auto operator ^= (Ty& _lhs, Ty const& _rhs) noexcept -> Ut::EnableIf_t<Ut::EnumBitOps<Ty>::value, Ty&>
{
	return _lhs = _lhs ^ _rhs;
}



#endif      // INCLUDE GUARD

