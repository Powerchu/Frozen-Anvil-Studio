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

#include "Lib/GraphicsLib.h"
#include "Utility/DebugAssert.h"
#include "Allocator/StackAlloc.h"

#include <fstream>


Dystopia::ShaderProgram::ShaderProgram(void) noexcept
	: mnProgram{ Gfx::GetInstance()->CreateShaderProgram() }, mStage{ Gfx::ShaderStage::FRAGMENT }
{

}

Dystopia::ShaderProgram::~ShaderProgram(void) noexcept
{
	Gfx::GetInstance()->DestroyShaderProgram(mnProgram);
}

bool Dystopia::ShaderProgram::LoadProgram(Gfx::ShaderStage _stage, char const* _file) noexcept
{
	auto pAPI = Gfx::GetInstance();
	std::ifstream file{ _file, std::ios::ate | std::ios::in | std::ios::binary };

	if (file.fail())
	{
		DEBUG_PRINT(eLog::ERROR, "Shader LoadProgram failed! File %s not found!", _file);
		return false;
	}

	size_t sz = file.tellg();
	if (StackAlloc_t::GetUsableSize() < sz)
	{
		DEBUG_PRINT(eLog::ERROR, "Shader LoadProgram failed! Insufficient Memory, %uzbytes required!", sz);
		return false;
	}

	file.seekg(0);
	char* pData = StackAlloc_t::GetBufferAs<char>();

	file.read(pData, sz);
	auto shader = pAPI->CompileGLSL(_stage, pData);

	if (!shader)
		return false;

	pAPI->LinkShader(mnProgram, shader);

	mStage = _stage;
	return true;
}


