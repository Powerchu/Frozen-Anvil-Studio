#include "TestBehaviour.h"
#include <iostream>

namespace Dystopia
{
	void TestBehaviour::Update(const float _fDeltaTime)
	{
        for(int i =0; i<5; ++i)
		std::cout << "Behaviour HELLO WEI XIANG" << std::endl;
	}

}

