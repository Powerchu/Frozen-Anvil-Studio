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
#include "System/Graphics/RawMesh.h"	// File Header
#include "System/Graphics/VertexDefs.h"
#include "System/Graphics/MeshSystem.h"
#include "System/Driver/Driver.h"

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


void Dystopia::RawMesh::IncRef(void)
{
	++mnRefCount;
}

void Dystopia::RawMesh::DecRef(void)
{
	--mnRefCount;
}

unsigned Dystopia::RawMesh::GetVAO(void) const
{
	return mVAO;
}

unsigned Dystopia::RawMesh::GetVtxCount(void) const
{
	return mnVtxCount;
}

unsigned Dystopia::RawMesh::GetRefCount(void) const
{
	return mnRefCount;
}

void Dystopia::RawMesh::AppendVertexCount(unsigned _nVtxs)
{
	mnVtxCount += _nVtxs;
}


Dystopia::RawMesh* Dystopia::RawMesh::RequestDuplicate(unsigned _nVtxCount, size_t _nOffset, unsigned _nSkip) const
{
	auto pMeshSys = EngineCore::GetInstance()->Get<MeshSystem>();
	auto pRet = pMeshSys->GetRaw(pMeshSys->GenerateRaw());

	pRet->BindMesh();

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, pRet->mVtxBuffer);
	glBufferData(GL_ARRAY_BUFFER, _nVtxCount * sizeof(VertexBuffer::type), nullptr, GL_STATIC_DRAW);
	if (_nSkip != VertexBuffer::value)
	{
		glBindBuffer(GL_COPY_READ_BUFFER, mVtxBuffer);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, _nOffset, 0, _nVtxCount * sizeof(VertexBuffer::type));
	}
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // Vertices

	glBindBuffer(GL_ARRAY_BUFFER, pRet->mNmlBuffer);
	glBufferData(GL_ARRAY_BUFFER, _nVtxCount * sizeof(NormalBuffer::type), nullptr, GL_STATIC_DRAW);
	if (_nSkip != NormalBuffer::value)
	{
		glBindBuffer(GL_COPY_READ_BUFFER, mNmlBuffer);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, _nOffset, 0, _nVtxCount * sizeof(NormalBuffer::type));
	}
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); // Normals

	glBindBuffer(GL_ARRAY_BUFFER, pRet->mUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, _nVtxCount * sizeof(UVBuffer::type), nullptr, GL_STATIC_DRAW);
	if (_nSkip != UVBuffer::value)
	{
		glBindBuffer(GL_COPY_READ_BUFFER, mUVBuffer);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, _nOffset, 0, _nVtxCount * sizeof(UVBuffer::type));
	}
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0); // UV

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pRet->mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _nVtxCount * sizeof(IndexBuffer::type), nullptr, GL_STATIC_DRAW);
	if (_nSkip != IndexBuffer::value)
	{
		glBindBuffer(GL_COPY_READ_BUFFER, mEBO);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ELEMENT_ARRAY_BUFFER, _nOffset, 0, _nVtxCount * sizeof(IndexBuffer::type));
	}

	pRet->UnbindMesh();
	glBindBuffer(GL_COPY_READ_BUFFER, 0);
	return pRet;
}

void Dystopia::RawMesh::Build(
	const AutoArray<VertexBuffer::type>& _pVtx,
	const AutoArray<NormalBuffer::type>& _pNml,
	const AutoArray<UVBuffer::type    >& _pUVs,
	const AutoArray<IndexBuffer::type >& _nIndices
) {
	BindMesh();

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, mVtxBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexBuffer::type) * _pVtx.size(), _pVtx.begin(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // Vertices

	glBindBuffer(GL_ARRAY_BUFFER, mNmlBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(NormalBuffer::type) * _pNml.size(), _pNml.begin(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); // Normals

	glBindBuffer(GL_ARRAY_BUFFER, mUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(UVBuffer::type) * _pUVs.size(), _pUVs.begin(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0); // UV

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(IndexBuffer::type) * _nIndices.size(), _nIndices.begin(), GL_STATIC_DRAW);

	UnbindMesh();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void Dystopia::RawMesh::UpdateBuffer(unsigned _target, unsigned _nBuffer, void* _pData, ptrdiff_t _sz, bool _bFreq)
{
	BindMesh();

	glBindBuffer(_target, _nBuffer);
	glBufferData(_target, _sz, _pData, _bFreq ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

	UnbindMesh();
}

void Dystopia::RawMesh::UpdateBufferRange(unsigned _target, unsigned _nBuffer, void* _pData, ptrdiff_t _sz, ptrdiff_t _offset)
{
	BindMesh();
	
	glBindBuffer(_target, _nBuffer);
	glBufferSubData(_target, _offset, _sz, _pData);

	UnbindMesh();
}

void Dystopia::RawMesh::InitBuffersEmpty(bool _bVtx, bool _bNml, bool _bUV, bool _bIndex)
{
	BindMesh();

	if (_bVtx)
	{
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, mVtxBuffer);
		glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // Vertices
	}
	if (_bNml)
	{
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, mNmlBuffer);
		glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); // Normals
	}
	if (_bUV)
	{
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, mUVBuffer);
		glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0); // UV
	}
	if (_bIndex)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
	}

	UnbindMesh();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


