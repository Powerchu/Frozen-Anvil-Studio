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

#include <string>

namespace Dystopia
{
	class Scene;

	class SceneSystem : public Systems
	{
	public:
		
		void LoadScene(const std::string&);

		Scene& GetCurrentScene(void) const;

		template <typename T>
		T* FindGameObject(const std::string&);

		template <typename T>
		T* FindGameObject(size_t);

	private:

		Scene *mpNextScene;
		Scene *mpCurrScene;
		Scene *mpPrevScene;
	};
}

#endif


