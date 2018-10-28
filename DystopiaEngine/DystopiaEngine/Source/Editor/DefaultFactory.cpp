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
#include "Editor/EditorMetaHelpers.h"

#include "Utility/GUID.h"
#include "Utility/DebugAssert.h"
#include "Object/GameObject.h"
#include "IO/TextSerialiser.h"

#include "Component/AudioSource.h"
#include "Component/Camera.h"
#include "Component/Collider.h"
#include "Component/Circle.h"
#include "Component/AABB.h"
#include "Component/Convex.h"
#include "Component/Renderer.h"
#include "Component/RigidBody.h"
#include "Component/SpriteRenderer.h"
#include "Component/TextRenderer.h"
#include "Component/CharacterController.h"

#include "System/Sound/SoundSystem.h"
#include "System/Driver/Driver.h"
#include "System/Input/InputSystem.h"
#include "System/Camera/CameraSystem.h"
#include "System/Physics/PhysicsSystem.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Behaviour/BehaviourSystem.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Behaviour/BehaviourSystem.h"

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
			pObject->GetComponent<Transform>()->SetScale(Math::Vec3D{ 16, 16, 1 });
			auto rend = static_cast<ComponentDonor<Renderer>*>(EngineCore::GetInstance()->GetSystem<GraphicsSystem>())->RequestComponent();
			pObject->AddComponent(rend, typename Renderer::TAG{});
			auto rigid = EngineCore::GetInstance()->GetSystem<PhysicsSystem>()->RequestComponent();
			pObject->AddComponent(rigid, typename RigidBody::TAG{});
			auto col = static_cast<ComponentDonor<Convex>*> (EngineCore::GetInstance()->GetSystem<CollisionSystem>())->RequestComponent();
			pObject->AddComponent(col, typename Collider::TAG{});

			pObject->Identify();
			pObject->Init();
			rend->Init();
			rigid->Init();
			col->Init();

			rend->SetTexture(EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->LoadTexture("Resource/Editor/white_box.png"));
			return pObject;
		}

		GameObject* CreatePerformanceObjCol(const std::string& _name)
		{
			GameObject *pObject = new GameObject{ GUIDGenerator::GetUniqueID() };
			pObject->SetName(_name);
			pObject->SetActive(true);
			pObject->Init();
			pObject->GetComponent<Transform>()->SetScale(Math::Vec4{ 16, 16, 1 });
			const auto rend = static_cast<ComponentDonor<Renderer>*>(EngineCore::GetInstance()->GetSystem<GraphicsSystem>())->RequestComponent();
			const auto rigid = EngineCore::GetInstance()->GetSystem<PhysicsSystem>()->RequestComponent();
			const auto col = static_cast<ComponentDonor<Convex>*> (EngineCore::GetInstance()->GetSystem<CollisionSystem>())->RequestComponent();
			pObject->AddComponent(rend, Renderer::TAG{});
			pObject->AddComponent(rigid, RigidBody::TAG{});
			pObject->AddComponent(col, Collider::TAG{});
			rend->SetTexture(EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->LoadTexture("Resource/Editor/red_box.png"));
			rend->SetOwner(pObject);
			rend->Init();
			rigid->SetOwner(pObject);
			rigid->Init();
			col->SetOwner(pObject);
			col->Init();
			pObject->Init();
			return pObject;
		}

		GameObject* CreateCamera(const std::string& _name)
		{
			auto pCore = EngineCore::GetInstance();
			GameObject *pObject = CreateGameObj(_name);
			auto p = pCore->GetSystem<CameraSystem>()->RequestComponent();
			p->SetSurface(& (pCore->GetSystem<GraphicsSystem>()->GetView(3)));
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

			auto g = static_cast<ComponentDonor<Renderer>*>(EngineCore::GetInstance()->GetSystem<GraphicsSystem>())->RequestComponent();
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

			auto g = static_cast<ComponentDonor<Renderer>*>(EngineCore::GetInstance()->GetSystem<GraphicsSystem>())->RequestComponent();
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

			auto g = static_cast<ComponentDonor<Renderer>*>(EngineCore::GetInstance()->GetSystem<GraphicsSystem>())->RequestComponent();
			g->SetOwner(pObject);
			pObject->AddComponent(g, typename Renderer::TAG{});

			auto c = static_cast<ComponentDonor<Circle>*> (EngineCore::GetInstance()->GetSystem<CollisionSystem>())->RequestComponent();
			pObject->AddComponent(c, typename Collider::TAG{});
			return pObject;
		}

		void SaveAsPrefab(GameObject& _obj, TextSerialiser& _out)
		{
			auto& allCom = _obj.GetAllComponents();
			auto& allBehaviour = _obj.GetAllBehaviours();
			_out.InsertStartBlock("GameObject");
			_out << allCom.size();
			_obj.Serialise(_out);
			_out.InsertEndBlock("GameObject");

			for (const auto& elem : allCom)
			{
				_out.InsertStartBlock("Component");
				_out << elem->GetRealComponentType();
				elem->Serialise(_out);
				_out.InsertEndBlock("Component End");
			}
			//for (const auto & elem : allBehaviour)
			//{
			//	std::string name{ elem.BehaviourName };
			//	_out << name;
			//	auto Metadata = elem.GetMetaData();
			//	auto AllNames = Metadata.GetAllNames();
			//	for (auto const & elem : AllNames)
			//	{
			//		if(Metadata[elem])
			//			Metadata[elem].Serialise()
			//	}
			//}
		}
	
		std::string SaveAsPrefab(GameObject& _obj, const std::string& _path)
		{
			std::string fileName = _path + "\\" + _obj.GetName() + g_PayloadPrefabEx;
			std::ofstream file{ fileName, std::ios::out };
			if (!file.is_open())
				__debugbreak();

			auto toFile	= TextSerialiser::OpenFile(fileName, TextSerialiser::MODE_WRITE);
			SaveAsPrefab(_obj, toFile);
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
