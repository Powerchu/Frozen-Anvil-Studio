/* HEADER *********************************************************************************/
/*!
\file	TextSerialiser.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Encapsulates reading and writing to a text file.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "IO\TextSerialiser.h"		// File Header
#include "Utility\Utility.h"

#include <limits>		// numeric_limit
#include <string>		// string
#include <fstream>		// fstream, ifstream, ofstream, streamsize

#include <iostream>

Dystopia::TextSerialiser::TextSerialiser(void) :
	mbBlockRead{ true }, mFile { }
{

}

Dystopia::TextSerialiser::TextSerialiser(std::fstream& _file) :
	mbBlockRead{ false }, mFile{ Utility::Move(_file) }
{

}

Dystopia::TextSerialiser::~TextSerialiser(void)
{
	mFile.close();
}

void Dystopia::TextSerialiser::InsertEndBlock(const std::string& _strName)
{
	mFile << "\n[END_" << _strName << "]\n";
}

void Dystopia::TextSerialiser::InsertStartBlock(const std::string& _strName)
{
	mFile << "[START_" << _strName << "]\n";
}

void Dystopia::TextSerialiser::ConsumeStartBlock(void)
{
	do {
		mFile.ignore(std::numeric_limits<std::streamsize>::max(), '[');

		if (mFile.peek() == 'S')
		{
			mFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			mbBlockRead = false;
			break;
		}
	} while (mFile.good());
}

void Dystopia::TextSerialiser::ConsumeEndBlock(void)
{
	do {
		mFile.ignore(std::numeric_limits<std::streamsize>::max(), '[');

		if (mFile.peek() == 'E')
		{
			mFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			break;
		}
	} while (mFile.good());
}

bool Dystopia::TextSerialiser::EndOfInput(void) const
{
	return mbBlockRead;
}

void Dystopia::TextSerialiser::Validate(void)
{
	if (mFile.peek() == ',')
		mFile.ignore(1);

	if (mFile.peek() == '\n')
	{
		mbBlockRead = true;
	}
}

Dystopia::TextSerialiser Dystopia::TextSerialiser::OpenFile(const std::string& _strFilename, int _nMode)
{
	std::fstream file;

	file.open(_strFilename, _nMode);

	if (file.fail())
		__debugbreak();

	return TextSerialiser{ file };
}


