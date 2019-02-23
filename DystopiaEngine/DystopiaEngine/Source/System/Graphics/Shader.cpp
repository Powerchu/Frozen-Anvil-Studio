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
#include "Lib/GraphicsLib.h"
#include "System/Graphics/ShaderSystem.h"

#include "System/Driver/Driver.h"

#include "Math/MathLib.h"
#include "Utility/DebugAssert.h"
#include "DataStructure/AutoArray.h"

#include <GL/glew.h>
#include <GL/GL.h>


namespace
{
	using ::Gfx::GetInstance;
}


Dystopia::Shader::Shader(OString const& _strName, bool _bIsCustom) noexcept :
	mID{ GetInstance()->CreateShaderPipeline() }, mStages{ ::Gfx::ShaderStage::NONE }, mstrName{ _strName },
	mbUpdate{ true }, mbIsCustom{ _bIsCustom }, mbValid{ true }
{

}

Dystopia::Shader::~Shader(void)
{
	GetInstance()->Free(mID);
}


void Dystopia::Shader::AttachProgram(ShaderProgram* _prog)
{
	if (!_prog) return;

	if (static_cast<unsigned>(mStages & _prog->GetStage()))
	{
		for (auto& e : mPrograms)
			if (e->GetStage() == _prog->GetStage())
			{
				e = _prog;
				break;
			}
	}
	else
	{
		mStages |= _prog->GetStage();
		mPrograms.EmplaceBack(_prog);
	}

	mbValid = mbValid && _prog->IsValid();
	mbUpdate = true;

#if EDITOR
	if (_prog->IsValid())
#endif
	GetInstance()->AttachShaderProgram(mID, _prog->GetID(), _prog->GetStage());

#   if defined(_DEBUG) | defined(DEBUG)
	if (auto err = glGetError())
		__debugbreak();
#   endif 
}

void Dystopia::Shader::ReattachProgram(ShaderProgram* _prog)
{
	mbValid = true;
	for (auto& e : mPrograms)
	{
		if (e == _prog && e->IsValid())
			GetInstance()->AttachShaderProgram(mID, _prog->GetID(), _prog->GetStage());

		mbValid = mbValid && e->IsValid();
	}

	mbUpdate = true;
}

void Dystopia::Shader::DetachProgram(ShaderProgram* _prog)
{
	if (static_cast<unsigned>(mStages & _prog->GetStage()))
	{
		for (auto& e : Ut::Range(mPrograms).Reverse())
			if (e->GetStage() == _prog->GetStage())
			{
				mPrograms.FastRemove(&e);
				break;
			}

		GetInstance()->AttachShaderProgram(mID, 0, _prog->GetStage());
		mStages &= ~_prog->GetStage();
	}

	mbUpdate = true;
}

void Dystopia::Shader::DetachProgram(Gfx::ShaderStage _stage)
{
	if (static_cast<unsigned>(mStages & _stage))
	{
		for (auto& e : Ut::Range(mPrograms).Reverse())
		{
			if (static_cast<unsigned>(e->GetStage() & _stage))
			{
				GetInstance()->AttachShaderProgram(mID, 0, e->GetStage());
				mPrograms.FastRemove(&e);
			}
		}

		mStages &= ~_stage;
	}

	mbUpdate = true;
}


void Dystopia::Shader::Bind(void) const noexcept
{
	GetInstance()->UseShaderPipeline(mID);
}

void Dystopia::Shader::Unbind(void) const noexcept
{
	//pGfxAPI->UseShaderPipeline(0);
}


bool Dystopia::Shader::IsValid(void) const noexcept
{
	return mbValid;
}

OString const& Dystopia::Shader::GetName(void) const noexcept
{
	return mstrName;
}

bool Dystopia::Shader::IsCustomShader(void) const noexcept
{
	return mbIsCustom;
}

void Dystopia::Shader::Unserialize(TextSerialiser& _file)
{
	_file << GetName();

	for (auto& e : mPrograms)
		_file << e->GetName();
}


void Dystopia::Shader::OnEditorUI(void) const
{
}

AutoArray<std::pair<OString, unsigned>> const & Dystopia::Shader::GetTextureList(void) noexcept
{
	if (mbUpdate)
		ImportVariables();

	return mTextures;
}

AutoArray<std::pair<OString, Gfx::eUniform_t>> const& Dystopia::Shader::GetVariables(void) noexcept
{
	if (mbUpdate)
		ImportVariables();

	return mVars;
}

void Dystopia::Shader::ImportVariables(void) noexcept
{
	mVars.clear();

	for (auto& p : mPrograms)
	{
		for (auto& e : p->GetVariables())
		{
			bool bNew = true;
			for (auto& var : mVars)
			{
				if (var.first == e.first)
				{
					bNew = false;
					break;
				}
			}

			if (bNew) mVars.EmplaceBack(e);
		}

		for (auto& e : p->GetTextureList())
			if (mTextures.size() <= e.second)
				mTextures.EmplaceBack(e);
	}

	mTextures.Sort([](auto& lhs, auto& rhs) {
		return lhs.second < rhs.second;
	});

	mbUpdate = false;
}


namespace
{
	template <typename T, typename C, typename ... U>
	inline void UploadUniformAux(C const& _target, T _func, char const* _strName, U&& ... _args) noexcept
	{
		for (auto& e : _target)
		{
			auto loc = GetInstance()->GetUniformLocation(e->GetID(), _strName);
			(GetInstance()->*_func)(e->GetID(), loc, Ut::Fwd<U>(_args)...);
		}
	}
}

void Dystopia::Shader::UploadUniform(char const* _strName, float _f) const noexcept
{
	UploadUniformAux(mPrograms, &::Gfx::GraphicsAPI::UploadUniform1f, _strName, 1u, &_f);
}

void Dystopia::Shader::UploadUniform(char const* _strName, float _f1, float _f2) const noexcept
{
	float tempArr[]{ _f1, _f2 };

	UploadUniformAux(mPrograms, &::Gfx::GraphicsAPI::UploadUniform2f, _strName, 1u, tempArr);
}

void Dystopia::Shader::UploadUniform(char const* _strName, float _f1, float _f2, float _f3) const noexcept
{
	float tempArr[]{ _f1, _f2, _f3 };

	UploadUniformAux(mPrograms, &::Gfx::GraphicsAPI::UploadUniform3f, _strName, 1u, tempArr);
}

void Dystopia::Shader::UploadUniform(char const* _strName, float _f1, float _f2, float _f3, float _f4) const noexcept
{
	float tempArr[]{ _f1, _f2, _f3, _f4 };

	UploadUniformAux(mPrograms, &::Gfx::GraphicsAPI::UploadUniform4f, _strName, 1u, tempArr);
}

void Dystopia::Shader::UploadUniformi(char const* _strName, int _n) const noexcept
{
	UploadUniformAux(mPrograms, &::Gfx::GraphicsAPI::UploadUniform1i, _strName, 1u, &_n);
}

void Dystopia::Shader::UploadUniformi(char const* _strName, int _n1, int _n2) const noexcept
{
	int tempArr[]{ _n1, _n2 };

	UploadUniformAux(mPrograms, &::Gfx::GraphicsAPI::UploadUniform2i, _strName, 1u, tempArr);
}

void Dystopia::Shader::UploadUniformi(char const* _strName, int _n1, int _n2, int _n3) const noexcept
{
	int tempArr[]{ _n1, _n2, _n3 };

	UploadUniformAux(mPrograms, &::Gfx::GraphicsAPI::UploadUniform3i, _strName, 1u, tempArr);
}

void Dystopia::Shader::UploadUniformi(char const* _strName, int _n1, int _n2, int _n3, int _n4) const noexcept
{
	int tempArr[]{ _n1, _n2, _n3, _n4 };

	UploadUniformAux(mPrograms, &::Gfx::GraphicsAPI::UploadUniform4i, _strName, 1u, tempArr);
}

void Dystopia::Shader::UploadUniform(char const * _strName, const Math::Vector2& _v) const noexcept
{
	UploadUniformAux(mPrograms, &::Gfx::GraphicsAPI::UploadUniform2f, _strName, 1u, reinterpret_cast<float const*>(&_v));
}

void Dystopia::Shader::UploadUniform3(char const* _strName, const Math::Vector4& _v) const noexcept
{
	UploadUniformAux(mPrograms, &::Gfx::GraphicsAPI::UploadUniform3f, _strName, 1u, reinterpret_cast<float const*>(&_v));
}

void Dystopia::Shader::UploadUniform(char const* _strName, const Math::Vector4& _v) const noexcept
{
	UploadUniformAux(mPrograms, &::Gfx::GraphicsAPI::UploadUniform4f, _strName, 1u, reinterpret_cast<float const*>(&_v));
}

void Dystopia::Shader::UploadUniform(char const* _strName, const Math::Matrix2& _m) const noexcept
{
	UploadUniformAux(mPrograms, &::Gfx::GraphicsAPI::UploadMatrix2, _strName, 1u, reinterpret_cast<float const*>(&_m), true);
}

void Dystopia::Shader::UploadUniform(char const* _strName, const Math::Matrix4& _m) const noexcept
{
	UploadUniformAux(mPrograms, &::Gfx::GraphicsAPI::UploadMatrix4, _strName, 1u, reinterpret_cast<float const*>(&_m), true);
}




