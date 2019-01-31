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

Dystopia::InitialColorAffector::InitialColorAffector(void)
	: ParticleAffector{ &InitialColorAffector::AffectorSpawn }
{
	memset(data, 0, 16);
	memset(reserved, 0, 4);
}

Dystopia::InitialColorAffector::~InitialColorAffector(void)
{}

void Dystopia::InitialColorAffector::AffectorSpawn(Emitter&, float)
{

}

const char * Dystopia::InitialColorAffector::EditorDisplayLabel(void) const
{
	return "Initial Color";
}

void Dystopia::InitialColorAffector::EditorUI(void)
{
#if EDITOR



#endif 
}

/********************************************************************************************************************/

Dystopia::ColorOverLifeAffector::ColorOverLifeAffector(void)
	: ParticleAffector{ &ColorOverLifeAffector::AffectorUpdate }
{
	memset(data, 0, 16);
	memset(reserved, 0, 4);
}

Dystopia::ColorOverLifeAffector::~ColorOverLifeAffector(void)
{}

void Dystopia::ColorOverLifeAffector::AffectorUpdate(Emitter&, float)
{

}

const char * Dystopia::ColorOverLifeAffector::EditorDisplayLabel(void) const
{
	return "Color Over Life";
}

void Dystopia::ColorOverLifeAffector::EditorUI(void)
{
#if EDITOR



#endif 
}




