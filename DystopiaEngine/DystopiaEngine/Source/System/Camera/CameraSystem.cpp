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
#include "System\Camera\CameraSystem.h" // File Header
#include "Component\Camera.h"

#include "DataStructure\MagicArray.h"


Dystopia::CameraSystem::CameraSystem(void) noexcept
	: mpMasterCam{ nullptr }, mpUICam{ nullptr }, mCameras{}
{

}


Dystopia::Camera* Dystopia::CameraSystem::RequestComponent(void)
{
	return mCameras.Emplace();
}

void Dystopia::CameraSystem::SetMasterCamera(Camera* _pCamera)
{
	mpMasterCam = _pCamera;
}

Dystopia::Camera* Dystopia::CameraSystem::GetMasterCamera(void) const
{
	return mpMasterCam;
}

MagicArray<Dystopia::Camera>& Dystopia::CameraSystem::GetAllCameras(void)
{
	return mCameras;
}

