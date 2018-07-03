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
		virtual void Init() override;
		virtual void Update(const float&) override;
		virtual void Window() override;
		virtual void Shutdown() override;
		virtual std::string GetLabel() const override;

		void SetFocusObj(GameObject*);
		void RemoveFocus();

	private:
		std::string mLabel;
		GameObject *mpFocusGameObj;
	};
}



#endif // _INSPECTOR_H_

#endif // EDITOR 


