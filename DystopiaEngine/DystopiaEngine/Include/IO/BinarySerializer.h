/* HEADER *********************************************************************************/
/*!
\file	BinarySerializer.h
\author Digipen (100%)
\par    email:
\@digipen.edu
\brief
reference usage of text brought over as reference for structural
TextSer.. = Text::OpenFile("");
InsertStartBlock("ME");
tetxser.. << variable;
tetxser.. << variable;
tetxser.. << variable;
tetxser.. << variable;
InsertEndBlock("End ME");

TextSer.. = Text::OpenFile("");
ConsumeStart();
tetxser.. >> variable;
tetxser.. >> variable;
tetxser.. >> variable;
tetxser.. >> variable;
ConsumeEnd();

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _BINARY_SERIALIZER_H_
#define _BINARY_SERIALIZER_H_
#include <cstdint>
#include <limits>		// numeric_limit
#include <string>		// string
#include <fstream>		// fstream, ifstream, ofstream, streamsize

namespace Dystopia
{
	enum eEndianess
	{
		eENDIAN_LITTLE,
		eENDIAN_BIG
	};

	class BinarySerializer
	{
	public:
		static constexpr int MODE_READ = std::ios::in;
		static constexpr int MODE_WRITE = std::ios::out;

		BinarySerializer(BinarySerializer&&) = default;

		void InsertEndBlock(const std::string& = "");
		void InsertStartBlock(const std::string& = "");

		void ConsumeStartBlock(void);
		void ConsumeEndBlock(void);

		static BinarySerializer OpenFile(const std::string&, int = MODE_READ);

		template <typename T>
		BinarySerializer& Read(const T&);

		template <typename T>
		BinarySerializer& Write(const T&);

		~BinarySerializer(void);

	private:
		typedef void(BinarySerializer::*MemFnPtr)(const char * const &, const size_t);

		bool mbBlockRead;
		std::fstream mFile;
		MemFnPtr mfpWrite;

		void WriteBE(const char * const &, const size_t);
		void WriteLE(const char * const &, const size_t);

		explicit BinarySerializer(void);
		BinarySerializer(const BinarySerializer&) = delete; // Disallow copying!

		void Validate(void);
	};

	namespace System
	{
		inline eEndianess GetSoftwareEndian()
		{
			std::int16_t subject = 0x5501;
			std::int8_t test = *((std::int8_t*)&subject);
			return (test == 1) ? eEndianess::eENDIAN_LITTLE : eEndianess::eENDIAN_BIG;
		}
	}
}

template <typename T>
Dystopia::BinarySerializer& operator << (Dystopia::BinarySerializer&, const T&);

template <typename T>
Dystopia::BinarySerializer& operator >> (Dystopia::BinarySerializer&, const T&);


// ============================================ FUNCTION DEFINITIONS ============================================ // 

template <typename T>
Dystopia::BinarySerializer& Dystopia::BinarySerializer::Write(const T& _rhs)
{
	//mFile << _rhs << ',';
	size_t size = sizeof(_rhs);
	(this->*mfpWrite)(reinterpret_cast<const char*>(&_rhs), size);
	return *this;
}

template <typename T>
Dystopia::BinarySerializer& Dystopia::BinarySerializer::Read(const T& _rhs)
{
	//if (!mbBlockRead)
	//	mFile >> _rhs;
	//else
	//	_rhs = T{};
	//
	//Validate();


	return *this;
}

template <typename T>
Dystopia::BinarySerializer& operator << (Dystopia::BinarySerializer& _file, const T& _rhs)
{
	return _file.Write(_rhs);
}

template <typename T>
Dystopia::BinarySerializer& operator >> (Dystopia::BinarySerializer& _file, const T& _rhs)
{
	return _file.Read(_rhs);
}












#endif //_BINARY_SERIALIZER_H_




