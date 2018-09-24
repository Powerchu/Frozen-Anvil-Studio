/* HEADER *********************************************************************************/
/*!
\file	Collider.h
\author Keith (70%)
Aaron (30%)
\par    email: keith.goh\@digipen.edu
email: m.chu\@digipen.edu
\brief
Collider2D for 2D Sprites.

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef CIRCLE_H
#define CIRCLE_H

#include "Component/Collider.h"

namespace Dystopia
{
	class _DLL_EXPORT Circle : public Collider
	{
	public:
		using SYSTEM = CollisionSystem;

		unsigned GetComponentType(void) const
		{
			return Utility::MetaFind_t<Utility::Decay_t<Collider>, AllComponents>::value;
		};

#if EDITOR
		static const std::string GetCompileName(void) { return "Circle"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }
#endif

		static const eColliderType ColliderType = eColliderType::CIRCLE;
		virtual const eColliderType GetColliderType(void) const override { return ColliderType; }

		/*Constructors*/

		/*Default - (Box Collider)*/
		Circle();
		/*Constructor*/
		Circle(float const & _radius, Math::Vec3D const & _v3Offset = { 0,0,0,0 });

		/*Load the Component*/
		virtual void Load(void);
		/*Initialise the Component*/
		virtual void Init(void);
		/*OnDestroy*/
		virtual void OnDestroy(void);
		/*Unload the Component*/
		virtual void Unload(void);
		/*Duplicate the Component*/
		virtual Circle* Duplicate() const;

		/*Serialise and Unserialise*/
		virtual void Serialise(TextSerialiser&) const;
		virtual void Unserialise(TextSerialiser&);

		/*Collision Check Functions*/
		bool isColliding(const Circle & other_col) const;
		bool isColliding(const Circle * const & other_col) const;

		/*Collision Check Functions*/
		bool isColliding(const AABB & other_col) const;
		bool isColliding(const AABB * const & other_col) const;

		/*Collision Check Functions*/
		bool isColliding(const Convex & other_col) const;
		bool isColliding(const Convex * const & other_col) const;

	private:
		float m_radius;
		Math::Vec3D m_originCentre; // GLOBAL COORDINATES
	};
}

#endif //CIRCLE_H