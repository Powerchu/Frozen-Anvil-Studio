/* HEADER *********************************************************************************/
/*!
\file	TextRenderer.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Text Renderer.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Component/TextRenderer.h"
#include "System/Driver/Driver.h"

#include "System/Graphics/Mesh.h"
#include "System/Graphics/MeshSystem.h"
#include "System/Graphics/TextureAtlas.h"
#include "System/Graphics/FontSystem.h"


Dystopia::TextRenderer::TextRenderer(void) noexcept
	: mstrText{ "" }, mpBaseMesh{ nullptr }
{
}

void Dystopia::TextRenderer::SetText(const char* _str)
{
	SetText(std::string{ _str });
}

void Dystopia::TextRenderer::SetText(const std::string& _str)
{
	mstrText = _str;
}

void Dystopia::TextRenderer::SetFont(const char* _strPath)
{
	SetFont(std::string{ _strPath });
}

void Dystopia::TextRenderer::SetFont(const std::string& _strPath)
{
	EngineCore::GetInstance()->Get<FontSystem>()->LoadFont(_strPath);
}

void Dystopia::TextRenderer::RegenMesh(void)
{
}


