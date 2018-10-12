/* HEADER *********************************************************************************/
/*!
\file	DefaultFactory.cpp
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
#include "Editor/DefaultFactory.h"
#include "Editor/Payloads.h"

#include "Component/Camera.h"
#include "Component/Renderer.h"
#include "Component/RigidBody.h"
#include "Component/ColliderList.h"

#include "System/Driver/Driver.h"
#include "System/Scene/SceneSystem.h"
#include "System/Camera/CameraSystem.h"
#include "System/Graphics/GraphicsSystem.h"

#include "Utility/GUID.h"
#include "Utility/DebugAssert.h"
#include "Object/GameObject.h"
#include "IO/TextSerialiser.h"

#include <fstream>
#include <iostream>

// TODO: DELETE
#include "System/Physics/PhysicsSystem.h"
#include "System/Collision/CollisionSystem.h"

static const std::string DEFAULT_PREFAB_FOLDER = "..\\DystopiaEngine\\Resource\\Prefab";
namespace Dystopia
{
	namespace Factory
	{
		GameObject* CreateGameObj(const std::string& _name)
		{
			GameObject *pObject = new GameObject{ GUIDGenerator::GetUniqueID() };
			pObject->SetName(_name);
			pObject->SetActive(true);
			return pObject;
		}

		GameObject* CreatePerformanceObj(const std::string& _name)
		{
			GameObject *pObject = new GameObject{ GUIDGenerator::GetUniqueID() };
			pObject->SetName(_name);
			pObject->SetActive(true);
			pObject->GetComponent<Transform>()->SetScale(Math::Vec4{ 100, 100, 1 });
			auto rend = EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->RequestComponent();
			pObject->AddComponent(rend, typename Renderer::TAG{});
			auto rigid = EngineCore::GetInstance()->GetSystem<PhysicsSystem>()->RequestComponent();
			pObject->AddComponent(rigid, typename RigidBody::TAG{});

			pObject->Identify();
			pObject->Init();
			rend->Init();
			rigid->Init();

			rend->SetTexture(EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->LoadTexture("Resource/Asset/Salamander_Stand.png"));
			return pObject;
		}

		GameObject* CreateCamera(const std::string& _name)
		{
			GameObject *pObject = CreateGameObj(_name);
			auto p = EngineCore::GetInstance()->GetSystem<CameraSystem>()->RequestComponent();
			pObject->AddComponent(p, typename Camera::TAG{});
			pObject->Identify();
			return pObject;
		}

		GameObject* CreateStaticBox(const std::string& _name)
		{
			GameObject *pObject = CreateGameObj(_name);
			auto p = EngineCore::GetInstance()->GetSystem<PhysicsSystem>()->RequestComponent();
			p->SetOwner(pObject);
			p->Init();
			p->Set_IsStatic(true);
			pObject->AddComponent(p, typename RigidBody::TAG{});

			auto g = EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->RequestComponent();
			g->SetOwner(pObject);
			pObject->AddComponent(g, typename Renderer::TAG{});

			auto c = static_cast<ComponentDonor<Convex>*> (EngineCore::GetInstance()->GetSystem<CollisionSystem>())->RequestComponent();
			pObject->AddComponent(c, typename Collider::TAG{});
			pObject->GetComponent<Transform>()->SetGlobalPosition(0, -185, 0);

			return pObject;
		}

		GameObject* CreateBox(const std::string& _name)
		{
			GameObject *pObject = CreateGameObj(_name);
			auto p = EngineCore::GetInstance()->GetSystem<PhysicsSystem>()->RequestComponent();
			p->SetOwner(pObject);
			p->Init();
			pObject->AddComponent(p, typename RigidBody::TAG{});

			auto g = EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->RequestComponent();
			g->SetOwner(pObject);
			pObject->AddComponent(g, typename Renderer::TAG{});

			auto c = static_cast<ComponentDonor<Convex>*> (EngineCore::GetInstance()->GetSystem<CollisionSystem>())->RequestComponent();
			pObject->AddComponent(c, typename Collider::TAG{});
			return pObject;
		}

		GameObject * CreateCircle(const std::string & _name)
		{
			GameObject *pObject = CreateGameObj(_name);
			auto p = EngineCore::GetInstance()->GetSystem<PhysicsSystem>()->RequestComponent();
			p->SetOwner(pObject);
			p->Init();
			pObject->AddComponent(p, typename RigidBody::TAG{});

			auto g = EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->RequestComponent();
			g->SetOwner(pObject);
			pObject->AddComponent(g, typename Renderer::TAG{});

			auto c = static_cast<ComponentDonor<Circle>*> (EngineCore::GetInstance()->GetSystem<CollisionSystem>())->RequestComponent();
			pObject->AddComponent(c, typename Collider::TAG{});
			return pObject;
		}
	
		void SaveAsPrefab(GameObject& _obj)
		{
			std::string fileName = DEFAULT_PREFAB_FOLDER + "\\" + _obj.GetName() + g_PayloadPrefabEx;
			std::ofstream file{ fileName, std::ios::out };
			if (!file.is_open())
				__debugbreak();

			auto toFile					= TextSerialiser::OpenFile(fileName, TextSerialiser::MODE_WRITE);
			const auto& allComponents	= _obj.GetAllComponents();

			_obj.Serialise(toFile);
			toFile << allComponents.size();
			for (const auto& elem : allComponents)
			{
				toFile.InsertStartBlock("Component");
				elem->Serialise(toFile);
				toFile.InsertEndBlock("Component End");
			}
		}

		GameObject* LoadFromPrefab(std::string _gameObjName)
		{
			unsigned	num			= 0;
			std::string fileName	= DEFAULT_PREFAB_FOLDER + "\\" + _gameObjName + g_PayloadPrefabEx;
			auto		fromFile	= TextSerialiser::OpenFile(fileName, TextSerialiser::MODE_READ);
			GameObject* pObj		= new GameObject{};

			pObj->Unserialise(fromFile);
			fromFile >> numComponents;
			for (unsigned i = 0; i < num; i++)
			{

			}

			return pObj;
		}
	}
}
#endif //EDITOR
