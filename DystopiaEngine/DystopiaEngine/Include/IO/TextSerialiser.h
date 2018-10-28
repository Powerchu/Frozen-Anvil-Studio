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
#include "DataStructure/HashString.h"
#include "IO/Serialiser.h"

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
inline void Dystopia::TextSerialiser::ApplyWrite(const T& _rhs)
{
	mFile << _rhs << ',';
}

template <>
inline void Dystopia::TextSerialiser::ApplyWrite(const HashString & _rhs)
{
	std::string str{ _rhs.cbegin(), _rhs.cend() };
	mFile << str << ',';
}

template <typename T>
void Dystopia::TextSerialiser::ApplyRead(T& _rhs)
{
	mFile >> _rhs;
}

template <>
inline void Dystopia::TextSerialiser::ApplyRead<std::string>(std::string& _rhs)
{
	std::getline(mFile, _rhs, ',');
}

template <>
inline void Dystopia::TextSerialiser::ApplyRead<HashString>(HashString & _rhs)
{
	std::string str;
	std::getline(mFile, str, ',');
	_rhs = HashString{str.c_str()};
}

template <>
inline void Dystopia::TextSerialiser::ApplyRead<bool>(bool& _rhs)
{
	int ch;
	mFile >> ch;

	_rhs = !!ch;
}



#endif		// INCLUDE GUARD

