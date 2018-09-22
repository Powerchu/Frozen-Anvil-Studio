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
#include "System\Window\WindowManager.h"
#include "System\Window\Window.h"
#include "System\Driver\Driver.h"

#include "DataStructure\MagicArray.h"
#include "System\Graphics\GraphicsDefs.h"

#include <GL\glew.h>				// glViewport


Dystopia::CameraSystem::CameraSystem(void) noexcept
	: mpMasterCam{ nullptr }, mpUICam{ nullptr }, mMasterViewport{}
{

}


bool Dystopia::CameraSystem::Init(void)
{
	return true;
}

void Dystopia::CameraSystem::PostInit(void)
{
	mMasterViewport.mnWidth  = 2048;
	mMasterViewport.mnHeight = 2048;
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

MagicArray<Dystopia::Camera>& Dystopia::CameraSystem::GetAllCameras(void)
{
	return mComponents;
}


void Dystopia::CameraSystem::SetMasterViewport(int _x, int _y, int _w, int _h) noexcept
{
	mMasterViewport.mnX = _x;
	mMasterViewport.mnY = _y;
	mMasterViewport.mnWidth = _w;
	mMasterViewport.mnHeight = _h;
}

void Dystopia::CameraSystem::ApplyViewport(const Gfx::Viewport& _viewport)
{
	glViewport(
		static_cast<int>(mMasterViewport.mnX + _viewport.mnX * mMasterViewport.mnWidth),
		static_cast<int>(mMasterViewport.mnY + _viewport.mnY * mMasterViewport.mnHeight),
		static_cast<int>(_viewport.mnWidth  * mMasterViewport.mnWidth),
		static_cast<int>(_viewport.mnHeight * mMasterViewport.mnHeight)
	);
}


