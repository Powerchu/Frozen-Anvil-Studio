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


	private:

		unsigned mnShaderID;

		void LoadShader(unsigned, const std::string&);
	};
}



#endif		// INCLUDE GUARD

