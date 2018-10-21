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
#include "Reflection/ReadWriteObject.h"

#include <map>

#define STRINGIFY(_NAME_) #_NAME_
#define PP_REFLECT(_STRUCT_, ...)                                                                                             \
template <>                                                                                                                   \
struct MetaData<_STRUCT_>                                                                                         \
{                                                                                                                             \
																                                                              \
	using Map_t = std::map<char const*, Dystopia::TypeErasure::ReadWriteObject>;                                              \
    Map_t mMetaMap = {PP_FOREACH(REFLECT_AUX, (_STRUCT_), __VA_ARGS__)};                                                      \
											                                                                                  \
																															  \
};                                                                                                                            \
//MetaData<_STRUCT_>::Map_t MetaData<_STRUCT_>::mMetaMap {PP_FOREACH(REFLECT_AUX, (_STRUCT_), __VA_ARGS__)   };                 

template <typename T>
struct  GET_MEMPTR_TYPE;

template <typename T, typename C>
struct  GET_MEMPTR_TYPE<T C::*>
{
	using type = T;
};



#define GENERATE_REFLECT_DATA(_TYPE_, _MEMBER_) ReflectedData<_TYPE_, typename GET_MEMPTR_TYPE<decltype(&_TYPE_::_MEMBER_)>::type >{ &_TYPE_::_MEMBER_ }

#define REFLECT_AUX(_TYPE_, _MEMBER_)           std::pair<const char*,Dystopia::TypeErasure::ReadWriteObject> { STRINGIFY(_MEMBER_),  GENERATE_REFLECT_DATA(_TYPE_,_MEMBER_).Get() } \

//#define REFLECT_AUX(_TYPE_, _MEMBER_) std::pair<char const*, decltype(&_TYPE_::_MEMBER_)>{STRINGIFY(_MEMBER_), &_TYPE_::_MEMBER_}

#endif		// INCLUDE GUARD

/*
 * Expanded to
 * 

template <>                                                     
class MetaData<_STRUCT_>                                        
{                                                               
																
	static std::map<char const*, Dystopia::TypeErasure::ReadWriteObject> mMetaMap;     						                    
}

MetaData<_STRUCT_>::mMetaMap 
{ 
	std::pair<const char *, ReflectData<_STRUCT_, float>{float _STRUCT_::*>{"MyMember", &_STRUCT_::MyMember}.Get() } 
};

 */