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
#include "Math/Matrix4.h"
#include "Math/MathUtility.h"

#include "System/Scene/SceneSystem.h"
#include "System/Scene/Scene.h"
#include "System/Camera/CameraSystem.h"
#include "System/Driver/Driver.h"

#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"

#if EDITOR
#include "Editor/EGUI.h"
#endif


Dystopia::Camera::Camera(const float _fWidth, const float _fHeight) : Component{},
	mViewport{0, 0, _fWidth, _fHeight}, mView{}, mProjection{},
	mTransform{ nullptr }, mInvScreen {}
{
//	CameraManager::RegisterCamera(this);
}

Dystopia::Camera::Camera(const Camera& _oOther) : Component{ _oOther }, 
	mViewport{ _oOther.mViewport }, mView{ _oOther.mView }, mProjection{},
	mTransform{ nullptr }, mInvScreen{ _oOther.mInvScreen }
{}

Dystopia::Camera::~Camera(void)
{
//	CameraManager::DeregisterCamera(this);
}

void Dystopia::Camera::Init(void)
{
	if (nullptr == mTransform)
	{
		if (GetOwner())
		{
			mTransform = GetOwner()->GetComponent<Transform>();
		}
	}

	if (mnFlags & eObjFlag::FLAG_RESERVED)
		mnFlags |= eObjFlag::FLAG_ACTIVE;


	mProjection = Math::Matrix4{
		2.f / 800.f, .0f, .0f, .0f,
		.0f, -2.f / 500.f, .0f, .0f,
		.0f, .0f, 2.f / 1000.f, .0f,
		.0f, .0f, .0f, 1.f
	};
}
/*
void Dystopia::Camera::Update(const float)
{

}
*/
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

Math::Vec3D Dystopia::Camera::ScreenToWorld(const Math::Vec3D& _vCoords) const
{
	return mInvScreen * _vCoords;
}

void Dystopia::Camera::SetPosition(const Math::Pt3D& _vPos)
{
	mTransform->SetGlobalPosition(_vPos);
}

void Dystopia::Camera::SetPosition(const float _x, const float _y, const float _z)
{
	mTransform->SetGlobalPosition(_x, _y, _z);
}

void Dystopia::Camera::SetSize(const unsigned _nWidth, const unsigned _nHeight)
{
	mTransform->SetScale(1.f * _nWidth, 1.f * _nHeight, .0f);
}
/*
void Dystopia::Camera::SetZoom(const float _fZoom)
{
	SetZoom(_fZoom, _fZoom);
}

void Dystopia::Camera::SetZoom(const float _fZoomX, const float _fZoomY)
{
	mZoom.x = _fZoomX;
	mZoom.y = _fZoomY;
}

void Dystopia::Camera::SetRotation(const float _fRadians)
{
	mTransform->SetRotation(_fRadians);
}

void Dystopia::Camera::SetRotationDeg(const float _fDegrees)
{
	mTransform->SetRotationDeg(_fDegrees);
}
*/
void Dystopia::Camera::SetCamera(void)
{
	EngineCore::GetInstance()->GetSystem<CameraSystem>()->ApplyViewport(mViewport);

	mView = mTransform->GetTransformMatrix();

	mInvScreen =
		Math::Mat4{
			2.f / mViewport.mnWidth, .0f, -(1.f + 2.f * mViewport.mnX) / mViewport.mnWidth - 1.f, .0f,
			.0f, -2.f / mViewport.mnHeight, 1.f + (1.f + 2.f * mViewport.mnY) / mViewport.mnHeight, .0f,
			.0f, .0f, .0f, .0f,
			.0f, .0f, .0f, 1.f
		} * mView; // Screen space -> Viewport space -> projection space

	mView.AffineInverse();
}

void Dystopia::Camera::SetViewport(const int _x, const int _y, const int _nWidth, const int _nHeight)
{
	const Gfx::AbsViewport& master =
		EngineCore::GetInstance()->GetSystem<CameraSystem>()->GetMasterViewport();

	float InvWidth  = 1.f / master.mnWidth;
	float InvHeight = 1.f / master.mnHeight;

	mViewport.mnX = (_x - master.mnX) * InvWidth;
	mViewport.mnY = (_y - master.mnY) * InvHeight;
	mViewport.mnWidth  = _nWidth  * InvWidth;
	mViewport.mnHeight = _nHeight * InvHeight;
}

void Dystopia::Camera::SetViewport(float _x, float _y, float _nWidth, float _nHeight)
{
	mViewport.mnX = _x;
	mViewport.mnY = _y;
	mViewport.mnWidth = _nWidth;
	mViewport.mnHeight = _nHeight;
}

Math::Pt3D Dystopia::Camera::GetPosition(void) const
{
	return mTransform->GetGlobalPosition();
}
/*
Math::Vec3D Dystopia::Camera::GetZoom(void) const
{
	return mZoom;
}
*/
Math::Vec3D Dystopia::Camera::GetSize(void) const
{
	return mTransform->GetScale();
}

const Dystopia::Gfx::Viewport& Dystopia::Camera::GetViewport(void) const
{
	return mViewport;
}

const Math::Mat4& Dystopia::Camera::GetViewMatrix(void)
{
	return mView;
}

const Math::Matrix4 & Dystopia::Camera::GetProjectionMatrix(void)
{
	return mProjection;
}


Dystopia::Camera* Dystopia::Camera::Duplicate(void) const
{
	return nullptr;
}

void Dystopia::Camera::Serialise(TextSerialiser& _out) const
{
	_out.InsertStartBlock("Camera");
	_out << mID;
	_out.InsertEndBlock("Camera");
}

void Dystopia::Camera::Unserialise(TextSerialiser& _in)
{
	_in.ConsumeStartBlock();
	_in >> mID;
	_in.ConsumeEndBlock();

	if (GameObject* owner =
		EngineCore::GetInstance()->GetSystem<SceneSystem>()->GetCurrentScene().FindGameObject(mID))
	{
		owner->AddComponent(this, Camera::TAG{});
		Init();
	}
}

void Dystopia::Camera::EditorUI(void) noexcept
{
#if EDITOR
	Math::Vec2 viewport_xy { mViewport.mnX, mViewport.mnY };
	Math::Vec2 viewport_wh { mViewport.mnWidth, mViewport.mnHeight };

	//switch (EGUI::Display::VectorFields("Viewport", &viewport_wh, 0.01f, 0, 1.f))
	//{
	//case EGUI::eDragStatus::eSTART_DRAG:
	//	EGUI::GetCommandHND()->StartRecording<Camera>(GetOwner()->GetID(), &viewport_wh);
	//	break;
	//case EGUI::eDragStatus::eEND_DRAG:
	//	EGUI::GetCommandHND()->EndRecording();
	//	break;
	//}
	//
	//switch (EGUI::Display::VectorFields("Viewport Offset", &viewport_xy, 0.01f, 0, 1.f))
	//{
	//case EGUI::eDragStatus::eSTART_DRAG:
	//	EGUI::GetCommandHND()->StartRecording<Camera>(GetOwner()->GetID(), &viewport_wh);
	//	break;
	//case EGUI::eDragStatus::eEND_DRAG:
	//	EGUI::GetCommandHND()->EndRecording();
	//	break;
	//}

#endif 
}


