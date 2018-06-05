/* HEADER *********************************************************************************/
/*!
\file	BinarySerializer.cpp
\author Digipen (100%)
\par    email:
\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "IO\BinarySerializer.h"
#include <iostream>

Dystopia::BinarySerializer::BinarySerializer(void) :
	mbBlockRead{ true }, mFile{}, mfpWrite{ Dystopia::System::GetSoftwareEndian() ? &Dystopia::BinarySerializer::WriteLE : &Dystopia::BinarySerializer::WriteBE }
{

}

Dystopia::BinarySerializer::~BinarySerializer(void)
{
	mFile.close();
}

void Dystopia::BinarySerializer::InsertEndBlock(const std::string& _strName)
{
	//mFile << "\n[END_" << _strName << "]\n";
}

void Dystopia::BinarySerializer::InsertStartBlock(const std::string& _strName)
{
	//mFile << "[START_" << _strName << "]\n";
}

void Dystopia::BinarySerializer::ConsumeStartBlock(void)
{
	//do {
	//	mFile.ignore(std::numeric_limits<std::streamsize>::max(), '[');
	//
	//	if (mFile.peek() == 'S')
	//	{
	//		mFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	//		break;
	//	}
	//} while (mFile.good());
}

void Dystopia::BinarySerializer::ConsumeEndBlock(void)
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

void Dystopia::BinarySerializer::Validate(void)
{
	//if (mFile.peek() == ',')
	//	mFile.ignore(1);
	//
	//if (mFile.peek() == '\n')
	//{
	//	mbBlockRead = true;
	//}
}

Dystopia::BinarySerializer Dystopia::BinarySerializer::OpenFile(const std::string& _strFilename, int _nMode)
{
	BinarySerializer file;
	//file.mFile.open(_strFilename, _nMode);
	file.mFile.open(_strFilename, std::ios_base::out | std::ios_base::binary);
	return file;
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
