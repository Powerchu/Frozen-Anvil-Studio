/* HEADER *********************************************************************************/
/*!
\file	FileLogger.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Implmentation of FileLogger

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Logger/FileLogger.h"

#include <cstdio>


Dystopia::FileLogger::FileLogger(void * const _p) noexcept
	: mFile{ static_cast<FILE*>(_p) }
{
}

#pragma warning(push)
#pragma warning(disable : 4996)
Dystopia::FileLogger::FileLogger(const std::string& _strFilename, const char* _strMode) noexcept
	: mFile{ std::fopen(_strFilename.c_str(), _strMode) }
{
}
#pragma warning(pop)

Dystopia::FileLogger::~FileLogger(void) noexcept
{
	if (mFile)
	{
		std::fclose(mFile);
		mFile = nullptr;
	}
}

bool Dystopia::FileLogger::Good(void) const noexcept
{
	return nullptr != mFile;
}


