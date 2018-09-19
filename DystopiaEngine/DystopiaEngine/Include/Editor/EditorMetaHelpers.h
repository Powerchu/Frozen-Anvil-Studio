/* HEADER *********************************************************************************/
/*!
\file	Inspector.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_META_HELPER_H_
#define _EDITOR_META_HELPER_H_

#include "Object\GameObject.h"
#include <tuple>
#include <utility>

namespace Dystopia
{
	template<typename A, typename B>
	struct ListOfComponentsName;

	template <class List, size_t ... Ns>
	struct ListOfComponentsName<std::index_sequence<Ns ...>, List>
	{
		template<size_t N, typename T, typename ... Ts>
		static inline void AuxEx(Array<std::string, sizeof...(Ns)>& arr, T&& t, Ts&& ... ts)
		{
			arr[N] = t;
			if constexpr (N != sizeof...(Ns) - 1)
				AuxEx<N + 1>(arr, ts...);
		}

		template<size_t Num>
		static void Extract(Array<std::string, Num>& arr)
		{
			AuxEx<0>(arr, Utility::MetaExtract<Ns, List>::result::type::GetCompileName() ...);
		}
	};

	template <typename C, typename SFINAE = std::enable_if_t<!std::is_same_v<std::decay_t<C>, Component>, C>>
	struct AuxGenFunction
	{
		static C* Extract(void) 
		{
			return EngineCore::GetInstance()->GetSystem<typename C::SYSTEM>()->RequestComponent();
		}
	};

	struct ListOfComponents
	{
		static constexpr size_t size = Utility::SizeofList<UsableComponents>::value;

		template<typename A>
		struct GenerateCollection;

		template<size_t ... Ns>
		struct GenerateCollection<std::index_sequence<Ns ...>>
		{
			template <typename T>
			struct GetType
			{
				using type = T;
			};

			using tupleType = std::tuple<typename Utility::MetaExtract<Ns, UsableComponents>::result::type* (&)(void) ...>;
			tupleType mData = { AuxGenFunction<typename Utility::MetaExtract<Ns, UsableComponents>::result::type>::Extract ... };

			struct ApplyFunction
			{
				template<typename T>
				T* operator()(T*(&_fn)(void))
				{
					return _fn();
				}
			};

			template<typename A, typename List>
			struct BreakTuple;
			template<size_t Head, size_t ... Rest, typename List>
			struct BreakTuple<std::index_sequence<Head, Rest ...>, List>
			{
				std::tuple<typename GetType<typename Utility::MetaExtract<Rest, List>::result::type* (&)(void)>::type ...> mData
					= { AuxGenFunction<typename GetType<typename Utility::MetaExtract<Rest, List>::result::type>::type>::Extract ... };
			};
			template<template<typename ... T> typename List>
			struct BreakTuple<std::index_sequence<0>, List<>>
			{
				std::tuple<typename Utility::MetaExtract<size - 1, UsableComponents>::result::type* (&)(void)> mData
					= { AuxGenFunction<typename Utility::MetaExtract<size - 1, UsableComponents>::result::type>::Extract };
			};

			template<typename A>
			struct Helper;
			template<size_t Head, size_t ... Rest>
			struct Helper<std::index_sequence<Head, Rest ...>>
			{
				template <typename List, typename... Ts>
				void* HelperFunction(unsigned int _i, std::tuple<Ts ...>& _data)
				{
					if (!_i)
					{
						ApplyFunction a;
						return a(std::get<0>(_data));
					}
					BreakTuple<std::make_index_sequence<sizeof...(Ts)>, typename Utility::MetaPopFront<UsableComponents>::type> newData;
					//BreakTuple<std::make_index_sequence<sizeof...(Ts)>, typename Utility::MetaPopFront<List>::type> newData;
					//return HelperFunction<typename Utility::MetaPopFront<List>::type>(_i - 1, newData.mData);
				}
			};

			Component* Get(unsigned int _i)
			{
				Helper<std::make_index_sequence<size>> h;
				if (_i >= size || _i < 0)
					return nullptr;
				return static_cast<Component*>(h.HelperFunction<UsableComponents>(_i, mData));
			}
		};

		Component* Get(unsigned int _i)
		{
			return mCollection.Get(_i);
		}

		GenerateCollection<std::make_index_sequence<size>> mCollection;
	};
}

#endif //_AUX_META_HELPER_H_
#endif //EDITOR
