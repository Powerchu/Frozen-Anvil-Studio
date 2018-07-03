/* HEADER *********************************************************************************/
/*!
\file	EditorTab.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_TAB_H_
#define _EDITOR_TAB_H_
#include "Math\Vector4.h"
#include "DataStructure\AutoArray.h"
#include <string>

namespace Dystopia
{
	class EditorTab
	{
	public:
		EditorTab();
		~EditorTab();

		/************************************************************************************************************/
		/* TODO: Implement your own definitions for the pure virtual functions in your class that needs to be a tab */
		virtual void Init() = 0;
		virtual void Update(const float&) = 0;
		virtual void Window() = 0;
		virtual void Shutdown() = 0;
		virtual std::string GetLabel() const = 0;
		/************************************************************************************************************/

		void SetSize(const Math::Vec4&);
		void SetSize(const float&, const float&);
		
	private:
		Math::Vec4 mSize;
	};
}

#endif //_EDITOR_TAB_H_
#endif // EDITOR
