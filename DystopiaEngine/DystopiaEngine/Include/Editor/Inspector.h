/* HEADER *********************************************************************************/
/*!
\file	Inspector.h
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
#ifndef _INSPECTOR_H_
#define _INSPECTOR_H_
#include "EditorTab.h"

namespace Dystopia
{
	class GameObject;
	class BehaviourSystem;
	class Component;
	static constexpr size_t MAX_BUFFER_SIZE = 128;
	class Inspector : public EditorTab
	{
	public:
		static Inspector* GetInstance();
		~Inspector();

		/* Init() is called immediately after the creation of the object */
		virtual void Init() override;
		
		/* Update() is called before Window(), so alter most variables (frame based) here to be printed in Window() later */
		virtual void Update(const float&) override;

		/* Window() is where you do the EGUI/IMGUI functions. GUI variable changes will be recorded here */
		virtual void EditorUI() override;

		/* Shutdown() is called right before deleting this object */
		virtual void Shutdown() override;

		/* GetLabel() returns the string to identify this class. EditorTab requires this to create a tab for you using the label */
		virtual std::string GetLabel() const override;

		//void	SetFocus(GameObject&) override final;
		//void	RemoveFocus() override final;

	private:
		Inspector();

		BehaviourSystem	*mpBehaviourSys;
		GameObject		*mpFocus;
		std::string		mLabel;
		bool			mShowListOfComponents;
		bool			mPromptNewBehaviour;
		bool			mPromptCreateBehaviour;
		char			mBufferInput[MAX_BUFFER_SIZE];
		char			mBufferCreator[MAX_BUFFER_SIZE];
		char			mBufferLogin[MAX_BUFFER_SIZE];

		void			GameObjectDetails();
		void			GameObjectComponents();
		void			AddComponentButton(const Math::Vec2&);
		void			ComponentsDropDownList();
		void			AddBehaviourButton(const Math::Vec2&);
		void			BehaviourDropDownList();
		void			PromptCreateBehaviour();
		void			ConfirmCreateBehaviour();
		void			ResetBehaviourCreation();
		bool			RemoveComponent(Component*);
	};
}



#endif // _INSPECTOR_H_

#endif // EDITOR 


