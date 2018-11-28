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
#include "IO/TextSerialiser.h"		// File Header
#include "Utility/Utility.h"
#include "DataStructure/HashString.h"

#include <limits>		// numeric_limit
#include <string>		// string
#include <fstream>		// fstream, ifstream, ofstream, streamsize

#include <iostream>

Dystopia::TextSerialiser::TextSerialiser(void) :
	mFile { }
{

}

Dystopia::TextSerialiser::TextSerialiser(std::fstream& _file) :
	mFile{ Ut::Move(_file) }
{

}

Dystopia::TextSerialiser::~TextSerialiser(void)
{
	mFile.close();
}


void Dystopia::TextSerialiser::WriteEndBlock(char const* _strName)
{
	mFile << "\n[END_" << _strName << "]\n";
}

void Dystopia::TextSerialiser::WriteStartBlock(char const* _strName)
{
	mFile << "[START_" << _strName << "]\n";
}


bool Dystopia::TextSerialiser::ReadStartBlock(void)
{
	do {
		mFile.ignore(std::numeric_limits<std::streamsize>::max(), '[');

		if (mFile.peek() == 'S')
		{
			mFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			return true;
		}
	} while (mFile.good());

	return false;
}

void Dystopia::TextSerialiser::ReadEndBlock(void)
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

bool Dystopia::TextSerialiser::Validate(void)
{
	if (mFile.peek() == ',')
		mFile.ignore(1);

	return mFile.peek() != '\n' && mFile.good();
}


Dystopia::TextSerialiser Dystopia::TextSerialiser::OpenFile(char const* _strFilename, int _nMode)
{
	std::fstream file;

	file.open(_strFilename, _nMode);

	if (file.fail())
		__debugbreak();

	return TextSerialiser{ file };
}


