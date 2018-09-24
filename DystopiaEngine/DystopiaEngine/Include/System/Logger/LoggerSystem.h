/* HEADER *********************************************************************************/
/*!
\file	LoggerSystem.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Idk what else this thing is supposed to do.jpg

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _LOGGERSYS_H_
#define _LOGGERSYS_H_

#include "System/Logger/LogPriority.h"

#include <cstdio>
#include <string>


namespace Dystopia
{
	class LoggerSystem
	{
	public:
		LoggerSystem(void) noexcept;
		~LoggerSystem(void) noexcept;

		static LoggerSystem* GetInstance(void) noexcept;

		void RedirectOutput(void(*)(const std::string&));
		void ParseInput(const std::string&);

		template <typename ... Ty>
		static void ConsoleLog(eLog, const char* _strFormat, Ty&& ...);

	private:

		eLog mActiveFlags;
		void(*mpOut)(const std::string&);

		template <typename ... Ty>
		void Write(const char*, Ty&&...);

		void SendOutput(const std::string&);
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename ... Ty>
void Dystopia::LoggerSystem::ConsoleLog(eLog _Mode, const char *_strFormat, Ty&& ...Args)
{
	LoggerSystem* self = LoggerSystem::GetInstance();
	if (nullptr == self)
	{
		std::terminate();
	}

	if (static_cast<unsigned>(self->mActiveFlags) & static_cast<unsigned>(_Mode))
	{
		self->Write(_strFormat, (Args)...);
	}
}

template <typename ... Ty>
void Dystopia::LoggerSystem::Write(const char *_strFormat, Ty&& ...Args)
{
	std::string buf;
	buf.reserve(2048);
	std::snprintf(&buf[0], 2048, _strFormat, Args...);
	SendOutput(buf);
}



#endif

