#pragma once
#ifndef TEST_CLASS_H
#define TEST_CLASS_H

#define DllExport   __declspec( dllexport )

struct TestClass
{

	int col = 5;
};

extern "C"
{
	DllExport TestClass * Clone();
}

#endif