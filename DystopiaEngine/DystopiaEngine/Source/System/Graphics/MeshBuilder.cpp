/* HEADER *********************************************************************************/
/*!
\file	MeshBuilder.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Proxy class that uses the builder pattern to create a mesh

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Graphics/MeshBuilder.h"
#include "System/Graphics/RawMesh.h"


Dystopia::MeshBuilder::MeshBuilder(Dystopia::RawMesh& _mesh) noexcept
	: mMesh{ _mesh }
{

}


