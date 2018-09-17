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
#include "Component\Renderer.h"
#include "System\Graphics\GraphicsSystem.h"
#include "System\Graphics\Mesh.h"
#include "System\Graphics\MeshSystem.h"
#include "System\Graphics\Shader.h"
#include "System\Graphics\Texture2D.h"
#include "System\Driver\Driver.h"
#include "Object\ObjectFlags.h"


Dystopia::Renderer::Renderer(void) noexcept
	: mnUnique{ 0 }, mpMesh{ nullptr }, mpShader{ nullptr }, mpTexture{ nullptr }
{
	SetMesh("Quad");
	SetShader(EngineCore::GetInstance()->GetSystem<GraphicsSystem>()->shaderlist.begin()->second);
	SetTexture(new Texture2D{ "Resource/Editor/EditorStartup.png" });
}

void Dystopia::Renderer::Init(void)
{
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

void Dystopia::Renderer::Serialise(TextSerialiser&) const
{

}

void Dystopia::Renderer::Unserialise(TextSerialiser&)
{

}


