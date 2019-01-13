/* HEADER *********************************************************************************/
/*!
\file	ShaderSystem.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Manages Shaders

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _SHADERSYS_H_
#define _SHADERSYS_H_

#include "DataStructure/AutoArray.h"
#include "DataStructure/MagicArray.h"

#include "Lib/Gfx/Shaders.h"

#include <map>


namespace Dystopia
{
	class Shader;
	class ShaderProgram;

	class ShaderSystem
	{
	public:

		ShaderSystem(void) noexcept;
		~ShaderSystem(void) noexcept;

		void EditorUpdate(void);
		void Shutdown(void) noexcept;

		Shader* CreateShader(char const* _strName) noexcept;
		ShaderProgram* CreateShaderProgram(::Gfx::ShaderStage, char const* _strName) noexcept;

		Shader* GetShader(char const* _strName) const noexcept;
		ShaderProgram* GetShaderProgram(char const* _strName) const noexcept;

		void NotifyReplace(ShaderProgram*);

		inline Shader* operator[] (char const* _strName) const noexcept;

	private:

		AutoArray<ShaderProgram*> mChanges;

		MagicArray<Shader> mShaders;
		MagicArray<ShaderProgram> mPrograms;
	};
}


inline Dystopia::Shader* Dystopia::ShaderSystem::operator[](char const * _str) const noexcept
{
	return GetShader(_str);
}



#endif		// INCLUDE GUARD

