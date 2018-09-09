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
#include "System\Graphics\Shader.h"		// File Header

#include "Math\Vector4.h"

#include <GL\glew.h>
#include <GL\GL.h>
#include <fstream>		// ifstream
#include <sstream>		// stringstream
#include <cstdio>		// printf

#if defined(EDITOR)
#define PRINT_ERRORS EDITOR
#else
#define PRINT_ERRORS 0
#endif


Dystopia::Shader::Shader(void) noexcept :
	mnShaderID{ glCreateProgram() }
{

}

Dystopia::Shader::~Shader(void)
{
	glDeleteProgram(mnShaderID);
}

void Dystopia::Shader::CreateShader(const std::string& _strVert, const std::string& _strFrag)
{
	unsigned vert = glCreateShader(GL_VERTEX_SHADER),
			 frag = glCreateShader(GL_FRAGMENT_SHADER);

	// Give OpenGL the shader data to compile
	LoadShader(vert, _strVert);
	LoadShader(frag, _strFrag);

	// Attach compiled shader to program
	glAttachShader(mnShaderID, vert);
	glAttachShader(mnShaderID, frag);
	glLinkProgram(mnShaderID);

	// Free the vertex and fragment shaders
	glDetachShader(mnShaderID, vert);
	glDetachShader(mnShaderID, frag);
	glDeleteShader(vert);
	glDeleteShader(frag);


#if PRINT_ERRORS

	int nStatus = 0;
	glGetProgramiv(mnShaderID, GL_LINK_STATUS, &nStatus);

	if (GL_FALSE == nStatus)
	{
		glGetProgramiv(mnShaderID, GL_INFO_LOG_LENGTH, &nStatus);

		std::string log;
		log.reserve(nStatus);

		glGetProgramInfoLog(mnShaderID, nStatus, nullptr, &log[0]);

		std::printf("Shader Link Error : %s", log.c_str());
	}

#endif		// PRINT_ERRORS
}

void Dystopia::Shader::CreateShader(const std::string& _strVert, const std::string& _strFrag, const std::string& _strGeo)
{
	// Create and attach the geometry shader
	unsigned geo = glCreateShader(GL_GEOMETRY_SHADER);
	LoadShader(geo, _strGeo);
	glAttachShader(mnShaderID, geo);

	// Create the rest of the Shader Program
	CreateShader(_strVert, _strFrag);

	// Free geometry shader
	glDetachShader(mnShaderID, geo);
	glDeleteShader(geo);
}

void Dystopia::Shader::UseShader(void) const
{
	glUseProgram(mnShaderID);
}

void Dystopia::Shader::LoadShader(unsigned _nProg, const std::string& _path)
{
	std::ifstream file;
	std::stringstream buffer;
	const char* str;

	file.open(_path);
	buffer << file.rdbuf();
	str = buffer.str().c_str();
	file.close();

	glShaderSource(_nProg, 1, &str, nullptr);
	glCompileShader(_nProg);


#if PRINT_ERRORS

	int nStatus = 0;
	glGetShaderiv(_nProg, GL_COMPILE_STATUS, &nStatus);

	if (GL_FALSE == nStatus)
	{
		glGetShaderiv(_nProg, GL_INFO_LOG_LENGTH, &nStatus);

		std::string log;
		log.reserve(nStatus);

		glGetShaderInfoLog(_nProg, nStatus, nullptr, &log[0]);

		std::printf("Shader Compile Error : %s", log.c_str());
	}

#endif		//  PRINT_ERRORS
}

int Dystopia::Shader::GetUniformLocation(const std::string& _strName)
{
	return glGetUniformLocation(mnShaderID, _strName.c_str());
}


void Dystopia::Shader::UploadUniform(const std::string& _strName, float _f)
{
	glUniform1f(GetUniformLocation(_strName), _f);
}

void Dystopia::Shader::UploadUniform(const std::string& _strName, float _f1, float _f2)
{
	glUniform2f(GetUniformLocation(_strName), _f1, _f2);
}

void Dystopia::Shader::UploadUniform(const std::string& _strName, float _f1, float _f2, float _f3)
{
	glUniform3f(GetUniformLocation(_strName), _f1, _f2, _f3);
}

void Dystopia::Shader::UploadUniform(const std::string& _strName, float _f1, float _f2, float _f3, float _f4)
{
	glUniform4f(GetUniformLocation(_strName), _f1, _f2, _f3, _f4);
}

void Dystopia::Shader::UploadUniform(const std::string& _strName, Math::Vector2& _v)
{
	glUniform2fv(GetUniformLocation(_strName), 1, reinterpret_cast<float*>(&_v));
}

void Dystopia::Shader::UploadUniform(const std::string& _strName, Math::Vector4& _v)
{
	glUniform4fv(GetUniformLocation(_strName), 1, reinterpret_cast<float*>(&_v));
}

void Dystopia::Shader::UploadUniform3(const std::string& _strName, Math::Vector4& _v)
{
	glUniform3fv(GetUniformLocation(_strName), 1, reinterpret_cast<float*>(&_v));
}

void Dystopia::Shader::UploadUniform(const std::string& _strName, Math::Matrix2& _m)
{
	glUniform4fv(GetUniformLocation(_strName), 1, reinterpret_cast<float*>(&_m));
}

void Dystopia::Shader::UploadUniform(const std::string& _strName, Math::Matrix4& _m)
{
	glUniformMatrix4fv(
		GetUniformLocation(_strName), 1, GL_FALSE, reinterpret_cast<float*>(&_m)
	);
}



// Remove our defines
#undef PRINT_ERRORS

