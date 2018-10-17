/* HEADER *********************************************************************************/
/*!
\file	FileLogger.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Writes output to file

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _FILELOGGER_H_
#define _FILELOGGER_H_

#include "Utility/Utility.h"

#include <string>
#include <cstdio>


namespace Dystopia
{
	class FileLogger
	{
	public:
		explicit FileLogger(void* const) noexcept;
		explicit FileLogger(const std::string&, const char* _strMode = "rw") noexcept;
		~FileLogger(void) noexcept;

		template <typename ... Ty>
		void Write(const char*, Ty&&...) noexcept;

		bool Good(void) const noexcept;

	private:

		FILE* mFile;
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename ... Ty>
void Dystopia::FileLogger::Write(const char *_strFormat, Ty&& ...Args) noexcept
{
	if (mFile)
	{
		std::fprintf(mFile, _strFormat, Ut::Forward<Ty>(Args)...);
	}
}



#endif

