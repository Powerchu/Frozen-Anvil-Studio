/* HEADER *********************************************************************************/
/*!
\file	SceneSystem.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System\Scene\SceneSystem.h" // File Header
#include "Utility/Meta.h"

#include "DataStructure/Array.h"
#include "IO/TextSerialiser.h"

#include "System\Collision\CollisionSystem.h"
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
				static void Serialise()
				{
					using System = typename C::SYSTEM;
					(EngineCore::GetInstance()->GetSystem<System>())->Serialise();
				}

				static void Unserialise()
				{
					using System = typename C::SYSTEM;
					(EngineCore::GetInstance()->GetSystem<System>())->Unserialise();
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
				using type = typename Utility::MetaConcat< typename RemoveRepeatAux< (isAllTrue<Utility::IsSame<Head, Rest>::value...>::value) , Rest ...>::type, typename RemoveRepeat<Rest...>::type >::type;
				static constexpr unsigned Size = (Utility::IsSame<Head, Rest>::value ? 1 : 0) + RemoveRepeat<Rest...>::Size;
			};

			template<typename Head>
			struct RemoveRepeat<Head>
			{
				using type = typename Utility::TypeList<Head>;
				static constexpr unsigned Size = 1;
			};

			static void SystemSerialise()
			{
				using pLoadFunc = void(*)();
				using NonRepeatUsable = typename RemoveRepeat<typename Utility::MetaExtract<Seq, UsableComponents>::result::type...>::type;

				static auto SysArray = Ctor::MakeArray<pLoadFunc>
				(
					static_cast<pLoadFunc>(&SystemFunctionHelper<typename Utility::MetaExtract<NonRepeatUsable::Size, NonRepeatUsable>::result::type>::Serialise) ...
				);

				for (unsigned i = 0; i < sizeof...(Seq); ++i)
				{
					SysArray[i]();
				}
			}

			static void SystemUnserialise()
			{
				using pLoadFunc = void(*)();
				static auto SysArray = Ctor::MakeArray<pLoadFunc>
					(
						static_cast<pLoadFunc>(&SystemFunctionHelper<typename Utility::MetaExtract<Seq, UsableComponents>::result::type>::Unserialise) ...
					);
				for (unsigned i = 0; i < sizeof...(Seq); ++i)
				{
					SysArray[i]();
				}
			}

		};
	}


}


Dystopia::SceneSystem::SceneSystem(void) :
	mpCurrScene{ nullptr }, mpNextScene{ nullptr }
{
}

Dystopia::SceneSystem::~SceneSystem(void)
{
}

void Dystopia::SceneSystem::PreInit(void)
{
	mpCurrScene = new Scene{};
}

bool Dystopia::SceneSystem::Init(void)
{
	return true;
}

void Dystopia::SceneSystem::PostInit(void)
{
	mpNextScene = mpCurrScene;
}

void Dystopia::SceneSystem::FixedUpdate(float _dt)
{
	if (mpNextScene == mpCurrScene)
	{
		mpCurrScene->FixedUpdate(_dt);
	}
}

void Dystopia::SceneSystem::Update(float _dt)
{
	if (mpNextScene == mpCurrScene)
	{
		mpCurrScene->Update(_dt);
	}
	else
	{
		
	}
}

void Dystopia::SceneSystem::PostUpdate(void)
{
	if (mpNextScene == mpCurrScene)
	{
		mpCurrScene->PostUpdate();
	}
}

void Dystopia::SceneSystem::Shutdown(void)
{
	if (mpNextScene != mpCurrScene)
		delete mpNextScene;
	delete mpCurrScene;

	mpNextScene = mpCurrScene = nullptr;
}

void Dystopia::SceneSystem::LoadDefaults(void)
{

}

void Dystopia::SceneSystem::LoadSettings(TextSerialiser&)
{

}

void Dystopia::SceneSystem::LoadScene(const std::string& _strFile)
{
	UNUSED_PARAMETER(_strFile);
	static constexpr size_t size = Utility::SizeofList<UsableComponents>::value;

	if(!mpNextScene)
		return;

	/*Open File*/
	auto & SerialObj = TextSerialiser::OpenFile(_strFile, TextSerialiser::MODE_READ);
	/*Consume Start Block*/
	SerialObj.ConsumeStartBlock();
	/*Get Next Scene to Unserialise*/
	mpNextScene->Unserialise(SerialObj);
	/*Get all System who are ComponentDonor to unserialise*/
	SceneSystemHelper::SystemFunction< std::make_index_sequence< size >>::SystemUnserialise();
	/*Consume End Block*/
	SerialObj.ConsumeEndBlock();
}


