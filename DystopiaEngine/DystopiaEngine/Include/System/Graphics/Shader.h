/* HEADER *********************************************************************************/
/*!
\file	Shader.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Shader

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _SHADER_H_
#define _SHADER_H_

#include "Globals.h"
#include "Math/MathFwd.h"

#include "Lib/Gfx/Shaders.h"
#include "System/Graphics/ShaderProgram.h"

#include <string>


namespace Dystopia
{
	class _DLL_EXPORT Shader
	{
	public:

		Shader(void) noexcept;
		~Shader(void);

		void CreateShader(
			std::string const& _strVert,
			std::string const& _strFrag
		);

		void CreateShader(
			std::string const& _strVert,
			std::string const& _strFrag,
			std::string const& _strGeo
		);

		void AttachProgram(ShaderProgram*);

		void Bind(void) const;
		void Unbind(void) const;

		void UploadUniform(char const*, const Math::Vector2&) const;
		void UploadUniform(char const*, const Math::Vector4&) const;
		void UploadUniform(char const*, const Math::Matrix2&) const;
		void UploadUniform(char const*, const Math::Matrix4&) const;
		void UploadUniform3(char const*, const Math::Vector4&) const;

		void UploadUniform(char const*, float) const;
		void UploadUniform(char const*, float, float) const;
		void UploadUniform(char const*, float, float, float) const;
		void UploadUniform(char const*, float, float, float, float) const;


	private:

		::Gfx::ShaderStage mStages;

		int GetUniformLocation(char const*) const;
		void LoadShader(unsigned, const std::string&);
	};
}



#endif		// INCLUDE GUARD

