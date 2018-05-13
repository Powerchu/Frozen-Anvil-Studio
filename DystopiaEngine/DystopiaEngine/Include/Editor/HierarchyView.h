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

namespace Dystopia
{
	class GameObject;
	class Scene;

	class HierarchyView
	{
	public:
		HierarchyView();
		~HierarchyView();
		void Init();
		void Update(const float&);
		void Shutdown();

		void Window();
		void SetWidth(float);
		void SetHeight(float);

	private:
		float mWidth;
		float mHeight;
		GameObject *mpFocusGameObj;
		Scene *mpCurrentScene;
	};
}

#endif // _HEIRARCHY_H_
#endif // EDITOR



