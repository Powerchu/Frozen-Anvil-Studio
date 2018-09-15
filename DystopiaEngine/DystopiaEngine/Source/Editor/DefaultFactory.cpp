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
#include "Editor\DefaultFactory.h"

#include "Component\Camera.h"
#include "Component\Collider.h"
#include "Component\Renderer.h"
#include "Component\RigidBody.h"

#include "System\Driver\Driver.h"
#include "System\Graphics\GraphicsSystem.h"
#include "System\Camera\CameraSystem.h"

#include "Object\GameObject.h"
#include "Utility\GUID.h"

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

		GameObject* CreateCamera(const std::string& _name)
		{
			GameObject *pObject = CreateGameObj(_name);
			auto p = EngineCore::GetInstance()->GetSystem<CameraSystem>()->RequestComponent();
			p->SetOwner(pObject);
			p->Init();
			pObject->AddComponent(p, typename Camera::TAG{});
			return pObject;
		}
	}
}
#endif //EDITOR
