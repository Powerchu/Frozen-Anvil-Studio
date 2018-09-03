/* HEADER *********************************************************************************/
/*!
\file	CameraSystem.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Manages game cameras.

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _CAMERA_SYS_H_
#define _CAMERA_SYS_H_

#include "System\Base\Systems.h"

#include "DataStructure\MagicArray.h"
#include "System\Graphics\GraphicsDefs.h"


namespace Dystopia
{
	class Camera;

	class CameraSystem : public Systems
	{
	public:

		CameraSystem(void) noexcept;

		Camera* RequestComponent(void);

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

		MagicArray<Camera>& GetAllCameras(void);


	private:

		Gfx::AbsViewport mMasterViewport;
		Camera* mpMasterCam, *mpUICam;
		Ctor::MagicArrayBuilder<Camera>::SetBlockSize<64>::type mCameras;
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

