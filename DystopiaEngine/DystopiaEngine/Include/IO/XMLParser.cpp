/* HEADER *********************************************************************************/
/*!
\file	XMLParser.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Reads XMLs

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "XMLParser.h"
#include "Allocator/DefaultAlloc.h"

#include <fstream>


void Dystopia::XMLParser::Parse(char const* _strFile)
{
	using ios = std::ios;
	using Alloc_t = Dystopia::DefaultAllocator<char[]>;
	std::basic_ifstream<char> file{ _strFile, ios::binary };

	if (file.bad())
	{
		__debugbreak();
		return;
	}

	file.seekg(0, ios::end);
	auto sz = static_cast<size_t>(file.tellg());
	char* buf = Alloc_t::Alloc(1 + sz);

	file.seekg(0);
	file.read(buf, sz);
	buf[sz] = '\0';

	file.close();
}

