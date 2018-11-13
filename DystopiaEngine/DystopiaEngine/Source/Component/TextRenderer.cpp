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

#include "Object/GameObject.h"

#if EDITOR
#include "Editor/EGUI.h"
#include "Editor/Payloads.h"
#endif

#include <GL/glew.h>


Dystopia::TextRenderer::TextRenderer(void) noexcept
	: Renderer{}, mText {}, mnBaseMesh{ Ut::Constant<decltype(mnBaseMesh), -1>::value }, mColor{ 1.f, 1.f, 1.f },
	mpAtlas{ nullptr }
{
}

void Dystopia::TextRenderer::Awake(void)
{
	mpShader = EngineCore::GetInstance()->Get<GraphicsSystem>()->shaderlist["Font Shader"];

	if (!mpMesh)
	{
		mnBaseMesh = EngineCore::GetInstance()->Get<MeshSystem>()->GenerateRaw();
		mpMesh = EngineCore::GetInstance()->Get<MeshSystem>()->CreateMesh(
			EngineCore::GetInstance()->Get<MeshSystem>()->GetRaw(mnBaseMesh), 0
		);
	}

	if (!mpAtlas)
		SetFont("Times New Roman.ttf");

	RegenMesh();
}

void Dystopia::TextRenderer::Draw(void) const noexcept
{
	GetShader()->UploadUniform("vColor", mColor);
	Renderer::Draw();
}


void Dystopia::TextRenderer::SetText(const char* _str)
{
	mText.clear();
	mText = _str;
	RegenMesh();
}

void Dystopia::TextRenderer::SetText(const std::string& _str)
{
	SetText(_str.c_str());
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

	auto& atlas = mpAtlas->GetAllSections();

	short index = 0;

	float x = .0f, y =.0f;
	constexpr float scale = 1.f / 100.f;
	for (auto& e : mText)
	{
		auto const n = e - ' ';
		auto& ch = mSpaces[n];

		y = ch.mnBearingY * scale;
		float const dx = ch.mnBearingX * scale;
		float const dy = y - ch.mnHeight * scale;
		float const w  = ch.mnWidth * scale + dx;
		
		verts.EmplaceBack(x + dx, y, .0f);
		verts.EmplaceBack(.0f, .0f, 1.f);
		verts.EmplaceBack(x + dx, dy, .0f);
		verts.EmplaceBack(.0f, .0f, 1.f);
		verts.EmplaceBack(x + w, dy, .0f);
		verts.EmplaceBack(.0f, .0f, 1.f);
		verts.EmplaceBack(x + w, y, .0f);
		verts.EmplaceBack(.0f, .0f, 1.f);

		uvs.EmplaceBack(atlas[n].uStart, atlas[n].vStart);
		uvs.EmplaceBack(atlas[n].uStart, atlas[n].vEnd);
		uvs.EmplaceBack(atlas[n].uEnd, atlas[n].vEnd);
		uvs.EmplaceBack(atlas[n].uEnd, atlas[n].vStart);

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


//Dystopia::TextRenderer* Dystopia::TextRenderer::Duplicate(void) const
//{
//	return nullptr;
//}

void Dystopia::TextRenderer::Serialise(TextSerialiser& _out) const
{
	Component::Serialise(_out);

	_out << mpAtlas->GetName();
	_out << mText;
}

void Dystopia::TextRenderer::Unserialise(TextSerialiser& _in)
{
	std::string font;
	Component::Unserialise(_in);

	_in >> font;
	_in >> mText;
}


void Dystopia::TextRenderer::EditorUI(void) noexcept
{
	char buf[256]{ };

	Ut::Copy(mText, &buf[0]);
	if (EGUI::Display::TextField("Text ", buf, 256, true, 225))
	{
		mText.clear();
		mText = buf;

		RegenMesh();
	}
	EGUI::Display::EmptyBox("Font ", 200, (mpAtlas) ? mpAtlas->GetName() : "-empty-", true);

	Dystopia::File* t = nullptr;
	if (auto i = EGUI::Display::StartPayloadReceiver<Dystopia::File>(EGUI::TTF))
	{
		t = i;
		EGUI::Display::EndPayloadReceiver();
	}

	if (t)
	{
		SetFont(t->mName);
	}

	EGUI::SameLine();
	if (EGUI::Display::IconCross("Clear", 8.f))
	{
		auto fOld = EGUI::GetCommandHND()->Make_FunctionModWrapper(static_cast<void(TextRenderer::*)(const char*)>(&TextRenderer::SetFont), mpAtlas->GetName().c_str());
		auto fNew = EGUI::GetCommandHND()->Make_FunctionModWrapper(static_cast<void(TextRenderer::*)(const char*)>(&TextRenderer::SetFont), "");
		EGUI::GetCommandHND()->InvokeCommand(GetOwner()->GetID(), fOld, fNew);
	}
}


