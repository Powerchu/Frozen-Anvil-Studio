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
#include "System\Graphics\MeshSystem.h"		// File Header
#include "System\Graphics\Mesh.h"
#include "System\Graphics\RawMesh.h"
#include "System\Graphics\VertexDefs.h"
#include "IO\TextSerialiser.h"
#include "Math\MathUtility.h"

#include <GL\glew.h>


void Dystopia::MeshSystem::Init(void)
{
	Mesh::LinkSystem(this);
//	mpMeshes.reserve(10);
}

void Dystopia::MeshSystem::Shutdown(void) noexcept
{
	//glDisableVertexAttribArray(2);
	//glDisableVertexAttribArray(1);
	//glDisableVertexAttribArray(0);

	FreeMeshes();
}

void Dystopia::MeshSystem::StartMesh(void)
{
	mpRawMeshes.EmplaceBack();
}

void Dystopia::MeshSystem::LoadMesh(const std::string& _strPath)
{
	RawMesh& CurrentMesh = mpRawMeshes.back();
	TextSerialiser input = Serialiser::OpenFile<TextSerialiser>(_strPath, Serialiser::MODE_READ);

	unsigned short nVtxCount = 0;
	input.Read(nVtxCount);

	mUVs.reserve(nVtxCount * 2);
	mVtx.reserve(nVtxCount * 6);
	mIndex.reserve(nVtxCount * 3);

	input.ConsumeStartBlock();

	Vertex vtxBuf;
	Vertex normBuf;
	UV uvBuf;

	for (unsigned short n = 0; n < nVtxCount; ++n)
	{
		input >> vtxBuf;
		input >> normBuf;
		input >> uvBuf;

		mVtx.EmplaceBack(vtxBuf);
		mVtx.EmplaceBack(normBuf);
		mUVs.EmplaceBack(uvBuf);
	}

	CurrentMesh.mVtxCount += nVtxCount;
	input.ConsumeStartBlock();

	do
	{
		unsigned nNumIndices = 0, nCurrOffset = static_cast<unsigned>(mIndex.size());
		while (!input.EndOfInput())
		{
			mIndex.EmplaceBack();
			input >> mIndex.back();

			++nNumIndices;
		}

		auto pCurrMesh = mpMeshes.Emplace(CurrentMesh.mVAO, nNumIndices, nCurrOffset);

		input.ConsumeStartBlock();

		input >> const_cast<std::string&>(pCurrMesh->GetName());

		input.ConsumeStartBlock();
	} while (!input.EndOfInput());
}

void Dystopia::MeshSystem::AddVertex(float x, float y, float z, float u, float v)
{
	mVtx.EmplaceBack(x, y, z);
	mUVs.EmplaceBack(u, v);
	++(mpRawMeshes.back().mVtxCount);
}

void Dystopia::MeshSystem::AddIndices(const std::string& _strName, const AutoArray<short>& _indices)
{
	RawMesh& CurrentMesh = mpRawMeshes.back();
	size_t nCurrOffset = mIndex.size();

	for (auto& e : _indices)
		mIndex.push_back(e);

	auto pCurrMesh = mpMeshes.Emplace(CurrentMesh.mVAO, static_cast<unsigned>(_indices.size()), nCurrOffset);
	pCurrMesh->SetName(_strName);
}

void Dystopia::MeshSystem::EndMesh(void)
{
	mpRawMeshes.back().BuildMesh(mVtx, mUVs, mIndex);
	
	mVtx.clear();
	mUVs.clear();
	mIndex.clear();
}

void Dystopia::MeshSystem::FreeMeshes(void)
{
	mpMeshes.clear();
	mpRawMeshes.clear();
}

void Dystopia::MeshSystem::ExportMeshes(void)
{

}

Dystopia::Mesh* Dystopia::MeshSystem::GetMesh(const std::string& _strName) noexcept
{
	for (auto& e : mpMeshes)
		if (e.GetName() == _strName)
			return &e;

	return nullptr;
}


