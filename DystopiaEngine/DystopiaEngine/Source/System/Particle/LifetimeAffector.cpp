/* HEADER ********************************************************************************* /
/*!
\file	LifetimeAffector.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
		location affector bytes and bits:

		data[0,1,2,3] for delta counting
		data[4,5,6,7] for min lifetime 
		data[8,9,10,11] for max lifetime 

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Particle/LifetimeAffector.h"

#include "Component/Emitter.h"

#include "Math/MathLib.h"

#include <random>

#if EDITOR
#include "Editor/EGUI.h"
#endif 

Dystopia::LifetimeAffector::LifetimeAffector(void)
	: ParticleAffector{ &LifetimeAffector::AffectorSpawn }
{
	memset(data, 0, 16);
	memset(reserved, 0, 4);
	*reinterpret_cast<float*>(data + 4) = 1.f;
	*reinterpret_cast<float*>(data + 8) = 1.f;
}

Dystopia::LifetimeAffector::~LifetimeAffector(void)
{}

void Dystopia::LifetimeAffector::SetMin(float _val)
{
	*reinterpret_cast<float*>(data + 4) = _val;
}

void Dystopia::LifetimeAffector::SetMax(float _val)
{
	*reinterpret_cast<float*>(data + 8) = _val;
}

void Dystopia::LifetimeAffector::AffectorSpawn(Emitter& _emitter, float)
{
	std::random_device rDev;
	std::mt19937 gen{ rDev() };
	std::uniform_real_distribution<float> distr{ *reinterpret_cast<float*>(data + 4), *reinterpret_cast<float*>(data + 8) };
	_emitter.GetSpawnDefaults().mfLifeDur = distr(gen);
}

const char * Dystopia::LifetimeAffector::EditorDisplayLabel(void) const
{
	return "Lifetime";
}

void Dystopia::LifetimeAffector::EditorUI(void)
{
#if EDITOR
	EGUI::PushLeftAlign(80.f);

	float out = *reinterpret_cast<float*>(data + 4);
	if (EGUI::Display::DragFloat("Min", &out, 0.1f, -FLT_MAX, FLT_MAX))
		SetMin(out);

	out = *reinterpret_cast<float*>(data + 8);
	if (EGUI::Display::DragFloat("Max", &out, 0.1f, -FLT_MAX, FLT_MAX))
		SetMax(out);

	EGUI::PopLeftAlign();
#endif 
}



