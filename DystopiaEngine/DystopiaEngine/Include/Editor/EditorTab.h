/* HEADER *********************************************************************************/
/*!
\file	EditorTab.h
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
#ifndef _EDITOR_TAB_H_
#define _EDITOR_TAB_H_
#include "Math\Vector2.h"
#include <string>

namespace Dystopia
{
	static constexpr size_t MAX_SEARCH = 128;
	class GameObject;
	class EditorEventHandler;
	class CommandHandler;
	class Scene;
	class Editor;

	class EditorTab
	{
	public:
		EditorTab(bool _defaultOpen = false);
		virtual ~EditorTab();

		/************************************************************************************************************/
		/* TODO: Implement your own definitions for the pure virtual functions in your class that needs to be a tab */
		/* Init() is called immediately after the creation of the object */
		virtual void		Init() = 0;
		/* Update() is called before Window(), so alter most variables (frame based) here to be printed in Window() later */
		virtual void		Update(const float&) = 0;
		/* Window() is where you do the EGUI/IMGUI functions. GUI variable changes will be recorded here */
		virtual void		EditorUI() = 0;
		/* Shutdown() is called right before deleting this object */
		virtual void		Shutdown() = 0;
		/* GetLabel() returns the string to identify this class. EditorTab requires this to create a tab for you using the label */
		virtual std::string GetLabel() const = 0;
		/************************************************************************************************************/

		virtual void		SetFocus(GameObject&);
		virtual void		RemoveFocus();

		void				SetSize(const Math::Vec2&);
		void				SetSize(const float&, const float&);
		void				SetPosition(const Math::Vec2&);
		void				SetPosition(const float&, const float&);
		void				SetComdContext(CommandHandler * const);
		void				SetEventSysContext(EditorEventHandler * const);
		void				SetSceneContext(Scene * const);
		Math::Vec2			Size() const;
		Math::Vec2			Position() const;
		bool*				GetOpenedBool();
		
	private:
		Math::Vec2			mSize;
		Math::Vec2			mPos;
		bool				mIsOpened;

	protected:
		Editor&				GetMainEditor() const;

		CommandHandler		*mpComdHandler;
		EditorEventHandler	*mpEditorEventSys;
		Scene				*mpCurrentScene;
	};
}

#endif //_EDITOR_TAB_H_
#endif // EDITOR

