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

#include "DataStructure/Tuple.h"
#include "DataStructure/Variant.h"
#include "DataStructure/OString.h"
#include "DataStructure/AutoArray.h"


namespace Dystopia
{
	class Texture;

	class Shader
	{
	public:

		Shader(OString const& = "") noexcept;
		~Shader(void);

		//void CreateShader(char const* _strVert, char const* _strFrag);
		//void CreateShader(char const* _strVert, char const* _strFrag, char const* _strGeo);
		
		void AttachProgram(ShaderProgram*);
		void DetachProgram(ShaderProgram*);
		void DetachProgram(::Gfx::ShaderStage);
		void ReattachProgram(ShaderProgram*);

		void Bind(void) const;
		void Unbind(void) const;

		void UploadUniform(char const*, const Math::Vector2&) const noexcept;
		void UploadUniform(char const*, const Math::Vector4&) const noexcept;
		void UploadUniform(char const*, const Math::Matrix2&) const noexcept;
		void UploadUniform(char const*, const Math::Matrix4&) const noexcept;
		void UploadUniform3(char const*, const Math::Vector4&) const noexcept;

		void UploadUniform(char const*, float) const noexcept;
		void UploadUniform(char const*, float, float) const noexcept;
		void UploadUniform(char const*, float, float, float) const noexcept;
		void UploadUniform(char const*, float, float, float, float) const noexcept;

		void UploadUniformi(char const*, int) const noexcept;
		void UploadUniformi(char const*, int, int) const noexcept;
		void UploadUniformi(char const*, int, int, int) const noexcept;
		void UploadUniformi(char const*, int, int, int, int) const noexcept;

		OString const& GetName(void) const noexcept;

		void OnEditorUI(void) const;

		AutoArray<std::pair<OString, ::Gfx::eUniform_t>> const& GetVariables(void) noexcept;

	private:

		void ImportVariables(void) noexcept;

		OString mstrName;
		::Gfx::ShaderPipeline mID;
		::Gfx::ShaderStage mStages;
		AutoArray<ShaderProgram*> mPrograms;

		bool mbUpdate;
		AutoArray<std::pair<OString, ::Gfx::eUniform_t>> mVars;
	};
}



#endif		// INCLUDE GUARD

