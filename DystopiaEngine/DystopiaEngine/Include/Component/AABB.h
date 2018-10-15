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
#ifndef AABB_H
#define AABB_H

#include "Component/Convex.h"

namespace Dystopia
{
	class _DLL_EXPORT AABB : public Convex
	{
	public:

		using SYSTEM = CollisionSystem;

		unsigned GetComponentType(void) const
		{
			return Utility::MetaFind_t<Utility::Decay_t<Collider>, AllComponents>::value;
		};

#if EDITOR
		static const std::string GetCompileName(void) { return "Box Collider2D"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }
#endif

		static const eColliderType ColliderType = eColliderType::AABB;
		virtual const eColliderType GetColliderType(void) const { return ColliderType; }
		
		/*Constructors*/

		/*Default - (Box Collider)*/
		AABB();
		/*Constructor*/
		AABB(float const & _width, float const & _height, Math::Vec3D const & _v3Offset = { 0,0,0,0 });

		/*Load the Component*/
		virtual void Load(void);
		/*Initialise the Component*/
		virtual void Init(void);
		/*OnDestroy*/
		virtual void OnDestroy(void);
		/*Unload the Component*/
		virtual void Unload(void);
		/*Duplicate the Component*/
		virtual AABB* Duplicate() const;

		/*Serialise and Unserialise*/
		void Serialise(TextSerialiser&) const override;
		void Unserialise(TextSerialiser& override);

		/*Collision Check Functions*/
		bool isColliding(AABB & other_col);
		bool isColliding(AABB * const & other_col);

		/* Gettors */
		float GetWidth() const;
		float GetHeight() const;
		float GetHalfWidth() const;
		float GetHalfHeight() const;

	private:
		float mfWidth;
		float mfHeight;

		Vertice * mMin;
		Vertice * mMax;
	};

}

#endif //AABB_H
