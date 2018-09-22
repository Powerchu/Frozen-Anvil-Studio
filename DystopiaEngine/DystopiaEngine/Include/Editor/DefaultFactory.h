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
		GameObject* CreateGameObj(const std::string& _name);

		GameObject* CreateCamera(const std::string& _name);

		// TODO: DELETE
		//GameObject* CreateStaticBox(const std::string& _name);
		//GameObject* CreateBox(const std::string& _name);
	}
}
#endif // _DEFAULT_FACTORY_H_
#endif //EDITOR



