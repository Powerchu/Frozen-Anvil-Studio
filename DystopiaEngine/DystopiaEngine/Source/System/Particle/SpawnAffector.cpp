/* HEADER *********************************************************************************/
/*!
\file	SpawnAffector.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
		Spawn affector bytes and bits:

		data[0,1,2,3] used for float dt
		data[4,5] used for spawn rate
		data[6,7] used for burst count
		data[8,9] used for burst low  // only if enabled random burst
		data[10, 11, 12, 13] used for delay

		reserved[0] used for flags
			- 1 << 0 continuous
			- 1 << 1 burst enabled
			- 1 << 2 use burst low to range to count
			- 1 << 3 burst done 

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Particle/SpawnAffector.h"
#include "Component/Emitter.h"

#include "Math/MathLib.h"

#include <random>

#if EDITOR
#include "Editor/EGUI.h"
#endif 

Dystopia::SpawnAffector::SpawnAffector(void)
	: ParticleAffector{ &SpawnAffector::AffectorUpdate }
{
	memset(reinterpret_cast<void*>(data), 0, 16);
	memset(reinterpret_cast<void*>(reserved), 0, 4);
}

Dystopia::SpawnAffector::~SpawnAffector(void)
{
}

/* Continuous spawn */
void Dystopia::SpawnAffector::SetSpawnRate(unsigned short _particlePerSecond)
{
	*reinterpret_cast<unsigned short*>(data + 4) = _particlePerSecond;
}

unsigned short Dystopia::SpawnAffector::GetSpawnRate(void) const
{
	return *reinterpret_cast<const unsigned short*>(data + 4);
}

void Dystopia::SpawnAffector::EnableContinuous(bool _enabled)
{
	if (_enabled)
		reserved[0] |= 1 << 0;
	else
		reserved[0] &= ~(1 << 0);
}

/* Burst spawn */
void Dystopia::SpawnAffector::SetBurstCount(unsigned short _particlePerBurst)
{
	*reinterpret_cast<unsigned short*>(data + 6) = _particlePerBurst;
}

void Dystopia::SpawnAffector::SetBurstLow(unsigned short _particlePerBurstLow)
{
	*reinterpret_cast<unsigned short*>(data + 8) = _particlePerBurstLow;
}

unsigned short Dystopia::SpawnAffector::GetBurstCount(void) const
{
	return *reinterpret_cast<const unsigned short*>(data + 6);
}

unsigned short Dystopia::SpawnAffector::GetBurstLow(void) const
{
	return *reinterpret_cast<const unsigned short*>(data + 8);
}

void Dystopia::SpawnAffector::EnableBurst(bool _enabled)
{
	if (_enabled)
		reserved[0] |= 1 << 1;
	else
		reserved[0] &= ~(1 << 1);
}

void Dystopia::SpawnAffector::EnableRandomBurst(bool _enabled)
{
	if (_enabled)
		reserved[0] |= 1 << 2;
	else
		reserved[0] &= ~(1 << 2);
}

void Dystopia::SpawnAffector::SetInitialDelay(float _delay)
{
	*reinterpret_cast<float*>(data + 10) = _delay;
}

float Dystopia::SpawnAffector::GetInitialDelay(void) const
{
	return *reinterpret_cast<const float*>(data + 10);
}

void Dystopia::SpawnAffector::AffectorUpdate(Dystopia::Emitter& _emitter, float _dt)
{
	float& dtCounter = *reinterpret_cast<float*>(data);
	float& delay = *reinterpret_cast<float*>(data + 10);

	if (!_emitter.IsAlive())
	{
		dtCounter = 0.f;		// reset continuous
		reserved[0] &= ~(1 << 3); //reset burst
		return;
	}

	if (delay)
	{
		delay = Math::Clamp(delay - _dt, 0.f, delay - _dt);
		return;
	}

	dtCounter += _dt;
	// do continuous spawn
	if (reserved[0] & (1 << 0))
	{
		unsigned short spawnPerSecond = GetSpawnRate();
		if (spawnPerSecond)
		{
			float intervals = (1.f / static_cast<float>(spawnPerSecond));
			while (dtCounter >= intervals)
			{
				_emitter.SpawnParticle();
				dtCounter -= intervals;
			}
		}
	}

	// stop if already bursted
	if (reserved[0] & (1 << 3))
		return;

	// do burst spawn
	if (reserved[0] & (1 << 1))
	{
		// do random burst
		if (reserved[0] & (1 << 2))
		{
			unsigned short min = GetBurstLow();
			unsigned short max = GetBurstCount();

			std::random_device rd; 
			std::mt19937 gen{ rd() };
			std::uniform_int_distribution<unsigned short> distr{ min, max };

			unsigned short rand = distr(gen);
			while (rand)
			{
				_emitter.SpawnParticle();
				rand--;
			}
		}
		// do fixed burst
		else
		{
			unsigned short count = *reinterpret_cast<unsigned short*>(data + 6);
			while (count)
			{
				_emitter.SpawnParticle();
				count--;
			}
		}
		reserved[0] |= (1 << 3);
	}

	//_emitter.SpawnParticle();
}

const char * Dystopia::SpawnAffector::EditorDisplayLabel(void) const
{
	return "Spawn";
}

void Dystopia::SpawnAffector::EditorUI(void)
{
#if EDITOR
	EGUI::PushLeftAlign(100.f);

	float mfSpawnDelay = *reinterpret_cast<float*>(data + 10);
	if (EGUI::Display::DragFloat("Spawn Delay", &mfSpawnDelay, 0.1f, 0.f, FLT_MAX))
		SetInitialDelay(mfSpawnDelay);

	bool mbContinuous = reserved[0] & (1 << 0);
	if (EGUI::Display::CheckBox("Continuous", &mbContinuous))
		EnableContinuous(mbContinuous);

	int out = static_cast<int>(GetSpawnRate());
	if (EGUI::Display::DragInt("Spawn Rate", &out, 1.f, 0, 10000))
		SetSpawnRate(static_cast<unsigned short>(out));

	bool mbBurst = reserved[0] & (1 << 1);
	if (EGUI::Display::CheckBox("Burst", &mbBurst))
		EnableBurst(mbBurst);

	bool mbBurstRandom = reserved[0] & (1 << 2);
	if (EGUI::Display::CheckBox("Burst Random", &mbBurstRandom))
		EnableRandomBurst(mbBurstRandom);

	out = static_cast<int>(GetBurstCount());
	if (EGUI::Display::DragInt("Burst High", &out, 1.f, GetBurstLow(), 10000))
		SetBurstCount(static_cast<unsigned short>(out));

	out = static_cast<int>(GetBurstLow());
	if (EGUI::Display::DragInt("Burst Low", &out, 1.f, 0, static_cast<int>(GetBurstCount())))
		SetBurstLow(static_cast<unsigned short>(out));

	EGUI::PopLeftAlign();
#endif 
}












