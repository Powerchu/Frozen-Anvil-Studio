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

		void Shutdown(void);


		void SetMasterCamera(Camera*);
		Camera* GetMasterCamera(void) const;

		inline bool IsMasterCamera(Camera const*) const;

		void SetMasterViewport(int _nX, int _nY, int _nWidth, int _nHeight) noexcept;
		inline const Gfx::AbsViewport& GetMasterViewport(void) const noexcept;

		void ApplyViewport(const Gfx::Viewport&);

		CameraArr& GetAllCameras(void);


	private:

		Gfx::AbsViewport mMasterViewport;
		Camera* mpMasterCam, *mpUICam;
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


inline bool Dystopia::CameraSystem::IsMasterCamera(Dystopia::Camera const*  _pCamera) const
{
	return mpMasterCam == _pCamera;
}

inline const Dystopia::Gfx::AbsViewport& Dystopia::CameraSystem::GetMasterViewport(void) const noexcept
{
	return mMasterViewport;
}



#endif		// INCLUDE GUARD

