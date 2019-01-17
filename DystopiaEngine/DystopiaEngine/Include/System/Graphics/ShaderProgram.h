/* HEADER *********************************************************************************/
/*!
\file	ShaderProgram.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Each one of these represent a shader stage

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _SHADER_PROGRAM_H_
#define _SHADER_PROGRAM_H_

#include "Lib/Gfx/Shaders.h"

#include "DataStructure/OString.h"


namespace Dystopia
{
	class ShaderProgram
	{
	public:

		ShaderProgram(void) noexcept;
		~ShaderProgram(void) noexcept;

		bool LoadProgram(::Gfx::ShaderStage, char const* _strFile) noexcept;

		::Gfx::ShaderStage const& GetStage(void) const noexcept;
		::Gfx::ShaderProg const& GetID(void) const noexcept;

		OString const& GetName(void) const noexcept;

		void TrackChangesCallback(void);

	private:

		OString mstrName;
		::Gfx::ShaderProg mProgram;
		::Gfx::ShaderStage mStage;
	};
}



#endif		// INCLUDE GUARD

