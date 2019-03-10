/* HEADER *********************************************************************************/
/*!
\file	FloorAffector.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Particle/FloorAffector.h"

#include "System/Particle/Emitter.h"

#include "Math/MathLib.h"
#include "Object/GameObject.h"
#include "Component/Transform.h"

#include <random>

#if EDITOR
#include "Editor/EGUI.h"
#endif 
Dystopia::FloorAffector::FloorAffector()
	: ParticleAffector{ &FloorAffector::AffectorUpdate }
{
	memset(reinterpret_cast<void*>(data), 0, 16);
	memset(reinterpret_cast<void*>(reserved), 0, 4);
}

Dystopia::FloorAffector::~FloorAffector()
{
}

void Dystopia::FloorAffector::SetOffset(float _x, float _y)
{
	*reinterpret_cast<float*>(data) = _x;
	*reinterpret_cast<float*>(data + 4) = _y;
}

float Dystopia::FloorAffector::GetOffsetX(void) const
{
	return *reinterpret_cast<const float*>(data);
}

float Dystopia::FloorAffector::GetOffsetY(void) const
{
	return *reinterpret_cast<const float*>(data + 4);
}

void Dystopia::FloorAffector::SetSize(float _x, float _y)
{
	*reinterpret_cast<float*>(data + 8) = _x;
	*reinterpret_cast<float*>(data + 12) = _y;
}

float Dystopia::FloorAffector::GetSizeX(void) const
{
	return *reinterpret_cast<const float*>(data + 8);
}

float Dystopia::FloorAffector::GetSizeY(void) const
{
	return *reinterpret_cast<const float*>(data + 12);
}


void Dystopia::FloorAffector::AffectorUpdate(Dystopia::Emitter& _e, float) 
{
	auto& allAcceleration = _e.GetAcceleration();
	auto& allVelocities = _e.GetVelocity();
	auto& allPositions = _e.GetPosition();

	Math::Point3D emitterPos = _e.GetOwnerTransform().GetGlobalPosition();
	Math::Vector3 sourcePos = Math::Vector3{ emitterPos.x + GetOffsetX(),
											 emitterPos.y + GetOffsetY(),
											 emitterPos.z };
	float halfx = GetSizeX() / 2;
	float halfy = GetSizeY() / 2;

	Math::Vec2 rectMin{ sourcePos.x - halfx, sourcePos.y - halfy };
	Math::Vec2 rectMax{ sourcePos.x + halfx, sourcePos.y + halfy };

	for (int i = 0; i < allPositions.size(); ++i)
	{
		auto& pos = allPositions[i];
		if (pos.x < rectMin.x || pos.x > rectMax.x || pos.y < rectMin.y || pos.y > rectMax.y)
			continue;

		allVelocities[i] = Math::Vec3{ 0,0,0 };
		allAcceleration[i] = Math::Vec3{ 0,0,0 };
	}
}

const char * Dystopia::FloorAffector::EditorDisplayLabel(void) const
{
	return "Floor Affector";
}

void Dystopia::FloorAffector::EditorUI(void)
{
#if EDITOR
	EGUI::PushLeftAlign(100.f);
	EGUI::PushID(61);

	float x = GetOffsetX();
	float y = GetOffsetY();
	bool b1 = EGUI::Display::DragFloat("Offset X", &x, 0.1f, -FLT_MAX, FLT_MAX);
	bool b2 = EGUI::Display::DragFloat("Offset Y", &y, 0.1f, -FLT_MAX, FLT_MAX);
	if (b1 || b2)
		SetOffset(x, y);

	x = GetSizeX();
	y = GetSizeY();

	if (EGUI::Display::DragFloat("Size X", &x, 0.1f, -FLT_MAX, FLT_MAX) || EGUI::Display::DragFloat("Size Y", &y, 0.1f, -FLT_MAX, FLT_MAX))
		SetSize(x, y);

	EGUI::PopID();
	EGUI::PopLeftAlign();
#endif 
}




