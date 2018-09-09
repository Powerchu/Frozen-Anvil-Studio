#pragma once
#ifndef TEST_CLASS_H
#define TEST_CLASS_H

#define DllExport   __declspec( dllexport )

struct TestClass
{
	void Test();
	int col = 100;
};

extern "C"
{
	DllExport TestClass * Clone();
}

#endif