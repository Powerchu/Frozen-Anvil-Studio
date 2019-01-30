/* HEADER ********************************************************************************* /
/*!
\file	ColorAffector.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Particle/ColorAffector.h"

#include "Component/Emitter.h"

#include "Math/MathLib.h"

#include <random>

#if EDITOR
#include "Editor/EGUI.h"
#endif 

Dystopia::ColorAffector::ColorAffector(void)
	: ParticleAffector{ &ColorAffector::AffectorSpawn }
{}

Dystopia::ColorAffector::~ColorAffector(void)
{}

void Dystopia::ColorAffector::AffectorSpawn(Emitter&, float)
{}

const char * Dystopia::ColorAffector::EditorDisplayLabel(void) const
{
	return "Color";
}

void Dystopia::ColorAffector::EditorUI(void)
{
}





