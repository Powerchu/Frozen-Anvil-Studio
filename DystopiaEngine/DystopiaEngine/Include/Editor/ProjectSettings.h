/* HEADER *********************************************************************************/
/*!
\file	ProjectSettings.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _PROJ_SETTINGS_H_
#define _PROJ_SETTINGS_H_
#include "Editor/EditorPanel.h"

namespace Dystopia
{
	class TextSerialiser;
	class GraphicsSystem;
	class PhysicsSystem;
	class InputManager;
	class TagSystem;
	class CollisionSystem;
}

namespace Editor
{
	class ProjectSettings : public EditorPanel//EditorTab
	{
	public:
		//static ProjectSettings* GetInstance(void);
		ProjectSettings(void);
		~ProjectSettings(void);

		void Load(void);
		bool Init(void);
		void Update(float);
		void EditorUI(void);
		void Shutdown(void);
		void Message(eEMessage);
		void SaveSettings(Dystopia::TextSerialiser&) const;
		void LoadSettings(Dystopia::TextSerialiser&);
		HashString GetLabel(void) const;

	private:
		HashString		mLabel;

		Dystopia::GraphicsSystem  *mpGfxSys;
		Dystopia::PhysicsSystem	  *mpPhySys;
		Dystopia::InputManager    *mpInputSys;
		Dystopia::TagSystem       *mpTagSystem;
		Dystopia::CollisionSystem *mpColSystem;

	};

}

#endif //_PROJ_SETTINGS_H_
#endif //EDITOR














