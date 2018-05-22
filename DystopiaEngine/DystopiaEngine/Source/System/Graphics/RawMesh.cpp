/* HEADER *********************************************************************************/
/*!
\file	RawMesh.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	"Raw" Mesh Data

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System\Graphics\RawMesh.h"	// File Header

#include <GL\glew.h>


Dystopia::RawMesh::RawMesh(void) noexcept
	: mVtxCount{ 0 }
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(3, &mVtxBuffer);
}

Dystopia::RawMesh::~RawMesh(void)
{
	glDeleteBuffers(3, &mVtxBuffer);
	glDeleteVertexArrays(1, &mVAO);
}


