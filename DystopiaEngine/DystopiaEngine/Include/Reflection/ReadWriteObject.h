#pragma once
#ifndef _READ_WRITE_OBJECT_H
#define _READ_WRITE_OBJECT_H

#include <functional>
#include <any>

#include "Globals.h"
#include "DataStructure/SharedPtr.h"
#include "Math/MathLib.h"
#define _ALL_TYPE_QUALIFERS_(_TYPE_) _TYPE_, const _TYPE_, _TYPE_ *, const _TYPE_ *, _TYPE_ * const, _TYPE_ const * const
#define _VOID_RETURN_FUNC_TYPE_(...) std::function<void(__VA_ARGS__)>, void (*)(__VA_ARGS__)
namespace Dystopia
{
	class TextSerialiser;
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
					                   _ALL_TYPE_QUALIFERS_(unsigned long long),
					                   _ALL_TYPE_QUALIFERS_(Math::Vec3D),
					                   _VOID_RETURN_FUNC_TYPE_(int, void*),
					                   _VOID_RETURN_FUNC_TYPE_(float, void*),
					                   _VOID_RETURN_FUNC_TYPE_(double, void*),
					                   _VOID_RETURN_FUNC_TYPE_(short, void*),
					                   _VOID_RETURN_FUNC_TYPE_(char, void*),
					                   _VOID_RETURN_FUNC_TYPE_(std::string, void*),
					                   _VOID_RETURN_FUNC_TYPE_(unsigned int, void*),
					                   _VOID_RETURN_FUNC_TYPE_(unsigned char, void*),
					                   _VOID_RETURN_FUNC_TYPE_(unsigned short, void*),
					                   _VOID_RETURN_FUNC_TYPE_(long, void*),
					                   _VOID_RETURN_FUNC_TYPE_(unsigned long, void*),
					                   _VOID_RETURN_FUNC_TYPE_(long long, void*),
					                   _VOID_RETURN_FUNC_TYPE_(unsigned long long, void*),
					                   _VOID_RETURN_FUNC_TYPE_(Math::Vec3D, void*)
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
				virtual void Set(void *, void (*) (std::any, void*))                                                    = 0;
				virtual void Set(void *, void(*) (std::any, void*, std::any), std::any)                                 = 0;
				virtual void Get(void *, void (*) (std::any))                                                           = 0;
				virtual void Reflect(const char *, void *, void(*) (const char *,std::any, std::any, void*))            = 0;
				virtual void Serialise(void*, TextSerialiser&, void(*)(std::any,void*, TextSerialiser&)) const          = 0;
				virtual void Unserialise(void*, TextSerialiser &,void(*) (std::any, std::any, void*, TextSerialiser&))  = 0;
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

				virtual void Set(void *_Address, void(*_fp) (std::any, void*)) override
				{
					_fp(mObj.mSet.GetFunc(), _Address);

				}
				virtual void Set(void *_Address, void(*_fp) (std::any, void*, std::any), std::any _v) override
				{
					_fp(mObj.mSet.GetFunc(), _Address, _v);
				}
				virtual void Get(void *_Address, void(*_fp) (std::any)) override
				{
					_fp(mObj.mGet(_Address));
				}
				virtual void Reflect(const char * _name,void *_Address, void(*_fp) (const char *, std::any, std::any, void*)) override
				{
					_fp(_name,mObj.mGet(_Address), mObj.mSet.GetFunc(), _Address);
				}

				virtual void Serialise(void* _addr, TextSerialiser& _t, void(*_fp)(std::any, void*, TextSerialiser&)) const override
				{
					_fp(mObj.mGet(_addr), _addr, _t);
				}

				virtual void Unserialise(void* _addr, TextSerialiser & _serialiser,void(*_fp) (std::any, std::any, void*, TextSerialiser&)) override
				{
					_fp(mObj.mGet(_addr), mObj.mSet.GetFunc(), _addr, _serialiser);
				}
				~Wrapper(){}

				T mObj;
			};

			//template <typename T>
			//struct TypeResolve
			//{
			//	void ResolveType(std::any&)
			//	{
			//		// Do type resolve
			//	}

			//	void(*GetTypeResolver(void))(std::any)
			//	{
			//		return [=](std::any r) { this->ResolveType(r); };
			//	}
			//};

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
				:mpWrapper(nullptr)
			{
				if (_TypeEraseRhs.mpWrapper)
					mpWrapper = _TypeEraseRhs.mpWrapper->Duplicate();
			}
			

			ReadWriteObject(ReadWriteObject  && _TypeEraseRhs)
				:mpWrapper(_TypeEraseRhs.mpWrapper)
			{
				_TypeEraseRhs.mpWrapper = nullptr;
			}
			ReadWriteObject& operator=(ReadWriteObject const & _rhs) = delete;

			template <typename SF>
			void Get(void * _Address, SF)
			{
				//TypeResolve<SF> x{ _sf };
				void(*passdown)(std::any) = ResolveType<SF>;
				mpWrapper->Get(_Address, passdown);
			}

			template<typename SF>
			void Set(void * _Address, SF)
			{
				void(*passdown)(std::any, void*) = ResolveType_Set<SF>;
				mpWrapper->Set(_Address, passdown);
			}

			template<typename SF, typename T>
			void Set(void * _Address, SF, T _value)
			{
				void(*passdown)(std::any, void*, std::any) = ResolveType_Set<SF,T>;
				mpWrapper->Set(_Address, passdown, _value);
			}

			template <typename SF>
			void Reflect(const char * _name,void * _Address, SF)
			{
				void(*passdown)(const char *,std::any, std::any, void*) = ResolveType_Reflect<SF>;
				if(mpWrapper)
					mpWrapper->Reflect(_name,_Address, passdown);
			}

			template <typename SF>
			void Serialise(void * _Address, TextSerialiser & _serialiser, SF) const
			{
				void(*passdown)(std::any, void*, TextSerialiser &) = ResolveType_Serialise<SF>;
				mpWrapper->Serialise(_Address, _serialiser, passdown);
			}

			template<typename SF>
			void Unserialise(void* _Address, TextSerialiser & _serialiser, SF)
			{
				void(*passdown)(std::any ,std::any, void*, TextSerialiser&) = ResolveType_Unserialise<SF>;
				mpWrapper->Unserialise(_Address, _serialiser, passdown);
			}

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

			template<typename SuperFunctor>
			static void ResolveType_Set(std::any _a, void*_addr)
			{
				using FunctorMap = std::map<decltype(typeid(int).hash_code()), void(*)(std::any, SuperFunctor, void*) >;
				static FunctorMap MyMap = GenerateMap_Set<SuperFunctor>(typename HelperMeta::AllPossibleType::type{});

				auto it = MyMap.find(_a.type().hash_code());
				if (it != MyMap.end())
				{
					(*it).second(_a, SuperFunctor{}, _addr);
				}
				else
				{
					// Don't exist
					SuperFunctor{}(std::nullptr_t{ nullptr }, _addr);
				}
			}

			template<typename SuperFunctor, typename ... T>
			static auto GenerateMap_Set(Variant<T...>)
			{
				using FunctorMap = std::map<decltype(typeid(int).hash_code()), void(*)(std::any, SuperFunctor, void*)>;
				static FunctorMap myMap
				{
					std::make_pair
					(
						typeid(T).hash_code(),
						[](std::any _a, SuperFunctor _f, void* _addr) {_f(std::any_cast<T>(_a), _addr); }
					)...
				};
				return myMap;
			}

			template<typename SuperFunctor, typename T>
			static void ResolveType_Set(std::any _a, void*_addr, std::any _value)
			{
				using FunctorMap = std::map<decltype(typeid(int).hash_code()), void(*)(std::any, SuperFunctor, void*, std::any) >;
				static FunctorMap MyMap = GenerateMap_Set<SuperFunctor, T>(typename HelperMeta::AllPossibleType::type{});

				auto it = MyMap.find(_a.type().hash_code());
				if (it != MyMap.end())
				{
					(*it).second(_a, SuperFunctor{}, _addr);
				}
				else
				{
					// Don't exist
					SuperFunctor{}(std::nullptr_t{ nullptr }, _addr);
				}
			}

			template<typename SuperFunctor, typename ValueType, typename ... T>
			static auto GenerateMap_Set(Variant<T...>)
			{
				using FunctorMap = std::map<decltype(typeid(int).hash_code()), void(*)(std::any, SuperFunctor, void*, std::any)>;
				static FunctorMap myMap
				{
					std::make_pair
					(
						typeid(T).hash_code(),
						[](std::any _a, SuperFunctor _f, void* _addr, std::any _v) {_f(std::any_cast<T>(_a), _addr, std::any_cast<ValueType>(_v)); }
					)...
				};
				return myMap;
			}

			template<typename SuperFunctor>
			static void ResolveType_Reflect(const char * _name, std::any _a, std::any _settor, void * _addr)
			{
				using FunctorMap = std::map<decltype(typeid(int).hash_code()), void(*)(const char *, std::any, SuperFunctor, std::any, void*)>;
				static FunctorMap MyMap = GenerateMap_Reflect<SuperFunctor>(typename HelperMeta::AllPossibleType::type{});

				auto it = MyMap.find(_a.type().hash_code());
				if (it != MyMap.end())
				{
					(*it).second(_name, _a, SuperFunctor{}, _settor, _addr);
				}
				else
				{
					// Don't exist
					SuperFunctor{}(_name, std::nullptr_t{ nullptr }, _settor, _addr);
				}
			}

			template<typename SuperFunctor, typename ... T>
			static auto GenerateMap_Reflect(Variant<T...>)
			{
				using FunctorMap = std::map<decltype(typeid(int).hash_code()), void(*)(const char *, std::any, SuperFunctor, std::any, void*)>;
				static FunctorMap myMap
				{
					std::make_pair
					(
						typeid(T).hash_code(),
						[](const char * _name,std::any _a, SuperFunctor _f, std::any _set, void * _addr) {_f(_name,std::any_cast<T>(_a), std::any_cast<std::function<void(T,void*)>>(_set),_addr); }
					)...
				};
				return myMap;
			}

			template<typename SuperFunctor>
			static void ResolveType_Serialise(std::any _a, void * _addr, TextSerialiser & _serialiser)
			{
				using FunctorMap = std::map<decltype(typeid(int).hash_code()), void(*)(std::any, SuperFunctor, TextSerialiser &, void *)>;
				static FunctorMap MyMap = GenerateMap_Serialise<SuperFunctor>(typename HelperMeta::AllPossibleType::type{});

				auto it = MyMap.find(_a.type().hash_code());
				if (it != MyMap.end())
				{
					(*it).second(_a, SuperFunctor{}, _serialiser, _addr);
				}
				else
				{
					// Don't exist
					SuperFunctor{}(std::nullptr_t{ nullptr }, _serialiser, _addr);
				}
			}

			template<typename SuperFunctor, typename ... T>
			static auto GenerateMap_Serialise(Variant<T...>)
			{
				using FunctorMap = std::map<decltype(typeid(int).hash_code()), void(*)(std::any, SuperFunctor, TextSerialiser &, void*)>;
				static FunctorMap myMap
				{
					std::make_pair
					(
						typeid(T).hash_code(),
						[](std::any _a, SuperFunctor _f, TextSerialiser & _obj, void * _addr) {_f(std::any_cast<T>(_a),_obj,_addr); }
					)...
				};
				return myMap;
			}

			template<typename SuperFunctor>
			static void ResolveType_Unserialise(std::any _a, std::any _settor,void * _addr, TextSerialiser & _serialiser)
			{
				using FunctorMap = std::map<decltype(typeid(int).hash_code()), void(*)(std::any, SuperFunctor, std::any, void*, TextSerialiser &)>;
				static FunctorMap MyMap = GenerateMap_Unserialise<SuperFunctor>(typename HelperMeta::AllPossibleType::type{});

				auto it = MyMap.find(_a.type().hash_code());
				if (it != MyMap.end())
				{
					(*it).second(_a, SuperFunctor{}, _settor, _addr, _serialiser);
				}
				else
				{
					// Don't exist
					SuperFunctor{}(std::nullptr_t{ nullptr });
				}
			}

			template<typename SuperFunctor, typename ... T>
			static auto GenerateMap_Unserialise(Variant<T...>)
			{
				using FunctorMap = std::map<decltype(typeid(int).hash_code()), void(*)(std::any, SuperFunctor, std::any, void*, TextSerialiser &)>;
				static FunctorMap myMap
				{
					std::make_pair
					(
						typeid(T).hash_code(),
						[](std::any _a, SuperFunctor _f, std::any _set, void * _addr, TextSerialiser & _serialiser) {_f(std::any_cast<T>(_a), std::any_cast<std::function<void(T,void*)>>(_set),_addr, _serialiser); }
					)...
				};
				return myMap;
			}
		};

	}
}




#endif