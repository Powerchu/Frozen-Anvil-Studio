#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H
#include "System/Base/Systems.h"
#include "System/Base/ComponentDonor.h"
#include "System/SystemTypes.h"
#include "DataStructure/MagicArray.h"
#include "Math/Vector4.h"


namespace Dystopia
{
	class RigidBody;

	class PhysicsSystem : public Systems, public ComponentDonor<RigidBody>
	{
	public:
		PhysicsSystem();

		void PreInit(void) override;
		bool Init(void) override;
		void PostInit(void) override;



		void Step(float _dt);
		void FixedUpdate(float) override;
		void Update(float) override;
		
		void Shutdown(void) override;

		void LoadDefaults(void) override;
		void LoadSettings(TextSerialiser&) override;


		virtual ~PhysicsSystem(void) = default;
	private:
		void IntegrateRigidBodies(float _dt);
		void ResolveCollision(float);
		void PostResults();
		void DebugPrint();
		void DebugDraw();

		bool  mbIsDebugActive;
		float mTimeAccumulator;

		//CollisionMap		CollisionMap;
		//CollisionEvents	Contacts;

	public:
		float mGravity;
		float mMaxVelocityConstant;
		float mMaxVelSquared;

		// Position Correction Tolerance
		float mPenetrationEpsilon;
		// Position Correction Resolution as Percentage;
		float mPenetrationResolutionPercentage;
	};
}

#endif
