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

#include "System\Time\TimeSystem.h"
#include "System\Input\InputSystem.h"
#include "System\Sound\SoundSystem.h"
#include "System\Graphics\GraphicsSystem.h"

#include <tuple>

namespace
{
	using AllSys = std::tuple<
		Dystopia::TimeSystem, Dystopia::InputManager, Dystopia::SoundSystem, Dystopia::GraphicsSystem
	>;
}

SharedPtr<Dystopia::EngineCore> Dystopia::EngineCore::GetInstance(void)
{
	static SharedPtr<EngineCore> pInstance;
	return pInstance;
}

void Dystopia::EngineCore::Init(void)
{

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