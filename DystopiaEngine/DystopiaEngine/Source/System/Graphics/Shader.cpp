/* HEADER *********************************************************************************/
/*!
\file	Shader.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Shader

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Graphics/Shader.h"		// File Header

#include "Math/MathLib.h"
#include "Utility/DebugAssert.h"

#include <GL/glew.h>
#include <GL/GL.h>
#include <fstream>		// ifstream
#include <sstream>		// stringstream
#include <cstdio>		// printf

#if defined(EDITOR)
#define PRINT_ERRORS EDITOR
#else
#define PRINT_ERRORS 0
#endif


Dystopia::Shader::Shader(void) noexcept :
	mStages{ ::Gfx::ShaderStage::NONE }
{

}

Dystopia::Shader::~Shader(void)
{
}

void Dystopia::Shader::CreateShader(const std::string& _strVert, const std::string& _strFrag)
{

}

void Dystopia::Shader::CreateShader(const std::string& _strVert, const std::string& _strFrag, const std::string& _strGeo)
{

}

void Dystopia::Shader::Bind(void) const
{
}

void Dystopia::Shader::Unbind(void) const
{
}


int Dystopia::Shader::GetUniformLocation(char const* _strName) const
{
	return -1; //glGetUniformLocation(mnShaderID, _strName);
}


void Dystopia::Shader::UploadUniform(char const* _strName, float _f) const
{
	glUniform1f(GetUniformLocation(_strName), _f);
}

void Dystopia::Shader::UploadUniform(char const* _strName, float _f1, float _f2) const
{
	glUniform2f(GetUniformLocation(_strName), _f1, _f2);
}

void Dystopia::Shader::UploadUniform(char const* _strName, float _f1, float _f2, float _f3) const
{
	glUniform3f(GetUniformLocation(_strName), _f1, _f2, _f3);
}

void Dystopia::Shader::UploadUniform(char const* _strName, float _f1, float _f2, float _f3, float _f4) const
{
	glUniform4f(GetUniformLocation(_strName), _f1, _f2, _f3, _f4);
}

void Dystopia::Shader::UploadUniform(char const * _strName, const Math::Vector2& _v) const
{
	glUniform2fv(GetUniformLocation(_strName), 1, reinterpret_cast<float const*>(&_v));
}

void Dystopia::Shader::UploadUniform(char const* _strName, const Math::Vector4& _v) const
{
	glUniform4fv(GetUniformLocation(_strName), 1, reinterpret_cast<float const*>(&_v));
}

void Dystopia::Shader::UploadUniform3(char const* _strName, const Math::Vector4& _v) const
{
	glUniform3fv(GetUniformLocation(_strName), 1, reinterpret_cast<float const*>(&_v));
}

void Dystopia::Shader::UploadUniform(char const* _strName, const Math::Matrix2& _m) const
{
	glUniform4fv(GetUniformLocation(_strName), 1, reinterpret_cast<float const*>(&_m));
}

void Dystopia::Shader::UploadUniform(char const* _strName, const Math::Matrix4& _m) const
{
	glUniformMatrix4fv(
		GetUniformLocation(_strName), 1, GL_TRUE, reinterpret_cast<float const*>(&_m)
	);
}



// Remove our defines
#undef PRINT_ERRORS

