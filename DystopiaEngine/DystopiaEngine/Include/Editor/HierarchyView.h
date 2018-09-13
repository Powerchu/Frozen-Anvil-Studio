/* HEADER *********************************************************************************/
/*!
\file	HierarchyView.h
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
#ifndef _HIERARCHY_H_
#define _HIERARCHY_H_
#include "EditorTab.h"
#include "DataStructure\AutoArray.h"

namespace Dystopia
{
	class GameObject;
	class Scene;
	class HierarchyView : public EditorTab
	{
	public:
		static HierarchyView* GetInstance();
		~HierarchyView();

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

		virtual void SetFocus(GameObject&) override final;

		virtual void RemoveFocus() override final;

	private:
		HierarchyView(void);

		GameObject					*mpFocus;
		char						mSearchText[MAX_SEARCH];
		char						mSearchTextPrevFrame[MAX_SEARCH];
		std::string					mLabel;
		std::string					mPopupID;
		AutoArray<unsigned long>	mArrSearchID;

		void			SearchBar();
		void			UpdateSearch();
		void			CreateButton();
		void			CreatePopup();
		void			GameObjectName(GameObject&);
	};
}

#endif // _HEIRARCHY_H_
#endif // EDITOR



