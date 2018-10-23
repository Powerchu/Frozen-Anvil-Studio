/* HEADER *********************************************************************************/
/*!
\file	TextureAtlas.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Texture containing multiple textures

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Graphics/TextureAtlas.h"
#include "System/Graphics/Texture.h"


void Dystopia::TextureAtlas::Bind(void) const noexcept
{
	mpTexture->Bind();
}

void Dystopia::TextureAtlas::Unbind(void) const noexcept
{
	mpTexture->Unbind();
}

std::string Dystopia::TextureAtlas::GetName(void) const
{
	return mpTexture->GetName();
}

const std::string& Dystopia::TextureAtlas::GetPath(void) const noexcept
{
	return mpTexture->GetPath();
}


