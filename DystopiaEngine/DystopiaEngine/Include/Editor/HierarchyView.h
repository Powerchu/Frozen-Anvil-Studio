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
//#include "EditorTab.h"
#include "Editor/EditorPanel.h"
#include "DataStructure/AutoArray.h"

namespace Dystopia
{
	class GameObject;
	class Scene;
}

namespace Editor//Dystopia
{
	class HierarchyView : public EditorPanel//EditorTab
	{
	public:
		//static HierarchyView* GetInstance();

		HierarchyView(void);
		~HierarchyView(void);

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

		char						mSearchText[MAX_SEARCH];
		char						mSearchTextPrevFrame[MAX_SEARCH];
		HashString					mLabel;
		HashString					mPopupID;
		AutoArray<uint64_t>			mArrSearchID;

		void			SearchBar();
		void			UpdateSearch();
		void			CreateButton();
		void			CreatePopup();
		void			GameObjectPopups(Dystopia::GameObject&);
		void			SelectedObj(Dystopia::GameObject&);

		void			ShowGameObjects(void);
		void			ShowAsParent(Dystopia::GameObject&, const AutoArray<uint64_t>&);
		void			ShowAsChild(Dystopia::GameObject&, const AutoArray<uint64_t>&);
	};
}

#endif // _HEIRARCHY_H_
#endif // EDITOR



