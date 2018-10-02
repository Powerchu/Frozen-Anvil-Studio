/* HEADER *********************************************************************************/
/*!
\file	AdminCalls.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _ADMIN_CALL_H_
#define _ADMIN_CALL_H_

#include "Editor/DefaultFactory.h"
#include "Object/GameObject.h"
#include <map>

namespace Dystopia
{
	namespace Admin
	{
		static std::map<std::string, GameObject*(*)(const std::string&)> g_AdminFuncs
		{
			std::make_pair("SpawnObj", &Factory::CreateGameObj),
			std::make_pair("SpawnObj_T", &Factory::CreatePerformanceObj)
		};

		bool ValidCommand(const std::string& _toValidate, int& _outCount, std::string& _outParam, std::string& _outAdminFuncName);
	}
}

#endif // _ADMIN_CALL_H_
#endif //EDITOR



