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

#include "Lib/Gfx/Shaders.h"

#include "DataStructure/OString.h"
#include "DataStructure/AutoArray.h"


namespace Dystopia
{
	class ShaderProgram
	{
	public:

		ShaderProgram(bool = true) noexcept;
		~ShaderProgram(void) noexcept;

		bool IsValid(void) const noexcept;
		bool LoadProgram(::Gfx::ShaderStage, char const* _strFile, char const* _strName) noexcept;

		::Gfx::ShaderStage const& GetStage(void) const noexcept;
		::Gfx::ShaderProg const& GetID(void) const noexcept;

		OString const& GetName(void) const noexcept;
		bool IsCustomProgram(void) const noexcept;

		void TrackChangesCallback(void);
		AutoArray<std::pair<OString, ::Gfx::eUniform_t>> const& GetVariables(void) noexcept;

	private:

		OString mstrName;
		::Gfx::ShaderProg mProgram;
		::Gfx::ShaderStage mStage;

		bool mbIsCustom, mbValid;

		AutoArray<std::pair<OString, ::Gfx::eUniform_t>> mVars;
	};
}



#endif		// INCLUDE GUARD

