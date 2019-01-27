/* HEADER *********************************************************************************/
/*!
\file	ShaderProgram.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Each one of these represent a shader stage

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Graphics/ShaderProgram.h"
#include "System/Graphics/ShaderSystem.h"
#include "System/Driver/Driver.h"

#include "Lib/GraphicsLib.h"
#include "Utility/DebugAssert.h"
#include "Allocator/StackAlloc.h"
#include "DataStructure/OString.h"

#include <fstream>
#include "GL/glew.h"

namespace
{
	static auto const& pGfxAPI = ::Gfx::GetInstance();
}


Dystopia::ShaderProgram::ShaderProgram(bool _bIsCustom) noexcept
	: mProgram{ pGfxAPI->CreateShaderProgram() }, mStage{ Gfx::ShaderStage::NONE }, mstrName{}, mVars{},
	mbIsCustom{ _bIsCustom }
{

}

Dystopia::ShaderProgram::~ShaderProgram(void) noexcept
{
	pGfxAPI->Free(mProgram);
}

::Gfx::ShaderStage const& Dystopia::ShaderProgram::GetStage(void) const noexcept
{
	return mStage;
}

::Gfx::ShaderProg const& Dystopia::ShaderProgram::GetID(void) const noexcept
{
	return mProgram;
}

OString const& Dystopia::ShaderProgram::GetName(void) const noexcept
{
	return mstrName;
}

bool Dystopia::ShaderProgram::IsCustomProgram(void) const noexcept
{
	return mbIsCustom;
}

void Dystopia::ShaderProgram::TrackChangesCallback(void)
{
	CORE::Get<ShaderSystem>()->NotifyReplace(this);
}

AutoArray<std::pair<HashString, Gfx::eUniform_t>> const& Dystopia::ShaderProgram::GetVariables(void) noexcept
{
	return mVars;
}

bool Dystopia::ShaderProgram::LoadProgram(Gfx::ShaderStage _stage, char const* _file, char const* _strName) noexcept
{
	std::ifstream file{ _file, std::ios::ate | std::ios::in | std::ios::binary };

	if (file.fail())
	{
		DEBUG_PRINT(eLog::ERROR, "Shader LoadProgram failed! File %s not found!\n", _file);
		return false;
	}

	size_t sz = file.tellg();
	if (StackAlloc_t::GetUsableSize() < sz)
	{
		DEBUG_PRINT(eLog::ERROR, "Shader LoadProgram failed!\nInsufficient Memory: %uzbytes available, %uzbytes required!\n", StackAlloc_t::GetUsableSize(), sz);
		return false;
	}

	file.seekg(0);
	char* pData = StackAlloc_t::GetBufferAs<char>();

	file.read(pData, sz);
	pData[sz] = '\0';
	auto shader = pGfxAPI->CompileGLSL(_stage, pData);

	if (!shader)
		return false;

	bool ret = pGfxAPI->LinkShader(mProgram, shader);
	pGfxAPI->Free(shader);

	mStage   = _stage;
	mstrName = _file;
	mstrName = _strName;

#	if EDITOR
		mVars = pGfxAPI->QueryVariables(mProgram);
#	endif

#   if defined(_DEBUG) | defined(DEBUG)
		if (auto err = glGetError())
			__debugbreak();
#   endif 

	return ret;
}

