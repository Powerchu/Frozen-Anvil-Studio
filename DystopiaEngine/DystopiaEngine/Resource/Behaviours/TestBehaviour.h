#pragma once
#include "Behaviour/Behaviour.h"

#define DllExport   __declspec( dllexport )

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
