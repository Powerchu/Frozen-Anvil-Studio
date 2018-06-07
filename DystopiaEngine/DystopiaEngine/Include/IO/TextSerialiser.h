/* HEADER *********************************************************************************/
/*!
\file	TextSerialiser.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Encapsulates reading and writing to a text file.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _TEXT_SERIALISER_H_
#define _TEXT_SERIALISER_H_

#include <string>
#include <fstream>

#include "IO\Serialiser.h"

namespace Dystopia
{
	class TextSerialiser : public SerialiserBase<TextSerialiser>
	{
	public:
		friend class SerialiserBase<TextSerialiser>;

		TextSerialiser(TextSerialiser&&) = default;
		~TextSerialiser(void);

		static TextSerialiser OpenFile(const std::string&, int = MODE_READ);


	private:
		std::fstream mFile;

		explicit TextSerialiser(void);
		explicit TextSerialiser(std::fstream&);
		TextSerialiser(const TextSerialiser&) = delete; // Disallow copying!

		void ReadEndBlock(void);
		bool ReadStartBlock(void);

		void WriteEndBlock(const std::string&);
		void WriteStartBlock(const std::string&);

		template <typename T>
		void ApplyRead(T&);
		template <typename T>
		void ApplyWrite(const T&);

		bool Validate(void);
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename T>
void Dystopia::TextSerialiser::ApplyWrite(const T& _rhs)
{
	mFile << _rhs << ',';
}

template <typename T>
void Dystopia::TextSerialiser::ApplyRead(T& _rhs)
{
	mFile >> _rhs;
}



#endif		// INCLUDE GUARD

