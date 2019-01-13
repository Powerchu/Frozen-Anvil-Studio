#pragma once
#ifndef _POINT_COLLIDER_H
#include "Component/Collider.h"
namespace Dystopia
{
	class _DLL_EXPORT PointCollider : public Collider
	{
	public:
		using SYSTEM = CollisionSystem;

		unsigned GetComponentType(void) const
		{
			return Ut::MetaFind_t<Ut::Decay_t<Collider>, AllComponents>::value;
		};
		unsigned GetRealComponentType(void) const
		{
			return Ut::MetaFind_t<Ut::Decay_t<PointCollider>, UsableComponents>::value;
		};

#if EDITOR
		static const std::string GetCompileName(void) { return "Point Collider"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }
#endif

		static const eColliderType ColliderType = eColliderType::POINT;
		virtual const eColliderType GetColliderType(void) const override { return ColliderType; }


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
		virtual PointCollider* Duplicate() const;

		//virtual Math::Matrix3D   GetTransformationMatrix() const;
		virtual BroadPhaseCircle GenerateBoardPhaseCircle() const;

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

	private:
	};
}




#endif