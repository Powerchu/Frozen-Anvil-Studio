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

namespace Dystopia
{
	class TextSerialiser
	{
	public:
		static constexpr int MODE_READ = std::ios::in;
		static constexpr int MODE_WRITE = std::ios::out;

		TextSerialiser(TextSerialiser&&) = default;

		void InsertEndBlock(const std::string& = "");
		void InsertStartBlock(const std::string& = "");

		void ConsumeStartBlock(void);
		void ConsumeEndBlock(void);

		static TextSerialiser OpenFile(const std::string&, int = MODE_READ);

		template <typename T>
		TextSerialiser& Read(const T&);

		template <typename T>
		TextSerialiser& Write(const T&);

		~TextSerialiser(void);

	private:

		bool mbBlockRead;
		std::fstream mFile;

		explicit TextSerialiser(void);
		TextSerialiser(const TextSerialiser&) = delete; // Disallow copying!

		void Validate(void);
	};
}

template <typename T>
Dystopia::TextSerialiser& operator << (Dystopia::TextSerialiser&, const T&);

template <typename T>
Dystopia::TextSerialiser& operator >> (Dystopia::TextSerialiser&, const T&);






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename T>
Dystopia::TextSerialiser& Dystopia::TextSerialiser::Write(const T& _rhs)
{
	mFile << _rhs << ',';

	return *this;
}

template <typename T>
Dystopia::TextSerialiser& Dystopia::TextSerialiser::Read(const T& _rhs)
{
	if (!mbBlockRead)
		mFile >> _rhs;
	else
		_rhs = T{};

	Validate();
	return *this;
}

template <typename T>
Dystopia::TextSerialiser& operator << (Dystopia::TextSerialiser& _file, const T& _rhs)
{
	return _file.Write(_rhs);
}

template <typename T>
Dystopia::TextSerialiser& operator >> (Dystopia::TextSerialiser& _file, const T& _rhs)
{
	return _file.Read(_rhs);
}



#endif		// INCLUDE GUARD

