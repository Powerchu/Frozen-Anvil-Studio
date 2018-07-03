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

#include "System\Window\WindowManager.h"
#include "System\Graphics\GraphicsSystem.h"
#include "System\Graphics\MeshSystem.h"

#include "DataStructure\AutoArray.h"

#include <string>

#include "Math\Vector2.h"
#include "Math\Vector4.h"
#include "Math\Matrix4.h"
#include <iostream>
using namespace Math;
#define _ZERO_(_X_) (IsZero(_X_) ? .0f : _X_)
void PrintVector(Vec4 _v)
{
	std::cout << 
		_ZERO_(_v.x) << " " <<
		_ZERO_(_v.y) << " " <<
		_ZERO_(_v.z) << " " <<
		_ZERO_(_v.w) << std::endl;
}
void PrintMatrix(Mat4 _m)
{
	for (int n = 0; n < 4; ++n)
	{
		PrintVector(_m.GetRow(n));
	}
}

void PrintStuff(void)
{

	AutoArray<std::string> x;

	x.push_back("What");
	x.EmplaceBack("On Earth");

	for (int i = 0; i < 100; ++i)
		x.push_back("EXPLODE");
	x.clear();

	for (int n = 0; n < 200; ++n)
		x.EmplaceBack("DEATH");

	x[176] = "HOLY SHIT MAN";
	x.back() = "WILL I EXPLODE? NOBODY KNOWS!";

	std::cout << x[176] << std::endl;
	std::cout << x.back() << std::endl;
	x.back() = "OH THE INTENSITY, OH THE TENSION, OH THE SUSPENSE, OH THE RAW WILDNESS";
	x = x;

	AutoArray<std::string> y = x;
	std::cout << x.back() << std::endl;
	std::cout << y.back() << std::endl;
}

// Entry point for editor
int WinMain(HINSTANCE, HINSTANCE, char *, int)
{
	Dystopia::WindowManager *win = new Dystopia::WindowManager{};
	Dystopia::GraphicsSystem *gfx = new Dystopia::GraphicsSystem{};
	Dystopia::MeshSystem *mesh = new Dystopia::MeshSystem{};

	win->LoadDefaults();
	win->Init();

	gfx->InitOpenGL(win->GetMainWindow());
	gfx->Init();

	mesh->Init();

	win->DestroySplash();

	mesh->StartMesh();
	mesh->LoadMesh("Resource/Meshes/Quad.txt");
	mesh->EndMesh();

	while (true)
	{
		win->Update(1.f);
		gfx->Update(1.f);
	}

	gfx->Shutdown();
	win->Shutdown();

	delete win;
	delete gfx;

	return 0;
}



#endif		// EDITOR ONLY

