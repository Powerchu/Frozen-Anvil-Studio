/* HEADER ********************************************************************************* /
/*!
\file	AccelerationAffector.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

		bytes and bits:

		data[0,1,2,3] for delta
		data[4,5] for x min
		data[6,7] for y min
		data[8,9] for x max
		data[10,11] for y max

		reserved[0]
			- 1 << 1 randome velocity

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Particle/AccelerationAffector.h"

#include "System/Particle/Emitter.h"

#include "Math/MathLib.h"

#include <random>

#if EDITOR
#include "Editor/EGUI.h"
#endif 

Dystopia::InitialAccelerationAffector::InitialAccelerationAffector(void)
	: ParticleAffector{ &InitialAccelerationAffector::AffectorSpawn }
{
	memset(data, 0, 16);
	memset(reserved, 0, 4);
}

Dystopia::InitialAccelerationAffector::~InitialAccelerationAffector(void)
{
}

void Dystopia::InitialAccelerationAffector::SetMinX(float _val)
{
	*reinterpret_cast<float*>(data) = _val;
}

void Dystopia::InitialAccelerationAffector::SetMinY(float _val)
{
	*reinterpret_cast<float*>(data + 4) = _val;
}

void Dystopia::InitialAccelerationAffector::SetMaxX(float _val)
{
	*reinterpret_cast<float*>(data + 8) = _val;
}

void Dystopia::InitialAccelerationAffector::SetMaxY(float _val)
{
	*reinterpret_cast<float*>(data + 12) = _val;
}

float Dystopia::InitialAccelerationAffector::GetMinX(void) const
{
	return *reinterpret_cast<const float*>(data);
}

float Dystopia::InitialAccelerationAffector::GetMinY(void) const
{
	return *reinterpret_cast<const float*>(data + 4);
}

float Dystopia::InitialAccelerationAffector::GetMaxX(void) const
{
	return *reinterpret_cast<const float*>(data + 8);
}

float Dystopia::InitialAccelerationAffector::GetMaxY(void) const
{
	return *reinterpret_cast<const float*>(data + 12);
}

void Dystopia::InitialAccelerationAffector::EnableRandom(bool _enabled)
{
	if (_enabled)
		reserved[0] |= (1 << 1);
	else
		reserved[0] &= ~(1 << 1);
}

void Dystopia::InitialAccelerationAffector::AffectorSpawn(Emitter& _emitter, float)
{
	std::random_device rDev;
	std::mt19937 gen{ rDev() };

	float x = GetMinX();
	float y = GetMinY();
	if (reserved[0] & (1 << 1))
	{
		std::uniform_real_distribution<float> distrX{ static_cast<float>(GetMinX()), static_cast<float>(GetMaxX()) };
		std::uniform_real_distribution<float> distrY{ static_cast<float>(GetMinY()), static_cast<float>(GetMaxY()) };

		x = distrX(gen);
		y = distrY(gen);
	}

	_emitter.GetSpawnDefaults().mAccel.x = x;
	_emitter.GetSpawnDefaults().mAccel.y = y;
}

const char * Dystopia::InitialAccelerationAffector::EditorDisplayLabel(void) const
{
	return "Initially Acceleration";
}

void Dystopia::InitialAccelerationAffector::EditorUI(void)
{
#if EDITOR
	EGUI::PushLeftAlign(100.f);
	EGUI::PushID(59);

	bool rand = reserved[0] & (1 << 1);
	if (EGUI::Display::CheckBox("Random Accel", &rand))
		EnableRandom(rand);

	Math::Vec2 min{ GetMinX(), GetMinY() };
	auto arrResult = EGUI::Display::VectorFields("Min Accel", &min, 0.1f, -FLT_MAX, FLT_MAX);
	for (auto &e : arrResult)
	{
		switch (e)
		{
		case EGUI::eDragStatus::eDRAGGING:
		case EGUI::eDragStatus::eSTART_DRAG:
			SetMinX(min.x);
			SetMaxX(Math::Max<float>(GetMaxX(), min.x));
			SetMinY(min.y);
			SetMaxY(Math::Max<float>(GetMaxY(), min.y));
			break;
		default:
			break;
		}
	}

	if (rand)
	{
		Math::Vec2 max{ GetMaxX(), GetMaxY() };
		auto arrResult2 = EGUI::Display::VectorFields("Max Accel", &max, 0.1f, -FLT_MAX, FLT_MAX);
		for (auto &e : arrResult2)
		{
			switch (e)
			{
			case EGUI::eDragStatus::eDRAGGING:
			case EGUI::eDragStatus::eSTART_DRAG:
				SetMaxX(max.x);
				SetMinX(Math::Min<float>(GetMinX(), max.x));
				SetMaxY(max.y);
				SetMinY(Math::Min<float>(GetMinY(), max.y));
				break;
			default:
				break;
			}
		}
	}

	EGUI::PopID();
	EGUI::PopLeftAlign();
#endif 
}





