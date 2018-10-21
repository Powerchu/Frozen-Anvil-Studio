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
#include "DataStructure/AutoArray.h"
#include <string>
#include <map>
#include <functional>
#include <any>
#include <typeinfo>

namespace Dystopia
{

	namespace TypeErasure
	{

		struct _DLL_EXPORT TypeEraseMetaData
		{
		protected:
			struct _DLL_EXPORT Concept
			{
				virtual Concept * Duplicate() const = 0;
				virtual ReadWriteObject       & operator[](std::string const & _name)       = 0;
				virtual ReadWriteObject const & operator[](std::string const & _name) const = 0;
				virtual std::map<char const*, ReadWriteObject>&        GetAllReflectedData()       = 0;
				virtual std::map<const char*, ReadWriteObject> const & GetAllReflectedData() const = 0;

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

				virtual ReadWriteObject & operator[](std::string const & _name)
				{
					if (mObj.mMetaMap.find(_name.c_str()) != mObj.mMetaMap.end())
						return mObj.mMetaMap[_name.c_str()];
					return ReadWriteObject{};
				}
				virtual ReadWriteObject const & operator[](std::string const & _name) const
				{
					if (mObj.mMetaMap.find(_name.c_str()) != mObj.mMetaMap.end())
						return mObj.mMetaMap[_name.c_str()];
					return ReadWriteObject{};
				}
				virtual std::map<char const*, ReadWriteObject>& GetAllReflectedData() override
				{
					return mObj.mMetaMap;
				}
				virtual std::map<const char*, ReadWriteObject> const & GetAllReflectedData() const override
				{
					return mObj.mMetaMap;
				}
				virtual ~Wrapper() {}

				T mObj;
			};

		public:

			template<typename T, typename SFINAE = Ut::EnableIf_t< !Ut::IsSame<Ut::Decay_t<T>, TypeEraseMetaData>::value>>
			TypeEraseMetaData(T && _Obj)
				:mpWrapper{ new Wrapper<T>{Ut::Forward<T>(_Obj)} }
			{

			}

			TypeEraseMetaData(void);

			TypeEraseMetaData(TypeEraseMetaData const & _TypeEraseRhs);

			TypeEraseMetaData(TypeEraseMetaData  && _TypeEraseRhs);

			~TypeEraseMetaData();

			ReadWriteObject&        operator[](std::string const & _name);
			ReadWriteObject const & operator[](std::string const & _name) const;
			std::map<char const*, ReadWriteObject>&        GetAllReflectedData();
			std::map<const char*, ReadWriteObject> const & GetAllReflectedData() const;

			operator bool() const;

			Concept* GetPointer();
		private:
			Concept* mpWrapper = nullptr;
		};
	}


}



#endif
