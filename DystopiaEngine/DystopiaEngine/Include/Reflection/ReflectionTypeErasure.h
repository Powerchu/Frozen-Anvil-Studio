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
				virtual ReadWriteObject        operator[](const char * _name)       = 0;
				virtual ReadWriteObject        operator[](const char * _name) const = 0;
				//virtual std::map<char const*, ReadWriteObject, CustomMetaComp>        GetAllReflectedData()       = 0;
				//virtual std::map<const char*, ReadWriteObject, CustomMetaComp>        GetAllReflectedData() const = 0;
				virtual AutoArray<const char *> GetAllNames() const = 0;


				virtual bool isThereMatch(const char * _name) = 0;
				virtual ~Concept() = default;
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

				virtual ReadWriteObject  operator[](const char * _name)
				{
					return mObj[_name];
					//return ReadWriteObject{};
				}
				virtual ReadWriteObject operator[](const char * ) const
				{
					return ReadWriteObject{};
				}
				//virtual std::map<char const*, ReadWriteObject, CustomMetaComp> GetAllReflectedData() override
				//{
				//	return mObj.mMetaMap;
				//}
				//virtual std::map<const char*, ReadWriteObject, CustomMetaComp> GetAllReflectedData() const override
				//{
				//	return mObj.mMetaMap;
				//}
				virtual AutoArray<const char *> GetAllNames() const
				{
					AutoArray<const char *> toRet;
					for (auto const & elem : mObj.mMetaMap)
						toRet.push_back(elem.first);
					return toRet;
				}
				bool isThereMatch(const char *) 
				{
					return false;
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

			TypeEraseMetaData operator=(TypeEraseMetaData const & _TypeEraseRhs);

			TypeEraseMetaData operator=(TypeEraseMetaData && _TypeEraseRhs);

			~TypeEraseMetaData();

			ReadWriteObject        operator[](const char * _name);
			ReadWriteObject        operator[](const char * _name) const;
			//std::map<char const*, ReadWriteObject, CustomMetaComp>        GetAllReflectedData();
			//std::map<const char*, ReadWriteObject, CustomMetaComp>        GetAllReflectedData() const;
			AutoArray<const char *> GetAllNames()                               const;
			operator bool() const;
			bool isThereMatch(const char * _name)
			{
				return mpWrapper->isThereMatch(_name);
			}
			Concept* GetPointer();
		private:
			Concept* mpWrapper = nullptr;
		};
	}


}



#endif
