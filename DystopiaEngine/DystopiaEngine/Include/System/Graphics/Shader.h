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
	class Shader
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

		void UseShader(void) const;


		void UploadUniform(const std::string&, Math::Vector2&);
		void UploadUniform(const std::string&, Math::Vector4&);
		void UploadUniform(const std::string&, Math::Matrix2&);
		void UploadUniform(const std::string&, Math::Matrix4&);
		void UploadUniform3(const std::string&, Math::Vector4&);

		void UploadUniform(const std::string&, float);
		void UploadUniform(const std::string&, float, float);
		void UploadUniform(const std::string&, float, float, float);
		void UploadUniform(const std::string&, float, float, float, float);


	private:

		unsigned mnShaderID;

		int GetUniformLocation(const std::string&);
		void LoadShader(unsigned, const std::string&);
	};
}



#endif		// INCLUDE GUARD

