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

Dystopia::Camera* Dystopia::CameraSystem::RequestComponent(void)
{
	return mCameras.Emplace();
}
