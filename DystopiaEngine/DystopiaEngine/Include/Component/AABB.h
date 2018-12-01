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
			return Ut::MetaFind_t<Ut::Decay_t<Collider>, AllComponents>::value;
		};
		unsigned GetRealComponentType(void) const
		{
			return Ut::MetaFind_t<Ut::Decay_t<AABB>, UsableComponents>::value;
		};

#if EDITOR
		static const std::string GetCompileName(void) { return "Box Collider 2D"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }
#endif

		static const eColliderType ColliderType = eColliderType::AABB;
		virtual const eColliderType GetColliderType(void) const { return ColliderType; }
		
		/*Constructors*/

		/*Default - (Box Collider)*/
		AABB();
		/*Constructor*/
		AABB(float const & _width, float const & _height, Math::Vec3D const & _v3Offset = { 0,0,0,0 });

		void Awake() override;
		/*Initialise the Component*/
		void Init(void) override;
		/*Update Frame*/
		void Update(float) override;

		void EditorUI() noexcept override final ;

		/*OnDestroy*/
		void OnDestroy(void) override;

		/*Duplicate the Component*/
		AABB* Duplicate() const override;

		/*Serialise and Unserialise*/
		void Serialise(TextSerialiser&) const override;
		void Unserialise(TextSerialiser& _in) override;

		void Recompute();

		/*Collision Check Functions*/

		/* AABB - AABB Collision*/
		bool isColliding(AABB & _ColB);
		bool isColliding(AABB * const & _ColB);

		/* AABB - Point Collision*/
		bool isColliding(const Math::Pt3D& _point);
		bool isColliding(Math::Pt3D * const & _point);
		bool isColliding(const Math::Mat3D& _ownerTrans, const Math::Mat3D& _transMat,
						 const Math::Vec3D& _offset, const Math::Pt3D _point);


		/* Gettors */
		float GetWidth() const;
		float GetHeight() const;
		float GetHalfWidth() const;
		float GetHalfHeight() const;
		void eIsTriggerCheckBox();
		void ePositionOffsetVectorFields();

	private:
		float mfWidth;
		float mfHeight;

		Vertice * mMin;
		Vertice * mMax;
	};

}

#endif //AABB_H
