#pragma once
#ifndef _READ_WRITE_OBJECT_H
#define _READ_WRITE_OBJECT_H

#include <functional>
#include <any>

#include "Globals.h"
#include "DataStructure/SharedPtr.h"
namespace Dystopia
{
	namespace TypeErasure
	{

		namespace HelperMeta
		{

			template <typename Func, typename T>
			struct GenerateFuncMapPair
			{
				using PairType = std::pair<decltype(typeid(T).hash_code()), void(*)(const std::any&, Func)>;
				operator PairType()
				{
					return PairType
					{
						typeid(T).hash_code(),
						[](const std::any& _a, Func _f) {_f(std::any_cast<T>(_a)); }
					};
				}
			};
		}

		struct ReadWriteObject
		{
		protected:

			struct Concept
			{
				virtual void Set(std::function<void(std::any)> _functor) = 0;
				virtual void Get(std::function<void(std::any)> _functor) = 0;
				virtual void Get(void *, void (*) (std::any))            = 0;
				virtual Concept * Duplicate() const = 0;
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

				virtual void Set(std::function<void(std::any)> _functor) override
				{
					_functor(mObj);
				}
				virtual void Get(void *, void(*_fp) (std::any)) override
				{
					_fp(mObj.mGet(/*his address*/));
				}

				T mObj;
			};



		public:

			template<typename T, Ut::EnableIf_t< !Ut::IsSame<Ut::Decay_t<T>, ReadWriteObject>::value>>
			ReadWriteObject(T && _Obj)
				:mpWrapper{ new Wrapper<T>{ Ut::Forward<T &&>(_Obj) } }
			{

			}

			ReadWriteObject(ReadWriteObject const & _TypeEraseRhs);

			ReadWriteObject(ReadWriteObject  && _TypeEraseRhs);


			template <typename SF>
			void Get(void * _Address ,SF)
			{
				void(*passdown)(const std::any&) = ResolveType<SF>;
				mpWrapper->Get(_Address, passdown);
			}

			void Set(std::function<void(std::any)> _SuperFunctor);

		private:
			SharedPtr<Concept> mpWrapper;

			template<typename SuperFunctor>
			static void ResolveType(std::any _a)
			{
				static using FunctorMap = std::map<decltype(typeid(int)), void(*)(std::any, SuperFunctor) >;
				static FunctorMap MyMap = GenerateMap<SuperFunctor>(MetaData<T>::PossibleTypes{});

				auto it = MyMap.find(_a.type().hash_code());
				if (it != MyMap.end())
				{
					it.second(_a, SuperFunctor{});
				}
				else
				{
					// Don't exist
					SuperFunctor{}(std::nullptr_t{nullptr});
				}
			}

			template<typename SuperFunctor, typename ... T>
			auto GenerateMap(Variant<T...>)
			{
				using FunctorMap = std::map<decltype(typeid(int).hash_code()), void(*)(std::any, SuperFunctor)>;
				return FunctorMap
				{
					HelperMeta::GenerateFuncMapPair<SuperFunctor, T>{}...
				}
			}

		};

	}
}




#endif