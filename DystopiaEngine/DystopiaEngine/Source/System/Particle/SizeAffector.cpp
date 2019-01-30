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

void Dystopia::InitialSizeAffector::SetInitialSize(float _val)
{
	*reinterpret_cast<float*>(data + 4) = _val;
}

void Dystopia::InitialSizeAffector::AffectorSpawn(Emitter& _emitter, float)
{
	_emitter.GetSpawnDefaults().mfLifeDur = *reinterpret_cast<float*>(data + 4);
}

const char * Dystopia::InitialSizeAffector::EditorDisplayLabel(void) const
{
	return "Initial Size";
}

void Dystopia::InitialSizeAffector::EditorUI(void)
{
#if EDITOR
	EGUI::PushLeftAlign(80.f);
	EGUI::PushID(53);
	
	float out = *reinterpret_cast<float*>(data + 4);
	if (EGUI::Display::DragFloat("Size", &out, 0.1f, -FLT_MAX, FLT_MAX))
		SetInitialSize(out);
	
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

void Dystopia::SizeOverLifeAffector::AffectorUpdate(Emitter& _emitter, float _dt)
{

}

const char * Dystopia::SizeOverLifeAffector::EditorDisplayLabel(void) const
{
	return "Size Over Life";
}

void Dystopia::SizeOverLifeAffector::EditorUI(void)
{
#if EDITOR
	EGUI::PushLeftAlign(80.f);

	EGUI::PopLeftAlign();
#endif 
}


