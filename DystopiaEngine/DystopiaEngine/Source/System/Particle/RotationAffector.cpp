/* HEADER *********************************************************************************/
/*!
\file	RotationAffector.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

		bytes and bits:
		data[0,1,2,3] used for min deg
		data[4,5,6,7] used for max deg

		reserved[0] used for flags
			- 1 << 0 used for random rotation

		data[0,1,2,3] used for min rate
		data[4,5,6,7] used for max rate

		reserved[0] used for flags
			- 1 << 0 used for random rate

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Particle/RotationAffector.h"

#include "System/Particle/Emitter.h"

#include "Math/MathLib.h"

#include <random>

#if EDITOR
#include "Editor/EGUI.h"
#endif 

Dystopia::InitialRotationAffector::InitialRotationAffector(void)
	: ParticleAffector{ &InitialRotationAffector::AffectorSpawn }
{
	memset(reinterpret_cast<void*>(data), 0, 16);
	memset(reinterpret_cast<void*>(reserved), 0, 4);
}

Dystopia::InitialRotationAffector::~InitialRotationAffector(void)
{
}

void Dystopia::InitialRotationAffector::SetMinDeg(float _d)
{
	*reinterpret_cast<float*>(data) = _d;
}

void Dystopia::InitialRotationAffector::SetMaxDeg(float _d)
{
	*reinterpret_cast<float*>(data + 4) = _d;
}

float Dystopia::InitialRotationAffector::GetMinDeg(void) const
{
	return *reinterpret_cast<const float*>(data);
}

float Dystopia::InitialRotationAffector::GetMaxDeg(void) const
{
	return *reinterpret_cast<const float*>(data + 4);
}

void Dystopia::InitialRotationAffector::ToggleRandom(void)
{
	reserved[0] ^= (1 << 0);
}

bool Dystopia::InitialRotationAffector::IsRandom(void) const
{
	return reserved[0] & (1 << 0);
}

void Dystopia::InitialRotationAffector::AffectorSpawn(Emitter& _e, float)
{
	auto& def = _e.GetSpawnDefaults();
	if (IsRandom())
	{
		std::random_device rDev;
		std::mt19937 gen{ rDev() };
		std::uniform_real_distribution<float> distr{ GetMinDeg(), GetMaxDeg() };
		def.mRotation = Math::Degrees{ distr(gen) }.Radians();
	}
	else
		def.mRotation = Math::Degrees{ GetMinDeg() }.Radians();
}

const char * Dystopia::InitialRotationAffector::EditorDisplayLabel(void) const
{
	return "Initial Rotation";
}

void Dystopia::InitialRotationAffector::EditorUI(void)
{
#if EDITOR
	EGUI::PushLeftAlign(100.f);
	EGUI::PushID(62);

	bool isRand = IsRandom();
	if (EGUI::Display::CheckBox("Random", &isRand))
		ToggleRandom();
	
	float out = static_cast<float>(GetMinDeg());
	if (EGUI::Display::DragFloat("Min Degrees", &out, 0.1f, -FLT_MAX, FLT_MAX))
		SetMinDeg(out);

	if (GetMinDeg() > GetMaxDeg())
		SetMaxDeg(GetMinDeg());

	if (IsRandom())
	{
		out = static_cast<float>(GetMaxDeg());
		if (EGUI::Display::DragFloat("Max Degrees", &out, 0.1f, GetMinDeg(), FLT_MAX))
			SetMaxDeg(out);
	}

	EGUI::PopID();
	EGUI::PopLeftAlign();
#endif 
}

/* ****************************************************************************************/

Dystopia::RotationRateAffector::RotationRateAffector(void)
	: ParticleAffector{ &RotationRateAffector::AffectorSpawn }
{
	memset(reinterpret_cast<void*>(data), 0, 16);
	memset(reinterpret_cast<void*>(reserved), 0, 4);
}

Dystopia::RotationRateAffector::~RotationRateAffector(void)
{
}

void Dystopia::RotationRateAffector::SetMinRate(float _f)
{
	*reinterpret_cast<float*>(data) = _f;
}

void Dystopia::RotationRateAffector::SetMaxRate(float _f)
{
	*reinterpret_cast<float*>(data + 4) = _f;
}

float Dystopia::RotationRateAffector::GetMinRate(void) const
{
	return *reinterpret_cast<const float*>(data);
}

float Dystopia::RotationRateAffector::GetMaxRate(void) const
{
	return *reinterpret_cast<const float*>(data + 4);
}

void Dystopia::RotationRateAffector::ToggleRandom(void)
{
	reserved[0] ^= (1 << 0);
}

bool Dystopia::RotationRateAffector::IsRandom(void) const
{
	return reserved[0] & (1 << 0);
}

void Dystopia::RotationRateAffector::AffectorSpawn(Emitter& _e, float)
{
	auto& def = _e.GetSpawnDefaults();
	if (IsRandom())
	{
		std::random_device rDev;
		std::mt19937 gen{ rDev() };
		std::uniform_real_distribution<float> distr{ GetMinRate(), GetMaxRate() };
		def.mRotVel = distr(gen);
	}
	else
		def.mRotVel = GetMinRate();
}

const char * Dystopia::RotationRateAffector::EditorDisplayLabel(void) const
{
	return "Rotation Rate";
}

void Dystopia::RotationRateAffector::EditorUI(void)
{
#if EDITOR
	EGUI::PushLeftAlign(100.f);
	EGUI::PushID(63);

	bool isRand = IsRandom();
	if (EGUI::Display::CheckBox("Random", &isRand))
		ToggleRandom();

	float out = static_cast<float>(GetMinRate());
	if (EGUI::Display::DragFloat("Min Rate", &out, 0.1f, -FLT_MAX, FLT_MAX))
		SetMinRate(out);

	if (GetMinRate() > GetMaxRate())
		SetMaxRate(GetMinRate());

	if (IsRandom())
	{
		out = static_cast<float>(GetMaxRate());
		if (EGUI::Display::DragFloat("Max Rate", &out, 0.1f, GetMinRate(), FLT_MAX))
			SetMaxRate(out);
	}

	EGUI::PopID();
	EGUI::PopLeftAlign();
#endif 
}












