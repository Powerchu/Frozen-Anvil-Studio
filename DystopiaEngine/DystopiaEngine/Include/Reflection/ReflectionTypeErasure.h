#pragma once
#ifndef _REFLECTION_TYPE_ERASURE_H
#define _REFLECTION_TYPE_ERASURE_H

#include "Globals.h"
#include "DataStructure/SharedPtr.h"
#include "Utility/Utility.h"
#include "Allocator/Allocator.h"
#include "Reflection/MetaData.h"

#include <string>
#include <map>
namespace Dystopia
{

	namespace TypeErasure
	{
		template<typename T>
		struct Type
		{
			using type = T;
		};

		template<typename T>
		struct TypeDescriptor : Type<T>
		{

		};

		struct TypeEraseMetaData;
		struct ReadWriteObject;

		struct ReadWriteObject
		{
		protected:

			struct Concept
			{
				virtual Concept * Duplicate() const = 0;
				//static std::map<int, > m;
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
					return new Wrapper{ *this };
				}

				T mObj;
			};



		public:

			template<typename T>
			void Get(T _functor) const
			{

			}

		private:
			Concept * mpWrapper = nullptr;
		};
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
					return ReadWriteObject{ mObj[_name] };
				}
				virtual ReadWriteObject const & operator[](std::string const & _name) const
				{
					
					return ReadWriteObject{ mObj[_name] };
					
				}

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
