#include "TestBehaviour.h"
#include "Component/Transform.h"
#include <iostream>

namespace Dystopia
{
	void TestBehaviour::Update(const float _fDeltaTime)
	{
        for(int i =0; i<10; ++i)
			std::cout << "HDHFDHFDHFHD" << i << std::endl;
		Transform * pt = new Transform;
	}
    
    void TestBehaviour::Serialise(TextSerialiser&) const
    {
        
    }
    
	void TestBehaviour::Unserialise(TextSerialiser&)
    {
        
    }

	TestBehaviour::~TestBehaviour()
    {
        
    }

}

