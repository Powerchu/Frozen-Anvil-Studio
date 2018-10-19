/* HEADER *********************************************************************************/
/*!
\file	Reflection.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	The all in one include file for reflection

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _REFLECTIONLIB_H_
#define _REFLECTIONLIB_H_

#include "Reflection/MetaData.h"
#include "Reflection/PPForEach.h"
#include "Reflection/PPStringify.h"
#include "Reflection/PPToSequence.h"
#include "Reflection/ReflectedData.h"


#define PP_REFLECT(_STRUCT_, ...)                                \
template <>                                                      \
class MetaData<_STRUCT_>                                         \
{                                                                \
	PP_FOREACH(REFLECT_AUX, (_STRUCT_), __VA_ARGS__)             \
											                     \
}


#define REFLECT_AUX(_TYPE_, _MEMBER_) std::pair<char const*, decltype(&_TYPE_::_MEMBER_)>{STRINGIFY(_MEMBER_), &_TYPE_::_MEMBER_}



#endif		// INCLUDE GUARD

