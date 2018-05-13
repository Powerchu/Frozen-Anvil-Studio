/* HEADER *********************************************************************************/
/*!
\file	TextSerialiser.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Encapsulates reading and writing to a text file.

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "IO\TextSerialiser.h"		// File Header

#include <limits>		// numeric_limit
#include <string>		// string
#include <fstream>		// fstream, ifstream, ofstream, streamsize


Dystopia::TextSerialiser::TextSerialiser(void) :
	mbBlockRead{ true }, mFile { }
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

void Dystopia::TextSerialiser::ConsumeBlock(void)
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
	TextSerialiser file;

	file.mFile.open(_strFilename, _nMode);

	return file;
}


