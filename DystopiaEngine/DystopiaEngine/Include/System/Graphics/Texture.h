/* HEADER *********************************************************************************/
/*!
\file	Texture.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Texture base class. 
	All data required to use the texture is contained within this class,
	Child classes are to be just wrappers to create different types of textures.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "DataStructure/HashString.h"

#include <string>


namespace Dystopia
{
	struct Image;

	class Texture
	{
	public:

		Texture(void) noexcept;
		explicit Texture(unsigned _nType, HashString const& _strPath) noexcept;
		explicit Texture(unsigned _nWidth, unsigned _nHeight, unsigned _nType) noexcept;

		Texture(Texture&&) noexcept;
		~Texture(void);

		void Bind(int = 0) const noexcept;
		void Unbind(void) const noexcept;

		unsigned GetWidth(void) const noexcept;
		unsigned GetHeight(void) const noexcept;

		void SetWidth(unsigned) noexcept;
		void SetHeight(unsigned) noexcept;
		void SetType(unsigned) noexcept;

		unsigned GetID(void) const noexcept;

		HashString GetName(void) const;
		HashString const& GetPath(void) const noexcept;

		Texture& operator= (Texture&&) noexcept;

	protected:

		HashString mstrPath;
		unsigned mnWidth, mnHeight, mnType, mnID;
	};
}



#endif		// INCLUDE GUARD

