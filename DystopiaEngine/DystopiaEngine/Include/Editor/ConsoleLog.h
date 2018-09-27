/* HEADER *********************************************************************************/
/*!
\file	ConsoleLog.h
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
#ifndef _CONSOLE_LOG_H_
#define _CONSOLE_LOG_H_

#include "Globals.h"
#include "EditorTab.h"
#include "DataStructure\Array.h"


namespace Dystopia
{
	void _DLL_EXPORT PrintToConsoleLog(const std::string& _text);
	void _DLL_EXPORT PrintToConsoleLog(const char* _text);

	class ConsoleLog : public EditorTab
	{
	public:
		static ConsoleLog* GetInstance();
		~ConsoleLog();

		void			Init()						override;
		void			Update(const float&)		override;
		void			EditorUI()					override;
		void			Shutdown()					override;
		std::string		GetLabel() const			override;

		void			Debug(const std::string&);
		void			Clear();

	private:
		ConsoleLog(void);

		void PrintLogs();

		static constexpr unsigned int	maxLog = 50;
		unsigned int					mLoggingIndex;
		unsigned int					mRecordIndex;
		std::string						mLabel;
		Array<std::string, maxLog>		mArrDebugTexts;
	};
}




#endif //_CONSOLE_LOG_H_
#endif //EDITOR
