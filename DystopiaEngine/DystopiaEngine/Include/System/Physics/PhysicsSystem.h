#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H
#include "System/Base/Systems.h"
#include "System/SystemTypes.h"
#include "DataStructure/AutoArray.h"


namespace Dystopia
{
	class RigidBody;

	class PhysicsSystem : public Systems
	{
	public:
		PhysicsSystem();

		void PreInit(void) override;
		bool Init(void) override;
		void PostInit(void) override;

		void FixedUpdate(float) override;
		void Update(float) override;
		void Shutdown(void) override;

		void Insert(RigidBody * const rigid_body);
		void Remove(RigidBody * const rigid_body);

		void LoadDefaults(void) override;
		void LoadSettings(TextSerialiser&) override;

		virtual ~PhysicsSystem(void) = default;
	private:
		AutoArray<RigidBody*> m_rigidBodies_arr;
	};
}

#endif
