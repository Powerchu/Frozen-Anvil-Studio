#include "Behaviour/TestClass.h"
#include <iostream>

DllExport TestClass * Clone()
{
	return new TestClass;
}

void TestClass::Test()
	{
		std::cout << "After     hshdshdshdh      Hotloading" << std::endl;
	}