#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H
#include "System/Base/Systems.h"
#include "System/Base/ComponentDonor.h"
#include "System/SystemTypes.h"
#include "DataStructure/MagicArray.h"


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

		void FixedUpdate(float) override;
		void Update(float) override;
		void Shutdown(void) override;

		void LoadDefaults(void) override;
		void LoadSettings(TextSerialiser&) override;

		void ResolveCollision(float);

		virtual ~PhysicsSystem(void) = default;
	private:
	};
}

#endif
