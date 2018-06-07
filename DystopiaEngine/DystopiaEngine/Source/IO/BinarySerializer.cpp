/* HEADER *********************************************************************************/
/*!
\file	BinarySerializer.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "IO\BinarySerializer.h"
#include "DataStructure\Stack.h"
#include "Utility\Utility.h"
#include <iostream>

#include <limits>		// numeric_limit
#include <string>		// string
#include <fstream>		// fstream, ifstream, ofstream, streamsize

#include <iostream>

Dystopia::BinarySerializer::BinarySerializer(void) 
	: mFile{}, 
	mfpWrite{ Dystopia::System::GetSoftwareEndian() ? &Dystopia::BinarySerializer::WriteLE : &Dystopia::BinarySerializer::WriteBE },
	mfpRead{ Dystopia::System::GetSoftwareEndian() ? &Dystopia::BinarySerializer::ReadLE : &Dystopia::BinarySerializer::ReadBE }
{
}

Dystopia::BinarySerializer::BinarySerializer(std::fstream& _file) 
	: mFile{ Utility::Move(_file) },
	mfpWrite{ Dystopia::System::GetSoftwareEndian() ? &Dystopia::BinarySerializer::WriteLE : &Dystopia::BinarySerializer::WriteBE },
	mfpRead{ Dystopia::System::GetSoftwareEndian() ? &Dystopia::BinarySerializer::ReadLE : &Dystopia::BinarySerializer::ReadBE }
{
}

Dystopia::BinarySerializer::~BinarySerializer(void)
{
	mFile.close();
}

void Dystopia::BinarySerializer::WriteEndBlock(const std::string& _strName)
{
	//mFile << "\n[END_" << _strName << "]\n";
}

void Dystopia::BinarySerializer::WriteStartBlock(const std::string& _strName)
{
	//mFile << "[START_" << _strName << "]\n";
}

bool Dystopia::BinarySerializer::ReadStartBlock(void)
{
	//do {
	//	mFile.ignore(std::numeric_limits<std::streamsize>::max(), '[');
	//
	//	if (mFile.peek() == 'S')
	//	{
	//		mFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	//		return true;
	//	}
	//} while (mFile.good());

	return false;
}

void Dystopia::BinarySerializer::ReadEndBlock(void)
{
	//do {
	//	mFile.ignore(std::numeric_limits<std::streamsize>::max(), '[');
	//
	//	if (mFile.peek() == 'E')
	//	{
	//		mFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	//		break;
	//	}
	//} while (mFile.good());
}

bool Dystopia::BinarySerializer::Validate(void)
{
	//if (mFile.peek() == ',')
	//	mFile.ignore(1);
	//
	//return mFile.peek() != '\n';
}

Dystopia::BinarySerializer Dystopia::BinarySerializer::OpenFile(const std::string& _strFilename, int _nMode)
{
	std::fstream file;

	file.open(_strFilename, _nMode | std::ios_base::binary);

	if (file.fail())
		__debugbreak();

	return BinarySerializer{ file };
}

void Dystopia::BinarySerializer::WriteBE(const char * const &_pStart, const size_t _size)
{
	mFile.write(reinterpret_cast<const char*>(_pStart), _size);
}

void Dystopia::BinarySerializer::WriteLE(const char * const &_pStart, const size_t _size)
{
	size_t pos = _size;
	const char *pElement = nullptr;
	do
	{
		pElement = _pStart + (pos - 1);
		mFile.put(*pElement);
		pos--;
	} while (pos);
}

void Dystopia::BinarySerializer::ReadBE(char * const &_pStart, const size_t _size)
{
	mFile.read(_pStart, _size);
}

void Dystopia::BinarySerializer::ReadLE(char * const &_pStart, const size_t _size)
{
	Stack<char> tempStack{ static_cast<unsigned int>(_size) };
	char tempChar;

	size_t track = _size;
	do
	{
		mFile.read(&tempChar, 1);
		tempStack.Push(tempChar);
		track--;
	} while (track);

	while (!tempStack.IsEmpty()) 
	{
		tempChar = tempStack.Peek();
		tempStack.Pop();
		*(_pStart + track) = tempChar;
		track++;
	}
}

void Dystopia::BinarySerializer::ManualEndainOverride(eEndianess endian)
{
	switch (endian)
	{
	case eENDIAN_LITTLE:
		mfpWrite = &Dystopia::BinarySerializer::WriteLE;
		mfpRead = &Dystopia::BinarySerializer::ReadLE;
		break;
	case eENDIAN_BIG:
		mfpWrite = &Dystopia::BinarySerializer::WriteBE;
		mfpRead = &Dystopia::BinarySerializer::ReadBE;
		break;
	}
}

