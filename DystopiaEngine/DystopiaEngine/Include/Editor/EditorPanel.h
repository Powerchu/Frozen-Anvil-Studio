/* HEADER *********************************************************************************/
/*!
\file	EditorPanel.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_PANEL_H_
#define _EDITOR_PANEL_H_
#include "DataStructure/HashString.h"
#include "Editor/EditorMessages.h"
#include "IO/TextSerialiser.h"
#include "Math/Vector2.h"

namespace Editor
{
	static constexpr size_t MAX_SEARCH = 256;
	class EditorPanel
	{
	public:
		/************************** Virtual functions to implement *************************/

		virtual void Load(void) = 0;
		virtual bool Init(void) = 0;
		virtual void Update(float) = 0;
		virtual void EditorUI(void) = 0;
		virtual void Shutdown(void) = 0;
		virtual void Message(eEMessage) = 0;
		virtual void SaveSettings(Dystopia::TextSerialiser& _out) const = 0;
		virtual void LoadSettings(Dystopia::TextSerialiser& _in) = 0;
		virtual HashString GetLabel(void) const = 0;
		virtual ~EditorPanel(void) {}


		/******************************* General panel stuff *******************************/

		bool IsScrollEnabled(void) const { return mbScrollEnabled; }
		void SetScrollEnabled(bool _b) { mbScrollEnabled = _b; }
		bool IsOpened(void) const { return mbOpened; }
		void SetOpened(bool _b) { mbOpened = _b; }
		bool& GetOpenedBool(void) { return mbOpened; }
		Math::Vec2 Size() const { return mSize; }
		Math::Vec2 Position() const { return mPos; }

		void SetSize(const Math::Vec2& _s) { mSize = _s; }
		void SetPosition(const Math::Vec2& _p) { mPos = _p; }

	private:
		bool mbOpened = false;
		bool mbScrollEnabled = true;
		Math::Vec2 mSize;
		Math::Vec2 mPos;
	};
	
}


#endif 
#endif










