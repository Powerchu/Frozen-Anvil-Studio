/* HEADER *********************************************************************************/
/*!
\file	ShaderSystem.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Manages Shaders

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _SHADERSYS_H_
#define _SHADERSYS_H_

#include "DataStructure/Array.h"
#include "DataStructure/AutoArray.h"
#include "DataStructure/MagicArray.h"

#include "Lib/Gfx/Shaders.h"
#include "System/File/FileSystem.h"


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

		Shader* CreateShader(char const* _strName, bool _bCustom) noexcept;
		ShaderProgram* CreateShaderProgram(::Gfx::ShaderStage, char const* _strName, bool _bTrack = true) noexcept;

		Shader* GetShader(char const* _strName) const noexcept;
		ShaderProgram* GetShaderProgram(char const* _strName) const noexcept;

		void NotifyReplace(ShaderProgram*) noexcept;

		void LoadShaderList(char const* _strPath, eFileDir, bool _bTrack = true) noexcept;

		inline Shader* operator[] (char const* _strName) const noexcept;

	private:

		void SaveCustomShaders(void) noexcept;
		ShaderProgram* ResolveShaderProgram(std::string const&, eFileDir, bool);

		AutoArray<ShaderProgram*> mChanges;

		MagicArray<Shader> mShaders;
		MagicArray<ShaderProgram> mPrograms;
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


inline Dystopia::Shader* Dystopia::ShaderSystem::operator[](char const * _str) const noexcept
{
	return GetShader(_str);
}



#endif		// INCLUDE GUARD

