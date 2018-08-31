#pragma once
#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H
#include "System\Base\Systems.h"
#include "System\SystemTypes.h"

namespace Dystopia
{
	class PhysicsSystem : public Systems
	{
	public:
		PhysicsSystem();

		virtual void PreInit(void);
		virtual bool Init(void);
		virtual void PostInit(void);

		virtual void FixedUpdate(float);
		virtual void Update(float);
		virtual void Shutdown(void);

		virtual void LoadDefaults(void);
		virtual void LoadSettings(TextSerialiser&);


		virtual ~PhysicsSystem(void) = default;
	};
}





#endif
