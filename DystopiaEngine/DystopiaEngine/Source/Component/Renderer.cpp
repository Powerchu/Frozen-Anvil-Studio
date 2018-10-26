/* HEADER *********************************************************************************/
/*!
\file	Renderer.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Basic Renderer.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Component/Renderer.h"

#include "System/Graphics/GraphicsSystem.h"
#include "System/Graphics/Mesh.h"
#include "System/Graphics/MeshSystem.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/Texture2D.h"
#include "System/Driver/Driver.h"
#include "System/Scene/SceneSystem.h"
#include "System/Scene/Scene.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"
#include "IO/TextSerialiser.h"
#if EDITOR
#include "Editor/ProjectResource.h"
#include "Editor/EGUI.h"
#include "Editor/Editor.h"
#endif 


Dystopia::Renderer::Renderer(void) noexcept
	: Component{}, mnUnique{ 0 }, mpMesh{ nullptr }, mpShader{ nullptr },
	mpTexture{ nullptr }, mTexturePath{""}
{
	SetMesh("Quad");
	SetShader(EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->shaderlist["Default Shader"]);
}

Dystopia::Renderer::Renderer(Dystopia::Renderer&& _rhs) noexcept
	: Component{ Ut::Move(_rhs) }, mnUnique{ _rhs.mnUnique }, mpMesh{ _rhs.mpMesh }, mpShader{ _rhs.mpShader }, 
	mpTexture{ _rhs.mpTexture }, mTexturePath{ _rhs.mTexturePath }
{
	_rhs.mpMesh    = nullptr;
	_rhs.mpShader  = nullptr;
	_rhs.mpTexture = nullptr;
}

Dystopia::Renderer::Renderer(const Renderer& _rhs) noexcept
	: Component{ _rhs }, mnUnique{ 0 }, mpMesh{ nullptr }, mpShader{ nullptr }, 
	mpTexture{ nullptr }, mTexturePath{ _rhs.mTexturePath }
{
	SetMesh("Quad");
	SetShader(EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->shaderlist["Default Shader"]);
}

void Dystopia::Renderer::Init(void)
{
	if (mTexturePath.length())
	{
		mpTexture = EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->LoadTexture(mTexturePath);
	}
}

void Dystopia::Renderer::Draw(void) const noexcept
{
	if (mpMesh)
	{
		mpMesh->DrawMesh(GraphicsSystem::GetDrawMode());
	}
}

void Dystopia::Renderer::SetMesh(Mesh* _pMesh) noexcept
{
	mpMesh = _pMesh;
}

void Dystopia::Renderer::SetMesh(const std::string& _strMesh) noexcept
{
	mpMesh = EngineCore::GetInstance()->GetSubSystem<MeshSystem>()->GetMesh(_strMesh);
}


void Dystopia::Renderer::SetShader(Shader* _p) noexcept
{
	mpShader = _p;
}

void Dystopia::Renderer::SetShader(const std::string&) noexcept
{
	// TODO
	__debugbreak();
}

Dystopia::Shader* Dystopia::Renderer::GetShader(void) const noexcept
{
	return mpShader;
}


void Dystopia::Renderer::SetTexture(Texture* _pTexture) noexcept
{
	mpTexture = _pTexture;

	if (_pTexture)
	{
		mTexturePath = _pTexture->GetPath();
	}
	else
	{
		mTexturePath.clear(); 
	}
}

Dystopia::Texture* Dystopia::Renderer::GetTexture(void) const noexcept
{
	return mpTexture;
}


bool Dystopia::Renderer::HasTransparency(void) const noexcept
{
	return (mnFlags & eObjFlag::FLAG_RESERVED) != 0;
}


Dystopia::Renderer* Dystopia::Renderer::Duplicate(void) const
{
	return static_cast<ComponentDonor<Renderer>*>(
		EngineCore::GetInstance()->Get<Renderer::SYSTEM>()
	)->RequestComponent(*this);
}

void Dystopia::Renderer::Serialise(TextSerialiser& _out) const
{
	_out.InsertStartBlock("Renderer");
	Component::Serialise(_out);
	_out << mTexturePath;
	_out.InsertEndBlock("Renderer");
}

void Dystopia::Renderer::Unserialise(TextSerialiser& _in)
{
	_in.ConsumeStartBlock();
	Component::Unserialise(_in);
	_in >> mTexturePath;
	_in.ConsumeEndBlock();
}

void Dystopia::Renderer::EditorUI(void) noexcept
{
#if EDITOR
	EGUI::PushLeftAlign(80);
	TextureField();
	MeshField();
	ShaderField();
	EGUI::PopLeftAlign();
#endif
}

#if EDITOR
void Dystopia::Renderer::TextureField()
{
	if (EGUI::Display::EmptyBox("Texture", 150, (mpTexture) ? mpTexture->GetName() : "-empty-", true))
	{
	}

	if (Dystopia::File *t = EGUI::Display::StartPayloadReceiver<Dystopia::File>(EGUI::PNG))
	{
		Texture *pTex = EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->LoadTexture(t->mPath);

		auto fOld = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Renderer::SetTexture, mpTexture);
		auto fNew = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Renderer::SetTexture, pTex);
		EGUI::GetCommandHND()->InvokeCommand(GetOwner()->GetID(), fOld, fNew);

		EGUI::Display::EndPayloadReceiver();
	}

	EGUI::SameLine();
	if (EGUI::Display::IconCross("Clear", 8.f))
	{
		auto fOld = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Renderer::SetTexture, mpTexture);
		auto fNew = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Renderer::SetTexture, nullptr);
		EGUI::GetCommandHND()->InvokeCommand(GetOwner()->GetID(), fOld, fNew);
	}

	if (mpTexture)
	{
		EGUI::Display::Label("Preview");
		EGUI::SameLine(DefaultAlighnmentSpacing, 80);
		float ratio = static_cast<float>(mpTexture->GetHeight()) / static_cast<float>(mpTexture->GetWidth());
		EGUI::Display::Image(mpTexture->GetID(), Math::Vec2{ 140, 140 * ratio }, false, true);
	}
}

void Dystopia::Renderer::MeshField()
{
	if (EGUI::Display::EmptyBox("Mesh", 150, (mpMesh) ? mpMesh->GetName() : "", true))
	{

	}
	if (Dystopia::File *t = EGUI::Display::StartPayloadReceiver<Dystopia::File>(EGUI::FILE))
	{
		EGUI::Display::EndPayloadReceiver();
	}
}

void Dystopia::Renderer::ShaderField()
{
	if (EGUI::Display::EmptyBox("Shader", 150, "shader has no name or id", true))
	{

	}
	if (Dystopia::File *t = EGUI::Display::StartPayloadReceiver<Dystopia::File>(EGUI::FILE))
	{
		EGUI::Display::EndPayloadReceiver();
	}
}
#endif
