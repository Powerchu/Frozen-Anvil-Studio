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

#include "Component/ColliderList.h"
#include "Component/Component.h"

#include "System/Base/ComponentDonor.h"
#include "System/Driver/Driver.h"

#include "DataStructure/Array.h"
#include "Object/GameObject.h"

#include "Utility/Utility.h"
#include "Utility/MetaAlgorithms.h"

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
			AuxEx<0>(arr, Ut::MetaExtract<Ns, List>::result::type::GetCompileName() ...);
		}
	};

	template <typename C, typename SFINAE = std::enable_if_t<!std::is_same_v<std::decay_t<C>, Component>, C>>
	struct AuxGenFunction
	{
		static Component* Extract(GameObject *_owner)
		{
			C* comp = static_cast<ComponentDonor<C>*>(EngineCore::GetInstance()->GetSystem<typename C::SYSTEM>())->RequestComponent();
			comp->SetOwner(_owner);
			comp->Init();
			return comp;
		}
	};

	struct ListOfComponents
	{
		static constexpr size_t size = Ut::SizeofList<UsableComponents>::value;

		template<typename A>
		struct GenerateCollection;

		template<size_t ... Ns>
		struct GenerateCollection<std::index_sequence<Ns ...>>
		{

			/*
			template <typename T>
			struct GetType
			{
				using type = T;
			};

			using tupleType = std::tuple
			<
				typename GetType<typename Ut::MetaExtract<Ns, UsableComponents>::result::type * (&)(void) >::type ...    //::result::type * (&)(void)
			>;

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
			template<typename List, size_t Head, size_t ... Rest>
			struct BreakTuple<std::index_sequence<Head, Rest ...>, List>
			{
				std::tuple< typename GetType<typename Ut::MetaExtract_t<Rest, List>::type* (&)(void)>::type ...> mData
					= { AuxGenFunction<typename Ut::MetaExtract_t<Rest, List>::type>::Extract ... };
			};
			template<typename List>
			struct BreakTuple<std::index_sequence<0>, List>
			{
				std::tuple<typename Ut::MetaExtract<size - 1, UsableComponents>::result::type* (&)(void)> mData
					= { AuxGenFunction<typename Ut::MetaExtract<size - 1, UsableComponents>::result::type>::Extract  };
			};


			template<typename A>
			struct Helper;
			template<size_t Head, size_t ... Rest>
			struct Helper<std::index_sequence<Head, Rest ...>>
			{
				template <typename List, typename... Ts>
				void* HelperFunction(unsigned int _i, std::tuple<Ts ...>& _data)
				{
					BreakTuple<Ut::MetaPopFront_t<List>> newData;
					if (!_i)
					{
						ApplyFunction a;
						return a(std::get<0>(_data));
					}
					return HelperFunction<Ut::MetaPopFront_t<List>>(_i - 1, newData.mData);
				}
			};
			*/

			Component* Get(unsigned int _i, GameObject *_owner)
			{
				static auto mData = Ctor::MakeArray<Component*(*)(GameObject *)>(AuxGenFunction<typename Ut::MetaExtract<Ns, UsableComponents>::result::type>::Extract
					... );
				if (_i < size || _i >= 0) 
					return mData[_i](_owner);
				return nullptr;
			}
		};

		Component* Get(unsigned int _i, GameObject *_owner)
		{
			return mCollection.Get(_i, _owner);
		}

		GenerateCollection<std::make_index_sequence<size>> mCollection;
	};


}

#endif //_AUX_META_HELPER_H_
#endif //EDITOR
