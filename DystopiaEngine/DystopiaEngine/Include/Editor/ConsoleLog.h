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
#include "EditorTab.h"

namespace Dystopia
{
	class ConsoleLog : public EditorTab
	{
	public:
		static ConsoleLog* GetInstance();
		~ConsoleLog();

		virtual void Init() override;
		virtual void Update(const float&) override;
		virtual void EditorUI() override;
		virtual void Shutdown() override;
		virtual std::string GetLabel() const override;


	private:
		ConsoleLog(void);
		
		std::string mLabel;
	
	
	};
}




#endif //_CONSOLE_LOG_H_
#endif //EDITOR
