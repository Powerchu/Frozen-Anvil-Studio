/* HEADER ********************************************************************************* /
/*!
\file	VelocityAffector.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

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

Dystopia::VelocityAffector::VelocityAffector(void)
	: ParticleAffector{ &VelocityAffector::AffectorSpawn }
{}

Dystopia::VelocityAffector::~VelocityAffector(void)
{}

void Dystopia::VelocityAffector::AffectorSpawn(Emitter&, float)
{}

const char * Dystopia::VelocityAffector::EditorDisplayLabel(void) const
{
	return "Velocity";
}

void Dystopia::VelocityAffector::EditorUI(void)
{
}





