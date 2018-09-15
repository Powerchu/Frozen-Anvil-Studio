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
#include "Component\Transform.h"
#include "Object\GameObject.h"
#include "Math\Matrix4.h"
#include "Math\Vector4.h"
#include "Editor\EGUI.h"

Dystopia::Transform::Transform(GameObject* _pOwner) noexcept
	: mRotation{ .0f, .0f, .0f }, mScale{ 1.f, 1.f, 1.f }, mPosition{ .0f, .0f, .0f }, 
	mMatrix{}, mbChanged{ true }, mpParent{ nullptr }, Component { _pOwner }
{

}

Dystopia::Transform::Transform(const Transform& _oOther) :
	mRotation{ _oOther.mRotation }, mScale{ _oOther.mScale }, mPosition{ _oOther.mPosition },
	mMatrix{}, mbChanged{ true }, mpParent{ nullptr }, Component{ _oOther }
{

}


void Dystopia::Transform::SetParent(Transform* _pParent)
{
	if (mpParent)
		RemoveParent();

	mpParent = _pParent;
    if (mpParent)
    {
		// Convert our data to the parent's local coordinates
        Math::Mat4 InvTrans = Math::AffineInverse(mpParent->GetTransformMatrix());

        mScale		= InvTrans * mScale;
        mPosition	= InvTrans * mPosition;
        mRotation  -= mpParent->GetRotation();
        mbChanged	= true;
    }
}

void Dystopia::Transform::OnParentRemove(Transform* _pParent)
{
	// Convert our data to world coordinates
	Math::Mat4 InvTrans = _pParent->GetTransformMatrix();

	mScale		= InvTrans * mScale;
	mPosition	= InvTrans * mPosition;
	mRotation  += _pParent->GetRotation();

	mbChanged = true;
}

void Dystopia::Transform::RemoveParent(void)
{
	if (mpParent)
	{
		Transform* parent = mpParent;
		mpParent = nullptr;

		OnParentRemove(parent);
		parent->OnChildRemove(this);
	}
}

void Dystopia::Transform::OnChildRemove(Transform*)
{
	// Do nothing?
}

void Dystopia::Transform::RemoveChild(Transform* _pChild)
{
	auto pChild = mChildren.Find(_pChild);

	if (pChild != mChildren.end())
	{
		auto child = *pChild;
		mChildren.FastRemove(pChild);
		OnChildRemove(child);
		child->OnParentRemove(this);
	}
}

/*
void Dystopia::Transform::SetRotation(const float _fRadians)
{
	mbChanged = true;
	mRotation = _fRadians;
}

void Dystopia::Transform::SetRotationDeg(const float _fDegrees)
{
	mbChanged = true;
	SetRotation(Math::DegreeToRadian(_fDegrees));
}
*/
void Dystopia::Transform::SetScale(const Math::Vec4& _vScale)
{
	mbChanged = true;
	mScale = _vScale;
}

void Dystopia::Transform::SetScale(const float _fScaleX, const float _fScaleY, const float _fScaleZ)
{
	SetScale(Math::Vec3D{ _fScaleX, _fScaleY, _fScaleZ });
}

void Dystopia::Transform::SetPosition(const Math::Point3D& _vPos)
{
	mbChanged = true;
	mPosition = _vPos;
}

void Dystopia::Transform::SetPosition(const float _x, const float _y, const float _z)
{
	SetPosition(Math::Pt3D{ _x, _y, _z });
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


Math::Quaternion Dystopia::Transform::GetGlobalRotation(void) const
{
	if (mpParent)
		return mpParent->GetGlobalRotation() + GetRotation();
	
	return GetRotation();
}
/*
Math::Vec4 Dystopia::Transform::GetGlobalRotationDeg(void) const
{
	if (mpParent)
		return mpParent->GetGlobalRotationDeg() + GetRotationDeg();
	
	return GetRotationDeg();
}
*/
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
//		mMatrix = Math::RotTrans(mRotation, mPosition) * Math::Matrix2D::Scale(mScale);
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

Dystopia::Transform* Dystopia::Transform::Duplicate(void) const
{
	return nullptr;
}

void Dystopia::Transform::Serialise(TextSerialiser&) const
{

}

void Dystopia::Transform::Unserialise(TextSerialiser&)
{

}

void Dystopia::Transform::EditorUI(void) noexcept
{
#if EDITOR
	if (EGUI::Display::VectorFields("Scale", GetOwner()->GetID(), &mScale, 0.5f, -FLT_MAX, FLT_MAX))
	{
		ConsolePrint("Changed");
	}

#endif 
}

