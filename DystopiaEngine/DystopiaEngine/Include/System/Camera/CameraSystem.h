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

#include "System/Base/Systems.h"
#include "System/Base/ComponentDonor.h"

#include "System/Graphics/GraphicsDefs.h"


namespace Dystopia
{
	class Camera;

	class CameraSystem : public Systems, public ComponentDonor<Camera>
	{
		using CameraArr = ComponentDonor<Camera>::Array_t;
	public:

		CameraSystem(void) noexcept;

		bool Init(void);
		void PostInit(void);

		void Update(float);

		void PostUpdate(void);

		void Shutdown(void);


		void SetMasterCamera(Camera*);
		Camera* GetMasterCamera(void) const;

		bool IsMasterCamera(Camera const*) const;

		CameraArr& GetAllCameras(void);


	private:

		Camera* mpMasterCam, *mpUICam;
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


inline bool Dystopia::CameraSystem::IsMasterCamera(Dystopia::Camera const*  _pCamera) const
{
	return mpMasterCam == _pCamera;
}



#endif		// INCLUDE GUARD

