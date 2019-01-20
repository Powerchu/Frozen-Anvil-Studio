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
#include "Reflection/ReflectedData.h"
#include "Reflection/ReadWriteObject.h"
#include "PP/PPForEach.h"
#include "PP/PPStringify.h"
#include "PP/PPVaToSeq.h"

#include <map>

struct _DLL_EXPORT CustomMetaComp
{
	bool operator()(const char * _1, const char * _2) const
	{
		return strcmp(_1, _2) < 0;
	}
};

/*
If std::map still giving problems, use
using Map_t = std::pair<char const*, Dystopia::TypeErasure::ReadWriteObject>[PP_VARIADIC_SIZE(__VA_ARGS__)];
*/

#define STRINGIFY(_NAME_) #_NAME_

#define PP_REFLECT(_STRUCT_, ...) PP_REFLECT_AUX (                 \
PP_IF(PP_IS_EMPTY(__VA_ARGS__),PP_REFLECT_EMPTY,PP_REFLECT_STUFF), \
PP_IF(PP_IS_EMPTY(__VA_ARGS__),(_STRUCT_),(_STRUCT_,__VA_ARGS__)))
#define PP_REFLECT_AUX(macro, args) PP_CONCAT(macro,args)

#define PP_REFLECT_STUFF(_STRUCT_, ...)                                                                                       \
template <>                                                                                                                   \
struct MetaData<_STRUCT_>                                                                                                     \
{                                                                                                                             \
													                                                                          \
	using Map_t = std::map<char const*, Dystopia::TypeErasure::ReadWriteObject>;                                              \
    Map_t mMetaMap = {PP_FOREACH(REFLECT_AUX, (_STRUCT_), __VA_ARGS__)};                                                      \
	Dystopia::TypeErasure::ReadWriteObject	operator[](const char * _name)									                  \
   {																														  \
		for(auto & elem :  mMetaMap)  															                              \
		   if(!strcmp(elem.first, _name)) 													                                  \
		    	return elem.second;								                                                              \
		return Dystopia::TypeErasure::ReadWriteObject{};																	  \
                                                                                                                              \
   }                                                                                                                          \
																															  \
};																															  \

//inline Dystopia::TypeErasure::TypeEraseMetaData _STRUCT_::GetMetaData()											  \
		//{																													  \
		//	static MetaData<Dystopia::_STRUCT_> mMetaData;																	  \
		//	static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };												  \
		//	return mReturn;																									  \
		//}																													  \
		//inline Dystopia::TypeErasure::TypeEraseMetaData const _STRUCT_::GetMetaData() const							      \
		//{																													  \
		//	static MetaData<Dystopia::_STRUCT_> mMetaData;																	  \
		//	static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };												  \
		//	return mReturn;																									  \
		//}																													  \

#define PP_REFLECT_EMPTY(_STRUCT_)                                                                                            \
template <>                                                                                                                   \
struct MetaData<_STRUCT_>                                                                                                     \
{                                                                                                                             \
													                                                                          \
	using Map_t = std::map<char const*, Dystopia::TypeErasure::ReadWriteObject>;                                              \
    Map_t mMetaMap;                                                                                                           \
	Dystopia::TypeErasure::ReadWriteObject	operator[](const char * _name)									                  \
   {																														  \
		for(auto & elem :  mMetaMap)  															                              \
		   if(!strcmp(elem.first, _name)) 													                                  \
		    	return elem.second;								                                                              \
		return Dystopia::TypeErasure::ReadWriteObject{};																	  \
                                                                                                                              \
   }                                                                                                                          \
																															  \
};                                                                                                                            \

		//inline Dystopia::TypeErasure::TypeEraseMetaData _STRUCT_::GetMetaData()											  \
		//{																													  \
		//	static MetaData<Dystopia::_STRUCT_> mMetaData;																	  \
		//	static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };												  \
		//	return mReturn;																									  \
		//}																													  \
		//inline Dystopia::TypeErasure::TypeEraseMetaData const _STRUCT_::GetMetaData() const								  \
		//{																													  \
		//	static MetaData<Dystopia::_STRUCT_> mMetaData;																	  \
		//	static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };												  \
		//	return mReturn;																									  \
		//}																													  \

//MetaData<_STRUCT_>::Map_t MetaData<_STRUCT_>::mMetaMap {PP_FOREACH(REFLECT_AUX, (_STRUCT_), __VA_ARGS__)   };                 

template <typename T>
struct  GET_MEMPTR_TYPE_AUX
{

};

template <typename T, typename C>
struct  GET_MEMPTR_TYPE<T C::*>
{
	using type = T;
};

template <typename C, typename T>
constexpr auto GenReflectedData(T C::* ptr)
{
	return ReflectedData<C, T>{ ptr };
}



#define GENERATE_REFLECT_DATA(_TYPE_, _MEMBER_) GenReflectedData( &_TYPE_::_MEMBER_ )

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