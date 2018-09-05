#include "TestClass.h"

DllExport TestClass * Clone()
{
	return new TestClass;
}
