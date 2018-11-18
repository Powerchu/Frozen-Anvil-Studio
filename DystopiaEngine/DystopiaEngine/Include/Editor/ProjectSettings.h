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

#include "Editor/EditorTab.h"

namespace Dystopia
{
	class TextSerialiser;
	class GraphicsSystem;
	class PhysicsSystem;
	class InputManager;

	class ProjectSettings : public EditorTab
	{
	public:
		static ProjectSettings* GetInstance(void);
		~ProjectSettings(void);

		void		Init(void);
		void		Update(const float&);
		void		EditorUI(void);
		void		Shutdown(void);
		std::string GetLabel(void) const;

		void		SaveSettings(TextSerialiser& _out) const;
		void		LoadSettings(TextSerialiser& _in);

	private:
		ProjectSettings(void);

		std::string		mLabel;

		GraphicsSystem	*mpGfxSys;
		PhysicsSystem	*mpPhySys;
		InputManager     *mpInputSys;


	};

}

#endif //_PROJ_SETTINGS_H_
#endif //EDITOR
