/* HEADER *********************************************************************************/
/*!
\file	Camera.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Wrapper class for camera functions.
	Graphics will render the screen based on the camera.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Component/Camera.h"		// File header
#include "Component/Transform.h"

#include "System/Graphics/GraphicsSystem.h"
#include "System/Graphics/Framebuffer.h"
#include "System/Camera/CameraSystem.h"
#include "System/Driver/Driver.h"
#include "System/Scene/SceneSystem.h"
#include "System/Scene/Scene.h"

#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"

#include "Math/Matrix4.h"
#include "Math/MathUtility.h"

#if EDITOR
#include "Editor/EGUI.h"
#endif

#include <cmath>


Dystopia::Camera::Camera(const float _fWidth, const float _fHeight) : Component{},
	mViewport{0, 0, _fWidth, _fHeight}, mView{}, mProjection{},
	mInvScreen{}, mpSurface{ nullptr }, mbDebugDraw{ false }, mnSurfaceID{ 0 }
{

}

Dystopia::Camera::Camera(const Camera& _oOther) : Component{ _oOther }, 
	mViewport{ _oOther.mViewport }, mView{}, mProjection{ _oOther.mProjection },
	mInvScreen{}, mpSurface{ _oOther.mpSurface }, mbDebugDraw{ _oOther.mbDebugDraw }, 
	mnSurfaceID{ _oOther.mnSurfaceID }
{

}

Dystopia::Camera::~Camera(void)
{
}

void Dystopia::Camera::Awake(void)
{
	SetSurface(&EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->GetView(mnSurfaceID));
	SetOrthographic(800.f, 500.f, -500.f, 500.f);
}

void Dystopia::Camera::Init(void)
{
	if (mnFlags & eObjFlag::FLAG_RESERVED)
		mnFlags |= eObjFlag::FLAG_ACTIVE;

}

void Dystopia::Camera::SetMasterCamera(void)
{
	EngineCore::GetInstance()->GetSystem<CameraSystem>()->SetMasterCamera(this);
}

bool Dystopia::Camera::IsMasterCamera(void) const
{
	return EngineCore::GetInstance()->GetSystem<CameraSystem>()->IsMasterCamera(this);
}


void Dystopia::Camera::InitiallyActive(bool b)
{
	if (b)
		mnFlags |= eObjFlag::FLAG_RESERVED;
	else
		mnFlags &= ~eObjFlag::FLAG_RESERVED;
}

bool Dystopia::Camera::IsWithinCameraBounds(const Math::Pt3D& _vCoords) const
{
	if (
		mViewport.mnX > _vCoords.x ||
		mViewport.mnY > _vCoords.y ||
		mViewport.mnX + mViewport.mnWidth < _vCoords.x ||
		mViewport.mnY + mViewport.mnHeight < _vCoords.y
		)
	{
		return false;
	}

	return true;
}


Math::Pt3D Dystopia::Camera::ScreenToWorld(const Math::Pt3D& _vCoords) const
{
	Math::Pt3D temp = mInvScreen * _vCoords;
	return temp * Math::Reciprocal(temp.wwww);
}


void Dystopia::Camera::SetPosition(const Math::Pt3D& _vPos)
{
	GetOwner()->GetComponent<Transform>()->SetGlobalPosition(_vPos);
}

void Dystopia::Camera::SetPosition(const float _x, const float _y, const float _z)
{
	GetOwner()->GetComponent<Transform>()->SetGlobalPosition(_x, _y, _z);
}

void Dystopia::Camera::SetSurface(Framebuffer* _ptr)
{
	mpSurface = _ptr;
}

void Dystopia::Camera::SetViewport(float _x, float _y, float _nWidth, float _nHeight)
{
#if EDITOR
	if (((_x + _nWidth) > 1.f) || ((_y + _nHeight > 1.f)))
	{
		DEBUG_BREAK(true, "Camera Error: Invalid viewport specified!\n");
		return;
	}
#endif

	mViewport.mnX = _x;
	mViewport.mnY = _y;
	mViewport.mnWidth = _nWidth;
	mViewport.mnHeight = _nHeight;
}


void Dystopia::Camera::SetPerspective(Math::Angle _fFOV, float _fAspectRatio, float _fNear, float _fFar)
{
	float fDistZ = 1.f / (_fFar - _fNear);
	float fDist = 1.f / std::tanf(_fFOV.Radians() * .5f);

	mProjection = Math::Matrix4{
		fDist * _fAspectRatio, .0f  , .0f          , .0f,
		.0f                  , fDist, .0f          , .0f,
		.0f                  , .0f  , -2.f * fDistZ, (_fFar + _fNear) * fDistZ,
		.0f                  , .0f  , -1.f         , .0f
	};
}

void Dystopia::Camera::SetPerspective(float _fLeft, float _fRight, float _fTop, float _fBottom, float _fNear, float _fFar)
{
	float fDistZ  = 1.f / (_fFar - _fNear);
	float fWidth  = 1.f / (_fRight - _fLeft);
	float fHeight = 1.f / (_fTop - _fBottom);

	mProjection = Math::Matrix4{
		(2.f * _fNear) * fWidth, .0f                     , (_fRight + _fLeft) * fWidth , .0f,
		.0f                    , (2.f * _fNear) * fHeight, (_fTop + _fBottom) * fHeight, .0f,
		.0f                    , .0f                     , -(_fFar + _fNear) * fDistZ  , 2.f * _fFar * _fNear * fDistZ,
		.0f                    , .0f                     , -1.f                        , .0f
	};
}

void Dystopia::Camera::SetOrthographic(float _fWidth, float _fHeight, float _fNear, float _fFar)
{
	float fDistZ = 1.f / (_fFar - _fNear);

	mProjection = Math::Matrix4{
		2.f / _fWidth, .0f               , .0f          , .0f,
		.0f          , -2.f / _fHeight   , .0f          , .0f,
		.0f          , .0f               , -2.f * fDistZ, (_fNear + _fFar) * fDistZ,
		.0f          , .0f               , .0f          , 1.f
	};
}


void Dystopia::Camera::SetCamera(void)
{
	auto mTransform = GetOwner()->GetComponent<Transform>();

	mView = mTransform->GetTransformMatrix();

	mInvScreen = mView * Math::AffineInverse(mProjection);
		// * 
		//Math::Mat4{
		//	2.f / (masterView.mnWidth), .0f, -(1.f + 2.f * masterView.mnX) / masterView.mnWidth - 1.f, .0f,
		//	.0f, 2.f / masterView.mnHeight, 1.f + (1.f + 2.f * masterView.mnY) / masterView.mnHeight, .0f,
		//	.0f, .0f, .0f, .0f,
		//	.0f, .0f, .0f, 1.f
		//}; // Screen space -> Viewport space -> projection space

	mView.AffineInverse();
}

void Dystopia::Camera::SetDebugDraw(bool _bDebugDraw) noexcept
{
	mbDebugDraw = _bDebugDraw;
}

bool Dystopia::Camera::DrawDebug(void) const noexcept
{
	return mbDebugDraw;
}


Math::Pt3D Dystopia::Camera::GetPosition(void) const
{
	return GetOwner()->GetComponent<Transform>()->GetGlobalPosition();
}

Math::Vec3D Dystopia::Camera::GetSize(void) const
{
	return GetOwner()->GetComponent<Transform>()->GetScale();
}


Dystopia::Framebuffer* Dystopia::Camera::GetSurface(void) const noexcept
{
	return mpSurface;
}

const Dystopia::Gfx::Viewport Dystopia::Camera::GetViewport(void) const
{
	DEBUG_BREAK(!mpSurface, "Camera Error: Camera has no surface!\n");

	Gfx::Viewport ret{
		mViewport.mnX * mpSurface->GetWidth(),
		mViewport.mnY * mpSurface->GetHeight(),
		mViewport.mnWidth * mpSurface->GetWidth(),
		mViewport.mnHeight * mpSurface->GetHeight()
	};

	return ret;
}

const Math::Mat4& Dystopia::Camera::GetViewMatrix(void) noexcept
{
	return mView;
}

const Math::Mat4& Dystopia::Camera::GetProjectionMatrix(void) noexcept
{
	return mProjection;
}

Dystopia::Camera* Dystopia::Camera::Duplicate(void) const
{
	return static_cast<ComponentDonor<Camera>*>(EngineCore::GetInstance()->Get<Camera::SYSTEM>())->RequestComponent(*this);
}

void Dystopia::Camera::Serialise(TextSerialiser& _out) const
{
	_out.InsertStartBlock("Camera");
	Component::Serialise(_out);
	_out << mnSurfaceID;
	_out << mbDebugDraw;
	_out.InsertEndBlock("Camera");
}

void Dystopia::Camera::Unserialise(TextSerialiser& _in)
{
	_in.ConsumeStartBlock();
	Component::Unserialise(_in);

	_in >> mnSurfaceID;
	_in >> mbDebugDraw;
	_in.ConsumeEndBlock();
}

void Dystopia::Camera::EditorUI(void) noexcept
{
#if EDITOR

#endif 
}


