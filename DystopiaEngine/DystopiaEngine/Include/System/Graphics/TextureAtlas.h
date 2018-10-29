/* HEADER *********************************************************************************/
/*!
\file	TextureAtlas.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Texture containing multiple textures

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _TEXTUREATLAS_H_
#define _TEXTUREATLAS_H_

#include "Math/MathFwd.h"
#include "DataStructure/AutoArray.h"

#include <string>


namespace Dystopia
{
	class Shader;
	class Texture;
	class TextSerialiser;

	class TextureAtlas
	{
	public:
		struct SubTexture
		{
			float uStart, vStart;
			float uEnd, vEnd;

			float mCol, mRow;
		};

		explicit TextureAtlas(Texture* = nullptr) noexcept;
		explicit TextureAtlas(const std::string&);
		~TextureAtlas(void) noexcept;

		void Bind(void) const noexcept;
		void Unbind(void) const noexcept;

		unsigned AddSection(const Math::Vec2& _vPos, unsigned _nWidth, unsigned _nHeight, unsigned _nCols = 1, unsigned _nRows = 1);

		void SetSection(unsigned _nID, unsigned _nCol, unsigned _nRow, Shader const&);

		void SetTexture(Texture* _pTexture);

		AutoArray<SubTexture>& GetAllSections(void) noexcept;

		std::string GetName(void) const;
		std::string const& GetPath(void) const noexcept;

		void SaveAtlas(TextSerialiser&) const noexcept;
		void LoadAtlas(TextSerialiser&) noexcept;

		Texture* GetInternal(void) const noexcept;

	private:

		Texture* mpTexture;
		AutoArray<SubTexture> mSections;
	};
}



#endif		// INCLUDE GUARD

