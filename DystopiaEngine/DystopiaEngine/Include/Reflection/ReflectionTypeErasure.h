#pragma once
#ifndef _REFLECTION_TYPE_ERASURE_H
#define _REFLECTION_TYPE_ERASURE_H

#include "Globals.h"
#include "DataStructure/SharedPtr.h"
#include "Utility/Utility.h"
#include "Allocator/Allocator.h"
#include "Reflection/MetaData.h"
#include "ReadWriteObject.h"
#include "Reflection.h"

#include <string>
#include <map>
#include <functional>
#include <any>
#include <typeinfo>
namespace Dystopia
{

	namespace TypeErasure
	{

		struct TypeEraseMetaData;



		struct TypeEraseMetaData
		{
		protected:
			struct Concept
			{
				virtual Concept * Duplicate() const = 0;
				virtual ReadWriteObject       & operator[](std::string const & _name)       = 0;
				virtual ReadWriteObject const & operator[](std::string const & _name) const = 0;


				virtual ~Concept() {}
			};

			template<typename T>
			struct Wrapper : Concept
			{
				template<typename U>
				Wrapper(U && _obj)
					:mObj{ _obj }
				{

				}

				virtual Wrapper * Duplicate() const
				{
					return new Wrapper<T>{*this};
				}

				T mObj;
			};

			template<typename T>
			struct Wrapper<MetaData<T>> : Concept
			{
				template<typename U>
				Wrapper(U && _obj)
					:mObj{ _obj }
				{

				}

				virtual Wrapper * Duplicate() const
				{
					return new Wrapper<MetaData<T>>{*this};
				}

				virtual ReadWriteObject       & operator[](std::string const & _name)
				{
					return ReadWriteObject{  };
				}
				virtual ReadWriteObject const & operator[](std::string const & _name) const
				{
					return ReadWriteObject{  };
				}

			private:

				MetaData<T> mObj;
			};

		public:

			template<typename T, Ut::EnableIf_t< !Ut::IsSame<Ut::Decay_t<T>, TypeEraseMetaData>::value>>
			TypeEraseMetaData(T && _Obj)
				:mpWrapper{ new Wrapper<T>{Ut::Forward<T &&>(_Obj)} }
			{

			}

			TypeEraseMetaData(TypeEraseMetaData const & _TypeEraseRhs);

			TypeEraseMetaData(TypeEraseMetaData  && _TypeEraseRhs);

			~TypeEraseMetaData();

			ReadWriteObject&        operator[](std::string const & _name);
			ReadWriteObject const & operator[](std::string const & _name) const;


		private:
			Concept* mpWrapper = nullptr;
		};
	}


}



#endif
