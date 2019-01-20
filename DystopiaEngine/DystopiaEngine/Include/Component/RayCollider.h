#pragma once
#ifndef _RAY_COLLIDER_H
#define _RAY_COLLIDER_H
#include "Component/Collider.h"

namespace Dystopia
{
	class _DLL_EXPORT RayCollider : public Collider
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
		static const std::string GetCompileName(void) { return "Ray Collider";   }
		const std::string GetEditorName(void) const   { return GetCompileName(); }
#endif

		static const eColliderType ColliderType = eColliderType::RAY;
		virtual const eColliderType GetColliderType(void) const override { return ColliderType; }

		/*Constructors*/
		RayCollider(void);
		RayCollider(Math::Point3D const & _Pos, Math::Vec3D const & _Dir, float const & _MaxLength = 0.f, Math::Vec3D const & _Offset = { 0,0,0 });

		/*Member Functions*/

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
		virtual RayCollider* Duplicate() const;

		//virtual Math::Matrix3D   GetTransformationMatrix() const;
		virtual BroadPhaseCircle GenerateBoardPhaseCircle() const;

		/*Serialise and Unserialise*/
		virtual void Serialise(TextSerialiser&) const;
		virtual void Unserialise(TextSerialiser&);

		/*Collision Check Functions*/
		bool isColliding(Circle & other_col);
		bool isColliding(Circle * const & other_col);

		/*Collision Check Functions*/
		bool isColliding(AABB & other_col);
		bool isColliding(AABB * const & other_col);

		/*Collision Check Functions*/
		bool isColliding(Convex& other_col);
		bool isColliding(Convex * const & other_col);

		/*Collision Check Functions*/
		bool isColliding(PointCollider& other_col);
		bool isColliding(PointCollider * const & other_col);

		/*Collision Check Functions*/
		bool isColliding(RayCollider & other_col);
		bool isColliding(RayCollider * const & other_col);

		bool isFinite() const;
		Math::Vec3D GetGlobalRay() const;

		CollisionEvent GetFirstHitEvent() const;
		Collider*      GetFirstHit()      const;

		static bool Raycast(Math::Vec3D const & _RayDir,Math::Point3D const & _Pos, Collider * _Collider, CollisionEvent * _OutputResult);
	private:
		Math::Vec3D mRayDir;
		float       mMaxLength;
		float       mTimeIntersect;

		static bool Raycast_Circle(Math::Vec3D const & _RayDir, Math::Point3D const & _Pos, Circle * _Collider, CollisionEvent * _OutputResult);
		static bool Raycast_Convex(Math::Vec3D const & _RayDir, Math::Point3D const & _Pos, Convex * _Collider, CollisionEvent * _OutputResult);
	};
}


#endif


