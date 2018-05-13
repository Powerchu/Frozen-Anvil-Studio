/* HEADER *********************************************************************************/
/*!
\file	ResourceView.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor\ResourceView.h"
#include <string>

constexpr float DEFAULT_WIDTH = 300;
constexpr float DEFAULT_HEIGHT = 300;

namespace Dystopia
{
	ResourceView::ResourceView()
		: mWidth{ DEFAULT_WIDTH }, mHeight{ DEFAULT_HEIGHT }, mpFocusData{ nullptr }
	{
	}
	
	ResourceView::~ResourceView()
	{
	}
	
	void ResourceView::Init()
	{
	}

	void ResourceView::Update(const float& _dt)
	{
		_dt;
	}

	void ResourceView::Shutdown()
	{
	}

	void ResourceView::SetWidth(float _width)
	{
		mWidth = _width;
	}

	void ResourceView::SetHeight(float _height)
	{
		mHeight = _height;
	}

	void ResourceView::Window()
	{
	}

}




#endif // EDITOR





