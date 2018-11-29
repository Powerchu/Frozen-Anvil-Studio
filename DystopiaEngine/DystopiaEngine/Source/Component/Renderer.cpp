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
#include "Component/Transform.h"

#include "System/Graphics/GraphicsSystem.h"
#include "System/Graphics/Mesh.h"
#include "System/Graphics/MeshSystem.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/Texture2D.h"
#include "System/Driver/Driver.h"
#include "System/File/FileSystem.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"
#include "IO/TextSerialiser.h"
#if EDITOR
#include "Editor/ProjectResource.h"
#include "Editor/EditorMain.h"
#include "Editor/EditorCommands.h"
#include "Editor/EGUI.h"
#endif 


Dystopia::Renderer::Renderer(void) noexcept
	: Component{}, mnUnique{ 0 }, mpMesh{ nullptr }, mpShader{ nullptr },
	mpTexture{ nullptr }, mTexturePath{""}
{
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
}

void Dystopia::Renderer::Awake(void)
{
	SetMesh("Quad");
	SetShader(EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->shaderlist["Default Shader"]);

	if (mTexturePath.length())
	{
		mpTexture = EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->LoadTexture(mTexturePath);
	}
}

void Dystopia::Renderer::Init(void)
{
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
	_out << EngineCore::GetInstance()->Get<FileSystem>()->ConvertToRelative(mTexturePath);
	_out.InsertEndBlock("Renderer");
}

void Dystopia::Renderer::Unserialise(TextSerialiser& _in)
{
	std::string path;
	_in.ConsumeStartBlock();
	Component::Unserialise(_in);
	_in >> path;
	_in.ConsumeEndBlock();
	mTexturePath = EngineCore::GetInstance()->Get<FileSystem>()->GetFullPath(path, eFileDir::eResource);
}

void Dystopia::Renderer::EditorUI(void) noexcept
{
#if EDITOR
	/*EGUI::PushLeftAlign(80);

	TextureField();
	MeshField();
	ShaderField();

	EGUI::PopLeftAlign();*/
#endif
}

#if EDITOR
void Dystopia::Renderer::TextureField()
{
	::Editor::File *t = nullptr;
	EGUI::Display::EmptyBox("Texture", 150, (mpTexture) ? mpTexture->GetName() : "-empty-", true);
	auto cmd = ::Editor::EditorMain::GetInstance()->GetSystem<::Editor::EditorCommands>();
	t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::PNG);
	if (t)  EGUI::Display::EndPayloadReceiver();

	if (!t)
	{
		t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::DDS);
		if (t) EGUI::Display::EndPayloadReceiver();
	}
	if (!t)
	{
		t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::BMP);
		if (t) EGUI::Display::EndPayloadReceiver();
	}
	if (t)
	{
		Texture *pTex = EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->LoadTexture(t->mPath.c_str());
		cmd->FunctionCommand(GetOwnerID(), cmd->MakeFnCommand(&Renderer::SetTexture, mpTexture),
										   cmd->MakeFnCommand(&Renderer::SetTexture, pTex));
	}

	EGUI::SameLine();
	if (EGUI::Display::IconCross("Clear", 8.f))
	{
		cmd->FunctionCommand(GetOwnerID(), cmd->MakeFnCommand(&Renderer::SetTexture, mpTexture),
										   cmd->MakeFnCommand(&Renderer::SetTexture, nullptr));
	}

	if (mpTexture)
	{
		EGUI::Display::Label("Preview");
		EGUI::SameLine(DefaultAlighnmentSpacing, 80);
		float ratio = static_cast<float>(mpTexture->GetHeight()) / static_cast<float>(mpTexture->GetWidth());
		EGUI::Display::Image(mpTexture->GetID(), Math::Vec2{ 140, 140 * ratio }, false, true);

		EGUI::SameLine();
		if (EGUI::Display::Button("Auto", Math::Vec2{ 35, 20 }))
		{
			auto w = static_cast<float>(mpTexture->GetWidth());
			auto h = static_cast<float>(mpTexture->GetHeight());
			w /= 10;
			h /= 10;
			cmd->FunctionCommand(GetOwnerID(), cmd->MakeFnCommand<Transform, const Math::Vec4&>(&Transform::SetScale, GetOwner()->GetComponent<Transform>()->GetScale()),
											   cmd->MakeFnCommand<Transform, const Math::Vec4&>(&Transform::SetScale, Math::Vec4{ w, h, 1.f }));
		}
	}
}

void Dystopia::Renderer::MeshField()
{
	if (EGUI::Display::EmptyBox("Mesh", 150, (mpMesh) ? mpMesh->GetName() : "", true))
	{

	}
	if (::Editor::File *t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::FILE))
	{
		EGUI::Display::EndPayloadReceiver();
	}
}

void Dystopia::Renderer::ShaderField()
{
	if (EGUI::Display::EmptyBox("Shader", 150, "shader has no name or id", true))
	{

	}
	if (::Editor::File *t = EGUI::Display::StartPayloadReceiver<::Editor::File>(EGUI::FILE))
	{
		EGUI::Display::EndPayloadReceiver();
	}
}
#endif
