/* HEADER *********************************************************************************/
/*!
\file	MeshSystem.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Manages the creation, loading and destruction of Meshes.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Graphics/MeshSystem.h"		// File Header
#include "System/Graphics/Mesh.h"
#include "System/Graphics/RawMesh.h"
#include "System/Graphics/VertexDefs.h"
#include "IO/TextSerialiser.h"
#include "Math/MathUtility.h"

#include <GL/glew.h>


void Dystopia::MeshSystem::Init(void)
{

}

void Dystopia::MeshSystem::Shutdown(void) noexcept
{
	FreeMeshes();
}

void Dystopia::MeshSystem::StartMesh(void)
{
	mpRawMeshes.EmplaceBack();
}

void Dystopia::MeshSystem::LoadMesh(std::string const& _strPath)
{
	RawMesh& CurrentMesh = mpRawMeshes.back();
	TextSerialiser input = Serialiser::OpenFile<TextSerialiser>(_strPath.c_str(), Serialiser::MODE_READ);

	unsigned short nVtxCount = 0;
	input >> nVtxCount;

	mUVBuffer.reserve(nVtxCount);
	mVtxBuffer.reserve(nVtxCount);
	mNormalBuffer.reserve(nVtxCount);
	mIndexBuffer.reserve(nVtxCount * 3);

	input.ConsumeStartBlock();

	Gfx::Vertex vtxBuf;
	Gfx::Vertex normBuf;
	Gfx::UV uvBuf;

	for (unsigned short n = 0; n < nVtxCount; ++n)
	{
		input >> vtxBuf;
		input >> normBuf;
		input >> uvBuf;

		mVtxBuffer.EmplaceBack(vtxBuf);
		mNormalBuffer.EmplaceBack(normBuf);
		mUVBuffer.EmplaceBack(uvBuf);
	}

	CurrentMesh.AppendVertexCount(nVtxCount);
	input.ConsumeStartBlock();

	do
	{
		unsigned nNumIndices = 0, nCurrOffset = static_cast<unsigned>(mIndexBuffer.size());
		while (!input.EndOfInput())
		{
			mIndexBuffer.EmplaceBack();
			input >> mIndexBuffer.back();

			++nNumIndices;
		}

		auto pCurrMesh = mpMeshes.Emplace(CurrentMesh.GetVAO(), nNumIndices, nCurrOffset);

		input.ConsumeStartBlock();

		CurrentMesh.IncRef();
		input >> const_cast<std::string&>(pCurrMesh->GetName());

		input.ConsumeStartBlock();
	} while (!input.EndOfInput());
}

void Dystopia::MeshSystem::AddVertex(float x, float y, float z)
{
	mVtxBuffer.EmplaceBack(x, y, z);
}

void Dystopia::MeshSystem::AddVertex(Gfx::Vertex const& _v)
{
	mVtxBuffer.EmplaceBack(_v);
}

void Dystopia::MeshSystem::AddNormal(float x, float y, float z)
{
	mNormalBuffer.EmplaceBack(x, y, z);
}

void Dystopia::MeshSystem::AddNormal(Gfx::Normal const& _v)
{
	mNormalBuffer.EmplaceBack(_v);
}

void Dystopia::MeshSystem::AddUV(float u, float v)
{
	mUVBuffer.EmplaceBack(u, v);
}

void Dystopia::MeshSystem::AddUV(Gfx::UV const & _v)
{
	mUVBuffer.EmplaceBack(_v);
}

Dystopia::Mesh* Dystopia::MeshSystem::AddIndices(const std::string& _strName, const AutoArray<short>& _indices)
{
	RawMesh& CurrentMesh = mpRawMeshes.back();
	size_t nCurrOffset = mIndexBuffer.size();
	CurrentMesh.IncRef();

	for (auto& e : _indices)
		mIndexBuffer.push_back(e);

	auto pCurrMesh = mpMeshes.Emplace(CurrentMesh.GetVAO(), static_cast<unsigned>(_indices.size()), nCurrOffset);
	pCurrMesh->SetName(_strName);

	return pCurrMesh;
}

void Dystopia::MeshSystem::EndMesh(void)
{
	mpRawMeshes.back().Build(mVtxBuffer, mNormalBuffer, mUVBuffer, mIndexBuffer);
	
	mVtxBuffer.clear();
	mNormalBuffer.clear();
	mUVBuffer.clear();
	mIndexBuffer.clear();
}

void Dystopia::MeshSystem::FreeMeshes(void)
{
	mpMeshes.clear();
	mpRawMeshes.clear();
}

void Dystopia::MeshSystem::ExportMeshes(void)
{

}

Dystopia::Mesh* Dystopia::MeshSystem::CreateMesh(RawMesh* _pRaw, unsigned _nIndices, size_t offset)
{
	return mpMeshes.Emplace(_pRaw->GetVAO(), _nIndices, offset);
}

Dystopia::Mesh* Dystopia::MeshSystem::GetMesh(const std::string& _strName) noexcept
{
	for (auto& e : mpMeshes)
		if (e.GetName() == _strName)
			return &e;

	return nullptr;
}

unsigned Dystopia::MeshSystem::GenerateRaw(void) noexcept
{
	auto ret = mpRawMeshes.size();
	mpRawMeshes.EmplaceBack();
	return static_cast<unsigned>(ret);
}

Dystopia::RawMesh* Dystopia::MeshSystem::GetRaw(unsigned _nIndex) noexcept
{
	return &mpRawMeshes[_nIndex];
}

Dystopia::RawMesh* Dystopia::MeshSystem::GetRawFromBuffer(unsigned _VAO) noexcept
{
	for (auto& e : mpRawMeshes)
		if (_VAO == e.GetVAO())
			return &e;

	return nullptr;
}


