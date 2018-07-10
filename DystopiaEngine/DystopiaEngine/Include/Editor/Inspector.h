/* HEADER *********************************************************************************/
/*!
\file	Inspector.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
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

	class Inspector : public EditorTab
	{
	public:
		Inspector();
		~Inspector();

		/* Init() is called immediately after the creation of the object */
		virtual void Init() override;
		
		/* Update() is called before Window(), so alter most variables (frame based) here to be printed in Window() later */
		virtual void Update(const float&) override;

		/* Window() is where you do the EGUI/IMGUI functions. GUI variable changes will be recorded here */
		virtual void Window() override;

		/* Shutdown() is called right before deleting this object */
		virtual void Shutdown() override;

		/* GetLabel() returns the string to identify this class. EditorTab requires this to create a tab for you using the label */
		virtual std::string GetLabel() const override;

		void SetFocusObj(GameObject*);
		void RemoveFocus();

	private:
		Math::Vec4 mDemoVec;
		char mDemoText[32];
		std::string mLabel;
		GameObject *mpFocusGameObj;
	};
}



#endif // _INSPECTOR_H_

#endif // EDITOR 


