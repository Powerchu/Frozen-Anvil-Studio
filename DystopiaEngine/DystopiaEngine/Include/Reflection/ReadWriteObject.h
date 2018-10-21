#pragma once
#ifndef _READ_WRITE_OBJECT_H
#define _READ_WRITE_OBJECT_H

#include <functional>
#include <any>

#include "Globals.h"
#include "DataStructure/SharedPtr.h"
#define _ALL_TYPE_QUALIFERS_(_TYPE_) _TYPE_, const _TYPE_, _TYPE_ *, const _TYPE_ *, _TYPE_ * const, _TYPE_ const * const
namespace Dystopia
{
	namespace TypeErasure
	{

		namespace HelperMeta
		{

			struct AllPossibleType
			{
				using type = Variant < _ALL_TYPE_QUALIFERS_(int), 
				                       _ALL_TYPE_QUALIFERS_(float),
				                       _ALL_TYPE_QUALIFERS_(double),
				                       _ALL_TYPE_QUALIFERS_(short),
				                       _ALL_TYPE_QUALIFERS_(char),
				                       _ALL_TYPE_QUALIFERS_(std::string),
				                       _ALL_TYPE_QUALIFERS_(unsigned int),
								       _ALL_TYPE_QUALIFERS_(unsigned char),
									  _ALL_TYPE_QUALIFERS_(unsigned short),
									  _ALL_TYPE_QUALIFERS_(long),
					                  _ALL_TYPE_QUALIFERS_(unsigned long),
									  _ALL_TYPE_QUALIFERS_(long long),
					                  _ALL_TYPE_QUALIFERS_(unsigned long long)

				>;
			};

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

		struct _DLL_EXPORT ReadWriteObject
		{
		protected:

			struct _DLL_EXPORT Concept
			{
				virtual void Set(std::function<void(std::any)> _functor) = 0;
				virtual void Get(void *, void (*) (std::any))    = 0;
				virtual Concept * Duplicate() const = 0;
				virtual ~Concept(){}
			};

			template<typename T>
			struct Wrapper : public Concept
			{
				template<typename U, typename SFINAE = Ut::EnableIf_t< !Ut::IsSame<Ut::Decay_t<U>, Wrapper>::value>>
				Wrapper(U && _obj)
					:mObj{ Ut::Forward<U>(_obj) }
				{

				}

				Wrapper(Wrapper const & _rhs)
					:mObj{_rhs.mObj}
				{}
				Wrapper(Wrapper && _rhs)
					:mObj{ Ut::Move(_rhs.mObj) }
				{
					
				}
				Wrapper& operator=(Wrapper const & _rhs)
				{
					mObj = _rhs;
					return *this;
				}
				Wrapper& operator=(Wrapper && _rhs)
				{
					mObj = Ut::Move(_rhs);
					return *this;
				}

				virtual Wrapper * Duplicate() const
				{
					return new Wrapper{ *this };
				}

				virtual void Set(std::function<void(std::any)> _functor) override
				{
					_functor(mObj);
				}
				virtual void Get(void *_Address, void(*_fp) (std::any)) override
				{
					_fp(mObj.mGet(_Address));
				}

				~Wrapper(){}

				T mObj;
			};



		public:

			ReadWriteObject()
				:mpWrapper(nullptr)
			{}
			template<typename T, typename SFINAE = Ut::EnableIf_t< !Ut::IsSame<Ut::Decay_t<T>, ReadWriteObject>::value>>
			ReadWriteObject(T && _Obj)
				:mpWrapper{ new Wrapper<T>{ Ut::Forward<T>(_Obj) } }
			{

			}

			ReadWriteObject(ReadWriteObject const & _TypeEraseRhs)
				:mpWrapper(_TypeEraseRhs.mpWrapper->Duplicate())
			{}
			

			ReadWriteObject(ReadWriteObject  && _TypeEraseRhs)
				:mpWrapper(_TypeEraseRhs.mpWrapper)
			{
				_TypeEraseRhs.mpWrapper = nullptr;
			}


			template <typename SF>
			void Get(void * _Address ,SF)
			{
				void(*passdown)(std::any) = ResolveType<SF>;
				mpWrapper->Get(_Address, passdown);
			}

			void Set(std::function<void(std::any)> _SuperFunctor);

			operator bool() const { return mpWrapper; }

			~ReadWriteObject()
			{
				delete mpWrapper;
				mpWrapper = nullptr;
			}


		private:
			Concept*  mpWrapper = nullptr;

			template<typename SuperFunctor>
			static void ResolveType(std::any _a)
			{
				using FunctorMap = std::map<decltype(typeid(int).hash_code()), void(*)(std::any, SuperFunctor) >;
				static FunctorMap MyMap = GenerateMap<SuperFunctor>(typename HelperMeta::AllPossibleType::type{});

				auto it = MyMap.find(_a.type().hash_code());
				if (it != MyMap.end())
				{
					(*it).second(_a, SuperFunctor{});
				}
				else
				{
					// Don't exist
					SuperFunctor{}(std::nullptr_t{nullptr});
				}
			}

			template<typename SuperFunctor, typename ... T>
			static auto GenerateMap(Variant<T...>)
			{
				using FunctorMap = std::map<decltype(typeid(int).hash_code()), void(*)(std::any, SuperFunctor)>;
				static FunctorMap myMap
				{
					std::make_pair
					(
						typeid(T).hash_code(),
						[](std::any _a, SuperFunctor _f) {_f(std::any_cast<T>(_a)); }
					)...
				};
				return myMap;
			}

		};

	}
}




#endif