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
	SaveCustomShaders();
}

void Dystopia::ShaderSystem::EditorUpdate(void)
{
	auto pFileSys = CORE::Get<FileSystem>();
	for (auto& e : mChanges)
	{
		ShaderProgram temp{};

		if (temp.LoadProgram(e->GetStage(), pFileSys->GetFullPath(e->GetName().c_str(), eFileDir::eResource).c_str(), e->GetName().c_str()))
			Ut::Swap(*e, temp);
#		if defined(_DEBUG) | defined(DEBUG)
		if (auto err = glGetError())
			__debugbreak();
#		endif 

		for (auto& s : mShaders)
			s.ReattachProgram(e);
#		if defined(_DEBUG) | defined(DEBUG)
		if (auto err = glGetError())
			__debugbreak();
#		endif 
	}

	mChanges.clear();
#	if defined(_DEBUG) | defined(DEBUG)
	if (auto err = glGetError())
		__debugbreak();
#	endif 
}

Dystopia::Shader* Dystopia::ShaderSystem::GetShader(char const* _strName) const noexcept
{
	OString strName{ _strName };

	for (auto& e : mShaders)
		if (e.GetName() == strName)
			return &e;

	return nullptr;
}

Dystopia::Shader* Dystopia::ShaderSystem::CreateShader(char const* _strName, bool _bCustom) noexcept
{
	if (auto p = GetShader(_strName))
		return p;

	return mShaders.Emplace(_strName, _bCustom);
}

Dystopia::ShaderProgram* Dystopia::ShaderSystem::CreateShaderProgram(::Gfx::ShaderStage _stage, char const* _strName, bool _bTrack) noexcept
{
	OString strName{ _strName };
	strName = strName.substr(strName.find_last_of("/\\", strName.find_last_of("/\\") - 1) + 1);
	if (auto ret = GetShaderProgram(strName.c_str()))
		return ret;

	auto p = mPrograms.Emplace(_bTrack);
	if (p->LoadProgram(_stage, _strName, strName.c_str()))
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

Dystopia::ShaderProgram* Dystopia::ShaderSystem::ResolveShaderProgram(std::string const& _strPath, eFileDir _dir, bool _bTrack)
{
	OString const ext = _strPath.substr(_strPath.rfind('.') + 1).c_str();
	ShaderProgram* ret = nullptr;
	auto pFileSys = CORE::Get<FileSystem>();

	if (Gbl::VERTSHADER_EXT == ext)
	{
		ret = CreateShaderProgram(::Gfx::ShaderStage::VERTEX, pFileSys->GetFullPath(_strPath, _dir).c_str(), _bTrack);
	}
	else if (Gbl::FRAGSHADER_EXT == ext)
	{
		ret = CreateShaderProgram(::Gfx::ShaderStage::FRAGMENT, pFileSys->GetFullPath(_strPath, _dir).c_str(), _bTrack);
	}
	else if (Gbl::GEOGSHADER_EXT == ext)
	{
		ret = CreateShaderProgram(::Gfx::ShaderStage::GEOMETRY, pFileSys->GetFullPath(_strPath, _dir).c_str(), _bTrack);
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
	auto file = Serialiser::OpenFile<TextSerialiser>(pFileSys->GetFullPath(_strPath, _dir).c_str(), Serialiser::MODE_READ);
	std::string strName;

	// Load shader programs
	file.ConsumeStartBlock();
	while (!file.EndOfInput())
	{
		file >> strName;

		ResolveShaderProgram(strName, _dir, _bTrack);
	}

	file.ConsumeStartBlock();
	while (!file.EndOfInput())
	{
		file >> strName;
		auto pShader = CreateShader(strName.c_str(), _bTrack);

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

void Dystopia::ShaderSystem::SaveCustomShaders(void) noexcept
{
	auto pFileSys = CORE::Get<FileSystem>();

	auto path = pFileSys->GetProjectFolders<std::string>(eFileDir::eResource) + "/Shader/ShaderList.txt";
	auto file = Serialiser::OpenFile<TextSerialiser>(path.c_str(), Serialiser::MODE_WRITE);

	char buffer[256]{};

	file.InsertStartBlock("PROGRAMS");
	for (auto& e : mPrograms)
	{
		if (e.IsCustomProgram())
		{
			*Ut::Copy(e.GetName(), &buffer[0]) = '\0';
			file << static_cast<char*>(buffer);
		}
	}

	file.InsertEndBlock("PROGRAMS");
	for (auto& e : mShaders)
	{
		if (e.IsCustomShader())
		{
			file.InsertStartBlock("SHADER");
			e.Unserialize(file);
			file.InsertEndBlock("SHADER");
		}
	}
}


