#pragma once
#include "Behaviour/Behaviour.h"


#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	struct TestBehaviour : Behaviour
	{
		virtual void Update(const float _fDeltaTime);

		virtual void Serialise(TextSerialiser&) const;
		virtual void Unserialise(TextSerialiser&);

		~TestBehaviour();
	};


	extern "C"
	{
		DllExport TestBehaviour * TestBehaviourClone()
		{
			return new TestBehaviour;
		}
	}
}
