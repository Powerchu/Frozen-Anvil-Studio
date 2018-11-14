/* HEADER *********************************************************************************/
/*!
\file	EditorFactory.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_FACTORY_H_
#define _EDITOR_FACTORY_H_
#include "DataStructure/HashString.h"
#include "DataStructure/AutoArray.h"
#include "Editor/EditorSystem.h"

namespace Dystopia
{
	class GameObject;
	class SceneSystem;
	class Component;
	class Behaviour;
}

namespace Editor
{
	class EditorFactory : public EditorSystem
	{
	public:
		EditorFactory(void);
		~EditorFactory(void);

		void Load(void);
		bool Init(void);
		void StartFrame(void);
		void Update(float);
		void EndFrame(void);
		void Shutdown(void);
		void Message(eEMessage);
		void SaveSettings(Dystopia::TextSerialiser&) const;
		void LoadSettings(Dystopia::TextSerialiser&);

		bool SaveAsPrefab(const uint64_t& _objID, Dystopia::TextSerialiser&);
		bool LoadFromPrefab(Dystopia::GameObject&, Dystopia::TextSerialiser&);

	private:
		bool SavePrefab(const uint64_t& _objID, Dystopia::TextSerialiser&);
		bool LoadPrefab(Dystopia::GameObject&, Dystopia::TextSerialiser&);

		void SaveSegment(Dystopia::GameObject&, Dystopia::TextSerialiser&);
		void SaveSegment(const AutoArray<Dystopia::Component*>&, Dystopia::TextSerialiser&);
		void SaveSegment(const AutoArray<Dystopia::Behaviour*>&, Dystopia::TextSerialiser&);
		
		unsigned LoadSegment(Dystopia::GameObject&, Dystopia::TextSerialiser&);
		void LoadSegmentC(Dystopia::GameObject&, unsigned, Dystopia::TextSerialiser&);
		void LoadSegmentB(Dystopia::GameObject&, Dystopia::TextSerialiser&);

		AutoArray<Dystopia::GameObject> mArrFactoryObj;

		Dystopia::SceneSystem *mpSceneSys;
	};

}

#endif
#endif
















