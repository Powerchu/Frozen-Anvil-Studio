/* HEADER *********************************************************************************/
/*!
\file	Scene.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Scene

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

#include "System\Base\Systems.h"
#include "Object\GameObject.h"
#include "DataStructure\AutoArray.h"

#include <string>

namespace Dystopia
{
	class Scene
	{
	public:

		Scene(void);
		~Scene(void);

		void Init(void);

		void FixedUpdate(float _dt);
		void Update(float _dt);
		void PostUpdate(void);

		void Shutdown(void);

		GameObject* FindGameObject(const size_t _nID);
		GameObject* FindGameObject(const std::string& _strName);

	private:

		AutoArray<GameObject> mGameObjs;
	};
}

#endif


