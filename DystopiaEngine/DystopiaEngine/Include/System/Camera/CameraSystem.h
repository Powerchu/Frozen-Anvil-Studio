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
#ifndef _CAMERA_SYS_H_
#define _CAMERA_SYS_H_

#include "DataStructure\MagicArray.h"


namespace Dystopia
{
	class Camera;

	class CameraSystem final
	{
	public:
		
		CameraSystem(void) noexcept;

		Camera* RequestComponent(void);

		void SetMasterCamera(Camera*);
		Camera* GetMasterCamera(void) const;

		inline bool IsMasterCamera(Camera const*) const;

		MagicArray<Camera> GetAllCameras(void) const;


	private:

		Camera* mpMasterCam, *mpUICam;
		Ctor::MagicArrayBuilder<Camera>::SetBlockSize<64>::type mCameras;
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


inline bool Dystopia::CameraSystem::IsMasterCamera(Dystopia::Camera const*  _pCamera) const
{
	return mpMasterCam == _pCamera;
}



#endif		// INCLUDE GUARD

