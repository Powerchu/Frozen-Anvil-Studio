/* HEADER *********************************************************************************/
/*!
\file	Driver.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Internal part of Game Engine that runs everything

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System\Driver\Driver.h"
#include "DataStructure\SharedPtr.h"

SharedPtr<Dystopia::EngineCore> Dystopia::EngineCore::GetInstance(void)
{
	static SharedPtr<EngineCore> pInstance;
	return pInstance;
}

#if !EDITOR

int WinMain(HINSTANCE, HINSTANCE, char *, int)
{
	auto CORE = Dystopia::EngineCore::GetInstance();

	/*
	CORE->ExecuteGame();
	*/

	return 0;
}

#endif