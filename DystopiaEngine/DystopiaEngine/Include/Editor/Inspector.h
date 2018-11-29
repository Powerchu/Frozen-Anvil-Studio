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
//#include "EditorTab.h"
#include "Editor/EditorPanel.h"
#include "DataStructure/HashString.h"

namespace Dystopia
{
	class GameObject;
	class BehaviourSystem;
	class Behaviour;
	class Component;
}

namespace Editor //Dystopia
{
	static constexpr size_t MAX_BUFFER_SIZE = 128;
	class Inspector : public EditorPanel //public EditorTab
	{
	public:
		Inspector(void);
		~Inspector(void);

		void Load(void);
		bool Init(void);
		void Update(float);
		void EditorUI(void);
		void Shutdown(void);
		void Message(eEMessage);
		void SaveSettings(Dystopia::TextSerialiser& _out) const;
		void LoadSettings(Dystopia::TextSerialiser& _in);
		HashString GetLabel(void) const;

	private:

		Dystopia::BehaviourSystem	*mpBehaviourSys;
		Dystopia::GameObject		*mpFocus;

		HashString		mLabel;
		bool			mShowListOfComponents;
		bool			mPromptNewBehaviour;
		bool			mPromptCreateBehaviour;
		char			mBufferInput[MAX_BUFFER_SIZE];
		char			mBufferCreator[MAX_BUFFER_SIZE];
		char			mBufferLogin[MAX_BUFFER_SIZE];
		int				mnConfirmations;

		void			GameObjectDetails();
		void			GameObjectComponents();
		void			AddComponentButton(const Math::Vec2&);
		void			ComponentsDropDownList();
		void			AddBehaviourButton(const Math::Vec2&);
		void			BehaviourDropDownList();
		void			PromptCreateBehaviour();
		void			ConfirmCreateBehaviour();
		void			ResetBehaviourCreation();
		bool			RemoveComponent(Dystopia::Component*);
		bool			RemoveComponent(Dystopia::Behaviour*);

		void			AddTagButton(const Math::Vec2&);
		void			TagsDropDownList();
		void			PrefabObject(void);
		void			Confirmations(void*);
	};
}



#endif // _INSPECTOR_H_

#endif // EDITOR 


