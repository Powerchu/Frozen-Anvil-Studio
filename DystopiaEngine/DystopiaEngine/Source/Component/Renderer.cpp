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
#endif 


Dystopia::Renderer::Renderer(void) noexcept
	: mnUnique{ 0 }, mpMesh{ nullptr }, mpShader{ nullptr }, mpTexture{ nullptr }, mLastKnownPath{""}
{
	SetMesh("Quad");
	SetShader(EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->shaderlist["Default Shader"]);
}

void Dystopia::Renderer::Init(void)
{
	if (!mLastKnownPath.empty())
	{		
		Texture *pTex = EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->LoadTexture(mLastKnownPath);
		SetTexture(pTex);
		pTex = nullptr;
	}

}

void Dystopia::Renderer::Draw(void) const noexcept
{
	if (mpMesh)
	{
		mpMesh->UseMesh(GraphicsSystem::GetDrawMode());
	}
}


void Dystopia::Renderer::SetMesh(Mesh* _pMesh) noexcept
{
	mpMesh = _pMesh;
}

void Dystopia::Renderer::SetMesh(const std::string& _strMesh) noexcept
{
	mpMesh =
		EngineCore::GetInstance()->GetSubSystem<MeshSystem>()->GetMesh(_strMesh);
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
	Renderer* pThis = const_cast<Renderer*>(this);
	++(pThis->mnUnique);

	return pThis;
}

void Dystopia::Renderer::Serialise(TextSerialiser& _out) const
{
	_out.InsertStartBlock("Renderer");
	_out << GetOwner()->GetID();
	_out << mLastKnownPath;
	_out.InsertEndBlock("Renderer");
}

void Dystopia::Renderer::Unserialise(TextSerialiser& _in)
{
	uint64_t id;
	_in.ConsumeStartBlock();
	_in >> id;
	_in >> mLastKnownPath;
	_in.ConsumeEndBlock();
	
	if (GameObject* owner = 
		EngineCore::GetInstance()->GetSystem<SceneSystem>()->GetCurrentScene().FindGameObject(id))
	{
		owner->AddComponent(this, Renderer::TAG{});
		Init();
	}
}

void Dystopia::Renderer::EditorUI(void) noexcept
{
#if EDITOR
	TextureField();
	MeshField();
	ShaderField();
#endif
}

#if EDITOR
void Dystopia::Renderer::TextureField()
{
	if (EGUI::Display::EmptyBox("Texture   ", 150, (mpTexture) ? std::to_string(mpTexture->GetID()) : "", true))
	{

	}

	if (Dystopia::File *t = EGUI::Display::StartPayloadReceiver<Dystopia::File>(EGUI::PNG))
	{
		mLastKnownPath = t->mPath;
		Texture *pTex = EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->LoadTexture(t->mPath);
		auto fOld = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Renderer::SetTexture, mpTexture);
		auto fNew = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Renderer::SetTexture, pTex);
		EGUI::GetCommandHND()->InvokeCommand(GetOwner()->GetID(), fOld, fNew);
		EGUI::Display::EndPayloadReceiver();
	}
}

void Dystopia::Renderer::MeshField()
{
	if (EGUI::Display::EmptyBox("Mesh      ", 150, (mpMesh) ? mpMesh->GetName() : "", true))
	{

	}
	if (Dystopia::File *t = EGUI::Display::StartPayloadReceiver<Dystopia::File>(EGUI::FILE))
	{
		//Mesh *pMesh = EngineCore::GetInstance()->GetSystem<GraphicsSystem>()-> ??? ;
		//auto fOld = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Renderer::SetTexture, mpMesh);
		//auto fNew = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Renderer::SetTexture, pMesh);
		//EGUI::GetCommandHND()->InvokeCommand(GetOwner()->GetID(), fOld, fNew);
		EGUI::Display::EndPayloadReceiver();
	}
}

void Dystopia::Renderer::ShaderField()
{
	if (EGUI::Display::EmptyBox("Shader    ", 150, "shader has no name or id", true))
	{

	}
	if (Dystopia::File *t = EGUI::Display::StartPayloadReceiver<Dystopia::File>(EGUI::FILE))
	{
		//Shader *pShade = EngineCore::GetInstance()->GetSystem<GraphicsSystem>()-> ??? ;
		//auto fOld = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Renderer::SetTexture, mpShader);
		//auto fNew = EGUI::GetCommandHND()->Make_FunctionModWrapper(&Dystopia::Renderer::SetTexture, pShade);
		//EGUI::GetCommandHND()->InvokeCommand(GetOwner()->GetID(), fOld, fNew);
		EGUI::Display::EndPayloadReceiver();
	}
}
#endif
