/* HEADER *********************************************************************************/
/*!
\file	ShaderSystem.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Manages Shaders

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Graphics/ShaderSystem.h"
#include "System/Graphics/Shader.h"

#include "Lib/GraphicsLib.h"
#include "Lib/Gfx/Shaders.h"

#include "DataStructure/OString.h"

namespace
{
	static auto const& pGfxAPI = ::Gfx::GetInstance();
}


Dystopia::ShaderSystem::~ShaderSystem(void) noexcept
{
}

void Dystopia::ShaderSystem::Shutdown(void) noexcept
{
}

void Dystopia::ShaderSystem::EditorUpdate(void)
{
	auto GfxAPI = ::Gfx::GetInstance();
	for (auto& e : mChanges)
	{
		ShaderProgram temp{};

		if (temp.LoadProgram(e->GetStage(), e->GetName().c_str()))
			Ut::Swap(*e, temp);
	}

	mChanges.clear();
}

Dystopia::Shader* Dystopia::ShaderSystem::GetShader(char const* _strName) const noexcept
{
	OString strName{ _strName };

	for (auto& e : mShaders)
		if (e.GetName() == strName)
			return &e;

	return nullptr;
}

Dystopia::Shader* Dystopia::ShaderSystem::CreateShader(char const* _strName) noexcept
{
	if (auto p = GetShader(_strName))
		return p;

	return mShaders.Emplace(_strName);
}

Dystopia::ShaderProgram* Dystopia::ShaderSystem::CreateShaderProgram(::Gfx::ShaderStage _stage, char const* _strName) noexcept
{
	if (auto ret = GetShaderProgram(_strName))
		return ret;

	auto p = mPrograms.Emplace();
	if (p->LoadProgram(_stage, _strName))
		return p;

	mPrograms.Remove(p);
	return nullptr;
}

Dystopia::ShaderProgram* Dystopia::ShaderSystem::GetShaderProgram(char const* _strName) const noexcept
{
	OString strName{ _strName };

	for (auto& e : mPrograms)
		if (e.GetName() == strName)
			return &e;

	return nullptr;
}


