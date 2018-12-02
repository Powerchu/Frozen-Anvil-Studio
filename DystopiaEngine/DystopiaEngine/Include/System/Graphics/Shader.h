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

#include <string>
#include "Globals.h"

namespace Math
{
	union  Vector2;
	union  Vector4;
	struct Matrix2;
	struct Matrix4;
	union  Quaternion;
}

namespace Dystopia
{
	class _DLL_EXPORT Shader
	{
	public:

		Shader(void) noexcept;
		~Shader(void);

		void CreateShader(
			const std::string& _strVert,
			const std::string& _strFrag
		);

		void CreateShader(
			const std::string& _strVert, 
			const std::string& _strFrag,
			const std::string& _strGeo
		);

		void Bind(void) const;
		void Unbind(void) const;

		unsigned GetID(void) const;

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

		unsigned mnShaderID;

		int GetUniformLocation(char const*) const;
		void LoadShader(unsigned, const std::string&);
	};
}



#endif		// INCLUDE GUARD

