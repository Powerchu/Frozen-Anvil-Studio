#pragma once
#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H

#include "DataStructure/AutoArray.h"
#include "System/Base/Systems.h"
#include "System/Base/ComponentDonor.h"
#include "System/Collision/BoundingColliderNode.h"
#include "System/Collision/CollisionLayers.h"
#include <map>
namespace Dystopia
{

	class Collider;
	class AABB;
	class Convex;
	class Circle;
	class PointCollider;
	class RayCollider;
	struct CollisionEvent;
	class CollisionSystem : public Systems, public ComponentDonor<Convex>,
											public ComponentDonor<Circle>, 
											public ComponentDonor<AABB>,
		                                    public ComponentDonor<PointCollider>,
											public ComponentDonor<RayCollider>
	{
	public:

		CollisionSystem();
		~CollisionSystem();


		virtual void PreInit(void) override;
		virtual bool Init(void) override;
		virtual void PostInit(void) override;

		virtual void FixedUpdate(float) override;
		virtual void Update(float) override;

		virtual void PostUpdate() override;

		virtual void Shutdown(void) override;

		virtual void LoadDefaults(void) override;
		virtual void LoadSettings(TextSerialiser&) override;
		virtual void SaveSettings(DysSerialiser_t&);

#if EDITOR
		void EditorUI(void);
		void RenderVerticalColName();
#endif

		bool AABBvsAABB(Collider  * const & _ColA,
			            Collider  * const & _ColB) const;

		bool ConvexVsConvex(Collider  * const & _ColA,
			                Collider  * const & _ColB) const;

		bool CircleVsCircle(Collider * const & _ColA,
							Collider * const & _Colb) const;

		bool CircleVsAABB(Collider * const & _ColA,
							Collider * const & _ColB) const;

		bool AABBvsCircle(Collider * const & _ColA,
						  Collider * const & _ColB) const;

		bool CircleVsConvex(Collider * const & _ColA,
							Collider * const & _ColB) const;

		bool ConvexVsCircle(Collider * const & _ColA,
							Collider * const & _ColB) const;

		bool PointVsPoint(Collider * const & _ColA,
			              Collider * const & _ColB) const;

		bool PointVsConvex(Collider * const & _ColA,
			               Collider * const & _ColB) const;


		bool ConvexVsPoint(Collider * const & _ColA,
			               Collider * const & _ColB) const;

		bool PointVsCircle(Collider * const & _ColA,
						   Collider * const & _ColB) const;


		bool CircleVsPoint(Collider * const & _ColA,
						   Collider * const & _ColB) const;

		bool PointVsAABB(Collider * const & _ColA,
			             Collider * const & _ColB) const;


		bool AABBVsPoint(Collider * const & _ColA,
			             Collider * const & _ColB) const;

		AutoArray<Collider *> GetAllColliders() const;

		_DLL_EXPORT bool RaycastFirstHit(Math::Vec3D const & _Dir, Math::Point3D const & _mPos,CollisionEvent * _Output, float _MaxLength = 0.f, eColLayer layer = eColLayer::LAYER_GLOBAL) const;
		_DLL_EXPORT bool RaycastAllHits (Math::Vec3D const & _Dir, Math::Point3D const & _mPos,AutoArray<CollisionEvent> & _Output, float _MaxLength = 0.f, eColLayer layer = eColLayer::LAYER_GLOBAL) const;

		void MapIgnoreLayer(eColLayer _layer, eColLayer _toIgnore);
		_DLL_EXPORT bool ToIgnore      (eColLayer _Layer1, eColLayer _Layer2) const;
		_DLL_EXPORT void SetIgnore	   (unsigned _Layer1, unsigned _Layer2, bool _toignore);

		static std::string const * GetColLayerNames();
		static unsigned            GetColLayerSize();
	private:
		using Map_t = std::map<eColLayer, eColLayer>;

		static Map_t       mIgnoreTable;
		static bool        mIgnoreBoolTable[32][32];
		static std::string arrColLayer[33];
	};
}

#endif