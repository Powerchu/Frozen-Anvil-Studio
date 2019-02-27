/* HEADER *********************************************************************************/
/*!
\file	AttractionAffector.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Particle/AttractionAffector.h"
#include "System/Particle/Emitter.h"

#include "Math/MathLib.h"

#include <random>

#include "Object/GameObject.h"
#include "Component/Transform.h"

#if EDITOR
#include "Editor/EGUI.h"
#endif


Dystopia::PointAffector::PointAffector(void)
	: ParticleAffector{ &PointAffector::AffectorUpdate }
{
	memset(reinterpret_cast<void*>(data), 0, 16);
	memset(reinterpret_cast<void*>(reserved), 0, 4);
}

Dystopia::PointAffector::~PointAffector(void)
{
}

void Dystopia::PointAffector::SetStrength(unsigned short _n)
{
	*reinterpret_cast<unsigned short*>(data) = _n;
}

void Dystopia::PointAffector::SetRange(unsigned short _n)
{
	*reinterpret_cast<unsigned short*>(data + 2) = _n;
}

unsigned short Dystopia::PointAffector::GetStrength(void) const
{
	return *reinterpret_cast<const unsigned short*>(data);
}

unsigned short Dystopia::PointAffector::GetRange(void) const
{
	return *reinterpret_cast<const unsigned short*>(data + 2);
}

void Dystopia::PointAffector::TogglePoint(bool _enabled)
{
	if (_enabled)
	{
		reserved[0] |= (1 << 0);
		reserved[0] &= ~(1 << 2);
	}
	else
		reserved[0] &= ~(1 << 0);
}

bool Dystopia::PointAffector::IsPoint(void) const
{
	return reserved[0] & (1 << 0);
}

void Dystopia::PointAffector::SetOffsetX(float _f)
{
	*reinterpret_cast<float*>(data + 4) = _f;
}

void Dystopia::PointAffector::SetOffsetY(float _f)
{
	*reinterpret_cast<float*>(data + 8) = _f;
}

float Dystopia::PointAffector::GetOffsetX(void) const
{
	return *reinterpret_cast<const float*>(data + 4);
}

float Dystopia::PointAffector::GetOffsetY(void) const
{
	return *reinterpret_cast<const float*>(data + 8);
}

void Dystopia::PointAffector::ToggleRepulse(bool _enabled)
{
	if (_enabled)
	{
		reserved[0] |= (1 << 2);
		reserved[0] &= ~(1 << 0);
	}
	else
		reserved[0] &= ~(1 << 2);
}

bool Dystopia::PointAffector::IsRepulse(void) const
{
	return reserved[0] & (1 << 2);
}

//void Dystopia::PointAffector::ToggleHelix(bool _enabled)
//{
//	if (_enabled)
//	{
//		reserved[0] |= (1 << 1);
//		reserved[0] &= ~(1 << 0);
//	}
//	else
//		reserved[0] &= ~(1 << 1);
//}

//bool Dystopia::PointAffector::IsHelix(void) const
//{
//	return reserved[0] & (1 << 1);
//}

void Dystopia::PointAffector::AffectorUpdate(Emitter& _emitter, float _dt)
{
	//if (IsHelix())
	//	HelixAttract(_emitter, _dt);
	if (IsPoint())
		PointAttract(_emitter, _dt);
	if (IsRepulse())
		PointRepulse(_emitter, _dt);
}

//void Dystopia::AttractionAffector::HelixAttract(Emitter& _emitter, float _dt)
//{
//	float& accDt = *reinterpret_cast<float*>(data + 4);
//
//	accDt += _dt;
//	if (accDt > Math::pi2)
//		accDt -= Math::pi2;
//
//	Math::Point3D emitterPos = _emitter.GetOwner()->GetComponent<Transform>()->GetGlobalPosition();
//	auto& allPos = _emitter.GetPosition();
//	auto& allAccel = _emitter.GetAcceleration();
//
//	float x = GetRange() * cos(accDt);
//	float y = accDt;
//	float z = GetRange() * sin(accDt);// sin(accDt);
//
//	Math::Vec4 target{ emitterPos.x + x,
//					   emitterPos.y + y,
//					   emitterPos.z + z,
//					   emitterPos.w };
//
//	for (unsigned i = 0; i < allAccel.size(); ++i)
//	{
//		Math::Vec3 range{ emitterPos.x - allPos[i].x,
//						  emitterPos.y - allPos[i].y,
//						  emitterPos.z - allPos[i].z };
//
//		if (range.MagnitudeSqr() < (GetRange()*GetRange()))
//		{
//			Math::Vec3 direction{ target.x - allPos[i].x,
//								  target.y - allPos[i].y,
//								  target.z - allPos[i].z };
//			if (Math::Dot(allAccel[i], direction) <= 0.f)
//				allAccel[i] = allAccel[i] + (direction.Normalise() * static_cast<float>(GetStrength()));
//		}
//	}
//}

void Dystopia::PointAffector::PointAttract(Emitter& _emitter, float)
{
	Math::Point3D emitterPos = _emitter.GetOwnerTransform().GetGlobalPosition();
	auto& allPos = _emitter.GetPosition();
	auto& allAccel = _emitter.GetAcceleration();
	for (unsigned i = 0; i < allAccel.size(); ++i)
	{
		Math::Vec3 direction{ GetOffsetX() + emitterPos.x - allPos[i].x, 
							  GetOffsetY() + emitterPos.y - allPos[i].y, 
							  emitterPos.z - allPos[i].z };
	
		if (direction.MagnitudeSqr() > Math::epsilon && direction.MagnitudeSqr() < (GetRange()*GetRange()))
			allAccel[i] = direction.Normalise() * static_cast<float>(GetStrength());
	}
}

void Dystopia::PointAffector::PointRepulse(Emitter& _emitter, float)
{
	Math::Point3D emitterPos = _emitter.GetOwnerTransform().GetGlobalPosition();
	auto& allPos = _emitter.GetPosition();
	auto& allAccel = _emitter.GetAcceleration();
	for (unsigned i = 0; i < allAccel.size(); ++i)
	{
		Math::Vec3 direction{ allPos[i].x - (GetOffsetX() + emitterPos.x),
							  allPos[i].y - (GetOffsetY() + emitterPos.y),
							  allPos[i].z - emitterPos.z };
	
		if (direction.MagnitudeSqr() > Math::epsilon && direction.MagnitudeSqr() < (GetRange()*GetRange()))
			allAccel[i] = direction.Normalise() * static_cast<float>(GetStrength());
	}
}

const char * Dystopia::PointAffector::EditorDisplayLabel(void) const
{
	return "Point Attractor";
}

void Dystopia::PointAffector::EditorUI(void)
{
#if EDITOR
	EGUI::PushLeftAlign(80.f);
	EGUI::PushID(64);

	bool mode = IsPoint();
	if (EGUI::Display::CheckBox("Attract", &mode))
		TogglePoint(mode);

	mode = IsRepulse();
	if (EGUI::Display::CheckBox("Repulse", &mode))
		ToggleRepulse(mode);

	//mode = IsHelix();
	//if (EGUI::Display::CheckBox("Helix", &mode))
	//	ToggleHelix(mode);

	int s = static_cast<int>(GetStrength());
	if (EGUI::Display::DragInt("Strength", &s, 1.f, 0, 65534))
		SetStrength(static_cast<short>(s));

	s = static_cast<int>(GetRange());
	if (EGUI::Display::DragInt("Range", &s, 1.f, 0, 65534))
		SetRange(static_cast<short>(s));

	float f = GetOffsetX();
	if (EGUI::Display::DragFloat("Offset X", &f, 1.f, -FLT_MAX, FLT_MAX))
		SetOffsetX(f);

	f = GetOffsetY();
	if (EGUI::Display::DragFloat("Offset Y", &f, 1.f, -FLT_MAX, FLT_MAX))
		SetOffsetY(f);

	EGUI::PopID();
	EGUI::PopLeftAlign();
#endif
}



















