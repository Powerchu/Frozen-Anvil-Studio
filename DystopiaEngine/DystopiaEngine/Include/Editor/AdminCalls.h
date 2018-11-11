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
		static std::map<HashString, GameObject*(*)(const  HashString&)> g_AdminFuncs
		{
			std::make_pair("SpawnObj", &Factory::CreateGameObj),
			std::make_pair("SpawnObj_T", &Factory::CreatePerformanceObj),
			std::make_pair("SpawnObj_C", &Factory::CreatePerformanceObjCol)
		};

		bool ValidCommand(const HashString& _toValidate, int& _outCount, HashString& _outParam, HashString& _outAdminFuncName);
	}
}

#endif // _ADMIN_CALL_H_
#endif //EDITOR



