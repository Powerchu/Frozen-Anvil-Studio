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
#include "System\Graphics\VertexDefs.h"
#include "DataStructure\AutoArray.h"

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

void Dystopia::RawMesh::BindMesh(void) const
{
	glBindVertexArray(mVAO);
}

void Dystopia::RawMesh::UnbindMesh(void) const
{
	glBindVertexArray(0);
}

void Dystopia::RawMesh::BuildMesh(AutoArray<Vertex>& _pVtx, AutoArray<UV>& _pUVs, AutoArray<short>& _nIndices)
{
	BindMesh();
//	int err;
	glBindBuffer(GL_ARRAY_BUFFER, mVtxBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * _pVtx.size(), &_pVtx[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);						                    // Vertices
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(sizeof(Vertex)));	// Normals

	glBindBuffer(GL_ARRAY_BUFFER, mUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(UV) * _pUVs.size(), &_pUVs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(UV), 0);												// UV

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * _nIndices.size(), &_nIndices[0], GL_STATIC_DRAW);

	UnbindMesh();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


