/* HEADER *********************************************************************************/
/*!
\file	EditorStates.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_STATE_H_
#define _EDITOR_STATE_H_
#include "Editor/EditorStatesTypes.h"
#include "Editor/EditorSystem.h"

namespace Editor
{
	class EditorStates : public EditorSystem
	{
	public:
		EditorStates(void);
		~EditorStates(void);
		void Load(void);
		bool Init(void);
		void StartFrame(void);
		void Update(float);
		void EndFrame(void);
		void Shutdown(void);
		void Message(eEMessage);
		void SaveSettings(Dystopia::TextSerialiser& _out) const;
		void LoadSettings(Dystopia::TextSerialiser& _in);

	private:
		eState mState;
	};

}
#endif
#endif


