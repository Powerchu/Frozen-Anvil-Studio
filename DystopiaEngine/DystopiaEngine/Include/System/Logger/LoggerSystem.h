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

#include "System\Driver\Driver.h"
#include "System\Logger\LogPriority.h"

#include <cstdio>
#include <string>


namespace Dystopia
{
	class LoggerSystem
	{
	public:
		LoggerSystem(void) noexcept;
		~LoggerSystem(void) noexcept;

		void RedirectOutput();
		void RedirectInput();

		template <typename ... Ty>
		static void ConsoleLog(eLog, const char* _strFormat, Ty&& ...);

	private:

		eLog mActiveFlags;

		template <typename ... Ty>
		void Write(const char*, Ty&&...);

		void SendOutput(const std::string&);
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename ... Ty>
void Dystopia::LoggerSystem::ConsoleLog(eLog _Mode, const char *_strFormat, Ty&& ...Args)
{
	LoggerSystem* self = EngineCore::GetInstance()->GetSubSystem<LoggerSystem>();
	if (nullptr == self)
	{
		std::terminate();
	}

	if (static_cast<unsigned>(self->mActiveFlags) & static_cast<unsigned>(_Mode))
	{
		self->Write(_strFormat, Utility::Forward<Ty>(Args)...);
	}
}

template <typename ... Ty>
void Dystopia::LoggerSystem::Write(const char *_strFormat, Ty&& ...Args)
{
	char buf[4096];
	std::snprintf(buf, sizeof(buf), _strFormat, Utility::Forward<Ty>(Args)...);
	SendOutput(buf);
}



#endif

