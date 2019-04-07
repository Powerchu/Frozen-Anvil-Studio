/* HEADER *********************************************************************************/
/*!
\file	PostProcessSys.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Post Processing System

All Content Copyright © 2019 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Graphics/PostProcessSys.h"	// File header
#include "System/Graphics/GraphicsSystem.h"
#include "System/Graphics/Framebuffer.h"
#include "Lib/GraphicsLib.h"
#include "System/Driver/Driver.h"			// EngineCore

// Texture Includes
#include "System/Graphics/Texture.h"
#include "System/Graphics/Texture2D.h"

// Shader Includes
#include "System/Graphics/Shader.h"
#include "System/Graphics/ShaderSystem.h"

#include "IO/TextSerialiser.h"
#include "Utility/DebugAssert.h"			// DEBUG_ASSERT


bool Dystopia::PostProcessSystem::Init(void)
{
	return true;
}

void Dystopia::PostProcessSystem::Update(float)
{
	// Blur pass
}

void Dystopia::PostProcessSystem::Shutdown(void)
{
}

void Dystopia::PostProcessSystem::LoadDefaults(void)
{
	mSettings = ~Ut::Constant<decltype(mSettings), 0>{};
}

void Dystopia::PostProcessSystem::LoadSettings(DysSerialiser_t& in)
{
	in >> mSettings;
	mSettings = ~mSettings;
}

void Dystopia::PostProcessSystem::SaveSettings(DysSerialiser_t& out)
{
	out << mSettings;
}

