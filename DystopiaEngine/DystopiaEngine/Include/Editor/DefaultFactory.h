/* HEADER *********************************************************************************/
/*!
\file	DefaultFactory.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "DataStructure/HashString.h"
#if EDITOR
#ifndef _DEFAULT_FACTORY_H_
#define _DEFAULT_FACTORY_H_
#include <string>

namespace Dystopia
{
	class GameObject;
	class Scene;
	namespace Factory
	{
		GameObject* CreateGameObj(const HashString& _name);
		GameObject* CreatePerformanceObj(const HashString& _name);
		GameObject* CreatePerformanceObjCol(const HashString& _name);

		GameObject* CreateCamera(const HashString& _name);

		HashString SaveAsPrefab(GameObject& _obj, const HashString& = "..\\DystopiaEngine\\Resource\\Prefab");
		GameObject* LoadFromPrefab(HashString _gameObjName, const HashString& = "..\\DystopiaEngine\\Resource\\Prefab");

		// TODO: DELETE
		GameObject* CreateStaticBox(const HashString& _name);
		GameObject* CreateBox(const HashString& _name);
		GameObject* CreateCircle(const HashString& _name);
	}
}
#endif // _DEFAULT_FACTORY_H_
#endif //EDITOR



