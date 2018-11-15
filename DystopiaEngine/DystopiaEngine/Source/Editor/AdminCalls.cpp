/* HEADER *********************************************************************************/
/*!
\file	AdminCalls.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/AdminCalls.h"
#include <regex>

namespace Dystopia
{
	namespace Admin
	{
		bool ValidCommand(const HashString& _toValidate, int& _outCount, HashString& _outParam, HashString& _outAdminFuncName)
		{
			static std::regex adminFormat{ "(.*)\\s(.*)\\s(.*)" };
			std::string parse{ _toValidate.c_str() };
			std::smatch match;
			int failPass = 3, iterations = 0, counter = -1;
			std::string var = "";
			while (std::regex_match(parse, match, adminFormat))
			{
				for (auto e : match)
				{
					switch (counter)
					{
					case 0:
						if (g_AdminFuncs.find(HashString{ std::string{ e }.c_str() }) != g_AdminFuncs.end())
						{
							_outAdminFuncName = HashString{ std::string{ e }.c_str() };
							failPass--;
						}
						break;
					case 1:
						try
						{
							iterations = std::stoi(std::string{ e });
							failPass--;
						}
						catch (...)
						{
						}
						break;
					case 2:
						var = std::string{ e };
						failPass--;
						break;
					}
					++counter;
				}
				parse = match.suffix().str();
			}

			if (failPass)
			{
				return false;
			}

			_outCount = iterations;
			_outParam = var.c_str();
			return true;
		}
	}
}

#endif //EDITOR
