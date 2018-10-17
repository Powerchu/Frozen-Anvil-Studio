/* HEADER *********************************************************************************/
/*!
\file	DefaultFactory.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/DefaultFactory.h"
#include "Editor/Payloads.h"
#include "Editor/EditorMetaHelpers.h"

#include "Utility/GUID.h"
#include "Utility/DebugAssert.h"
#include "Object/GameObject.h"
#include "IO/TextSerialiser.h"

#include "Component/Camera.h"
#include "Component/Collider.h"
#include "Component/Circle.h"
#include "Component/AABB.h"
#include "Component/Convex.h"
#include "Component/Renderer.h"
#include "Component/RigidBody.h"
#include "Component/CharacterController.h"
#include "Component/AudioSource.h"

#include "System/Driver/Driver.h"
#include "System/Input/InputSystem.h"
#include "System/Camera/CameraSystem.h"
#include "System/Physics/PhysicsSystem.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Behaviour/BehaviourSystem.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Sound/SoundSystem.h"

#include <fstream>
#include <iostream>

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
	
		std::string SaveAsPrefab(GameObject& _obj, const std::string& _path)
		{
			std::string fileName = _path + "\\" + _obj.GetName() + g_PayloadPrefabEx;
			std::ofstream file{ fileName, std::ios::out };
			if (!file.is_open())
				__debugbreak();

			auto toFile					= TextSerialiser::OpenFile(fileName, TextSerialiser::MODE_WRITE);
			const auto& allComponents	= _obj.GetAllComponents();

			toFile.InsertStartBlock("GameObject");
			toFile << allComponents.size();
			_obj.Serialise(toFile);
			toFile.InsertEndBlock("GameObject");
			for (const auto& elem : allComponents)
			{
				toFile.InsertStartBlock("Component");
				toFile << elem->GetRealComponentType();
				elem->Serialise(toFile);
				toFile.InsertEndBlock("Component End");
			}
			return _obj.GetName() + g_PayloadPrefabEx;
		}

		GameObject* LoadFromPrefab(std::string _gameObjName, const std::string& _path)
		{
			std::string fileName = _gameObjName.length() ? _path + "\\" + _gameObjName : _path;

			if (fileName.find(Dystopia::g_PayloadPrefabEx) == std::string::npos)
				return nullptr;

			ListOfComponents availComponents;
			unsigned	num			= 0;
			unsigned	sysID		= 0;
			auto		fromFile	= TextSerialiser::OpenFile(fileName, TextSerialiser::MODE_READ);
			GameObject* pObj		= new GameObject{};

			fromFile.ConsumeStartBlock();
			fromFile >> num;
			pObj->Unserialise(fromFile);
			fromFile.ConsumeEndBlock();
			for (unsigned i = 0; i < num; i++)
			{
				fromFile.ConsumeStartBlock();
				fromFile >> sysID;
				Component* pComp = availComponents.Get(sysID, pObj);
				pObj->AddComponent(pComp, typename Component::TAG{});
				pComp->Unserialise(fromFile);
				fromFile.ConsumeEndBlock();
			}

			pObj->SetID(GUIDGenerator::GetUniqueID());
			pObj->Identify();
			return pObj;
		}
	}
}
#endif //EDITOR
