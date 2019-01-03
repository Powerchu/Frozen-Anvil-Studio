/* HEADER *********************************************************************************/
/*!
\file	ShaderProgram.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Each one of these represent a shader stage

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Graphics/ShaderProgram.h"

#include "Lib/GraphicsLib.h"


Dystopia::ShaderProgram::ShaderProgram(void) noexcept
	: mnProgram{ 0 }
{

}

bool Dystopia::ShaderProgram::LoadProgram(Gfx::ShaderStage _stage, void const* _data, Gfx::ShaderLanguage _lan)
{
	auto pAPI = Gfx::GetInstance();

}


