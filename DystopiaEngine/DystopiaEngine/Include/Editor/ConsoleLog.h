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
//#include "EditorTab.h"
#include "Editor/EditorPanel.h"
#include "DataStructure/Array.h"
#include "DataStructure/HashString.h"
#define MAX_CLOG 256

void _DLL_EXPORT PrintToConsoleLog(const std::string& _text);
void _DLL_EXPORT PrintToConsoleLog(const HashString& _text);
void _DLL_EXPORT PrintToConsoleLog(const char* _text);

namespace Editor//Dystopia
{
	class ConsoleLog : public EditorPanel //EditorTab
	{
	public:
		//static ConsoleLog* GetInstance();
		//~ConsoleLog();

		//void			Init()						override;
		//void			Update(const float&)		override;
		//void			EditorUI()					override;
		//void			Shutdown()					override;
		//std::string		GetLabel() const			override;

		ConsoleLog(void);
		~ConsoleLog(void);
		void Load(void);
		bool Init(void);
		void Update(float);
		void EditorUI(void);
		void Shutdown(void);
		void Message(eEMessage);
		void SaveSettings(Dystopia::TextSerialiser& _out) const;
		void LoadSettings(Dystopia::TextSerialiser& _in);
		const HashString& GetLabel(void) const;

		void SetIsClearedOnPlay(bool _b);
		void Debug(const HashString&);
		void Clear(void);
		bool GetIsClearedOnPlay(void) const;

	private:
		void PrintLogs(void);
		void AdminInput(void);

		unsigned int					mnLoggingIndex;
		unsigned int					mnRecordIndex;
		HashString						mLabel;
		char							mAdminCommands[MAX_SEARCH];
		Array<HashString, MAX_CLOG>		mArrDebugTexts;
		unsigned int					mnScrollToBottom;
		bool							mbClearOnPlay;
	};
}




#endif //_CONSOLE_LOG_H_
#endif //EDITOR
