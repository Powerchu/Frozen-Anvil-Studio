/* HEADER *********************************************************************************/
/*!
\file	EditorResource.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_RESOURCE_H_
#define _EDITOR_RESOURCE_H_
#include "Editor/EditorSystem.h"
#include "DataStructure/HashString.h"
#include "DataStructure/AutoArray.h"

namespace Dystopia
{
	class Component;
}

namespace Editor
{
	class EditorResource : public EditorSystem
	{
	public:
		EditorResource(void);
		~EditorResource(void);

		void Load(void);
		bool Init(void);
		void StartFrame(void);
		void Update(float);
		void EndFrame(void);
		void Shutdown(void);
		void Message(eEMessage);
		void SaveSettings(Dystopia::TextSerialiser& _out) const;
		void LoadSettings(Dystopia::TextSerialiser& _in);

		void AddComponent(Dystopia::Component* _p, const uint64_t&, bool);
		void RemoveComponent(const uint64_t&);
		Dystopia::Component* GetComponent(const uint64_t&);

		HashString GetCurrProjectPath(void) const;

	private:

		HashString mCurrProjectPath;
		AutoArray<Dystopia::Component*> mArrComponentResource;

	};

}

#endif
#endif











