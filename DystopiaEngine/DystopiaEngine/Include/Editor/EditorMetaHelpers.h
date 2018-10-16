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
			AuxEx<0>(arr, Utility::MetaExtract<Ns, List>::result::type::GetCompileName() ...);
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
		static constexpr size_t size = Utility::SizeofList<UsableComponents>::value;

		template<typename A>
		struct GenerateCollection;

		template<size_t ... Ns>
		struct GenerateCollection<std::index_sequence<Ns ...>>
		{
			Component* Get(unsigned int _i, GameObject *_owner)
			{
				static auto mData = Ctor::MakeArray<Component*(*)(GameObject *)>(AuxGenFunction<typename Utility::MetaExtract<Ns, UsableComponents>::result::type>::Extract
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
