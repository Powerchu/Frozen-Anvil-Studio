/* HEADER *********************************************************************************/
/*!
\file	Inspector.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR

#include "Editor\Inspector.h"
#include "Math\Vector4.h"
#include "Editor\EGUI.h"

namespace Dystopia
{
	Inspector::Inspector()
		: mWidth{ 300 }, mHeight{ 300 }, mpFocusGameObj{ nullptr }
	{}

	Inspector::~Inspector()
	{}

	void Inspector::Init()
	{
	}

	void Inspector::Update(const float _dt)
	{
		_dt;
	}

	void Inspector::Shutdown()
	{
	}

	void Inspector::SetWidth(float _width)
	{
		mWidth = _width;
	}

	void Inspector::SetHeight(float _height)
	{
		mHeight = _height;
	}

	void Inspector::Window()
	{
		if (!mpFocusGameObj) return;
	}
}

#endif // EDITOR 


