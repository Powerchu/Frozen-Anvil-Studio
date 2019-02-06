/* HEADER ********************************************************************************* /
/*!
\file	VelocityAffector.cpp
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
#include "System/Particle/VelocityAffector.h"

#include "Component/Emitter.h"

#include "Math/MathLib.h"

#include <random>

#if EDITOR
#include "Editor/EGUI.h"
#endif 

Dystopia::InitialVelocityAffector::InitialVelocityAffector(void)
	: ParticleAffector{ &InitialVelocityAffector::AffectorSpawn }
{
	memset(data, 0, 16);
	memset(reserved, 0, 4);
}

Dystopia::InitialVelocityAffector::~InitialVelocityAffector(void)
{
}

void Dystopia::InitialVelocityAffector::SetMinX(float _val)
{
	*reinterpret_cast<float*>(data) = _val;
}

void Dystopia::InitialVelocityAffector::SetMinY(float _val)
{
	*reinterpret_cast<float*>(data + 4) = _val;
}

void Dystopia::InitialVelocityAffector::SetMaxX(float _val)
{
	*reinterpret_cast<float*>(data + 8) = _val;
}

void Dystopia::InitialVelocityAffector::SetMaxY(float _val)
{
	*reinterpret_cast<float*>(data + 12) = _val;
}

float Dystopia::InitialVelocityAffector::GetMinX(void) const
{
	return *reinterpret_cast<const float*>(data);
}

float Dystopia::InitialVelocityAffector::GetMinY(void) const
{
	return *reinterpret_cast<const float*>(data + 4);
}

float Dystopia::InitialVelocityAffector::GetMaxX(void) const
{
	return *reinterpret_cast<const float*>(data + 8);
}

float Dystopia::InitialVelocityAffector::GetMaxY(void) const
{
	return *reinterpret_cast<const float*>(data + 12);
}

void Dystopia::InitialVelocityAffector::EnableRandomVel(bool _enabled)
{
	if (_enabled)
		reserved[0] |= (1 << 1);
	else
		reserved[0] &= ~(1 << 1);
}

void Dystopia::InitialVelocityAffector::AffectorSpawn(Emitter& _emitter, float)
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

	_emitter.GetSpawnDefaults().mVelocity.x = x;
	_emitter.GetSpawnDefaults().mVelocity.y = y;
}

const char * Dystopia::InitialVelocityAffector::EditorDisplayLabel(void) const
{
	return "Initially Velocity";
}

void Dystopia::InitialVelocityAffector::EditorUI(void)
{
#if EDITOR
	EGUI::PushLeftAlign(100.f);
	EGUI::PushID(54);

	bool rand = reserved[0] & (1 << 1);
	if (EGUI::Display::CheckBox("Random Vel", &rand))
		EnableRandomVel(rand);

	Math::Vec2 min{ GetMinX(), GetMinY() };
	auto arrResult = EGUI::Display::VectorFields("Min Vel", &min, 0.1f, -FLT_MAX, FLT_MAX);
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
		auto arrResult2 = EGUI::Display::VectorFields("Max Vel", &max, 0.1f, -FLT_MAX, FLT_MAX);
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

/********************************************************************************************************************/

Dystopia::VelocityOverLifeAffector::VelocityOverLifeAffector(void)
	: ParticleAffector{ &VelocityOverLifeAffector::AffectorUpdate }
{
	memset(data, 0, 16);
	memset(reserved, 0, 4);
}

Dystopia::VelocityOverLifeAffector::~VelocityOverLifeAffector(void)
{
}

void Dystopia::VelocityOverLifeAffector::AffectorUpdate(Emitter&, float)
{
}

const char * Dystopia::VelocityOverLifeAffector::EditorDisplayLabel(void) const
{
	return "Velocity Over Life";
}

void Dystopia::VelocityOverLifeAffector::EditorUI(void)
{
#if EDITOR
	EGUI::PushLeftAlign(100.f);



	EGUI::PopLeftAlign();
#endif 
}






