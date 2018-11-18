/* HEADER *********************************************************************************/
/*!
\file	RawMesh.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	"Raw" Mesh Data

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Graphics/RawMesh.h"	// File Header
#include "System/Graphics/VertexDefs.h"
#include "DataStructure/AutoArray.h"

#include <GL/glew.h>


Dystopia::RawMesh::RawMesh(void) noexcept
	: mnVtxCount{ 0 }, mnRefCount{ 0 }
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(4, &mVtxBuffer);
}

Dystopia::RawMesh::~RawMesh(void)
{
	BindMesh();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	UnbindMesh();

	glDeleteBuffers(4, &mVtxBuffer);
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

unsigned Dystopia::RawMesh::RequestDuplicate(unsigned _nVtxCount, void * _nOffset)
{
	return 0;
}

void Dystopia::RawMesh::Build(
	const AutoArray<Gfx::Vertex>& _pVtx, 
	const AutoArray<Gfx::Normal>& _pNml, 
	const AutoArray<Gfx::UV>& _pUVs, 
	const AutoArray<short>& _nIndices
) {
	BindMesh();

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	
	glBindBuffer(GL_ARRAY_BUFFER, mVtxBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Ut::RemoveRef_t<decltype(_pVtx)>::Val_t) * _pVtx.size(), _pVtx.begin(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // Vertices

	glBindBuffer(GL_ARRAY_BUFFER, mNmlBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Ut::RemoveRef_t<decltype(_pNml)>::Val_t) * _pNml.size(), _pNml.begin(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); // Normals

	glBindBuffer(GL_ARRAY_BUFFER, mUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Gfx::UV) * _pUVs.size(), _pUVs.begin(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Gfx::UV), 0);												// UV

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * _nIndices.size(), _nIndices.begin(), GL_STATIC_DRAW);
	
	UnbindMesh();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void Dystopia::RawMesh::IncRef(void)
{
	++mnRefCount;
}

void Dystopia::RawMesh::DecRef(void)
{
	--mnRefCount;
}

unsigned Dystopia::RawMesh::GetVAO(void)
{
	return mVAO;
}

unsigned Dystopia::RawMesh::GetVtxCount(void)
{
	return mnVtxCount;
}

unsigned Dystopia::RawMesh::GetRefCount(void)
{
	return mnRefCount;
}


