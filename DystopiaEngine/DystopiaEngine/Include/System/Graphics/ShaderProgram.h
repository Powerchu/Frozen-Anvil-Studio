/* HEADER *********************************************************************************/
/*!
\file	ShaderProgram.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Each one of these represent a shader stage

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _SHADER_PROGRAM_H_
#define _SHADER_PROGRAM_H_

#include "Lib\Gfx\Shaders.h"


namespace Dystopia
{
	class ShaderProgram
	{
	public:

		ShaderProgram(void) noexcept;
		~ShaderProgram(void) noexcept;

		bool LoadProgram(Gfx::ShaderStage, char const*) noexcept;


	private:

		unsigned mnProgram;
		Gfx::ShaderStage mStage;
	};
}



#endif		// INCLUDE GUARD

