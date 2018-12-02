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

#include "Math/Angles.h"
#include "Math/Vector4.h"
#include "Math/Matrix4.h"

#include "Component/Component.h"
#include "System/Graphics/GraphicsDefs.h"
#include "Math/Vector2.h"

namespace Dystopia
{
	class Transform;
	class Framebuffer;
	class CameraSystem;

	class _DLL_EXPORT Camera : public Component
	{
	public:

		using SYSTEM = CameraSystem;
		unsigned GetComponentType(void) const
		{
			return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllComponents>::value;
		};
		unsigned GetRealComponentType(void) const
		{
			return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, UsableComponents>::value;
		};
		static const std::string GetCompileName(void) { return "Camera"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }


		// ====================================== CONSTRUCTORS ======================================= // 

		Camera(const float _fWidth = 1.f, const float _fHeight = 1.f);
		Camera(const Camera&);
		~Camera(void);


		// ===================================== MEMBER FUNCTIONS ==================================== // 
		void Awake(void);
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
		Math::Pt3D ScreenToWorld(const Math::Pt3D&) const;

		// Sets the Camera's position in *GLOBAL* coordinates
		void SetPosition(const Math::Pt3D&);
		// Sets the Camera's position in *GLOBAL* coordinates
		void SetPosition(const float _x, const float _y, const float _z = -1000);

		void SetRotation(const float);
		void SetRotationDeg(const float);

		void SetSurface(Framebuffer*);
		void SetSurface(int);
		void SetSurfaceID(int _i);

		// Set the area the camera renders to in relative coordinates, from 0.0 to 1.0
		void SetViewport(float _x, float _y, float _nWidth, float _nHeight);

		void SetPerspective(Math::Angle _fFOV, float _fAspectRatio, float _fNear, float _fFar);
		void SetPerspective(float _fLeft, float _fRight, float _fTop, float _fBottom, float _fNear, float _fFar);
		void SetOrthographic(float _fWidth, float _fHeight, float _fNear, float _fFar);


		// Returns the *GLOBAL* position of the Camera
		Math::Pt3D GetPosition(void) const;

		Math::Vec4 GetSize(void) const;

		Framebuffer* GetSurface(void) const noexcept;
		const Gfx::Viewport GetViewport(void) const;
		const Math::Matrix4& GetViewMatrix(void) noexcept;
		const Math::Matrix4& GetProjectionMatrix(void) noexcept;

		Camera* Duplicate(void) const;
		void Serialise(TextSerialiser&) const;
		void Unserialise(TextSerialiser&);

		// Applies the camera bounds to screen
		// and calculates the camera matrices
		void SetCamera(void);

		bool DrawDebug(void) const noexcept;
		void SetDebugDraw(bool) noexcept;
		void SetSize(float _scale) const;

		void EditorUI(void) noexcept override;
		void EditorMasterCameraCheckbox();
		void EditorDebugCheckbox();
		void EditorProjectionDropdown();
		void EditorOptions();

		// TODO 
		bool mbDebugDraw;
		int  mnProjectionIndex = 0;
	private:

		Gfx::Viewport mViewport;
		Gfx::ClippingPlane mClippingPlane;

		Math::Mat4 mView;
		Math::Mat4 mInvScreen;
		Math::Mat4 mProjection;

		Framebuffer* mpSurface;
		int mnSurfaceID;

		// Orthographic Stuff
		float mfOrthoSize = 1.f;

		// Perspective Stuff
		int mnPersFOV_deg = 50;
	};
}



#endif		// INCLUDE GUARD

