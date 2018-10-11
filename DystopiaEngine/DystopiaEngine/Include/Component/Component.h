/* HEADER *********************************************************************************/
/*!
\file	Component.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
Base class for all components

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "Globals.h"
#include "Object/ObjectFlags.h"
#include "Component/ComponentList.h"	// eComponents

#include <string>


namespace Dystopia
{
	class GameObject;
	class TextSerialiser;

	class _DLL_EXPORT Component
	{
	public:
		using TAG = ComponentTag;
		using SYSTEM = class NULL_SYSTEM;
		virtual unsigned GetComponentType(void) const { return unsigned(-1); };
		virtual const std::string GetEditorName(void) const { return "Generic Component"; }

		// ====================================== CONSTRUCTORS ======================================= // 

		Component(void) noexcept;
		explicit Component(GameObject* _pOwner) noexcept;
		Component(const Component&) noexcept;
		virtual ~Component(void);


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		bool IsActive(void) const;
		void SetActive(const bool _bEnable);

		virtual void Awake(void);
		virtual void Load(void);
		virtual void Init(void);

		virtual void GameObjectDestroy(void);
		virtual void Unload(void);

		void DestroyComponent(void);

		virtual void SetOwner(GameObject*);
		GameObject* GetOwner(void) const;

		virtual Component* Duplicate() const;

		virtual void Serialise(TextSerialiser&) const = 0;
		virtual void Unserialise(TextSerialiser&) = 0;
		virtual void EditorUI(void) noexcept;

		uint64_t GetID(void) const;
		uint64_t GetOwnerID(void) const;
		unsigned GetFlags(void) const;
		void SetFlags(eObjFlag);
		void RemoveFlags(eObjFlag _flags);

	protected:
		uint64_t mID;
		unsigned mnFlags;
		uint64_t mnOwner;

	private:
	};
}



#endif		// INCLUDE GUARD

