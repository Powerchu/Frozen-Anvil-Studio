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

#include <vector>
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
	}

	gfx->Shutdown();
	win->Shutdown();

	delete win;
	delete gfx;

	return 0;
}



#endif		// EDITOR ONLY

