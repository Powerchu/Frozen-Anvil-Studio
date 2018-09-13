#include "TestClass.h"
#include <iostream>

DllExport TestClass * Clone()
{
	return new TestClass;
}

void TestClass::Test()
	{
		std::cout << "After v16 CHANGE            Hotloading" << std::endl;
	}  