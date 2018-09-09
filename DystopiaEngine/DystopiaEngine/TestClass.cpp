#include "TestClass.h"

DllExport TestClass * Clone()
{
	return new TestClass;
}

void TestClass::Test()
	{
		std::cout << "After Hotloading" << std::endl;
	}