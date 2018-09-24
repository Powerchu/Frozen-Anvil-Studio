#include "Component/CollisionEvent.h"

Dystopia::CollisionEvent::CollisionEvent(GameObject * _Target)
	: mdPeneDepth(0.0F)
	, mCollidedWith{ _Target }
	, mfRestitution(0.0F)
	, mfFrictionCof(0.5F)
{

}
