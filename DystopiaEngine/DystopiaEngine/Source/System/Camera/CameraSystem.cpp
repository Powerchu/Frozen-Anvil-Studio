/* HEADER *********************************************************************************/
/*!
\file	CameraSystem.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Manages game cameras.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Camera/CameraSystem.h" // File Header
#include "Component/Camera.h"
#include "System/Window/WindowManager.h"
#include "System/Window/Window.h"
#include "System/Driver/Driver.h"

#include "DataStructure/MagicArray.h"
#include "System/Graphics/GraphicsDefs.h"


Dystopia::CameraSystem::CameraSystem(void) noexcept
	: mpMasterCam{ nullptr }, mpUICam{ nullptr }
{

}


bool Dystopia::CameraSystem::Init(void)
{
	return true;
}

void Dystopia::CameraSystem::PostInit(void)
{

}

void Dystopia::CameraSystem::Update(float)
{

}

void Dystopia::CameraSystem::Shutdown(void)
{
	mComponents.clear();
}


void Dystopia::CameraSystem::SetMasterCamera(Camera* _pCamera)
{
	mpMasterCam = _pCamera;
}

Dystopia::Camera* Dystopia::CameraSystem::GetMasterCamera(void) const
{
	return mpMasterCam;
}

Dystopia::CameraSystem::CameraArr& Dystopia::CameraSystem::GetAllCameras(void)
{
	return mComponents;
}


