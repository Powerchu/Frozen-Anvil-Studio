/* HEADER *********************************************************************************/
/*!
\file	TextureSystem.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Manages Textures

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Graphics/TextureSystem.h"

#include "System/Graphics/Texture.h"
#include "System/Graphics/Texture2D.h"
#include "System/Graphics/TextureAtlas.h"


Dystopia::TextureSystem::TextureSystem(void) noexcept
	: mTextures{}
{
}

Dystopia::TextureSystem::~TextureSystem(void) noexcept
{
	mTextures.clear();
}


