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

	while (true)
	{
		driver->Update();
	}

	driver->Shutdown();

	return 0;
}



#endif		// EDITOR ONLY

