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
#include "Editor/EditorCommands.h"


Dystopia::Camera::Camera(const float _fWidth, const float _fHeight) : Component{},
	mViewport{0, 0, _fWidth, _fHeight}, mClippingPlane{ -500.f,500.f } , mView{}, mProjection{},
	mInvScreen{}, mpSurface{ nullptr }, mbDebugDraw{ false }, mnSurfaceID{ 0 }
{

}

Dystopia::Camera::Camera(const Camera& _oOther) : Component{ _oOther }, 
	mViewport{ _oOther.mViewport }, mClippingPlane{ _oOther.mClippingPlane }, mView{}, mProjection{ _oOther.mProjection },
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
	SetOrthographic(800.f, 500.f, mClippingPlane.mnNear, mClippingPlane.mnFar);
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
	return !(mViewport.mnX > _vCoords.x || mViewport.mnY > _vCoords.y || mViewport.mnX + mViewport.mnWidth < _vCoords.x
		|| mViewport.mnY + mViewport.mnHeight < _vCoords.y);
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

void Dystopia::Camera::SetSurface(int _i)
{
	mnSurfaceID = _i;
	SetSurface(&EngineCore::Get<GraphicsSystem>()->GetView(_i));
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

void Dystopia::Camera::SetSize(float _scale) const
{
	const float z = GetSize().z;
	GetOwner()->GetComponent<Transform>()->SetScale(_scale,_scale,z);
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
	EGUI::PushLeftAlign(120.f);
	auto cmd = Editor::EditorMain::GetInstance()->GetSystem<Editor::EditorCommands>();
	int surfaceID = mnSurfaceID;
	if (EGUI::Display::DropDownSelection<21>("Surface", surfaceID, 4))
	{
		cmd->FunctionCommand(GetOwnerID(), cmd->MakeFnCommand<Camera, int>(&Camera::SetSurface, mnSurfaceID),
										   cmd->MakeFnCommand<Camera, int>(&Camera::SetSurface, surfaceID));
	}
	EditorProjectionDropdown();
	EGUI::PopLeftAlign();
	EditorOptions();


#endif 
}

void Dystopia::Camera::EditorProjectionDropdown(void)
{
	auto cmd = Editor::EditorMain::GetInstance()->GetSystem<Editor::EditorCommands>();
	
	std::string arr[2]{ " Orthographic"," Perspective"};

	if (EGUI::Display::DropDownSelection("Projection", mnProjectionIndex, arr, 160.f))
	{
		cmd->StartRec(&Camera::mProjection, this);

		switch (mnProjectionIndex)
		{
		case 0: // Orthographic
			SetOrthographic(800.f, 500.f, mClippingPlane.mnNear, mClippingPlane.mnFar);
			break;
		case 1: // Perspective
			SetPerspective(Math::Degrees{ static_cast<float>(mnPersFOV_deg) }, 0.625f, mClippingPlane.mnNear, mClippingPlane.mnFar);
			break;
		default:
			break;
		}
		cmd->EndRec(&Camera::mProjection, this);

		cmd->StartRec(&Camera::mViewport, this);
		SetViewport(0.f, 0.f, 1.f, 1.f);
		cmd->EndRec(&Camera::mViewport, this);
	}
}


void Dystopia::Camera::EditorOptions(void)
{
	EGUI::PushLeftAlign(120.f);

	auto cmd = Editor::EditorMain::GetInstance()->GetSystem<Editor::EditorCommands>();
	auto trans = GetOwner()->GetComponent<Transform>();
	auto scale = trans->GetScale();
	float z = trans->GetScale().z;
	float tempF = trans->GetScale().x;
	Math::Vec4 tempScale{scale};

	switch (mnProjectionIndex)
	{
	case 0: // ortho
		switch (EGUI::Display::DragFloat("Size", &tempF, 0.01f, -FLT_MAX, FLT_MAX, false, 160.f))
		{
		case EGUI::eDragStatus::eSTART_DRAG:
			cmd->StartRec<Transform, const Math::Vec4&>(&Transform::SetScale, scale);
			break;
		case EGUI::eDragStatus::eTABBED:
		case EGUI::eDragStatus::eDEACTIVATED:
		case EGUI::eDragStatus::eEND_DRAG:
		case EGUI::eDragStatus::eENTER:
			tempScale.x = tempScale.y = tempF;
			cmd->EndRec<Transform, const Math::Vec4&>(GetOwnerID(), &Transform::SetScale, tempScale);
			break;
		default:
		case EGUI::eDragStatus::eNO_CHANGE:
		case EGUI::eDragStatus::eDRAGGING:
			trans->SetScale(tempF, tempF, z);
			break;
		}
		break;
	case 1: // persc
		switch (EGUI::Display::SliderInt("Cam FOV", &mnPersFOV_deg, 1, 179, false, 160.f))
		{
		case EGUI::eDragStatus::eSTART_DRAG:
			break;
		case EGUI::eDragStatus::eTABBED:
		case EGUI::eDragStatus::eDEACTIVATED:
		case EGUI::eDragStatus::eEND_DRAG:
		case EGUI::eDragStatus::eENTER:
			break;
		default:
		case EGUI::eDragStatus::eNO_CHANGE:
		case EGUI::eDragStatus::eDRAGGING:
			SetPerspective(Math::Degrees{ static_cast<float>(mnPersFOV_deg) }, 0.625f, mClippingPlane.mnNear, mClippingPlane.mnFar);
			break;
		}
		break;
	default:
		break;
	}

	EGUI::PopLeftAlign();


	/*
	 * Clipping Plane
	 */

	EGUI::Display::LabelWrapped("Clipping Planes");
	switch (EGUI::Display::DragFloat("Near", &mClippingPlane.mnNear, 0.01f, -500.f, FLT_MAX, false, 100.f))
	{
	case EGUI::eDragStatus::eSTART_DRAG:
		cmd->StartRec(&Camera::mClippingPlane, this);
		break;
	case EGUI::eDragStatus::eTABBED:
	case EGUI::eDragStatus::eDEACTIVATED:
	case EGUI::eDragStatus::eEND_DRAG:
	case EGUI::eDragStatus::eENTER:
		cmd->EndRec(&Camera::mClippingPlane, this);
		break;
	default:
	case EGUI::eDragStatus::eNO_CHANGE:
	case EGUI::eDragStatus::eDRAGGING:
		if (!mnProjectionIndex) // Ortho
		{
			SetOrthographic(800.f, 500.f, mClippingPlane.mnNear, mClippingPlane.mnFar);
		}
		else // pers
		{
			SetPerspective(Math::Degrees{ static_cast<float>(mnPersFOV_deg) }, 0.625f, mClippingPlane.mnNear, mClippingPlane.mnFar);
		}
		break;
	}
	EGUI::SameLine(8);
	EGUI::ChangeAlignmentYOffset(0);
	switch (EGUI::Display::DragFloat("Far", &mClippingPlane.mnFar, 0.01f, mClippingPlane.mnNear+0.01f, FLT_MAX, false, 100.f))
	{
	case EGUI::eDragStatus::eSTART_DRAG:
		cmd->StartRec(&Camera::mClippingPlane, this);
		break;
	case EGUI::eDragStatus::eTABBED:
	case EGUI::eDragStatus::eDEACTIVATED:
	case EGUI::eDragStatus::eEND_DRAG:
	case EGUI::eDragStatus::eENTER:
		if (mClippingPlane.mnFar <= mClippingPlane.mnNear)
			mClippingPlane.mnFar = mClippingPlane.mnNear + 0.01f;
		cmd->EndRec(&Camera::mClippingPlane, this);
		break;
	default:
	case EGUI::eDragStatus::eNO_CHANGE:
	case EGUI::eDragStatus::eDRAGGING:
		break;
	}
	EGUI::ChangeAlignmentYOffset();

	/*
	 * Viewport Rect
	 */

	EGUI::Display::LabelWrapped("Viewport Rect");
	switch (EGUI::Display::DragFloat("X", &mViewport.mnX, 0.01f, 0, 1.f, false, 90.f))
	{
	case EGUI::eDragStatus::eSTART_DRAG:
		cmd->StartRec(&Camera::mViewport, this);
		break;
	case EGUI::eDragStatus::eTABBED:
	case EGUI::eDragStatus::eDEACTIVATED:
	case EGUI::eDragStatus::eEND_DRAG:
	case EGUI::eDragStatus::eENTER:
		if (mViewport.mnX < 0) mViewport.mnX = 0;
		else if (mViewport.mnX > 1) mViewport.mnX = 1;
		cmd->EndRec(&Camera::mViewport, this);
		break;
	default:
	case EGUI::eDragStatus::eNO_CHANGE:
	case EGUI::eDragStatus::eDRAGGING:
		break;
	}
	EGUI::SameLine(8);
	EGUI::ChangeAlignmentYOffset(0);
	switch (EGUI::Display::DragFloat("Y", &mViewport.mnY, 0.01f, 0, 1.f, false, 90.f))
	{
	case EGUI::eDragStatus::eSTART_DRAG:
		cmd->StartRec(&Camera::mViewport, this);
		break;
	case EGUI::eDragStatus::eTABBED:
	case EGUI::eDragStatus::eDEACTIVATED:
	case EGUI::eDragStatus::eEND_DRAG:
	case EGUI::eDragStatus::eENTER:
		if (mViewport.mnY < 0) mViewport.mnY = 0;
		else if (mViewport.mnY > 1) mViewport.mnY = 1;
		cmd->EndRec(&Camera::mViewport, this);
		break;
	default:
	case EGUI::eDragStatus::eNO_CHANGE:
	case EGUI::eDragStatus::eDRAGGING:
		break;
	}
	EGUI::ChangeAlignmentYOffset();

	switch (EGUI::Display::DragFloat("W", &mViewport.mnWidth, 0.01f, 0, 1.f, false, 90.f))
	{
	case EGUI::eDragStatus::eSTART_DRAG:
		cmd->StartRec(&Camera::mViewport, this);
		break;
	case EGUI::eDragStatus::eTABBED:
	case EGUI::eDragStatus::eDEACTIVATED:
	case EGUI::eDragStatus::eEND_DRAG:
	case EGUI::eDragStatus::eENTER:
		if (mViewport.mnWidth < 0) mViewport.mnWidth = 0;
		else if (mViewport.mnWidth > 1) mViewport.mnWidth = 1;
		cmd->EndRec(&Camera::mViewport, this);
		break;
	default:
	case EGUI::eDragStatus::eNO_CHANGE:
	case EGUI::eDragStatus::eDRAGGING:
		break;
	}
	EGUI::SameLine(8);
	EGUI::ChangeAlignmentYOffset(0);
	switch (EGUI::Display::DragFloat("H", &mViewport.mnHeight, 0.01f, 0, 1.f, false, 90.f))
	{
	case EGUI::eDragStatus::eSTART_DRAG:
		cmd->StartRec(&Camera::mViewport, this);
		break;
	case EGUI::eDragStatus::eTABBED:
	case EGUI::eDragStatus::eDEACTIVATED:
	case EGUI::eDragStatus::eEND_DRAG:
	case EGUI::eDragStatus::eENTER:
		if (mViewport.mnHeight < 0) mViewport.mnHeight = 0;
		else if (mViewport.mnHeight> 1) mViewport.mnHeight = 1;
		cmd->EndRec(&Camera::mViewport, this);
		break;
	default:
	case EGUI::eDragStatus::eNO_CHANGE:
	case EGUI::eDragStatus::eDRAGGING:
		break;
	}
	EGUI::ChangeAlignmentYOffset();



}


