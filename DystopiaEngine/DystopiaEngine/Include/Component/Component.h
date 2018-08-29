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

#include "Component\ComponentList.h"	// eComponents

namespace Dystopia
{
	class GameObject;
	class TextSerialiser;

	class Component
	{
	public:

		using TAG = ComponentTag;
		using SYSTEM = class NULL_SYSTEM;
		virtual unsigned GetComponentType(void) const {	return unsigned(-1); };


		// ====================================== CONSTRUCTORS ======================================= // 

		Component(void);
		virtual ~Component(void);


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		bool IsActive(void) const;
		void SetActive(const bool _bEnable);

		void Load(void);
		void Init(void);

		virtual void GameObjectDestroy(void);
		void Unload(void);

		void DestroyComponent(void);

		void SetOwner(GameObject*);
		GameObject* GetOwner(void) const;

		virtual Component* Duplicate() const;

		virtual void Serialise(TextSerialiser&) const = 0;
		virtual void Unserialise(TextSerialiser&) = 0;

	private:

		GameObject * mpOwner;

	protected:

		unsigned mnFlags;
	};
}



#endif		// INCLUDE GUARD

