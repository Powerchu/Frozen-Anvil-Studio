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
			return Ut::MetaFind_t<Ut::Decay_t<Collider>, AllComponents>::value;
		};
		unsigned GetRealComponentType(void) const
		{
			return Ut::MetaFind_t<Ut::Decay_t<Circle>, UsableComponents>::value;
		};

#if EDITOR
		static const std::string GetCompileName(void) { return "Circle Collider 2D"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }
#endif

		static const eColliderType ColliderType = eColliderType::CIRCLE;
		virtual const eColliderType GetColliderType(void) const override { return ColliderType; }

		/*Constructors*/

		/*Default - (Box Collider)*/
		Circle();
		/*Constructor*/
		Circle(float const & _radius, Math::Vec3D const & _v3Offset = { 0,0,0,0 });
		
		void Awake(void);
		/*Load the Component*/
		virtual void Load(void);
		/*Initialise the Component*/
		virtual void Init(void);
		/* Update */
		virtual void Update(float) override;
		/*OnDestroy*/
		virtual void OnDestroy(void);
		/*Unload the Component*/
		virtual void Unload(void);
		/*Duplicate the Component*/
		virtual Circle* Duplicate() const;

		//virtual Math::Matrix3D   GetTransformationMatrix() const;
		virtual BroadPhaseCircle GenerateBoardPhaseCircle() const;
		float GetRadius() const;
		float GetRadiusRaw() const;
		void SetRadius(float _radius);

		/*Serialise and Unserialise*/
		virtual void Serialise(TextSerialiser&) const;
		virtual void Unserialise(TextSerialiser&);

		/*Collision Check Functions*/
		bool isColliding(Circle & other_col, Math::Vec3D other_pos);
		bool isColliding(Circle * const & other_col);

		/*Collision Check Functions*/
		bool isColliding(const AABB & other_col);
		bool isColliding(const AABB * const & other_col);

		/*Collision Check Functions*/
		bool isColliding(Convex& other_col);
		bool isColliding(Convex * const & other_col);

		bool isColliding(PointCollider & other_col);
		bool isColliding(PointCollider * const & other_col);

	private:
		float m_radius;

#if EDITOR
		void ePositionOffsetVectorFields();
		void eScaleField();

		// INFO
		void eAttachedBodyEmptyBox();
		void eNumberOfContactsLabel();
		void eIsTriggerCheckBox();
		
		// Editor UI
		void EditorUI(void) noexcept override;

#endif // EDITOR
	};
}
#endif //CIRCLE_H
