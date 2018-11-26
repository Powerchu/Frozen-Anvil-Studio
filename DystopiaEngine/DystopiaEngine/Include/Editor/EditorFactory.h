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
#include "DataStructure/MagicArray.h"
#include "Editor/EditorSystem.h"
#include "Math/Vector4.h"

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

		void DefaultSceneCamera(void);

		void ReattachToPrefab(Dystopia::Component* _p, uint64_t c = 0);
		bool SpawnPrefab(const HashString&, const Math::Pt3D&, uint64_t& _outRootObjID);
		bool SaveAsPrefab(const uint64_t& _objID, Dystopia::TextSerialiser&, bool _temp = false);
		bool SaveAsPrefabTemp(const uint64_t& _objID, Dystopia::TextSerialiser&);
		bool DettachPrefab(const uint64_t&);
		void ValidatePrefabInstances(void);

		bool LoadAsPrefab(const HashString& _path);
		void LoadIntoScene(Dystopia::TextSerialiser&);

	private:
		struct PrefabData
		{
			PrefabData(const HashString&, size_t, size_t);

			HashString mPrefabFile;
			size_t mnStart;
			size_t mnEnd;

			AutoArray<AutoArray<uint64_t>> mArrInstanced;
		};

		void SaveChild(Dystopia::GameObject&, Dystopia::TextSerialiser&, bool _temp = false);
		void LoadChild(Dystopia::GameObject&, Dystopia::TextSerialiser&);
		bool SavePrefab(const uint64_t& _objID, Dystopia::TextSerialiser&, bool _temp = false);
		bool LoadPrefab(Dystopia::GameObject&, Dystopia::TextSerialiser&);

		void SaveSegment(Dystopia::GameObject&, Dystopia::TextSerialiser&);
		void SaveSegment(Dystopia::GameObject&, const AutoArray<Dystopia::Component*>&, Dystopia::TextSerialiser&);
		void SaveSegment(const AutoArray<Dystopia::Behaviour*>&, Dystopia::TextSerialiser&);
		
		unsigned LoadSegment(Dystopia::GameObject&, Dystopia::TextSerialiser&);
		void LoadSegmentC(Dystopia::GameObject&, unsigned, Dystopia::TextSerialiser&);
		void LoadSegmentB(Dystopia::GameObject&, Dystopia::TextSerialiser&);

		void RecursiveCounter(Dystopia::GameObject&, unsigned&);

		uint64_t PutToScene(const HashString&, const Math::Pt3D&);
		uint64_t PutToScene(PrefabData&, const Math::Pt3D&);

		MagicArray<Dystopia::GameObject> mArrFactoryObj;
		MagicArray<PrefabData> mArrPrefabData;

		Dystopia::SceneSystem *mpSceneSys;
	};

}

#endif
#endif
















