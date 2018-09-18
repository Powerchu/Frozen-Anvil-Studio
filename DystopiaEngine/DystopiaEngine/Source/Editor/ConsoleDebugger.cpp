/* HEADER *********************************************************************************/
/*!
\file	ConsoleDebugger.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor\ConsoleDebugger.h"
#include "Editor\ConsoleLog.h"
namespace Dystopia
{
	void format_impl(std::stringstream& _stream, const char * _format)
	{
		while (*_format)
		{
			if (*_format == '%' && *++_format != '%')
				throw std::invalid_argument("not enough arguments!\n");
			_stream << *_format++;
		}
	}

	void ConsolePrint(const std::string& _text)
	{
		Dystopia::PrintToConsoleLog(_text);
	}

}


#endif //EDITOR
