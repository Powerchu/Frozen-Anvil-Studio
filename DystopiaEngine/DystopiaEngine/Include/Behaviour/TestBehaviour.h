#pragma once
#include "Behaviour/Behaviour.h"

namespace Dystopia
{
	struct TestBehaviour : Behaviour
	{
		virtual void Update(const float _fDeltaTime);

		virtual void Serialise(TextSerialiser&) const
		{

		}
		virtual void Unserialise(TextSerialiser&)
		{

		}
	};


	extern "C"
	{
		DllExport TestBehaviour * Clone()
		{
			return new TestBehaviour;
		}
	}
}
