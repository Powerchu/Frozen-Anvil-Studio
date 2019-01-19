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
#include "System/File/FileSystem.h"
#include "System/Driver/Driver.h"

#include "Lib/GraphicsLib.h"
#include "Lib/Gfx/Shaders.h"

#include "DataStructure/OString.h"
#include "IO/TextSerialiser.h"
#include "Globals.h"

#include <GL/glew.h>


namespace
{
	static auto const& pGfxAPI = ::Gfx::GetInstance();
}


Dystopia::ShaderSystem::ShaderSystem(void) noexcept
{

}

Dystopia::ShaderSystem::~ShaderSystem(void) noexcept
{
}

void Dystopia::ShaderSystem::Shutdown(void) noexcept
{
}

void Dystopia::ShaderSystem::EditorUpdate(void)
{
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

Dystopia::ShaderProgram* Dystopia::ShaderSystem::CreateShaderProgram(::Gfx::ShaderStage _stage, char const* _strName, bool _bTrack) noexcept
{
	OString strName{ _strName };
	strName = strName.substr(strName.find_last_of("/\\") + 1);
	if (auto ret = GetShaderProgram(strName.c_str()))
		return ret;

	auto p = mPrograms.Emplace();
	if (p->LoadProgram(_stage, _strName))
	{
#		if EDITOR
		if (_bTrack)
		{
			auto id = CORE::Get<FileSystem>()->TrackFile(_strName);
			CORE::Get<FileSystem>()->BindFileTrackEvent(id, &ShaderProgram::TrackChangesCallback, p);
		}
#		endif

		return p;
	}

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

Dystopia::ShaderProgram* Dystopia::ShaderSystem::ResolveShaderProgram(std::string const& _strPath, bool _bTrack)
{
	OString const ext = _strPath.substr(_strPath.rfind('.') + 1).c_str();
	ShaderProgram* ret = nullptr;

	if (Gbl::VERTSHADER_EXT == ext)
	{
		ret = CreateShaderProgram(::Gfx::ShaderStage::VERTEX, _strPath.c_str(), _bTrack);

	}
	else if (Gbl::FRAGSHADER_EXT == ext)
	{
		ret = CreateShaderProgram(::Gfx::ShaderStage::FRAGMENT, _strPath.c_str(), _bTrack);
	}
	else if (Gbl::GEOGSHADER_EXT == ext)
	{
		ret = CreateShaderProgram(::Gfx::ShaderStage::GEOMETRY, _strPath.c_str(), _bTrack);
	}

	return ret;
}

void Dystopia::ShaderSystem::NotifyReplace(ShaderProgram* _p) noexcept
{
	mChanges.EmplaceBack(_p);
}

void Dystopia::ShaderSystem::LoadShaderList(char const* _strPath, eFileDir _dir, bool _bTrack) noexcept
{
	auto pFileSys = CORE::Get<FileSystem>();
	auto pShaderSys = CORE::Get<ShaderSystem>();
	auto file = Serialiser::OpenFile<TextSerialiser>(_strPath, Serialiser::MODE_READ);
	//auto file = Serialiser::OpenFile<TextSerialiser>(pFileSys->GetFullPath(_strPath, _dir).c_str(), Serialiser::MODE_READ);
	std::string strName;

	// Load shader programs
	file.ConsumeStartBlock();
	while (!file.EndOfInput())
	{
		file >> strName;
		//strName = pFileSys->GetFullPath(strName, _dir);

		ResolveShaderProgram(strName, _bTrack);
	}

	file.ConsumeStartBlock();
	while (!file.EndOfInput())
	{
		file >> strName;
		auto pShader = pShaderSys->CreateShader(strName.c_str());

		if (pShader)
		{
			do
			{
				file >> strName;
				pShader->AttachProgram(GetShaderProgram(strName.c_str()));
			} while (!file.EndOfInput());
		}

		file.ConsumeStartBlock();
	}
}

