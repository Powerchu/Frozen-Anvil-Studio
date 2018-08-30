/* HEADER *********************************************************************************/
/*!
\file	Editor.cpp
\author Digipen (100%)
\par    email: digipen\@digipen.edu
\brief
	INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR

#include "System\Driver\Driver.h"
#include "System\Window\WindowManager.h"

#include "DataStructure\AutoArray.h"
#include "DataStructure\MagicArray.h"
#include "DataStructure\Array.h"

#include "DataStructure\Pointer.h"

#include <string>

#include "Math\Vector2.h"
#include "Math\Vector4.h"
#include "Math\Matrix4.h"
#include "Math\Angles.h"
#include <iostream>

#include "Utility/MetaAlgorithms.h"
#include "Utility/MetaDataStructures.h"

#define _ZERO_(_X_) (IsZero(_X_) ? .0f : _X_)
void PrintVector(Math::Vec4 _v)
{
	std::cout << 
		_ZERO_(_v.x) << " " <<
		_ZERO_(_v.y) << " " <<
		_ZERO_(_v.z) << " " <<
		_ZERO_(_v.w) << std::endl;
}
void PrintMatrix(Math::Mat4 _m)
{
	for (int n = 0; n < 4; ++n)
	{
		PrintVector(_m.GetRow(n));
	}
}

float Rand(int lim)
{
	return (rand() % lim) * 1.f;
}

// Entry point for editor
int WinMain(HINSTANCE, HINSTANCE, char *, int)
{
	auto driver = Dystopia::EngineCore::GetInstance();

	driver->LoadSettings();
	driver->Init();
//	mesh->StartMesh();
//	mesh->LoadMesh("Resource/Meshes/Quad.txt");
//	mesh->EndMesh();

	Ctor::MagicArrayBuilder<int>::SetBlockSize<2>::type x;

	Math::Vec4 sdasd;

	sdasd.Negate<Math::NegateFlag::XYZW>();

	Array<float, 128> y{ 7.1f, 8.f };
	Array<short, 128> z;

	auto arr1 = Ctor::MakeArray<float>(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f, 13.f, 14.f, 15.f, 16.f);
	auto arr2 = Ctor::MakeArray<double>(0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);

	arr2 = arr1;
	auto w{ y };
	y = y;
	y = z;

	int random = Math::Min(5, 16);

	Pointer<int> why{};

	x.Insert(random);
	x.Insert(6);
	auto ptr = x.Insert(5);
	x.Emplace(Math::Max(5, 16));

	for (auto& e : x)
		std::cout << e << " ";
	std::cout << std::endl;

	x.Remove(ptr);

	for (auto& e : x)
		std::cout << e << " ";
	std::cout << std::endl;

	while (true)
	{
		driver->Update();
	}

	driver->Shutdown();

	return 0;
}



#endif		// EDITOR ONLY

