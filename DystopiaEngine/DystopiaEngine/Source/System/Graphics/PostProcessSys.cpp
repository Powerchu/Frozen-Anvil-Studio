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
#include "System/Graphics/MeshSystem.h"
#include "System/Graphics/Framebuffer.h"
#include "Lib/GraphicsLib.h"
#include "System/Driver/Driver.h"			// EngineCore

// Texture Includes
#include "System/Graphics/Texture.h"
#include "System/Graphics/Texture2D.h"

// Shader Includes
#include "System/Graphics/Shader.h"
#include "System/Graphics/ShaderSystem.h"

#include "Math/Vectors.h"
#include "Math/Matrix4.h"

#include "IO/TextSerialiser.h"
#include "Utility/DebugAssert.h"			// DEBUG_ASSERT


#include <GL/glew.h>

bool Dystopia::PostProcessSystem::Init(void)
{
	first  = & CORE::Get<GraphicsSystem>()->GetView(3);
	second = & CORE::Get<GraphicsSystem>()->GetView(4);
	third  = & CORE::Get<GraphicsSystem>()->GetView(5);
	game   = & CORE::Get<GraphicsSystem>()->GetGameView();

	return first && second && third && game;
}

void Dystopia::PostProcessSystem::PostInit(void)
{
	auto pShaderSys = CORE::Get<ShaderSystem>();

	auto s = pShaderSys->GetShader("Blur Pass");

	Math::Matrix4 MVP{
		2.f, .0f, .0f, .0f,
		.0f, 2.f, .0f, .0f,
		.0f, .0f, .0f, .0f,
		.0f, .0f, .0f, 1.f
	};

	s->UploadUniform("MVP", MVP);

	s = pShaderSys->GetShader("Blur Merge");
	MVP[5] = -MVP[5];
	s->UploadUniform("MVP", MVP);
}

void Dystopia::PostProcessSystem::Update(float)
{
}

void Dystopia::PostProcessSystem::ApplyBlur(void)
{
	auto pShaderSys = CORE::Get<ShaderSystem>();
	auto blurShader = pShaderSys->GetShader("Blur Pass");
	auto Mesh = CORE::Get<MeshSystem>()->GetMesh("Quad");

	// Blur pass
	auto const w = first->GetWidth();
	auto const h = first->GetHeight();
	auto const w_step = 1.f / w;
	auto const h_step = 1.f / h;

	blurShader->Bind();

	glViewport(0, 0, first->GetWidth(), first->GetHeight());

	blurShader->UploadUniform("vStep", Math::Vec2{ 1.f / game->GetWidth(), 0 });
	first->Bind();
	game->AsTexture(2)->Bind();
	Mesh->DrawMesh(4);

	blurShader->UploadUniform("vStep", Math::Vec2{ 0, h_step });
	second->Bind();
	first->AsTexture()->Bind();
	Mesh->DrawMesh(4);

	for (int n = 0; n < 8; ++n)
	{
		blurShader->UploadUniform("vStep", Math::Vec2{ w_step, 0 });
		first->Bind();
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		second->AsTexture()->Bind();
		Mesh->DrawMesh(4);
	
		blurShader->UploadUniform("vStep", Math::Vec2{ 0, h_step });
		second->Bind();
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		first->AsTexture()->Bind();
		Mesh->DrawMesh(4);
	}

	pShaderSys->GetShader("Blur Merge")->Bind();
	third->Bind();
	glViewport(0, 0, third->GetWidth(), third->GetHeight());

	game  ->AsTexture()->Bind(0);
	second->AsTexture()->Bind(1);
	Mesh->DrawMesh(4);
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

