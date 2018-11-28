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

#include "DataStructure/MagicArray.h"

#include <map>


namespace Dystopia
{
	class Shader;

	class ShaderSystem
	{
	public:

		~ShaderSystem(void) noexcept;

		void EditorUpdate(void);


	private:

		MagicArray<Shader> mPrograms;
	};
}



#endif		// INCLUDE GUARD

