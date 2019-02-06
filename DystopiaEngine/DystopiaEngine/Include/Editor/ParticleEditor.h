/* HEADER *********************************************************************************/
/*!
\file	ParticleEditor.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _PARTICLE_EDITOR_H_
#define _PARTICLE_EDITOR_H_
#include "DataStructure/HashString.h"
#include "DataStructure/AutoArray.h"
#include "Math/Vectors.h"
#include "Editor/EditorPanel.h"

namespace Dystopia
{
	class GameObject;
}

namespace Editor
{
	class ParticleEditor : public EditorPanel
	{
	public:
		ParticleEditor(void);
		~ParticleEditor(void);

		void Load(void);
		bool Init(void);
		void Update(float);
		void EditorUI(void);
		void Shutdown(void);
		void Message(eEMessage);
		void SaveSettings(Dystopia::TextSerialiser& _out) const;
		void LoadSettings(Dystopia::TextSerialiser& _in);
		HashString GetLabel(void) const;

	private:

		HashString mLabel;
		uint64_t mnObjectID;

		Dystopia::GameObject *mpObject;
		AutoArray<int> mArrUsedAffectors;

		bool ValidateEmitter(void);
		void AddAffectors(void);
		void UpdateAffectorUsed(int);
		bool AffectorIsUsed(int);
		void ListOfAffectors(void);
		void AddAffector(int);
		void EmitterAffectors(void);
	};
}


#endif // _PARTICLE_EDITOR_H_
#endif // EDITOR









