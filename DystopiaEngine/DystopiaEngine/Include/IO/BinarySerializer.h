/* HEADER *********************************************************************************/
/*!
\file	BinarySerializer.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
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
#include <string>
#include <fstream>

#include "IO/Serialiser.h"

namespace Dystopia
{
	enum eEndianess
	{
		eENDIAN_LITTLE,
		eENDIAN_BIG
	};

	class BinarySerializer : public SerialiserBase<BinarySerializer>
	{
	public:
		friend class SerialiserBase<BinarySerializer>;

		BinarySerializer(BinarySerializer&&) = default;
		~BinarySerializer(void);

		static BinarySerializer OpenFile(const std::string&, int = MODE_READ);
		void ManualEndainOverride(eEndianess);

		void Skip(size_t _numOfBytes);

	private:

		explicit BinarySerializer(void);
		explicit BinarySerializer(std::fstream&);
		BinarySerializer(const BinarySerializer&) = delete; // Disallow copying!

		typedef void(BinarySerializer::*MemFnPtr)(const char * const &, const size_t);
		typedef void(BinarySerializer::*MemFnPtr2)(char * const &, const size_t);

		std::fstream	mFile;
		bool			mbBlockRead;
		MemFnPtr		mfpWrite;
		MemFnPtr2		mfpRead;

		bool			Validate(void);
		bool			ReadStartBlock(void);
		void			ReadEndBlock(void);
		void			WriteEndBlock(const std::string&);
		void			WriteStartBlock(const std::string&);
		void			WriteBE(const char * const &, const size_t);
		void			WriteLE(const char * const &, const size_t);
		void			ReadBE(char * const &, const size_t);
		void			ReadLE(char * const &, const size_t);

		template <typename T>
		void ApplyRead(T&);
		template <typename T>
		void ApplyWrite(const T&);
	};

	namespace Hardware
	{
		inline eEndianess GetSoftwareEndian()
		{
			std::int16_t subject = 0x5501;
			std::int8_t test = *((std::int8_t*)&subject);
			return (test == 1) ? eEndianess::eENDIAN_LITTLE : eEndianess::eENDIAN_BIG;
		}
	}
}


// ============================================ FUNCTION DEFINITIONS ============================================ // 

template <typename T>
void Dystopia::BinarySerializer::ApplyWrite(const T& _rhs)
{
	(this->*mfpWrite)(reinterpret_cast<const char*>(&_rhs), sizeof(_rhs));
}

template <typename T>
void Dystopia::BinarySerializer::ApplyRead(T& _rhs)
{
	(this->*mfpRead)(reinterpret_cast<char*>(&_rhs), sizeof(_rhs));
}





#endif //_BINARY_SERIALIZER_H_




