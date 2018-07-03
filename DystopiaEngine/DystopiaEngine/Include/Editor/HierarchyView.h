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

namespace Dystopia
{
	class GameObject;
	class Scene;

	class HierarchyView : public EditorTab
	{
	public:
		HierarchyView();
		~HierarchyView();
		virtual void Init() override;
		virtual void Update(const float&) override;
		virtual void Window() override;
		virtual void Shutdown() override;
		virtual std::string GetLabel() const override;

	private:
		std::string mLabel;
		GameObject *mpFocusGameObj;
		Scene *mpCurrentScene;
	};
}

#endif // _HEIRARCHY_H_
#endif // EDITOR



