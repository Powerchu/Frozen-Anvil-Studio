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
#include "System/Graphics/VertexDefs.h"
#include "System/Graphics/TextureAtlas.h"
#include "System/Graphics/CharSpace.h"
#include "System/Graphics/FontSystem.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/GraphicsSystem.h"

#if EDITOR
#include "Editor/EGUI.h"
#endif

#include <GL/glew.h>


Dystopia::TextRenderer::TextRenderer(void) noexcept
	: Renderer{}, mText {}, mnBaseMesh{ Ut::Constant<decltype(mnBaseMesh), -1>::value }, mColor{ 1.f, 1.f, 1.f }
{
}

void Dystopia::TextRenderer::Awake(void)
{
	mpShader = EngineCore::GetInstance()->Get<GraphicsSystem>()->shaderlist["Font Shader"];
	mnBaseMesh = EngineCore::GetInstance()->Get<MeshSystem>()->GenerateRaw();
	mpMesh = EngineCore::GetInstance()->Get<MeshSystem>()->CreateMesh(
		EngineCore::GetInstance()->Get<MeshSystem>()->GetRaw(mnBaseMesh), 0
	);

	SetFont("Resource/Font/Times New Roman.ttf");
}

void Dystopia::TextRenderer::Draw(void) const noexcept
{
	GetShader()->UploadUniform("vColor", mColor);
	Renderer::Draw();
}


void Dystopia::TextRenderer::SetText(const char* _str)
{
	SetText(std::string{ _str });
}

void Dystopia::TextRenderer::SetText(const std::string& _str)
{
	mText.clear();

	for (auto& e : _str)
		mText.Insert(e - ' ');

	RegenMesh();
}

void Dystopia::TextRenderer::SetFont(const char* _strPath)
{
	SetFont(std::string{ _strPath });
}

void Dystopia::TextRenderer::SetFont(const std::string& _strPath)
{
	mpAtlas = EngineCore::GetInstance()->Get<FontSystem>()->LoadFont(_strPath, mSpaces);
	mpTexture = mpAtlas->GetInternal();
}

void Dystopia::TextRenderer::RegenMesh(void)
{
	AutoArray<Vertex> verts;
	AutoArray<UV> uvs;
	AutoArray<short> indices;

	short index = 0;

	float x = .0f;
	constexpr float scale = 1.f / 100.f;
	for (auto& e : mText)
	{
		auto& ch = mSpaces[e];

		float y = ch.mnBearingY * scale;
		float dy = y - ch.mnHeight * scale;
		float w = ch.mnWidth * scale;
		
		verts.EmplaceBack(x, y, .0f);
		verts.EmplaceBack(.0f, .0f, 1.f);
		verts.EmplaceBack(x, dy, .0f);
		verts.EmplaceBack(.0f, .0f, 1.f);
		verts.EmplaceBack(x + w, dy, .0f);
		verts.EmplaceBack(.0f, .0f, 1.f);
		verts.EmplaceBack(x + w, y, .0f);
		verts.EmplaceBack(.0f, .0f, 1.f);

		uvs.EmplaceBack(ch.u, ch.v);
		uvs.EmplaceBack(ch.u, ch.t);
		uvs.EmplaceBack(ch.s, ch.t);
		uvs.EmplaceBack(ch.s, ch.v);

		indices.Insert(index);
		indices.Insert(index + 1);
		indices.Insert(index + 2);
		indices.Insert(index);
		indices.Insert(index + 2);
		indices.Insert(index + 3);

		index += 4;
		x += ch.mnAdvance * scale;
	}

	EngineCore::GetInstance()->Get<MeshSystem>()->GetRaw(mnBaseMesh)->BuildMesh(verts, uvs, indices);

#   if defined(_DEBUG) | defined(DEBUG)
	if (auto err = glGetError())
		__debugbreak();
#   endif 
	mpMesh->SetIndices(static_cast<unsigned>(indices.size()), 0);

#   if defined(_DEBUG) | defined(DEBUG)
	if (auto err = glGetError())
		__debugbreak();
#   endif 
}

void Dystopia::TextRenderer::EditorUI(void) noexcept
{
	static char strText[256];
	if (EGUI::Display::TextField("Text", strText, 256))
	{
		mText.clear();
		char* ptr = strText;

		while(*ptr)
			mText.EmplaceBack(static_cast<unsigned char>(*ptr++ - ' '));

		RegenMesh();
	}
}


