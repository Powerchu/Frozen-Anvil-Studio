/* HEADER ********************************************************************************* /
/*!
\file	SizeAffector.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Particle/SizeAffector.h"

#include "Component/Emitter.h"

#include "Math/MathLib.h"

#include <random>

#if EDITOR
#include "Editor/EGUI.h"
#endif 

Dystopia::InitialSizeAffector::InitialSizeAffector(void)
	: ParticleAffector{ &InitialSizeAffector::AffectorSpawn }
{
	memset(data, 0, 16);
	memset(reserved, 0, 4);
}

Dystopia::InitialSizeAffector::~InitialSizeAffector(void)
{}

void Dystopia::InitialSizeAffector::SetInitialSizeMin(float _val)
{
	*reinterpret_cast<float*>(data + 4) = _val;
}

void Dystopia::InitialSizeAffector::SetInitialSizeMax(float _val)
{
	*reinterpret_cast<float*>(data + 8) = _val;
}

float Dystopia::InitialSizeAffector::GetInitialSizeMin(void) const
{
	return *reinterpret_cast<const float*>(data + 4);
}

float Dystopia::InitialSizeAffector::GetInitialSizeMax(void) const
{
	return *reinterpret_cast<const float*>(data + 8);
}

void Dystopia::InitialSizeAffector::EnableRandomInitial(bool _enable)
{
	if (_enable)
		reserved[0] |= (1 << 1);
	else
		reserved[0] &= ~(1 << 1);
}

void Dystopia::InitialSizeAffector::AffectorSpawn(Emitter& _emitter, float)
{
	if (reserved[0] & (1 << 1))
	{
		std::random_device rDev;
		std::mt19937 gen{ rDev() };
		std::uniform_real_distribution<float> distr{ GetInitialSizeMin(), GetInitialSizeMax() };
		_emitter.GetSpawnDefaults().mfSize = distr(gen);
	}
	else
		_emitter.GetSpawnDefaults().mfSize = GetInitialSizeMax();
}

const char * Dystopia::InitialSizeAffector::EditorDisplayLabel(void) const
{
	return "Initial Size";
}

void Dystopia::InitialSizeAffector::EditorUI(void)
{
#if EDITOR
	EGUI::PushLeftAlign(100.f);
	EGUI::PushID(53);
	
	float out = GetInitialSizeMin();
	if (EGUI::Display::DragFloat("Size Min", &out, 0.1f, -FLT_MAX, GetInitialSizeMax()))
		SetInitialSizeMin(out);

	out = GetInitialSizeMax();
	if (EGUI::Display::DragFloat("Size Max", &out, 0.1f, GetInitialSizeMin(), FLT_MAX))
		SetInitialSizeMax(out);

	bool rand = reserved[0] & (1 << 1);
	if (EGUI::Display::CheckBox("Random Size", &rand))
		EnableRandomInitial(rand);

	EGUI::PopID();
	EGUI::PopLeftAlign();
#endif 
}

/********************************************************************************************************************/

Dystopia::SizeOverLifeAffector::SizeOverLifeAffector(void)
	: ParticleAffector{ &SizeOverLifeAffector::AffectorUpdate }
{
	memset(data, 0, 16);
	memset(reserved, 0, 4);
}

Dystopia::SizeOverLifeAffector::~SizeOverLifeAffector(void)
{}

void Dystopia::SizeOverLifeAffector::SetTargetSize(float _val)
{
	*reinterpret_cast<float*>(data + 4) = _val;
}

void Dystopia::SizeOverLifeAffector::SetTargetDuration(float _val)
{
	*reinterpret_cast<float*>(data + 8) = _val;
}

void Dystopia::SizeOverLifeAffector::SetTargetSizeAfter(float _val)
{
	*reinterpret_cast<float*>(data + 12) = _val;
}

void Dystopia::SizeOverLifeAffector::EnableBounceback(bool _enabled)
{
	if (_enabled)
		reserved[0] |= (1 << 1);
	else
		reserved[0] &= ~(1 << 1);
}

void Dystopia::SizeOverLifeAffector::AffectorUpdate(Emitter&, float)
{
	// float change = *reinterpret_cast<float*>(data + 4) - _emitter.GetSpawnDefaults().mfSize;
	// unsigned int idx = _emitter.GetInitialLifetime().size();
	// while (idx)
	// {
	// 	--idx;
	// 
	// 	float initial = _emitter.GetInitialLifetime()[idx];
	// 	float current = _emitter.GetLifetime()[idx];
	// 	float alphaV = (initial - current) / initial;
	// }
}

const char * Dystopia::SizeOverLifeAffector::EditorDisplayLabel(void) const
{
	return "Size Over Life";
}

void Dystopia::SizeOverLifeAffector::EditorUI(void)
{
#if EDITOR
	EGUI::PushLeftAlign(100.f);

	EGUI::PopLeftAlign();
#endif 
}


