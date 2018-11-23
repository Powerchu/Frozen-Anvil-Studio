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
	: Renderer{}, mText {}, mColor{ 1.f, 1.f, 1.f, 1.f },
	mpData{ nullptr }, mnAnchorX{ 0 }, mnAnchorY{ 0 }
{
}

Dystopia::TextRenderer::TextRenderer(const TextRenderer& _rhs) noexcept
	: Renderer{ _rhs }, mText{ _rhs.mText }, mColor{ _rhs.mColor },
	mpData{ _rhs.mpData }, mnAnchorX{ _rhs.mnAnchorX }, mnAnchorY{ _rhs.mnAnchorY }
{
	mpTexture = _rhs.mpTexture;
}

void Dystopia::TextRenderer::Awake(void)
{
	mpShader = EngineCore::GetInstance()->Get<GraphicsSystem>()->shaderlist["Font Shader"];

	if (!mpMesh)
	{
		auto pMeshSys = EngineCore::GetInstance()->Get<MeshSystem>();
		auto pBaseMesh = pMeshSys->GetRaw(pMeshSys->GenerateRaw());
		mpMesh = EngineCore::GetInstance()->Get<MeshSystem>()->CreateMesh(
			pBaseMesh, 0
		);

		pBaseMesh->BuildEmpty<VertexBuffer, UVBuffer, IndexBuffer>();
	}

	if (!mpData)
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
	mpData = EngineCore::GetInstance()->Get<FontSystem>()->LoadFont(_strPath);
	mpTexture = mpData->mpAtlas->GetInternal();
}

void Dystopia::TextRenderer::RegenMesh(void)
{
	AutoArray<Gfx::Vertex> verts;
	AutoArray<Gfx::UV> uvs;
	AutoArray<short> indices;

	auto& atlas = mpData->mpAtlas->GetAllSections();

	short index = 0;

	float x = .0f, y =-1.0f, mx = .0f;
	constexpr float scale = 1.f / 100.f;
	for (auto b = mText.begin(), e = mText.end(); b != e; ++b)
	{
		if (*b == '\\')
		{
			if (++b == e)
				break;

			if (*b == 'n')
			{
				mx = Ut::Max(x, mx);
				x = .0f;
				y -= 1.1f;
				continue;
			}

			if (*b != '\\')
				continue;
		}

		auto const n = *b - ' ';
		auto& ch = mpData->mSpaces[n];

		float const dy = ch.mnBearingY * scale;
		float const dx = ch.mnBearingX * scale;
		float const w  = ch.mnWidth * scale + dx;
		float const h  = dy - ch.mnHeight * scale;
		
		verts.EmplaceBack(x + dx, y + dy, .0f);
		verts.EmplaceBack(x + dx, y + h, .0f);
		verts.EmplaceBack(x + w , y + h, .0f);
		verts.EmplaceBack(x + w , y + dy, .0f);

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

	float(*alignOp[])(float) {
		[](float) { return .0f; },
		[](float x) { return x * .5f; },
		[](float x) { return x; }
	};

	mx = Ut::Max(x, mx);
	x = alignOp[mnAnchorX](mx);
	y = alignOp[mnAnchorY](y);
	for (auto& e : verts)
	{
		e.x -= x;
		e.y -= y;
	}

	mpMesh->UpdateBuffer<VertexBuffer>(verts);
	mpMesh->UpdateBuffer<UVBuffer>(uvs);
	mpMesh->UpdateBuffer<IndexBuffer>(indices);
	mpMesh->SetIndices(static_cast<unsigned>(indices.size()), 0);

#   if defined(_DEBUG) | defined(DEBUG)
	if (auto err = glGetError())
		__debugbreak();
#   endif 
}


Dystopia::TextRenderer* Dystopia::TextRenderer::Duplicate(void) const
{
	return static_cast<ComponentDonor<TextRenderer>*>(
		EngineCore::GetInstance()->Get<GraphicsSystem>()
	)->RequestComponent(*this);
}

void Dystopia::TextRenderer::Serialise(TextSerialiser& _out) const
{
	Component::Serialise(_out);

	_out << mpData->mstrName;
	_out << mText;
	_out << mColor;
	_out << mnAnchorX;
	_out << mnAnchorY;
}

void Dystopia::TextRenderer::Unserialise(TextSerialiser& _in)
{
	std::string font;
	Component::Unserialise(_in);

	_in >> font;
	_in >> mText;
	_in >> mColor;
	_in >> mnAnchorX;
	_in >> mnAnchorY;
}


void Dystopia::TextRenderer::EditorUI(void) noexcept
{
	bool bRegenMesh = false;
	static char buf[512]{ };
	static std::string anchorX[3]{ "Left", "Center", "Right" };
	static std::string anchorY[3]{ "Top", "Center", "Bottom" };
	namespace UI = EGUI::Display;

	if (UI::DropDownSelection("Anchor X", mnAnchorX, anchorX, 100))
	{
		bRegenMesh = true;
	}
	if (UI::DropDownSelection("Anchor Y", mnAnchorY, anchorY, 100))
	{
		bRegenMesh = true;
	}

	*(Ut::Copy(mText, &buf[0])) = '\0';
	if (EGUI::Display::TextField("Text ", buf, 256, true, 225))
	{
		mText.clear();
		mText = buf;

		bRegenMesh = true;
	}
	EGUI::Display::EmptyBox("Font ", 200, (mpData) ? mpData->mstrName.c_str() : "-empty-", true);

	Dystopia::File* t = nullptr;
	if (auto i = EGUI::Display::StartPayloadReceiver<Dystopia::File>(EGUI::TTF))
	{
		t = i;
		EGUI::Display::EndPayloadReceiver();
	}

	if (t)
	{
		SetFont(t->mName.c_str());
	}

	EGUI::SameLine();
	if (EGUI::Display::IconCross("Clear", 8.f))
	{
		auto fOld = EGUI::GetCommandHND()->Make_FunctionModWrapper(static_cast<void(TextRenderer::*)(const char*)>(&TextRenderer::SetFont), mpData->mpAtlas->GetName().c_str());
		auto fNew = EGUI::GetCommandHND()->Make_FunctionModWrapper(static_cast<void(TextRenderer::*)(const char*)>(&TextRenderer::SetFont), "");
		EGUI::GetCommandHND()->InvokeCommand(GetOwner()->GetID(), fOld, fNew);
	}

	if (ImGui::ColorPicker4("Color", &mColor[0]))
	{

	}

	if (bRegenMesh)
		RegenMesh();
}


