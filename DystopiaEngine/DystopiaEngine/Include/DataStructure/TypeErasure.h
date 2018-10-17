#pragma once
#ifndef _TYPE_ERASURE_H
#define _TYPE_ERASURE_H

#include "Globals.h"
#include "DataStructure/SharedPtr.h"
#include "Utility/Utility.h"
#include "Allocator/Allocator.h"

namespace Dystopia
{
	struct TypeErase
	{
	protected:
		struct Concept
		{
			virtual SharedPtr<Concept> Duplicate() const = 0;
		};

		template<typename T>
		struct Wrapper : Concept
		{
			template<typename U>
			Wrapper(U && _obj)
				:mObj{ _obj }
			{

			}

			virtual SharedPtr<Wrapper> Duplicate() const
			{
				//return new (operator new(sizeof(Wrapper))) Wrapper{ *this };
				CreateShared<Wrapper>(Wrapper<T>{*this});
			}

			T mObj;
		};

	public:

		template<typename T, Ut::EnableIf_t< !Ut::IsSame<Ut::Decay_t<T>, TypeErase>::value>>
		TypeErase(T && _Obj)
			:mpWrapper{ CreateShared<Wrapper<T>>(Wrapper<T>{Ut::Forward<T &&>(_Obj)}) }
		{

		}

		TypeErase(TypeErase const & _TypeEraseRhs)
			:mpWrapper{ _TypeEraseRhs.mpWrapper->Duplicate() }
		{

		}

		TypeErase(TypeErase  && _TypeEraseRhs)
			:mpWrapper{ _TypeEraseRhs.mpWrapper }
		{

		}

	private:
		SharedPtr<Concept> mpWrapper;
	};

	//struct PointerErasure : TypeErase
	//{

	//};
}



#endif
