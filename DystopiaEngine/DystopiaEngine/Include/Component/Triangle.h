/* HEADER *********************************************************************************/
/*!
\file	Collider.h
\author Keith (70%)
Aaron (30%)
\par    email: keith.goh\@digipen.edu
email: m.chu\@digipen.edu
\brief
Collider2D for 2D Sprites.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Component/Convex.h"

namespace Dystopia
{
	class Triangle : public Convex
	{
	public:

		using SYSTEM = Collider::SYSTEM;

		static const eColliderType ColliderType = eColliderType::TRIANGLE;
		virtual const eColliderType GetColliderType(void) const { return ColliderType; }
		/*Constructors*/

		/*Default - (Box Collider)*/
		Triangle();
		/**/

		/*Load the Component*/
		virtual void Load(void);
		/*Initialise the Component*/
		virtual void Init(void);
		/*OnDestroy*/
		virtual void OnDestroy(void);
		/*Unload the Component*/
		virtual void Unload(void);
		/*Duplicate the Component*/
		virtual Triangle* Duplicate() const;

		/*Serialise and Unserialise*/
		virtual void Serialise(TextSerialiser&) const;
		virtual void Unserialise(TextSerialiser&);
	private:
	};
}

#endif //TRIANGLE_H