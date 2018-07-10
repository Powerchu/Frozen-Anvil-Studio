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

#include "System\Base\Systems.h"
#include "System\Scene\Scene.h"

#include <string>

namespace Dystopia
{
	class SceneSystem : public Systems
	{
	public:

		SceneSystem(void);
		~SceneSystem(void);

		void PreInit(void);
		bool Init(void);
		void PostInit(void);

		void Update(float);
		void Shutdown(void);

		void LoadDefaults(void);
		void LoadSettings(TextSerialiser&);

//		void ReceiveMessage(const Message&);
		
		void LoadScene(const std::string&);

		Scene& GetCurrentScene(void) const;

		template <typename T>
		T* FindGameObject(const std::string& _strName);

		template <typename T>
		T* FindGameObject(size_t _nID);

	private:

		Scene *mpNextScene;
		Scene *mpCurrScene;
		Scene *mpPrevScene;
	};
}

#endif


