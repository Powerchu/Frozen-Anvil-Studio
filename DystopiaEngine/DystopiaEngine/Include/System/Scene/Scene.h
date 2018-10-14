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

#include "Utility/Utility.h"
#include "Object/GameObject.h"
#include "DataStructure/AutoArray.h"

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

		template <typename ... Ty>
		GameObject* InsertGameObject(Ty&& ...);

		GameObject* FindGameObject(const uint64_t _nID);
		GameObject* FindGameObject(const std::string& _strName);

		inline AutoArray<GameObject>& GetAllGameObjects(void);

		void Serialise(TextSerialiser &) const;

		void Unserialise(TextSerialiser &);

		void SetSceneName(const std::string& _name);

		uint64_t GetSceneID(void) const;

		std::string GetSceneName(void) const;

	private:

		uint64_t			  mID;
		std::string			  mName;
		AutoArray<GameObject> mGameObjs;

		//Ctor::MagicArrayBuilder<GameObject>::SetBlockLimit<16>::SetBlockSize<256>::type mGameObjs;
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 



template <typename ... Ty>
Dystopia::GameObject* Dystopia::Scene::InsertGameObject(Ty&& ..._args)
{
	mGameObjs.EmplaceBack(Ut::Forward<Ty>(_args)...);
	return &mGameObjs.back();

	//return mGameObjs.Emplace(Ut::Forward<Ty>(_args)...);
}


inline AutoArray<Dystopia::GameObject>& Dystopia::Scene::GetAllGameObjects(void)
{
	return mGameObjs;
}





#endif		// INCLUDE GUARD

