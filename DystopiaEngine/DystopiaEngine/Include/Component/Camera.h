/* HEADER *********************************************************************************/
/*!
\file	Camera.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Wrapper class for camera functions.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Math/Vector4.h"
#include "Math/Matrix4.h"

#include "Component/Component.h"
#include "System/Graphics/GraphicsDefs.h"

namespace Dystopia
{
	class Transform;
	class CameraSystem;

	class _DLL_EXPORT Camera : public Component
	{
	public:

		using SYSTEM = CameraSystem;
		unsigned GetComponentType(void) const
		{
			return Utility::MetaFind_t<Utility::Decay_t<decltype(*this)>, AllComponents>::value;
		};

		static const std::string GetCompileName(void) { return "Camera"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }


		// ====================================== CONSTRUCTORS ======================================= // 

		Camera(const float _fWidth = 1.f, const float _fHeight = 1.f);
		Camera(const Camera&);
		~Camera(void);


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		void Init(void);
		/*
		void Update(const float dt);
		*/
		void SetMasterCamera(void);
		bool IsMasterCamera(void) const;

		void InitiallyActive(bool);

		// Checks if screen coords is within the Viewport of the current camera
		bool IsWithinCameraBounds(const Math::Pt3D&) const;

		// Converts Screen coords to World coords based on this camera
		Math::Pt3D ScreenToWorld(const Math::Vec3D&) const;

		// Sets the Camera's position in *GLOBAL* coordinates
		void SetPosition(const Math::Pt3D&);
		// Sets the Camera's position in *GLOBAL* coordinates
		void SetPosition(const float _x, const float _y, const float _z = -1000);

		// Modifies the view area of the camera
		void SetSize(const unsigned _nWidth, const unsigned _nHeight);

		//// Zoom X & Y axis uniformly
		//// Input taken in percentage form, 1.0f == 100%.
		//void SetZoom(const float _fZoom);
		//// Input taken in percentage form, 1.0f == 100%.
		//void SetZoom(const float _fZoomX, const float _fZoomY);

		void SetRotation(const float);
		void SetRotationDeg(const float);

		// Sets the area the camera renders to in absolute screen coordinates
		void SetViewport(const int _x, const int _y, const int _nWidth, const int _nHeight);
		// Set the area the camera renders to in relative coordinates, from 0 to 1.0
		void SetViewport(float _x, float _y, float _nWidth, float _nHeight);

		// Returns the *GLOBAL* position of the Camera
		Math::Pt3D GetPosition(void) const;

		Math::Vec4 GetSize(void) const;

		const Gfx::Viewport& GetViewport(void) const;
		const Math::Matrix4& GetViewMatrix(void);
		const Math::Matrix4& GetProjectionMatrix(void);

		Camera* Duplicate(void) const;
		void Serialise(TextSerialiser&) const;
		void Unserialise(TextSerialiser&);

		// Applies the camera bounds to screen
		// and calculates the camera matrices
		void SetCamera(void);

		void EditorUI(void) noexcept;

	private:

		Gfx::Viewport mViewport;

		Transform* mTransform;

		Math::Mat4 mView;
		Math::Mat4 mInvScreen;
		Math::Mat4 mProjection;
	};
}



#endif		// INCLUDE GUARD

