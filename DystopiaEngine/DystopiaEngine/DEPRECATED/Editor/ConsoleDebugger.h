/* HEADER *********************************************************************************/
/*!
\file	ConsoleDebugger.h
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
#ifndef _CONSOLE_DEBUGGER_H_
#define _CONSOLE_DEBUGGER_H_
#include <sstream>
#include <string>
#include <iostream>
#include <type_traits>
#include <exception>

void format_impl(std::stringstream&, const char *);

template<typename Arg, typename ... Args>
void format_impl(std::stringstream& _stream, const char * _format, Arg arg, Args ... args)
{
	while (*_format)
	{
		if (*_format == '%' && *++_format != '%')
		{
			auto current_fmt = *_format;
			switch (current_fmt)
			{
			case 'd':
				if (!std::is_integral<Arg>())
					throw std::invalid_argument("%d introduces integral argument");
				break;
			case 'f':
				if (!std::is_floating_point<Arg>())
					throw std::invalid_argument("%f introduces floating argument");
				break;
			}
			_stream << arg;
			return format_impl(_stream, ++_format, args ...);
		}
		_stream << *_format++;
	}
	throw std::invalid_argument("Too many args");
}

template<typename ... Args>
std::string format(const char * _format, Args ... args)
{
	std::stringstream ss;
	format_impl(ss, _format, args ...);
	return ss.str();
}

void ConsolePrint(const std::string&);

template<typename ... Args>
void ConsolePrint(const char * _format, Args ... args)
{
	ConsolePrint(format(_format, args ...));
}




#endif //_CONSOLE_DEBUGGER_H_
#endif // EDITOR
