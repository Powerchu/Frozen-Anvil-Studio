#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H
#include "System/Base/Systems.h"
#include "System/Base/ComponentDonor.h"

namespace Dystopia
{
	class RigidBody;

	enum InterpolationMode
	{
		none = 0,
		interpolate,
		extrapolate,

		total
	};

	class PhysicsSystem : public Systems, public ComponentDonor<RigidBody>
	{
	public:
		PhysicsSystem();

		void PreInit(void) override;
		bool Init(void) override;
		void PostInit(void) override;

		void PreFixedUpdate(float) override;
		void FixedUpdate(float) override;
		void Update(float) override;
		void PostUpdate() override;

		void Shutdown(void) override;

		void LoadDefaults(void) override;
		void LoadSettings(TextSerialiser&) override;

		void EditorUI(void);

	private:
		void CheckSleepingBodies(float _dt);
		void IntegrateRigidBodies(float _dt);
		void ResolveCollision(float);
		void UpdateResults(float);
		void DebugPrint();

#if EDITOR
		void GravityUI(void);
		void IsDebugUI(void);
		void ResolutionUI(void);
#endif 

		bool  mbIsDebugActive;
		
		InterpolationMode mInterpolation_mode;

		virtual ~PhysicsSystem(void) = default;

	public:
		float mGravity;
		float mMaxVelocityConstant;
		float mMaxVelSquared;

		// Position Correction Tolerance
		float mPenetrationEpsilon;
		// Position Correction Resolution as Percentage;
		int mResolutionIterations;
	};
}

#endif
