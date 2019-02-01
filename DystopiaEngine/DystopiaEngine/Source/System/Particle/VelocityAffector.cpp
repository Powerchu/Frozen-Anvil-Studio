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
	std::uniform_real_distribution<float> distrX{ *reinterpret_cast<float*>(data), *reinterpret_cast<float*>(data + 8) };
	std::uniform_real_distribution<float> distrY{ *reinterpret_cast<float*>(data + 4), *reinterpret_cast<float*>(data + 12) };

	float x = reserved[0] & (1 << 1) ? distrX(gen) : *reinterpret_cast<float*>(data + 8) ;
	float y = reserved[0] & (1 << 1) ? distrY(gen) : *reinterpret_cast<float*>(data + 12) ;

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

	Math::Vec2 min{ *reinterpret_cast<float*>(data) , *reinterpret_cast<float*>(data + 4) };
	auto arrResult = EGUI::Display::VectorFields("Min Vel", &min, 0.1f, -FLT_MAX, FLT_MAX);
	for (auto &e : arrResult)
	{
		switch (e)
		{
		case EGUI::eDragStatus::eDRAGGING:
		case EGUI::eDragStatus::eSTART_DRAG:
			SetMinX(min.x);
			SetMinY(min.y);
			break;
		default:
			break;
		}
	}

	Math::Vec2 max{ *reinterpret_cast<float*>(data + 8) , *reinterpret_cast<float*>(data + 12) };
	auto arrResult2 = EGUI::Display::VectorFields("Max Vel", &max, 0.1f, -FLT_MAX, FLT_MAX);
	for (auto &e : arrResult2)
	{
		switch (e)
		{
		case EGUI::eDragStatus::eDRAGGING:
		case EGUI::eDragStatus::eSTART_DRAG:
			SetMaxX(min.x);
			SetMaxY(min.y);
			break;
		default:
			break;
		}
	}

	bool rand = reserved[0] & (1 << 1);
	if (EGUI::Display::CheckBox("Random Vel", &rand))
		EnableRandomVel(rand);

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






