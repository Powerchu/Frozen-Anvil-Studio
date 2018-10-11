/* HEADER *********************************************************************************/
/*!
\file	ReflectedData.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Type erased pointer to member or getter/setter pair

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _REFLECTED_DATA_H_
#define _REFLECTED_DATA_H_

#include "Utility/Meta.h"
#include "Utility/Utility.h"


template <typename C, typename Ty>
struct ReflectedData
{
public:

	ReflectedData(Ty C::*);

	template <typename Get_t, typename Set_t>
	ReflectedData(Get_t&&, Set_t&&);




private:

	template <typename T>
	struct DefaultGet
	{
		T operator () (C* _obj) const
		{
			return _obj->*mMemPtr;
		}

		T C::*mMemPtr;
	};

	template <typename T>
	struct DefaultSet
	{
		T operator () (C* _obj, T&& _val) const
		{
			_obj->*mMemPtr = Ut::Forward<T>(_val);
		}

		T C::*mMemPtr;
	};
	
	template <typename Get_t, typename Set_t>
	struct ProxyObj
	{
		Get_t mGet;
		Set_t mSet;

		ProxyObj(Get_t, Set_t);
	};
};






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template<typename C, typename Ty>
template<typename Get_t, typename Set_t>
inline ReflectedData<C, Ty>::ProxyObj<Get_t, Set_t>::ProxyObj(Get_t _get, Set_t _set) :
	mGet{ _get }, mSet{ _set }
{

}

template <typename C, typename Ty>
ReflectedData<C, Ty>::ReflectedData(Ty C::*_ptr) :
	mData{ DefaultGet<Ty>{_ptr}, DefaultSet<Ty>{_ptr} }
{

}

template <typename C, typename Ty>
template<typename Get_t, typename Set_t>
ReflectedData::ReflectedData(Get_t&& _get, Set_t&& _set) :
	mData{ Ut::Forward<Get_t>(_get), Ut::Forward<Set_t>(_set) }
{

}



#endif		// INCLUDE GUARD

