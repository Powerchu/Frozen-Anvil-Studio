/* HEADER *********************************************************************************/
/*!
\file	Transform2D.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Component to store the information about the object's transformations in space.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Component/Transform.h"
#include "Object/GameObject.h"
#include "Math/Matrix4.h"
#include "Math/Vectors.h"
#include "IO/TextSerialiser.h"
#include "System/Driver/Driver.h"
#include "System/Scene/SceneSystem.h"
#include "System/Scene/Scene.h"
#include "Utility/GUID.h"
#include "Component/SpriteRenderer.h"
#include "Component/TextRenderer.h"
#include "System/Graphics/VertexDefs.h"

#if EDITOR
#include "Editor/EGUI.h"
#include "Editor/ConsoleLog.h"
#include "Editor/EditorCommands.h"
#include "Editor/EditorMain.h"
// #include "System/Logger/LoggerSystem.h"
#endif 

Dystopia::Transform::Transform(GameObject* _pOwner) noexcept
	: mRotation{ }, mScale{ 1.f, 1.f, 1.f }, mPosition{ .0f, .0f, .0f, 1.f }, 
	mMatrix{}, mbChanged{ true }, mpParent{ nullptr }, mnParentID{ GUIDGenerator::INVALID },
	Component { _pOwner }
{
	
}

Dystopia::Transform::Transform(const Transform& _oOther) :
	mRotation{ _oOther.mRotation }, mScale{ _oOther.mScale }, mPosition{ _oOther.mPosition },
	mMatrix{}, mbChanged{ true }, mpParent{ nullptr }, mnParentID{ _oOther.mnParentID }, Component{ _oOther }
	//,mfOpacity(_oOther.mfOpacity)
{

}

void Dystopia::Transform::Awake(void) 
{
	if (mnParentID != GUIDGenerator::INVALID && mnParentID)
	{
		GameObject *p = EngineCore::GetInstance()->GetSystem<SceneSystem>()->FindGameObject(mnParentID);
		if (p)
			SetParent(p->GetComponent<Transform>());
	}
}

void Dystopia::Transform::Init(void)
{
	//if (!mfOpacity) SetOpacity(1.f);
}

void Dystopia::Transform::SetParent(Transform* _pParent)
{
	if (mpParent)
		RemoveParent();

	if (IsDescendant(_pParent))
		return;

	if (_pParent == this)
		__debugbreak();

	mpParent = _pParent;

    if (mpParent)
    {
		// Convert our data to the parent's local coordinates
        Math::Mat4 const InvTrans = Math::AffineInverse(mpParent->GetTransformMatrix());

        mScale		= mpParent->GetGlobalScale().Reciprocal() * mScale;
        mPosition	= InvTrans * mPosition;
		mRotation   = mpParent->GetGlobalRotation().Conjugate() * mRotation;
		mbChanged	= true;
		mnParentID = mpParent->GetOwnerID();

		mpParent->OnChildAdd(this);
    }
}

void Dystopia::Transform::OnParentRemove(Transform* _pParent)
{
	// Convert our data to world coordinatesF
	Math::Mat4 const Trans = _pParent->GetTransformMatrix();

	mScale		= _pParent->GetGlobalScale() * mScale;
	mPosition	= Trans * mPosition;
	mRotation   = _pParent->GetGlobalRotation() * mRotation;

	mbChanged = true;
}

void Dystopia::Transform::RemoveParent(void)
{
	if (mpParent)
	{
		Transform* parent = mpParent;
		mpParent = nullptr;

		//OnParentRemove(parent);
		parent->RemoveChild(this);
		mnParentID = GUIDGenerator::INVALID;
	}
}

void Dystopia::Transform::AddChild(Transform* _pChild)
{
	_pChild->SetParent(this);
}

void Dystopia::Transform::OnChildAdd(Transform* _pChild)
{
	mChildren.Insert(_pChild);
}

void Dystopia::Transform::OnChildRemove(Transform*)
{
	// Do nothing
}

void Dystopia::Transform::RemoveChild(Transform* _pChild)
{
	auto const pChild = mChildren.Find(_pChild);

	if (pChild != mChildren.end())
	{
		auto child = *pChild;
		mChildren.FastRemove(pChild);
		OnChildRemove(child);
		child->OnParentRemove(this);
	}
}

void Dystopia::Transform::SetScale(const Math::Vec4& _vScale)
{
	mbChanged = true;
	mScale = _vScale;
}

void Dystopia::Transform::SetScale(const float _fScaleX, const float _fScaleY, const float _fScaleZ)
{
	SetScale(Math::Vec3D{ _fScaleX, _fScaleY, _fScaleZ });
}

void Dystopia::Transform::SetGlobalScale(const Math::Vec4& _vScale)
{
	mbChanged = true;

	if (mpParent)
	{
		mScale = Math::AffineInverse(mpParent->GetTransformMatrix()) * _vScale;
	}
	else
	{
		mScale = _vScale;
	}
}

void Dystopia::Transform::SetGlobalScale(const float _fScaleX, const float _fScaleY, const float _fScaleZ)
{
	SetGlobalScale(Math::Vec3D{ _fScaleX, _fScaleY, _fScaleZ });
}

void Dystopia::Transform::SetPosition(const Math::Point3D& _vPos)
{
	mbChanged = true;
	mPosition = _vPos;
}

void Dystopia::Transform::SetPosition(const float _x, const float _y, const float _z)
{
	SetPosition(Math::Pt3D{ _x, _y, _z, 1.f });
}

void Dystopia::Transform::SetGlobalPosition(const Math::Point3D& _vPos)
{
	mbChanged = true;

	if (mpParent)
	{
		mPosition = Math::AffineInverse(mpParent->GetTransformMatrix()) * _vPos;
	}
	else
	{
		mPosition = _vPos;
	}
}

void Dystopia::Transform::SetGlobalPosition(const float _x, const float _y, const float _z)
{
	SetGlobalPosition(Math::Pt3D{ _x, _y, _z });
}


void Dystopia::Transform::SetRotation(const Math::Angle _x, const Math::Angle _y, const Math::Angle _z)
{
	mRotation = mRotation.FromEuler(_x, _y, _z);
}

void Dystopia::Transform::SetRotation(const Math::Quaternion& _q)
{
	mRotation = _q;
}

Math::Quaternion Dystopia::Transform::GetGlobalRotation(void) const
{
	if (mpParent)
		return mpParent->GetGlobalRotation() * GetRotation();
	
	return GetRotation();
}


Math::Vec3D Dystopia::Transform::GetGlobalScale(void) const
{
	if (mpParent)
		return mpParent->GetTransformMatrix() * GetScale();

	return GetScale();
}

Math::Pt3D Dystopia::Transform::GetGlobalPosition(void) const
{
	if (mpParent)
		return mpParent->GetTransformMatrix() * GetPosition();

	return GetPosition();
}

Math::Quaternion Dystopia::Transform::GetRotation(void) const
{
	return mRotation;
}

Math::Vec3D Dystopia::Transform::GetScale(void) const
{
	return mScale;
}

Math::Pt3D Dystopia::Transform::GetPosition(void) const
{
	return mPosition;
}

const Math::Mat4& Dystopia::Transform::GetLocalTransformMatrix(void)
{
	if (mbChanged)
	{
		mbChanged = false;
		mRotation.Normalise();
		mMatrix = Math::Translate(mPosition) * mRotation.Matrix() * Math::Scale(mScale);
	}

	return mMatrix;
}

Math::Mat4 Dystopia::Transform::GetTransformMatrix(void)
{
	if (mpParent)
	{
		// Local on the right as we apply local transforms first!
		return mpParent->GetTransformMatrix() * GetLocalTransformMatrix();
	}

	return GetLocalTransformMatrix();
}
	
AutoArray<Dystopia::Transform*>& Dystopia::Transform::GetAllChild(void)
{
	return mChildren;
}

const AutoArray<Dystopia::Transform*>& Dystopia::Transform::GetAllChild(void) const
{
	return mChildren;
}

Dystopia::Transform* Dystopia::Transform::GetParent(void)
{
	return mpParent;
}

Dystopia::Transform* Dystopia::Transform::Duplicate(void) const
{
	return nullptr;
}

void Dystopia::Transform::Serialise(TextSerialiser& _out) const
{
	_out.InsertStartBlock("Transform");
	auto r = GetGlobalRotation();
	auto s = GetGlobalScale();
	auto p = GetGlobalPosition();
	_out << static_cast<float>(s.x);
	_out << static_cast<float>(s.y);
	_out << static_cast<float>(s.z);
	_out << static_cast<float>(p[0]);
	_out << static_cast<float>(p[1]);
	_out << static_cast<float>(p[2]);
	_out << static_cast<float>(r[0]);
	_out << static_cast<float>(r[1]);
	_out << static_cast<float>(r[2]);
	_out << static_cast<float>(r[3]);
	_out << mnParentID;
	//_out << mfOpacity;
	_out.InsertEndBlock("Transform");
}

void Dystopia::Transform::Unserialise(TextSerialiser& _in)
{
	_in.ConsumeStartBlock();
	_in >> mScale[0];
	_in >> mScale[1];
	_in >> mScale[2];
	_in >> mPosition[0];
	_in >> mPosition[1];
	_in >> mPosition[2];
	_in >> mRotation[0];
	_in >> mRotation[1];
	_in >> mRotation[2];
	_in >> mRotation[3];
	_in >> mnParentID;
	//_in >> mfOpacity;

	mPosition.w = 1.f;
	_in.ConsumeEndBlock();
}

void Dystopia::Transform::EditorUI(void) noexcept
{
#if EDITOR

	auto cmd = ::Editor::EditorMain::GetInstance()->GetSystem<::Editor::EditorCommands>();

	/*EGUI::PushLeftAlign(80);

	switch (EGUI::Display::SliderFloat("Opacity", &mfOpacity,0.01f))
	{
	case EGUI::eEND_DRAG: 
	case EGUI::eENTER: 
	case EGUI::eDEACTIVATED:
		cmd->EndRec(&Transform::mfOpacity, this);
		break;
	case EGUI::eSTART_DRAG:
		cmd->StartRec(&Transform::mfOpacity, this);
		break;
	case EGUI::eDRAGGING:
		SetOpacity(mfOpacity);
		break;
	default: ;
	};
	EGUI::PopLeftAlign();*/

	EGUI::PushID(0);
	EGUI::PushLeftAlign(20);
	EGUI::Display::LabelWrapped("Position");
	auto arrResult = EGUI::Display::VectorFields("", &mPosition, 0.01f, -FLT_MAX, FLT_MAX, 60.f);

	for (auto &e : arrResult)
	{
		switch (e)
		{
		case EGUI::eDragStatus::eDRAGGING:
			mbChanged = true;
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
			cmd->StartRec<Transform, const Math::Pt3D&>(&Transform::SetPosition, mPosition);
			break;
		case EGUI::eDragStatus::eEND_DRAG:
		case EGUI::eDragStatus::eENTER:
		case EGUI::eDragStatus::eDEACTIVATED:
			cmd->EndRec<Transform, const Math::Pt3D&>(GetOwnerID(), &Transform::SetPosition, mPosition);
			break;
		}
	}
	EGUI::PopID();

	EGUI::PushID(1);
	EGUI::Display::LabelWrapped("Scale");
	arrResult = EGUI::Display::VectorFields("", &mScale, 0.01f, -FLT_MAX, FLT_MAX, 60.f);
	for (auto &e : arrResult)
	{
		switch (e)
		{
		case EGUI::eDragStatus::eSTART_DRAG:
			cmd->StartRec<Transform, const Math::Vec4&>(&Transform::SetScale, mScale);
			break;
		case EGUI::eDragStatus::eDRAGGING:
			mbChanged = true;
			break;
		case EGUI::eDragStatus::eEND_DRAG:
		case EGUI::eDragStatus::eENTER:
		case EGUI::eDragStatus::eDEACTIVATED:
			cmd->EndRec<Transform, const Math::Vec4&>(GetOwnerID(), &Transform::SetScale, mScale);
			break;
		}
	}
	EGUI::PopID();

	EGUI::PushID(2);
	EGUI::Display::LabelWrapped("Rotation");
	static bool convertEuler = true;
	static Math::Vector4 eulerAngle;
	arrResult = EGUI::Display::VectorFields("", &eulerAngle, 0.01f, -FLT_MAX, FLT_MAX, 60.f);
	for (auto& e : arrResult)
	{
		switch (e)
		{
		case EGUI::eDragStatus::eSTART_DRAG:
			convertEuler = false;
			cmd->StartRec<Transform, const Math::Quaternion&>(&Transform::SetRotation, mRotation);
			break;
		case EGUI::eDragStatus::eDRAGGING:
			mbChanged = true;
			mRotation = mRotation.FromEuler(
				Math::Degrees(eulerAngle[0]),
				Math::Degrees(eulerAngle[1]),
				Math::Degrees(eulerAngle[2])
			);
			break;
		case EGUI::eDragStatus::eENTER:
		case EGUI::eDragStatus::eDEACTIVATED:
		case EGUI::eDragStatus::eEND_DRAG:
			convertEuler = true;
			cmd->EndRec<Transform, const Math::Quaternion&>(GetOwnerID(), &Transform::SetRotation, mRotation);
			break;
		}
	}

	if(convertEuler)
		eulerAngle = mRotation.ToEuler();
	EGUI::PopID();

	EGUI::PopLeftAlign();
#endif 
}

Dystopia::Transform& Dystopia::Transform::operator=(const Dystopia::Transform& _rhs)
{
	mbChanged	= _rhs.mbChanged;
	mMatrix		= _rhs.mMatrix;
	mScale		= _rhs.mScale;
	mPosition	= _rhs.mPosition;
	mRotation	= _rhs.mRotation;
	mnParentID	= _rhs.mnParentID;
	return *this;
}

uint64_t Dystopia::Transform::GetParentID(void) const
{
	return mnParentID;
}

void Dystopia::Transform::SetParentID(uint64_t _id)
{
	mnParentID = _id;
}

//void Dystopia::Transform::SetOpacity(float _perc)
//{
//	if (_perc <= 0.01f) _perc = 0.01f;
//	else if (_perc > 1.0f) _perc = 1.0f;
//
//	mfOpacity = _perc;
//
//	if (const auto spriteRend = GetOwner()->GetComponent<SpriteRenderer>())
//	{
//		spriteRend->SetAlphaPerc(mfOpacity);
//	}
//
//	if (const auto textRend = GetOwner()->GetComponent<TextRenderer>())
//	{
//		textRend->SetAlphaPerc(mfOpacity);
//	}
//
//	for (const auto childT : mChildren)
//	{
//		if (const auto spriteRend = childT->GetOwner()->GetComponent<SpriteRenderer>())
//		{
//			spriteRend->SetAlphaPerc(mfOpacity);
//		}
//
//		if (const auto textRend = childT->GetOwner()->GetComponent<TextRenderer>())
//		{
//			textRend->SetAlphaPerc(mfOpacity);
//		}
//	}
//}
//
//float Dystopia::Transform::GetOpacity() const
//{
//	return mfOpacity;
//}

bool Dystopia::Transform::IsDescendant(Transform* _toBeParent)
{
	for (auto& child : mChildren)
	{
		if (child == _toBeParent)
			return true;

		if (child->IsDescendant(_toBeParent))
			return true;
	}
	return false;
}
