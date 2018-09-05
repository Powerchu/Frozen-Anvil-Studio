/* HEADER *********************************************************************************/
/*!
\file	Mesh.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Mesh
	TODO: Mesh Runtime modification

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System\Graphics\Mesh.h"		// File Header
#include "System\Graphics\MeshSystem.h"

#include <GL\glew.h>

Dystopia::MeshSystem* Dystopia::Mesh::mpSystem;

Dystopia::Mesh::Mesh(void) noexcept
{

}

Dystopia::Mesh::Mesh(unsigned _VAO, unsigned _nVertices, size_t _nOffset) noexcept
	: mVAO{ _VAO }, mnVertices{ _nVertices }, mnOffset{ reinterpret_cast<void*>(_nOffset) }
{

}

void Dystopia::Mesh::LinkSystem(MeshSystem* _pSystem)
{
	mpSystem = _pSystem;
}

void Dystopia::Mesh::UseMesh(int _nMode) const
{
	glBindVertexArray(mVAO);
	glDrawElements(_nMode, mnVertices, GL_UNSIGNED_SHORT, mnOffset);
	glBindVertexArray(0);
}

void Dystopia::Mesh::UseMesh(int _nMode, unsigned _nCount) const
{
	glBindVertexArray(mVAO);
	glDrawElementsInstanced(_nMode, mnVertices, GL_UNSIGNED_SHORT, mnOffset, _nCount);
	glBindVertexArray(0);
}

void Dystopia::Mesh::SetName(const std::string& _strName)
{
	mName = _strName;
}

const std::string& Dystopia::Mesh::GetName(void) const
{
	return mName;
}

bool Dystopia::Mesh::IsUnique(void) const
{
	return mbUnique;
}


