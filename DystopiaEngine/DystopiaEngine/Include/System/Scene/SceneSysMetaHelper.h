#pragma once
#ifndef SCENE_SYS_META_HELPER_H
#define SCENE_SYS_META_HELPER_H

#include "Utility/Meta.h"
#include <utility>


#include "System/Base/ComponentDonor.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Physics/PhysicsSystem.h"
#include "System/Camera/CameraSystem.h"

#include "Component/ComponentList.h"
#include "Component/Component.h"
#include "Component/Collider.h"
#include "Component/Renderer.h"
#include "Component/RigidBody.h"
#include "Component/Camera.h"

namespace Dystopia
{
	namespace SceneSystemHelper
	{
		template<typename T>
		struct SystemFunction;

		template<unsigned ... Seq>
		struct SystemFunction<std::index_sequence<Seq...>>
		{
			template<typename C>
			struct SystemFunctionHelper
			{
				static void Serialise(TextSerialiser & _Serialiser)
				{
					using System = typename C::SYSTEM;
					static_cast<ComponentDonor<C>*>(EngineCore::GetInstance()->GetSystem<System>())->Serialise(_Serialiser);
				}

				static void Unserialise(TextSerialiser & _Serialiser)
				{
					using System = typename C::SYSTEM;
					static_cast<ComponentDonor<C>*>(EngineCore::GetInstance()->GetSystem<System>())->Unserialise(_Serialiser);
				}
			};

			template<bool, typename T>
			struct RemoveRepeatAux
			{
				using type = typename Utility::TypeList<T>;
				static constexpr bool value = true;
			};
			template<typename T>
			struct RemoveRepeatAux<false, T>
			{
				using type = typename Utility::TypeList<>;
				static constexpr bool value = false;
			};

			template<bool ... Bools>
			struct isAllTrue
			{
				static constexpr bool value = (Bools & ...);
			};
			template <typename ... Ts>
			struct RemoveRepeat;

			template<typename Head, typename ... Rest>
			struct RemoveRepeat<Head, Rest...>
			{
				//using type = typename RemoveRepeat<Rest...>::type;
				using type = typename Utility::MetaConcat< typename RemoveRepeatAux< (isAllTrue<Utility::IsSame<Head, Rest>::value...>::value), Rest ...>::type, typename RemoveRepeat<Rest...>::type >::type;
				static constexpr unsigned Size = (Utility::IsSame<Head, Rest>::value ? 1 : 0) + RemoveRepeat<Rest...>::Size;
			};

			template<typename Head>
			struct RemoveRepeat<Head>
			{
				using type = typename Utility::TypeList<Head>;
				static constexpr unsigned Size = 1;
			};

			static void SystemSerialise(TextSerialiser & _Serialiser)
			{
				using pLoadFunc = void(*)(TextSerialiser &);
				//using NonRepeatUsable = typename RemoveRepeat<typename Utility::MetaExtract<Seq, UsableComponents>::result::type...>::type;

				static auto SysArray = Ctor::MakeArray<pLoadFunc>
					(
						static_cast<pLoadFunc>(&SystemFunctionHelper<typename Utility::MetaExtract<Seq, UsableComponents>::result::type>::Serialise) ...
						);

				for (unsigned i = 0; i < sizeof...(Seq); ++i)
				{
					SysArray[i](_Serialiser);
				}
			}

			static void SystemUnserialise(TextSerialiser & _Serialiser)
			{
				using pLoadFunc = void(*)(TextSerialiser &);
				static auto SysArray = Ctor::MakeArray<pLoadFunc>
					(
						static_cast<pLoadFunc>(&SystemFunctionHelper<typename Utility::MetaExtract<Seq, UsableComponents>::result::type>::Unserialise) ...
						);
				for (unsigned i = 0; i < sizeof...(Seq); ++i)
				{
					SysArray[i](_Serialiser);
				}
			}

		};
	}


}
#endif