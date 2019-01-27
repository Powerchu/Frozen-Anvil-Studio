/* HEADER *********************************************************************************/
/*!
\file	MetaLogicOps.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	TODO: Rename this file

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _META_LOGIC_OPS_H_
#define _META_LOGIC_OPS_H_

#define VALUE = static constexpr auto value
#define VAR_DECL(STRUCT_, ...) \
template <__VA_ARGS__>         \
static inline constexpr auto STRUCT_##_v


namespace Ut
{
  template <auto V>
  struct Not
  {
    VALUE = !V;
  };
  VAR_DECL(Not, auto V) = Not<V>::value;
  
  template <auto ... Vs>
  struct And
  {
    VALUE = (Vs && ...);
  };
  VAR_DECL(And, auto ... Vs) = And<Vs...>::value;
  
  template <auto ... Vs>
  struct Or
  {
    VALUE = (Vs || ...);
  };
  VAR_DECL(Or, auto ... Vs) = Or<Vs...>::value;
  
  template <auto A, auto B>
  struct Xor
  {
    VALUE = And_v<Not_v<And_v<A, B>>, Or_v<A, B>>;
  };
  VAR_DECL(Xor, auto A, auto B) = Xor<A, B>::value;


}



#undef VALUE
#undef VAR_DECL



#endif		// INCLUDE GUARD

