/* HEADER *********************************************************************************/
/*!
\file	BubbleAffector.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Particle/BubbleAffector.h"
#include "System/Particle/Emitter.h"

#include "Math/MathLib.h"

#include <random>

#include "Object/GameObject.h"
#include "Component/Transform.h"

#if EDITOR
#include "Editor/EGUI.h"
#endif

#define HARD_CODED_LIMIT 5000

Dystopia::BubbleAffector::BubbleAffector(void)
	: ParticleAffector{ &BubbleAffector::AffectorUpdate }
{
	memset(reinterpret_cast<void*>(data), 0, 16);
	memset(reinterpret_cast<void*>(reserved), 0, 4);
}

Dystopia::BubbleAffector::~BubbleAffector(void)
{
}

void Dystopia::BubbleAffector::ToggleSolid(void)
{
	reserved[0] ^= (1 << 0);
}

bool Dystopia::BubbleAffector::IsSolid(void) const
{
	return reserved[0] & (1 << 0);
}

void Dystopia::BubbleAffector::SetRadius(unsigned short _s)
{
	*reinterpret_cast<unsigned short*>(data) = _s;
}

unsigned short Dystopia::BubbleAffector::GetRadius(void) const
{
	return *reinterpret_cast<const unsigned short*>(data);
}

void Dystopia::BubbleAffector::SetOffsetX(float _f)
{
	*reinterpret_cast<float*>(data + 2) = _f;
}

void Dystopia::BubbleAffector::SetOffsetY(float _f)
{
	*reinterpret_cast<float*>(data + 6) = _f;
}

float Dystopia::BubbleAffector::GetOffsetX(void) const
{
	return *reinterpret_cast<const float*>(data + 2);
}

float Dystopia::BubbleAffector::GetOffsetY(void) const
{
	return *reinterpret_cast<const float*>(data + 6);
}

void Dystopia::BubbleAffector::AffectorUpdate(Emitter& _emitter, float _dt)
{
	auto& allVelocities			= _emitter.GetVelocity();
	auto& allPositions			= _emitter.GetPosition();
	Math::Point3D emitterPos	= _emitter.GetOwnerTransform().GetGlobalPosition();
	Math::Vector3 sourcePos		= Math::Vector3{ emitterPos.x + GetOffsetX(),
												 emitterPos.y + GetOffsetY(), 
												 emitterPos.z };

	if (allVelocities.size() >= HARD_CODED_LIMIT)
		return;

	if (IsSolid())
	{
		for (int i = 0; i < allVelocities.size(); ++i)
		{
			Math::Vector3 projectedPos = allPositions[i] + (allVelocities[i] * _dt);
			Math::Vector3 vec = projectedPos - sourcePos;
			Math::Vec2 intersection{ 0,0 };
			float len = vec.Magnitude();
			if (len <= GetRadius())
			{
				float dx, dy, A, B, C, det, t;
				float point1_x = allPositions[i].x;
				float point1_y = allPositions[i].y;
				Math::Vec2 hitvec{ 0,0 };
				dx = vec.x;
				dy = vec.y;

				A = dx * dx + dy * dy;
				B = 2 * (dx * (point1_x - sourcePos.x) + dy * (point1_y - sourcePos.y));
				C = (point1_x - sourcePos.x) * (point1_x - sourcePos.x) + (point1_y - sourcePos.y) * (point1_y - sourcePos.y) - GetRadius() *  GetRadius();

				det = B * B - 4 * A * C;
				if ((A <= Math::epsilon) || (det < 0))
					continue;
				else if (det == 0)
				{
					t = -B / (2 * A);
					intersection.x = point1_x + t * dx;
					intersection.y = point1_y + t * dy;
					//hitvec = Math::Vector2{ intersection - Math::Vector2{point1_x, point1_y} };
				}
				else
				{
					Math::Vec2 intersection2{ 0,0 };

					t = static_cast<float>((-B + std::sqrt(det)) / (2 * A));
					intersection.x = point1_x + t * dx;
					intersection.y = point1_y + t * dy;

					t = static_cast<float>((-B - std::sqrt(det)) / (2 * A));
					intersection2.x = point1_x + t * dx;
					intersection2.y = point1_y + t * dy;

					float dist1 = Math::Vector2{ intersection - Math::Vector2{point1_x, point1_y} }.Magnitude();
					float dist2 = Math::Vector2{ intersection2 - Math::Vector2{point1_x, point1_y} }.Magnitude();
					intersection = dist1  < dist2 ? intersection : intersection2;
					//hitvec = Math::Vector2{ intersection - Math::Vector2{point1_x, point1_y} };
				}

				hitvec.x = allVelocities[i].x;
				hitvec.y = allVelocities[i].y;
				Math::Vector2 normal{ (intersection.x - sourcePos.x) / GetRadius(), (intersection.y - sourcePos.y) / GetRadius() };
				auto r = hitvec - (2 * hitvec.Dot(normal)) * normal;
				
				allPositions[i].x = intersection.x;
				allPositions[i].y = intersection.y;
				allVelocities[i].x = r.x;
				allVelocities[i].y = r.y;
			}
		}
	}
	else
	{
	}
}

Math::Vector2 Dystopia::BubbleAffector::GetIntersection(const Math::Vec2& , const Math::Vec2& ) const
{
	return Math::Vec2{};
}

const char * Dystopia::BubbleAffector::EditorDisplayLabel(void) const
{
	return "Bubble Affector";
}

void Dystopia::BubbleAffector::EditorUI(void)
{
#if EDITOR
	EGUI::PushLeftAlign(80.f);
	EGUI::PushID(37);

	EGUI::Display::Label("Hard-coded limit. If reach limit, skip affector module. [%d]", HARD_CODED_LIMIT);

	bool mode = IsSolid();
	if (EGUI::Display::CheckBox("Solid", &mode))
		ToggleSolid();

	float f = GetOffsetX();
	if (EGUI::Display::DragFloat("Offset X", &f, 0.1f, -FLT_MAX, FLT_MAX))
		SetOffsetX(f);

	f = GetOffsetY();
	if (EGUI::Display::DragFloat("Offset Y", &f, 0.1f, -FLT_MAX, FLT_MAX))
		SetOffsetY(f);

	int rad = static_cast<int>(GetRadius());
	if (EGUI::Display::DragInt("Radius", &rad, 1.f, 0, USHRT_MAX))
		SetRadius(static_cast<unsigned short>(rad));

	EGUI::PopID();
	EGUI::PopLeftAlign();
#endif
}





