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

#include "Component/Camera.h"
#include "Component/ColliderList.h"
#include "Component/Renderer.h"
#include "Component/RigidBody.h"
#include "Component/SpriteRenderer.h"
#include "Component/TextRenderer.h"

#include "System/Driver/Driver.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Camera/CameraSystem.h"

// TODO: DELETE
#include "System/Physics/PhysicsSystem.h"
#include "System/Collision/CollisionSystem.h"

#include "Object/GameObject.h"
#include "Utility/GUID.h"

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

		GameObject* CreateCamera(const std::string& _name)
		{
			auto pCore = EngineCore::GetInstance();
			GameObject *pObject = CreateGameObj(_name);
			auto p = pCore->GetSystem<CameraSystem>()->RequestComponent();
			p->SetSurface(pCore->GetSystem<GraphicsSystem>()->CreateView(2048u, 2048u, false));
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
	}
}
#endif //EDITOR
