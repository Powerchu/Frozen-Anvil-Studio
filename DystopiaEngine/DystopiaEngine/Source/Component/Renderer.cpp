/* HEADER *********************************************************************************/
/*!
\file	Renderer.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Basic Renderer.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Component\Renderer.h"
#include "System\Graphics\Mesh.h"


Dystopia::Renderer::Renderer(void) noexcept
	: mnUnique { 0 }, mpMesh { nullptr }
{
}

Dystopia::Renderer* Dystopia::Renderer::Duplicate(void) const
{
	Renderer* pThis = const_cast<Renderer*>(this);
	++(pThis->mnUnique);

	return pThis;
}

void Dystopia::Renderer::Serialise(TextSerialiser &) const
{
}

void Dystopia::Renderer::Unserialise(TextSerialiser &)
{
}


