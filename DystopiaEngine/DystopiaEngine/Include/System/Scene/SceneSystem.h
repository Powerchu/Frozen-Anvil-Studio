/* HEADER *********************************************************************************/
/*!
\file	SceneSystem.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Manager for game scenes

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _SCENE_SYSTEM_H_
#define _SCENE_SYSTEM_H_

#pragma warning(push)
#pragma warning(disable : 4251)

#include "System/Base/Systems.h"
#include "System/Scene/Scene.h"
#include "System/Tag/Tags.h"
#include "Globals.h"
//#include "DataStructure/Stack.h"

#include <string>


namespace Dystopia
{
	class _DLL_EXPORT SceneSystem : public Systems
	{
	public:

		SceneSystem(void);
		~SceneSystem(void);

		void PreInit(void);
		bool Init(void);
		void PostInit(void);

		void FixedUpdate(float);
		void Update(float);
		void PostUpdate(void);
		void Shutdown(void);

		void LoadDefaults(void);
		void LoadSettings(TextSerialiser&);

//		void ReceiveMessage(const Message&);

		void RestartScene(void);
		void LoadScene(const std::string& _strName);
		void LoadScene(const char* _strName);
		void SaveScene(const std::string & _strName, const std::string& _sceneName = "defaultSceneName");

		inline Scene& GetCurrentScene(void) const;
		inline Scene& GetNextScene(void) const;
		inline Scene& GetActiveScene(void) const;

		inline GameObject* FindGameObject(uint64_t _nID);
		inline GameObject* FindGameObject(const HashString& _strName);
		inline GameObject* FindGameObject_cstr(const char * const _str);
		inline HashString GetCurrentSceneName(void) const;

		AutoArray<GameObject*>& FindGameObjectByTag(const HashString& _tag);

		GameObject* Instantiate(const HashString& _prefabName, const Math::Pt3D& _position);

	private:

		Scene *mpNextScene;
		Scene *mpCurrScene;
		bool mbRestartScene;

		HashString mLastSavedData;
		HashString mNextSceneFile;

		void SceneChanged(void);
	};

	// ============================================ FUNCTION DEFINITIONS ============================================ // 


	inline Scene& Dystopia::SceneSystem::GetCurrentScene(void) const
	{
		return *mpCurrScene;
	}

	inline GameObject* Dystopia::SceneSystem::FindGameObject(uint64_t _nID)
	{
		return mpCurrScene->FindGameObject(_nID);
	}

	inline GameObject* Dystopia::SceneSystem::FindGameObject(const HashString& _strName)
	{
		return mpCurrScene->FindGameObject(_strName);
	}

	inline Scene& Dystopia::SceneSystem::GetNextScene(void) const
	{
		return *mpNextScene;
	}

	inline Scene& Dystopia::SceneSystem::GetActiveScene(void) const
	{
		return (mpNextScene == mpCurrScene) ? *mpCurrScene : *mpNextScene;
	}
}

#pragma warning(pop)

#endif





